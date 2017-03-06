#ifndef JVM_ENUMS_H
#define JVM_ENUMS_H

namespace nigiri {
    namespace internal {

        enum class JVMState {
            Created = 0,
            ErrorShutdown = -1,
            Started = 1,
            WorkPrepared = 2,
            WorkCompleted = 3,
            Shutdown = 4
        };



        enum class JVMEvent {
            Init_Error,
            Init_Success,
            Work_Prepared,
            Work_Completed,
			Work_Idle,
            JVM_Destroyed,
        };

        enum class JVMWorkOperation {
            ExecuteOp,
            Shutdown
        };

        std::string getStateString(JVMState state);
        std::string getEventString(JVMEvent event);
    }

}

#endif