#include <nigiri.h>
#include <cassert>

int main() {
    bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
    assert(jvmAvailable);
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    bool jvmStarted = jvm->start({});
    assert(jvmStarted);
}
