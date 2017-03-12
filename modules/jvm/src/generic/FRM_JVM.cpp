#include <cstdlib>
#include <fstream>
#include <nigiri.h>

#include "nigiri_jvm_build.h"
#include "JVMForeignRuntime.h"

class JVMRuntimeLoader : public nigiri::ForeignRuntimeLoader {
public:
    ~JVMRuntimeLoader(){
    }

    bool check() override {
        return true;
    }
    std::shared_ptr<nigiri::ForeignRuntime> load(nigiri::FR_Id id) override {
        return std::make_shared<nigiri::internal::JVMForeignRuntime>(id);
    }

};
extern "C" {

NIGIRI_JVM_EXPORT void* fr_loader(){
    return new JVMRuntimeLoader();
}

}
