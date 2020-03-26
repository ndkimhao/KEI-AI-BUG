#include "stdafx.h"
#include "AI.h"

namespace ksh {

	// Kiểm tra xem đã chạy hết time limit chưa
	void AI::check_for_timeout()
	{
		static int cnt = 0;
		if (--cnt < 0) {
			cnt = 1000;
#pragma warning(suppress: 28159)
			if (GetTickCount() > stop_time) {
				ai_terminate_reason = 2;
			}
		}
	}

	// Hàm main để chạy alpha-beta search
	int AI::impl_search_alpha_beta_uncached(int player1, PCell *ptr_cur_moves, int log_win_moves,
		PCell start_from, PCell last_cell, int is_attacking)
	{
		// Chuẩn bị trước khi bắt đầu tìm
		int player2 = 1 - player1;
		check_for_timeout();
		cnt_searched_nodes++;

		// Kiểm tra đường thắng ngay của [player1]
		{
			bool retflag;
			int retval = impl_ab_check_win_p1(player1, log_win_moves, is_attacking, retflag);
			if (retflag) return retval;
		}
		// Kiểm tra đường thắng ngay của [player2]
		{
			bool retflag;
			int retval = impl_ab_check_win_p2(player2, player1, ptr_cur_moves, log_win_moves,
				last_cell, start_from, is_attacking, retflag);
			if (retflag) return retval;
		}

		// Biến/flags dùng điều khiển quá trình tìm kiếm
		PCell *ptr_cur_moves_old = ptr_cur_moves;
		int attacking_player = is_attacking ? player1 : player2;
		int must_defend = 0, must_attack = 0, can_attack = 0;

		// Thêm những ô có level 2 vào [cur_moves] để xét
		{
			bool retflag;
			int retval = impl_ab_add_level2_cells(player1, must_attack, log_win_moves,
				ptr_cur_moves, attacking_player, ptr_cur_moves_old, retflag);
			if (retflag) return retval;
		}

		// Tìm những nước phòng thủ
		PCell *ptr_moves_defend_x1 = ptr_cur_moves, p_defend = 0;
		impl_ab_find_defensive_moves(player2, must_defend, must_attack, p_defend,
			ptr_cur_moves, attacking_player, ptr_moves_defend_x1);

		PCell *ptr_moves_defend_x2 = ptr_cur_moves;
		if (cur_search_depth >= global_search_depth) {
			reached_depth = true;
			// Dừng tìm kiếm nếu không có threat tức thời
			if (!must_attack && !must_defend) return 0;
		}
		// Vẫn còn thời gian
		else if (!ai_terminate_reason) {
			// Tìm những nước phòng thủ (x2)
			impl_ab_find_defensive_moves_x2(
				p_defend, player2, ptr_cur_moves, attacking_player, ptr_moves_defend_x2,
				is_attacking, last_cell, must_attack);

			// Tìm nước tấn công cho [player1]
			impl_ab_find_attacking_moves(
				is_attacking, p_defend, start_from, must_defend, player1, can_attack,
				ptr_cur_moves, attacking_player, ptr_moves_defend_x2);

			// Xét những ô có evaluation thấp (level 1 -> 2)
			impl_ab_find_low_eval_moves(
				ptr_cur_moves, start_from, last_cell, p_defend, is_attacking, player1,
				ptr_cur_moves_old, attacking_player);
		}

		// Đéo tìm được nước đi nào
		if (ptr_cur_moves_old == ptr_cur_moves) return 0;
		// Nghịch lý ngược đời
		if (is_attacking && !can_attack && p_defend && !must_attack) return 0;


		// Xử lý lần lượt các nước đi được chuẩn bị trong mảng [cur_moves]
		int best_ab;
		{
			bool retflag;
			int retval = impl_process_cur_moves(
				best_ab, ptr_cur_moves_old, ptr_cur_moves, is_attacking, player1,
				player2, log_win_moves, last_cell, ptr_moves_defend_x1,
				ptr_moves_defend_x2, start_from, retflag);
			if (retflag) return retval;
		}
		return best_ab;
	}

	int AI::impl_search_alpha_beta(int player1, PCell *ptr_cur_moves, int log_win_moves,
		PCell start_from, PCell last_cell, int is_attacking) {
		if (log_win_moves) {
			int res = impl_search_alpha_beta_uncached(
				player1, ptr_cur_moves, log_win_moves,
				start_from, last_cell, is_attacking);
			assert(std::abs(res) < 5000);
			return res;
		}

		hash_tbl.add_info(start_from, last_cell, is_attacking);
		bool ok; HashTable::Record *rec;
		hash_tbl.get_result(ok, rec);
		hash_tbl.remove_info(start_from, last_cell, is_attacking);
		if (ok) {
			//if (rec->result != res)
			cnt_cache_hit++;
#ifdef KSH_DEBUG
			//std::string b;
			//for (PCell p = board_beg; p < board_end; p++)
			//	b.push_back(p->piece + '0');
			//if (res == 0) assert(rec->result == 0);
			//if (res < 0) assert(rec->result < 0);
			//if (res > 0) assert(rec->result > 0);
#endif
			//if (rec->reached_depth) assert(reached_depth);
			//if (rec->reached_depth) reached_depth = true;
		}
		else {
			int res = impl_search_alpha_beta_uncached(
				player1, ptr_cur_moves, log_win_moves,
				start_from, last_cell, is_attacking);
			assert(std::abs(res) < 5000);
			rec->result = short(res);
			//rec->reached_depth = reached_depth;
#ifdef KSH_DEBUG
			//rec->ptr_cur_moves = ptr_cur_moves;
			//rec->player1 = player1;
			//rec->is_attacking = is_attacking;
			//rec->dep = cur_search_depth;
			//rec->global_dep = global_search_depth;
			/*rec->board.clear();
			for (PCell p = board_beg; p < board_end; p++)
				rec->board.push_back(p->piece + '0');*/
#endif
		}
		return rec->result;
	}

	// Hàm entry cho hàm [impl_search_alpha_beta]
	int AI::search_alpha_beta(bool wary, int is_attacking, int player1, int log_win_moves, PCell start_from)
	{
		alpha_beta_search_attacking = wary;
		hash_tbl.reset_state();
		return impl_search_alpha_beta(player1, cur_moves, log_win_moves, start_from, 0, is_attacking);
	}

}