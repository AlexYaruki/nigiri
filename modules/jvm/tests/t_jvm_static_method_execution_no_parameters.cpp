#include <cassert>
#include <nigiri.h>

int main(){
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));
    auto jvmLong = jvm->lookupType("long");
    assert(jvmLong);
    auto jvmSystem = jvm->lookupType("java.lang.System");
    assert(jvmSystem);
    auto jvmSystem_currentTimeMillis = jvm->lookupMethod(jvmSystem,"currentTimeMillis",{},jvmLong);
    assert(jvmSystem_currentTimeMillis);
    auto result = jvm->call(jvmSystem,jvmSystem_currentTimeMillis,{});
    assert(result);
}
