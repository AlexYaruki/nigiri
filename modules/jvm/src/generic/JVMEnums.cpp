#include <string>
#include <map>
#include "JVMEnums.h"

namespace nigiri {
    namespace internal {
		
        std::string getStateString(JVMState jvmState){
			switch(jvmState)
			{
				case JVMState::Created:{
					return "JVMState::Created";
				}
				case JVMState::Started: {
					return "JVMState::Started";
				}
				case JVMState::ErrorShutdown: {
					return "JVMState::ErrorShutdown";
				}
				case JVMState::Shutdown: {
					return "JVMState::Shutdown";
				}
				case JVMState::WorkPrepared: {
					return "JVMState::WorkPrepared";
				}
				case JVMState::WorkCompleted: {
					return "JVMState::WorkCompleted";
				}
				default: {
					return "Out-of-range";
				}
			}
        }

        std::string getEventString(JVMEvent jvmEvent){
			switch(jvmEvent)
			{
			   case JVMEvent::Init_Success: {
				   return "JVMEvent::Init_Success";
			   }
			   case JVMEvent::Init_Error: {
				   return "JVMEvent::Init_Error";
			   }
			   case JVMEvent::Work_Prepared: {
				   return "JVMEvent::Work_Prepared";
			   }
			   case JVMEvent::Work_Completed: {
				   return "JVMEvent::Work_Completed";
			   }
			   case JVMEvent::Work_Idle: {
				   return "JVMEvent::Work_Idle";
			   }
			   case JVMEvent::JVM_Destroyed: {
				   return "JVMEvent::JVM_Destroyed";
			   }
			   default: {
				   return "Out-of-range";
			   }
			}
        }
    }
}
