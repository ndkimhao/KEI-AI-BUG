#include "stdafx.h"
#include "AI.h"

namespace ksh {
	// Tính điểm eval thứ cấp cho ô [p], cho [player]
	int AI::calc_sec_eval(int player1, PCell p0)
	{
		// Đếm quân
		int eval_additional = 0, count_1, count_2;
		count_pieces_8dir(p0, player1, count_1, count_2);
		int count_total = count_1 + count_2;

		if (count_total == 0) eval_additional -= 20; // vắng vẻ vkl
		if (count_total > 4) eval_additional -= 4 * (count_total - 3); // chật chội vkl

		// Đếm hướng bị chặn
		int blocked_1, blocked_2;
		count_blocked_4dir(p0, player1, blocked_1, blocked_2);

		// Nếu khu xung quanh [p0] chỉ có [player1] => ngon
		if (count_2 == 0 && count_1 > 0 && p0->eval[player1].eval_sum >= 11)
			eval_additional += (count_1 + 1) * 5;

		// Nếu khu xung quanh [p0], [player2] bị chặn tứ phía => ngon vãi c*t
		if (blocked_2 == 4) {
			int high_eval_cnt = 0;
			for (int i = 0; i < 4; i++)
				if (p0->eval[player1].eval[i] >= 11) high_eval_cnt++;

			eval_additional += 15;
			if (high_eval_cnt > 1) eval_additional += high_eval_cnt * 64;
		}

		// Kim So Hyun is so BEAUTIFUL <3 <3
		return eval_additional + p0->eval[player1].eval_sum;
	}

	// Đếm số hướng bị chặn (ko triển quân dc nữa), cho 2 player
	// Return max = 4 hướng (ngang, dọc, 2 đường chéo)
	void AI::count_blocked_4dir(const ksh::PCell &p0, int player1, int &blocked_1, int &blocked_2)
	{
		blocked_1 = blocked_2 = 0;
		for (int i = 0; i < 4; i++) {
			if (p0->eval[player1].eval[i] <= 1) blocked_1++;
			if (p0->eval[1 - player1].eval[i] <= 1) blocked_2++;
		}
	}

	// Đếm số quân xung quanh 8 ô [p0], cho 2 player
	void AI::count_pieces_8dir(const ksh::PCell &p0, int player_1, int &count_1, int &count_2)
	{
		count_1 = count_2 = 0;
		for (int i = 0; i < 8; i++) {
			PCell p = p0;
			next_cell_ptr(p, 1, direction_offset[i]);
			if (p->piece == player_1 + 1) count_1++;
			if (p->piece == 2 - player_1) count_2++;
		}
	}

	// Tính điểm eval thứ cấp cho ô [p] cho 2 player
	int AI::calc_sec_eval(PCell p)
	{
		int eval_0 = calc_sec_eval(0, p);
		int eval_1 = calc_sec_eval(1, p);
		return eval_0 > eval_1 ? eval_0 + eval_1 / 2 : eval_0 / 2 + eval_1;
	}

	// Tìm nước đi cho eval tốt nhất, sau 6 nước
	void AI::search_cell_best_eval()
	{
		int cnt_res = 0;
		PCell best_move = 0;
		// Trước hết tìm những ô có eval cao
		if (global_moves_cnt > 4) {
			int best_val = -INF;
			for (PCell p = board_beg; p < board_end; p++) {
				if (!p->piece && (p->eval[0].eval_sum > 10 || p->eval[1].eval_sum > 10)) {
					int eval_0 = calc_sec_eval(0, p), eval_1 = calc_sec_eval(1, p);
					int accum = (eval_0 > eval_1) ?
						int((eval_0 + eval_1 / 2)*1) :
						(eval_0 / 2 + eval_1);
					p->piece = 1; eval_cell(p);
					accum -= dfs_eval(1);
					p->piece = 0; eval_cell(p);
					if (maxi(best_val, accum)) best_move = p, cnt_res = 1;
					else if (accum > best_val - 22) {
						cnt_res++;
						if (!random(cnt_res)) best_move = p;
					}
				}
			}
		}
		// Nếu tất cả các ô đều có eval thấp
		if (!best_move) {
			int m = -SMALL_INF;
			for (PCell p = board_beg; p < board_end; p++)
				if (!p->piece) maxi(m, calc_sec_eval(p));

			int d = std::min(abs(m / 12), 26);
			cnt_res = 0;
			for (PCell p = board_beg; p < board_end; p++) {
				if (!p->piece && calc_sec_eval(p) >= m - d) {
					cnt_res++;
					if (!random(cnt_res)) best_move = p;
				}
			}
		}
		// Gâu gâu
		set_result_move(best_move);
		cell_with_best_eval = best_move;

		// Đi như cứt => chặn thôi chứ làm đéo gì nữa
		defend_isolated_move();
	}

	// Thủ khi đối thủ đi nước xa vùng đang chơi
	void AI::defend_isolated_move()
	{
		if (last_move) {
			PCell p = last_move;
			for (int i = 0;; i++) {
				if (i == 8) {
					// Nếu đối thủ đi nước có eval rất thấp => xa khu vực đang chơi => thủ ngay chỗ đó
					set_result_move(next_cell_dir(p, random(8), direction_offset));
					break;
				}
				if (next_cell_dir(p, i, direction_offset)->eval[1].eval[i & 3] != 11) break;
			}
		}
	}

	// Tìm ô có eval cao nhất, nếu ô cao nhất có eval < 0 thì trả về ptr NULL
	PCell AI::find_cell_best_eval()
	{
		int best_eval = -1;
		PCell best_cell = 0;
		for (int i = 2; i > 0 && !best_cell; i--)
			for (int k = 0; k < 2; k++)
				for (PCell p = head_eval_level[i][k]; p; p = p->eval[k].ptr_next)
					if (maxi(best_eval, calc_sec_eval(p))) best_cell = p;
		return best_cell;
	}

	// Tìm tổng eval tốt nhất sau 6 nước
	int AI::dfs_eval(int player1)
	{
		if (head_eval_level[3][player1]) return 700; // Bố có đường thắng => ngon
		int player2 = 1 - player1;

		// Đường ngon của thằng đ* kia
		int res = 0;

		// Nếu thằng đ* kia có đường ngon (Level 3) => chặn
		PCell p = head_eval_level[3][player2];

		// Thằng đ* kia cũng éo có đường ngon => kiếm ô nào hơi ngon ngon tí
		if (!p && cur_search_depth < 6) p = find_cell_best_eval();

		// Đéo có luôn
		if (!p) {
			// Đời như c*t thật là đây, time đéo có => tính đại
			if (time_limit_turn < 150) {
				res = eval_global_sum[0] - eval_global_sum[1];
			}
			else {
				// Ngồi ngẫm nghĩ lại cuộc đời mình
				// Tính điểm eval bằng tổng eval các khu vực 5x5 trong bảng
				res = 0;
				for (PCell q = board_beg; q < board_end; q++)
				{
					if (q->piece || q->eval[0].eval_sum == q->eval[1].eval_sum) continue;
					int count[4] = {};
					count_pieces_5x5(q, count);
					res += 
						(q->eval[0].eval_sum - q->eval[1].eval_sum) *
						(count[0] + (count[1] - count[2]) / 2 - 1);
				}
				res /= 12;
			}

			for (int i = 2; i > 0; i--) {
				for (p = head_eval_level[i][0]; p; p = p->eval[0].ptr_next) 
					res += 90;
				for (p = head_eval_level[i][1]; p; p = p->eval[1].ptr_next) 
					res -= 32;
			}
			if (player1) res = -res;
			return int(res / 3);
		}

		cur_search_depth++; p->piece = player1 + 1; eval_cell(p);
		res = -dfs_eval(player2);
		cur_search_depth--; p->piece = 0; eval_cell(p);

		return res;
	}

	// Đếm quân trong khu vực 5x5 ô, [q] là ô giữa
	// Mảng [count] phải được reset về 0 sẵn
	void AI::count_pieces_5x5(const ksh::PCell &q, int count[4])
	{
		PCell q2 = q - 2 * height2 - 2; // Nhảy xéo trái trên 2 ô
		for (int i = 0; i < 5; q2 += height2, i++)
			for (int j = 0; j < 5; j++)
				count[q2[j].piece]++;
	}
}