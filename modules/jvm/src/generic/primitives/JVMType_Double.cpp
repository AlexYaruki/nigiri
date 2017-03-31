#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Double::JVMType_Double(FR_Id id, jclass type) : JVMType(id,nullptr,"double","D") {
            primitive = true;
            primitiveDouble = true;
		}

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Double::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticDoubleMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Double>(method->getReturnType());
                return std::make_shared<JVM_Double>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Double::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallDoubleMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Double>(method->getReturnType());
                return std::make_shared<JVM_Double>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Double::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticDoubleField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Double>(field->getType());
                return std::make_shared<JVM_Double>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Double::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetDoubleField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Double>(field->getType());
                return std::make_shared<JVM_Double>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
