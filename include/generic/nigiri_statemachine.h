#ifndef NIGIRI_STATEMACHINE_H
#define NIGIRI_STATEMACHINE_H

#include <condition_variable>
#include <initializer_list>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <type_traits>

namespace nigiri {

// #define LOG_NIGIRI_STATEMACHINE

template<typename TState,
        typename TEvent,
        TState initialState,
        bool stateEnum,
        bool stateConvert,
        bool eventEnum,
        bool eventConvert>
class _StateMachine {

public:

    using StateToStringFunc = std::function<std::string(TState)>;
    using EventToStringFunc = std::function<std::string(TEvent)>;

	using StateValue = typename std::underlying_type<TState>::type;
	using EventValue = typename std::underlying_type<TState>::type;

    _StateMachine(){
        static_assert(stateEnum && !stateConvert,"State type is not enum class");
        static_assert(eventEnum && !eventConvert,"Event type is not enum class");
    };

    ~_StateMachine(){
        #ifdef LOG_NIGIRI_STATEMACHINE
        std::cout << "DEBUG - nigiri::StateMachine - dtor" << std::endl;
        #endif
    }

    TState getState(){
        return state;
    }

    void submitEvent(TEvent event){
        if(isEventValid(event)){
            #ifdef LOG_NIGIRI_STATEMACHINE
            if(eventToStringFunc){
                std::string msg = "StateMachine - Submitting event \"" + eventToStringFunc(event) + "\"";
                std::cout << msg << std::endl;
            }
            TState oldState = state;
            #endif
            std::lock_guard<std::mutex> guard(mtx);
            state = topology[state][event];
            #ifdef LOG_NIGIRI_STATEMACHINE
            if (eventToStringFunc && stateToStringFunc) {
				std::string msg = "StateMachine - ";
				msg += "Event \"" + eventToStringFunc(event) + "\" changed state from \"" + stateToStringFunc(oldState) + "\" to \"" + stateToStringFunc(state) + "\"";
				std::cout << msg << std::endl;
			}
            #endif
            cv.notify_one();
            #ifdef LOG_NIGIRI_STATEMACHINE
            if(eventToStringFunc){
                std::string msg = "StateMachine - Event \"" + eventToStringFunc(event) + "\" submitted";
                std::cout << msg << std::endl;
            }
            #endif
        }
    };

    void waitForState(TState waitState){
        #ifdef LOG_NIGIRI_STATEMACHINE
        if(stateToStringFunc){
            std::string msg = "StateMachine - Waiting for state \"" + stateToStringFunc(waitState) + "\"";
			std::cout << msg << std::endl;
        }
        #endif
        std::unique_lock<std::mutex> lock(mtx);
        TState* statePtr = &state;
        cv.wait(lock,[statePtr,waitState](){
			#ifdef LOG_NIGIRI_STATEMACHINE
            StateValue requiredStateValue = static_cast<StateValue>(waitState);
			StateValue currentStateValue = static_cast<StateValue>(*statePtr);
            std::string msg = "StateMachine - State check - ";
			msg += "Required: \"" + std::to_string(requiredStateValue) + "\", Current: \"" + std::to_string(currentStateValue) + "\"";
			std::cout << msg << std::endl;
            #endif
			return waitState == *statePtr;
        });
        #ifdef LOG_NIGIRI_STATEMACHINE
        if(stateToStringFunc){
            std::string msg = "StateMachine - State \"" + stateToStringFunc(waitState) + "\" reached";
            std::cout << msg << std::endl;
        }
        #endif
    };

    void waitForStates(std::initializer_list<TState> waitStates){
        #ifdef LOG_NIGIRI_STATEMACHINE
        if(stateToStringFunc){
            std::string msg =" StateMachine - Waiting for ";
            for(auto waitState : waitStates){
                msg += getStateString(waitState) + " ";
            }
            std::cout << msg << std::endl;
        }
        #endif
        std::unique_lock<std::mutex> lock(mtx);
		bool condition = true;
		while (condition) {
			cv.wait(lock);
			for (auto waitState : waitStates) {
				if (waitState == state) {
					#ifdef LOG_NIGIRI_STATEMACHINE
                    StateValue requiredStateValue = static_cast<StateValue>(waitState);
					StateValue currentStateValue = static_cast<StateValue>(state);
                    std::string msg = "StateMachine - State check - ";
					msg += "Required: \"" + std::to_string(requiredStateValue) + "\", Current: \"" + std::to_string(currentStateValue) + "\"";
					std::cout << msg << std::endl;
                    #endif
					condition = false;
				}
			}
		}
        #ifdef LOG_NIGIRI_STATEMACHINE
        if(stateToStringFunc){
            std::string stateString = "\"" + stateToStringFunc(state) + "\"";
            std::cout << "StateMachine - State " << stateString << " reached" << std::endl;
        }
        #endif
    };

    void addConnection(TState fromState, TEvent cause, TState toState){
        topology[fromState][cause] = toState;
    };

    void registerStateToString(StateToStringFunc func) {
        if(func) {
            stateToStringFunc = func;
        }
    }

    void registerEventToString(EventToStringFunc func) {
        if(func) {
            eventToStringFunc = func;
        }
    }

private:
    TState state = initialState;
    std::map<TState,std::map<TEvent,TState>> topology;
    std::mutex mtx;
    std::condition_variable cv;
    StateToStringFunc stateToStringFunc;
    EventToStringFunc eventToStringFunc;

    bool isEventValid(TEvent event) {
        std::string stateString = "";
        if(stateToStringFunc != nullptr){
            stateString = "\"" + stateToStringFunc(state) + "\"";
        }
        auto fromSearch = topology.find(state);
        if(fromSearch == topology.end()){
            #ifdef LOG_NIGIRI_STATEMACHINE
            std::cout << "Current state" << stateString << " do not have any connections" << std::endl;
            #endif
            return false;
        }
        auto stateTopology = fromSearch->second;
        auto eventSearch = stateTopology.find(event);
        if(eventSearch == stateTopology.end()) {
            #ifdef LOG_NIGIRI_STATEMACHINE
            if(eventToStringFunc != nullptr){
                std::string eventString = "\"" + eventToStringFunc(event) + "\"";
                std::cout << "Current state" << stateString << " do not respond to provided event" << eventString << std::endl;
            }
            #endif
            return false;
        }
        return true;
    }
};

template<typename TState, typename TEvent, TState initialState>
using StateMachine =
      _StateMachine<TState,
                    TEvent,
                    initialState,
                    std::is_enum<TState>::value,
                    std::is_convertible<TState,int>::value,
                    std::is_enum<TEvent>::value,
                    std::is_convertible<TEvent,int>::value>;

}
#endif //NIGIRI_STATEMACHINE_H
