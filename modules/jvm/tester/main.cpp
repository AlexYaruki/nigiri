#include <iostream>
#include <chrono>
#include "nigiri.h"

unsigned long getTime(){
    unsigned long milliseconds_since_epoch =
    std::chrono::system_clock::now().time_since_epoch() /
    std::chrono::milliseconds(1);
	return milliseconds_since_epoch;
}

void check(std::shared_ptr<nigiri::FR_Object> obj, std::string msg) {
	if (obj == nullptr) {
		std::cout << msg << std::endl;
		std::exit(-1);
	}
}

void check(std::shared_ptr<nigiri::FR_Type> obj, std::string msg) {
	if (obj == nullptr) {
		std::cout << msg << std::endl;
		std::exit(-1);
	}
}

void check(std::shared_ptr<nigiri::FR_Method> obj, std::string msg) {
	if (obj == nullptr) {
		std::cout << msg << std::endl;
		std::exit(-1);
	}
}

int main() {
    bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
    if(!jvmAvailable) {
        std::cout << "JVM module is not available" << std::endl;
        return -1;
    }
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    jvm->start({"nigiri.jar"});
    auto type_long = jvm->lookupType("long");
    auto type_java_lang_System = jvm->lookupType("java.lang.System");
	check(type_java_lang_System, "Cannot find java.lang.System");
    auto method_System_currentTimeMillis = jvm->lookupMethod(type_java_lang_System,"currentTimeMillis",{},type_long);
	check(method_System_currentTimeMillis, "Cannot find java.lang.System.currentTimeMillis()");
    auto result = jvm->call(type_java_lang_System,method_System_currentTimeMillis,{});
	check(result, "Result of java.lang.System.currentTimeMillis() is null");
    std::cout << "CurrentTimeMillis: " << std::get<1>(result->castToInt64()) << std::endl;

    auto type_math = jvm->lookupType("java.lang.Math");
    auto type_double = jvm->lookupType("double");
    auto method_abs = jvm->lookupMethod(type_math,"abs",{type_double},type_double);
    auto input = jvm->wrapPrimitive(-0.1);
    auto output = jvm->call(type_math, method_abs, {input});
    std::cout << "Output: " << std::get<1>(output->castToDouble()) << std::endl;

    auto method_max_long = jvm->lookupMethod(type_math,"max",{type_long,type_long},type_long);
    auto in1 = jvm->wrapPrimitive(1L);
    auto in2 = jvm->wrapPrimitive(3L);
    auto out = jvm->call(type_math,method_max_long,{in1,in2});
    std::cout << "Type: " << type_math->getName() << std::endl;
    std::cout << "Out - Type: " << out->getType()->getName() << std::endl;
    std::cout << "Out: " << std::get<1>(out->castToInt64()) << std::endl;

    auto type_Date = jvm->lookupType("java.util.Date");
    auto constructor_Date = jvm->lookupConstructor(type_Date,{});
    auto date = jvm->createObject(type_Date,constructor_Date,{});
    auto type_String = jvm->lookupType("java.lang.String");
    auto method_Date_toString = jvm->lookupMethod(date,"toString",{},type_String);
    auto str = jvm->call(date,method_Date_toString,{});
    std::cout << "Type(toString): " << str->getType()->getName() << std::endl;
    std::cout << "Value(toString): " << jvm->toString(str) << std::endl;

    auto type_Point = jvm->lookupType("nigiri.Point");
    check(type_Point, "Cannot find nigiri.Point");
    auto type_int = jvm->lookupType("int");
    check(type_Point, "Cannot find int");
    auto constructor_Point = jvm->lookupConstructor(type_Point,{type_int, type_int});
    check(constructor_Point, "Cannot find Point(int,int)");
    auto point = jvm->createObject(type_Point,constructor_Point,
        {
            jvm->wrapPrimitive(1),
            jvm->wrapPrimitive(3)
        });
    check(point, "point is nullptr");

    return 0;
}
