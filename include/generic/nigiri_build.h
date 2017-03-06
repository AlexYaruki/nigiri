#ifndef NIGIRI_NIGIRI_UTILS_H
#define NIGIRI_NIGIRI_UTILS_H

#ifdef  __linux__
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_EXPORT
    #else
        #define NIGIRI_EXPORT
    #endif
#elif WIN32
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_EXPORT __declspec(dllexport)
    #else
        #define NIGIRI_EXPORT __declspec(dllimport)
    #endif
#elif __APPLE__
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_EXPORT
    #else
        #define NIGIRI_EXPORT
    #endif
#else
    #error Current platform is not supported
#endif

#endif //NIGIRI_NIGIRI_UTILS_H
