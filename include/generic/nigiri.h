#ifndef NIGIRI_H
#define NIGIRI_H

#include <string>
#include <memory>
#include <thread>

#include "nigiri_build.h"
#ifdef __linux__
	#include "nigiri_unix.h"
#else
	#ifdef _WIN64
		#include "nigiri_win32.h"
	#else
		#ifdef __APPLE__
			#include "../unix/nigiri_unix.h"
		#else
			#error This platform is not supported
		#endif
	#endif
#endif

#include "nigiri_foreignruntime.h"

namespace nigiri {

    template <typename T>
    struct Result {
        uint32_t status;
        std::shared_ptr<T> value;
    };

    NIGIRI_EXPORT std::shared_ptr<Handle> load(const std::string &path);

    NIGIRI_EXPORT std::shared_ptr<Handle> loadSelf();

}

#endif //NIGIRI_H
