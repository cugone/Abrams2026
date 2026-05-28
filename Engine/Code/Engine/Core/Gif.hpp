#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/IntVector2.hpp"

#include "Engine/Renderer/Texture.hpp"

#include <chrono>
#include <filesystem>
#include <memory>

namespace FileUtils {

struct GifDesc;

class Gif {
public:
    enum class PlayMode {
        PlayToEnd
        ,PlayToBeginning
        ,Loop
        ,Reverse
        ,PingPong
    };
    Gif() = default;
    Gif(const Gif& other) = default;
    Gif(Gif&& other) = default;
    Gif& operator=(const Gif& other) = default;
    Gif& operator=(Gif&& other) = default;
    ~Gif() noexcept = default;

    explicit Gif(const GifDesc& desc) noexcept;

    void SetPlayMode(const Gif::PlayMode& newMode) noexcept;
    void SetStartFrame(const std::size_t& newStartFrame) noexcept;
    void SetEndFrame(const std::size_t& newEndFrame) noexcept;
    void SetFrameRange(const std::size_t& newStartFrame, const std::size_t& newEndFrame) noexcept;

    bool Load(std::filesystem::path filepath) noexcept;
    void Update(TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render(const Matrix4& transform = Matrix4::I) const noexcept;

    IntVector2 GetDimensions() const noexcept;
    std::size_t GetFrameCount() const noexcept;
    std::size_t GetCurrentFrame() const noexcept;
    void SetImage(std::filesystem::path newFilepath) noexcept;

    bool IsAtEnd() const noexcept;
    bool IsAtBeginning() const noexcept;
    bool IsAtFrameStart() const noexcept;
    bool IsAtFrameEnd() const noexcept;
    bool IsAtFrameStart(std::size_t frameIdx) const noexcept;
    bool IsAtFrameEnd(std::size_t frameIdx) const noexcept;
    void Restart() noexcept;

protected:
private:
    bool Load(const GifDesc& desc) noexcept;

    std::vector<std::chrono::milliseconds> m_frameDelays{};
    Texture* m_texture{};
    TimeUtils::FPSeconds m_duration{};
    TimeUtils::FPSeconds m_frameDuration{};
    std::size_t m_currentFrame{0u};
    std::size_t m_startFrame{0u};
    std::size_t m_endFrame{static_cast<std::size_t>(-1)};
    std::size_t m_totalFrames{};
    Gif::PlayMode m_playMode{Gif::PlayMode::Loop};
    int m_direction{1};
};

struct GifDesc {
    std::filesystem::path filepath{};
    std::size_t startFrame{0u};
    std::size_t endFrame{static_cast<std::size_t>(-1)};
    Gif::PlayMode playMode{Gif::PlayMode::Loop};
};


} // namespace FileUtils

