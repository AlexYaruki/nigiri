#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Int16::JVMType_Int16(FR_Id id, jclass type) : JVMType(id,nullptr,"short","S") {
            primitive = true;
            primitiveInt16 = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Int16::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticShortMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int16>(method->getReturnType());
                return std::make_shared<JVM_Int16>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Int16::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallShortMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int16>(method->getReturnType());
                return std::make_shared<JVM_Int16>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Int16::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticShortField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int16>(field->getType());
                return std::make_shared<JVM_Int16>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Int16::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetShortField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int16>(field->getType());
                return std::make_shared<JVM_Int16>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
