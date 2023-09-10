#pragma once

#include "utility/fnv1a.hpp"

#include <any>
#include <mutex>
#include <string>
#include <unordered_map>

#define CONFIG_PATH R"(C:\cs2-sdk\config.json)"

namespace config {
	namespace detail {
		extern std::unordered_map<fnv1a::HashType, std::any> map;

		extern std::mutex mutex;
	}

	bool save_to_file(const std::string& file_path);

	bool load_from_file(const std::string& file_path);

	template <typename T>
	__forceinline void set_impl(const fnv1a::HashType hash, const T& value) {
		std::lock_guard<std::mutex> lock(detail::mutex);

		detail::map[hash] = value;
	}

	template <typename T>
	__forceinline T& get_impl(const fnv1a::HashType hash) {
		std::lock_guard<std::mutex> lock(detail::mutex);

		const auto& it = detail::map.find(hash);

		if (it == detail::map.end()) {
			detail::map[hash] = std::make_any<T>();

			return std::any_cast<T&>(detail::map[hash]);
		}

		return std::any_cast<T&>(it->second);
	}

	template <typename T, std::size_t N>
	__forceinline constexpr void set(const char(&key)[N], const T& value) {
		set_impl(fnv1a::fnv_hash_const(key), value);
	}

	template <typename T, std::size_t N>
	__forceinline constexpr T& get(const char(&key)[N]) {
		return get_impl<T>(fnv1a::fnv_hash_const(key));
	}
}