#include "rzxpch.h"
#include "RZUUID.h"

namespace Razix {

    #if defined(__GLIBC__) || defined(__GNU_LIBRARY__) || defined(__ANDROID__)
    #include <endian.h>
    #elif defined(__APPLE__) && defined(__MACH__)
    #include <machine/endian.h>
    #elif defined(BSD) || defined(_SYSTYPE_BSD)
    #if defined(__OpenBSD__)
    #include <machine/endian.h>
    #else
    #include <sys/endian.h>
    #endif
    #endif

    #if defined(__BYTE_ORDER)
    #if defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
    #define BIGENDIAN
    #elif defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
    #define LITTLEENDIAN
    #endif
    #elif defined(_BYTE_ORDER)
    #if defined(_BIG_ENDIAN) && (_BYTE_ORDER == _BIG_ENDIAN)
    #define BIGENDIAN
    #elif defined(_LITTLE_ENDIAN) && (_BYTE_ORDER == _LITTLE_ENDIAN)
    #define LITTLEENDIAN
    #endif
    #elif defined(__BIG_ENDIAN__)
    #define BIGENDIAN
    #elif defined(__LITTLE_ENDIAN__)
    #define LITTLEENDIAN
    #else
    #if defined(__ARMEL__) || \
          defined(__THUMBEL__) || \
          defined(__AARCH64EL__) || \
          defined(_MIPSEL) || \
          defined(__MIPSEL) || \
          defined(__MIPSEL__) || \
          defined(__ia64__) || defined(_IA64) || \
          defined(__IA64__) || defined(__ia64) || \
          defined(_M_IA64) || defined(__itanium__) || \
          defined(i386) || defined(__i386__) || \
          defined(__i486__) || defined(__i586__) || \
          defined(__i686__) || defined(__i386) || \
          defined(_M_IX86) || defined(_X86_) || \
          defined(__THW_INTEL__) || defined(__I86__) || \
          defined(__INTEL__) || \
          defined(__x86_64) || defined(__x86_64__) || \
          defined(__amd64__) || defined(__amd64) || \
          defined(_M_X64) || \
          defined(__bfin__) || defined(__BFIN__) || \
          defined(bfin) || defined(BFIN)

    #define LITTLEENDIAN
    #elif defined(__m68k__) || defined(M68000) || \
            defined(__hppa__) || defined(__hppa) || defined(__HPPA__) || \
            defined(__sparc__) || defined(__sparc) || \
            defined(__370__) || defined(__THW_370__) || \
            defined(__s390__) || defined(__s390x__) || \
            defined(__SYSC_ZARCH__)

    #define BIGENDIAN

    #elif defined(__arm__) || defined(__arm64) || defined(__thumb__) || \
            defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || \
            defined(__ARM_ARCH) || \
            defined(_M_ARM) || defined(_M_ARM64)

    #if defined(_WIN32) || defined(_WIN64) || \
              defined(__WIN32__) || defined(__TOS_WIN__) || \
              defined(__WINDOWS__)

    #define LITTLEENDIAN

    #else
    #error "Cannot determine system endianness."
    #endif
    #endif
    #endif


    #if defined(BIGENDIAN)
        // Try to use compiler intrinsics
    #if defined(__INTEL_COMPILER) || defined(__ICC)
    #define betole16(x) _bswap16(x)
    #define betole32(x) _bswap(x)
    #define betole64(x) _bswap64(x)
    #elif defined(__GNUC__) // GCC and CLANG
    #define betole16(x) __builtin_bswap16(x)
    #define betole32(x) __builtin_bswap32(x)
    #define betole64(x) __builtin_bswap64(x)
    #elif defined(_MSC_VER) // MSVC
    #include <stdlib.h>
    #define betole16(x) _byteswap_ushort(x)
    #define betole32(x) _byteswap_ulong(x)
    #define betole64(x) _byteswap_uint64(x)
    #else
    #define FALLBACK_SWAP
    #define betole16(x) swap_u16(x)
    #define betole32(x) swap_u32(x)
    #define betole64(x) swap_u64(x)
    #endif
    #define betole128(x) swap_u128(x)
    #define betole256(x) swap_u256(x)
    #else
    #define betole16(x) (x)
    #define betole32(x) (x)
    #define betole64(x) (x)
    #define betole128(x) (x)
    #define betole256(x) (x)
    #endif // BIGENDIAN

    #if defined(BIGENDIAN)
    #include <emmintrin.h>
    #include <smmintrin.h>
    #include <immintrin.h>
    #include <tmmintrin.h>

        inline __m128i swap_u128(__m128i value)
        {
            const __m128i shuffle = _mm_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f);
            return _mm_shuffle_epi8(value, shuffle);
        }

        inline __m256i swap_u256(__m256i value)
        {
            const __m256i shuffle = _mm256_set_epi64x(0x0001020304050607, 0x08090a0b0c0d0e0f, 0x0001020304050607, 0x08090a0b0c0d0e0f);
            return _mm256_shuffle_epi8(value, shuffle);
        }
    #endif // BIGENDIAN

    #if defined(FALLBACK_SWAP)
    #include <stdint.h>
        inline uint16_t swap_u16(uint16_t value)
        {
            return
                ((value & 0xFF00u) >> 8u) |
                ((value & 0x00FFu) << 8u);
        }
        inline uint32_t swap_u32(uint32_t value)
        {
            return
                ((value & 0xFF000000u) >> 24u) |
                ((value & 0x00FF0000u) >> 8u) |
                ((value & 0x0000FF00u) << 8u) |
                ((value & 0x000000FFu) << 24u);
        }
        inline uint64_t swap_u64(uint64_t value)
        {
            return
                ((value & 0xFF00000000000000u) >> 56u) |
                ((value & 0x00FF000000000000u) >> 40u) |
                ((value & 0x0000FF0000000000u) >> 24u) |
                ((value & 0x000000FF00000000u) >> 8u) |
                ((value & 0x00000000FF000000u) << 8u) |
                ((value & 0x0000000000FF0000u) << 24u) |
                ((value & 0x000000000000FF00u) << 40u) |
                ((value & 0x00000000000000FFu) << 56u);
        }
    #endif // FALLBACK_SWAP#pragma once

    //---------------------------------------------------------------------------------------------------------
    // RZUUID Class
    //---------------------------------------------------------------------------------------------------------

    RZUUID::RZUUID()
    {
        // Automatically generate a UUID
        std::shared_ptr<std::mt19937_64> generator = std::make_shared<std::mt19937_64>(std::random_device()());
        std::uniform_int_distribution<uint64_t> distribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
        // The two masks set the uuid version (4) and variant (1)
        const __m128i and_mask = _mm_set_epi64x(0xFFFFFFFFFFFFFF3Full, 0xFF0FFFFFFFFFFFFFull);
        const __m128i or_mask = _mm_set_epi64x(0x0000000000000080ull, 0x0040000000000000ull);
        __m128i n = _mm_set_epi64x(distribution(*generator), distribution(*generator));
        __m128i uuid = _mm_or_si128(_mm_and_si128(n, and_mask), or_mask);

        //// _mm_store_si128((__m128i*)data, uuid);
    }
    
    RZUUID::RZUUID(const RZUUID& other)
    {
        __m128i x = _mm_load_si128((__m128i*)other.data);
        // _mm_store_si128((__m128i*)data, x);
    }

    RZUUID::RZUUID(__m128i uuid)
    {
        // _mm_store_si128((__m128i*)data, uuid);
    }

    RZUUID::RZUUID(uint64_t x, uint64_t y)
    {
        __m128i z = _mm_set_epi64x(x, y);
        // _mm_store_si128((__m128i*)data, z);
    }

    RZUUID::RZUUID(const uint8_t* bytes)
    {
        __m128i x = _mm_loadu_si128((__m128i*)bytes);
        // _mm_store_si128((__m128i*)data, x);
    }

    RZUUID::RZUUID(const std::string& bytes)
    {
        __m128i x = betole128(_mm_loadu_si128((__m128i*)bytes.data()));
        // _mm_store_si128((__m128i*)data, x);
    }

    Razix::RZUUID RZUUID::FromStrFactory(const std::string& s)
    {
        return FromStrFactory(s.c_str());
    }

    Razix::RZUUID RZUUID::FromStrFactory(const char* raw)
    {
        return RZUUID(stom128i(raw));
    }

    void RZUUID::FromStr(const char* raw)
    {
        // _mm_store_si128((__m128i*)data, stom128i(raw));
    }

    std::string RZUUID::bytes() const
    {
        std::string mem;
        bytes(mem);
        return mem;
    }

    void RZUUID::bytes(std::string& out) const
    {
        out.resize(sizeof(data));
        bytes((char*) out.data());
    }

    void RZUUID::bytes(char* bytes) const
    {
        __m128i x = betole128(_mm_load_si128((__m128i*)data));
        _mm_storeu_si128((__m128i*)bytes, x);
    }

    std::string RZUUID::str() const
    {
        std::string mem;
        str(mem);
        return mem;
    }

    void RZUUID::str(std::string& s) const
    {
        s.resize(36);
        str((char*) s.data());
    }

    void RZUUID::str(char* res) const
    {
        __m128i x = _mm_load_si128((__m128i*)data);
        m128itos(x, res);
    }

    size_t RZUUID::hash() const
    {
        //return *((uint64_t*) data) ^ *((uint64_t*) data + 8);
        // https://github.com/crashoz/uuid_v4/pull/13
        const uint64_t a = *((uint64_t*) data);
        const uint64_t b = *((uint64_t*) &data[8]);
        return a ^ b + 0x9e3779b9 + (a << 6) + (a >> 2);
    }

    void RZUUID::m128itos(__m128i x, char* mem)
    {
        // Expand each byte in x to two bytes in res
        // i.e. 0x12345678 -> 0x0102030405060708
        // Then translate each byte to its hex ascii representation
        // i.e. 0x0102030405060708 -> 0x3132333435363738
        const __m256i mask = _mm256_set1_epi8(0x0F);
        const __m256i add = _mm256_set1_epi8(0x06);
        const __m256i alpha_mask = _mm256_set1_epi8(0x10);
        const __m256i alpha_offset = _mm256_set1_epi8(0x57);

        __m256i a = _mm256_castsi128_si256(x);
        __m256i as = _mm256_srli_epi64(a, 4);
        __m256i lo = _mm256_unpacklo_epi8(as, a);
        __m128i hi = _mm256_castsi256_si128(_mm256_unpackhi_epi8(as, a));
        __m256i c = _mm256_inserti128_si256(lo, hi, 1);
        __m256i d = _mm256_and_si256(c, mask);
        __m256i alpha = _mm256_slli_epi64(_mm256_and_si256(_mm256_add_epi8(d, add), alpha_mask), 3);
        __m256i offset = _mm256_blendv_epi8(_mm256_slli_epi64(add, 3), alpha_offset, alpha);
        __m256i res = _mm256_add_epi8(d, offset);

        // Add dashes between blocks as specified in RFC-4122
        // 8-4-4-4-12
        const __m256i dash_shuffle = _mm256_set_epi32(0x0b0a0908, 0x07060504, 0x80030201, 0x00808080, 0x0d0c800b, 0x0a090880, 0x07060504, 0x03020100);
        const __m256i dash = _mm256_set_epi64x(0x0000000000000000ull, 0x2d000000002d0000ull, 0x00002d000000002d, 0x0000000000000000ull);

        __m256i resd = _mm256_shuffle_epi8(res, dash_shuffle);
        resd = _mm256_or_si256(resd, dash);

        _mm256_storeu_si256((__m256i*)mem, betole256(resd));
        *(uint16_t*) (mem + 16) = betole16(_mm256_extract_epi16(res, 7));
        *(uint32_t*) (mem + 32) = betole32(_mm256_extract_epi32(res, 7));
    }

    __m128i RZUUID::stom128i(const char* mem)
    {
        // Remove dashes and pack hex ascii bytes in a 256-bits int
        const __m256i dash_shuffle = _mm256_set_epi32(0x80808080, 0x0f0e0d0c, 0x0b0a0908, 0x06050403, 0x80800f0e, 0x0c0b0a09, 0x07060504, 0x03020100);

        __m256i x = betole256(_mm256_loadu_si256((__m256i*)mem));
        x = _mm256_shuffle_epi8(x, dash_shuffle);
        x = _mm256_insert_epi16(x, betole16(*(uint16_t*) (mem + 16)), 7);
        x = _mm256_insert_epi32(x, betole32(*(uint32_t*) (mem + 32)), 7);

        // Build a mask to apply a different offset to alphas and digits
        const __m256i sub = _mm256_set1_epi8(0x2F);
        const __m256i mask = _mm256_set1_epi8(0x20);
        const __m256i alpha_offset = _mm256_set1_epi8(0x28);
        const __m256i digits_offset = _mm256_set1_epi8(0x01);
        const __m256i unweave = _mm256_set_epi32(0x0f0d0b09, 0x0e0c0a08, 0x07050301, 0x06040200, 0x0f0d0b09, 0x0e0c0a08, 0x07050301, 0x06040200);
        const __m256i shift = _mm256_set_epi32(0x00000000, 0x00000004, 0x00000000, 0x00000004, 0x00000000, 0x00000004, 0x00000000, 0x00000004);

        // Translate ascii bytes to their value
        // i.e. 0x3132333435363738 -> 0x0102030405060708
        // Shift hi-digits
        // i.e. 0x0102030405060708 -> 0x1002300450067008
        // Horizontal add
        // i.e. 0x1002300450067008 -> 0x12345678
        __m256i a = _mm256_sub_epi8(x, sub);
        __m256i alpha = _mm256_slli_epi64(_mm256_and_si256(a, mask), 2);
        __m256i sub_mask = _mm256_blendv_epi8(digits_offset, alpha_offset, alpha);
        a = _mm256_sub_epi8(a, sub_mask);
        a = _mm256_shuffle_epi8(a, unweave);
        a = _mm256_sllv_epi32(a, shift);
        a = _mm256_hadd_epi32(a, _mm256_setzero_si256());
        a = _mm256_permute4x64_epi64(a, 0b00001000);

        return _mm256_castsi256_si128(a);
    }
}
