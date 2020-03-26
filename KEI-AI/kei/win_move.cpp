#include "stdafx.h"
#include "AI.h"

namespace ksh {

	// Biến thể của TSS, chỉ xét tới đường 4
	// Kiếm đường thắng 100% bằng cách đi liên tục các đường 4
	// Và nước cuối tạo ra được 2 đường 4 cùng lúc
	PCell AI::dfs_win_moves(int player1)
	{
		// Nếu đang có đường 4 thì gọi hàm này làm đéo gì
		assert(!head_eval_level[3][0] && !head_eval_level[3][1]);

		// Tìm các đường ba của [player1] (level 1,2)
		PCell *ptr_cur_moves = cur_moves;
		find_all_triplets(player1, ptr_cur_moves);

		// Thử tất cả các cell level 1,2 vừa tìm được
		for (ptr_cur_moves--; ptr_cur_moves >= cur_moves; ptr_cur_moves--)
			if (dfs_win_moves(player1, *ptr_cur_moves)) {
				calc_eval_win_moves();
				return *ptr_cur_moves;
			}
		return 0;
	}

	// Implement của dfs_win_moves, chạy sau khi lọc ra cell level 1,2
	// Đi từ ô [start_from] đầu tiên
	PCell AI::dfs_win_moves(int player1, PCell start_from)
	{
		PCell p2 = 0, res = 0;

		// Tấn công, bắt đầu từ ô [start_from]
		start_from->piece = player1 + 1; eval_cell(start_from);

		// Nếu [player2] không có sẵn đường 4
		if (!head_eval_level[3][1 - player1]) {
			// Đường 4 của [player1]
			p2 = head_eval_level[3][player1];
			if (p2) {
				// [player2] có 1 đường chống thôi
				p2->piece = 2 - player1; eval_cell(p2);

				// Đường 4 của [player1]
				PCell p = head_eval_level[3][player1];
				// [player1] vẫn còn đường 4 => haha =]]
				if (p) {
					append_to_win_moves(p);
					res = p; // Thắng cmnr đó
				}
				// [player1] bị chặn hết đường 4
				else {
					// Đường 4 của [player2] sau khi chặn
					p = head_eval_level[3][1 - player1];
					// Có => ăn *** => bắt buộc phải chặn
					if (p) {
						// Chặn xong ăn dc => ngon
						if (p->eval[player1].eval_sum >= 270
							&& dfs_win_moves(player1, p))
							res = p;
					}
					// 2 thằng hết đường 4, nhưng giờ là lượt của bố :v
					else {
						// Tìm các đường 3 tạo ra xung quang nước vừa đi
						for (int i = 0; i < 8; i++) {
							int diroff = direction_offset[i];
							p = start_from; next_cell_ptr(p, 1, diroff);
							for (int j = 0; j < 4 && (p->piece != 3); j++, next_cell_ptr(p, 1, diroff)) {
								if (p->eval[player1].eval_sum >= 270) {
									// Có đường 3 thì đệ quy tìm tiếp
									if (dfs_win_moves(player1, p)) {
										res = p;
										goto grand_exit;
									}
								}
							}
						}
					}
				}
			grand_exit:
				// Quay lui
				p2->piece = 0; eval_cell(p2);
			}
		}

		// Quay lui
		start_from->piece = 0; eval_cell(start_from);

		// Ẳng ẳng gâu gâu win cmnr =]]
		if (res) {
			append_to_win_moves(p2);
			append_to_win_moves(start_from);
		}
		return res;
	}

	// Tính mảng [win_moves_path_eval], là điểm eval cho các nước trong [win_moves_path]
	void AI::calc_eval_win_moves()
	{
		wary_attack = true;

		// Thêm cả những ô có eval cao (level 2)
		for (PCell p = head_eval_level[2][1]; p; p = p->eval[1].ptr_next)
			append_to_win_moves(p);

		// Tính eval score (calc_sec_eval)
		int win_len = int(ptr_win_moves_path - win_moves_path);
		assert(win_len < WIN_MOVES_SIZE);

		int *ptr_eval = win_moves_path_eval + win_len - 1;
		for (PCell *ptr_win = ptr_win_moves_path - 1;
			ptr_win != win_moves_path - 1;
			ptr_win--, ptr_eval--) {
			*ptr_eval = calc_sec_eval(1, *ptr_win);
			if (*ptr_win == cell_with_best_eval) *ptr_eval += 75;
		}

		// Thêm cả cơ hội tạo đường 4 cho [Me] (từ ô level 1)
		for (PCell p = head_eval_level[1][0]; p; p = p->eval[0].ptr_next) {
			if (p->eval[0].eval_sum < 270) continue;
			append_to_win_moves(p);
			if (p->ptr_in_win_moves_path == ptr_win_moves_path - 1) {
				assert(win_len < WIN_MOVES_SIZE - 1);
				win_moves_path_eval[win_len++] = p->eval[0].eval_sum >> 3;
			}
		}
	}

	// Thêm [p] vào [win_moves]
	void AI::append_to_win_moves(PCell p)
	{
		if (p->ptr_in_win_moves_path >= ptr_win_moves_path ||
			*p->ptr_in_win_moves_path != p) {
			if (ptr_win_moves_path >= win_moves_path + WIN_MOVES_SIZE) { assert(0); return; }
			p->ptr_in_win_moves_path = ptr_win_moves_path;
			*ptr_win_moves_path++ = p;
		}
	}

	// Tìm tất cả đường ba của [player1], cho vào [cur_moves]
	void AI::find_all_triplets(int player1, ksh::PCell * &ptr_cur_moves)
	{
		for (int j = 1; j <= 2; j++) {
			for (PCell p = head_eval_level[j][player1]; p; p = p->eval[player1].ptr_next) {
				if (p->eval[player1].eval_sum >= 270) {
					if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break; // Overflow
					*ptr_cur_moves++ = p;
				}
			}
		}
	}
}