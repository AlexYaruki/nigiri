#ifndef JVMTHREAD_H
#define JVMTHREAD_H

#include <jni.h>
#include "JVMForeignRuntime.h"

namespace nigiri
{
	namespace internal
	{
		class JVMThread
		{

		public:
			JVMThread(JVMForeignRuntime::ControlData* controlData);
			~JVMThread();
			bool createJVM();
			void notifyInit();
			void notifyRollback();
			void quit();

            /*
             * Possibly change to state machine
            */
            void workLoop();
			void waitForWork();
			void executeWork();
            void notifyWorkFinished();
			void prepareIdle();

			bool cacheMetadata();
            bool isRunning();
		private:
			JVMThread(const JVMThread&) = delete;
			JVMThread operator=(const JVMThread&) = delete;
			std::string getCurrentDirectory();
			JVMForeignRuntime::ControlData* controlData;
			JavaVM* jvm;
			JNIEnv* env;
			bool shutdownRequested = false;
		};
	}
}

#endif //JVMTHREAD_H
