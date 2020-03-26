#pragma once
#include "stdafx.h"
#include "kei\AI.h"
#include "log_queue.h"
#include "log_manager.h"

namespace ai {

	enum class AiVariant {
		Invalid = 0, KeiAi = 1
	};

	enum class Cell {
		Empty = 0, Ai = 1, Human = 2, Invalid = 3
	};

	enum class AiTurn {
		Idle = 0, Thinking = 1, ResultReady = 2
	};

	struct Size { int width, height; };
	struct Point {
		int x, y;
		bool operator ==(const Point &b) const {
			return x == b.x && y == b.y;
		}
	};

	class AiManager {
	public:
		void set_time_turn(int timeout);
		int get_time_turn() const;
		void set_max_memory(long long maxmem);
		long long get_max_memory() const;
		void set_board_size(Size size);
		Size get_board_size() const;
		void new_match(AiVariant vari);

		AiTurn get_ai_turn();
		int remaining_time();

		bool turn_human(Point move);
		void start_turn_ai();
		Point get_result_move();
		int undo_remain = 0;
		std::pair<Point, Point> undo();
		Point get_last_move();
		const Point INVALID_POINT = { -1,-1 };
		const std::pair<Point, Point> INVALID_UNDO = { INVALID_POINT,INVALID_POINT };

		LogQueue logs;
		Cell has_win_moves(Point last);
		std::vector<ai::Point> win_moves;

		void abort_thinking();
		void determine_max_memory();
		AiManager();
		~AiManager();

		const std::vector<Point>& get_undo_stack() const;

	private:
		static const int MAX_BOARD_SIZE = 24;

		void delete_ai();
		void new_ai();
		void init_ai();
		void init_board();
		void set_stop_time();
		Cell& board(int x, int y);

		void _undo_at(Point p);
		bool is_valid_move(Point p);

		static const long long reserved_memory = 2 * 1000 * 1000 * 1000;
		int time_turn = 15000;
		long long max_memory = 1024 * 1024 * 1024;
		DWORD stop_time = 0;

		AiTurn ai_turn = AiTurn::Idle;
		Size board_size = { 15,15 };
		Cell _board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
		Point result_move = { 0,0 };

		ksh::AI *kei_ai = nullptr;
		
		AiVariant variant = AiVariant::Invalid;
		std::vector<ai::Point> undo_stack;
	};
}