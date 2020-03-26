#include "stdafx.h"
#include "ws_frame.h"

namespace server {

	
	SendFrame::SendFrame(WsOpcode op, std::string data)
		: m_data(std::move(data))
	{
		write_opcode(op);
		write_len();
	}

	void SendFrame::write_opcode(WsOpcode op)
	{
		const auto FinalFragmentFlag = 0x80;
		m_header[0] = FinalFragmentFlag | static_cast<std::uint8_t>(op);
	}

	void SendFrame::write_len()
	{
		auto n = m_data.size();
		if (n <= 125)
		{
			m_headerLen = 1 + 1;
			m_header[1] = static_cast<std::uint8_t>(n);
		}
		else if (n <= 0xFFFF)
		{
			m_headerLen = 1 + 1 + 2;
			m_header[1] = 126;

			m_header[2] = (n >> 8) & 0xFF;
			m_header[3] = n & 0xFF;
		}
		else if (n <= 0xFFffFFff)
		{
			m_headerLen = 1 + 1 + 8;
			m_header[1] = 127;

			m_header[2] = 0;
			m_header[3] = 0;
			m_header[4] = 0;
			m_header[5] = 0;
			m_header[6] = (n >> 8 * 3) & 0xFF;
			m_header[7] = (n >> 8 * 2) & 0xFF;
			m_header[8] = (n >> 8 * 1) & 0xFF;
			m_header[9] = n & 0xFF;
		}
		else
		{
			assert(0);
		}
	}

	const char* SendFrame::header() const {
		return (char*)m_header;
	}
	int SendFrame::header_size() const {
		return m_headerLen;
	}
	const char* SendFrame::payload() const {
		return m_data.data();
	}
	int SendFrame::payload_size() const {
		return (int)m_data.size();
	}

}