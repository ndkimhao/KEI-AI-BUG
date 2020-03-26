#pragma once
#include "stdafx.h"

namespace logman {

	class LogManager {
	public:
		LogManager();
		void write_log(std::string msg);
		void write_free_mem();
		static LogManager inst;

	private:
		std::ofstream log_file{ "ksh.csv", std::ofstream::out | std::ofstream::app };
		int cur_no = 0;
		uint32_t cur_crc = 0;

		static uint32_t crc_table[256];
		void write_sys_spec();
	};

}