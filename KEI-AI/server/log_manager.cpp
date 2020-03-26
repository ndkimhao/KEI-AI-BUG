#include "stdafx.h"
#include "log_manager.h"

namespace logman {
	namespace crc32
	{
		void generate_table(uint32_t(&table)[256])
		{
			uint32_t polynomial = 0xEDB88320;
			for (uint32_t i = 0; i < 256; i++)
			{
				uint32_t c = i;
				for (size_t j = 0; j < 8; j++)
				{
					if (c & 1) {
						c = polynomial ^ (c >> 1);
					}
					else {
						c >>= 1;
					}
				}
				table[i] = c;
			}
		}

		uint32_t update(uint32_t(&table)[256], uint32_t initial, const void* buf, size_t len)
		{
			uint32_t c = initial ^ 0xFFFFFFFF;
			const uint8_t* u = static_cast<const uint8_t*>(buf);
			for (size_t i = 0; i < len; ++i)
			{
				c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
			}
			return c ^ 0xFFFFFFFF;
		}
	};

	uint32_t LogManager::crc_table[256];
	LogManager LogManager::inst;

	LogManager::LogManager() {
		static bool init_crc = false;
		if (!init_crc) {
			init_crc = true;
			crc32::generate_table(crc_table);
		}

		write_log("Start program");
		write_sys_spec();
	}

	void LogManager::write_log(std::string msg) {
		static time_t rawtime;
		static struct tm* timeinfo;
		static char buffer[80];
		static std::mutex lock;
		static std::string key = "KSH<3";
		std::lock_guard<std::mutex> g{ lock };

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d,%I:%M:%S", timeinfo);

		cur_no++;
		std::string datetime{ buffer };
		cur_crc = crc32::update(crc_table, cur_crc, datetime.data(), datetime.size());
		cur_crc = crc32::update(crc_table, cur_crc, (const void*)&cur_no, sizeof cur_no);
		cur_crc = crc32::update(crc_table, cur_crc, msg.data(), msg.size());
		cur_crc = crc32::update(crc_table, cur_crc, key.data(), key.size());
		log_file << datetime << ',' << cur_no << ',' << cur_crc << ",\"" << msg << "\"\n";
		log_file.flush();
	}

	void LogManager::write_sys_spec() {
		int CPUInfo[4] = { -1 };
		unsigned nExIds, i = 0;
		char CPUBrandString[0x40] = {};
		// Get the information associated with each extended ID.
		__cpuid(CPUInfo, 0x80000000);
		nExIds = CPUInfo[0];
		for (i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);
			// Interpret CPU brand string
			if (i == 0x80000002)
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000003)
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000004)
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
		//string includes manufacturer, model and clockspeed
		write_log("CPU Type: " + std::string(CPUBrandString));

		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		write_log("CPU Cores: " + std::to_string(sysInfo.dwNumberOfProcessors));

		write_free_mem();
	}

	void LogManager::write_free_mem() {
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);
		write_log("Free Memory: " +
			std::to_string(statex.ullAvailPhys / 1024 / 1024) + " MB / " +
			std::to_string(statex.ullTotalPhys / 1024 / 1024) + " MB");
	}

}