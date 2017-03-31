#include <cassert>
#include <nigiri.h>
#include <iostream>

int main(){
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));
    auto jvmMath = jvm->lookupType("java.lang.Math");
    assert(jvmMath);
    auto jvmDouble = jvm->lookupType("double");
    assert(jvmDouble);
    auto input = jvm->wrapPrimitive(-0.1);
    assert(input);
    auto jvmMath_abs = jvm->lookupMethod(jvmMath,"abs",{input->getType()},input->getType());
    assert(jvmMath_abs);
    auto output = jvm->call(jvmMath, jvmMath_abs, {input});
    assert(output);
    double out = std::get<1>(output->castToDouble());
    std::cout << "Out: " << out << std::endl;
    assert(out == 0.1);
}
