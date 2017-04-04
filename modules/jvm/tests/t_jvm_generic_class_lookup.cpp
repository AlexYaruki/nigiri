#include <cassert>
#include <iostream>
#include <nigiri.h>

int main(){
    auto jvm = nigiri::ForeignRuntimeManager::createRuntime("jvm");
    assert(jvm->start({}));

    auto jvmNumber = jvm->lookupType("java.lang.Number");
    assert(jvmNumber);
    auto jvmArrayList = jvm->lookupGenericType("java.util.ArrayList", {jvmNumber});
    assert(jvmArrayList);
	auto jvmArrayList_constructor = jvm->lookupConstructor(jvmArrayList, {});
	assert(jvmArrayList_constructor);
	auto container = jvm->createObject(jvmArrayList, jvmArrayList_constructor, {});
	assert(container);

	auto jvmObject = jvm->lookupType("java.lang.Object");
	assert(jvmObject);
	auto jvmObject_constructor = jvm->lookupConstructor(jvmObject, {});
	assert(jvmObject_constructor);
	auto item = jvm->createObject(jvmObject, jvmObject_constructor, {});
	assert(item);

	auto jvmArrayList_add = jvm->lookupMethod(container, "add", { jvmObject }, jvm->lookupType("boolean"));
	assert(jvmArrayList_add);

	bool exceptionCatched = false;
	try {
		auto result = jvm->call(container, jvmArrayList_add, { item });
	}
	catch (...) {
		exceptionCatched = true;
	}
	assert(exceptionCatched);
}
