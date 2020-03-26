#include "stdafx.h"
#include "AI.h"

namespace ksh {

	// Thêm những ô có level 2 vào [cur_moves] để xét
	int AI::impl_ab_add_level2_cells(int player1, int &must_attack, int log_win_moves,
		ksh::PCell * &ptr_cur_moves, int attacking_player, ksh::PCell * ptr_cur_moves_old, bool &retflag)
	{
		retflag = true;
		PCell p = head_eval_level[2][player1];
		if (p) {
			must_attack++;
			do {
				if (!log_win_moves && p->eval[player1].eval_sum >= 523) {
					// [player1] có đường 3 ko bị chặn => thấy cmnr
					if (!cur_search_depth) global_best_move = p;
					return 999 - cur_search_depth;
				}
				if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;

				int p_eval = p->eval[attacking_player].eval_sum;
				PCell *q;
				// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
				for (q = ptr_cur_moves++;
					q > ptr_cur_moves_old && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
					q--)	*q = *(q - 1);
				*q = p;
				p = p->eval[player1].ptr_next;
			} while (p);
		}
		retflag = false;
		return {};
	}

	// Kiểm tra đường thắng ngay của [player2]
	int AI::impl_ab_check_win_p2(int player2, int player1, ksh::PCell * ptr_cur_moves,
		int log_win_moves, const ksh::PCell &last_cell,
		const ksh::PCell &start_from, int is_attacking, bool &retflag)
	{
		retflag = true;
		PCell p = head_eval_level[3][player2];
		if (p) {
			// Đi ô [p]
			p->piece = player1 + 1; eval_cell(p);
			hash_tbl.add_piece(p->x, p->y, player1 + 1);
			cur_search_depth++;
			// Thử
			int ab_res = -impl_search_alpha_beta(player2, ptr_cur_moves,
				log_win_moves, last_cell, start_from, is_attacking ^ 1);
			// Quay lui
			hash_tbl.remove_piece(p->x, p->y, player1 + 1);
			p->piece = 0; eval_cell(p);
			cur_search_depth--;

			assert(cur_search_depth > 0);
			if (log_win_moves && ab_res && ((ab_res > 0) == is_attacking)) append_to_win_moves(p);
			return ab_res;
		}
		retflag = false;
		return {};
	}

	// Kiểm tra đường thắng ngay của [player1]
	int AI::impl_ab_check_win_p1(int player1, int log_win_moves, int is_attacking, bool &retflag)
	{
		retflag = true;
		PCell p = head_eval_level[3][player1];
		// Nếu [player1] đã có đường 4 => ngon
		if (p) {
			assert(cur_search_depth > 0);
			if (log_win_moves && is_attacking) append_to_win_moves(p);
			return 1000 - cur_search_depth; // Bố thắng luôn cmnr
		}
		retflag = false;
		return {};
	}

	// Tìm những nước đi phòng thủ cho [player1]
	void AI::impl_ab_find_defensive_moves(int player2, int &must_defend, int must_attack,
		ksh::PCell &p_defend, ksh::PCell * &ptr_cur_moves,
		int attacking_player, ksh::PCell * ptr_moves_defend_x1)
	{
		for (PCell p = head_eval_level[2][player2]; p; p = p->eval[player2].ptr_next) {
			if (p->eval[player2].eval_sum >= 270 + 150) {
				// [player2] có đường 3 => buộc phải chặn
				if (!must_defend) must_defend = 1;
				if (p->eval[player2].eval_sum >= 523) must_defend = 2;
			}
			else {
				// Nếu đang tấn công và [player2] chỉ có đường 2 => kệ m* nó
				if (must_attack) continue;
			}
			p_defend = p;
			// Overflow guard
			if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;

			// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
			int p_eval = p->eval[attacking_player].eval_sum;
			PCell *q;
			for (q = ptr_cur_moves++;
				q > ptr_moves_defend_x1 && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
				q--) *q = *(q - 1);
			*q = p;
		}
	}


	// Tìm nước tấn công cho [player1]
	void AI::impl_ab_find_attacking_moves(int is_attacking, const ksh::PCell &p_defend,
		const ksh::PCell &start_from, int must_defend, int player1, int &can_attack,
		ksh::PCell * &ptr_cur_moves, int attacking_player, ksh::PCell * ptr_moves_defend_x2)
	{
		if (is_attacking || alpha_beta_search_attacking || !p_defend) {
			if (start_from && !must_defend) {
				// Tìm đường tấn công sau nước vừa đi của [player1]
				for (int i = 0; i < 8; i++) {
					int diroff = direction_offset[i];
					PCell p = start_from; next_cell_ptr(p, 1, diroff);
					for (int j = 0; j < 4 && (p->piece != 3); j++, next_cell_ptr(p, 1, diroff)) {
						if (p->eval[player1].eval_level == 1) {
							can_attack = 1;
							if (!must_defend || p->eval[player1].eval_sum >= 270) {
								// Overflow guard
								if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;
								// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
								int p_eval = p->eval[attacking_player].eval_sum;
								PCell *q;
								for (q = ptr_cur_moves++;
									q > ptr_moves_defend_x2 && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
									q--)	*q = *(q - 1);
								*q = p;
							}
						}
					}
				}
			}
			else {
				for (PCell p = head_eval_level[1][player1]; p; p = p->eval[player1].ptr_next) {
					if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;
					can_attack = 1;
					if (!must_defend || p->eval[player1].eval_sum >= 270) {
						// Overflow guard
						if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;
						// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
						int p_eval = p->eval[attacking_player].eval_sum;
						PCell *q;
						for (q = ptr_cur_moves++;
							q > ptr_moves_defend_x2 && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
							q--) *q = *(q - 1);
						*q = p;
					}
				}
			}
		}
	}

	// Tìm những nước phòng thủ (x2)
	void AI::impl_ab_find_defensive_moves_x2(const ksh::PCell &p_defend, int player2,
		ksh::PCell * &ptr_cur_moves, int attacking_player, ksh::PCell * &ptr_moves_defend_x2,
		int is_attacking, const ksh::PCell &last_cell, int must_attack)
	{
		if (p_defend) {
			// Tìm những ô level 1 xung quang ô [player2] vừa đi
			for (int i = 0; i < 8; i++) {
				int diroff = direction_offset[i];
				PCell p = p_defend; next_cell_ptr(p, 1, diroff);
				for (int j = 0; j < 4 && (p->piece != 3); j++, next_cell_ptr(p, 1, diroff)) {
					if (p->eval[player2].eval_level == 1) {
						// Overflow guard
						if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;

						// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
						int p_eval = p->eval[attacking_player].eval_sum;
						PCell *q;
						for (q = ptr_cur_moves++;
							q > ptr_moves_defend_x2 && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
							q--)	*q = *(q - 1);
						*q = p;
					}
				}
			}
		}
		else if (!is_attacking) {
			for (PCell p = head_eval_level[1][player2]; p; p = p->eval[player2].ptr_next) {
				if ((!last_cell || distance(p, last_cell) < 7)
					&& (!must_attack || p->eval[player2].eval_sum >= 270)) {
					// Overflow guard
					if (ptr_cur_moves == cur_moves + CUR_MOVES_SIZE) break;

					// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
					int p_eval = p->eval[attacking_player].eval_sum;
					PCell *q;
					for (q = ptr_cur_moves++;
						q > ptr_moves_defend_x2 && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
						q--)	*q = *(q - 1);
					*q = p;
				}
			}
		}
		ptr_moves_defend_x2 = ptr_cur_moves;
	}

	// Xét những ô có evaluation thấp (level 1 -> 2)
	void AI::impl_ab_find_low_eval_moves(ksh::PCell * &ptr_cur_moves, const ksh::PCell &start_from,
		const ksh::PCell &last_cell, const ksh::PCell &p_defend, int is_attacking,
		int player1, ksh::PCell * ptr_cur_moves_old, int attacking_player)
	{
		PCell *ptr_moves_low_eval = ptr_cur_moves;
		if (!start_from && !last_cell && !p_defend &&
			(ptr_win_moves_path == win_moves_path || !is_attacking || player1)) {
			int n = -3 + int(global_search_depth*2) - int(ptr_cur_moves - ptr_cur_moves_old);
			if (n > 0) {
				for (PCell p = board_beg; p < board_end; p++) {
					int p_eval = p->eval[attacking_player].eval_sum;
					if (p_eval > 7 && p_eval < 150) {
						assert(p->piece == 0 && p->eval[attacking_player].eval_level == 0);
						// Thêm [p] vào mảng [cur_moves], sort theo [eval_sum]
						PCell *q;
						for (q = ptr_cur_moves;
							q > ptr_moves_low_eval && (*(q - 1))->eval[attacking_player].eval_sum < p_eval;
							q--)	*q = *(q - 1);
						*q = p;
						if (n > 0) ptr_cur_moves++, n--;
					}
				}
			}
		}
	}

	// Xử lý lần lượt các nước đi được chuẩn bị trong mảng [cur_moves]
	int AI::impl_process_cur_moves(int &best_ab, ksh::PCell * ptr_cur_moves_old, 
		ksh::PCell * ptr_cur_moves, int is_attacking, int player1, int player2, 
		int log_win_moves, const ksh::PCell &last_cell, ksh::PCell * ptr_moves_defend_x1, 
		ksh::PCell * ptr_moves_defend_x2, const ksh::PCell &start_from, bool &retflag)
	{
		retflag = true;
		best_ab = -SHORT_INF;
		PCell *ptr_win_old = ptr_win_moves_path;
		for (PCell *t = ptr_cur_moves_old; t < ptr_cur_moves; t++) {
			PCell p = *t;
			if (!cur_search_depth && opp_win_4 && is_attacking && is_not_in_win_moves_path(p)) continue;

			// Thử nước đi
			p->piece = player1 + 1; eval_cell(p);
			hash_tbl.add_piece(p->x, p->y, player1 + 1);
			cur_search_depth++;
			// Đệ quy...
			int ab_res = -impl_search_alpha_beta(player2, ptr_cur_moves, log_win_moves, last_cell,
				(t >= ptr_moves_defend_x1 && t < ptr_moves_defend_x2) ? start_from : p, is_attacking ^ 1);
			// Quay lui
			hash_tbl.remove_piece(p->x, p->y, player1 + 1);
			p->piece = 0; eval_cell(p);
			cur_search_depth--;

			// Tìm ra đường thắng cmnr
			if (ab_res > 0) {
				if (!cur_search_depth) global_best_move = p;
				if (!is_attacking) ptr_win_moves_path = ptr_win_old;
				else if (log_win_moves) append_to_win_moves(p);
				return ab_res;
			}
			// Đéo ra
			if (ab_res == 0) {
				if (!is_attacking) {
					ptr_win_moves_path = ptr_win_old; // Xóa đường thắng đang tính
					if (!cur_search_depth) global_best_move = p;
					return ab_res;
				}
				best_ab = ab_res;
			}
			// if(ab_res < 0)
			else {
				if (ab_res >= best_ab) {
					// Ăn loz rồi :3 đéo có win đâu :3
					if (log_win_moves && !is_attacking) append_to_win_moves(p);
					if (!cur_search_depth) {
						// Kéo dài ván đấu, hi vọng thằng kia chơi ngu
						// hoặc thằng kia ăn BUG =]]]
						// Đi vào chỗ ngon của thằng kia
						if (ab_res > best_ab 
							|| p->eval[player2].eval_sum > global_best_move->eval[player2].eval_sum) 
							global_best_move = p;
					}
					best_ab = ab_res;
				}
			}
		}
		retflag = false;
		return {};
	}

	// Trả về khoảng cách lớn nhất theo chiều dọc hoặc ngang giữa p1 và p2
	int AI::distance(PCell p1, PCell p2)
	{
		return std::max(abs(p1->x - p2->x), abs(p1->y - p2->y));
	}

	// Kiểm tra p có đang nằm trong list [win_moves_path] hay không
	bool AI::is_not_in_win_moves_path(PCell p)
	{
		return !p || p->ptr_in_win_moves_path >= ptr_win_moves_path || *p->ptr_in_win_moves_path != p;
	}
}