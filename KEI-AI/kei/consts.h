#pragma once

namespace ksh {
	/*##### Evaluation Module ####*/
	extern const signed char opening_moves[];

	/*##### Alpha-beta search module ####*/

	/*##### Other ####*/

	// Border chừa ra khi đi opening moves
	const int BORDER_LIMIT = 4;

	// Cái tên nói lên tất cả
	const int INF = int(2e9);
	const int SMALL_INF = int(1e7);
	const int SHORT_INF = int(32000);
	const int CUR_MOVES_SIZE = 8192;
	const int WIN_MOVES_SIZE = 1024;

}