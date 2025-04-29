#pragma once

#if defined(_MSC_VER)
    #define ELBOW_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define ELBOW_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define ELBOW_FORCE_INLINE inline
#endif