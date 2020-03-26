#include "stdafx.h"
#include "log_queue.h"

namespace ai {

	bool LogQueue::has_logs() {
		lk.lock();
		bool res = !logs.empty();
		lk.unlock();
		return res;
	}

	std::vector<std::string> LogQueue::get_logs() {
		lk.lock();
		std::vector<std::string> res;
		swap(res, logs);
		lk.unlock();
		return res;
	};

	void LogQueue::log(std::string log) {
		lk.lock();
		logs.push_back("L " + log);
		lk.unlock();
	};
	void LogQueue::msg(std::string log) {
		lk.lock();
		logs.push_back("MSG " + log);
		lk.unlock();
	};

}