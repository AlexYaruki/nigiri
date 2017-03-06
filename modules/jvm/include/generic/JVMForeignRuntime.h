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

        class JVMMethod : public nigiri::FR_Method {
        public:
            JVMMethod(FR_Id, jmethodID, std::shared_ptr<FR_Type>);
            FR_Id getRuntimeId();
            jmethodID getMethod();
			std::shared_ptr<FR_Type> getReturnType();
        private:
            FR_Id runtimeId;
            jmethodID method;
			std::shared_ptr<FR_Type> returnType;
        };

        class JVMOpParams {
        public:
            virtual ~JVMOpParams() = default;
        };

		class JVMObjectBase : public nigiri::FR_Object {
		public:
			virtual ~JVMObjectBase() = default;
			std::shared_ptr<FR_Type> getType();
			FR_Id getRuntimeId();
			virtual jvalue toValue() = 0;
			virtual std::experimental::optional<int64_t> castToInt64();
	        virtual std::experimental::optional<double> castToDouble();
		protected:
			std::shared_ptr<FR_Type> type;
			FR_Id runtimeId;
		};

		class JVM_Int64 : public nigiri::internal::JVMObjectBase {
		public:
			JVM_Int64(int64_t v, std::shared_ptr<JVMType_Int64> vType, FR_Id id);
			std::experimental::optional<int64_t> castToInt64();
			jvalue toValue();
		private:
			jlong value;
		};

		class JVM_Double : public nigiri::internal::JVMObjectBase {
		public:
			JVM_Double(double v, std::shared_ptr<JVMType_Double> vType, FR_Id id);
			std::experimental::optional<double> castToDouble();
			jvalue toValue();
		private:
			jdouble value;
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
			std::shared_ptr<FR_Object> wrapPrimitive(double d) override;
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

			std::shared_ptr<FR_Type> TYPE_INT8;
			std::shared_ptr<FR_Type> TYPE_INT16;
			std::shared_ptr<FR_Type> TYPE_INT32;
			std::shared_ptr<FR_Type> TYPE_INT64;

			std::shared_ptr<FR_Type> TYPE_FLOAT;
			std::shared_ptr<FR_Type> TYPE_DOUBLE;
		};



    }
}
