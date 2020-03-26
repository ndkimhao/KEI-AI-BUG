#pragma once

namespace ksh {
	// Trạng thái của 1 ô trong bảng, kiểu int
	typedef int TPieceInt;
	struct TCell;
	// Con trỏ tới 1 ô trong bảng
	typedef TCell *PCell;

	// Trạng thái của 1 ô trong bảng
	enum class TPiece {
		Empty = 0, Me = 1, Opp = 2, Invalid = 3
	};

	// Giá trị evalutaion của 1 ô trong bảng
	struct TEvaluation
	{
		// Tổng các giá trị [eval]
		int eval_sum;
		// 5 6 7
		// 4 8 0
		// 3 2 1
		// Giá trị eval theo 4 hướng: 0,1,2,3
		int eval[4];
		// Level của evaluation
		int eval_level;
		// Linked list, các cell có cùng level
		PCell ptr_next, *ptr_ptr_prev;
	};

	// Dữ liệu của 1 ô trong bảng
	struct TCell
	{
		// Kiểu int, dùng tương tự như [enum class TPiece]
		// Empty = 0, Me = 1, Opp = 2, Invalid = 3
		TPieceInt piece;
		// Giá trị evaluation tại cell cho cả 2 player
		// Me = eval[0], Opp = eval[1]
		TEvaluation eval[2];
		// Tọa độ (x,y) của cell, 0-based
		short x, y;
		// Con trỏ tới vị trí trong mảng [win_moves_path]
		PCell *ptr_in_win_moves_path;
	};

	// Giá trị evaluation cho 1 ô pattern cụ thể
	struct TPatternEvaluation { 
		// Giá trị evaluation của pattern cho 2 đối thủ
		// Me = eval[0], Opp = eval[1]
		short eval[2]; 
	};

	// HCN giới hạn một vùng trong bảng
	struct RectRegion {
		// Giới hạn cho HCN
		short left, right, top, bot;
	};
}