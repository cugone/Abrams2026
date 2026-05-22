#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/TypeUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include <Thirdparty/TinyXML2/tinyxml2.h>

#include <bit>
#include <concepts>
#include <cstdlib>
#include <cstdint>
#include <functional>
#include <filesystem>
#include <span>
#include <string>
#include <type_traits>


using XMLElement = tinyxml2::XMLElement;
using XMLAttribute = tinyxml2::XMLAttribute;

namespace DataUtils {

namespace Base64 {

struct Base64CodecOptions {
    bool use_filename_safe_table{false};
};
[[nodiscard]] std::string encode(std::span<const std::uint8_t> input, const Base64CodecOptions& options = Base64CodecOptions{}) noexcept;
[[nodiscard]] std::string encode(std::span<const char> input) noexcept;
[[nodiscard]] std::string encode(const std::filesystem::path& input) noexcept;
[[nodiscard]] std::string encode(const std::string& input) noexcept;

[[nodiscard]] std::string decode(std::span<const char> input, const Base64CodecOptions& options = Base64CodecOptions{}) noexcept;

} //namespace Base64

[[nodiscard]] constexpr inline auto ShiftLeft(uint8_t value, uint8_t distance) noexcept -> uint8_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint16_t value, uint16_t distance) noexcept -> uint16_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint32_t value, uint32_t distance) noexcept -> uint32_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftLeft(uint64_t value, uint64_t distance) noexcept -> uint64_t {
    return value << distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint8_t value, uint8_t distance) noexcept -> uint8_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint16_t value, uint16_t distance) noexcept -> uint16_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint32_t value, uint32_t distance) noexcept -> uint32_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto ShiftRight(uint64_t value, uint64_t distance) noexcept -> uint64_t {
    return value >> distance;
}

[[nodiscard]] constexpr inline auto Bit(uint8_t n) noexcept -> uint8_t {
    return ShiftLeft(1, n);
}

[[nodiscard]] constexpr inline auto Bit(uint16_t n) noexcept -> uint16_t {
    return ShiftLeft(1, n);
}

[[nodiscard]] constexpr inline auto Bit(uint32_t n) noexcept -> uint32_t {
    return ShiftLeft(1, n);
}

[[nodiscard]] constexpr inline auto Bit(uint64_t n) noexcept -> uint64_t {
    return ShiftLeft(1, n);
}

//Unconditional byte order swap. Deprecated. Replace with std::byteswap when available.
[[nodiscard]] inline auto EndianSwap(uint16_t value) noexcept -> uint16_t {
    return _byteswap_ushort(value);
}

//Unconditional byte order swap. Deprecated. Replace with std::byteswap when available.
[[nodiscard]] inline auto EndianSwap(uint32_t value) noexcept -> uint32_t {
    return _byteswap_ulong(value);
}

//Unconditional byte order swap. Deprecated. Replace with std::byteswap when available.
[[nodiscard]] inline auto EndianSwap(uint64_t value) noexcept -> uint64_t {
    return _byteswap_uint64(value);
}

void ValidateXmlElement(const XMLElement& element,
                        std::string name,
                        std::string requiredChildElements,
                        std::string requiredAttributes,
                        std::string optionalChildElements = std::string{},
                        std::string optionalAttributes = std::string{}) noexcept;

void ValidateXmlAttribute(const XMLElement& elem, std::string attributeName, std::string validValuesList) noexcept;


std::string EscapeGlyphToXmlCharacterEntity(const char glyph) noexcept;

std::string GetElementTextAsString(const XMLElement& element);
std::string GetAttributeAsString(const XMLElement& element, const std::string& attributeName);

[[nodiscard]] std::size_t GetAttributeCount(const XMLElement& element) noexcept;
[[nodiscard]] std::size_t GetChildElementCount(const XMLElement& element, const std::string& elementName = std::string{}) noexcept;

[[nodiscard]] std::string GetElementName(const XMLElement& elem) noexcept;
[[nodiscard]] std::vector<std::string> GetChildElementNames(const XMLElement& element) noexcept;
[[nodiscard]] bool HasChild(const XMLElement& elem) noexcept;
[[nodiscard]] bool HasChild(const XMLElement& elem, const std::string& name) noexcept;

[[nodiscard]] std::string GetAttributeName(const XMLAttribute& attrib) noexcept;
[[nodiscard]] std::vector<std::string> GetAttributeNames(const XMLElement& element) noexcept;
[[nodiscard]] bool HasAttribute(const XMLElement& element) noexcept;
[[nodiscard]] bool HasAttribute(const XMLElement& element, const std::string& name);

//************************************
// Method:    ForEachChildElement
// FullName:  DataUtils::ForEachChildElement
// Access:    public
// Returns:   UnaryFunction: A copy of the UnaryFunction Callable argument.
// Qualifier: noexcept
// Parameter: const XMLElement& element: The parent element.
// Parameter: const std::string& childname: The name of the child element to iterate. Provide an empty string to iterate over all children.
// Parameter: UnaryFunction&& f: UnaryFunction Callable to invoke for each child element of the parent. Must be of the signature void f(const XMLElement&).
//************************************
template<typename UnaryFunction>
UnaryFunction ForEachChildElement(const XMLElement& element, const std::string& childname, UnaryFunction&& f) noexcept {
    auto childNameAsCStr = childname.empty() ? nullptr : childname.c_str();
    for(auto* xml_iter = element.FirstChildElement(childNameAsCStr); xml_iter != nullptr; xml_iter = xml_iter->NextSiblingElement(childNameAsCStr)) {
        std::invoke(f, *xml_iter);
    }
    return f;
}

//************************************
// Method:    ForEachAttribute
// FullName:  DataUtils::ForEachAttribute
// Access:    public
// Returns:   UnaryFunction: A copy of the UnaryFunction Callable argument.
// Qualifier: noexcept
// Parameter: const XMLElement& element: The parent element.
// Parameter: UnaryFunction&& f: UnaryFunction Callable to invoke for each attribute of the parent element. Must be of the signature void f(const XMLAttribute&).
//************************************
template<typename UnaryFunction>
UnaryFunction ForEachAttribute(const XMLElement& element, UnaryFunction&& f) noexcept {
    for(auto* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
        std::invoke(f, *attribute);
    }
    return f;
}

namespace detail {

bool to_bool(const std::string& value) noexcept;

template<typename T>
[[nodiscard]] const T CalculateUnboundedIntegerRangeResult() noexcept {
    if constexpr(std::is_same_v<T, bool>) {
        return static_cast<T>(MathUtils::GetRandomLessThan(2));
    } else if constexpr(!std::is_unsigned_v<T>) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
    } else {
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomLessThan(upper));
    }
}

template<typename T>
[[nodiscard]] const T CalculateUpperBoundedIntegerRangeResult(std::string maxvalue) noexcept {
    if constexpr (std::is_same_v<T, bool>) {
        constexpr auto lower = (std::numeric_limits<const bool>::min)();
        auto upper = to_bool(maxvalue);
        if(lower == upper) {
            return lower;
        } else {
            return static_cast<T>(MathUtils::GetRandomLessThan(2));
        }
    } else if constexpr(!std::is_unsigned_v<T>) {
        constexpr auto lower = (std::numeric_limits<T>::min)();
        const auto upper = static_cast<T>(std::stoll(maxvalue));
        return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
    } else {
        const auto upper = static_cast<T>(std::stoull(maxvalue));
        return static_cast<T>(MathUtils::GetRandomLessThan(upper));
    }
}

template<typename T>
[[nodiscard]] const T CalculateLowerBoundedIntegerRangeResult(std::string minvalue) noexcept {
    if constexpr(std::is_same_v<T, bool>) {
        const auto lower = to_bool(minvalue);
        constexpr auto upper = (std::numeric_limits<const bool>::max)();
        if(lower == upper) {
            return lower;
        } else {
            return static_cast<T>(MathUtils::GetRandomLessThan(2));
        }
    } else if constexpr(!std::is_unsigned_v<T>) {
        const auto lower = static_cast<T>(std::stoll(minvalue));
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
    } else {
        const auto lower = static_cast<T>(std::stoull(minvalue));
        constexpr auto upper = (std::numeric_limits<T>::max)();
        return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
    }
}

template<typename T>
[[nodiscard]] const T CalculateClosedIntegerRangeResult(std::string minvalue, std::string maxvalue) noexcept {
    if constexpr(std::is_same_v<T, bool>) {
        const auto lower = to_bool(minvalue);
        const auto upper = to_bool(maxvalue);
        if(lower == upper) {
            return lower;
        } else {
            return static_cast<T>(MathUtils::GetRandomLessThan(2));
        }
    } else if constexpr(!std::is_unsigned_v<T>) {
        const auto lower = static_cast<T>(std::stoll(minvalue));
        const auto upper = static_cast<T>(std::stoll(maxvalue));
        return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
    } else {
        const auto lower = static_cast<T>(std::stoull(minvalue));
        const auto upper = static_cast<T>(std::stoull(maxvalue));
        return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
    }
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] const T CalculateUnboundedFloatRangeResult() noexcept {
    constexpr auto lower = (std::numeric_limits<T>::min)();
    constexpr auto upper = (std::numeric_limits<T>::max)();
    return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] const T CalculateUpperBoundedFloatRangeResult(std::string maxvalue) noexcept {
    constexpr auto lower = (std::numeric_limits<T>::min)();
    const auto upper = static_cast<T>(std::stold(maxvalue));
    return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] const T CalculateLowerBoundedFloatRangeResult(std::string minvalue) noexcept {
    const auto lower = static_cast<T>(std::stold(minvalue));
    constexpr auto upper = (std::numeric_limits<T>::max)();
    return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] const T CalculateClosedFloatRangeResult(std::string minvalue, std::string maxvalue) noexcept {
    const auto lower = static_cast<T>(std::stold(minvalue));
    const auto upper = static_cast<T>(std::stold(maxvalue));
    return static_cast<T>(MathUtils::GetRandomInRange(lower, upper));
}

template<typename T>
[[nodiscard]] const T CalculateIntegerRangeResult(std::string txt) noexcept {
    const auto values = StringUtils::SplitOnFirst(txt, '~');
    if(values.first.empty() && values.second.empty()) {
        return CalculateUnboundedIntegerRangeResult<T>();
    } else if(values.first.empty()) {
        return CalculateUpperBoundedIntegerRangeResult<T>(values.second);
    } else if(values.second.empty()) {
        return CalculateLowerBoundedIntegerRangeResult<T>(values.first);
    } else {
        return CalculateClosedIntegerRangeResult<T>(values.first, values.second);
    }
}

template<typename T>
requires std::floating_point<T>
[[nodiscard]] const auto CalculateFloatRangeResult(const std::string& txt) noexcept {
    const auto values = StringUtils::SplitOnFirst(txt, '~');
    if(values.first.empty() && values.second.empty()) {
        return detail::CalculateUnboundedFloatRangeResult<T>();
    } else if(values.first.empty()) {
        return detail::CalculateUpperBoundedFloatRangeResult<T>(values.second);
    } else if(values.second.empty()) {
        return detail::CalculateLowerBoundedFloatRangeResult<T>(values.first);
    } else {
        return detail::CalculateClosedFloatRangeResult<T>(values.first, values.second);
    }
}

template<typename T>
[[nodiscard]] const auto CalculateRangeResult(const std::string& txt) noexcept {
    //std::uniform_int_distribution doesn't allow 8-bit types or bool.
    constexpr auto is_invalid_type_v = TypeUtils::is_any_of<std::remove_cvref_t<T>, unsigned char, signed char, char, int8_t, uint8_t>;
    if constexpr(!is_invalid_type_v && std::is_integral_v<T>) {
        return CalculateIntegerRangeResult<T>(txt);
    } else if constexpr(std::is_floating_point_v<T>) {
        return CalculateFloatRangeResult<T>(txt);
    } else {
        if constexpr (is_invalid_type_v) {
            return static_cast<std::add_const_t<std::remove_cvref_t<T>>>(CalculateIntegerRangeResult<int>(txt));
        } else {
            return T{txt};
        }
    }
}

} // namespace detail

template<typename T>
[[nodiscard]] T ParseXmlAttribute(const XMLElement& element, const std::string& attributeName, const T defaultValue) noexcept {
    auto retVal = defaultValue;
    const auto attr = GetAttributeAsString(element, attributeName);
    const auto is_range = attr.find('~') != std::string::npos;
    if(is_range && attr == "~") {
        if constexpr(std::is_same_v<std::remove_cvref_t<T>, char>) {
            return attr[0];
        } else if constexpr(std::is_unsigned_v<T>) {
            return static_cast<T>(std::stoull(attr));
        } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
            return static_cast<T>(std::stoll(attr));
        } else if constexpr(std::is_signed_v<T> && std::is_floating_point_v<T>) {
            return static_cast<T>(std::stold(attr));
        } else {
            if(attr.empty()) {
                return defaultValue;
            } else {
                return T{attr};
            }
        }
    }
    if(!is_range) {
        try {
            if constexpr(std::is_same_v<T, bool>) {
                element.QueryBoolAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_same_v<T, unsigned char>) {
                auto ucharVal = static_cast<unsigned int>(retVal);
                element.QueryUnsignedAttribute(attributeName.c_str(), &ucharVal);
                return static_cast<T>(ucharVal);
            } else if constexpr(std::is_same_v<T, char>) {
                const auto* s = element.Attribute(attributeName.c_str());
                return T(s ? s[0] : defaultValue);
            } else if constexpr(std::is_same_v<T, signed char>) {
                auto scharVal = static_cast<signed int>(retVal);
                element.QueryIntAttribute(attributeName.c_str(), &scharVal);
                return static_cast<T>(scharVal);
            } else if constexpr(std::is_same_v<T, std::string>) {
                const auto* s = element.Attribute(attributeName.c_str());
                return T(s ? s : defaultValue);
            } else if constexpr(std::is_same_v<T, const char*>) {
                const auto* s = element.Attribute(attributeName.c_str());
                return s ? s : "";
            } else if constexpr(std::is_same_v<T, std::size_t>) {
                if constexpr(std::is_same_v<T, std::uint64_t>) {
                    element.QueryUnsigned64Attribute(attributeName.c_str(), &retVal);
                } else {
                    element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
                }
            } else if constexpr(std::is_unsigned_v<T> && std::is_same_v<T, std::uint64_t>) {
                element.QueryUnsigned64Attribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_unsigned_v<T> && std::is_same_v<T, std::uint32_t>) {
                element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_unsigned_v<T>) {
                element.QueryUnsignedAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_signed_v<T> && std::is_same_v<T, std::int64_t>) {
                element.QueryInt64Attribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_signed_v<T> && std::is_same_v<T, std::int32_t>) {
                element.QueryIntAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
                element.QueryIntAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_floating_point_v<T> && std::is_same_v<T, float>) {
                element.QueryFloatAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_floating_point_v<T> && std::is_same_v<T, double>) {
                element.QueryDoubleAttribute(attributeName.c_str(), &retVal);
            } else if constexpr(std::is_floating_point_v<T> && std::is_same_v<T, long double>) {
                element.QueryDoubleAttribute(attributeName.c_str(), &retVal);
            } else {
                if(attr.empty()) {
                    return defaultValue;
                } else {
                    return T{attr};
                }
            }
        } catch(...) {
            return defaultValue;
        }
    } else {
        if (const auto values = StringUtils::SplitOnFirst(attr, '~'); (values.first.empty() || values.second.empty())) {
            if(const std::string value = values.first.empty() ? values.second : (values.second.empty() ? values.first : attr); !value.empty()) {
                if constexpr(std::is_unsigned_v<T>) {
                    return static_cast<T>(std::stoull(value));
                } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
                    return static_cast<T>(std::stoll(value));
                } else if constexpr(std::is_signed_v<T> && std::is_floating_point_v<T>) {
                    return static_cast<T>(std::stold(value));
                } else {
                    if(attr.empty()) {
                        return defaultValue;
                    } else {
                        return T{attr};
                    }
                }
            }
            return defaultValue;
        }
        retVal = static_cast<T>(detail::CalculateRangeResult<decltype(retVal)>(attr));
    }
    return retVal;
}

template<typename T>
[[nodiscard]] T ParseXmlElementText(const XMLElement& element, T defaultValue) noexcept {
    auto retVal = defaultValue;
    using R = decltype(retVal);
    const auto txt = GetElementTextAsString(element);
    const auto is_range = txt.find('~') != std::string::npos;
    if(!is_range) {
        try {
            if constexpr(std::is_same_v<T, bool>) {
                const auto txt_bool = StringUtils::ToLowerCase(txt);
                if(txt_bool == "true") {
                    return true;
                } else if(txt_bool == "false") {
                    return false;
                } else {
                    try {
                        return static_cast<R>(std::stoi(txt_bool));
                    } catch(...) {
                        return defaultValue;
                    }
                }
            } else if constexpr(std::is_unsigned_v<T>) {
                retVal = static_cast<R>(std::stoul(txt));
            } else if constexpr(std::is_signed_v<T> && !std::is_floating_point_v<T>) {
                retVal = static_cast<R>(std::stoll(txt));
            } else if constexpr(std::is_signed_v<T> && std::is_floating_point_v<T>) {
                retVal = static_cast<R>(std::stold(txt));
            } else {
                if(txt.empty()) {
                    return defaultValue;
                } else {
                    return T(txt);
                }
            }
        } catch(...) {
            return defaultValue;
        }
    } else {
        if(!txt.empty()) {
            retVal = detail::CalculateRangeResult<R>(txt);
        } else {
            return defaultValue;
        }
    }
    return retVal;
}

} // namespace DataUtils
