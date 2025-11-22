#pragma once

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <Thirdparty/Imgui/imgui.h>
#include <Thirdparty/Imgui/imgui_impl_dx11.h>
#include <Thirdparty/Imgui/imgui_impl_win32.h>
#include <Thirdparty/Imgui/imgui_stdlib.h>

#include <filesystem>


class DearImgui {
public:
    DearImgui() noexcept;
    DearImgui(const DearImgui& other) = default;
    DearImgui(DearImgui&& other) = default;
    DearImgui& operator=(const DearImgui& other) = default;
    DearImgui& operator=(DearImgui&& other) = default;
    ~DearImgui() noexcept;

    void Initialize() noexcept;
    void BeginFrame() noexcept;
    void Update() noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

    [[nodiscard]] bool HasFocus() const noexcept;
    [[nodiscard]] bool WantsInputCapture() const noexcept;
    [[nodiscard]] bool WantsInputKeyboardCapture() const noexcept;
    [[nodiscard]] bool WantsInputMouseCapture() const noexcept;

    [[nodiscard]] bool IsImguiDemoWindowVisible() const noexcept;
    void ToggleImguiDemoWindow() noexcept;
    [[nodiscard]] bool IsImguiMetricsWindowVisible() const noexcept;
    void ToggleImguiMetricsWindow() noexcept;
    [[nodiscard]] bool IsAnyImguiDebugWindowVisible() const noexcept;

    bool ProcessSystemMessage(const EngineMessage& msg) noexcept;
protected:
private:
    std::filesystem::path m_ini_filepath{FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::EngineConfig) / "ui.ini"};
    Stopwatch m_ini_saveTimer{};
    ImGuiContext* m_imguiContext{};
    bool m_show_imgui_demo_window = false;
    bool m_show_imgui_metrics_window = false;
    bool m_save_settings_to_disk = false;
};

class Texture;
class Rgba;
class Vector2;
class Vector4;
//Custom ImGui overloads
namespace ImGui {
void Image(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1) noexcept;
void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1) noexcept;
[[deprecated("Use ImageWithBg or remove tint and border")]] void Image(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept;
[[deprecated("Use ImageWithBg or remove tint and border")]] void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept;
void ImageWithBg(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept;
void ImageWithBg(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept;
[[nodiscard]] bool ImageButton(const std::string& id, const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& bg_col, const Rgba& tint_col) noexcept;
[[nodiscard]] bool ImageButton(const std::string& id, Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& bg_col, const Rgba& tint_col) noexcept;

[[nodiscard]] bool ColorEdit3(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0) noexcept;
[[nodiscard]] bool ColorEdit4(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0) noexcept;
[[nodiscard]] bool ColorPicker3(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0) noexcept;
[[nodiscard]] bool ColorPicker4(const char* label, Rgba& color, ImGuiColorEditFlags flags = 0, Rgba* refColor = nullptr) noexcept;
[[nodiscard]] bool ColorButton(const char* desc_id, Rgba& color, ImGuiColorEditFlags flags = 0, Vector2 size = Vector2::Zero) noexcept;
void TextColored(const Rgba& color, const char* fmt, ...) noexcept;
} // namespace ImGui
