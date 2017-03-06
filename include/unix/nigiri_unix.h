#ifndef NIGIRI_UNIX_H
#define NIGIRI_UNIX_H

#include <dlfcn.h>
#include "nigiri_base.h"

namespace nigiri {

    const std::string PATH_SEPARATOR = ":";

    class Handle_Unix : public HandleBase<void*> {
    public:
        Handle_Unix(void* nativeHandle);
        ~Handle_Unix();
        Symbol lookupSymbol(const std::string& name) const;
    private:
        void* nativeHandle;
    };

    using Handle = Handle_Unix;

}

#endif //NIGIRI_LINUX_H
