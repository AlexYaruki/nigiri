#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        JVMType_Int8::JVMType_Int8(FR_Id id, jclass type) : JVMType(id,nullptr,"byte","B") {
            primitive = true;
            primitiveInt8 = true;
        }

        //TODO: Respect Java language scope
        const StaticMethodCaller& JVMType_Int8::getStaticMethodCaller() {
            static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
                auto obj = env->CallStaticByteMethodA(targetType->getType(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int8>(method->getReturnType());
                return std::make_shared<JVM_Int8>(obj,jvmReturnType,targetType->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const InstanceMethodCaller& JVMType_Int8::getInstanceMethodCaller() {
            static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
                auto obj = env->CallByteMethodA(targetObject->getObject(),method->getMethod(),jniParams);
                auto jvmReturnType = std::static_pointer_cast<JVMType_Int8>(method->getReturnType());
                return std::make_shared<JVM_Int8>(obj,jvmReturnType,targetObject->getRuntimeId());
            };
            return caller;
        }

        //TODO: Respect Java language scope
        const StaticFieldAccessor& JVMType_Int8::getStaticFieldAccessor() {
            static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
                auto obj = env->GetStaticByteField(targetType->getType(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int8>(field->getType());
                return std::make_shared<JVM_Int8>(obj,jvmFieldType,targetType->getRuntimeId());
            };
            return accessor;
        }

        //TODO: Respect Java language scope
        const InstanceFieldAccessor& JVMType_Int8::getInstanceFieldAccessor() {
            static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
                auto obj = env->GetByteField(targetObject->getObject(), field->getField());
                auto jvmFieldType = std::static_pointer_cast<JVMType_Int8>(field->getType());
                return std::make_shared<JVM_Int8>(obj,jvmFieldType,targetObject->getRuntimeId());
            };
            return accessor;
        }

    }

}
