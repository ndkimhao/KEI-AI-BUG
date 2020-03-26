#pragma once
#include "stdafx.h"
#include "helpers.h"
#include "types.h"

namespace ksh {
	typedef unsigned long long ull;

	class HashTable {
	public:
		struct Record {
			ull hash_b;
			short result;
			unsigned int no;
			//bool reached_depth;
#ifdef KSH_DEBUG
			//int player1;
			//PCell *ptr_cur_moves; 
			//bool is_attacking;
			//int dep, global_dep;
			//std::string board;
#endif
		};

		HashTable();
		~HashTable();
		void reserve_memory(size_t mem_size);
		void add_piece(int x, int y, int p);
		void remove_piece(int x, int y, int p);
		void add_info(PCell p1, PCell p2, bool b);
		void remove_info(PCell p1, PCell p2, bool b);
		void reset_state();
		void get_result(bool &ok, Record* &r);

	private:
		static const int HASH_SIZE = 1 << 10;
		static ull HASH_CONST_A[HASH_SIZE], HASH_CONST_B[HASH_SIZE];

		static const int FORWARD_FIND = 10;

		Record *table;
		size_t table_size = 0;

		ull random_ull();

		ull cur_hash_a, cur_hash_b;
		unsigned int cur_no = 0;
		size_t last_mem = 0;
	};

}