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

    }
}

#endif
