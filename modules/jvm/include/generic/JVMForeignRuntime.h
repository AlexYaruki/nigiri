#pragma once

#include <jni.h>
#include <iostream>

#include <nigiri_foreignruntime.h>
#include <nigiri_statemachine.h>

#include "nigiri_jvm_build.h"
#include "JVMEnums.h"

namespace nigiri {
	namespace internal
	{
		const bool LOG_JVMTHREAD = true;
		const bool LOG_JVMFOREIGNRUNTIME = true;

        class JVMType : public nigiri::FR_Type {
        public:
            JVMType(FR_Id id ,jclass type);
            ~JVMType();
			FR_Id getRuntimeId();
			jclass getType();

        protected:
            jclass type;
			FR_Id runtimeId;

            bool primitiveBoolean = false;
            bool primitiveChar = false;
            bool primitiveInt8 = false;
            bool primitiveInt16 = false;
            bool primitiveInt32 = false;
            bool primitiveInt64 = false;
            bool primitiveFloat = false;
            bool primitiveDouble = false;
        };

        class JVMType_Int64 : public nigiri::internal::JVMType {
        public:
            JVMType_Int64(FR_Id id ,jclass type);
        };


        class JVMMethod : public nigiri::FR_Method {
        public:
            JVMMethod(FR_Id, jmethodID method);
            FR_Id getRuntimeId();
            jmethodID getMethod();
        private:
            FR_Id runtimeId;
            jmethodID method;

        };

        class JVMOpParams {
        public:
            virtual ~JVMOpParams() = default;
        };

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

        using JVMOp = std::function<void(JNIEnv* env,std::shared_ptr<JVMOpParams> params)>;

		class JVMForeignRuntime : public nigiri::ForeignRuntime
		{
			friend class JVMThread;
		public:
			JVMForeignRuntime(FR_Id id);
			~JVMForeignRuntime();
            FR_Id getId() override;
			bool start() override;
			void stop() override;
            bool isRunning() override;

            std::shared_ptr<FR_Type> lookupType(const std::string& name) override;

            std::shared_ptr<FR_Method> lookupMethod(std::shared_ptr<FR_Type> targetType,
													const std::string& name,
													const std::vector<std::shared_ptr<FR_Type>>& parameterTypes,
													const std::shared_ptr<FR_Type> returnType) override;

			std::shared_ptr<nigiri::FR_Object> call(std::shared_ptr<FR_Type> targetType,
                                                    std::shared_ptr<FR_Method> method,
                                                    const std::vector<std::shared_ptr<FR_Object>>& parameters) override;

            std::shared_ptr<nigiri::FR_Object> call(std::shared_ptr<FR_Object> targetObject,
                                                    std::shared_ptr<FR_Method> method,
                                                    const std::vector<std::shared_ptr<FR_Object>>& parameters) override;

			void waitForInitialization();
			void notifyWorkPrepared();
			void waitForWorkCompleted();
		private:
			bool isAvailable();
			std::string prepareMethodSignature(const std::vector<std::shared_ptr<FR_Type>> &parametersTypes,
	                                           const std::shared_ptr<FR_Type> returnType);
            void execute(JVMOp jvmOp, std::shared_ptr<JVMOpParams> params);
            FR_Id id;
            struct ControlData
			{
				~ControlData();
                JVMWorkOperation workOperation;
				nigiri::StateMachine<JVMState,JVMEvent,JVMState::Created> stateMachine;
			    JVMOp jvmOp;
                std::shared_ptr<JVMOpParams> jvmOpParams;
            };
			ControlData controlData;
			std::thread jvmThread;
			std::shared_ptr<FR_Type> TYPE_INT64;
		};



    }
}
