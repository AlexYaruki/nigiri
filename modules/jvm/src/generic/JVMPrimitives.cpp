#include "JVMPrimitives.h"

namespace nigiri {

    namespace internal {

        // JVM Primitive Types ////////////////////////////////////////////////////

        JVMType_Char::JVMType_Char(FR_Id id, jclass type) : JVMType(id,nullptr,"char") {
            primitiveChar = true;
        }

        JVMType_Boolean::JVMType_Boolean(FR_Id id, jclass type) : JVMType(id,nullptr,"boolean") {
            primitiveBoolean = true;
        }

        JVMType_Int8::JVMType_Int8(FR_Id id, jclass type) : JVMType(id,nullptr,"byte") {
            primitiveInt8 = true;
        }

		JVMType_Int16::JVMType_Int16(FR_Id id, jclass type) : JVMType(id,nullptr,"short") {
            primitiveInt16 = true;
        }

		JVMType_Int32::JVMType_Int32(FR_Id id, jclass type) : JVMType(id,nullptr,"int") {
            primitiveInt32 = true;
        }

		JVMType_Int64::JVMType_Int64(FR_Id id, jclass type) : JVMType(id,nullptr,"long") {
            primitiveInt64 = true;
        }

		JVMType_Float::JVMType_Float(FR_Id id, jclass type) : JVMType(id,nullptr,"float") {
            primitiveFloat = true;
        }

		JVMType_Double::JVMType_Double(FR_Id id, jclass type) : JVMType(id,nullptr,"double") {
			primitiveDouble = true;
		}

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - char ///////////////////////////////////////////

        JVM_Char::JVM_Char(uint16_t v, std::shared_ptr<JVMType_Char> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Char::toValue() {
            jvalue v;
            v.c = value;
            return v;
        }

        std::experimental::optional<uint16_t> JVM_Char::castToUInt16(){
            return std::experimental::optional<uint16_t>(value);
        }

        // JVM Primitive Wrapper - boolean ///////////////////////////////////////////

        JVM_Boolean::JVM_Boolean(bool v, std::shared_ptr<JVMType_Boolean> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Boolean::toValue() {
            jvalue v;
            v.z = value;
            return v;
        }

        std::experimental::optional<bool> JVM_Boolean::castToBool(){
            return std::experimental::optional<bool>(value == JNI_TRUE ? true : false);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - byte ///////////////////////////////////////////

        JVM_Int8::JVM_Int8(int8_t v, std::shared_ptr<JVMType_Int8> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int8::toValue() {
            jvalue v;
            v.b = value;
            return v;
        }

        std::experimental::optional<int8_t> JVM_Int8::castToInt8(){
            return std::experimental::optional<int8_t>(value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - short ///////////////////////////////////////////

        JVM_Int16::JVM_Int16(int16_t v, std::shared_ptr<JVMType_Int16> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int16::toValue() {
            jvalue v;
            v.s = value;
            return v;
        }

        std::experimental::optional<int16_t> JVM_Int16::castToInt16(){
            return std::experimental::optional<int16_t>(value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - int ///////////////////////////////////////////

        JVM_Int32::JVM_Int32(int32_t v, std::shared_ptr<JVMType_Int32> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int32::toValue() {
            jvalue v;
            v.i = value;
            return v;
        }

        std::experimental::optional<int32_t> JVM_Int32::castToInt32(){
            return std::experimental::optional<int32_t>(value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - long ///////////////////////////////////////////

        JVM_Int64::JVM_Int64(int64_t v, std::shared_ptr<JVMType_Int64> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Int64::toValue() {
            jvalue v;
            v.j = value;
            return v;
        }

        std::experimental::optional<int64_t> JVM_Int64::castToInt64(){
            return std::experimental::optional<int64_t>(value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - float //////////////////////////////////////////

        JVM_Float::JVM_Float(float v, std::shared_ptr<JVMType_Float> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Float::toValue() {
            jvalue v;
            v.f = value;
            return v;
        }

        std::experimental::optional<float> JVM_Float::castToFloat(){
            return std::experimental::optional<float>(value);
        }

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrapper - double /////////////////////////////////////////

        JVM_Double::JVM_Double(double v, std::shared_ptr<JVMType_Double> vType, FR_Id id) {
            type = vType;
            value = v;
            runtimeId = id;
        }

        jvalue JVM_Double::toValue() {
            jvalue v;
            v.d = value;
            return v;
        }

        std::experimental::optional<double> JVM_Double::castToDouble(){
            return std::experimental::optional<double>(value);
        }

        ///////////////////////////////////////////////////////////////////////////

    }

}
