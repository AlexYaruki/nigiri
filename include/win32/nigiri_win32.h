#ifndef NIGIRI_WIN32_H
#define NIGIRI_WIN32_H

#include <Windows.h>
#include "nigiri_base.h"

namespace nigiri {
    class Handle_Win32 : public HandleBase<FARPROC> {
    public:
        Handle_Win32(HMODULE nativeHandle);
        ~Handle_Win32();
        Symbol lookupSymbol(const std::string& name) const;
    private:
		HMODULE nativeHandle;
    };

    using Handle = Handle_Win32;
}

#endif //NIGIRI_WIN32_H
