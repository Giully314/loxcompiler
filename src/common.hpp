#ifndef LOX_COMMON_HPP
#define LOX_COMMON_HPP

#include <cstdint>
#include <type_traits>

// namespace lox
// {
//     template <typename Enum>
// 		requires std::is_enum_v<Enum>
// 	auto operator+(Enum e) -> std::underlying_type_t<Enum>
// 	{
// 		return static_cast<std::underlying_type_t<Enum>>(e);
// 	}
// } // namespace lox

namespace lox
{
    using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using f32 = float;
	using f64 = double;

    template <typename T>
    using non_owned_ptr = T*;
} // namespace lox


#endif
