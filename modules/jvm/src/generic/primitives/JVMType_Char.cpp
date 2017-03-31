#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Char::JVMType_Char(FR_Id id, jclass type) : JVMType(id,nullptr,"char","C") {
            primitive = true;
            primitiveChar = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Char::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticCharMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Char>(method->getReturnType());
                return std::make_shared<JVM_Char>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Char::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallCharMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Char>(method->getReturnType());
                return std::make_shared<JVM_Char>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Char::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticCharField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Char>(field->getType());
                return std::make_shared<JVM_Char>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Char::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetCharField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Char>(field->getType());
                return std::make_shared<JVM_Char>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
