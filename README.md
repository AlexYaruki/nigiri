# nigiri

C++ library for launching and interacting with Foreign Runtimes (JVM,CLR,V8)

Aim is to provide one API for common operations (Launching, Type lookup, Code execution and Shutdown) instead for runtime-specific APIs - JVM -> JNI, CLR -> CLR Hosting, etc.


### Operation

* Type lookup
* Constructor lookup
* Type method lookup (in Java: class methods)
* Object method lookup (in Java: instance methods)

### Operations (TODO)

* Field access
