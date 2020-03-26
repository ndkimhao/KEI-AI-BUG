#pragma once
#include "stdafx.h"

namespace server {
	class ServerListener {
		static const int RECV_BUFFER_SIZE = 256;

	private:
		int port;
		SOCKET listen_socket = INVALID_SOCKET;
		bool server_running;

		void client_handler(SOCKET client_socket);

		static inline std::string reason_phrase(int code);
		std::pair<const void*, int> get_resource_data(int res_id, LPWSTR type);
		bool raw_file(std::string &path, SOCKET socket);

	public:
		ServerListener(int port = 6996);

		static void send_response(SOCKET socket, int status, std::string mime = "text/plain", 
			const char* body = nullptr, int body_len = 0, bool compressed = false);

		void run();

		void stop();

		~ServerListener();
	};

	class NetException : public std::runtime_error {
	public:
		NetException(std::string info) :
			std::runtime_error("Network error: " + info) {}

		NetException(std::string info, int code) :
			std::runtime_error("Network error: " + info + ", code " + std::to_string(code)) {}

		virtual ~NetException() {}
	};
}