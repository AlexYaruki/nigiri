#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Boolean::JVMType_Boolean(FR_Id id, jclass type) : JVMType(id,nullptr,"boolean","Z") {
            primitive = true;
            primitiveBoolean = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Boolean::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticBooleanMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Boolean>(method->getReturnType());
                return std::make_shared<JVM_Boolean>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Boolean::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallBooleanMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Boolean>(method->getReturnType());
                return std::make_shared<JVM_Boolean>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Boolean::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticBooleanField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Boolean>(field->getType());
                return std::make_shared<JVM_Boolean>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Boolean::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetBooleanField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Boolean>(field->getType());
                return std::make_shared<JVM_Boolean>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
