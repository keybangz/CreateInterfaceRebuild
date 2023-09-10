#pragma once

namespace vmt {
	template <std::uint16_t Index, typename RetType, typename... Args>
	__forceinline RetType call_virtual(void* base_class, Args... args) {
		using Fn = RetType(__thiscall*)(void*, decltype(args)...);

		const auto fn = (*static_cast<Fn**>(base_class))[Index];

		return fn(base_class, args...);
	}

	__forceinline void* get_virtual(void* base_class, const std::uint16_t index) {
		return (*static_cast<void***>(base_class))[index];
	}
}