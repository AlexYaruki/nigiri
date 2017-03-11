#include <nigiri.h>
#include <vector>
#include <algorithm>
#include "JVMForeignRuntime.h"
#include "JVMPrimitives.h"
#include "JVMOpParams.h"
#include "JVMThread.h"

namespace nigiri
{
	namespace internal
	{

		// JVMType /////////////////////////////////////////////////////////////

        JVMType::JVMType(FR_Id id, jclass type_, std::string typeName, std::string sName) {
            type = type_;
            runtimeId = id;
			name = typeName;
			signatureName = sName;
        }

        JVMType::~JVMType() {

        }

        FR_Id JVMType::getRuntimeId() {
            return runtimeId;
        }

        jclass JVMType::getType() {
            return type;
        }

		const std::string& JVMType::getName() {
			return name;
		}

		const std::string& JVMType::getSignatureName() {
			return signatureName;
		}

		bool JVMType::isPrimitive() {
			return primitive;
		}

		/////////////////////////////////////////////////////////////////////////

		// JVMObjectBase ////////////////////////////////////////////////////////

		std::shared_ptr<FR_Type> JVMObjectBase::getType(){
			return type;
		}

		std::tuple<bool,uint16_t> JVMObjectBase::castToUInt16() {
			return std::tuple<bool,uint16_t>();
		}

		std::tuple<bool,bool> JVMObjectBase::castToBool() {
	        return std::tuple<bool,bool>();
	    }

		std::tuple<bool,int8_t> JVMObjectBase::castToInt8() {
	        return std::tuple<bool,int8_t>();
	    }

		std::tuple<bool,int16_t> JVMObjectBase::castToInt16() {
	        return std::tuple<bool,int16_t>();
	    }

		std::tuple<bool,int32_t> JVMObjectBase::castToInt32() {
	        return std::tuple<bool,int32_t>();
	    }

		std::tuple<bool,int64_t> JVMObjectBase::castToInt64() {
	        return std::tuple<bool,int64_t>();
	    }

		std::tuple<bool,float> JVMObjectBase::castToFloat() {
			return std::tuple<bool,float>();
		}

	    std::tuple<bool,double> JVMObjectBase::castToDouble() {
	        return std::tuple<bool,double>();
	    }

		FR_Id JVMObjectBase::getRuntimeId(){
			return runtimeId;
		}

		/////////////////////////////////////////////////////////////////////////

		// JVMObject ////////////////////////////////////////////////////////////

		JVMObject::JVMObject(jobject o, std::shared_ptr<JVMType> t, FR_Id id) {
			obj = o;
			type = t;
			runtimeId = id;
		}

		jobject JVMObject::getObject() {
			return obj;
		}

		jvalue JVMObject::toValue() {
			jvalue v;
            v.l = obj;
            return v;
		}

		/////////////////////////////////////////////////////////////////////////

		// JVMMethod ////////////////////////////////////////////////////////////

        JVMMethod::JVMMethod(FR_Id id, jmethodID method, std::shared_ptr<FR_Type> rT): runtimeId(id), method(method), returnType(rT) {}

        jmethodID JVMMethod::getMethod() {
            return method;
        }

        FR_Id JVMMethod::getRuntimeId() {
            return runtimeId;
        }

		std::shared_ptr<FR_Type> JVMMethod::getReturnType() {
			return returnType;
		}

		/////////////////////////////////////////////////////////////////////////

		JVMForeignRuntime::JVMForeignRuntime(FR_Id id_){
			id = id_;

			TYPE_CHAR = std::make_shared<JVMType_Char>(getId(), nullptr);
			TYPE_BOOLEAN = std::make_shared<JVMType_Boolean>(getId(), nullptr);

			TYPE_INT8 = std::make_shared<JVMType_Int8>(getId(), nullptr);
			TYPE_INT16 = std::make_shared<JVMType_Int16>(getId(), nullptr);
			TYPE_INT32 = std::make_shared<JVMType_Int32>(getId(), nullptr);
			TYPE_INT64 = std::make_shared<JVMType_Int64>(getId(), nullptr);

			TYPE_FLOAT = std::make_shared<JVMType_Float>(getId(), nullptr);
			TYPE_DOUBLE = std::make_shared<JVMType_Double>(getId(), nullptr);

			controlData.stateMachine.registerStateToString(getStateString);
			controlData.stateMachine.registerEventToString(getEventString);

			controlData.stateMachine.addConnection(JVMState::Created,JVMEvent::Init_Error,JVMState::ErrorShutdown);
			controlData.stateMachine.addConnection(JVMState::Created,JVMEvent::Init_Success,JVMState::Started);

			controlData.stateMachine.addConnection(JVMState::Started, JVMEvent::JVM_Destroyed, JVMState::Shutdown);
			controlData.stateMachine.addConnection(JVMState::Started,JVMEvent::Work_Prepared,JVMState::WorkPrepared);

			controlData.stateMachine.addConnection(JVMState::WorkPrepared, JVMEvent::Work_Completed, JVMState::WorkCompleted);

			controlData.stateMachine.addConnection(JVMState::WorkCompleted,JVMEvent::Work_Prepared,JVMState::WorkPrepared);
			controlData.stateMachine.addConnection(JVMState::WorkCompleted, JVMEvent::Work_Idle, JVMState::Started);

		}

		JVMForeignRuntime::~JVMForeignRuntime() {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "DEBUG - JVMForeignRuntime - dtor" << std::endl;
			}
			stop();
		}

		void JVMForeignRuntime::waitForInitialization()
		{
			if(LOG_JVMFOREIGNRUNTIME) {
            	std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Waiting for JVM startup ..." << std::endl;
			}
			controlData.stateMachine.waitForStates({JVMState::Started,JVMState::ErrorShutdown});
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVM startup completed" << std::endl;
			}
		}

		bool JVMForeignRuntime::start()
		{
			if(LOG_JVMFOREIGNRUNTIME) {
            	std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Starting JVM ..." << std::endl;
			}
			jvmThread = std::thread([](ControlData* controlData_)
			{
				if (LOG_JVMTHREAD) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Start of JVMThread" << std::endl;
				}
				JVMThread jvmThread(controlData_);
				bool jvmCreated = jvmThread.createJVM();
				if (!jvmCreated)
				{
					if(LOG_JVMTHREAD) {
						std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - JVM Cannot be created" << std::endl;
					}
					jvmThread.notifyRollback();
				}
				else
				{
					if(LOG_JVMTHREAD) {
						std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - JVM Created" << std::endl;
					    std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Starting work loop" << std::endl;
					}
					jvmThread.workLoop();
				}
				jvmThread.quit();
				if(LOG_JVMTHREAD) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - End of JVMThread" << std::endl;
				}
			}, &controlData);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread started" << std::endl;
			}
			waitForInitialization();
            if(controlData.stateMachine.getState() == JVMState::Started){
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVM is work-ready" << std::endl;
				}
				return true;
            } else {
				if(LOG_JVMFOREIGNRUNTIME) {
                	std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVM could not be started" << std::endl;
				}
				jvmThread.join();
                return false;
            }
		}

        bool JVMForeignRuntime::isRunning() {
			JVMState state = controlData.stateMachine.getState();
			return (state != JVMState::Created) &&
				   (state != JVMState::ErrorShutdown) &&
				   (state != JVMState::Shutdown);
		}

		void JVMForeignRuntime::stop()
		{
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Beginning of JVMThread shutdown" << std::endl;
			}
			if(isRunning()){
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVMThread is running" << std::endl;
				}
				controlData.workOperation = JVMWorkOperation::Shutdown;
                notifyWorkPrepared();
                waitForWorkCompleted();
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVMThread confirmation received" << std::endl;
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Joining JVM thread ..." << std::endl;
				}
				jvmThread.join();
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVM thread joined" << std::endl;
				}
			} else {
				if(LOG_JVMFOREIGNRUNTIME) {
                	std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVMThread is not running" << std::endl;
				}
			}
		}

		FR_Id JVMForeignRuntime::getId() {
			return id;
		}

		std::shared_ptr<FR_Type> JVMForeignRuntime::lookupType(const std::string& name) {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Looking for type: (" << name << ")" << std::endl;
			}
			if(name.compare("byte") == 0){
                return TYPE_INT8;
            } else if(name.compare("short") == 0){
                return TYPE_INT16;
            } else if(name.compare("int") == 0){
                return TYPE_INT32;
            } else if(name.compare("long") == 0){
                return TYPE_INT64;
            } else if(name.compare("float") == 0){
                return TYPE_FLOAT;
            } else if(name.compare("double") == 0){
                return TYPE_DOUBLE;
            }

			std::shared_ptr<JVMOpParams_TypeLookup> params = std::make_shared<JVMOpParams_TypeLookup>();

			params->typeName = name;
            auto& localId = id;
			auto op = [localId](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams) {
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "Started type lookup ..." << std::endl;
				}
				auto typeLookupParams = std::static_pointer_cast<JVMOpParams_TypeLookup>(opParams);
				std::string javaTypeName = typeLookupParams->typeName;
				std::replace(javaTypeName.begin(), javaTypeName.end(),'.','/');
				jclass type = env->FindClass(javaTypeName.c_str());
				if(type != nullptr) {
					javaTypeName = "L" + javaTypeName + ";";
					typeLookupParams->type = std::make_shared<JVMType>(localId,type,typeLookupParams->typeName,javaTypeName);
					if(LOG_JVMFOREIGNRUNTIME) {
						std::cout << ">>>> DEBUG: Type (" << typeLookupParams->typeName << ") found" << std::endl;
					}
				} else {
					if(LOG_JVMFOREIGNRUNTIME) {
	                	std::cout << ">>>> DEBUG: Type (" << typeLookupParams->typeName << ") not found" << std::endl;
					}
					typeLookupParams->type = nullptr;
				}
			};
			execute(op,params);
			return params->type;
		}

        std::string JVMForeignRuntime::prepareMethodSignature(const std::vector<std::shared_ptr<FR_Type>> &parametersTypes,
                                           const std::shared_ptr<FR_Type> returnType){
            std::string signature;
            signature.append("(");
            for(std::vector<std::shared_ptr<FR_Type>>::size_type i = 0; i < parametersTypes.size(); i++) {
                auto jvmType = std::static_pointer_cast<JVMType>(parametersTypes[i]);
				signature.append(jvmType->getSignatureName());
            }
			signature.append(")");
			if(returnType != nullptr) {
				signature.append(std::static_pointer_cast<JVMType>(returnType)->getSignatureName());
			} else {
				signature.append("V");
			}
            return signature;
        }

		std::shared_ptr<FR_Method> JVMForeignRuntime::lookupMethod(std::shared_ptr<FR_Object> targetObject,
																   const std::string &name,
																   const std::vector<std::shared_ptr<FR_Type>> &parametersTypes,
																   const std::shared_ptr<FR_Type> returnType) {
			std::string methodSignature = prepareMethodSignature(parametersTypes,returnType);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Signature: " << methodSignature << std::endl;
				std::cout << ">>>> DEBUG: Looking for method: (" << name << ")" << std::endl;
			}
			auto params = std::make_shared<JVMOpParams_MethodLookup>();
			params->type = targetObject->getType();
			params->methodName = name;
			params->methodSignature = methodSignature;
			auto op = [](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
				auto methodLookupParams = std::static_pointer_cast<JVMOpParams_MethodLookup>(opParams);
				if(LOG_JVMFOREIGNRUNTIME) {
                	std::cout << ">>>> DEBUG: Looking for method: (" << methodLookupParams->methodName << ")" << std::endl;
				}
				auto type = std::static_pointer_cast<JVMType>(methodLookupParams->type);
				jmethodID method = env->GetMethodID(type->getType(),methodLookupParams->methodName.c_str(),methodLookupParams->methodSignature.c_str());
				if(LOG_JVMFOREIGNRUNTIME) {
	            	if(method != nullptr) {
						std::cout << ">>>> DEBUG: Method (" << methodLookupParams->methodName << ") found" << std::endl;
					} else {
						std::cout << ">>>> DEBUG: Method (" << methodLookupParams->methodName << ") not found" << std::endl;
					}
				}
				methodLookupParams->method = method;
			};
			execute(op,params);
			if(params->method == nullptr) {
				return nullptr;
			}
			return std::make_shared<JVMMethod>(getId(),params->method,returnType);
		}

		std::shared_ptr<FR_Method> JVMForeignRuntime::lookupMethod(std::shared_ptr<FR_Type> targetType,
																   const std::string &name,
																   const std::vector<std::shared_ptr<FR_Type>> &parametersTypes,
                                                                   const std::shared_ptr<FR_Type> returnType) {
            std::string methodSignature = prepareMethodSignature(parametersTypes,returnType);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Signature: " << methodSignature << std::endl;
				std::cout << ">>>> DEBUG: Looking for method: (" << name << ")" << std::endl;
			}
			auto params = std::make_shared<JVMOpParams_MethodLookup>();
            params->type = targetType;
            params->methodName = name;
			params->methodSignature = methodSignature;
            auto op = [](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
                auto methodLookupParams = std::static_pointer_cast<JVMOpParams_MethodLookup>(opParams);
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << ">>>> DEBUG: Looking for method: (" << methodLookupParams->methodName << ")" << std::endl;
				}
				auto type = std::static_pointer_cast<JVMType>(methodLookupParams->type);
                jmethodID method = env->GetStaticMethodID(type->getType(),methodLookupParams->methodName.c_str(),methodLookupParams->methodSignature.c_str());
				//if(LOG_JVMFOREIGNRUNTIME) {
					if(method != nullptr) {
	                    std::cout << ">>>> DEBUG: Method (" << methodLookupParams->methodName << ") found" << std::endl;
	                } else {
	                    std::cout << ">>>> DEBUG: Method (" << methodLookupParams->methodName << ") not found" << std::endl;
	                }
				//}
                methodLookupParams->method = method;
            };
            execute(op,params);
			if(params->method == nullptr) {
				return nullptr;
			}
            return std::make_shared<JVMMethod>(getId(),params->method,returnType);
		}

		std::shared_ptr<FR_Method> JVMForeignRuntime::lookupConstructor(std::shared_ptr<FR_Type> targetType,
													 const std::vector<std::shared_ptr<FR_Type>>& parametersTypes) {
		    std::string methodSignature = prepareMethodSignature(parametersTypes,nullptr);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Signature: " << methodSignature << std::endl;
				std::cout << ">>>> DEBUG: Looking for constructor ..." << std::endl;
			}
			auto params = std::make_shared<JVMOpParams_MethodLookup>();
			params->type = targetType;
			params->methodName = "<init>";
			params->methodSignature = methodSignature;
			auto op = [](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
				auto methodLookupParams = std::static_pointer_cast<JVMOpParams_MethodLookup>(opParams);
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << ">>>> DEBUG: Looking for constructor: (" << methodLookupParams->methodName << ")" << std::endl;
				}
				auto type = std::static_pointer_cast<JVMType>(methodLookupParams->type);
				jmethodID method = env->GetMethodID(type->getType(),methodLookupParams->methodName.c_str(),methodLookupParams->methodSignature.c_str());
				if(LOG_JVMFOREIGNRUNTIME) {
					if(method != nullptr) {
				    	std::cout << ">>>> DEBUG: Constructor found" << std::endl;
					} else {
						std::cout << ">>>> DEBUG: Constructor not found" << std::endl;
					}
				}
				methodLookupParams->method = method;
			};
			execute(op,params);
			if(params->method == nullptr) {
				return nullptr;
			}
			return std::make_shared<JVMMethod>(getId(),params->method,nullptr);
		}

		std::shared_ptr<nigiri::FR_Object> JVMForeignRuntime::call(std::shared_ptr<FR_Object> targetObject,
									 std::shared_ptr<FR_Method> method,
									 const std::vector<std::shared_ptr<FR_Object>> &parameters) {
			 auto targetObjectType = std::static_pointer_cast<JVMType>(targetObject->getType());
			 if(targetObjectType->isPrimitive()){
				 throw "Attempt to call method on primitive type value";
			 }
			 if(LOG_JVMFOREIGNRUNTIME) {
			 	std::cout << ">>>> DEBUG: Invoking object method ..." << std::endl;
		 	 }
			 auto params = std::make_shared<JVMOpParams_InstanceMethodCall>();
			 params->object = targetObject;
			 params->method = method;
			 params->parameters = &parameters;
			 auto op = [this](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
			 	auto instanceMethodCall_params = std::static_pointer_cast<JVMOpParams_InstanceMethodCall>(opParams);
			 	auto jvmObject = std::static_pointer_cast<JVMObject>(instanceMethodCall_params->object);
			 	auto jvmMethod = std::static_pointer_cast<JVMMethod>(instanceMethodCall_params->method);
			 	auto callParams = new jvalue[instanceMethodCall_params->parameters->size()];
			 	for(size_t i = 0; i < instanceMethodCall_params->parameters->size(); i++) {
			 		auto jvmParam = std::static_pointer_cast<JVMObjectBase>(instanceMethodCall_params->parameters->at(i));
			 		callParams[i] = jvmParam->toValue();
			 	}
			 	auto returnType = jvmMethod->getReturnType();
			 	if(returnType == nullptr) {
			 		env->CallVoidMethodA(jvmObject->getObject(),
			 								   jvmMethod->getMethod(),
			 								   callParams);
			 	} else if(!std::static_pointer_cast<JVMType>(returnType)->isPrimitive()) {
			 		jobject obj = env->CallObjectMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVMObject>(obj,
			 																	  std::static_pointer_cast<JVMType>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_CHAR){
			 		jchar result = env->CallCharMethodA(jvmObject->getObject(),
			 												  jvmMethod->getMethod(),
			 												  callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Char>(result,
			 																	  std::static_pointer_cast<JVMType_Char>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_BOOLEAN){
			 		jboolean result = env->CallBooleanMethodA(jvmObject->getObject(),
			 														jvmMethod->getMethod(),
			 														callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Boolean>(result == JNI_TRUE ? true : false,
			 																	  std::static_pointer_cast<JVMType_Boolean>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_INT8) {
			 		jbyte result = env->CallByteMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Int8>(result,
			 																	  std::static_pointer_cast<JVMType_Int8>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_INT16) {
			 		jshort result = env->CallShortMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Int16>(result,
			 																	  std::static_pointer_cast<JVMType_Int16>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_INT32) {
			 		jint result = env->CallIntMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Int32>(result,
			 																	  std::static_pointer_cast<JVMType_Int32>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_INT64) {
			 		jlong result = env->CallLongMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Int64>(result,
			 																	  std::static_pointer_cast<JVMType_Int64>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_FLOAT){
			 		jfloat result = env->CallFloatMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Float>(result,
			 																	  std::static_pointer_cast<JVMType_Float>(returnType),
			 																	  getId());
			 	} else if(returnType == TYPE_DOUBLE){
			 		jdouble result = env->CallDoubleMethodA(jvmObject->getObject(),
			 												 jvmMethod->getMethod(),
			 												 callParams);
			 		instanceMethodCall_params->result = std::make_shared<JVM_Double>(result,
			 																	  std::static_pointer_cast<JVMType_Double>(returnType),
			 																	  getId());
			 	} else {
			 		throw 42;
			 	}
			 	delete[] callParams;
			 };
			 execute(op,params);
			 if(LOG_JVMFOREIGNRUNTIME) {
			 	std::cout << ">>>> DEBUG: Object method returned" << std::endl;
			 }
			 return params->result;
		}

		std::shared_ptr<nigiri::FR_Object> JVMForeignRuntime::call(std::shared_ptr<FR_Type> targetType,
									 std::shared_ptr<FR_Method> method,
									 const std::vector<std::shared_ptr<FR_Object>> &parameters) {
		 	if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Invoking type method ..." << std::endl;
			}
			auto params = std::make_shared<JVMOpParams_StaticMethodCall>();
			params->type = targetType;
			params->method = method;
			params->parameters = &parameters;
			auto op = [this](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
				auto staticMethodCall_params = std::static_pointer_cast<JVMOpParams_StaticMethodCall>(opParams);
				auto jvmType = std::static_pointer_cast<JVMType>(staticMethodCall_params->type);
				auto jvmMethod = std::static_pointer_cast<JVMMethod>(staticMethodCall_params->method);
				auto callParams = new jvalue[staticMethodCall_params->parameters->size()];
				for(size_t i = 0; i < staticMethodCall_params->parameters->size(); i++) {
					auto jvmParam = std::static_pointer_cast<JVMObjectBase>(staticMethodCall_params->parameters->at(i));
					callParams[i] = jvmParam->toValue();
				}
				auto returnType = jvmMethod->getReturnType();
				if(returnType == nullptr) {
					env->CallStaticVoidMethodA(jvmType->getType(),
										       jvmMethod->getMethod(),
									           callParams);
				} else if(!std::static_pointer_cast<JVMType>(returnType)->isPrimitive()) {
					jobject obj = env->CallStaticObjectMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVMObject>(obj,
										 										  std::static_pointer_cast<JVMType>(returnType),
										 										  getId());
				} else if(returnType == TYPE_CHAR){
				    jchar result = env->CallStaticCharMethodA(jvmType->getType(),
										                      jvmMethod->getMethod(),
									                          callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Char>(result,
										                                          std::static_pointer_cast<JVMType_Char>(returnType),
													                              getId());
				} else if(returnType == TYPE_BOOLEAN){
				    jboolean result = env->CallStaticBooleanMethodA(jvmType->getType(),
										                            jvmMethod->getMethod(),
									                                callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Boolean>(result,
										                                          std::static_pointer_cast<JVMType_Boolean>(returnType),
													                              getId());
				} else if(returnType == TYPE_INT8) {
					jbyte result = env->CallStaticByteMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Int8>(result,
										                                          std::static_pointer_cast<JVMType_Int8>(returnType),
													                              getId());
				} else if(returnType == TYPE_INT16) {
					jshort result = env->CallStaticShortMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Int16>(result,
										                                          std::static_pointer_cast<JVMType_Int16>(returnType),
													                              getId());
				} else if(returnType == TYPE_INT32) {
					jint result = env->CallStaticIntMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Int32>(result,
										                                          std::static_pointer_cast<JVMType_Int32>(returnType),
													                              getId());
				} else if(returnType == TYPE_INT64) {
					jlong result = env->CallStaticLongMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Int64>(result,
										                                          std::static_pointer_cast<JVMType_Int64>(returnType),
													                              getId());
				} else if(returnType == TYPE_FLOAT){
				    jfloat result = env->CallStaticFloatMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Float>(result,
										                                          std::static_pointer_cast<JVMType_Float>(returnType),
													                              getId());
				} else if(returnType == TYPE_DOUBLE){
				    jdouble result = env->CallStaticDoubleMethodA(jvmType->getType(),
										                     jvmMethod->getMethod(),
									                         callParams);
					staticMethodCall_params->result = std::make_shared<JVM_Double>(result,
										                                          std::static_pointer_cast<JVMType_Double>(returnType),
													                              getId());
				} else {
					throw 42;
				}
				delete[] callParams;
			};
			execute(op,params);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Type method returned" << std::endl;
			}
			return params->result;
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::createObject(std::shared_ptr<FR_Type> type,
												std::shared_ptr<FR_Method> constructor,
												const std::vector<std::shared_ptr<FR_Object>>& parameters) {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Creating object ..." << std::endl;
			}
			auto params = std::make_shared<JVMOpParams_ObjectConstruction>();
			params->type = type;
			params->constructor = constructor;
			params->parameters = &parameters;
			auto op = [this](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
				auto staticMethodCall_params = std::static_pointer_cast<JVMOpParams_StaticMethodCall>(opParams);
				auto jvmType = std::static_pointer_cast<JVMType>(staticMethodCall_params->type);
				auto jvmMethod = std::static_pointer_cast<JVMMethod>(staticMethodCall_params->method);
				jobject result;
				if(staticMethodCall_params->parameters->size() > 0) {
					auto callParams = new jvalue[staticMethodCall_params->parameters->size()];
					for(size_t i = 0; i < staticMethodCall_params->parameters->size(); i++) {
						auto jvmParam = std::static_pointer_cast<JVMObjectBase>(staticMethodCall_params->parameters->at(i));
						callParams[i] = jvmParam->toValue();
					}
					auto returnType = jvmMethod->getReturnType();
					result = env->NewObjectA(jvmType->getType(),jvmMethod->getMethod(),callParams);
					delete[] callParams;

				} else {
					std::cout << "No argument constructor" << std::endl;
					result = env->NewObject(jvmType->getType(),jvmMethod->getMethod());
				}
				staticMethodCall_params->result = std::make_shared<JVMObject>(result,jvmType,getId());
			};
			execute(op,params);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Object created" << std::endl;
			}
			return params->result;
		}

		void JVMForeignRuntime::execute(JVMOp jvmOp, std::shared_ptr<JVMOpParams> params) {
			controlData.jvmOp = jvmOp;
			controlData.jvmOpParams = params;
			notifyWorkPrepared();
			waitForWorkCompleted();
		}

		void JVMForeignRuntime::notifyWorkPrepared() {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Work prepared" << std::endl;
			}
			controlData.stateMachine.submitEvent(JVMEvent::Work_Prepared);
        }

		void JVMForeignRuntime::waitForWorkCompleted() {
            controlData.stateMachine.waitForState(JVMState::WorkCompleted);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Work completed" << std::endl;
			}
			controlData.stateMachine.submitEvent(JVMEvent::Work_Idle);
         }

			bool JVMForeignRuntime::isAvailable(){
			JVMState state = controlData.stateMachine.getState();
			if(state == JVMState::Started || state == JVMState::WorkCompleted) {
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - System ready for work" << std::endl;
				}
				return true;
			} else {
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - System not ready for work, current state: " << getStateString(state) << std::endl;
				}
				return false;
		 	}
		}

		JVMForeignRuntime::ControlData::~ControlData() {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "DEBUG - JVMForeignRuntime::ControlData - dtor" << std::endl;
			}
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(uint16_t value) {
			return std::make_shared<JVM_Char>(
				value,
				std::static_pointer_cast<JVMType_Char>(TYPE_CHAR),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(bool value) {
			return std::make_shared<JVM_Boolean>(
				value,
				std::static_pointer_cast<JVMType_Boolean>(TYPE_BOOLEAN),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(int8_t value) {
			return std::make_shared<JVM_Int8>(
				value,
				std::static_pointer_cast<JVMType_Int8>(TYPE_INT8),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(int16_t value) {
			return std::make_shared<JVM_Int16>(
				value,
				std::static_pointer_cast<JVMType_Int16>(TYPE_INT16),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(int32_t value) {
			return std::make_shared<JVM_Int32>(
				value,
				std::static_pointer_cast<JVMType_Int32>(TYPE_INT32),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(int64_t value) {
			return std::make_shared<JVM_Int64>(
				value,
				std::static_pointer_cast<JVMType_Int64>(TYPE_INT64),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(float value) {
			return std::make_shared<JVM_Float>(
				value,
				std::static_pointer_cast<JVMType_Float>(TYPE_FLOAT),
				getId());
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::wrapPrimitive(double value) {
			return std::make_shared<JVM_Double>(
				value,
				std::static_pointer_cast<JVMType_Double>(TYPE_DOUBLE),
				getId());
		}

		std::string JVMForeignRuntime::extractString(std::shared_ptr<JVMObject> obj){
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Extracting string ..." << std::endl;
			}
			auto params = std::make_shared<JVMOpParams_StringExtraction>();
			params->target = obj;
			auto op = [this](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams){
				auto opParams_StringExtraction = std::static_pointer_cast<JVMOpParams_StringExtraction>(opParams);
				jstring jvmStr = static_cast<jstring>(opParams_StringExtraction->target->getObject());
				jsize length = env->GetStringLength(jvmStr);
				char* data = new char[length];
				jsize start = 0;
				env->GetStringUTFRegion(jvmStr,start,length,data);
				opParams_StringExtraction->result = std::string(data);
				delete[] data;
			};
			execute(op,params);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: String extracted" << std::endl;
			}
			return params->result;
		}

		std::string JVMForeignRuntime::toString(std::shared_ptr<FR_Object> obj) {
			auto jvmObjectBase = std::static_pointer_cast<JVMObjectBase>(obj);
			auto jvmType = std::static_pointer_cast<JVMType>(obj->getType());
			if(jvmType->isPrimitive()) {
				return "";
			} else {
				auto jvmObject = std::static_pointer_cast<JVMObject>(jvmObjectBase);
				if(jvmType->getName().compare("java.lang.String") == 0) {
					return extractString(jvmObject);
				} else {
					return "";
				}
			}
		}

		std::string getThreadIdString(std::thread::id tid){
	        static std::hash<std::thread::id> hasher;
	        auto hash = hasher(tid);
	        return std::to_string(hash);
	    }

	}
}
