#include "stdafx.h"
#include "ai_manager.h"

namespace ai {

	AiManager::AiManager() {
		determine_max_memory();
	}

	void AiManager::determine_max_memory() {
		MEMORYSTATUSEX statex{ sizeof(statex) };
		GlobalMemoryStatusEx(&statex);
		long long free_mem = statex.ullAvailPhys;
		max_memory = std::max((long long)1e9, free_mem - reserved_memory);
		logman::LogManager::inst.write_free_mem();
	}

	void AiManager::delete_ai() {
		if (variant == AiVariant::KeiAi) { delete kei_ai; kei_ai = nullptr; }
	}
	void AiManager::new_ai() {
		if (variant == AiVariant::KeiAi) { kei_ai = new ksh::AI(); }
	}

	Cell& AiManager::board(int x, int y) { return _board[x + 1][y + 1]; };
	void AiManager::init_board() {
		memset(_board, 0, sizeof _board);
		for (int i = 0; i <= board_size.height; i++)
			_board[i][0] = _board[i][board_size.width + 1] = Cell::Invalid;
		for (int j = 0; j <= board_size.width; j++)
			_board[0][j] = _board[board_size.height + 1][j] = Cell::Invalid;

		stop_time = 0;
		ai_turn = AiTurn::Idle;
		win_moves.clear();
		undo_stack.clear();
	}

	void AiManager::init_ai() {
		determine_max_memory();
		logman::LogManager::inst.write_log("Time limit: " +
			std::to_string(time_turn) + " ms");
		logman::LogManager::inst.write_log("Mem limit: " +
			std::to_string(max_memory / 1024 / 1024) + " MB");

		set_time_turn(time_turn);
		set_max_memory(max_memory);
		logs.get_logs();
		if (variant == AiVariant::KeiAi) {
			kei_ai->logs = &logs;
			kei_ai->init(board_size.width, board_size.height);
		}
	}

	void AiManager::new_match(AiVariant vari) {
		assert(vari != AiVariant::Invalid);
		assert(ai_turn == AiTurn::Idle);
		if (vari != variant) {
			delete_ai();
			variant = vari;
			new_ai();
		}
		init_ai();
		init_board();
	}

	bool AiManager::is_valid_move(Point p) {
		return 0 <= p.x && p.x < board_size.height &&
			0 <= p.y && p.y < board_size.width &&
			board(p.x, p.y) == Cell::Empty;
	}

	bool AiManager::turn_human(Point move) {
		assert(variant != AiVariant::Invalid);
		assert(ai_turn == AiTurn::Idle);
		if (!is_valid_move(move)) return false;

		board(move.x, move.y) = Cell::Human;
		undo_stack.push_back(move);
		if (variant == AiVariant::KeiAi) {
			if (!kei_ai->do_move(move.x, move.y, 2)) assert(0);
		}
		return true;
	}

	void AiManager::start_turn_ai() {
		assert(variant != AiVariant::Invalid);
		assert(ai_turn == AiTurn::Idle);
		ai_turn = AiTurn::Thinking;
#pragma warning(suppress: 28159)
		stop_time = GetTickCount() + time_turn;
		set_stop_time();
		std::thread th([this]() {
			if (variant == AiVariant::KeiAi) {
				kei_ai->do_thinking();
				ksh::PCell res = kei_ai->get_result_move();
				result_move = { res->x, res->y };
				if (!kei_ai->do_move(result_move.x, result_move.y, 1)) assert(0);
			}

			assert(is_valid_move(result_move));
			board(result_move.x, result_move.y) = Cell::Ai;
			undo_stack.push_back(result_move);
			ai_turn = AiTurn::ResultReady;
		});
		th.detach();
	}

	void AiManager::_undo_at(Point p) {
		assert(variant != AiVariant::Invalid);
		assert(board(p.x, p.y) != Cell::Empty);
		board(p.x, p.y) = Cell::Empty;
		if (variant == AiVariant::KeiAi) {
			if (!kei_ai->undo_move(p.x, p.y)) assert(0);
		}
	}

	std::pair<Point, Point> AiManager::undo() {
		assert(ai_turn == AiTurn::Idle);
		if (undo_remain == 0) return INVALID_UNDO;
		else if (undo_stack.size() < 2) return INVALID_UNDO;
		else {
			std::pair<Point, Point> res = {
				undo_stack.back(), undo_stack[undo_stack.size() - 2] };
			_undo_at(res.first), _undo_at(res.second);
			undo_stack.pop_back(), undo_stack.pop_back();
			undo_remain--;
			return res;
		}
		//return INVALID_UNDO;
	}

	Cell AiManager::has_win_moves(Point last) {
		if (!win_moves.empty())
			return board(win_moves[0].x, win_moves[0].y);
		if (last == INVALID_POINT) return Cell::Invalid;
		Cell &p = board(last.x, last.y);
		assert(p != Cell::Invalid && p != Cell::Empty);
#define wm(x,y) win_moves.push_back({x,y})
		{
			int x1 = last.x, x2 = last.x + 1;
			while (board(x1, last.y) == p)  wm(x1, last.y), x1--;
			while (board(x2, last.y) == p)  wm(x2, last.y), x2++;
			if (win_moves.size() == 5) return p; else win_moves.clear();
		}
		{
			int y1 = last.y, y2 = last.y + 1;
			while (board(last.x, y1) == p)  wm(last.x, y1), y1--;
			while (board(last.x, y2) == p)  wm(last.x, y2), y2++;
			if (win_moves.size() == 5) return p; else win_moves.clear();
		}
		{
			int s = last.x + last.y, x1 = last.x, x2 = last.x + 1;
			while (board(x1, s - x1) == p) wm(x1, s - x1), x1--;
			while (board(x2, s - x2) == p)  wm(x2, s - x2), x2++;
			if (win_moves.size() == 5) return p; else win_moves.clear();
		}
		{
			int s = last.y - last.x, x1 = last.x, x2 = last.x + 1;
			while (board(x1, s + x1) == p)  wm(x1, s + x1), x1--;
			while (board(x2, s + x2) == p)  wm(x2, s + x2), x2++;
			if (win_moves.size() == 5) return p; else win_moves.clear();
		}
		return Cell::Invalid;
	}

	Point AiManager::get_last_move() {
		if (undo_stack.empty()) return INVALID_POINT;
		else return undo_stack.back();
	}

	Point AiManager::get_result_move() {
		assert(ai_turn == AiTurn::ResultReady);
		ai_turn = AiTurn::Idle;
		return result_move;
	}

	void AiManager::abort_thinking() {
		assert(variant != AiVariant::Invalid);
		if (variant == AiVariant::KeiAi)
			kei_ai->abort_thinking();
	}

	AiManager::~AiManager() {
		delete_ai();
	}

	AiTurn AiManager::get_ai_turn() {
		return ai_turn;
	}
	int AiManager::remaining_time() {
		if (ai_turn == AiTurn::Idle) return 0;
#pragma warning(suppress: 28159)
		else return stop_time - GetTickCount();
	}

	void AiManager::set_stop_time() {
		if (variant == AiVariant::KeiAi) kei_ai->stop_time = stop_time;
	}

	void AiManager::set_time_turn(int timeout) {
		time_turn = timeout;
		if (variant == AiVariant::KeiAi) kei_ai->time_limit_turn = time_turn;
	}
	int AiManager::get_time_turn() const {
		return time_turn;
	}
	void AiManager::set_max_memory(long long maxmem) {
		max_memory = maxmem;
		if (variant == AiVariant::KeiAi) kei_ai->mem_limit = max_memory;
	}
	long long AiManager::get_max_memory() const {
		return max_memory;
	}
	void AiManager::set_board_size(Size size) {
		assert(size.width <= MAX_BOARD_SIZE && size.height <= MAX_BOARD_SIZE);
		board_size = size;
	}
	Size AiManager::get_board_size() const {
		return board_size;
	}

	const std::vector<Point>& AiManager::get_undo_stack() const {
		return undo_stack;
	}
}