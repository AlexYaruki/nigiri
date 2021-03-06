# nigiri

C++ library for launching and interacting with Foreign Runtimes (JVM,CLR,V8)

Aim is to provide one API for common operations (Launching, Type lookup, Code execution and Shutdown) instead for runtime-specific APIs - JVM -> JNI, CLR -> CLR Hosting, etc.


### Features:

| Operation                         | JVM |
|-----------------------------------|-----|
| Type lookup                       | Yes |
| Constructor lookup                | Yes |
| Type method lookup                | Yes |
| Type method call                  | Yes |
| Type field lookup                 | Yes |
| Object method lookup              | Yes |
| Object method call                | Yes |
| Object field lookup               | Yes |
| Primitive type wrapping           | Yes |
| Conversion to text representation | Yes |

### TODO:

* Arrays
* Options
* Optional logging ( environment variable, option or other)
* One thread use synchronisation (for now)
* Internal Type/Constructor/Method/Field cache (probably only JVM specific)
* Access to path of resources loaded with runtime (JAR,DLL,JS, etc...)
* Execution modes: Thread or Process (required check if runtime support requested mode)
* JVM: Use of java.lang.Class for accessing required information (wrapper)
* JVM: Generics
* JVM: Inner classes
* JVM: JNI do not respect private/protected/"default" scopes, additional checkup needed before scheduling work on JVM
