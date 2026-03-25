#pragma once

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/TypeUtils.hpp"

#include <bitset>
#include <string>
#include <stdexcept>
#include <type_traits>

class Rgba;
class Vector2;
class Vector3;
class Vector4;
class IntVector2;
class IntVector3;
class IntVector4;
class Matrix4;

// clang-format off
enum class ArgumentParserState : uint8_t {
    None
    , BadBit
    , FailBit
    , EndOfFileBit
    , Max
};
// clang-format on

template<>
struct TypeUtils::is_bitflag_enum_type<ArgumentParserState> : std::true_type {};

class ArgumentParser {
public:
    explicit ArgumentParser(const std::string& args) noexcept;
    template<typename T>
    friend ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) noexcept;
    void clear() noexcept;
    [[nodiscard]] bool fail() const noexcept;
    [[nodiscard]] bool good() const noexcept;
    [[nodiscard]] bool bad() const noexcept;
    [[nodiscard]] bool eof() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;
    [[nodiscard]] bool operator!() const noexcept;

    bool GetNext(Rgba& value) const noexcept;
    bool GetNext(Vector2& value) const noexcept;
    bool GetNext(Vector3& value) const noexcept;
    bool GetNext(Vector4& value) const noexcept;
    bool GetNext(IntVector2& value) const noexcept;
    bool GetNext(IntVector3& value) const noexcept;
    bool GetNext(IntVector4& value) const noexcept;
    bool GetNext(Matrix4& value) const noexcept;

    [[nodiscard]] bool GetNext(std::string& value) const noexcept;
    bool GetNext(bool& value) const noexcept;
    bool GetNext(unsigned char& value) const noexcept;
    bool GetNext(signed char& value) const noexcept;
    bool GetNext(char& value) const noexcept;
    bool GetNext(unsigned short& value) const noexcept;
    bool GetNext(short& value) const noexcept;
    bool GetNext(unsigned int& value) const noexcept;
    bool GetNext(int& value) const noexcept;
    bool GetNext(unsigned long& value) const noexcept;
    bool GetNext(long& value) const noexcept;
    bool GetNext(unsigned long long& value) const noexcept;
    bool GetNext(long long& value) const noexcept;
    bool GetNext(float& value) const noexcept;
    bool GetNext(double& value) const noexcept;
    bool GetNext(long double& value) const noexcept;

protected:
private:
    void SetState(const ArgumentParserState& stateBits, bool newValue) const noexcept;
    bool GetNextValueFromBuffer(std::string& value) const noexcept;
    mutable std::string m_current{};
    mutable std::bitset<static_cast<std::size_t>(ArgumentParserState::Max)> m_state_bits{};

    template<typename T>
    bool GetNext_udt_helper(T& value) const noexcept {
        std::string value_str{};
        if(GetNext(value_str)) {
            value = T(value_str);
            return true;
        }
        SetState(ArgumentParserState::BadBit, true);
        return false;
    }

    template<typename T>
    bool GetNext_builtin_helper(T& value) const noexcept {
        std::string value_str{};
        if(GetNext(value_str)) {
            try {
                if constexpr(std::is_integral_v<T> && std::is_same_v<std::remove_cv_t<T>, bool>) {
                    //Is bool integral type
                    try {
                        value = static_cast<T>(std::stoul(value_str));
                    } catch([[maybe_unused]] std::invalid_argument& e) {
                        value_str = StringUtils::ToLowerCase(value_str);
                        if(value_str == "true") {
                            value = true;
                            return true;
                        }
                        if(value_str == "false") {
                            value = false;
                            return true;
                        }
                        SetState(ArgumentParserState::BadBit, true);
                        return false;
                    }
                    return true;
                } else if constexpr(std::is_unsigned_v<T> && std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>) {
                    //Is nonbool unsigned integral type
                    value = static_cast<T>(std::stoull(value_str));
                } else if constexpr(std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>) {
                    //Is nonbool signed integral type
                    value = static_cast<T>(std::stoll(value_str));
                } else if constexpr(std::is_floating_point_v<T>) {
                    value = static_cast<T>(std::stold(value_str));
                }
            } catch([[maybe_unused]] std::invalid_argument& e) {
                SetState(ArgumentParserState::BadBit, true);
                return false;
            }
            return true;
        }
        SetState(ArgumentParserState::BadBit, true);
        return false;
    }

};

template<typename T>
ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) noexcept {
    (void)parser.GetNext(std::forward<T>(arg)); //GetNext sets the state of the parser.
    return parser;                              //It is implicitly converted to bool, a bad state from the previous read is false.
}
