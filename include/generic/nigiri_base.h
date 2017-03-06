#ifndef NIGIRI_BASE_H
#define NIGIRI_BASE_H

#include <string>
#include "nigiri_build.h"

namespace nigiri {

    template <typename T>
    class NIGIRI_EXPORT HandleBase {
    public:
            struct Symbol {
                T nativeSymbol;
            };

            HandleBase() = default;
            virtual ~HandleBase() = default;
            template<typename S>
            S findSymbol(const std::string &name) {
                return reinterpret_cast<S>(lookupSymbol(name).nativeHandle);
            }

    protected:
            virtual Symbol lookupSymbol(const std::string& name) const = 0;
    };

}

#endif //NIGIRI_BASE_H
