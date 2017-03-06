#include <cstring>
#include <cassert>
#include <nigiri.h>
#include "JVMThread.h"

#if defined(__linux__) || defined(__APPLE__)
#	include <unistd.h>
#elif defined(WIN32)
#	include <Windows.h>
#else
#	error This platform is not supported
#endif

/*

JVM Classpath:
	- nigiri-java.jar -> Java-side of benchmark loading
		- Present in $JAVA_HOME/lib/ext
	- <filename>.jar -> File containing benchmarks implemented in Java

*/


namespace nigiri
{
	namespace internal
	{
		JVMThread::JVMThread(JVMForeignRuntime::ControlData* _controlData)
		{
			jvm = nullptr;
			env = nullptr;
			controlData = _controlData;
        }

        JVMThread::~JVMThread() {
            std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] " << "DEBUG - JVMThread - dtor" << std::endl;
        }

		std::string JVMThread::getCurrentDirectory()
		{
			static char cwdBuffer[1024];
		#if defined(__linux__) || defined(__APPLE__)
			getcwd(cwdBuffer, 1024);
		#else
			GetCurrentDirectoryA(1024, cwdBuffer);
		#endif
			std::string cwd(cwdBuffer);
			return cwd;
		}

		bool JVMThread::createJVM()
		{
			JavaVMInitArgs vm_args;
			memset(&vm_args, 0, sizeof(JavaVMInitArgs));
			vm_args.version = JNI_VERSION_1_8;
			vm_args.nOptions = 0;
			vm_args.ignoreUnrecognized = JNI_TRUE;
			jint res = JNI_CreateJavaVM(&jvm, reinterpret_cast<void**>(&env), &vm_args);
			if(res < 0)
			{
				return false;
			} else
			{
				return true;
			}
		}

		void JVMThread::notifyInit()
		{
            controlData->stateMachine.submitEvent(JVMEvent::Init_Success);
		}

		void JVMThread::notifyRollback()
		{
            controlData->stateMachine.submitEvent(JVMEvent::Init_Error);
		}

		void JVMThread::quit()
		{
			jvm->DestroyJavaVM();
            controlData->stateMachine.submitEvent(JVMEvent::JVM_Destroyed);
			std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] " << "JVM Destroyed" << std::endl;
		}

        void JVMThread::waitForWork() {
			std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] " << "JVMThread - Waiting for work ..." << std::endl;
            controlData->stateMachine.waitForState(JVMState::WorkPrepared);
			std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] " << "JVMThread - Work received" << std::endl;
        }

		void JVMThread::executeWork() {
            std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] " << "JVMThread - Work execution" << std::endl;
            switch(controlData->workOperation){
                case JVMWorkOperation::ExecuteOp: {
                    controlData->jvmOp(env,controlData->jvmOpParams);
					break;
                }
                case JVMWorkOperation::Shutdown:
                {
                    std::cout << "JVMThread - Shutdown requested" << std::endl;
                    shutdownRequested = true;
                    break;
                }

            }
		}

        void JVMThread::notifyWorkFinished() {
			controlData->stateMachine.submitEvent(JVMEvent::Work_Completed);
			std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Work finished" << std::endl;
        }

		void JVMThread::workLoop()
		{
            notifyInit();
			while (!shutdownRequested) {
				waitForWork();
                executeWork();
                notifyWorkFinished();
			}
            std::cout << "[" << nigiri::getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Work loop stopped" << std::endl;
        }

	}
}
