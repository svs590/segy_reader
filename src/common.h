#pragma once

#ifndef SR_STRONG_INLINE
#if _MSC_VER || __INTEL_COMPILER
#define SR_STRONG_INLINE __forceinline
#else
#define SR_STRONG_INLINE inline
#endif
#endif

enum class endian_order { big, little, mid_big, mid_little };