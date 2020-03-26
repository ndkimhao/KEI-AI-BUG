#pragma once
#include "stdafx.h"

namespace ai {

	class LogQueue {
	private:
		std::vector<std::string> logs;
		std::mutex lk;

	public:
		std::vector<std::string> get_logs();
		void log(std::string log);
		void msg(std::string log);
		bool has_logs();
	};

}