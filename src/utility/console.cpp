#include "utility/console.hpp"

namespace console {
	bool is_initialized = false;

	void create() {
		if (!is_initialized) {
			AllocConsole();

			freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

			SetConsoleTitleA("cs2-sdk");

			is_initialized = true;
		}
	}

	void destroy() {
		if (!is_initialized)
			return;

		fclose(stdout);

		FreeConsole();

		is_initialized = false;
	}
}