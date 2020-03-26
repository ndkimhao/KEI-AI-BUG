#include "stdafx.h"
#include "AI.h"

namespace ksh {

	// Xóa eval ở ô [p0]
	void AI::clear_cell_eval(PCell p0) {
		for (int k = 0; k < 2; k++) {
			TEvaluation *pr = &p0->eval[k];
			if (pr->eval_sum) {
				if (pr->eval_level) {
					if ((*pr->ptr_ptr_prev = pr->ptr_next) != 0) 
						pr->ptr_next->eval[k].ptr_ptr_prev = pr->ptr_ptr_prev;
					pr->eval_level = 0;
				}
				eval_global_sum[k] -= pr->eval_sum;
				pr->eval_sum = pr->eval[0] = pr->eval[1] = pr->eval[2] = pr->eval[3] = 0;
			}
		}
	}

	// Cập nhật eval cho các ô eval theo hướng dir
	void AI::eval_dir(PCell p0, int dir)
	{
		int diroff = direction_offset[dir];
		PCell p_plus5 = p0, p_end = p0, p = p0;
		next_cell_ptr(p_plus5, 5, diroff);

		// Lùi p cho tới viền bảng
		for (int m = 4; m > 0; m--) {
			prev_cell_ptr(p, 1, diroff);
			if (p->piece == 3) {
				next_cell_ptr(p_end, m, diroff);
				next_cell_ptr(p, 1, diroff);
				break;
			}
		}

		// Tính trước pattern bits cho cửa sổ 9 quân đầu
		unsigned pattern = 0;
		PCell qk = p_end;
		prev_cell_ptr(qk, 9, diroff);
		for (PCell q = p_end; q != qk; prev_cell_ptr(q, 1, diroff)) {
			pattern *= 4;
			pattern += q->piece;
		}

		// Nhảy từng cửa sổ 9 quân để tính eval score
		while (p->piece != 3) {
			if (!p->piece) {
				for (int k = 0; k < 2; k++) { // k = 0: [Me], k = 1: [Opp]
					TEvaluation *pr = &p->eval[k];
					// Cập nhật lại giá trị eval
					int *u = &pr->eval[dir];
					int pat_eval = pieces_patterns[pattern].eval[k];
					if (pat_eval >= 3000)
						// Thắng cmnr :3
						if (prev_cell(p, 5, diroff)->piece == k + 1 && next_cell(p, 1, diroff)->piece == 0 ||
							next_cell(p, 5, diroff)->piece == k + 1 && prev_cell(p, 1, diroff)->piece == 0) {
							pat_eval = 0;
						}

					int diff = pat_eval - *u;
					if (diff) {
						// Cập nhật eval score
						*u = pat_eval;
						eval_global_sum[k] += diff;
						pr->eval_sum += diff;

						// Xếp level cho ô
						int level = 0;
						if (pr->eval_sum >= 150) {
							level++;
							if (pr->eval_sum >= 2 * 150) {
								level++;
								if (pr->eval_sum >= 3000) level++;
							}
						}

						// Cho ô vào list level tương ứng
						if (level != pr->eval_level) {
							// Xóa khỏi linked list cũ
							if (pr->eval_level && (*pr->ptr_ptr_prev = pr->ptr_next) != 0)
								pr->ptr_next->eval[k].ptr_ptr_prev = pr->ptr_ptr_prev;

							// Đút vào linked list mới
							if ((pr->eval_level = level) != 0) {
								PCell *up = &head_eval_level[level][k];
								pr->ptr_next = *up;
								if (*up) (*up)->eval[k].ptr_ptr_prev = &pr->ptr_next;
								pr->ptr_ptr_prev = up;
								*up = p;
							}
						}
					}
				}
			}
			next_cell_ptr(p, 1, diroff);
			if (p == p_plus5) break;
			next_cell_ptr(p_end, 1, diroff);
			pattern >>= 2;
			pattern += p_end->piece << 16;
		}
	}

	// Cập nhật eval cho các ô theo 4 hướng ngang, dọc và 2 đường chéo
	void AI::eval_cell(PCell p0)
	{
		// Ô có người đi cmnr => xóa hết evaluate vì đéo còn ý nghĩa nữa
		if (p0->piece) clear_cell_eval(p0);
		// Tính eval 4 hướng
		for (int dir = 0; dir < 4; dir++) eval_dir(p0, dir);
		// Hàm ngắn vãi, comment cho dài thêm 1 dòng nữa
	}
}