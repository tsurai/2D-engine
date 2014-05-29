#ifndef PLATFORM_H
#define PLATFORM_H

// compiler defines
/*
#define TSUKI_COMPILER_GCC  1
#define TSUKI_COMPILER_MINGW 2
#define TSUKI_COMPILER_MSVC 3
#define TSUKI_COMPILER_BORDLAND 4
#define TSUKI_COMPILER_UNKNOWN 0
*/

#ifdef __GNUC__
    #define TSUKI_COMPILER_GCC
#elif defined(__MINGW32_)
    #define TSUKI_COMPILER_MINGW
#elif defined(_MSC_VER)
    #defineTSUKI_COMPILER_MSVC
#elif defined(__BORLANDC__)
    #define TSUKI_COMPILER_BORLAND
#else
    #define TSUKI_COMPILER_UNKNOWN
#endif

// os defines
/*
#define TSUKI_OS_LINUX 1
#define TSUKI_OS_WIN 2
#define TSUKI_OS_MACOSX 3
#define TSUKI_OS_UNKNOWN 0
*/

#ifdef __linux__
    #define TSUKI_OS_LINUX
#elif defined(_WIN32) || defined(__WIN32__)
    #define TSUKI_OS_WIN
#elif defined(__APPLE__) && defined(__MACH__)
    #define TSUKI_OS_MACOSX
#else
    #define TSUKI_OS_UNKNOWN
#endif

// endianness defines
/*
#define TSUKI_BYTE_ORDER_LITTLE 1
#define TSUKI_BYTE_ORDER_BIG 2
#define TSUKI_BYTE_ORDER_PDP 3
#define TSUKI_BYTE_ORDER_UNKNOWN 0
*/

#ifdef TSUKI_COMPILER_GCC
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define TSUKI_BYTE_ORDER_LITTLE
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define TSUKI_BYTE_ORDER_LITTLE
    #elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
        #define TSUKI_BYTE_ORDER_PDP
    #else
        #define TSUKI_BYTE_ORDER_UNKNOWN
    #endif
#endif

#endif // PLATFORM_H
