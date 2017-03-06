#ifndef NIGIRI_WIN32_H
#define NIGIRI_WIN32_H

#include <Windows.h>
#include "nigiri_base.h"

namespace nigiri {
    class Handle_Win32 : public HandleBase<FARPROC> {
    public:
        Handle_Win32(FARPROC nativeHandle);
        ~Handle_Win32();
        Symbol lookupSymbol(const std::string& name) const;
    private:
        void* nativeHandle;
    };

    using Handle = Handle_Linux;
}

#endif //NIGIRI_WIN32_H
