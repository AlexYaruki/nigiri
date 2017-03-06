#ifndef NIGIRI_JVM_BUILD
#define NIGIRI_JVM_BUILD

#ifdef  __linux__
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_JVM_EXPORT
    #else
        #define NIGIRI_JVM_EXPORT
    #endif
#elif WIN32
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_JVM_EXPORT __declspec(dllexport)
    #else
        #define NIGIRI_JVM_EXPORT __declspec(dllimport)
    #endif
#elif __APPLE__
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_JVM_EXPORT
    #else
        #define NIGIRI_JVM_EXPORT
    #endif
#else
    #error Current platform is not supported
#endif

#endif //NIGIRI_JVM_BUILD
