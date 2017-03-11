#include <thread>
#include <mutex>
#include <sstream>
#include <map>
#include <iostream>
#include "nigiri.h"

namespace {
    std::map<std::string,bool> categoryMap;
    std::map<std::string,std::shared_ptr<nigiri::Handle>> handleMap;
    std::map<std::string,nigiri::ForeignRuntimeLoader*> loaderMap;
    std::map<nigiri::FR_Id, std::shared_ptr<nigiri::ForeignRuntime>> runtimeMap;

    nigiri::FR_Id runtimeCounter = 0;

    bool checkCategory(const std::string& name) {
        std::string categoryLibraryName = FR_PREFIX + name + FR_SUFFIX;
        auto handle = nigiri::load(categoryLibraryName);
        if(handle == nullptr){
            categoryMap.insert({name,false});
            return false;
        } else {
            nigiri::Handle::Symbol symbol = handle->lookupSymbol(nigiri::FR_LOADER_FUNC_NAME);
            if(symbol.nativeSymbol == nullptr){
                std::cout << "Loader not found" << std::endl;
                categoryMap.insert({name,false});
                return false;
            } else {
                //TODO: Find better solution to void* to function-pointer cast or provide platform dependent solution
                union {
                    void* rawPointer;
                    nigiri::FR_LOADER_FUNC_TYPE loaderFunc;
                } CastHack;
                CastHack.rawPointer = symbol.nativeSymbol;

                nigiri::FR_LOADER_FUNC_TYPE loaderFunc = CastHack.loaderFunc;
                void* rawLoader = loaderFunc();
                if(rawLoader == nullptr) {
                    categoryMap.insert({name,false});
                    return false;
                } else {
                    nigiri::ForeignRuntimeLoader* loader = static_cast<nigiri::ForeignRuntimeLoader*>(rawLoader);
                    if(!loader->check()) {
                        categoryMap.insert({name,false});
                        return false;
                    } else {
                        handleMap.insert({name,handle});
                        loaderMap.insert({name,loader});
                        categoryMap.insert({name,true});
                        return true;
                    }
                }
            }
        }
    }

}

namespace nigiri {

    RuntimeIDMispatch::RuntimeIDMispatch(const std::string& msg) throw() : reason(msg) {

    }

    const char* RuntimeIDMispatch::what() const throw() {
        return reason.c_str();
    }

    bool ForeignRuntimeManager::isCategoryAvailable(const std::string &name) {
        auto search = categoryMap.find(name);
        if(search == categoryMap.end()){
            return checkCategory(name);
        } else {
            return search->second;
        }
    }

    std::shared_ptr<ForeignRuntime> ForeignRuntimeManager::createRuntime(const std::string &name) {
        if(!isCategoryAvailable(name))  {
            return nullptr;
        } else {
            nigiri::ForeignRuntimeLoader* loader = loaderMap.find(name)->second;
            auto runtime = loader->load(runtimeCounter+1);
            if(runtime != nullptr){
                runtimeMap.insert({runtimeCounter++,runtime});
            }
            return runtime;
        }
    }

    std::shared_ptr<ForeignRuntime> ForeignRuntimeManager::getRuntime(FR_Id id) {
        auto search = runtimeMap.find(id);
        if(search == runtimeMap.end()) {
            return nullptr;
        } else {
            return search->second;
        }
    }

}
