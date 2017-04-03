#ifndef JVM_OP_PARAMS_H
#define JVM_OP_PARAMS_H

#include "JVMForeignRuntime.h"

namespace nigiri {
    namespace internal {

        class JVMOpParams_TypeLookup : public JVMOpParams{
        public:
            ~JVMOpParams_TypeLookup() = default;
            std::string typeName;
            std::shared_ptr<JVMType> type;
        };

        class JVMOpParams_MethodLookup : public JVMOpParams{
        public:
            ~JVMOpParams_MethodLookup() = default;
            std::shared_ptr<FR_Type> type;
            std::string methodName;
			std::string methodSignature;
			jmethodID method;
        };

		class JVMOpParams_StaticMethodCall : public JVMOpParams {
		public:
			~JVMOpParams_StaticMethodCall() = default;
			std::shared_ptr<FR_Type> type;
			std::shared_ptr<FR_Method> method;
            const std::vector<std::shared_ptr<FR_Object>> *parameters;
			std::shared_ptr<FR_Object> result;
		};

        class JVMOpParams_InstanceMethodCall : public JVMOpParams {
		public:
			~JVMOpParams_InstanceMethodCall() = default;
			std::shared_ptr<FR_Object> object;
			std::shared_ptr<FR_Method> method;
            const std::vector<std::shared_ptr<FR_Object>> *parameters;
			std::shared_ptr<FR_Object> result;
		};

        class JVMOpParams_FieldLookup : public JVMOpParams{
        public:
            ~JVMOpParams_FieldLookup() = default;
            std::shared_ptr<FR_Type> targetType;
            std::string name;
            std::shared_ptr<FR_Type> type;
            std::shared_ptr<FR_Field> field;
        };

        class JVMOpParams_StaticFieldAccess : public JVMOpParams {
		public:
			~JVMOpParams_StaticFieldAccess() = default;
			std::shared_ptr<FR_Type> targetType;
			std::shared_ptr<FR_Field> field;
            std::shared_ptr<FR_Object> result;
		};

        class JVMOpParams_InstanceFieldAccess : public JVMOpParams {
		public:
			~JVMOpParams_InstanceFieldAccess() = default;
            std::shared_ptr<FR_Object> targetType;
			std::string name;
			std::shared_ptr<FR_Type> type;
            std::shared_ptr<FR_Object> result;
		};

        class JVMOpParams_ObjectConstruction : public JVMOpParams {
        public:
            ~JVMOpParams_ObjectConstruction() = default;
            std::shared_ptr<FR_Type> type;
   			std::shared_ptr<FR_Method> constructor;
            const std::vector<std::shared_ptr<FR_Object>> *parameters;
   			std::shared_ptr<FR_Object> result;
        };

        class JVMOpParams_StringExtraction : public JVMOpParams {
        public:
            ~JVMOpParams_StringExtraction() = default;
            std::shared_ptr<JVMObject> target;
   			std::string result;
        };

        class JVMOpParams_TypeParametersLookup : public JVMOpParams {
        public:
            ~JVMOpParams_TypeParametersLookup() = default;
            std::shared_ptr<JVMType> target;
   			std::vector<std::string> typeParameters;
        };


    }
}

#endif
