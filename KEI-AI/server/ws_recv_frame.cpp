#include "stdafx.h"
#include "ws_frame.h"

namespace server {

	char* RecvFrame::write_buff() { return buff + buff_count; }
	int RecvFrame::write_buff_size() const { return BufferSize - buff_count; }

	int RecvFrame::is_frame_ready() const
	{
		if (!is_frame_valid()) return -1;
		if (buff_count < 2) return false;
		return buff_count >= frame_len();
	}

	void RecvFrame::add_bytes(int n)
	{
		buff_count += n;
	}

	bool RecvFrame::is_frame_valid() const
	{
		if (buff_count == 0) return true;
		if (!is_final_fragment()) return false;

		if (buff_count == 1) return true;
		if (!is_masked()) return false;
		if (payload_len() > MaxPayloadLen) return false;

		return true;
	}

	bool RecvFrame::is_final_fragment() const { return (buff[0] & 0x80) != 0; }
	WsOpcode RecvFrame::opcode() const { return static_cast<WsOpcode>(buff[0] & 0x0F); }
	bool RecvFrame::is_masked() const { return (buff[1] & 0x80) != 0; }
	int RecvFrame::payload_len() const { return buff[1] & 0x7f; }
	int RecvFrame::payload_start() const { return MinHeaderLen; }
	int RecvFrame::frame_len() const { return payload_start() + payload_len(); }
	std::string RecvFrame::payload() const {
		return { buff + payload_start(), (unsigned)payload_len() };
	}

	void RecvFrame::unmask()
	{
		auto data = buff + payload_start();
		auto key = data - 4;

		for (auto i = 0; i != payload_len(); ++i)
			data[i] ^= key[i % 4];
	}

	void RecvFrame::shift_buffer()
	{
		auto currentFrameLen = frame_len();
		buff_count -= currentFrameLen;
		std::memmove(buff, buff + currentFrameLen, buff_count);
	}

}