#include "Engine/Renderer/Flipbook.hpp"

#include "Engine/Core/DataUtils.hpp"

#include "Engine/Renderer/TextureArray2D.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IRendererService.hpp"

Flipbook::Flipbook(std::filesystem::path folderpath, unsigned int framesPerSecond /*= 3*/) noexcept
: m_texture{nullptr}
, m_frameRate{framesPerSecond}
{
    auto* r = ServiceLocator::get<IRendererService>();
    auto t = r->Create2DTextureArrayFromFolder(folderpath);
    GUARANTEE_OR_DIE(r->RegisterTexture(folderpath.string(), std::move(t)), "Failed to register flipbook texture.");

    m_texture = r->GetTexture(folderpath.string());
    m_frameDimensions = IntVector2{m_texture->GetDimensions()};
}

Flipbook::Flipbook(Texture* const texture, unsigned int framesPerSecond /*= 3*/) noexcept
: m_texture{texture}
, m_frameRate{framesPerSecond} {

}

void Flipbook::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(CheckPlaymode(Playmode::Paused)) {
        return;
    }
    if(m_frameRate.Check()) {
        if(CheckPlaymode(Playmode::Forward)) {
            AdvanceFrame(1);
            if(m_currentFrame == m_texture->GetDimensions().z) {
                SetFrame(0);
            }
        } else if(CheckPlaymode(Playmode::Backward)) {
            AdvanceFrame(-1);
            if(m_currentFrame < 0) {
                SetFrame(m_texture->GetDimensions().z - 1);
            }
        } else if(CheckPlaymode(Playmode::StepForward)) {
            StepForward();
            if(m_currentFrame == m_texture->GetDimensions().z) {
                SetFrame(m_texture->GetDimensions().z - 1);
            }
            m_playmode = Playmode::Paused;
        } else if(CheckPlaymode(Playmode::StepBackward)) {
            StepBackward();
            if(m_currentFrame < 0) {
                SetFrame(0);
            }
            m_playmode = Playmode::Paused;
        }
    }
}

void Flipbook::Render(const Matrix4& transform /*= Matrix4::I*/) const noexcept {
    auto* r = ServiceLocator::get<IRendererService>();
    auto* mat = r->GetMaterial("__unlit2DSprite");
    if(const auto& cbs = mat->GetShader()->GetConstantBuffers(); !cbs.empty()) {
        auto& cb = cbs[0].get();
        IntVector4 data{m_currentFrame, 0, 0, 0};
        cb.Update(*(r->GetDeviceContext()), &data);
    }
    r->SetMaterial(mat);
    r->SetTexture(m_texture);
    r->DrawQuad2D(transform);
}
 
Vector2 Flipbook::GetDimensions() const noexcept {
    return Vector2{m_frameDimensions};
}

void Flipbook::SetFrameRate(unsigned int framesPerSecond) noexcept {
    m_frameRate.SetFrequency(framesPerSecond);
}

void Flipbook::SetDuration(TimeUtils::FPSeconds duration) noexcept {
    m_frameRate.SetSeconds(duration);
}

void Flipbook::SetFrame(int frameIndex) noexcept {
    m_currentFrame = std::clamp(frameIndex, -1, m_texture->GetDimensions().z);
    m_frameRate.Reset();
}

void Flipbook::AdvanceFrame(int frames_to_advance) noexcept {
    SetFrame(m_currentFrame + frames_to_advance);
}

void Flipbook::TogglePause() noexcept {
    if(m_playmode == Playmode::Paused) {
        ClearPlaymode(Playmode::Paused);
    } else {
        SetPlaymode(Playmode::Paused);
    }
}

void Flipbook::StepForward() noexcept {
    SetFrame(m_currentFrame + 1);
    m_playmode = Playmode::Paused;
}

void Flipbook::StepBackward() noexcept {
    SetFrame(m_currentFrame - 1);
    m_playmode = Playmode::Paused;
}

void Flipbook::SetPlaymode(const Playmode& mode) noexcept {
    switch(mode) {
    case Playmode::Forward:
        m_state = 0b1000;
        break;
    case Playmode::Backward:
        m_state = 0b0100;
        break;
    case Playmode::StepForward:
        m_state = 0b1010;
        break;
    case Playmode::StepBackward:
        m_state = 0b0110;
        break;
    case Playmode::Paused:
        m_state |= 0b0001;
        break;
    default:
        break;
    }
}

void Flipbook::ClearPlaymode(const Playmode& mode) noexcept {
    switch(mode) {
    case Playmode::Forward:
        m_state &= ~(0b1000);
        break;
    case Playmode::Backward:
        m_state &= ~(0b0100);
        break;
    case Playmode::StepForward:
        m_state &= ~(0b1010);
        break;
    case Playmode::StepBackward:
        m_state &= ~(0b0110);
        break;
    case Playmode::Paused:
        m_state &= ~(0b0001);
        break;
    default:
        break;

    }
}

bool Flipbook::CheckPlaymode(const Playmode& mode) const noexcept {
    switch(mode) {
    case Playmode::Forward:
        return (m_state & 0b1000) > uint8_t{0u};
    case Playmode::Backward:
        return (m_state & 0b0100) > uint8_t{0u};
    case Playmode::StepForward:
        return (m_state & 0b1010) > uint8_t{0u};
    case Playmode::StepBackward:
        return (m_state & 0b0110) > uint8_t{0u};
    case Playmode::Paused:
        return (m_state & 0b0001) > uint8_t{0u};
    default:
        return false;
    }
}
