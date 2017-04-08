#include "JVMInternalWrappers.h"
#include <cassert>

namespace nigiri {
	namespace internal {
		namespace wrappers {
			
			Type::Type(JNIEnv * env, jobject obj) : env(env), obj(obj)
			{
				clazz = env->GetObjectClass(obj);
				assert(clazz);
			}
			std::string Type::getTypeName()
			{
				static jmethodID method_getTypeName = env->GetMethodID(clazz, "getTypeName", "()Ljava/lang/String;");
				assert(method_getTypeName);
				jstring java_TypeName = static_cast<jstring>(env->CallObjectMethod(obj, method_getTypeName));
				assert(java_TypeName);
				jboolean isNameCopy;
				const char* typeNameRaw = env->GetStringUTFChars(java_TypeName, &isNameCopy);
				std::string typeName(typeNameRaw);
				if (isNameCopy == JNI_TRUE) {
					env->ReleaseStringUTFChars(java_TypeName, typeNameRaw);
				}
				return std::move(typeName);
			}

			TypeVariable::TypeVariable(JNIEnv * env, jobject obj) : env(env), obj(obj)
			{
				clazz = env->GetObjectClass(obj);
				assert(clazz);
			}

			std::string TypeVariable::getName()
			{
				static jmethodID method_getName = env->GetMethodID(clazz, "getName", "()Ljava/lang/String;");
				assert(method_getName);
				jstring java_name = static_cast<jstring>(env->CallObjectMethod(obj, method_getName));
				assert(java_name);
				jboolean isNameCopy;
				const char* nameRaw = env->GetStringUTFChars(java_name, &isNameCopy);
				std::string name(nameRaw);
				if (isNameCopy == JNI_TRUE) {
					env->ReleaseStringUTFChars(java_name, nameRaw);
				}
				return std::move(name);
			}

			jobjectArray TypeVariable::getBounds()
			{
				static jmethodID method_getBounds = env->GetMethodID(clazz, "getBounds", "()[Ljava/lang/reflect/Type;");
				assert(method_getBounds);
				jobjectArray bounds = static_cast<jobjectArray>(env->CallObjectMethod(obj, method_getBounds));
				assert(bounds);
				return bounds;
			}

		}
	}
}