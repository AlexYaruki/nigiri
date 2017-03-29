#include <cassert>
#include <nigiri.h>


int main() {
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    jvm->start({});

    auto jvmBoolean = jvm->lookupType("boolean");
    assert(jvmBoolean);

    auto jvmChar = jvm->lookupType("char");
    assert(jvmChar);

    auto jvmByte = jvm->lookupType("byte");
    assert(jvmByte);
    auto jvmShort = jvm->lookupType("short");
    assert(jvmShort);
    auto jvmInt = jvm->lookupType("int");
    assert(jvmInt);
    auto jvmLong = jvm->lookupType("long");
    assert(jvmLong);

    auto jvmFloat = jvm->lookupType("float");
    assert(jvmFloat);
    auto jvmDouble = jvm->lookupType("double");
    assert(jvmDouble);
}
