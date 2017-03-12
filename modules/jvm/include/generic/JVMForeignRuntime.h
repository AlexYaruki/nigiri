#ifndef JVM_FOREIGN_RUNTIME_H
#define JVM_FOREIGN_RUNTIME_H

#undef NIGIRI_SHARED_BUILD

#include <jni.h>
#include <iostream>
#include <map>

#include <nigiri_foreignruntime.h>
#include <nigiri_statemachine.h>

#include "nigiri_jvm_build.h"
#include "JVMEnums.h"

namespace nigiri {
	namespace internal
	{
		const bool LOG_JVMTHREAD = false;
		const bool LOG_JVMFOREIGNRUNTIME = false;

        class JVMType : public nigiri::FR_Type {
        public:
            JVMType(FR_Id id ,jclass type, std::string name, std::string signatureName);
            ~JVMType();
			FR_Id getRuntimeId();
			const std::string& getName();
			const std::string& getSignatureName();
			bool isPrimitive();
			jclass getType();

        protected:
            jclass type;
			std::string name;
			std::string signatureName;
			bool primitive = false;
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

        class JVMMethod : public nigiri::FR_Method {
        public:
            JVMMethod(FR_Id, jmethodID, std::shared_ptr<FR_Type>, std::shared_ptr<FR_Type>);
            FR_Id getRuntimeId();
            jmethodID getMethod();
	        std::shared_ptr<FR_Type> getType();
			std::shared_ptr<FR_Type> getReturnType();
        private:
            FR_Id runtimeId;
            jmethodID method;
			std::shared_ptr<FR_Type> type;
			std::shared_ptr<FR_Type> returnType;
        };

		class JVMObjectBase : public nigiri::FR_Object {
		public:
			virtual ~JVMObjectBase() = default;
			std::shared_ptr<FR_Type> getType();
			FR_Id getRuntimeId();
			virtual jvalue toValue() = 0;
			virtual std::tuple<bool,uint16_t> castToUInt16();
	        virtual std::tuple<bool,bool> castToBool();
	        virtual std::tuple<bool,int8_t> castToInt8();
	        virtual std::tuple<bool,int16_t> castToInt16();
	        virtual std::tuple<bool,int32_t> castToInt32();
	        virtual std::tuple<bool,int64_t> castToInt64();
	        virtual std::tuple<bool,float> castToFloat();
	        virtual std::tuple<bool,double> castToDouble();
		protected:
			std::shared_ptr<FR_Type> type;
			FR_Id runtimeId;
		};

		class JVMObject : public JVMObjectBase {
		public:
			JVMObject(jobject o, std::shared_ptr<JVMType> type, FR_Id id);
			jobject getObject();
			jvalue toValue();
		private:
			jobject obj;
		};

		class JVMOpParams {
		public:
		    virtual ~JVMOpParams() = default;
		};

        using JVMOp = std::function<void(JNIEnv* env,std::shared_ptr<JVMOpParams> params)>;

		class JVMForeignRuntime : public nigiri::ForeignRuntime
		{
			friend class JVMThread;
		public:
			JVMForeignRuntime(FR_Id id);
			~JVMForeignRuntime();
            FR_Id getId() override;
			bool start(const std::initializer_list<std::string>& resources) override;
			void stop() override;
            bool isRunning() override;

            std::shared_ptr<FR_Type> lookupType(const std::string& name) override;

            std::shared_ptr<FR_Method> lookupMethod(std::shared_ptr<FR_Type> targetType,
													const std::string& name,
													const std::vector<std::shared_ptr<FR_Type>>& parameterTypes,
													const std::shared_ptr<FR_Type> returnType) override;

			std::shared_ptr<FR_Method> lookupMethod(std::shared_ptr<FR_Object> targetObject,
													const std::string& name,
													const std::vector<std::shared_ptr<FR_Type>>& parameterTypes,
													const std::shared_ptr<FR_Type> returnType) override;

			std::shared_ptr<FR_Method> lookupConstructor(std::shared_ptr<FR_Type> targetType,
														 const std::vector<std::shared_ptr<FR_Type>>& parameterTypes) override;

			std::shared_ptr<nigiri::FR_Object> call(std::shared_ptr<FR_Type> targetType,
                                                    std::shared_ptr<FR_Method> method,
                                                    const std::vector<std::shared_ptr<FR_Object>>& parameters) override;

            std::shared_ptr<nigiri::FR_Object> call(std::shared_ptr<FR_Object> targetObject,
                                                    std::shared_ptr<FR_Method> method,
                                                    const std::vector<std::shared_ptr<FR_Object>>& parameters) override;

			std::shared_ptr<FR_Object> createObject(std::shared_ptr<FR_Type> type,
													std::shared_ptr<FR_Method> constructor,
													const std::vector<std::shared_ptr<FR_Object>>& parameters) override;

			void waitForInitialization();
			void notifyWorkPrepared();
			void waitForWorkCompleted();

			std::shared_ptr<FR_Object> wrapPrimitive(uint16_t p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(bool p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(int8_t p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(int16_t p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(int32_t p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(int64_t p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(float p) override;
	        std::shared_ptr<FR_Object> wrapPrimitive(double p) override;

			std::string toString(std::shared_ptr<FR_Object> obj) override;

		private:
			bool isAvailable();
			std::string prepareMethodSignature(const std::vector<std::shared_ptr<FR_Type>> &parametersTypes,
	                                           const std::shared_ptr<FR_Type> returnType);
            void execute(JVMOp jvmOp, std::shared_ptr<JVMOpParams> params);
			std::string extractString(std::shared_ptr<JVMObject> obj);
			void check(const std::shared_ptr<FR_Type>& type);
			void check(const std::shared_ptr<FR_Method>& method);
			void check(const std::shared_ptr<FR_Object>& object);

            FR_Id id;
			std::map<std::string,std::shared_ptr<FR_Type>> typeCache;
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

			std::shared_ptr<FR_Type> TYPE_CHAR;
			std::shared_ptr<FR_Type> TYPE_BOOLEAN;

			std::shared_ptr<FR_Type> TYPE_INT8;
			std::shared_ptr<FR_Type> TYPE_INT16;
			std::shared_ptr<FR_Type> TYPE_INT32;
			std::shared_ptr<FR_Type> TYPE_INT64;

			std::shared_ptr<FR_Type> TYPE_FLOAT;
			std::shared_ptr<FR_Type> TYPE_DOUBLE;
		};

		std::string getThreadIdString(std::thread::id tid);

    }
}

#endif
