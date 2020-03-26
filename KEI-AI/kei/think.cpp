#include "stdafx.h"
#include "AI.h"

namespace ksh {

	void AI::do_thinking()
	{
		show_msg("zzz");

		// Reset trạng thái
		ai_terminate_reason = 0;
		// start_time = GetTickCount();
		result_move = hold_move = cell_with_best_eval = 0;

		// Đi nước dễ (opening, đường 4...)
		find_easy_move();
		if (result_move) {
			show_msg();
			return;
		}

		// Xóa toàn bộ cells ra khỏi [win_moves_path]
		for (PCell p = board_beg; p < board_end; p++)
			p->ptr_in_win_moves_path = win_moves_path + WIN_MOVES_SIZE;
		ptr_win_moves_path = win_moves_path;

		// Tìm đường thắng bằng TSS
		if (set_result_move(dfs_win_moves(0))) {
			show_msg("I win :> Can you see it ?");
			return;
		}

		// Reset trạng thái cho module search
		finish_attacking = finish_defending = finish_defending_x2 = finish_testing = wary_attack = wary_defend = false;
		opp_win_4 = dfs_win_moves(1);
		if (opp_win_4) {
			msg("Haha you can't see it :D");
			set_result_move(opp_win_4);
		}
		else search_cell_best_eval();
		assert(result_move);

		// Bắt đầu tìm kiếm theo chiều sâu
		int total_searched_nodes = 0;
		int total_cache_hit = 0;
		for (int i = 4; i <= 75; i += 2) {
			global_search_depth = i;
			cnt_searched_nodes = 0;
			cnt_cache_hit = 0;

			// Chạy hàm tìm kiếm
#pragma warning(suppress: 28159)
			//DWORD t0 = GetTickCount();
			search_for_move();
#pragma warning(suppress: 28159)
			//DWORD t1 = GetTickCount();

			total_searched_nodes += cnt_searched_nodes;
			total_cache_hit += cnt_cache_hit;
			log(Format("d=%d, n=%d, c=%d", global_search_depth, cnt_searched_nodes, cnt_cache_hit));

			/*if (ai_terminate_reason
				|| t1 + 3 * (t1 - t0) >= stop_time) break;*/
#pragma warning(suppress: 28159)
			DWORD t = GetTickCount();

			if (ai_terminate_reason) break;
			if (t + 20 >= stop_time) break;
			if (i >= 20 && t + 100 >= stop_time) break;
		}
		if (ai_terminate_reason == 2) global_search_depth -= 2;

		int depth = std::min(std::max(global_search_depth, 0), 100);
		log(Format("FINAL d=%d, total=%d, c=%d",
			depth, total_searched_nodes, total_cache_hit));
		//log(Format("attack = %d, defend = %d, defend1 = %d", finish_attacking, finish_defending, finish_defending_x2));
		//log(Format("test = %d, wary_attack = %d, wary_defend = %d", finish_testing, wary_attack, wary_defend));

		if (ai_terminate_reason == 3)
			msg("WILL I win ?!");
		else {
			if (global_ab_res > 0) msg("Yolo =]] Can you see it?");
			else if (global_ab_res < 0) msg("Kim So Hyun <3");
			else msg("Hmm...");
		}
		show_msg();
	}

	// Trả về nước đi tốt nhất, sau khi suy nghĩ xong
	PCell AI::get_result_move() {
		return result_move;
	}

}