#include <iostream>
#include <memory>
#include "nigiri_win32.h"

namespace {
    std::shared_ptr<nigiri::Handle> load(const char* path) {
        std::cout << "Loading " << path << std::endl;
		HMODULE nativeHandle = LoadLibraryA(path);
        if(nativeHandle == nullptr){
            DWORD status = GetLastError();
            std::cout << "Cannot load library: " << status << std::endl;
            return nullptr;
        } else {
            return std::make_shared<nigiri::Handle_Win32>(nativeHandle);
        }
    }
}

namespace nigiri {

    NIGIRI_EXPORT std::shared_ptr<Handle> load(const std::string& path){
        return ::load(path.c_str());
    }

    NIGIRI_EXPORT std::shared_ptr<Handle> loadSelf(){
        return ::load(nullptr);
    }

    Handle_Win32::Handle_Win32(HMODULE nativeHandle) {
        this->nativeHandle = nativeHandle;
    }

	Handle_Win32::~Handle_Win32() {
        BOOL status = FreeLibrary(nativeHandle);
        if (status == 0) {
            std::cout << "Nigiri - Library cannot be freed" << std::endl;
        }
    }

	Handle_Win32::Symbol Handle_Win32::lookupSymbol(const std::string &name) const {
        FARPROC symbolHandle = GetProcAddress(nativeHandle, name.c_str());
        Symbol symbol;
        symbol.nativeSymbol = symbolHandle;
        return symbol;
    }

}
