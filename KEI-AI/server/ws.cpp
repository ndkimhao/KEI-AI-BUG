#include "stdafx.h"
#include "ws.h"
#include "http_parser.h"
#include "ws_key.h"
#include "ws_frame.h"
#include "ai_manager.h"
#include "log_manager.h"

#define END_CONN return true
using namespace std::chrono_literals;
namespace server {

	bool Ws::handshake(HttpParser &req, SOCKET socket) {
		if (req.getPath() == "/play") {
			auto headers = req.getHeaders();
			//if (headers["Connection"] != "Upgrade") END_CONN;
			if (headers["Upgrade"] != "websocket") END_CONN;

			std::string ver = headers["Sec-WebSocket-Version"];
			if (atoi(ver.c_str()) != 13) END_CONN;

			std::string key = headers["Sec-WebSocket-Key"];
			char resp_key[29] = {};
			WsKey::generate(key.c_str(), resp_key);

			std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
				"Upgrade: websocket\r\n"
				"Connection: Upgrade\r\n"
				"Sec-WebSocket-Accept: " + std::string(resp_key) + "\r\n\r\n";
			send(socket, response.data(), (int)response.size(), 0);

			Ws *ws = new Ws(socket);
			ws->loop();
			delete ws;

			return true;
		}
		else {
			return false;
		}
	}

	bool Ws::check_winner(ai::Point last_move) {
		ai::Cell p = ai.has_win_moves(last_move);
		if (p != ai::Cell::Invalid) {
			std::string winner = (p == ai::Cell::Ai ? "I" : "YOU");
			send_msg(Format("STT %s WIN !!", winner.c_str()));
			std::stringstream ss; ss << "WIN ";
			for (auto &c : ai.win_moves)
				ss << c.x << ' ' << c.y << ' ';
			send_msg(ss.str());
			std::stringstream history; history << "MOVES ";
			for (auto &pp : ai.get_undo_stack())
				history << pp.x << ' ' << pp.y << ' ';
			logman::LogManager::inst.write_log(history.str());
			return true;
		}
		return false;
	}

	Ws::Ws(SOCKET sock) {
		socket = sock;
#pragma warning(suppress: 28159)
		last_recv_pong = GetTickCount();
	}

	void Ws::finish_ai() {
		if (ai.get_ai_turn() == ai::AiTurn::Thinking) {
			ai.abort_thinking();
			while (ai.get_ai_turn() == ai::AiTurn::Thinking)
				std::this_thread::sleep_for(100ms);
			//std::cout << "Finish 1 orphan ai";
		}
	}

	void Ws::loop() {
#define ENDSOCKET { finish_ai(); close(); return; }
		RecvFrame recvFrame;
		fd_set fds;
		struct timeval tv;

		// Set up the file descriptor set.
		FD_ZERO(&fds);
		FD_SET(socket, &fds);

		// Set up the struct timeval for the timeout.
		tv.tv_sec = 0;
		tv.tv_usec = 500000;

		// TODO: ping from my side;
		while (true) {
			// Send ping
#pragma warning(suppress: 28159)
			DWORD cur_time = GetTickCount();
			if (last_send_ping && cur_time - last_send_ping > PING_TIMEOUT) {
				//std::cout << "ping timeout \n";
				ENDSOCKET;
			}
			if (cur_time - last_recv_pong > PING_INTERVAL) {
				//std::cout << "send ping \n";
				send_frame(SendFrame(WsOpcode::Ping, "KSH"));
				last_send_ping = cur_time;
			}

			// Check ai turn
			auto stt = ai.get_ai_turn();
			if (stt != ai::AiTurn::Idle) {
				if (ai.logs.has_logs()) {
					auto logs = ai.logs.get_logs();
					for (auto &e : logs) send_msg(e);
				}
			}
			if (stt == ai::AiTurn::ResultReady) {
				ai::Point p = ai.get_result_move();
				send_msg("PB -1");
				send_msg(Format("AI %d %d", p.x, p.y));
				if (check_winner(p)) continue;
				send_msg("STT Your turn now :)");
			}
			else if (stt == ai::AiTurn::Thinking) {
				send_msg(Format("STT I'm thinking (%ds)",
					(int)(ai.remaining_time() / 1000.0 + 0.5)));
			}

			// Wait data
			fd_set dump_fds = fds;
			int sel = select((int)socket, &dump_fds, NULL, NULL, &tv);
			if (sel == -1) ENDSOCKET;
			if (sel == 0) {
				continue;
			}

			// Process data
			int len = recv(socket, recvFrame.write_buff(), recvFrame.write_buff_size(), 0);
			recvFrame.add_bytes(len);
			int ready = recvFrame.is_frame_ready();
			if (ready == -1) ENDSOCKET;
			if (ready) {
				recvFrame.unmask();
				std::string payload = recvFrame.payload();
				recvFrame.shift_buffer();

				if (recvFrame.opcode() == WsOpcode::Ping) {
					send_frame(SendFrame(WsOpcode::Pong, payload));
				}
				else if (recvFrame.opcode() == WsOpcode::Pong) {
					if (payload != "KSH") ENDSOCKET;
					//std::cout << "recv pong \n";
#pragma warning(suppress: 28159)
					last_recv_pong = GetTickCount();
					last_send_ping = 0;
				}
				else if (recvFrame.opcode() == WsOpcode::Close) {
					//std::cout << "close " << socket << '\n';
					ENDSOCKET;
				}
				else {
					// Discard all messages while thinking
					if (ai.get_ai_turn() != ai::AiTurn::Idle) continue;
					if (!process_msg(payload)) ENDSOCKET;
				}
			}
		}
	}

	bool Ws::process_msg(std::string msg) {
		static const std::vector<ai::AiVariant> varis = { ai::AiVariant::KeiAi };
		static const std::vector<int> time_turn = { 2000, 7000, 15000, 30000, 60000, 120000 };
		static const std::vector<int> undos = { 3, 2, 1, 1, 0, 0 };

		logman::LogManager::inst.write_log("C " + msg);
		std::istringstream ss(msg);
		std::string cmd; ss >> cmd;
		if (cmd == "START") {
			int variant, level, pfirst; ss >> variant >> level >> pfirst;
			if (variant < 0 || variant >= varis.size()) return false;
			if (level < 0 || level >= time_turn.size()) return false;
			if (pfirst < 0 || pfirst>1) return false;
			ai.set_time_turn(time_turn[level]);
			ai.undo_remain = undos[level];
			send_msg("START");
			send_msg("STT Setting up, please wait...");
			send_msg("PB 30000");
			ai.new_match(varis[variant]);
			send_msg("PB -1");
			send_msg(Format("UNDOR %d", ai.undo_remain));
			if (pfirst == 0) ai.start_turn_ai();
			else send_msg("STT You go first !");
			return true;
		}
		if (ai.has_win_moves(ai.INVALID_POINT) != ai::Cell::Invalid) {
			send_msg("STT Please restart game first");
			return true;
		}
		if (cmd == "HM") {
			int x, y; ss >> x >> y;
			if (ai.turn_human({ x, y })) {
				send_msg("LOGCLR");
				send_msg(Format("HM %d %d", x, y));
				if (check_winner({ x, y })) return true;
				ai.start_turn_ai();
				send_msg(Format("PB %d", ai.remaining_time()));
				send_msg("STT My turn now :)");
			}
			else {
				send_msg("STT Invalid move");
			}
			return true;
		}
		else if (cmd == "UNDO") {
			auto res = ai.undo();
			if (res != ai.INVALID_UNDO) {
				send_msg("STT blew blew =))");
				auto last_move = ai.get_last_move();
				send_msg(Format("UNDOR %d", ai.undo_remain));
				send_msg(Format("UNDO %d %d %d %d %d %d",
					res.first.x, res.first.y, res.second.x, res.second.y,
					last_move.x, last_move.y));
			}
			else {
				send_msg("STT Invalid undo command");
			}
			return true;
		}
		return false;
	}

	void Ws::send_msg(std::string msg) {
		logman::LogManager::inst.write_log("S " + msg);
		send_frame(SendFrame(WsOpcode::Text, msg));
	}

	void Ws::send_frame(SendFrame frame) {
		send(socket, frame.header(), frame.header_size(), 0);
		send(socket, frame.payload(), frame.payload_size(), 0);
	}

	void Ws::close() {
		send_frame(SendFrame(WsOpcode::Close, {}));
		shutdown(socket, SD_BOTH);
	}
}