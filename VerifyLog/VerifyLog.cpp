
#include "stdafx.h"

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
uint32_t table[256];

int main()
{
	std::ifstream ifs("ksh.csv");

	crc32::generate_table(table);
	uint32_t cur_crc = 0;
	int prev_no = 0;
	static std::string key = "KSH<3";

	std::string date, time; int no; uint32_t crc; std::string dat;
	while (std::getline(ifs, date, ',')) {
		if (!std::getline(ifs, time, ',')) goto wrong;
		char dump;
		if (!(ifs >> no >> dump)) goto wrong;
		if (!(ifs >> crc >> dump)) goto wrong;
		if (!std::getline(ifs, dat)) goto wrong;

		if (no != 1 && no != prev_no + 1)
			goto wrong;
		if (no == 1) prev_no = 1, cur_crc = 0;
		else prev_no++;
		std::string dt = date + "," + time;
		cur_crc = crc32::update(table, cur_crc, dt.data(), dt.size());
		cur_crc = crc32::update(table, cur_crc, (const void*)&no, sizeof(no));
		if (dat.size() < 2)
			goto wrong;
		cur_crc = crc32::update(table, cur_crc, dat.data() + 1, dat.size() - 2);
		cur_crc = crc32::update(table, cur_crc, key.data(), key.size());
		if (cur_crc != crc)
			goto wrong;
	}
	std::cout << "CSV file is correct !";
	getchar();
	return 0;
wrong:
	std::cout << "CSV file is wrong !";
	getchar();
	return 0;
}

