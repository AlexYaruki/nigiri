#ifndef JVM_INTERNAL_WRAPPERS_H
#define JVM_INTERNAL_WRAPPERS_H

#include "JVMForeignRuntime.h"

namespace nigiri {
	namespace internal {
		namespace wrappers {

			class Type {
			public:
				Type(JNIEnv* env, jobject obj);
				std::string getTypeName();
			private:
				JNIEnv* env;
				jobject obj;
				jclass clazz;
			};

			class TypeVariable {
			public:
				TypeVariable(JNIEnv* env, jobject obj);
				std::string getName();
				jobjectArray getBounds();
			private:
				JNIEnv* env;
				jobject obj;
				jclass clazz;
			};

		}
	}
}

#endif
