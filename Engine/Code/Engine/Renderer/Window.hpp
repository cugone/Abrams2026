#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/RHI/RHITypes.hpp"

#include <functional>
#include <string>

struct WindowDesc {
    std::string title{"Created with Abrams 2026 (c) Casey Ugone"};
    IntVector2 position{};
    IntVector2 dimensions{1600, 900};
    RHIOutputMode mode{RHIOutputMode::Windowed};
};

class Window {
public:
    virtual ~Window() noexcept = default;

    static std::unique_ptr<Window> Create(const WindowDesc& desc = WindowDesc());

    virtual void Open() noexcept = 0;
    virtual void Close() noexcept = 0;

    virtual void Show() noexcept = 0;
    virtual void Hide() noexcept = 0;
    virtual void UnHide() noexcept = 0;
    [[nodiscard]] virtual bool IsOpen() const noexcept = 0;
    [[nodiscard]] virtual bool IsClosed() const noexcept = 0;
    [[nodiscard]] virtual bool IsWindowed() const noexcept = 0;
    [[nodiscard]] virtual bool IsFullscreen() const noexcept = 0;

    [[nodiscard]] virtual IntVector2 GetDimensions() const noexcept = 0;
    [[nodiscard]] virtual IntVector2 GetClientDimensions() const noexcept = 0;
    [[nodiscard]] virtual IntVector2 GetPosition() const noexcept = 0;

    [[nodiscard]] static IntVector2 GetDesktopResolution() noexcept;

    virtual void SetDimensionsAndPosition(const IntVector2& new_position, const IntVector2& new_size) noexcept = 0;
    virtual void SetPosition(const IntVector2& new_position) noexcept = 0;
    virtual void SetDimensions(const IntVector2& new_dimensions) noexcept = 0;
    virtual void SetForegroundWindow() noexcept = 0;
    virtual void SetFocus() noexcept = 0;

    [[nodiscard]] virtual void* GetWindowHandle() const noexcept = 0;
    virtual void SetWindowHandle(void* hWnd) noexcept = 0;

    [[nodiscard]] virtual void* GetWindowDeviceContext() const noexcept = 0;

    [[nodiscard]] virtual const RHIOutputMode& GetDisplayMode() const noexcept = 0;
    virtual void SetDisplayMode(const RHIOutputMode& display_mode) noexcept = 0;

    std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)> custom_message_handler;

    virtual void SetTitle(const std::string& title) noexcept = 0;
    [[nodiscard]] virtual const std::string& GetTitle() const noexcept = 0;

protected:
private:
};
