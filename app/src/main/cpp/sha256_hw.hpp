#pragma once
#include <cstdint>

// Declarations for the optional ARMv8 SHA-256 hardware path.
// Without NERDPHONE_HAVE_SHA2_HW (host tests, armeabi-v7a, x86_64) these are
// inline no-ops, so hash.hpp compiles and links everywhere at no cost.

#if defined(NERDPHONE_HAVE_SHA2_HW)

namespace np {
bool hw_available();
void header_hash_hw(const uint32_t mid[8], const uint32_t tail[16],
                    uint32_t nonce, uint8_t out[32]);
} // namespace np

#else

namespace np {
inline bool hw_available() { return false; }
inline void header_hash_hw(const uint32_t*, const uint32_t*, uint32_t, uint8_t*) {}
} // namespace np

#endif
