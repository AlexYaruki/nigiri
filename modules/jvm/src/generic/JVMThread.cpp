#include <cstring>
#include <cassert>
#include <nigiri.h>
#include "JVMThread.h"

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#elif defined(WIN32)
#include <Windows.h>
#else
#error This platform is not supported
#endif

namespace nigiri {
    namespace internal {

        JVMThread::JVMThread(JVMForeignRuntime::ControlData* _controlData) {
            jvm = nullptr;
            env = nullptr;
            controlData = _controlData;
        }

        JVMThread::~JVMThread() {
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] " << "DEBUG - JVMThread - dtor" << std::endl;
            }
        }

        std::string JVMThread::getCurrentDirectory() {
            static char cwdBuffer[1024];
#if defined(__linux__) || defined(__APPLE__)
            getcwd(cwdBuffer, 1024);
#else
            GetCurrentDirectoryA(1024, cwdBuffer);
#endif
            std::string cwd(cwdBuffer);
            return cwd;
        }

        bool JVMThread::createJVM(const std::initializer_list<std::string>& resources) {
            JavaVMInitArgs vm_args;
            jint res;
            memset(&vm_args, 0, sizeof (JavaVMInitArgs));
            vm_args.version = JNI_VERSION_1_8;
            vm_args.ignoreUnrecognized = JNI_TRUE;
            if (resources.size() != 0) {
                std::string op = "-Djava.class.path=";
                for (size_t i = 0; i < resources.size(); i++) {
                    op.append(resources.begin()[i]);
                    if (i != resources.size() - 1) {
                        op.append(":");
                    }
                }
                std::cout << "Option: " << op << std::endl;
                JavaVMOption options[1];
                options[0].optionString = const_cast<char*> (op.c_str());
                vm_args.options = options;
                vm_args.nOptions = 1;
                res = JNI_CreateJavaVM(&jvm, reinterpret_cast<void**> (&env), &vm_args);

            } else {
                vm_args.nOptions = 0;
                res = JNI_CreateJavaVM(&jvm, reinterpret_cast<void**> (&env), &vm_args);
            }
            if (res < 0) {
                return false;
            } else {
                return true;
            }
        }

        void JVMThread::notifyInit() {
            controlData->stateMachine.submitEvent(JVMEvent::Init_Success);
        }

        void JVMThread::notifyRollback() {
            controlData->stateMachine.submitEvent(JVMEvent::Init_Error);
        }

        void JVMThread::quit() {
            jvm->DestroyJavaVM();
            controlData->stateMachine.submitEvent(JVMEvent::JVM_Destroyed);
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] " << "JVM Destroyed" << std::endl;
            }
        }

        void JVMThread::waitForWork() {
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] " << "JVMThread - Waiting for work ..." << std::endl;
            }
            controlData->stateMachine.waitForState(JVMState::WorkPrepared);
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] " << "JVMThread - Work received" << std::endl;
            }
        }

        void JVMThread::executeWork() {
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] " << "JVMThread - Work execution" << std::endl;
            }
            switch (controlData->workOperation) {
                case JVMWorkOperation::ExecuteOp:
                {
                    controlData->jvmOp(env, controlData->jvmOpParams);
                    break;
                }
                case JVMWorkOperation::Shutdown:
                {
                    if (LOG_JVMTHREAD) {
                        std::cout << "JVMThread - Shutdown requested" << std::endl;
                    }
                    shutdownRequested = true;
                    break;
                }
            }
        }

        void JVMThread::notifyWorkFinished() {
            controlData->stateMachine.submitEvent(JVMEvent::Work_Completed);
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Work finished" << std::endl;
            }
        }

        void JVMThread::workLoop() {
            notifyInit();
            while (!shutdownRequested) {
                waitForWork();
                executeWork();
                notifyWorkFinished();
            }
            if (LOG_JVMTHREAD) {
                std::cout << "[" << getThreadIdString(std::this_thread::get_id()) << "] JVMThread - Work loop stopped" << std::endl;
            }
        }
    }
}
