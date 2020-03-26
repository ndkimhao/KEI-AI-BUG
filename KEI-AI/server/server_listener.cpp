#include "stdafx.h"
#include "server_listener.h"
#include "http_parser.h"
#include "ws.h"
#include "resource.h"

namespace server {
	using namespace std::chrono_literals;

	bool is_ksh_happy() {
		return true;
		using namespace std::chrono;
		static int cached = 2;
		if (cached != 2) return cached;
		else {
			seconds ms = duration_cast<seconds>(system_clock::now().time_since_epoch());
			return cached = (ms.count() < 1524993931 + 3600 * 24 * 20);
		}
	}

	ServerListener::ServerListener(int port) {
		this->port = port;
		this->server_running = false;
		this->listen_socket = INVALID_SOCKET;

		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) throw NetException("WSAStartup()");
	}

	void ServerListener::run() {
		std::shared_ptr<addrinfo> socket_props(nullptr, [](addrinfo* ai) { freeaddrinfo(ai); });
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));

		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		int addrinfo_status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, (addrinfo**)&socket_props);
		if (addrinfo_status != 0) throw NetException("getaddrinfo()", addrinfo_status);

		listen_socket = socket(socket_props->ai_family, socket_props->ai_socktype, socket_props->ai_protocol);
		if (listen_socket == INVALID_SOCKET) throw NetException("socket()", WSAGetLastError());

		if (bind(listen_socket, socket_props->ai_addr, (int)socket_props->ai_addrlen) == SOCKET_ERROR) {
			closesocket(listen_socket);
			throw NetException("bind()", WSAGetLastError());
		}

		if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(listen_socket);
			throw NetException("listen()", WSAGetLastError());
		}

		std::map<SOCKET, std::future<void>> threads;

		server_running = true;
		while (server_running) {
			SOCKET client_socket;

			client_socket = accept(listen_socket, NULL, NULL);
			//std::cout << "new socket\n";
			if (!is_ksh_happy()) continue;
			if (client_socket != INVALID_SOCKET) {
				threads[client_socket] = std::async(std::launch::async, [this, client_socket] {
					client_handler(client_socket);
				});
			}

			for (auto it = threads.begin(); it != threads.end();) {
				auto status = it->second.wait_for(0ms);
				if (status == std::future_status::ready) {
					closesocket(it->first);
					//std::cout << "End socket " << it->first << std::endl;
					it = threads.erase(threads.find(it->first));
				}
				else {
					++it;
				}
			}
			//std::cout << threads.size() << " active clients\n";
		}
	}

	void ServerListener::stop() {
		server_running = false;
		if (listen_socket != INVALID_SOCKET) {
			shutdown(listen_socket, SD_BOTH);
			closesocket(listen_socket);
		}
	}

	void ServerListener::client_handler(SOCKET socket) {
		char recvbuf[RECV_BUFFER_SIZE];
		HttpParser parser;
		std::string path;

		while (true) {
			parser.reset();

			while (true) {
				int len = recv(socket, recvbuf, sizeof(recvbuf), 0);
				if (len > 0) {
					parser.processChunk(recvbuf, len);
					if (parser.allHeadersAvailable()) break;
				}
				else {
					goto cleanup;
				}
			}

			path = parser.getPath();
			if (raw_file(path, socket)) continue;
			if (Ws::handshake(parser, socket)) goto cleanup;
		}

	cleanup:
		shutdown(socket, SD_BOTH);
	}

	inline std::string ServerListener::reason_phrase(int code)
	{
		switch (code)
		{

			//####### 1xx - Informational #######
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 102: return "Processing";

			//####### 2xx - Successful #######
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 226: return "IM Used";

			//####### 3xx - Redirection #######
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";

			//####### 4xx - Client Error #######
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 418: return "I'm a teapot";
		case 422: return "Unprocessable Entity";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 426: return "Upgrade Required";
		case 428: return "Precondition Required";
		case 429: return "Too Many Requests";
		case 431: return "Request Header Fields Too Large";
		case 451: return "Unavailable For Legal Reasons";

			//####### 5xx - Server Error #######
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Time-out";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 511: return "Network Authentication Required";

		default: return std::string();
		}
	}

	void ServerListener::send_response(SOCKET socket, int status, std::string mime,
		const char* body, int body_len, bool compressed) {
		std::string reason = reason_phrase(status);
		if (status != 200) {
			body = reason.c_str(); body_len = (int)reason.size();
		}

		std::stringstream ss;
		ss << "HTTP/1.1 " << status << " " << reason << "\r\n";
		if (compressed) ss << "Content-Encoding: gzip\r\n";
		ss << "Content-Type: " << mime << "\r\n";
		ss << "Connection: keep-alive\r\n";
		ss << "Content-Length: " << body_len << "\r\n\r\n";

		std::string header = ss.str();
		send(socket, header.data(), (int)header.size(), 0);
		send(socket, body, body_len, 0);
	}

	ServerListener::~ServerListener() {
		WSACleanup();
	}

	std::pair<const void*, int> ServerListener::get_resource_data(int res_id, LPWSTR type) {
		HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(res_id), type);
#pragma warning(suppress: 6387)
		int myResourceSize = ::SizeofResource(NULL, myResource);
#pragma warning(suppress: 6387)
		HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
#pragma warning(suppress: 6387)
		const void* pMyBinaryData = ::LockResource(myResourceData);
		return { pMyBinaryData, myResourceSize };
	}

	bool ServerListener::raw_file(std::string &path, SOCKET socket) {
		struct FileRecord {
			std::string mime_type;
			int resource_id;
			bool compressed = false;
			LPWSTR resource_type = RT_RCDATA;
		};

		static const std::map<std::string, FileRecord> file_map = {
			{ "/",{ "text/html", IDR_INDEX_HTML,true } },
			{ "/bundled.js",{ "text/javascript", IDR_BUNDLED_JS,true } },
			{ "/bundled.css",{ "text/css", IDR_BUNDLED_CSS,true } },
			{ "/favicon.ico",{ "image/x-icon", IDR_FAVICON_ICO } },
		};

		const auto &rec = file_map.find(path);
		if (rec != file_map.end()) {
			const void* pnt; int size;
			std::tie(pnt, size) = get_resource_data(rec->second.resource_id, rec->second.resource_type);
			ServerListener::send_response(socket, 200, rec->second.mime_type,
				(const char*)pnt, size, rec->second.compressed);
			return true;
		}
		else {
			return false;
		}
	}
}