#ifndef DATA_STRUCTURES_BYTE_ARRAY_H
#define DATA_STRUCTURES_BYTE_ARRAY_H

#include <array>
#include <cstdint>
#include <vector>

namespace wt
{
	/// We will define bytes as std::uint8_t here because
	/// it seemed like the best cross-platform representation
	/// for uninterpreted bits. std::byte looked great but it has
	/// very bad support and clashes with WinRT.
	using Byte = std::uint8_t;

	template <int N>
	using ByteArray = std::array<Byte, N>;

	using ByteVector = std::vector<Byte>;
}

#endif // DATA_STRUCTURES_BYTE_ARRAY_H
