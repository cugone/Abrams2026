#include "Engine/UI/DearImgui.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IRendererService.hpp"

#include <string_view>

#ifdef PROFILE_BUILD
#include <Thirdparty/Tracy/tracy/Tracy.hpp>
#endif

#ifndef UI_DEBUG
    #define IMGUI_DISABLE_DEMO_WINDOWS
    #define IMGUI_DISABLE_DEBUG_TOOLS
#else
    #undef IMGUI_DISABLE_DEMO_WINDOWS
    #undef IMGUI_DISABLE_DEBUG_TOOLS
#endif

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DearImgui::DearImgui() noexcept
: m_imguiContext(ImGui::CreateContext())
{
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
#ifdef UI_DEBUG
    IMGUI_CHECKVERSION();
#endif
}

DearImgui::~DearImgui() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext(m_imguiContext);
    m_imguiContext = nullptr;
}

void DearImgui::Initialize() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    auto* renderer = ServiceLocator::get<IRendererService>();
    const auto dims = Vector2{renderer->GetOutput()->GetDimensions()};
    auto& io = ImGui::GetIO();

    io.DisplaySize.x = dims.x;
    io.DisplaySize.y = dims.y;

    ImGui::StyleColorsDark();

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    if(std::filesystem::exists(m_ini_filepath)) {
        ImGui::LoadIniSettingsFromDisk(m_ini_filepath.string().c_str());
    } else {
        constexpr const std::string_view default_ini{
R"(
[Window][DockSpaceViewport_11111111]
Pos=0,19
Size=1600,881
Collapsed=0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][World Inspector]
Pos=1334,19
Size=266,662
Collapsed=0
DockId=0x00000004,1

[Window][Settings]
Pos=0,19
Size=368,662
Collapsed=0
DockId=0x00000005,0

[Window][Properties]
Pos=1334,19
Size=266,662
Collapsed=0
DockId=0x00000004,0

[Window][Content Browser]
Pos=0,683
Size=1600,217
Collapsed=0
DockId=0x00000002,0

[Window][Viewport]
Pos=370,19
Size=962,662
Collapsed=0
DockId=0x00000006,0

[Window][WindowOverViewport_11111111]
Pos=0,19
Size=1600,881
Collapsed=0

[Docking][Data]
DockSpace       ID=0x08BD597D Window=0x1BBC0F80 Pos=16,81 Size=1600,881 Split=Y Selected=0xC450F867
  DockNode      ID=0x00000001 Parent=0x08BD597D SizeRef=1600,662 Split=X Selected=0xC450F867
    DockNode    ID=0x00000003 Parent=0x00000001 SizeRef=1332,662 Split=X Selected=0xC450F867
      DockNode  ID=0x00000005 Parent=0x00000003 SizeRef=368,662 Selected=0x4746B4B8
      DockNode  ID=0x00000006 Parent=0x00000003 SizeRef=962,662 CentralNode=1 Selected=0xC450F867
    DockNode    ID=0x00000004 Parent=0x00000001 SizeRef=266,662 Selected=0x8C72BEA8
  DockNode      ID=0x00000002 Parent=0x08BD597D SizeRef=1600,217 Selected=0x3DF3100E
)"
};
        ImGui::LoadIniSettingsFromMemory(default_ini.data(), default_ini.size());
    }

    m_ini_saveTimer.SetSeconds(TimeUtils::FPSeconds{io.IniSavingRate});

    io.ConfigWindowsResizeFromEdges = true;
    io.ConfigDockingWithShift = true;
    io.ConfigNavMoveSetMousePos = true;
    #if 0
    io.ConfigDebugHighlightIdConflicts = true;
    io.ConfigDebugHighlightIdConflictsShowItemPicker = true;
    io.ConfigDebugBeginReturnValueOnce = true;
    io.ConfigDebugBeginReturnValueLoop = true;
    io.ConfigDebugIniSettings = true;
    #endif
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;

    auto* hwnd = renderer->GetOutput()->GetWindow()->GetWindowHandle();
    ImGui_ImplWin32_Init(hwnd);

    auto* dx_device = renderer->GetDevice()->GetDxDevice();
    auto* dx_context = renderer->GetDeviceContext()->GetDxContext();
    ImGui_ImplDX11_Init(dx_device, dx_context);
}

void DearImgui::BeginFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(m_ini_saveTimer.CheckAndReset()) {
        ImGui::SaveIniSettingsToDisk(m_ini_filepath.string().c_str());
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void DearImgui::Update() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
    if(m_show_imgui_demo_window) {
        ImGui::ShowDemoWindow(&m_show_imgui_demo_window);
    }
    if(m_show_imgui_metrics_window) {
        ImGui::ShowMetricsWindow(&m_show_imgui_metrics_window);
    }
#endif
}

void DearImgui::Render() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DearImgui::EndFrame() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    ImGui::EndFrame();
    ImGui::UpdatePlatformWindows();
}

bool DearImgui::HasFocus() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    return WantsInputCapture();
}

bool DearImgui::WantsInputCapture() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    return WantsInputKeyboardCapture() || WantsInputMouseCapture();
}

bool DearImgui::WantsInputKeyboardCapture() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool DearImgui::WantsInputMouseCapture() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    return ImGui::GetIO().WantCaptureMouse;
}

bool DearImgui::IsImguiDemoWindowVisible() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
    return m_show_imgui_demo_window;
#else
    return false;
#endif
}

void DearImgui::ToggleImguiDemoWindow() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
    m_show_imgui_demo_window = !m_show_imgui_demo_window;
    auto* input = ServiceLocator::get<IInputService>();
    if(!input->IsMouseCursorVisible()) {
        input->ShowMouseCursor();
    }
#endif
}

bool DearImgui::IsImguiMetricsWindowVisible() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
#if !defined(IMGUI_DISABLE_DEBUG_TOOLS)
    return m_show_imgui_metrics_window;
#else
    return false;
#endif
}

void DearImgui::ToggleImguiMetricsWindow() noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
#if !defined(IMGUI_DISABLE_DEBUG_TOOLS)
    m_show_imgui_metrics_window = !m_show_imgui_metrics_window;
    auto* input = ServiceLocator::get<IInputService>();
    if(!input->IsMouseCursorVisible()) {
        input->ShowMouseCursor();
    }
#endif
}

bool DearImgui::IsAnyImguiDebugWindowVisible() const noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
#ifdef UI_DEBUG
    return IsImguiDemoWindowVisible() || IsImguiMetricsWindowVisible();
#else
    return false;
#endif
}

bool DearImgui::ProcessSystemMessage(const EngineMessage& msg) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    return ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(msg.hWnd), msg.nativeMessage, msg.wparam, msg.lparam);
}

namespace ImGui {
void Image(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(texture) {
        ImGui::Image((const ImTextureID)(const intptr_t)texture->GetShaderResourceView(), size, uv0, uv1);
    }
}
void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(texture) {
        ImGui::Image((ImTextureID)(intptr_t)texture->GetShaderResourceView(), size, uv0, uv1);
    }
}

void Image(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept {
    ImageWithBg(texture, size, uv0, uv1, tint_col, border_col);
}
void Image(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& border_col) noexcept {
    ImageWithBg(texture, size, uv0, uv1, tint_col, border_col);
}

void ImageWithBg(const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& bg_col) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(texture) {
        const auto&& [tr, tg, tb, ta] = tint_col.GetAsFloats();
        const auto&& [br, bg, bb, ba] = bg_col.GetAsFloats();
        ImGui::ImageWithBg((const ImTextureID)(const intptr_t)texture->GetShaderResourceView(), size, uv0, uv1, Vector4{br, bg, bb, ba}, Vector4{tr, tg, tb, ta});
    }
}
void ImageWithBg(Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& tint_col, const Rgba& bg_col) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(texture) {
        const auto&& [tr, tg, tb, ta] = tint_col.GetAsFloats();
        const auto&& [br, bg, bb, ba] = bg_col.GetAsFloats();
        ImGui::ImageWithBg((ImTextureID)(intptr_t)texture->GetShaderResourceView(), size, uv0, uv1, Vector4{br, bg, bb, ba}, Vector4{tr, tg, tb, ta});
    }
}

bool ImageButton(const std::string& id, const Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& bg_col, const Rgba& tint_col) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(texture) {
        const auto&& [tr, tg, tb, ta] = tint_col.GetAsFloats();
        const auto&& [br, bg, bb, ba] = bg_col.GetAsFloats();
        return ImGui::ImageButton(id.c_str(), (ImTextureID)(intptr_t)texture->GetShaderResourceView(), size, uv0, uv1, Vector4{br, bg, bb, ba}, Vector4{tr, tg, tb, ta});
    }
    return false;
}
bool ImageButton(const std::string& id, Texture* texture, const Vector2& size, const Vector2& uv0, const Vector2& uv1, const Rgba& bg_col, const Rgba& tint_col) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    if(texture) {
        const auto&& [tr, tg, tb, ta] = tint_col.GetAsFloats();
        const auto&& [br, bg, bb, ba] = bg_col.GetAsFloats();
        return ImGui::ImageButton(id.c_str(), (ImTextureID)(intptr_t)texture->GetShaderResourceView(), size, uv0, uv1, Vector4{br, bg, bb, ba}, Vector4{tr, tg, tb, ta});
    }
    return false;
}

bool ColorEdit3(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    const auto&& [r, g, b, _] = color.GetAsFloats();
    Vector4 colorAsFloats{r, g, b, 1.0f};
    if(ImGui::ColorEdit3(label, colorAsFloats.GetAsFloatArray(), flags)) {
        color.SetFromFloats({colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, 1.0f});
        return true;
    }
    return false;
}
bool ColorEdit4(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    const auto&& [r, g, b, a] = color.GetAsFloats();
    Vector4 colorAsFloats{r, g, b, a};
    if(ImGui::ColorEdit4(label, colorAsFloats.GetAsFloatArray(), flags)) {
        color.SetFromFloats({colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w});
        return true;
    }
    return false;
}
bool ColorPicker3(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    const auto&& [r, g, b, _] = color.GetAsFloats();
    Vector4 colorAsFloats{r, g, b, 1.0f};
    if(ImGui::ColorPicker3(label, colorAsFloats.GetAsFloatArray(), flags)) {
        color.SetFromFloats({colorAsFloats.x, colorAsFloats.y, colorAsFloats.z});
        return true;
    }
    return false;
}
bool ColorPicker4(const char* label, Rgba& color, ImGuiColorEditFlags flags /*= 0*/, Rgba* refColor /*= nullptr*/) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    Vector4 refColorAsFloats{};
    if(refColor) {
        const auto&& [rr, rg, rb, ra] = refColor->GetAsFloats();
        refColorAsFloats = Vector4{rr, rg, rb, ra};
    }
    const auto&& [r, g, b, a] = color.GetAsFloats();
    Vector4 colorAsFloats{r, g, b, a};
    if(ImGui::ColorPicker4(label, colorAsFloats.GetAsFloatArray(), flags, refColor ? refColorAsFloats.GetAsFloatArray() : nullptr)) {
        color.SetFromFloats({colorAsFloats.x, colorAsFloats.y, colorAsFloats.z, colorAsFloats.w});
        if(refColor) {
            refColor->SetFromFloats({refColorAsFloats.x, refColorAsFloats.y, refColorAsFloats.z, refColorAsFloats.w});
        }
        return true;
    }
    return false;
}
bool ColorButton(const char* desc_id, const Rgba& color, ImGuiColorEditFlags flags /*= 0*/, Vector2 size /*= Vector2::ZERO*/) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    const auto&& [r, g, b, a] = color.GetAsFloats();
    return ImGui::ColorButton(desc_id, Vector4{r, g, b, a}, flags, size);
}

void TextColored(const Rgba& color, const char* fmt, ...) noexcept {
#ifdef PROFILE_BUILD
    ZoneScopedC(0xFF0000);
#endif
    auto&& [r, g, b, a] = color.GetAsFloats();
    va_list args;
    va_start(args, fmt);
    ImGui::TextColoredV(Vector4{r, g, b, a}, fmt, args);
    va_end(args);
}

} // namespace ImGui
