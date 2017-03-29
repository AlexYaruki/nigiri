#include <nigiri.h>
#include <cassert>

int main() {
    bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
    assert(jvmAvailable);
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));
    auto jvmObject = jvm->lookupType("java.lang.Object");
    assert(jvmObject);
    auto jvmObject_constructor = jvm->lookupConstructor(jvmObject,{});
    assert(jvmObject_constructor);
    auto obj = jvm->createObject(jvmObject,jvmObject_constructor,{});
    assert(obj);
}
