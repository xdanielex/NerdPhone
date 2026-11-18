// ARMv8 SHA-256 hardware path for the Bitcoin header double hash.
//
// Only this translation unit is compiled with -march=armv8-a+crypto (CMake).
// It always runs behind a runtime HWCAP_SHA2 check AND a hash-for-hash
// self-test against the portable path (see HeaderHasher in hash.hpp).
//
// The message schedule is ordinary C, word for word, exactly as in the
// reference implementation: only the 64 rounds use the hardware instructions.
// The state layout is the architectural one, {a,b,c,d} in Qd and {e,f,g,h} in
// Qn; an earlier ABEF/CDGH packing produced wrong digests and has been removed.

#if defined(NERDPHONE_HAVE_SHA2_HW)

#if !defined(__aarch64__)
#error "NERDPHONE_HAVE_SHA2_HW is only valid for arm64-v8a"
#endif
#if !defined(__ARM_FEATURE_SHA2)
#error "compiled without SHA-2 support: add -march=armv8-a+crypto to this file"
#endif

#include "sha256_hw.hpp"
#include <arm_neon.h>
#include <sys/auxv.h>

namespace np {
namespace {

constexpr unsigned long kHwcapSha2 = 1UL << 6; // HWCAP_SHA2, AArch64 Linux

constexpr uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

constexpr uint32_t kInit[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                               0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

inline uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

// One compression function: state st[8] (A..H) absorbed with 16 words.
inline void compress(uint32_t st[8], const uint32_t m[16]) {
    uint32_t w[64];
    for (int i = 0; i < 16; ++i) w[i] = m[i];
    for (int i = 16; i < 64; ++i)
        w[i] = w[i - 16] +
               (rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3)) +
               w[i - 7] +
               (rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10));

    // SHA256H takes {a,b,c,d} in Qd and {e,f,g,h} in Qn, and writes the new
    // {a,b,c,d}; SHA256H2 takes the same operands and writes the new {e,f,g,h}.
    // Both must see the state as it was *before* the pair, hence abcd_save.
    uint32x4_t abcd = vld1q_u32(&st[0]);
    uint32x4_t efgh = vld1q_u32(&st[4]);

    for (int i = 0; i < 16; ++i) {
        const uint32x4_t wk =
            vaddq_u32(vld1q_u32(&w[4 * i]), vld1q_u32(&K[4 * i]));
        const uint32x4_t abcd_save = abcd;
        abcd = vsha256hq_u32(abcd_save, efgh, wk);
        efgh = vsha256h2q_u32(efgh, abcd_save, wk);
    }

    uint32_t ao[4], eo[4];
    vst1q_u32(ao, abcd);
    vst1q_u32(eo, efgh);
    for (int i = 0; i < 4; ++i) st[i] += ao[i];
    for (int i = 0; i < 4; ++i) st[4 + i] += eo[i];
}

} // namespace

bool hw_available() { return (getauxval(AT_HWCAP) & kHwcapSha2) != 0; }

void header_hash_hw(const uint32_t mid[8], const uint32_t tail[16],
                    uint32_t nonce, uint8_t out[32]) {
    uint32_t block[16];
    for (int i = 0; i < 16; ++i) block[i] = tail[i];
    block[3] = __builtin_bswap32(nonce);

    uint32_t st[8];
    for (int i = 0; i < 8; ++i) st[i] = mid[i];
    compress(st, block);

    uint32_t second[16] = {0};
    for (int i = 0; i < 8; ++i) second[i] = st[i];
    second[8] = 0x80000000u;
    second[15] = 256;
    for (int i = 0; i < 8; ++i) st[i] = kInit[i];
    compress(st, second);

    for (int i = 0; i < 32; ++i)
        out[i] = static_cast<uint8_t>(st[i / 4] >> (24 - 8 * (i % 4)));
}

} // namespace np

#endif
