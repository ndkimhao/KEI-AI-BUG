#pragma once
#include "stdafx.h"
#include "http_parser.h"
#include "ws_frame.h"
#include "ai_manager.h"

namespace server {
	class Ws {
	private:
		static const int PING_TIMEOUT = 1500;
		static const int PING_INTERVAL = 20000;

		void loop();
		void close();
		void send_frame(SendFrame frame);

		Ws(SOCKET sock);
		SOCKET socket;

		DWORD last_send_ping = 0, last_recv_pong = 0;

		void finish_ai();
		bool check_winner(ai::Point last_move);
		bool process_msg(std::string msg);
		void send_msg(std::string msg);

		ai::AiManager ai;

	public:
		static bool handshake(HttpParser &req, SOCKET socket);
	};
}