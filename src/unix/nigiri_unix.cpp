#include <iostream>
#include <memory>
#include <dlfcn.h>
#include "nigiri_unix.h"

namespace {
    std::shared_ptr<nigiri::Handle> load(const char* path) {
        void* nativeHandle = dlopen(path,RTLD_NOW | RTLD_GLOBAL);
        if(nativeHandle == nullptr){
            std::cout << "Nigiri - Warning: library cannot be loaded with immediate type resolution.\n\tCause: " << dlerror() << std::endl;
            nativeHandle = dlopen(path,RTLD_LAZY | RTLD_GLOBAL);
            if(nativeHandle == nullptr){
                std::cout << "Nigiri - Warning: library cannot be loaded with lazy type resolution.\n\tCause: " << dlerror() << std::endl;
            }
        }
        if(nativeHandle == nullptr){
            return nullptr;
        } else {
            return std::make_shared<nigiri::Handle_Unix>(nativeHandle);
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

    Handle_Unix::Handle_Unix(void* nativeHandle) {
        this->nativeHandle = nativeHandle;
    }

    Handle_Unix::~Handle_Unix() {
        int status = dlclose(nativeHandle);
        if (status != 0) {
            std::cout << "Nigiri - Library cannot be freed" << std::endl;
        }
    }

    Handle_Unix::Symbol Handle_Unix::lookupSymbol(const std::string &name) const {
        void* symbolHandle = dlsym(nativeHandle, name.c_str());
        Symbol symbol;
        symbol.nativeSymbol = symbolHandle;
        return symbol;
    }

}
