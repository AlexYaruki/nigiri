#include <cassert>
#include <nigiri.h>

int main(){
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));

    auto jvmObject = jvm->lookupType("java.lang.Object");
    assert(jvmObject);
    auto jvmSystem = jvm->lookupGenericType("java.util.List",{jvmObject});
    assert(!jvmSystem);
}
