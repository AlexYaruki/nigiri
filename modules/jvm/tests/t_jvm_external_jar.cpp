#include <cassert>
#include <iostream>
#include <fstream>
#include <nigiri.h>

int main(){
    auto file = std::ifstream("nigiri.jar");
    assert(file.good());
    file.close();

    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({"nigiri.jar"}));

    auto jvmPoint = jvm->lookupType("com.nigiri.Point");
    assert(jvmPoint);
}
