#pragma once

namespace ksh {

	template<typename T>
	inline bool mini(T &a, T b) { return b < a ? a = b, true : false; }
	template<typename T>
	inline bool maxi(T &a, T b) { return a < b ? a = b, true : false; }

	inline PCell next_cell(const PCell p, int i, int s) {
		return (PCell)((char*)p + (i*s));
	}
	inline PCell prev_cell(const PCell p, int i, int s) {
		return (PCell)((char*)p - (i*s));
	}
	inline void next_cell_ptr(PCell &p, int i, int s) {
		p = (PCell)((char*)p + (i*s));
	}
	inline void prev_cell_ptr(PCell &p, int i, int s) {
		p = (PCell)((char*)p - (i*s));
	}
	inline PCell next_cell_dir(const PCell p, int s, int dir[9]) {
		return (PCell)((char*)p + dir[s]);
	}

}