#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Int64::JVMType_Int64(FR_Id id, jclass type) : JVMType(id,nullptr,"long","J") {
            primitive = true;
            primitiveInt64 = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Int64::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticLongMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int64>(method->getReturnType());
                return std::make_shared<JVM_Int64>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Int64::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallLongMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int64>(method->getReturnType());
                return std::make_shared<JVM_Int64>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Int64::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticLongField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int64>(field->getType());
                return std::make_shared<JVM_Int64>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Int64::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetLongField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int64>(field->getType());
                return std::make_shared<JVM_Int64>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
