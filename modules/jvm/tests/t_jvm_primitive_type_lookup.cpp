#include <cassert>
#include <nigiri.h>


int main() {
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));

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


    auto jvmBoolean2 = jvm->lookupType("boolean");
    assert(jvmBoolean2);
    assert(jvmBoolean == jvmBoolean2);
    auto jvmChar2 = jvm->lookupType("char");
    assert(jvmChar2);
    assert(jvmChar == jvmChar2);

    auto jvmByte2 = jvm->lookupType("byte");
    assert(jvmByte2);
    assert(jvmByte == jvmByte2);
    auto jvmShort2 = jvm->lookupType("short");
    assert(jvmShort2);
    assert(jvmShort == jvmShort2);
    auto jvmInt2 = jvm->lookupType("int");
    assert(jvmInt2);
    assert(jvmInt == jvmInt2);
    auto jvmLong2 = jvm->lookupType("long");
    assert(jvmLong2);
    assert(jvmLong == jvmLong2);

    auto jvmFloat2 = jvm->lookupType("float");
    assert(jvmFloat2);
    assert(jvmFloat == jvmFloat2);
    auto jvmDouble2 = jvm->lookupType("double");
    assert(jvmDouble2);
    assert(jvmDouble == jvmDouble2);
}
