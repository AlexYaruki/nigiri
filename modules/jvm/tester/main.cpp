#include <iostream>
#include <chrono>
#include "nigiri.h"

unsigned long getTime(){
    unsigned long milliseconds_since_epoch =
    std::chrono::system_clock::now().time_since_epoch() /
    std::chrono::milliseconds(1);
}

int main() {
    bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
    if(!jvmAvailable) {
        std::cout << "JVM module is not available" << std::endl;
        return -1;
    }
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    jvm->start();
    auto type_long = jvm->lookupType("long");
    auto type_java_lang_System = jvm->lookupType("java.lang.System");
    auto method_System_currentTimeMillis = jvm->lookupMethod(type_java_lang_System,"currentTimeMillis",{},type_long);
    auto result = jvm->call(type_java_lang_System,method_System_currentTimeMillis,{});
    std::cout << "CurrentTimeMillis: " << result->castToInt64().value() << std::endl;

    auto type_math = jvm->lookupType("java.lang.Math");
    auto type_double = jvm->lookupType("double");
    auto method_abs = jvm->lookupMethod(type_math,"abs",{type_double},type_double);
    auto input = jvm->wrapPrimitive(-0.1);
    auto output = jvm->call(type_math, method_abs, {input});
    std::cout << "Output: " << output->castToDouble().value() << std::endl;
    return 0;
}
