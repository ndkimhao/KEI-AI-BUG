#pragma once
#include "consts.h"
#include "types.h"
#include "helpers.h"
#include "server\log_queue.h"
#include "HashTable.h"

namespace ksh {

	class AI {
	private:
		/*##### Global ####*/

		// Con trỏ tới mảng được cấp phát động
		PCell board = 0;
		// Con trỏ tới vùng hợp lệ trong bảng [board_beg, board_end)
		PCell board_beg, board_end;
		// Thằng kia đi vào đây là ăn c*t
		PCell opp_win_4;
		// Nước đi tốt nhất cho tới hiện tại
		PCell result_move;
		// Nước đi tốt nhất theo cách đánh giá bằng eval
		PCell cell_with_best_eval;
		// Nước đi cuối cùng được thực hiện trên bảng (sau khi undo, last_move = 0)
		PCell last_move;
		PCell hold_move;
		// Lưu tạm các nước đi trong hàm đệ quy
		PCell cur_moves[CUR_MOVES_SIZE];

		// Độ sâu tìm kiếm hiện tại (tăng mỗi lần 2 nấc)
		int global_search_depth;

		// Kích thước bảng
		int width, height;
		// height+2, optimize speed :v
		int	height2;
		// Số nước đi đã được thực hiện
		int	global_moves_cnt;
		// Số ô bị không đi được
		int	global_blocked_cells_cnt;
		// Offset (bytes) theo 8 hướng, trong array [board]
		int	direction_offset[9];

		/*##### Evaluation Module ####*/

		// Head của linked list các ô theo level, cho 2 player
		PCell head_eval_level[4][2];
		// Chứa toàn bộ đường thắng đang được tính toán
		PCell win_moves_path[WIN_MOVES_SIZE];
		PCell* ptr_win_moves_path;
		// Nước đi đầu tiên của đường thắng, cho 2 player
		PCell win_moves[2];
		PCell global_best_move;

		// Điểm evaluation của win_moves_path
		int win_moves_path_eval[WIN_MOVES_SIZE];
		// Độ dài đường thắng của 2 thằng
		int win_moves_length[2];
		// Tổng điểm evaluation của 2 player (trên tất cả ô)
		int eval_global_sum[2];

		// Độ sâu của hàm đệ quy
		int	cur_search_depth = 0;

		/*##### Search Module ####*/

		// Flags theo dõi quá trình tìm kiếm
		// Từng công việc có thể thực hiện riêng lẻ, linh động tùy vào time limit
		bool finish_attacking, finish_defending, finish_defending_x2, finish_testing;
		// Flags thay đổi chiến thuật tùy vào tình hình quân trên bảng
		bool wary_attack, wary_defend;
		// Đã đạt tới độ sâu cần thiết (hoàn thành 1 task), nhưng vẫn có thể tìm tiếp
		bool reached_depth;

		// Tìm nước tấn công cho cả 2 player, trong khi chạy alpha-beta
		bool alpha_beta_search_attacking;
		// Khi set giá trị khác 0 thì AI sẽ stop ngay lập tức
		int ai_terminate_reason = 0;

		// Đếm số lượng nodes đã explore trong hàm alpha-beta
		int cnt_searched_nodes;
		// Đo hiệu quả của cache table
		int cnt_cache_hit;

		// Kết quả tìm alpha-beta cúng cuồi
		int global_ab_res;

		// Hash table cache những trạng thái đã tìm kiếm rồi
		HashTable hash_tbl;

		/*##### init.cpp #####*/

		// Reset tạo lại các biến
		void init_reset_mem();
		// Khởi tạo bộ nhớ cho mảng [board]
		void init_alloc_board();
		// Khởi tạo mảng [direction_offset]
		void init_direction_offset();
		// Điền giá trị ban đầu cho mảng [board]
		void init_board();

		/*##### pattern.cpp #####*/

		static const int PIECES_PATTERN_SIZE = 1 << (2 * 9);
		// Evaluation score cho toàn bộ pattern 9 quân, cho 2 player
		TPatternEvaluation pieces_patterns[PIECES_PATTERN_SIZE];

		// Tính evaluation score cho toàn bộ pattern 9 quân
		void calc_eval_pattern();

		/*##### utils.cpp #####*/

		// Ghi ra log (để debug)
		void log(std::string msg);
		// Thông báo chuẩn bị hiển thị cho người dùng
		std::string pending_msg;
		// Gán cho [pending_msg]
		void msg(std::string msg);
		// Hiển thị [pending_msg] cho người dùng
		void show_msg();
		// Hiển thị thông báo cho người dùng
		void show_msg(std::string msg);

		/*##### eval_cell.cpp #####*/

		// Xóa eval ở ô [p0]
		void clear_cell_eval(PCell p0);
		// Cập nhật eval cho các ô eval theo hướng dir
		void eval_dir(PCell p0, int dir);
		// Cập nhật eval cho các ô theo 4 hướng ngang, dọc và 2 đường chéo
		void eval_cell(PCell p0);

		/*##### move.cpp #####*/

		// Đi nước đầu (AI đi trước)
		void do_first_move();
		// Đi nước thứ 3 (AI đi trước)
		void do_third_move();
		// Tìm các nước đi bắt buộc, hoặc có sẵn trong database
		void find_easy_move();
		// Tìm HCN nhỏ nhất chứa toàn bộ quân cờ
		RectRegion find_active_rect_region();
		// Tìm nước đi có trong opening moves database
		void find_opening_move();
		// Gán nước đi tốt nhất cho tới hiện tại
		bool set_result_move(PCell p);
		// Thực hiện nước đi thật sự tại ô [p]
		bool do_move(PCell p, int piece);

		/***### eval_search.cpp #####*/

		// Tính điểm eval thứ cấp cho ô [p], cho [player]
		int calc_sec_eval(int player, PCell p);
		// Đếm số hướng bị chặn (ko triển quân dc nữa), cho 2 player
		// Return max = 4 hướng (ngang, dọc, 2 đường chéo)
		void count_blocked_4dir(const ksh::PCell& p0, int player1, int& d1, int& d2);
		// Đếm số quân xung quanh 8 ô [p0], cho 2 player
		void count_pieces_8dir(const ksh::PCell& p0, int player1, int& c1, int& c2);
		// Tính điểm eval thứ cấp cho ô [p] cho 2 player
		int calc_sec_eval(PCell p);

		// Search ô cho eval tốt nhất, sau 6 nước
		void search_cell_best_eval();
		// Thủ khi đối thủ đi nước xa vùng đang chơi
		void defend_isolated_move();
		// Tìm tổng eval tốt nhất sau 6 nước
		int dfs_eval(int player1);
		// Đếm quân trong khu vực 5x5 ô, [q] là ô giữa
		// Mảng [count] phải được reset về 0 sẵn
		void count_pieces_5x5(const ksh::PCell& q, int  count[4]);
		// Tìm ô có eval cao nhất, nếu ô cao nhất có eval < 0 thì trả về ptr NULL
		PCell find_cell_best_eval();

		/*##### win_move.cpp #####*/

		// Tính mảng [win_moves_path_eval], là điểm eval cho các nước trong [win_moves_path]
		void calc_eval_win_moves();
		// Thêm [p] vào [win_moves]
		void append_to_win_moves(PCell p);
		// Biến thể của TSS, chỉ xét tới đường 4
		// Kiếm đường thắng 100% bằng cách đi liên tục các đường 4
		// Và nước cuối tạo ra được 2 đường 4 cùng lúc
		PCell dfs_win_moves(int player1);
		// Implement của dfs_win_moves, chạy sau khi lọc ra cell level 1,2
		// Đi từ ô [start_from] đầu tiên
		PCell dfs_win_moves(int player1, PCell start_from);
		// Tìm tất cả đường ba của [player1], cho vào [cur_moves]
		void find_all_triplets(int player1, ksh::PCell*& ptr_cur_moves);

		/*##### utils.cpp #####*/

		// Trả về [PCell] tại ô (x,y), 0-based
		PCell cell(int x, int y);
		// Trả về số random [0..max_val)
		unsigned random(unsigned max_val);
		// Kiểm tra [PCell] có phải là pointer hợp lệ
		bool is_valid_cell(PCell p) const;

		/*##### search.cpp #####*/

		// Hàm entry cho thuật tìm kiếm nước đi tiếp theo bằng thuật alpha-beta
		void search_for_move();
		// Thử tìm nước đi phòng thủ
		void search_for_defensive_move(int& ab_res);
		// Thử đường thắng của đối thủ, nếu ta không có nước đi tốt
		void search_for_opponent_move(int& ab_res);
		// Thực hiện tấn công
		void search_for_attacking_move(int& ab_res, bool& retflag);
		// Thử các nước đi từ [win_moves_path] để phòng thủ, bằng phương pháp eval
		int search_for_defensive_move_by_eval();

		/*##### alpha_beta.cpp #####*/

		// Hàm entry cho hàm [impl_search_alpha_beta]
		int search_alpha_beta(bool wary, int is_attacking, int player1,
			int log_win_moves, PCell start_from = nullptr);
		// Kiểm tra xem đã chạy hết time limit chưa
		void check_for_timeout();
		// Hàm main để chạy alpha-beta search
		int impl_search_alpha_beta(int player1, PCell* ptr_cur_moves, int log_win_moves,
			PCell start_from, PCell last_cell, int is_attacking);
		int impl_search_alpha_beta_uncached(int player1, PCell* ptr_cur_moves, int log_win_moves,
			PCell start_from, PCell last_cell, int is_attacking);

		/*##### imlp_alpha_beta.cpp #####*/

		// Thêm những ô có level 2 vào [cur_moves] để xét
		int impl_ab_add_level2_cells(int player1, int& must_attack, int log_win_moves,
			ksh::PCell*& ptr_cur_moves, int attacking_player,
			ksh::PCell* ptr_cur_moves_old, bool& retflag);
		// Kiểm tra đường thắng ngay của [player2]
		int impl_ab_check_win_p2(int player2, int player1, ksh::PCell* ptr_cur_moves, int log_win_moves,
			const ksh::PCell& last_cell, const ksh::PCell& start_from, int is_attacking, bool& retflag);
		// Kiểm tra đường thắng ngay của [player1]
		int impl_ab_check_win_p1(int player1, int log_win_moves, int is_attacking, bool& retflag);
		// Tìm nước tấn công cho [player1]
		void impl_ab_find_attacking_moves(int is_attacking, const ksh::PCell& p_defend,
			const ksh::PCell& start_from, int must_defend, int player1, int& can_attack,
			ksh::PCell*& ptr_cur_moves, int attacking_player, ksh::PCell* ptr_moves_defend_x2);
		// Tìm những nước phòng thủ (x2)
		void impl_ab_find_defensive_moves_x2(const ksh::PCell& p_defend, int player2,
			ksh::PCell*& ptr_cur_moves, int attacking_player, ksh::PCell*& ptr_moves_defend_x2,
			int is_attacking, const ksh::PCell& last_cell, int must_attack);
		// Tìm những nước đi phòng thủ cho [player1]
		void impl_ab_find_defensive_moves(int player2, int& must_defend, int must_attack,
			ksh::PCell& p_defend, ksh::PCell*& ptr_cur_moves,
			int attacking_player, ksh::PCell* ptr_moves_defend_x1);
		// Xét những ô có evaluation thấp (level 1 -> 2)
		void impl_ab_find_low_eval_moves(ksh::PCell*& ptr_cur_moves, const ksh::PCell& start_from,
			const ksh::PCell& last_cell, const ksh::PCell& p_defend, int is_attacking,
			int player1, ksh::PCell* ptr_cur_moves_old, int attacking_player);
		// Xử lý lần lượt các nước đi được chuẩn bị trong mảng [cur_moves]
		int impl_process_cur_moves(int& best_ab, ksh::PCell* ptr_cur_moves_old,
			ksh::PCell* ptr_cur_moves, int is_attacking, int player1, int player2,
			int log_win_moves, const ksh::PCell& last_cell, ksh::PCell* ptr_moves_defend_x1,
			ksh::PCell* ptr_moves_defend_x2, const ksh::PCell& start_from, bool& retflag);
		// Trả về khoảng cách lớn nhất theo chiều dọc hoặc ngang giữa p1 và p2
		int distance(PCell p1, PCell p2);
		// Kiểm tra p có đang nằm trong list [win_moves_path] hay không
		bool is_not_in_win_moves_path(PCell p);

	public:
		/*##### init.cpp #####*/

		// Khởi tạo AI
		void init(int w = 15, int h = 15);

		/*##### move.cpp #####*/

		// Thực hiện nước đi thật sự tại ô (x,y), 0-based
		bool do_move(int x, int y, int piece);
		// Undo nước đi tại (x,y)
		bool undo_move(int x, int y);

		/*##### utils.cpp #####*/

		// Bắt buộc dừng AI ngay lập tức
		void abort_thinking();

		/*##### ai_move.cpp #####*/

		// Hàm entry bắt đầu thuật toán, sau khi chạy xong
		// kết quả nằm trong [result_move]
		void do_thinking();
		// Trả về nước đi tốt nhất, sau khi suy nghĩ xong
		PCell get_result_move();

		/*##### Public Variables #####*/

		// Thời điểm phải kết thúc suy nghĩ, được set từ bên ngoài
		DWORD stop_time;
		// Time limit cho một lượt suy nghĩ
		int time_limit_turn;
		// Giới hạn ram được sử dụng
		long long mem_limit;
		// Pointer tới [ai::LogQueue], để đưa log, message cho người dùng
		ai::LogQueue* logs = nullptr;
	};

}