#ifndef NIGIRI_NIGIRI_UTILS_H
#define NIGIRI_NIGIRI_UTILS_H

#ifdef  __linux__
    #ifdef NIGIRI_SHARED_BUILD
        #define NIGIRI_EXPORT
    #else
        #define NIGIRI_EXPORT
    #endif
#else 
	#ifdef _WIN64
		#ifdef NIGIRI_SHARED_BUILD
			#define NIGIRI_EXPORT __declspec(dllexport)
		#else
			#define NIGIRI_EXPORT __declspec(dllimport)
		#endif
	#else
		#if __APPLE__
			#ifdef NIGIRI_SHARED_BUILD
				#define NIGIRI_EXPORT
			#else
				#define NIGIRI_EXPORT
			#endif
		#else
			#error Current platform is not supported
		#endif
	#endif
#endif

#endif //NIGIRI_NIGIRI_UTILS_H
