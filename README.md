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
* Generics
* Options
* JVM: Inner classes
