#include "stdafx.h"
#include "AI.h"

namespace ksh {

	// Tính điểm eval cho pattern 5 con
	static inline void eval_p5(TPiece* p, TPiece* beg, TPiece* end, int c[4],
		TPiece Me, TPiece Opp, int& eval, bool& six) {
		// Có thể tạo 5 con cho [Me]
		if (!c[int(Opp)]) {
			int L = 0;
			// Cơ hội ở đầu & đuôi nè
			if (beg[0] == TPiece::Empty && end != &p[9] && (end[0] == TPiece::Empty || end[0] == Me)) {
				L++;
				if (end[-1] == TPiece::Empty && end > & p[5]) L++;
			}
			// Đi ra đường 6 => éo có ý nghĩa gì
			if (c[int(Me)] == 4 && (beg != p && beg[-1] == Me || end != &p[9] && end[0] == Me)) {
				six = true;
			}
			const int PATTERN_EVAL[3][5] = {
				{ 0, 2, 25, 270, 3000 },
				{ 1, 7, 150, 523, 0 },
				{ 1, 11, 170, 0, 0 }
			};
			// Gâu gâu
			maxi(eval, PATTERN_EVAL[L][c[int(Me)]]);
		}
	}

	// Tính evaluation score cho toàn bộ pattern 9 quân
	void AI::calc_eval_pattern()
	{
		TPiece p[10] = {};
		// Dãy bit tương đương với 9 quân cờ trên 1 hàng
		for (int p_bits = 0; p_bits < PIECES_PATTERN_SIZE; p_bits++) {
			// Đếm mỗi loại quân trong cửa sổ chạy 5 quân
			// Ô giữa (4) luôn là ô trống (TPiece::Empty = 0)
			//  0 1 2 3 _ 5 6 7 8
			// \____c____/
			int c[4] = {};
			for (int i = 0; i < 9; i++) {
				p[i] = TPiece((p_bits >> ((8 - i) * 2)) & 3);
				if (i < 4) c[int(p[i])]++;
			}

			int eval_me = 0, eval_opp = 0;
			bool six_me = false, six_opp = false;
			// Chỉ xét khi ô giữa là Empty
			if (p[4] == TPiece::Empty) {
				// Đánh giá trên cửa sổ 5 con trượt về bên phải
				// a, b là 2 đầu của cửa sổ 5 ô [a,b)
				for (TPiece* a = p, *b = &p[5]; a != &p[5]; a++, b++) {
					// Chỉ xét khi 5 ô đều nằm trong bảng
					if (!c[int(TPiece::Invalid)]) {
						eval_p5(p, a, b, c, TPiece::Me, TPiece::Opp, eval_me, six_me);
						eval_p5(p, a, b, c, TPiece::Opp, TPiece::Me, eval_opp, six_opp);
					}
					c[int(*a)]--, c[int(*b)]++;
				}
			}

			// Lưu kết quả
			pieces_patterns[p_bits].eval[0] = short(eval_me * !six_me);
			pieces_patterns[p_bits].eval[1] = short(eval_opp * !six_opp);
		}
	}
}