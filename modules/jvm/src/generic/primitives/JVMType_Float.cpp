#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Float::JVMType_Float(FR_Id id, jclass type) : JVMType(id,nullptr,"float","F") {
            primitive = true;
            primitiveFloat = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Float::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticFloatMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Float>(method->getReturnType());
                return std::make_shared<JVM_Float>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Float::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallFloatMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Float>(method->getReturnType());
                return std::make_shared<JVM_Float>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Float::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticFloatField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Float>(field->getType());
                return std::make_shared<JVM_Float>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Float::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetFloatField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Float>(field->getType());
                return std::make_shared<JVM_Float>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }
    }

}
