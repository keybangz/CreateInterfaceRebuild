#pragma once

#include <spdlog/spdlog.h>

namespace console {
	enum class LogLevel {
		Info,
		Warning,
		Error
	};

	void create();

	void destroy();

	template <LogLevel Level, typename... Args>
	__forceinline void log(std::format_string<Args...> fmt, Args&&... args) {
		create();

		const auto message = std::format(fmt, std::forward<Args>(args)...);

		switch (Level) {
			case LogLevel::Info:
				spdlog::info(message);
				break;

			case LogLevel::Warning:
				spdlog::warn(message);
				break;

			case LogLevel::Error:
				spdlog::error(message);
				break;
		}
	}
}