#include "Editor/Editor.hpp"

#include "Engine/Core/Image.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Platform/DirectX/DirectX11FrameBuffer.hpp"
#include "Engine/Platform/PlatformUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/RHI/RHIOutput.hpp"
#include "Engine/RHI/RHIDeviceContext.hpp"


#include "Engine/Scene/Scene.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IRendererService.hpp"
#include "Engine/Services/IInputService.hpp"

#include "Engine/Core/ThreadUtils.hpp"
#include "Engine/Math/Ray3.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/UISystem.hpp"

#include <algorithm>
#include <chrono>
#include <limits>
#include <numeric>
#include <sstream>
#include <thread>

void Editor::ShowSelectedEntityComponents([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    /* DO NOTHING */
}

void Editor::HandleInput(TimeUtils::FPSeconds deltaSeconds) noexcept {
    HandleMenuKeyboardInput(deltaSeconds);
    HandleCameraInput(deltaSeconds);
}

bool Editor::IsSceneLoaded() const noexcept {
    return m_ActiveScene.get() != nullptr;
}

Editor::~Editor() noexcept {
    buffer.reset();
}

void Editor::Initialize() noexcept {
    MathUtils::SetRandomEngineSeed(0u);
    auto* renderer = ServiceLocator::get<IRendererService>();
    m_ContentBrowser.currentDirectory = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData);
    renderer->RegisterTexturesFromFolder(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{"Images"}, true);
    renderer->RegisterTexturesFromFolder(m_ContentBrowser.currentDirectory / std::filesystem::path{"Resources/Icons"}, true);
    m_ContentBrowser.currentDirectory = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::EditorContent);
    m_ContentBrowser.UpdateContentBrowserPaths();
    buffer = FrameBuffer::Create(FrameBufferDesc{});
}

void Editor::BeginFrame() noexcept {
    ImGui::DockSpaceOverViewport();
}

void Editor::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* renderer = ServiceLocator::get<IRendererService>();
    renderer->UpdateGameTime(deltaSeconds);
    ShowUI(deltaSeconds);
    HandleInput(deltaSeconds);
}

void Editor::Render() const noexcept {

    auto* renderer = ServiceLocator::get<IRendererService>();
    renderer->BeginRender(buffer->GetTexture(), Rgba::Black, buffer->GetDepthStencil());

    renderer->SetOrthoProjectionFromViewWidth(static_cast<float>(m_ViewportWidth), m_editorCamera.GetAspectRatio(), 0.01f, 1.0f);
    renderer->SetCamera(m_editorCamera.GetCamera());

    renderer->BeginRenderToBackbuffer();

}

void Editor::EndFrame() noexcept {
    /* DO NOTHING */
}

void Editor::HandleWindowResize(unsigned int newWidth, unsigned int newHeight) noexcept {
    buffer->Resize(newWidth, newHeight);
}

const GameSettings* Editor::GetSettings() const noexcept {
    return GameBase::GetSettings();
}

GameSettings* Editor::GetSettings() noexcept {
    return GameBase::GetSettings();
}

void Editor::DoFileNew() noexcept {
    /* DO NOTHING */
}

void Editor::DoFileOpen() noexcept {
    if(auto path = FileDialogs::OpenFile("Abrams Scene (*.ascene)\0*.ascene\0All Files (*.*)\0*.*\0\0"); !path.empty()) {
        
    }
}

void Editor::DoFileSaveAs() noexcept {
    if(auto path = FileDialogs::SaveFile("Abrams Scene (*.ascene)\0*.ascene\0All Files (*.*)\0*.*\0\0"); !path.empty()) {
    }
}

void Editor::DoFileSave() noexcept {
    /* DO NOTHING */
}

void Editor::ShowUI(TimeUtils::FPSeconds deltaSeconds) noexcept {
    ShowMainMenu(deltaSeconds);
    ShowWorldInspectorWindow(deltaSeconds);
    ShowSettingsWindow(deltaSeconds);
    ShowPropertiesWindow(deltaSeconds);
    ShowContentBrowserWindow(deltaSeconds);
    ShowMainViewport(deltaSeconds);
}

void Editor::ShowMainMenu([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::BeginMainMenuBar();
    {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("New", "Ctrl+N")) {
                DoFileNew();
            }
            if(ImGui::MenuItem("Open...", "Ctrl+O")) {
                DoFileOpen();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Minimize")) {
                auto* app = ServiceLocator::get<IAppService>();
                app->Minimize();
            }
            if(ImGui::MenuItem("Maximize")) {
                auto* app = ServiceLocator::get<IAppService>();
                app->Maximize();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Save", "Ctrl+S", nullptr, m_ActiveScene.get())) {
                DoFileSave();
            }
            if(ImGui::MenuItem("Save As...", "", nullptr, m_ActiveScene.get())) {
                DoFileSaveAs();
            }
            if(ImGui::MenuItem("Exit")) {
                auto* app = ServiceLocator::get<IAppService>();
                app->SetIsQuitting(true);
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Debug")) {
            if(ImGui::MenuItem("Dear ImGui Demo Window", nullptr, g_theUISystem->IsImguiDemoWindowVisible(), IsDebuggerAvailable())) {
                g_theUISystem->ToggleImguiDemoWindow();
            }
            if(ImGui::MenuItem("Dear ImGui Metrics Window", nullptr, g_theUISystem->IsImguiMetricsWindowVisible(), IsDebuggerAvailable())) {
                g_theUISystem->ToggleImguiMetricsWindow();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Camera")) {
            if(ImGui::BeginMenu("Camera Speed")) {
                static int cameraTranslationMultiplier = 1;
                static int cameraSpeedMultiplier = 1;
                if(ImGui::SliderInt("Speed##CameraSpeedSlider", &cameraTranslationMultiplier, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoInput)) {
                    m_editorCamera.SetTranslationMultiplier(cameraTranslationMultiplier);
                }
                if(ImGui::InputInt("Multiplier##CameraSpeedMultipler", &cameraSpeedMultiplier, 1, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_NoUndoRedo)) {
                    cameraSpeedMultiplier = (std::max)(1, cameraSpeedMultiplier);
                    if(!ImGui::IsItemDeactivatedAfterEdit()) {
                        cameraSpeedMultiplier = (std::max)(1, cameraSpeedMultiplier);
                    }
                    m_editorCamera.SetSpeedMultiplier(cameraSpeedMultiplier);
                }
                ImGui::SameLine();
                ImGui::TextDisabled("(?)");
                if(ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Multiply speed by this value.");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        //TODO: Implement custom minimize, maximize, and close buttons
        //ShowMinMaxCloseButtons();
        ImGui::EndMainMenuBar();
    }
}

void Editor::ShowMinMaxCloseButtons() noexcept {
    static float nc_button_offset = 40.0f;
    static float close_button_offset = ImGui::GetWindowWidth() - nc_button_offset;
    static float maximize_button_offset = close_button_offset - nc_button_offset;
    static float minimize_button_offset = maximize_button_offset - nc_button_offset;
    static float nc_button_sizes = 32.0f;
    ImGui::SameLine(close_button_offset);
    ImGui::PushStyleColor(ImGuiCol_Button, Rgba::NoAlpha.GetAsRawValue());
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Rgba::NoAlpha.GetAsRawValue());
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Rgba::Red.GetAsRawValue());

    if(ImGui::ImageButton(GetAssetTextureFromPath(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / "Resources/Icons/CloseButtonAsset.png"), Vector2{nc_button_sizes, nc_button_sizes}, Vector2::Zero, Vector2::One, 0, Rgba::NoAlpha, Rgba::NoAlpha)) {
        auto* app = ServiceLocator::get<IAppService>();
        app->SetIsQuitting(true);
    }
    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Rgba::LightGray.GetAsRawValue());
    ImGui::SameLine(maximize_button_offset);
    const std::filesystem::path max_or_restore_down_button_path = []() {
        auto* renderer = ServiceLocator::get<IRendererService>();
        if(const auto is_fullscreen = renderer->GetOutput()->GetWindow()->IsFullscreen(); !is_fullscreen) {
            return FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / "Resources/Icons/MaximizeButtonAsset.png";
        }
        return FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / "Resources/Icons/RestoreDownButtonAsset.png";
    }(); //IIIL
    if(ImGui::ImageButton(GetAssetTextureFromPath(max_or_restore_down_button_path), Vector2{nc_button_sizes, nc_button_sizes}, Vector2::Zero, Vector2::One, 0, Rgba::NoAlpha, Rgba::NoAlpha)) {
        auto* renderer = ServiceLocator::get<IRendererService>();
        auto* app = ServiceLocator::get<IAppService>();
        !renderer->GetOutput()->GetWindow()->IsFullscreen() ? app->Maximize() : app->Restore(0,0);
    }
    ImGui::SameLine(minimize_button_offset);
    if(ImGui::ImageButton(GetAssetTextureFromPath(FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / "Resources/Icons/MinimizeButtonAsset.png"), Vector2{nc_button_sizes, nc_button_sizes}, Vector2::Zero, Vector2::One, 0, Rgba::NoAlpha, Rgba::NoAlpha)) {
        auto* app = ServiceLocator::get<IAppService>();
        app->SetIsQuitting(true);
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void Editor::ShowWorldInspectorWindow([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::Begin("World Inspector");
    {
        
    }
    ImGui::End();
}

void Editor::ShowSettingsWindow(TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::Begin("Settings");
    {
        ShowSelectedEntityComponents(deltaSeconds);
    }
    ImGui::End();
}

void Editor::ShowPropertiesWindow([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    ImGui::Begin("Properties");
    {

    }
    ImGui::End();
}

void Editor::ShowMainViewport([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    {
        std::ostringstream ss{};
        if(IsSceneLoaded()) {
            //TODO: Scenes should store their names.
            ss << m_ActiveScene;
        } else {
            ss << "Viewport";
        }
        ImGui::Begin(ss.str().c_str(), nullptr);
    }
    const auto viewportSize = ImGui::GetContentRegionAvail();
    if(viewportSize.x != m_ViewportWidth || viewportSize.y != m_ViewportHeight) {
        m_ViewportWidth = static_cast<uint32_t>(std::floor(viewportSize.x));
        m_ViewportHeight = static_cast<uint32_t>(std::floor(viewportSize.y));
        buffer->Resize(m_ViewportWidth, m_ViewportHeight);
    }
    ImGui::Image(buffer->GetTexture(), viewportSize, Vector2::Zero, Vector2::One, Rgba::White, Rgba::NoAlpha);
    m_IsViewportWindowActive = ImGui::IsWindowFocused() || ImGui::IsWindowHovered();
    ImGui::End();
}

void Editor::ShowContentBrowserWindow(TimeUtils::FPSeconds deltaSeconds) noexcept {
    m_ContentBrowser.Update(deltaSeconds);
}

void Editor::HandleMenuKeyboardInput([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto* input = ServiceLocator::get<IInputService>();
    if(input->IsKeyDown(KeyCode::Ctrl)) {
        if(input->WasKeyJustPressed(KeyCode::N)) {
            DoFileNew();
        } else if(input->WasKeyJustPressed(KeyCode::O)) {
            DoFileOpen();
        } else if(input->WasKeyJustPressed(KeyCode::S)) {
            DoFileSaveAs();
        }
    }
}

void Editor::HandleCameraInput(TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(m_IsViewportWindowActive) {
        m_editorCamera.Update(deltaSeconds);
    }
}

bool Editor::HasAssetExtension(const std::filesystem::path& path) const noexcept {
    return std::filesystem::is_directory(path) || path.has_extension() && IsAssetExtension(path.extension());
}

Texture* Editor::GetAssetTextureFromPath(const std::filesystem::path& path) const noexcept {
    auto* renderer = ServiceLocator::get<IRendererService>();
    auto* defaultTexture = renderer->GetTexture("__white");
    if(HasAssetExtension(path)) {
        const auto e = path.extension();
        std::filesystem::path p{};
        const auto BuildPath = [&](const char* pathSuffix) -> std::filesystem::path {
            auto p = FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::GameData) / std::filesystem::path{pathSuffix};
            p = std::filesystem::canonical(p); //Intentionally throw uncaught exception if path does not exist.
            p = p.make_preferred();
            return p;
        };
        if(std::filesystem::is_directory(path)) {
            p = BuildPath("Resources/Icons/FolderAsset.png");
        } else if(e == ".txt") {
            p = BuildPath("Resources/Icons/TextAsset.png");
        } else if(e == ".ascene") {
            p = BuildPath("Resources/Icons/SceneAsset.png");
        } else if(e == ".log") {
            p = BuildPath("Resources/Icons/LogAsset.png");
        } else if(IsImageAssetExtension(e)) {
            return renderer->CreateOrGetTexture(path, IntVector3::XY_Axis);
        }
        return p.empty() ? defaultTexture : renderer->GetTexture(p.string());
    }
    return defaultTexture;
}

Editor::AssetType Editor::GetAssetType(const std::filesystem::path& path) const noexcept {
    const auto e = path.extension();
    if(IsAssetExtension(e)) {
        if(e == ".txt") {
            return Editor::AssetType::Text;
        }
        if(e == ".log") {
            return Editor::AssetType::Log;
        }
        if(e == ".ascene") {
            return Editor::AssetType::Scene;
        }
        if(IsImageAssetExtension(e)) {
            return Editor::AssetType::Texture;
        }
    }
    if(std::filesystem::is_directory(path)) {
        return Editor::AssetType::Folder;
    }
    return Editor::AssetType::None;
}

bool Editor::IsAssetExtension(const std::filesystem::path& ext) const noexcept {
    if(ext == ".txt") {
        return true;
    } else if(ext == ".ascene") {
        return true;
    } else if(ext == ".log") {
        return true;
    } else {
        return IsImageAssetExtension(ext);
    }
}

bool Editor::IsImageAssetExtension(const std::filesystem::path& ext) const noexcept {
    return Image::IsSupportedExtension(ext);
}
