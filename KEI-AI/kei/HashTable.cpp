#include "stdafx.h"
#include "types.h"
#include "consts.h"
#include "HashTable.h"

namespace ksh {
	typedef unsigned long long ull;

	namespace prime {
		ull power(ull x, ull y, ull p)
		{
			ull res = 1;
			x = x % p;
			while (y > 0)
			{
				if (y & 1) res = (res*x) % p;
				x = (x*x) % p;
				y = y >> 1;
			}
			return res;
		}

		bool milllerTest(ull d, ull n)
		{
			ull a = 2 + rand() % (n - 4);
			ull x = power(a, d, n);
			if (x == 1 || x == n - 1) return true;
			while (d != n - 1)
			{
				x = (x * x) % n;
				d *= 2;
				if (x == 1) return false;
				if (x == n - 1) return true;
			}
			return false;
		}

		bool isPrime(ull n, int k)
		{
			if (n <= 1 || n == 4)  return false;
			if (n <= 3) return true;

			ull d = n - 1;
			while (d % 2 == 0) d /= 2;

			for (int i = 0; i < k; i++)
				if (!milllerTest(d, n)) return false;

			return true;
		}
	}

	ull HashTable::HASH_CONST_A[HASH_SIZE];
	ull HashTable::HASH_CONST_B[HASH_SIZE];

	ull HashTable::random_ull()
	{
		static ull seed = GetTickCount64();
		seed = seed * 1696969 + 69696969696969ull;
		ull h1 = seed;
		seed = seed * 296969 + 96969696969696ull;
		return h1 * seed;
	}

	HashTable::HashTable() {
		static bool initialized = false;
		static std::mutex lock;
		std::lock_guard<std::mutex> g{ lock };
		if (initialized) return;
		else initialized = true;
		for (int i = 0; i < HASH_SIZE; i++) {
			HASH_CONST_A[i] = random_ull();
			HASH_CONST_B[i] = random_ull();
		}
	}
	HashTable::~HashTable() {
		if (table != nullptr) delete[] table;
	}

	void HashTable::reserve_memory(size_t mem_size) {
		reset_state();
		size_t n = mem_size / sizeof(Record);
		while (!prime::isPrime(n, 5)) n--;
		if (n != table_size) {
			if (table != nullptr) delete[] table;
			size_t n_alloc = n + FORWARD_FIND;
			table = new Record[n_alloc];
			memset(table, 0, sizeof(Record)*n_alloc);
			table_size = n;
		}
	}

	void HashTable::reset_state() {
		cur_hash_a = random_ull();
		cur_hash_b = random_ull();
		cur_no++;
	}

	void HashTable::add_piece(int x, int y, int p) {
		assert(0 <= x && x <= 15 && 0 <= y && y <= 15 && 0 <= p && p <= 3);
		int offset = x + (y << 4) + (p << 8);
		cur_hash_a += HASH_CONST_A[offset];
		cur_hash_b += HASH_CONST_B[offset];
	}
	void HashTable::remove_piece(int x, int y, int p) {
		assert(0 <= x && x <= 15 && 0 <= y && y <= 15 && 0 <= p && p <= 3);
		int offset = x + (y << 4) + (p << 8);
		cur_hash_a -= HASH_CONST_A[offset];
		cur_hash_b -= HASH_CONST_B[offset];
	}

	void HashTable::add_info(PCell p1, PCell p2, bool b) {
		if (p1) add_piece(p1->x, p1->y, 2);
		if (p2) add_piece(p2->x, p2->y, 3);
		add_piece(15, 0, b);
	}
	void HashTable::remove_info(PCell p1, PCell p2, bool b) {
		if (p1) remove_piece(p1->x, p1->y, 2);
		if (p2) remove_piece(p2->x, p2->y, 3);
		remove_piece(15, 0, b);
	}

	void HashTable::get_result(bool &out_ok, Record* &out_r) {
		bool ok; Record *r = &table[cur_hash_a % table_size];
		for (int i = 0; i < FORWARD_FIND; i++, r++) {
			ok = r->hash_b == cur_hash_b;
			if (ok || r->no != cur_no) break;
		}
		if (r->no != cur_no) ok = false;
		r->hash_b = cur_hash_b;
		r->no = cur_no;
		out_ok = ok, out_r = r;
	}

}