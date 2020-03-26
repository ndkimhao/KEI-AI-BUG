#include "stdafx.h"
#include "server_listener.h"

namespace webui {
	int main() {
		ShellExecute(0, 0, L"http://localhost:6996/", 0, 0, SW_SHOW);
		std::cout << "Visit http://localhost:6996/ to play gomoku!";
		server::ServerListener().run();
#pragma warning(push, 0)
		return 0;
#pragma warning(pop)
	}
}

#ifdef USE_WEBUI
int main() {
	return webui::main();
}
#endif