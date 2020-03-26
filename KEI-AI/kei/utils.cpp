#include "stdafx.h"
#include "AI.h"

namespace ksh {
	// Trả về số random [0..max_val)
	unsigned AI::random(unsigned max_val)
	{
		static DWORD seed = GetTickCount();
		seed = seed * 1696969 + 696969696;
		return (unsigned)(UInt32x32To64(max_val, seed) >> 32);
	}

	// Trả về [PCell] tại ô (x,y), 0-based
	PCell AI::cell(int x, int y) {
		return board_beg + x * height2 + (y + 1);
	}

	// Kiểm tra [PCell] có phải là pointer hợp lệ
	bool AI::is_valid_cell(PCell p) const {
		return board_beg <= p && p < board_end;
	}

	// Ghi ra log (để debug)
	void AI::log(std::string msg) {
		if (logs != nullptr) logs->log(msg);
	}

	// Gán cho [pending_msg]
	void AI::msg(std::string msg) {
		pending_msg = msg;
	}

	// Hiển thị [pending_msg] cho người dùng
	void AI::show_msg() {
		if (logs != nullptr) logs->msg(pending_msg);
	}

	// Hiển thị thông báo cho người dùng
	void AI::show_msg(std::string msg) {
		if (logs != nullptr) logs->msg(msg);
	}

	// Bắt buộc dừng AI ngay lập tức
	void AI::abort_thinking() {
		ai_terminate_reason = 1;
	}
}