#pragma once

#include <cstdint>
#include <cstring>

namespace fnv1a {
	using HashType = std::uint64_t;

	constexpr std::uint64_t FNV_OFFSET_BASIS = 0xCBF29CE484222325;
	constexpr std::uint64_t FNV_PRIME = 0x100000001B3;

	__forceinline HashType fnv_hash(const char* str) {
		const std::size_t length = std::strlen(str) + 1;

		HashType result = FNV_OFFSET_BASIS;

		for (std::size_t i = 0; i < length; ++i) {
			result ^= str[i];
			result *= FNV_PRIME;
		}

		return result;
	}

	template <std::size_t N>
	 constexpr HashType fnv_hash_const(const char(& str)[N], const std::size_t i = N) {
		return i == 1 ? (FNV_OFFSET_BASIS ^ str[0]) * FNV_PRIME : (fnv_hash_const(str, i - 1) ^ str[i - 1]) * FNV_PRIME;
	}

	template <class T>
	__forceinline constexpr HashType fnv_hash_type() {
#ifdef _MSC_VER
		return fnv_hash_const(__FUNCDNAME__);
#else
		return fnv_hash_const(__PRETTY_FUNCTION__);
#endif
	}
}