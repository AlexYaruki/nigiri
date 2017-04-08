#include <nigiri.h>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include "JVMForeignRuntime.h"
#include "JVMPrimitives.h"
#include "JVMOpParams.h"
#include "JVMThread.h"
#include "JVMInternalWrappers.h"

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

		JVMType::~JVMType() {}

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


		bool JVMType::isGeneric() {
			return false;
		}


		JVMGenericType::JVMGenericType(FR_Id id, jclass type_, std::string typeName, std::string sName) : JVMType(id,type_,typeName,sName) {
			
		}

		bool JVMGenericType::isGeneric() {
			return true;
		}
		
		void JVMGenericType::setTypeParameterInfo(const std::map<std::string, std::vector<std::string>>& tPI)
		{
			typeParameterInfo = tPI;
		}

		//TODO: Respect Java language scope
		const StaticMethodCaller& JVMType::getStaticMethodCaller() {
			static StaticMethodCaller caller = [](auto env, auto targetType, auto method, auto jniParams) {
				auto obj = env->CallStaticObjectMethodA(targetType->getType(),method->getMethod(),jniParams);
				auto jvmReturnType = std::static_pointer_cast<JVMType>(method->getReturnType());
				return std::make_shared<JVMObject>(obj,jvmReturnType,targetType->getRuntimeId());
			};
			return caller;
		}

		//TODO: Respect Java language scope
		const InstanceMethodCaller& JVMType::getInstanceMethodCaller() {
			static InstanceMethodCaller caller = [](auto env, auto targetObject, auto method, auto jniParams) {
				auto obj = env->CallObjectMethodA(targetObject->getObject(),method->getMethod(),jniParams);
				auto jvmReturnType = std::static_pointer_cast<JVMType>(method->getReturnType());
				return std::make_shared<JVMObject>(obj,jvmReturnType,targetObject->getRuntimeId());
			};
			return caller;
		}

		//TODO: Respect Java language scope
		const StaticFieldAccessor& JVMType::getStaticFieldAccessor() {
			static StaticFieldAccessor accessor = [](auto env, auto targetType, auto field) {
				auto obj = env->GetStaticObjectField(targetType->getType(), field->getField());
				auto jvmFieldType = std::static_pointer_cast<JVMType>(field->getType());
				return std::make_shared<JVMObject>(obj,jvmFieldType,targetType->getRuntimeId());
			};
			return accessor;
		}

		//TODO: Respect Java language scope
		const InstanceFieldAccessor& JVMType::getInstanceFieldAccessor() {
			static InstanceFieldAccessor accessor = [](auto env, auto targetObject, auto field) {
				auto obj = env->GetObjectField(targetObject->getObject(), field->getField());
				auto jvmFieldType = std::static_pointer_cast<JVMType>(field->getType());
				return std::make_shared<JVMObject>(obj,jvmFieldType,targetObject->getRuntimeId());
			};
			return accessor;
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

		JVMMethod::JVMMethod(FR_Id id, jmethodID method,std::shared_ptr<FR_Type> t, std::shared_ptr<FR_Type> rT): runtimeId(id), method(method), type(t), returnType(rT) {}                                                                          

		jmethodID JVMMethod::getMethod() {
			return method;
		}

		FR_Id JVMMethod::getRuntimeId() {
			return runtimeId;
		}

		std::shared_ptr<FR_Type> JVMMethod::getType() {
			return type;
		}

		std::shared_ptr<FR_Type> JVMMethod::getReturnType() {
			return returnType;
		}

		/////////////////////////////////////////////////////////////////////////

		// JVMField ////////////////////////////////////////////////////////////

		JVMField::JVMField(FR_Id id, jfieldID field, std::shared_ptr<FR_Type> parentType, std::shared_ptr<FR_Type> type):
			runtimeId(id),
			field(field),
			parentType(parentType),
			type(type) {}

		jfieldID JVMField::getField() {
			return field;
		}

		FR_Id JVMField::getRuntimeId() {
			return runtimeId;
		}

		std::shared_ptr<FR_Type> JVMField::getParentType() {
			return parentType;
		}

		std::shared_ptr<FR_Type> JVMField::getType() {
			return type;
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

			controlData.workOperation = JVMWorkOperation::ExecuteOp;
			controlData.stateMachine.registerStateToString(getStateString);
			controlData.stateMachine.registerEventToString(getEventString);

			controlData.stateMachine.addConnection(JVMState::Created,JVMEvent::Init_Error,JVMState::ErrorShutdown);
			controlData.stateMachine.addConnection(JVMState::Created,JVMEvent::Init_Success,JVMState::Started);

			controlData.stateMachine.addConnection(JVMState::Started, JVMEvent::JVM_Destroyed, JVMState::Shutdown);
			controlData.stateMachine.addConnection(JVMState::Started,JVMEvent::Work_Prepared,JVMState::WorkPrepared);

			controlData.stateMachine.addConnection(JVMState::WorkPrepared, JVMEvent::Work_Completed, JVMState::WorkCompleted);

			controlData.stateMachine.addConnection(JVMState::WorkCompleted,JVMEvent::Work_Prepared,JVMState::WorkPrepared);
			controlData.stateMachine.addConnection(JVMState::WorkCompleted, JVMEvent::Work_Idle, JVMState::Started);
			typeCache.insert({ "byte",TYPE_INT8});
			typeCache.insert({ "short",TYPE_INT16 });
			typeCache.insert({ "int",TYPE_INT32 });
			typeCache.insert({ "long",TYPE_INT64 });
			typeCache.insert({ "float",TYPE_FLOAT });
			typeCache.insert({ "double",TYPE_DOUBLE });
			typeCache.insert({ "boolean",TYPE_BOOLEAN });
			typeCache.insert({ "char",TYPE_CHAR });

		}

		JVMForeignRuntime::~JVMForeignRuntime() {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "DEBUG - JVMForeignRuntime - dtor" << std::endl;
			}
			stop();
		}

		void JVMForeignRuntime::waitForInitialization() {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Waiting for JVM startup ..." << std::endl;
			}
			controlData.stateMachine.waitForStates({JVMState::Started,JVMState::ErrorShutdown});
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVM startup completed" << std::endl;
			}
		}

		bool JVMForeignRuntime::start(const std::initializer_list<std::string>& resources) {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Starting JVM ..." << std::endl;
			}
			jvmThread = std::thread([&resources](ControlData* controlData_)
			{
				if (LOG_JVMTHREAD) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Start of JVMThread" << std::endl;
				}
				JVMThread jvmThread(controlData_);
				bool jvmCreated = jvmThread.createJVM(resources);
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

		void JVMForeignRuntime::stop() {
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - Beginning of JVMThread shutdown" << std::endl;
			}
			if(isRunning()){
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMForeignRuntime - JVMThread is running" << std::endl;
				}
				controlData.workOperation = JVMWorkOperation::Shutdown;
				notifyWorkPrepared();
				//waitForWorkCompleted(); - On Windows, stoping in destructor happens when worker thread is already destroyed/vanished
				if(LOG_JVMFOREIGNRUNTIME) {
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
			auto search = typeCache.find(name);
			if(search != typeCache.end()) {
				return search->second;
			} else if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << "Type " << name << " not found in cache" << std::endl;
			}

			std::shared_ptr<JVMOpParams_TypeLookup> params = std::make_shared<JVMOpParams_TypeLookup>();

			params->typeName = name;
			auto& localId = id;
			JVMOp op = [localId](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams) {
				if(LOG_JVMFOREIGNRUNTIME) {
					std::cout << "Started type lookup ..." << std::endl;
				}
				auto typeLookupParams = std::static_pointer_cast<JVMOpParams_TypeLookup>(opParams);
				std::string javaTypeName = typeLookupParams->typeName;
				std::replace(javaTypeName.begin(), javaTypeName.end(),'.','/');
				jclass type = env->FindClass(javaTypeName.c_str());
				if(type != nullptr) {
					javaTypeName = "L" + javaTypeName + ";";
					if(LOG_JVMFOREIGNRUNTIME) {
						std::cout << ">>>> DEBUG: Type (" << typeLookupParams->typeName << ") found" << std::endl;
					}
					jmethodID method_GetClass = env->GetMethodID(type, "getClass", "()Ljava/lang/Class;");
					assert(method_GetClass);
					auto type_Class = env->CallObjectMethod(type, method_GetClass);
					jclass clazzClass = env->GetObjectClass(type_Class);
					assert(clazzClass);
					jmethodID method_getTypeParameters = env->GetMethodID(clazzClass, "getTypeParameters", "()[Ljava/lang/reflect/TypeVariable;");
					assert(method_getTypeParameters);
					jobjectArray typeParameters = static_cast<jobjectArray>(env->CallObjectMethod(type_Class, method_getTypeParameters));
					assert(typeParameters);
					jsize typeParametersCount = env->GetArrayLength(typeParameters);
					if (typeParametersCount == 0) {
						typeLookupParams->type = std::make_shared<JVMType>(localId, type, typeLookupParams->typeName,javaTypeName);
						return;
					}
					jclass classClass = env->FindClass("java/lang/Class");
					assert(classClass);
					jmethodID class_getName = env->GetMethodID(classClass, "getName", "()Ljava/lang/String;");
					assert(class_getName);
					std::map<std::string, std::vector<std::string>> typeBoundsMap;
					for (jsize i = 0; i < typeParametersCount; i++) {
						wrappers::TypeVariable typeParameter(env, env->GetObjectArrayElement(typeParameters, i));
						std::string name = typeParameter.getName();
						jobjectArray bounds = typeParameter.getBounds();
						jsize boundsCount = env->GetArrayLength(bounds);
						std::vector<std::string> typeBounds;
						for (jsize i = 0; i < boundsCount; i++) {
							wrappers::Type type(env, env->GetObjectArrayElement(bounds, i));
							typeBounds.push_back(std::move(type.getTypeName()));
						}
						typeBoundsMap.emplace(name, typeBounds);
					}
					auto jvmGenericType = std::make_shared<JVMGenericType>(localId, type, typeLookupParams->typeName, javaTypeName);
					jvmGenericType->setTypeParameterInfo(typeBoundsMap);
					typeLookupParams->type = jvmGenericType;

				} else {
					if(LOG_JVMFOREIGNRUNTIME) {
						std::cout << ">>>> DEBUG: Type (" << typeLookupParams->typeName << ") not found" << std::endl;
					}
					typeLookupParams->type = nullptr;
				}
			};
			std::cout << "Clinet - Op: " << op.target<void(JNIEnv*, std::shared_ptr<JVMOpParams>)>() << std::endl;
			execute(op,params);
			if(params->type != nullptr) {
				typeCache.insert({name,params->type});
			}
			return params->type;
		}


		// Currently, type can be "accidently" retrofited with generic information. 
		// Should for example "ArrayList" and "ArrayList<Integer>" should be represented with same JVMType instace ?
		// If yes, then must be decided how to distinct non-generic and generics calls and provide checks
		
		// If class lookup without explicit type parameters, calls to generic methods/fields checked against type bounds
		// If class lookup with explicit type parameters, calls to generic methods/fields checked against provided type parameters
		// Behind, Generic and Non-generic versions of class will have handle to barebone class.

		// Generic handling mode: JLS or Flexible
		
		// Mode_JLS - As defined in Java Language Specification, for example: 
		// void work(List<Object> items); 
		// List<String> messages = new ArrayList<String>();
		// //COMPILE_ERROR work(messages)
		
		// Mode_Flexible - Takes type parameter hierarchy into consideration, for example: 
		// String message = "Test Message";
		// Object obj = message; "String" extends "Object"
		//		>>>	Then >>>
		// void work(List<Object> items); 
		// List<String> messages = new ArrayList<String>();
		// work(messages) // Valid in this mode


		std::shared_ptr<FR_Type> JVMForeignRuntime::lookupGenericType(const std::string& name, std::initializer_list<std::shared_ptr<FR_Type>> typeParameters) {
			//if(typeParameters.size() == 0) {
			//	return nullptr;
			//}
			//auto type = lookupType(name);
			//auto jvmType = std::static_pointer_cast<JVMType>(type);
			//auto typeParametersLookup = std::make_shared<JVMOpParams_TypeParametersLookup>();
			//typeParametersLookup->target = jvmType;
			//JVMOp op = [](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams) {
			//	if(LOG_JVMFOREIGNRUNTIME) {
			//		std::cout << "Started type parameter lookup ..." << std::endl;
			//	}
			//	auto typeParametersLookup = std::static_pointer_cast<JVMOpParams_TypeParametersLookup>(opParams);
			//	auto jvmType = typeParametersLookup->target;
			//	jclass clazz = jvmType->getType();
			//	jmethodID method_GetClass = env->GetMethodID(clazz,"getClass","()Ljava/lang/Class;");
			//	assert(method_GetClass);
			//	jclass clazzClass = env->GetObjectClass(clazz);
			//	jmethodID method_getTypeParameters = env->GetMethodID(clazzClass,"getTypeParameters","()[Ljava/lang/reflect/TypeVariable;");
			//	assert(method_getTypeParameters);
			//	jobjectArray typeParameters = static_cast<jobjectArray>(env->CallObjectMethod(clazz,method_getTypeParameters));
			//	assert(typeParameters);
			//	jsize typeParametersCount = env->GetArrayLength(typeParameters);
			//	if (typeParametersCount == 0) {
			//		typeParametersLookup->target = nullptr;
			//		return;
			//	}
			//	std::cout << "Type parameters: " << typeParametersCount << std::endl;
			//	jclass classTypeParameter = env->FindClass("java/lang/reflect/TypeVariable");
			//	assert(classTypeParameter);
			//	jmethodID method_getName = env->GetMethodID(classTypeParameter,"getName","()Ljava/lang/String;");
			//	assert(method_getName);
			//	jmethodID method_getBounds = env->GetMethodID(classTypeParameter,"getBounds","()[Ljava/lang/reflect/Type;");
			//	assert(method_getBounds);

			//	jclass classClass = env->FindClass("java/lang/Class");
			//	assert(classClass);
			//	jmethodID class_getName = env->GetMethodID(classClass,"getName","()Ljava/lang/String;");
			//	assert(class_getName);
			//	std::map<jstring, std::vector<jobject>> typeBoundsMap;
			//	for(jsize i = 0; i < typeParametersCount; i++) {
			//		jobject typeParameter = env->GetObjectArrayElement(typeParameters,i);
			//		jstring name = static_cast<jstring>(env->CallObjectMethod(typeParameter,method_getName));
			//		jboolean isNameCopy;
			//		const char* nameNative = env->GetStringUTFChars(name,&isNameCopy);
			//		if(isNameCopy == JNI_TRUE) {
			//			env->ReleaseStringUTFChars(name,nameNative);
			//		}
			//		jobjectArray bounds = static_cast<jobjectArray>(env->CallObjectMethod(typeParameter,method_getBounds));
			//		assert(bounds);
			//		jsize boundsCount = env->GetArrayLength(bounds);
			//		std::vector<jobject> typeBounds;
			//		for(jsize i = 0; i < boundsCount; i++) {
			//			jobject bound = env->GetObjectArrayElement(bounds,i);
			//			typeBounds.push_back(bound);
			//		}
			//		typeBoundsMap.emplace(name, typeBounds);
			//	}
			//	auto jvmGenericType = std::make_shared<JVMGenericType>(jvmType->getRuntimeId(), jvmType->getType(), jvmType->getName(), jvmType->getSignatureName());
			//	jvmGenericType->setTypeParameterInfo(typeBoundsMap);
			//	typeParametersLookup->target = jvmGenericType;
			//};
			//execute(op,typeParametersLookup);
			//return typeParametersLookup->target;
			//// 1. Type parameters - count and namespace
			//// 2. If count do not match with type count passed to method, return error
			//// 2. If count do match with type count passed to method, assign types to parameter type names in order as passed to method;

			//// 	1. field lookup:	if field has generic type (aka.: specified with parameter type name)
			//// 	2. method lookup
			return nullptr;
		}

		std::string JVMForeignRuntime::prepareMethodSignature(const std::vector<std::shared_ptr<FR_Type>> &parametersTypes, const std::shared_ptr<FR_Type> returnType){
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

		std::shared_ptr<FR_Method> JVMForeignRuntime::lookupConstructor(std::shared_ptr<FR_Type> targetType, const std::vector<std::shared_ptr<FR_Type>>& parametersTypes) {
			check(targetType);
			for(auto parameterType : parametersTypes) {
				check(parameterType);
			}
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
			return std::make_shared<JVMMethod>(getId(),params->method,targetType,nullptr);
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::createObject(std::shared_ptr<FR_Type> type, std::shared_ptr<FR_Method> constructor, const std::vector<std::shared_ptr<FR_Object>>& parameters) {
			check(type);
			check(constructor);
			for(auto parameter : parameters) {
				check(parameter);
			}
			if(constructor->getType() != type) {

			}
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

		std::shared_ptr<FR_Field> JVMForeignRuntime::lookupField(std::shared_ptr<FR_Type> targetType, const std::string& name, const std::shared_ptr<FR_Type> fieldType) {
			auto params = std::make_shared<JVMOpParams_FieldLookup>();
			params->targetType = targetType;
			params->name = name;
			params->type = fieldType;
			auto op = [this](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams) {
				auto fieldLookup = std::static_pointer_cast<JVMOpParams_FieldLookup>(opParams);
				auto jvmTargetType = std::static_pointer_cast<JVMType>(fieldLookup->targetType);
				auto jvmFieldType = std::static_pointer_cast<JVMType>(fieldLookup->type);
				jfieldID field = env->GetStaticFieldID(jvmTargetType->getType(), fieldLookup->name.c_str(), jvmFieldType->getSignatureName().c_str());
				if(field == nullptr) {
					fieldLookup->field = nullptr;
				} else {
					fieldLookup->field = std::make_shared<JVMField>(jvmTargetType->getRuntimeId(),
																			field,
																			fieldLookup->targetType,
																			fieldLookup->type);
				}
			};
			execute(op,params);
			return params->field;
		}

		std::shared_ptr<FR_Field> JVMForeignRuntime::lookupField(std::shared_ptr<FR_Object> targetObject, const std::string& name, const std::shared_ptr<FR_Type> fieldType) {
			return nullptr;
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::getField(std::shared_ptr<FR_Type> targetType, std::shared_ptr<FR_Field> field) {
			auto params = std::make_shared<JVMOpParams_StaticFieldAccess>();
			params->targetType = targetType;
			params->field = field;
			auto op = [this](JNIEnv* env,std::shared_ptr<JVMOpParams> opParams) {
				auto staticFieldAccess = std::static_pointer_cast<JVMOpParams_StaticFieldAccess>(opParams);
				auto jvmTargetType = std::static_pointer_cast<JVMType>(staticFieldAccess->targetType);
				auto jvmField = std::static_pointer_cast<JVMField>(staticFieldAccess->field);
				auto jvmFieldType = std::static_pointer_cast<JVMType>(jvmField->getType());
				auto accessor = jvmFieldType->getStaticFieldAccessor();
				staticFieldAccess->result = accessor(env,jvmTargetType,jvmField);
			};
			execute(op,params);
			return params->result;
		}

		std::shared_ptr<FR_Object> JVMForeignRuntime::getField(std::shared_ptr<FR_Object> targetObject, std::shared_ptr<FR_Field> field) {
			return nullptr;
		}

		std::shared_ptr<FR_Method> JVMForeignRuntime::lookupMethod(std::shared_ptr<FR_Object> targetObject, const std::string &name, const std::vector<std::shared_ptr<FR_Type>> &parametersTypes, const std::shared_ptr<FR_Type> returnType) {
				check(targetObject);
				for(auto parameterType : parametersTypes) {
					check(parameterType);
				}
				check(returnType);
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
				return std::make_shared<JVMMethod>(getId(),params->method,targetObject->getType(), returnType);
			}

		std::shared_ptr<FR_Method> JVMForeignRuntime::lookupMethod(std::shared_ptr<FR_Type> targetType, const std::string &name, const std::vector<std::shared_ptr<FR_Type>> &parametersTypes, const std::shared_ptr<FR_Type> returnType) {
					check(targetType);
					for(auto parameterType : parametersTypes) {
						check(parameterType);
					}
					check(returnType);
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
					return std::make_shared<JVMMethod>(getId(),params->method,targetType,returnType);
				}

		std::shared_ptr<nigiri::FR_Object> JVMForeignRuntime::call(std::shared_ptr<FR_Object> targetObject, std::shared_ptr<FR_Method> method, const std::vector<std::shared_ptr<FR_Object>> &parameters) {
			check(targetObject);
			check(method);
			for(auto parameter : parameters) {
				check(parameter);
			}
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
				auto returnType = std::static_pointer_cast<JVMType>(jvmMethod->getReturnType());
				if(returnType == nullptr) {
					env->CallVoidMethodA(jvmObject->getObject(),jvmMethod->getMethod(),callParams);
				} else {
					auto caller = returnType->getInstanceMethodCaller();
					instanceMethodCall_params->result = caller(env,jvmObject,jvmMethod,callParams);
				}
				delete[] callParams;
			};
			execute(op,params);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Object method returned" << std::endl;
			}
			return params->result;
		}

		std::shared_ptr<nigiri::FR_Object> JVMForeignRuntime::call(std::shared_ptr<FR_Type> targetType, std::shared_ptr<FR_Method> method, const std::vector<std::shared_ptr<FR_Object>> &parameters) {
			check(targetType);
			check(method);
			for(auto parameter : parameters) {
				check(parameter);
			}
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
				auto returnType = std::static_pointer_cast<JVMType>(jvmMethod->getReturnType());
				if(returnType == nullptr) {
					env->CallStaticVoidMethodA(jvmType->getType(),jvmMethod->getMethod(),callParams);
				} else {
					auto caller = returnType->getStaticMethodCaller();
					staticMethodCall_params->result = caller(env,jvmType,jvmMethod,callParams);
				}
				delete[] callParams;
			};
			execute(op,params);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: Type method returned" << std::endl;
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
				jboolean isCopy;
				const char*  data = env->GetStringUTFChars(jvmStr,&isCopy);
				opParams_StringExtraction->result = std::string(data);
				if (isCopy == JNI_TRUE) {
					env->ReleaseStringUTFChars(jvmStr,data);
				}
			};
			execute(op,params);
			if(LOG_JVMFOREIGNRUNTIME) {
				std::cout << ">>>> DEBUG: String extracted" << std::endl;
			}
			return params->result;
		}

		std::string JVMForeignRuntime::toString(std::shared_ptr<FR_Object> obj) {
			check(obj);
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

		void JVMForeignRuntime::check(const std::shared_ptr<FR_Type>& type) {
			if(type == nullptr) {
				throw std::invalid_argument("Type is nullptr");
			} else if(type->getRuntimeId() != getId()) {
				throw std::invalid_argument("Provided type do not belongs to this foreign runtime");
			}
		}

		void JVMForeignRuntime::check(const std::shared_ptr<FR_Method>& method) {
			if(method == nullptr) {
				throw std::invalid_argument("Method is nullptr");
			} else if(method->getRuntimeId() != getId()) {
				throw std::invalid_argument("Provided method do not belongs to this foreign runtime");
			}
		}

		void JVMForeignRuntime::check(const std::shared_ptr<FR_Object>& object) {
			if(object == nullptr) {
				throw std::invalid_argument("Object is nullptr");
			} else if(object->getRuntimeId() != getId()) {
				throw std::invalid_argument("Provided object do not belongs to this foreign runtime");
			}
		}

		std::string getThreadIdString(std::thread::id tid){
			static std::hash<std::thread::id> hasher;
			auto hash = hasher(tid);
			return std::to_string(hash);
		}

	}
}
