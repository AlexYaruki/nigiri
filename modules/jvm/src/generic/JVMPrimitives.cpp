#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        // JVM Primitive Wrapper - char ///////////////////////////////////////////

        JVM_Char::JVM_Char(jchar v, std::shared_ptr<JVMType_Char> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Char::toValue() {
            jvalue v;
            v.c = value;
            return v;
        }

        std::tuple<bool,uint16_t> JVM_Char::castToUInt16(){
            return std::tuple<bool,uint16_t>(true,value);
        }

        // JVM Primitive Wrapper - boolean ///////////////////////////////////////////

        JVM_Boolean::JVM_Boolean(jboolean v, std::shared_ptr<JVMType_Boolean> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Boolean::toValue() {
            jvalue v;
            v.z = value;
            return v;
        }

        std::tuple<bool,bool> JVM_Boolean::castToBool(){
            return std::tuple<bool,bool>(true,value == JNI_TRUE ? true : false);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - byte ///////////////////////////////////////////

        JVM_Int8::JVM_Int8(jbyte v, std::shared_ptr<JVMType_Int8> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int8::toValue() {
            jvalue v;
            v.b = value;
            return v;
        }

        std::tuple<bool,int8_t> JVM_Int8::castToInt8(){
            return std::tuple<bool,int8_t>(true,value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - short ///////////////////////////////////////////

        JVM_Int16::JVM_Int16(jshort v, std::shared_ptr<JVMType_Int16> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int16::toValue() {
            jvalue v;
            v.s = value;
            return v;
        }

        std::tuple<bool,int16_t> JVM_Int16::castToInt16(){
            return std::tuple<bool,int16_t>(true,value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - int ///////////////////////////////////////////

        JVM_Int32::JVM_Int32(jint v, std::shared_ptr<JVMType_Int32> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int32::toValue() {
            jvalue v;
            v.i = value;
            return v;
        }

        std::tuple<bool,int32_t> JVM_Int32::castToInt32(){
            return std::tuple<bool,int32_t>(true,value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - long ///////////////////////////////////////////

        JVM_Int64::JVM_Int64(jlong v, std::shared_ptr<JVMType_Int64> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int64::toValue() {
            jvalue v;
            v.j = value;
            return v;
        }

        std::tuple<bool,int64_t> JVM_Int64::castToInt64(){
            return std::tuple<bool,int64_t>(true,value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - float //////////////////////////////////////////

        JVM_Float::JVM_Float(jfloat v, std::shared_ptr<JVMType_Float> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Float::toValue() {
            jvalue v;
            v.f = value;
            return v;
        }

        std::tuple<bool,float> JVM_Float::castToFloat(){
            return std::tuple<bool,float>(true,value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - double /////////////////////////////////////////

        JVM_Double::JVM_Double(jdouble v, std::shared_ptr<JVMType_Double> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Double::toValue() {
            jvalue v;
            v.d = value;
            return v;
        }

        std::tuple<bool,double> JVM_Double::castToDouble(){
            return std::tuple<bool,double>(true,value);
        }

        ///////////////////////////////////////////////////////////////////////////

    }

}
