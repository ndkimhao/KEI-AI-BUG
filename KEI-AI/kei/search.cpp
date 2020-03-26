#include "stdafx.h"
#include "AI.h"

namespace ksh {

	// Hàm entry cho thuật tìm kiếm nước đi tiếp theo bằng thuật alpha-beta
	void AI::search_for_move()
	{
		const int player1 = 0, player2 = 1;
		int ab_res = 0;

		// Nếu thằng kia có đường thắng 100%
		if (opp_win_4) {
			win_moves[player2] = opp_win_4;
			win_moves_length[player2] = 10;
			ab_res = -1;
		}
		global_best_move = 0;

		// Thực hiện tấn công
		bool retflag;
		search_for_attacking_move(ab_res, retflag);
		if (retflag) return;
		// Thử đường thắng của đối thủ, nếu ta không có nước đi tốt
		search_for_opponent_move(ab_res);
		// Thử tìm nước đi phòng thủ
		search_for_defensive_move(ab_res);

		assert(cur_search_depth == 0);
	}

	// Thử tìm nước đi phòng thủ
	void AI::search_for_defensive_move(int &ab_res)
	{
		const int player1 = 0, player2 = 1;
		if (!finish_defending && (!finish_defending_x2 || wary_defend)
			&& ptr_win_moves_path > win_moves_path) {

			reached_depth = false;
			global_best_move = 0;
			ab_res = search_for_defensive_move_by_eval();
			// Nếu dùng eval không tìm được nước phòng thủ tốt
			// thì dùng alpha-beta để chạy tìm nước phòng thủ
			if (!global_best_move)
				ab_res = search_alpha_beta(1, 0, player1, 0);
			if (ab_res < 0 && !ai_terminate_reason)
				global_ab_res = ab_res;

			if (!reached_depth) {
				if (!wary_defend) finish_defending_x2 = true;
				else finish_defending = true;
			}

			// Nếu đường thủ quá tệ thì phải tìm kỹ hơn
			if (ab_res < 0) wary_defend = true;
			set_result_move(global_best_move);
		}
	}

	// Thử đường thắng của đối thủ, nếu ta không có nước đi tốt
	void AI::search_for_opponent_move(int &ab_res)
	{
		const int player1 = 0, player2 = 1;
		if (!finish_testing && ptr_win_moves_path == win_moves_path) {
			reached_depth = false;
			ab_res = 0;
			// Nếu [player2] có nước đi ngon, thử từ nước đó
			if (win_moves[player2]) {
				ab_res = search_alpha_beta(0, 1, player2, 1, win_moves[player2]);
				// Đường ko ngon :v
				if (ab_res <= 0) {
					if (win_moves_length[player2] <= global_search_depth) win_moves[player2] = 0;
					ptr_win_moves_path = win_moves_path;
				}
			}
			// [player2] không có đường tấn công ngon
			if (ab_res <= 0) {
				// Thử đi mạo hiểm hơn
				ab_res = search_alpha_beta(0, 1, player2, 1);
				if (ab_res > 0) {
					win_moves[player2] = global_best_move;
					win_moves_length[player2] = global_search_depth;
				}
				else {
					ptr_win_moves_path = win_moves_path;
				}
			}
			if (ab_res > 0) calc_eval_win_moves();
			if (!reached_depth) finish_testing = true;
		}
	}

	// Thực hiện tấn công
	void AI::search_for_attacking_move(int &ab_res, bool &retflag)
	{
		const int player1 = 0, player2 = 1;
		retflag = true;
		if (!finish_attacking && (!opp_win_4 || win_moves[player1])) {
			reached_depth = false;
			// Nếu [player1] đã có đường thắng
			if (win_moves[player1]) {
				ab_res = search_alpha_beta(wary_attack, 1, player1, 0, win_moves[player1]);
				// Thật ra đường thắng là đường thua
				// Hàm [dfs_win_moves] đã bỏ sót trường hợp
				// Hoặc do code NGU ra bug =]]
				if (ab_res <= 0 && win_moves_length[player1] <= global_search_depth)
					win_moves[player1] = 0;
			}

			// Chưa có nước tốt nhất thì gọi alpha-beta để tìm
			if (!global_best_move)
				ab_res = search_alpha_beta(wary_attack, 1, player1, 0);

			// Nếu tìm thấy đường có lợi cho mình
			if (ab_res > 0 && global_best_move) {
				set_result_move(global_best_move);
				win_moves[player1] = global_best_move;
				win_moves_length[player1] = global_search_depth;

				if (!ai_terminate_reason)
					global_ab_res = ab_res;
				if (!wary_attack) wary_attack = true;
				// Nếu gọi search_alpha_beta với wary_attack = true
				// => đã tìm ra đường thắng cmnr => ngon vãi c*t
				else ai_terminate_reason = 3;
				return;
			}

			if (!reached_depth) finish_attacking = true;
		}
		retflag = false;
	}

	// Thử các nước đi từ [win_moves_path] để phòng thủ, bằng phương pháp eval
	int AI::search_for_defensive_move_by_eval()
	{
		const int player1 = 0, player2 = 1;
		static int win_idx[WIN_MOVES_SIZE];

		// Vòng search hiện tại cũng tính là 1 depth
		cur_search_depth++;

		// Khởi tạo mảng win_idx[i] = i
		int cnt_win_path = int(ptr_win_moves_path - win_moves_path);
		assert(cnt_win_path > 0);
		for (int i = 0; i < cnt_win_path; i++) win_idx[i] = i;

		// Thử các nước đi từ [win_moves_path]
		int best_eval = -SHORT_INF, best_alpha_beta = -SHORT_INF;
		for (int i = 0; cnt_win_path > 0 && i < 21; i++) {
			// Chọn ô có [win_moves_path_eval] cao nhất
			int j_best_win_eval = 0, best_win_eval = -SHORT_INF;
			for (int j = cnt_win_path - 1; j >= 0; j--) {
				if (maxi(best_win_eval, win_moves_path_eval[win_idx[j]]))
					j_best_win_eval = j;
			}
			// Nếu eval quá thấp thì không xét nữa
			if (best_win_eval < 11) break;

			// Xóa ô ra khỏi danh sách, O(1) nhờ optimize [win_idx]
			int wi = win_idx[j_best_win_eval];
			PCell p = win_moves_path[wi];
			cnt_win_path--;
			win_idx[j_best_win_eval] = win_idx[cnt_win_path];
			assert(p >= board_beg && p < board_end && !p->piece);

			// Đi nước phòng thủ, thử nước tấn công của đối thủ
			p->piece = player1 + 1; eval_cell(p);
			int ab_res = -search_alpha_beta(wary_defend, 1, player2, 0);
			p->piece = 0; eval_cell(p);

			// Đánh giá tình trạng sau nước đi
			int cur_eval = best_win_eval + ab_res * 20;
			// Cập nhật eval tốt nhất
			if (maxi(best_eval, cur_eval)) {
				best_alpha_beta = ab_res;
				global_best_move = p;
				if (ab_res > 0 || ab_res == 0 && !win_moves[player1]) {
					// Lưu lại nước cầm chừng đối thủ có khi cần sau này
					hold_move = p;
					break;
				}
			}
			// Nếu nước đi cho ra đường thua
			else if (ab_res < 0) {
				win_moves_path_eval[wi] -= 133;
			}
		}

		// Nếu không có nước phòng thủ hiệu quả
		// Thì dùng tạm nước cầm chân đối thủ
		if (best_alpha_beta < 0 && hold_move)
			global_best_move = hold_move;

		// Gâu gâu
		cur_search_depth--;
		return best_alpha_beta;
	}
}