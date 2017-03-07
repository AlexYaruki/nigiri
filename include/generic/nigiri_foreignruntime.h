#ifndef NIGIRI_FOREIGNRUNTIME_H
#define NIGIRI_FOREIGNRUNTIME_H

#include <vector>
#include <memory>
#include <experimental/optional>
#include "nigiri_build.h"
#include "nigiri.h"

#if defined(__linux__)
#   define FR_PREFIX "libnigiri_"
#   define FR_SUFFIX ".so"
#elif defined(WIN32)
#   define FR_PREFIX "nigiri_"
#   define FR_SUFFIX ".dll"
#elif defined(__APPLE__)
#   define FR_PREFIX "libnigiri_"
#   define FR_SUFFIX ".dylib"
#else
#   error This platform is not supported
#endif

namespace nigiri {

    using FR_LOADER_FUNC_TYPE = void*(*)(void);
    using FR_Id = uint64_t;
    const std::string FR_LOADER_FUNC_NAME = "fr_loader";

    enum class FR_PrimitiveType {
        Int8,
        Int16,
        Int32,
        Int64
    };

    class NIGIRI_EXPORT FR_Type {
    public:
        virtual ~FR_Type() = default;
        virtual FR_Id getRuntimeId() = 0;
        virtual const std::string& getName() = 0;
    };

    class NIGIRI_EXPORT FR_Object {
    public:
        virtual ~FR_Object() = default;
        virtual FR_Id getRuntimeId() = 0;
        virtual std::shared_ptr<FR_Type> getType() = 0;
        virtual std::experimental::optional<uint16_t> castToUInt16() = 0;
        virtual std::experimental::optional<bool> castToBool() = 0;
        virtual std::experimental::optional<int8_t> castToInt8() = 0;
        virtual std::experimental::optional<int16_t> castToInt16() = 0;
        virtual std::experimental::optional<int32_t> castToInt32() = 0;
        virtual std::experimental::optional<int64_t> castToInt64() = 0;
        virtual std::experimental::optional<float> castToFloat() = 0;
        virtual std::experimental::optional<double> castToDouble() = 0;
    };

    class NIGIRI_EXPORT FR_Method {
    public:
        virtual ~FR_Method() = default;
        virtual FR_Id getRuntimeId() = 0;
    };

    class NIGIRI_EXPORT ForeignRuntime {
    public:
        virtual ~ForeignRuntime() = default;
        virtual FR_Id getId() = 0;
        virtual bool start() = 0;
        virtual void stop() = 0;
        virtual bool isRunning() = 0;
        virtual std::shared_ptr<FR_Type> lookupType(const std::string& name) = 0;

        virtual std::shared_ptr<FR_Method> lookupMethod(std::shared_ptr<FR_Type> targetType,
                                                        const std::string& name,
                                                        const std::vector<std::shared_ptr<FR_Type>>& parameterTypes,
                                                        const std::shared_ptr<FR_Type> returnType) = 0;

        virtual std::shared_ptr<FR_Method> lookupMethod(std::shared_ptr<FR_Object> targetType,
                                                        const std::string& name,
                                                        const std::vector<std::shared_ptr<FR_Type>>& parameterTypes,
                                                        const std::shared_ptr<FR_Type> returnType) = 0;

        virtual std::shared_ptr<FR_Method> lookupConstructor(std::shared_ptr<FR_Type> targetType,
                                                        const std::vector<std::shared_ptr<FR_Type>>& parameterTypes) = 0;

        virtual std::shared_ptr<FR_Object> call(std::shared_ptr<FR_Type> targetType,
                                                std::shared_ptr<FR_Method> method,
                                                const std::vector<std::shared_ptr<FR_Object>>& parameters) = 0;

        virtual std::shared_ptr<FR_Object> call(std::shared_ptr<FR_Object> targetObject,
                                                std::shared_ptr<FR_Method> method,
                                                const std::vector<std::shared_ptr<FR_Object>>& parameters) = 0;

        virtual std::shared_ptr<FR_Object> createObject(std::shared_ptr<FR_Type> type,
                                                        std::shared_ptr<FR_Method> constructor,
                                                        const std::vector<std::shared_ptr<FR_Object>>& parameters) = 0;

        virtual std::shared_ptr<FR_Object> wrapPrimitive(uint16_t p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(bool p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(int8_t p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(int16_t p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(int32_t p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(int64_t p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(float p) = 0;
        virtual std::shared_ptr<FR_Object> wrapPrimitive(double p) = 0;
        virtual std::string toString(std::shared_ptr<FR_Object> obj) = 0;
    };

    class NIGIRI_EXPORT ForeignRuntimeLoader {
    public:
        virtual ~ForeignRuntimeLoader() = default;
        virtual bool check() = 0;
        virtual std::shared_ptr<ForeignRuntime> load(FR_Id id) = 0;
    };

    class NIGIRI_EXPORT ForeignRuntimeManager {
    public:
        static bool isCategoryAvailable(const std::string& name);
        static std::shared_ptr<ForeignRuntime> createRuntime(const std::string& name);
        static std::shared_ptr<ForeignRuntime> getRuntime(FR_Id id);
    };

}
#endif //NIGIRI_FOREIGNRUNTIME_H
