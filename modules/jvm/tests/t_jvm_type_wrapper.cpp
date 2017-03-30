#include <nigiri.h>
#include <cassert>
#include <iostream>

class TypeWrapper {
public:
    static std::shared_ptr<nigiri::ForeignRuntime> jvm;

};

std::shared_ptr<nigiri::ForeignRuntime> TypeWrapper::jvm;

class LocalTime : public TypeWrapper {
public:
    bool operator==(const LocalTime& lt) {
        return lt.handle == handle;
    }

    bool operator!=(const LocalTime& lt) {
        return !(operator==(lt));
    }

    static LocalTime now(){
        static auto methodHandle = jvm->lookupMethod(type,"now",{},type);
        assert(methodHandle);
        auto obj = jvm->call(type,methodHandle,{});
        return LocalTime(obj);
    }

    int32_t getHour() {
        static const auto jvmMethod_getHour = jvm->lookupMethod(handle,"getHour",{},jvm->lookupType("int"));
        assert(jvmMethod_getHour);
        auto result = jvm->call(handle,jvmMethod_getHour,{});
        return std::get<1>(result->castToInt32());
    }

    int32_t getMinute() {
        static const auto jvmMethod_getMinute = jvm->lookupMethod(handle,"getMinute",{},jvm->lookupType("int"));
        assert(jvmMethod_getMinute);
        auto result = jvm->call(handle,jvmMethod_getMinute,{});
        return std::get<1>(result->castToInt32());
    }

    int32_t getSecond() {
        static const auto jvmMethod_getSecond = jvm->lookupMethod(handle,"getSecond",{},jvm->lookupType("int"));
        assert(jvmMethod_getSecond);
        auto result = jvm->call(handle,jvmMethod_getSecond,{});
        return std::get<1>(result->castToInt32());
    }

    static void init() {
        if(type == nullptr) {
            type = jvm->lookupType("java.time.LocalTime");
            assert(type);
        }
    }
private:
    LocalTime(std::shared_ptr<nigiri::FR_Object> obj) {
        handle = obj;
    }

    static std::shared_ptr<nigiri::FR_Type> type;
    std::shared_ptr<nigiri::FR_Object> handle;
};

std::shared_ptr<nigiri::FR_Type> LocalTime::type;

int main() {
    bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
    assert(jvmAvailable);
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    bool jvmStarted = jvm->start({});
    assert(jvmStarted);

    TypeWrapper::jvm = jvm;
    LocalTime::init();

    LocalTime localTime = LocalTime::now();
    LocalTime localTime2 = LocalTime::now();
    LocalTime localTime3 = localTime;
    assert(localTime == localTime);
    assert(localTime != localTime2);
    assert(localTime == localTime3);
}
