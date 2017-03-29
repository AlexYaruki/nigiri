#include <cassert>
#include <nigiri.h>

int main(){
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));

    auto jvmSystem = jvm->lookupType("java.lang.System");
    assert(jvmSystem);
}
