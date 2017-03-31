#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Int32::JVMType_Int32(FR_Id id, jclass type) : JVMType(id,nullptr,"int","I") {
            primitive = true;
            primitiveInt32 = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Int32::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticIntMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int32>(method->getReturnType());
                return std::make_shared<JVM_Int32>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Int32::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallIntMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int32>(method->getReturnType());
                return std::make_shared<JVM_Int32>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Int32::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticIntField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int32>(field->getType());
                return std::make_shared<JVM_Int32>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Int32::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetIntField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int32>(field->getType());
                return std::make_shared<JVM_Int32>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
