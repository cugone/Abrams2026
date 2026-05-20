#pragma once

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Renderer/Texture.hpp"

#include <filesystem>
#include <memory>
#include <vector>

class Flipbook {
public:

    enum class Playmode : uint8_t {
        Forward
        ,Backward
        ,StepForward
        ,StepBackward
        ,Paused
    };

    Flipbook() = delete;
    Flipbook(std::filesystem::path folderpath, unsigned int framesPerSecond = 3) noexcept;
    Flipbook(std::unique_ptr<Texture>&& texture, unsigned int framesPerSecond = 3) noexcept;
    Flipbook(const Flipbook& other) = default;
    Flipbook(Flipbook&& other) = default;
    Flipbook& operator=(const Flipbook& other) = default;
    Flipbook& operator=(Flipbook&& other) = default;
    ~Flipbook() noexcept;

    void Update([[maybe_unused]] TimeUtils:: FPSeconds deltaSeconds) noexcept;
    void Render(const Matrix4& transform = Matrix4::I) const noexcept;

    Vector2 GetDimensions() const noexcept;

    void SetFrameRate(unsigned int framesPerSecond) noexcept;
    void SetDuration(TimeUtils::FPSeconds duration) noexcept;
    void SetFrame(int frameIndex) noexcept;
    void AdvanceFrame(int frames_to_advance) noexcept;
    void TogglePause() noexcept;
    void StepForward() noexcept;
    void StepBackward() noexcept;

    void SetPlaymode(const Playmode& mode) noexcept;
    void ClearPlaymode(const Playmode& mode) noexcept;
    bool CheckPlaymode(const Playmode& mode) const noexcept;

protected:
private:
    Playmode m_playmode{Playmode::Forward};
    std::unique_ptr<Texture> m_texture{};
    int m_currentFrame{0};
    IntVector2 m_frameDimensions{};
    Stopwatch m_frameRate{};
    float m_playRate{1.0f};
    //0b0000'FBSP
    uint8_t m_state{};
};
