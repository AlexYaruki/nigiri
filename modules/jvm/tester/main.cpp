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
    auto typeLong = jvm->lookupType("long");
    auto typeSystem = jvm->lookupType("java.lang.System");
    auto method_System_currentTimeMillis = jvm->lookupMethod(typeSystem,"currentTimeMillis",{},typeLong);
    std::cout << ">>>> DEBUG: Preparing to invoke method ..." << std::endl;
    auto preTime = getTime();
    for(int i = 0; i < 1000; i++) {
        auto result = jvm->call(typeSystem,method_System_currentTimeMillis,{});
    }
    auto postTime = getTime();
    std::cout << "Time: " << ((postTime - preTime)) << " ms" << std::endl;
    return 0;
}
