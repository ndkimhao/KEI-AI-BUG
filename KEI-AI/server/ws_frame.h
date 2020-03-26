#pragma once
#include "stdafx.h"

namespace server {

	enum class WsOpcode
	{
		Continuation = 0,
		Text = 1,
		Binary = 2,
		Close = 8,
		Ping = 9,
		Pong = 10,
	};

	class SendFrame
	{
	public:
		SendFrame(WsOpcode op, std::string data);

		const char* header() const;
		int header_size() const;
		const char* payload() const;
		int payload_size() const;

	private:
		void write_opcode(WsOpcode op);
		void write_len();

		std::uint8_t m_header[1 + 1 + 8];
		std::uint8_t m_headerLen;
		std::string m_data;
	};

	class RecvFrame
	{
	public:
		static const int MinHeaderLen = 1 + 1 + 4;
		static const int MaxPayloadLen = 125;
		static const int BufferSize = (MinHeaderLen + MaxPayloadLen) * 5;

		char* write_buff();
		int write_buff_size() const;

		int is_frame_ready() const;
		void add_bytes(int n);
		bool is_frame_valid() const;

		bool is_final_fragment() const;
		WsOpcode opcode() const;
		bool is_masked() const;
		int payload_len() const;
		int payload_start() const;
		int frame_len() const;
		std::string payload() const;
		void unmask();
		void shift_buffer();

	private:
		char buff[BufferSize];
		int buff_count = 0;
	};
}