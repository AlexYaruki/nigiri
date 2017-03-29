#include <nigiri.h>
#include <cassert>

int main() {
    bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
    assert(jvmAvailable);
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));
    auto jvmDate = jvm->lookupType("java.util.Date");
    auto jvmDate_constructor = jvm->lookupConstructor(jvmDate,{});
}
