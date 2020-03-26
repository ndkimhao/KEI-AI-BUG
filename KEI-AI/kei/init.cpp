#include "stdafx.h"
#include "AI.h"

namespace ksh {
	// Reset tạo lại các biến
	void AI::init_reset_mem() {
		memset(head_eval_level, 0, sizeof(head_eval_level));
		memset(win_moves, 0, sizeof(win_moves));
		memset(eval_global_sum, 0, sizeof(eval_global_sum));
		global_moves_cnt = global_blocked_cells_cnt = 0;
	}

	// Khởi tạo bộ nhớ cho mảng [board]
	void AI::init_alloc_board() {
		delete[] board;
		height2 = height + 2;
		board = new TCell[(width + 12)*(height2)];
		board_beg = board + 6 * height2;
		board_end = board_beg + width * height2;
	}

	// Khởi tạo mảng [direction_offset]
	void AI::init_direction_offset() {
		// 5 6 7
		// 4 8 0
		// 3 2 1
		direction_offset[0] = sizeof(TCell);
		direction_offset[4] = -direction_offset[0];
		direction_offset[1] = sizeof(TCell)*(1 + height2);
		direction_offset[5] = -direction_offset[1];
		direction_offset[2] = sizeof(TCell)* height2;
		direction_offset[6] = -direction_offset[2];
		direction_offset[3] = sizeof(TCell)*(-1 + height2);
		direction_offset[7] = -direction_offset[3];
		direction_offset[8] = 0;
	}

	// Điền giá trị ban đầu cho mảng [board]
	void AI::init_board() {
		PCell p = board;
		for (int x = -6; x <= width + 5; x++) {
			for (int y = -1; y <= height; y++) {
				p->piece = (x < 0 || y < 0 || x >= width || y >= height) ? 3 : 0;
				p->x = (short)x;
				p->y = (short)y;
				for (int k = 0; k < 2; k++) {
					TEvaluation *pr = &p->eval[k];
					pr->eval_level = 0;
					pr->eval_sum = 4;
					pr->eval[0] = pr->eval[1] = pr->eval[2] = pr->eval[3] = 1;
				}
				p++;
			}
		}
	}

	// Khởi tạo AI
	void AI::init(int w, int h)
	{
		width = w, height = h;
#ifdef KSH_DEBUG
		hash_tbl.reserve_memory(int(1e9));
#else
		hash_tbl.reserve_memory(mem_limit);
		//hash_tbl.reserve_memory(int(1e9));
#endif

		init_reset_mem();
		init_alloc_board();
		init_direction_offset();
		init_board();

		calc_eval_pattern();
	}
}