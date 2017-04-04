#include <nigiri.h>
#include <cassert>

int main() {
	{
		bool jvmAvailable = nigiri::ForeignRuntimeManager::isCategoryAvailable("jvm");
		assert(jvmAvailable);
		auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
		bool jvmStarted = jvm->start({});
		assert(jvmStarted);

		auto jvmRoundingMode = jvm->lookupType("java.math.RoundingMode");
		assert(jvmRoundingMode);
		auto jvmFieldDescriptor = jvm->lookupField(jvmRoundingMode, "CEILING", jvmRoundingMode);
		assert(jvmFieldDescriptor);
		auto jvmField = jvm->getField(jvmRoundingMode, jvmFieldDescriptor);
		assert(jvmField);
		auto jvmString = jvm->lookupType("java.lang.String");
		assert(jvmString);
		auto jvmRoundingMode_toString = jvm->lookupMethod(jvmField, "toString", {}, jvmString);
		assert(jvmRoundingMode_toString);
		auto jvmModeString = jvm->call(jvmField, jvmRoundingMode_toString, {});
		std::string str = jvm->toString(jvmModeString);
		assert(str.compare("CEILING") == 0);
	}
}
