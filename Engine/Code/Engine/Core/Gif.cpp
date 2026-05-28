#include "Engine/Core/Gif.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IFileLoggerService.hpp"
#include "Engine/Services/IRendererService.hpp"

#include "Thirdparty/stb/stb_image.h"

#include <numeric>

namespace FileUtils {

Gif::Gif(const GifDesc& desc) noexcept
: m_startFrame{desc.startFrame}
, m_endFrame{desc.endFrame}
, m_playMode{desc.playMode}
{
    const bool succeeded = Load(desc);
    GUARANTEE_OR_DIE(succeeded, "Gif: Failed to load");
}

void Gif::SetPlayMode(const Gif::PlayMode& newMode) noexcept {
    m_playMode = newMode;
}

void Gif::SetStartFrame(const std::size_t& newStartFrame) noexcept {
    m_startFrame = newStartFrame;
    if(m_currentFrame < m_startFrame) {
        m_currentFrame = m_startFrame;
    }
}

void Gif::SetEndFrame(const std::size_t& newEndFrame) noexcept {
    m_endFrame = newEndFrame;
    if(m_currentFrame > m_endFrame) {
        m_currentFrame = m_endFrame;
    }
}

void Gif::SetFrameRange(const std::size_t& newStartFrame, const std::size_t& newEndFrame) noexcept {
    if(newStartFrame > newEndFrame) {
        SetStartFrame(newEndFrame);
        SetEndFrame(newStartFrame);
    } else {
        SetStartFrame(newStartFrame);
        SetEndFrame(newEndFrame);
    }
}

bool Gif::Load(const GifDesc& desc) noexcept {
    SetFrameRange(desc.startFrame, desc.endFrame);
    SetPlayMode(desc.playMode);
    return Load(desc.filepath);
}

bool Gif::Load(std::filesystem::path filepath) noexcept {
    auto* r = ServiceLocator::get<IRendererService>();
    auto* logger = ServiceLocator::get<IFileLoggerService>();
    if(!std::filesystem::exists(filepath)) {
        logger->LogLineAndFlush(std::format("File: {} does not exist.", filepath));
        return false;
    }
    {
        std::error_code ec{};
        if(filepath = std::filesystem::canonical(filepath, ec); ec) {
            logger->LogErrorLine(std::format("File: {} is inaccessible.", filepath));
            return {};
        }
    }
    filepath.make_preferred();
    {
        const auto magicBuffer = FileUtils::ReadSomeBinaryBufferFromFile(filepath, 0, 6);
        if(const bool isGif = magicBuffer.has_value() && (*magicBuffer == "GIF89a" || *magicBuffer == "GIF87a"); !isGif) {
            logger->LogLineAndFlush(std::format("File: {} is not a .gif", filepath));
            return false;
        }
    }

    if(const auto buffer = FileUtils::ReadBinaryBufferFromFile(filepath); !buffer.has_value()) {
        logger->LogLineAndFlush("Gif: failed to read binary buffer.\n");
        return false;
    } else {
        int width{0};
        int height{0};
        int frame_count{0};
        int* delays{nullptr};
        auto data_deleter = [](uint8_t* p) {
            stbi_image_free(p);
        };
        auto data = std::unique_ptr<uint8_t, decltype(data_deleter)>(stbi_load_gif_from_memory((*buffer).data(), static_cast<int>((*buffer).size()), &delays, &width, &height, &frame_count, nullptr, 4), data_deleter);
        if(data.get() == nullptr) {
            logger->LogLineAndFlush(std::format("stbi failed to load .gif from file: {}", filepath));
            return false;
        }
        m_frameDelays = std::vector<std::chrono::milliseconds>(delays, delays + frame_count);
        m_totalFrames = frame_count;
        m_endFrame = (std::min)(m_endFrame, m_totalFrames - std::size_t{1u});
        m_currentFrame = m_startFrame;
        m_direction = 1;
        if(m_playMode == PlayMode::PlayToBeginning || m_playMode == PlayMode::Reverse) {
            m_currentFrame = m_endFrame;
            m_direction = -1;
        }
        m_duration = std::chrono::milliseconds{std::accumulate(std::cbegin(m_frameDelays), std::cend(m_frameDelays), std::chrono::milliseconds::zero(), [&](const auto& a, const auto& b) { return a + b; })};
        if(m_texture = r->GetTexture(filepath.string()); m_texture != nullptr) {
            return true;
        }
        if(auto texture = r->Create2DTextureArrayFromMemory(data.get(), width, height, frame_count); texture != nullptr) {
            m_texture = texture.get();
            if(!r->RegisterTexture(filepath.string(), std::move(texture))) {
                logger->LogLineAndFlush(std::format("Failed to register texture from .gif file: {}", filepath));
                return false;
            }
        }
        return true;
    }
}

void Gif::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    m_frameDuration += deltaSeconds;
    const auto frame_delay = [this]() {
        try {
            return m_frameDelays.at(m_currentFrame);
        } catch(...) {
            return m_frameDelays[m_startFrame];
        }
    }();
    if(m_frameDuration >= frame_delay) {
        m_currentFrame += m_direction;
        m_frameDuration = TimeUtils::FPSeconds::zero();
    }
    switch(m_playMode) {
    case PlayMode::PingPong:
        if(m_currentFrame < m_startFrame || m_currentFrame == std::size_t(-1)) {
            m_currentFrame = m_startFrame;
            m_direction = -m_direction;
        }
        if(m_currentFrame > m_endFrame) {
            m_currentFrame = m_endFrame;
            m_direction = -m_direction;
        }
        break;
    case PlayMode::PlayToBeginning:
        [[fallthrough]];
    case PlayMode::Loop:
        if(m_currentFrame > m_endFrame) {
            m_currentFrame = m_startFrame;
        }
        break;
    case PlayMode::PlayToEnd:
        [[fallthrough]];
    case PlayMode::Reverse:
        if(m_currentFrame > m_endFrame) {
            m_currentFrame = m_endFrame;
        }
        break;
    default:
        /* DO NOTHING */
        break;
    }
}

void Gif::Render(const Matrix4& transform /*= Matrix4::I*/) const noexcept {
    auto* r = ServiceLocator::get<IRendererService>();
    auto* mat = r->GetMaterial("__unlit2DSprite");
    if(const auto& cbs = mat->GetShader()->GetConstantBuffers(); !cbs.empty()) {
        auto& cb = cbs[0].get();
        IntVector4 data{static_cast<int>(m_currentFrame), 0, 0, 0};
        cb.Update(*(r->GetDeviceContext()), &data);
    }
    r->SetMaterial(mat);
    r->SetTexture(m_texture);
    r->DrawQuad2D(transform);
}

IntVector2 Gif::GetDimensions() const noexcept {
    return IntVector2{m_texture->GetDimensions().x, m_texture->GetDimensions().y};
}

std::size_t Gif::GetCurrentFrame() const noexcept {
    return m_currentFrame;
}

std::size_t Gif::GetFrameCount() const noexcept {
    return m_totalFrames;
}

void Gif::SetImage(std::filesystem::path newFilepath) noexcept {
    SetFrameRange(0u, static_cast<std::size_t>(-1));
    Load(newFilepath);
}

bool Gif::IsAtEnd() const noexcept {
    switch(m_playMode) {
    case Gif::PlayMode::PingPong:
        if(m_direction >= 0) {
            return IsAtFrameEnd() && m_currentFrame == m_endFrame;
        } else if(m_direction < 0) {
            return IsAtFrameStart() && m_currentFrame == 0;
        } else {
            return false;
        }
        break;
    case Gif::PlayMode::PlayToEnd:
        [[fallthrough]];
    case Gif::PlayMode::Loop:
        return IsAtFrameEnd() && m_currentFrame == m_endFrame;
    case Gif::PlayMode::PlayToBeginning:
        [[fallthrough]];
    case Gif::PlayMode::Reverse:
        return IsAtFrameStart() && m_currentFrame == 0;
    default:
        return false;
    }
}

bool Gif::IsAtBeginning() const noexcept {
    switch(m_playMode) {
    case Gif::PlayMode::PingPong:
        if(m_direction >= 0) {
            return IsAtFrameStart() && m_currentFrame == 0;
        } else if(m_direction < 0) {
            return IsAtFrameEnd() && m_currentFrame == m_endFrame;
        } else {
            return false;
        }
        break;
    case Gif::PlayMode::PlayToEnd:
        [[fallthrough]];
    case Gif::PlayMode::Loop:
        return IsAtFrameStart() && m_currentFrame == 0;
    case Gif::PlayMode::PlayToBeginning:
        [[fallthrough]];
    case Gif::PlayMode::Reverse:
        return IsAtFrameEnd() && m_currentFrame == m_endFrame;
    default:
        return false;
    }
}

bool Gif::IsAtFrameStart() const noexcept {
    return m_frameDelays[m_currentFrame] == TimeUtils::FPMilliseconds::zero();
}

bool Gif::IsAtFrameEnd() const noexcept {
    const auto frame_delay = [this]() {
        try {
            return m_frameDelays.at(m_currentFrame);
        } catch(...) {
            return m_frameDelays[m_startFrame];
        }
    }();
    return m_frameDuration >= frame_delay;
}

bool Gif::IsAtFrameStart(std::size_t frameIdx) const noexcept {
    return m_currentFrame == frameIdx ? IsAtFrameStart() : false;
}

bool Gif::IsAtFrameEnd(std::size_t frameIdx) const noexcept {
    return m_currentFrame == frameIdx ? IsAtFrameEnd() : false;
}

void Gif::Restart() noexcept {
    switch(m_playMode) {
    case Gif::PlayMode::PingPong:
        if(m_direction >= 0) {
            m_currentFrame = 0;
        } else if(m_direction < 0) {
            m_currentFrame = m_endFrame;
        }
        break;
    case Gif::PlayMode::PlayToEnd:
        [[fallthrough]];
    case Gif::PlayMode::Loop:
        m_currentFrame = 0;
        break;
    case Gif::PlayMode::PlayToBeginning:
        [[fallthrough]];
    case Gif::PlayMode::Reverse:
        m_currentFrame = m_endFrame;
        break;
    default:
        m_currentFrame = 0;
        break;
    }
}

const Texture* const Gif::DebugGetTexture() const noexcept {
    return m_texture;
}

} // namespace FileUtils