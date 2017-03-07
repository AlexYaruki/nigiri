#ifndef JVM_PRIMITIVES_H
#define JVM_PRIMITIVES_H

#include "JVMForeignRuntime.h"

namespace nigiri {
    namespace internal {

        // JVM Primitive Types ////////////////////////////////////////////////////

        class JVMType_Char : public nigiri::internal::JVMType {
        public:
            JVMType_Char(FR_Id id ,jclass type);
        };

        class JVMType_Boolean : public nigiri::internal::JVMType {
        public:
            JVMType_Boolean(FR_Id id ,jclass type);
        };

        class JVMType_Int8 : public nigiri::internal::JVMType {
        public:
            JVMType_Int8(FR_Id id ,jclass type);
        };

		class JVMType_Int16 : public nigiri::internal::JVMType {
        public:
            JVMType_Int16(FR_Id id ,jclass type);
        };

		class JVMType_Int32 : public nigiri::internal::JVMType {
        public:
            JVMType_Int32(FR_Id id ,jclass type);
        };

		class JVMType_Int64 : public nigiri::internal::JVMType {
        public:
            JVMType_Int64(FR_Id id ,jclass type);
        };

		class JVMType_Float : public nigiri::internal::JVMType {
		public:
			JVMType_Float(FR_Id id ,jclass type);
		};

		class JVMType_Double : public nigiri::internal::JVMType {
        public:
            JVMType_Double(FR_Id id ,jclass type);
        };

        ///////////////////////////////////////////////////////////////////////////

        // JVM Primitive Wrappers /////////////////////////////////////////////////

        class JVM_Char : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Char(uint16_t v, std::shared_ptr<JVMType_Char> vType, FR_Id id);
            std::experimental::optional<uint16_t> castToUInt16();
            jvalue toValue();
        private:
            jchar value;
        };

        class JVM_Boolean : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Boolean(bool v, std::shared_ptr<JVMType_Boolean> vType, FR_Id id);
            std::experimental::optional<bool> castToBool();
            jvalue toValue();
        private:
            jboolean value;
        };

        class JVM_Int8 : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Int8(int8_t v, std::shared_ptr<JVMType_Int8> vType, FR_Id id);
            std::experimental::optional<int8_t> castToInt8();
            jvalue toValue();
        private:
            jbyte value;
        };

        class JVM_Int16 : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Int16(int16_t v, std::shared_ptr<JVMType_Int16> vType, FR_Id id);
            std::experimental::optional<int16_t> castToInt16();
            jvalue toValue();
        private:
            jshort value;
        };

        class JVM_Int32 : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Int32(int32_t v, std::shared_ptr<JVMType_Int32> vType, FR_Id id);
            std::experimental::optional<int32_t> castToInt32();
            jvalue toValue();
        private:
            jint value;
        };

        class JVM_Int64 : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Int64(int64_t v, std::shared_ptr<JVMType_Int64> vType, FR_Id id);
            std::experimental::optional<int64_t> castToInt64();
            jvalue toValue();
        private:
            jlong value;
        };

        class JVM_Float : public nigiri::internal::JVMObjectBase {
        public:
            JVM_Float(float v, std::shared_ptr<JVMType_Float> vType, FR_Id id);
            std::experimental::optional<float> castToFloat();
            jvalue toValue();
        private:
            jdouble value;
        };

		class JVM_Double : public nigiri::internal::JVMObjectBase {
		public:
			JVM_Double(double v, std::shared_ptr<JVMType_Double> vType, FR_Id id);
			std::experimental::optional<double> castToDouble();
			jvalue toValue();
		private:
			jdouble value;
		};

        ////////////////////////////////////////////////////////////////////////////
    }
}

#endif
