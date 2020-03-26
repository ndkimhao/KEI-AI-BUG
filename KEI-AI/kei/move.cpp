#include "stdafx.h"
#include "AI.h"

namespace ksh {
	// Database chứa các opening moves, chôm từ AI Carbon
	const signed char opening_moves[] = {
		15, 1, 4, 0, 0, 0, 1, 1, 2, 3, 2, 2, 4, 2, 5, 3, 3, 4, 3, 3, 5, 5, 4, 3, 3, 6, 4, 5, 6, 5, 5, 4, 3, 1,
		11, 1, 1, 1, 0, 0, 2, 2, 3, 2, 3, 3, 1, 3, 2, 4, 1, 5, 3, 3, 3, 2, 5, 1, 4, 3,
		11, 1, 1, 0, 0, 0, 1, 1, 3, 2, 2, 2, 2, 3, 3, 3, 2, 4, 2, 5, 1, 5, 1, 4, 0, 3,
		9, 1, 0, 0, 0, 3, 0, 1, 1, 2, 0, 2, 2, 3, 3, 3, 3, 4, 2, 4, 1, 1,
		9, 1, 1, 1, 0, 0, 2, 2, 3, 2, 3, 3, 2, 3, 1, 4, 1, 3, 1, 5, 2, 4,
		9, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 1, 2, 3, 1, 2, 0, 3, 1, 4,
		9, 1, 0, 0, 2, 0, 1, 0, 1, 1, 0, 1, 2, 2, 3, 2, 3, 3, 2, 3, 0, 2,
		9, 1, 1, 1, 0, 0, 2, 2, 0, 2, 2, 1, 3, 1, 1, 2, 1, 3, 0, 3, 3, 0,
		9, 1, 0, 0, 2, 0, 1, 1, 1, 2, 2, 1, 1, 4, 3, 2, 4, 3, 2, 3, 0, 1,
		8, 1, 0, 2, 1, 1, 1, 2, 2, 0, 2, 2, 3, 0, 3, 1, 3, 2, 4, 0,
		8, 1, 1, 1, 3, 0, 0, 2, 3, 1, 1, 3, 2, 2, 2, 3, 1, 4, 3, 3,
		7, 1, 0, 0, 1, 1, 0, 1, 2, 2, 3, 2, 3, 3, 2, 3, 0, 2,
		7, 1, 3, 2, 2, 1, 2, 2, 1, 1, 1, 0, 0, 0, 0, 1, 1, 2,
		7, 1, 0, 0, 0, 1, 1, 0, 0, 3, 2, 1, 3, 2, 1, 2, 1, -1,
		7, 1, 1, 0, 0, 1, 0, 2, 2, 2, 2, 1, 3, 2, 5, 2, 3, 0,
		7, 1, 0, 0, 0, 1, 2, 0, 0, 3, 2, 1, 1, 3, 1, 2, 2, 3,
		7, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 2, 1, 2, 1, 3, 2, 3,
		7, 1, 0, 1, 1, 1, 2, 0, 0, 2, 1, 2, 3, 4, 2, 3, 2, 1,
		6, 1, 1, 0, 0, 0, 0, 2, 1, 1, 3, 3, 2, 2, 1, -1,
		6, 1, 1, 0, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2, 0, -1,
		6, 1, 0, 0, 1, 0, 2, 1, 3, 1, 3, 2, 2, 2, 1, -1,
		6, 1, 3, 0, 0, 0, 1, 1, 2, 1, 2, 2, 1, 2, -1, 0,
		6, 1, 0, 0, 1, 1, 3, 1, 2, 2, 4, 3, 3, 3, 2, 4,
		6, 1, 0, 2, 0, 0, 1, 1, 1, 2, 2, 2, 2, 1, 3, 0,
		6, 1, 2, 1, 0, 0, 2, 2, 0, 1, 2, 3, 1, 2, 2, 0,
		6, 1, 2, 1, 0, 0, 1, 3, 0, 1, 2, 3, 1, 2, 2, 0,
		5, 2, 2, 2, 0, 0, 1, 0, 1, 1, 0, 1, 2, 1, 1, 2,
		5, 1, 0, 0, 2, 1, 0, 1, 2, 3, 1, 2, 1, 3,
		5, 1, 1, 0, 0, 0, 1, 1, 1, 2, 2, 1, 1, -1,
		5, 1, 1, 1, 1, 0, 0, 2, 1, 3, 1, 2, 2, 2,
		5, 1, 1, 0, 0, 0, 0, 1, 1, 1, 2, 3, 2, 2,
		5, 2, 0, 0, 1, 1, 1, 2, 0, 1, 2, 1, 2, 0, 3, 0,
		5, 1, 0, 2, 1, 0, 2, 0, 2, 1, 1, 1, -1, 3,
		5, 1, 1, 0, 1, 1, 0, 1, 2, 1, 3, 2, 2, -1,
		5, 1, 1, 1, 0, 0, 2, 1, 0, 2, 3, 1, 0, 1,
		5, 2, 1, 1, 0, 0, 2, 1, 1, 3, 1, 2, 0, 1, 3, 1,
		5, 2, 1, 0, 0, 0, 0, 1, 2, 1, 1, 1, 1, 2, -1, 2,
		5, 1, 1, 0, 1, 2, 0, 1, 3, 2, 2, 1, 2, -1,
		5, 2, 0, 1, 1, 2, 2, 1, 3, 0, 0, 3, 1, 1, 0, 2,
		5, 1, 2, 0, 0, 2, 1, 1, 1, 2, 2, 2, 0, 0,
		5, 1, 0, 0, 1, 1, 1, 0, 1, 2, 0, 2, 0, 1,
		4, 1, 1, 0, 0, 1, 1, 3, 1, 2, -1, 0,
		4, 1, 1, 0, 0, 1, 1, 1, 1, 2, -1, 0,
		4, 1, 1, 0, 1, 1, 0, 2, 2, 2, 0, 0,
		4, 1, 2, 0, 0, 0, 0, 2, 1, 1, 2, 2,
		4, 1, 0, 0, 1, 1, 3, 1, 2, 2, 3, 3,
		4, 2, 2, 0, 0, 0, 2, 2, 1, 1, 0, -1, 1, -1,
		4, 1, 2, 0, 0, 0, 2, 1, 1, 1, 2, 2,
		4, 1, 1, 0, 0, 1, 2, 0, 1, 2, -1, 0,
		4, 1, 1, 0, 0, 0, 0, 2, 1, 1, -1, -1,
		4, 1, 1, 0, 0, 1, 2, 2, 1, 2, -1, 0,
		4, 1, 0, 0, 1, 1, 2, 1, 2, 2, 3, 3,
		3, 1, 0, 1, 0, 0, 1, 0, 1, 1,
		3, 1, 0, 0, 2, 0, 1, 1, 2, 2,
		3, 4, 1, 0, 0, 1, 1, 2, 0, 2, 0, 0, 2, 0, 2, 2,
		3, 1, 1, 2, 0, 0, 1, 1, 1, 3,
		3, 1, 1, 0, 0, 0, 1, 1, 1, 2,
		3, 1, 0, 0, 0, 2, 0, 1, 0, -1,
		3, 1, 0, 0, 2, 1, 1, 1, 2, 2,
		3, 1, 0, 0, 0, 1, 1, 2, 1, 1,
		3, 2, 0, 0, 2, 2, 1, 1, 2, 1, 1, 2,
		3, 1, 0, 1, 3, 0, 2, 0, 1, 1,
		3, 1, 0, 0, 2, 1, 2, 2, 1, 1,
		3, 4, 0, 0, 1, 1, 2, 2, 1, -1, -1, 1, 3, 1, 1, 3,
		3, 1, 0, 0, 0, 2, 1, 2, 1, 0,
		3, 2, 1, 0, 0, 3, 1, 2, 2, 3, 1, 1,
		3, 2, 0, 0, 0, 3, 0, 2, 1, 3, -1, 3,
		3, 1, 0, 0, 0, 3, 1, 2, 1, 1,
		3, 1, 1, 0, 0, 2, 1, 2, 1, 1,
		3, 1, 0, 0, 2, 1, 1, 2, 0, 2,
		3, 1, 0, 0, 2, 2, 1, 2, 0, 1,
		3, 1, 0, 0, 3, 3, 2, 2, 1, 1,
		3, 1, 0, 0, 3, 2, 2, 2, 1, 1,
		3, 1, 0, 0, 3, 1, 2, 2, 3, 3,
		3, 6, 0, 0, 1, 0, 2, 0, 1, 1, 1, -1, 0, 2, 0, -2, 2, 2, 2, -2,
		3, 3, 0, 0, 3, 2, 2, 1, 1, 0, 1, 1, 0, 1,
		2, 3, 0, 0, 1, 1, 0, 2, 2, 0, 1, 2,
		2, 8, 0, 0, 1, 0, -1, -1, 0, -1, 1, -1, 2, -1, -1, 1, 0, 1, 1, 1, 2, 1,
		2, 2, 0, 0, 2, 2, 1, 3, 3, 1,
		1, 8, 0, 0, -1, 0, 1, 0, 0, 1, 0, -1, 1, 1, -1, 1, 1, -1, -1, -1,
		0, 0
	};

	// Tìm HCN nhỏ nhất chứa toàn bộ quân cờ
	RectRegion AI::find_active_rect_region() {
		RectRegion res = { 0,0,0,0 };

		PCell p, k;
		for (p = board_beg; p->piece != 1 && p->piece != 2; p++);
		res.left = p->x;
		for (k = board_end; k->piece != 1 && k->piece != 2; k--);
		res.right = k->x;

		res.top = res.bot = k->y;
		for (; p < k; p++)
			if (p->piece == 1 || p->piece == 2) {
				mini(res.top, p->y);
				maxi(res.bot, p->y);
			}

		return res;
	}

	// Transform (x1, y1) thành tọa độ (x, y) trong [rect], theo hướng [flip]
	void transform_board_position(RectRegion rect, int flip, int &x, int &y, int x1, int y1) {
		switch (flip) {
		case 0: x = rect.left + x1; y = rect.top + y1; break;
		case 1: x = rect.right - x1; y = rect.top + y1; break;
		case 2: x = rect.left + x1; y = rect.bot - y1; break;
		case 3: x = rect.right - x1; y = rect.bot - y1; break;
		case 4: x = rect.left + y1; y = rect.top + x1; break;
		case 5: x = rect.right - y1; y = rect.top + x1; break;
		case 6: x = rect.left + y1; y = rect.bot - x1; break;
		case 7: x = rect.right - y1; y = rect.bot - x1; break;
		}
	}

	// Tìm nước đi có trong opening moves database
	void AI::find_opening_move()
	{
		RectRegion rect = find_active_rect_region();
		for (const signed char *ptr = opening_moves, *next_ptr;; ptr = next_ptr) {
			int len1 = *ptr++;
			int len2 = *ptr++;
			next_ptr = ptr + 2 * (len1 + len2);
			if (len1 < global_moves_cnt) return;
			else if (len1 > global_moves_cnt) continue;

			for (int flip = 0; flip < 8; flip++) {
				for (int i = 0;; i++) {
					int x1 = ptr[2 * i];
					int y1 = ptr[2 * i + 1];
					if (i == len1) {
						ptr += 2 * (len1 + random(len2));
						x1 = *ptr++;
						y1 = *ptr;
					}
					int x, y;
					transform_board_position(rect, flip, x, y, x1, y1);
					if (i == len1) {
						if (x >= BORDER_LIMIT && y >= BORDER_LIMIT && 
							x < width - BORDER_LIMIT && y < height - BORDER_LIMIT) 
							set_result_move(cell(x, y));
						msg("Super easy move :>");
						return;
					}
					if (cell(x, y)->piece != 2 - (i & 1)) break;
				}
			}
		}
	}

	// Đi nước đầu (AI đi trước)
	void AI::do_first_move()
	{
		// Đi random trong 4 ô giữa
		set_result_move(cell(width / 2 + random(2) - 1, height / 2 + random(2) - 1));
	}

	// Đi nước đầu (AI đi trước)
	void AI::do_third_move()
	{
		// Tìm vị trí đi đầu tiên (của AI)
		PCell first_move = 0;
		for (PCell p = board_beg; p < board_end; p++)
			if (p->piece == 1) first_move = p;
		// Đi ô random, cạnh ô đi đầu tiên
		set_result_move(next_cell_dir(first_move, random(8), direction_offset));
	}

	// Tìm các nước đi bắt buộc, hoặc có sẵn trong database
	void AI::find_easy_move()
	{
		// Đi các nước đầu, nước có sẵn trong database
		if (global_moves_cnt == 0) {
			msg("First move easy =]]");
			return do_first_move();
		}
		if (global_moves_cnt == 2) {
			msg("First move easy ??!");
			do_third_move();
		}
		find_opening_move();

		// Có ô Level 3
		if (head_eval_level[3][0]) {
			// Đi vào được => đm thắng cmnr
			if (set_result_move(head_eval_level[3][0])) {
				msg("I win cmnr :>");
				return;
			}
		}

		// Đ* thằng kia có ô Level 3 => chặn
		set_result_move(head_eval_level[3][1]);
		msg("Haha chicken :))");
	}

	// Gán nước đi tốt nhất cho tới hiện tại
	bool AI::set_result_move(PCell p)
	{
		if (p < board_beg || p >= board_end || p->piece) return false;
		if (!ai_terminate_reason || !result_move) result_move = p;
		return true;
	}

	// Thực hiện nước đi thật sự tại ô [p]
	bool AI::do_move(PCell p, int z)
	{
		if (p < board_beg || p >= board_end || p->piece) return false;
		p->piece = z;
		global_moves_cnt++;
		if (z == 3) global_blocked_cells_cnt++;
		eval_cell(p);
		last_move = p;
		return true;
	}

	// Thực hiện nước đi thật sự tại ô (x,y), 0-based
	bool AI::do_move(int x, int y, int z)
	{
		return do_move(cell(x, y), z);
	}

	// Undo nước đi tại (x,y)
	bool AI::undo_move(int x, int y) {
		PCell p = cell(x, y);
		if (!is_valid_cell(p) || !p->piece) return false;
		p->piece = 0;
		global_moves_cnt--;
		eval_cell(p);
		last_move = 0;
		return true;
	}
}