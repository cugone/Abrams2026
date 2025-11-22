#include "Editor/ContentBrowserPanel.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Platform/PlatformUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IConfigService.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Editor/Editor.hpp"

#include <format>
#include <string>

void ContentBrowserPanel::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(m_CacheNeedsImmediateUpdate) {
        UpdateContentBrowserPaths();
    } else {
        PollContentBrowserPaths();
    }
    ImGui::Begin("Content Browser");
    {
        ShowContextMenuOnEmptySpace();

        //ImGui::BeginPopupContextItem("##ImportAsset");
        //ImGui::BeginPopupContextVoid();
        if(currentDirectory != FileUtils::GetKnownFolderPath(FileUtils::KnownPathID::EditorContent)) {
            if(ImGui::ArrowButton("Back##LEFT", ImGuiDir_Left)) {
                currentDirectory = currentDirectory.parent_path();
                m_CacheNeedsImmediateUpdate = true;
            }
        }
        const auto padding = 16.0f;
        const auto* const config = ServiceLocator::get<IConfigService>();
        static auto scale = 0.5f;
        if(config->HasKey("UIScale")) {
            config->GetValue("UIScale", scale);
        }
        scale = std::clamp(scale, 0.125f, 2.0f);
        const auto thumbnailSize = (std::max)(32.0f, 256.0f * scale);
        const auto cellSize = thumbnailSize + padding;
        const auto panelWidth = static_cast<uint32_t>(std::floor(ImGui::GetContentRegionAvail().x));
        if(panelWidth != m_PanelWidth) {
            m_PanelWidth = panelWidth;
        }
        const auto columnCount = (std::min)((std::max)(1, static_cast<int>(m_PanelWidth / cellSize)), 64);
        ImGui::BeginTable("##ContentBrowser", columnCount, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ContextMenuInBody);
        {
            ContentBrowserItemStats stats{};
            for(auto& p : m_PathsCache) {
                ImGui::TableNextColumn();
                const auto* editor = GetGameAs<Editor>();
                const auto icon = editor->GetAssetTextureFromPath(p);
                ImGui::BeginGroup();
                ImGui::PushStyleColor(ImGuiCol_Button, std::make_from_tuple<Vector4>(Rgba::DarkGray.GetAsFloats()));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, std::make_from_tuple<Vector4>(Rgba::LightGray.GetAsFloats()));
                const std::string str_id = p.string();
                stats.id = str_id;
                stats.path = p;
                stats.filesize = std::filesystem::file_size(p);
                //stats.asset_type = editor->GetAssetType(p);
                stats.asset_type = Editor::GetAssetTypeName(editor->GetAssetType(p));
                if(std::filesystem::is_directory(p)) {
                    if(ImGui::ImageButton(str_id, icon, Vector2{thumbnailSize, thumbnailSize}, Vector2::Zero, Vector2::One, Rgba::NoAlpha, Rgba::White)) {
                        currentDirectory /= p.filename();
                        m_CacheNeedsImmediateUpdate = true;
                    }
                } else {
                    if(ImGui::ImageButton(str_id, icon, Vector2{thumbnailSize, thumbnailSize}, Vector2::Zero, Vector2::One, Rgba::NoAlpha, Rgba::White)) {
                        /* DO NOTHING */
                    }
                }
                const auto filename_string = p.filename().string();
                const auto filename_size = ImGui::CalcTextSize(filename_string.c_str(), nullptr);
                ImGui::TextWrapped(filename_string.c_str());
                ImGui::PopStyleColor(2);
                ImGui::EndGroup();
                if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ShowHoveredItemStats(stats);
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void ContentBrowserPanel::ShowHoveredItemStats(const ContentBrowserItemStats& stats) noexcept {
    ImGui::BeginTooltip();
    {
        ImGui::TextColored(Rgba::LightGray, "Type: %s", stats.asset_type.c_str());
        ImGui::TextColored(Rgba::LightGray, "ID: %s", stats.id.c_str());
        ImGui::TextColored(Rgba::LightGray, "Path: %s", stats.path.string().c_str());
        ImGui::TextColored(Rgba::LightGray, "Size: %u bytes", stats.filesize);
    }
    ImGui::EndTooltip();
}

void ContentBrowserPanel::ShowContextMenuOnEmptySpace() noexcept {
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    if(ImGui::BeginPopupContextWindow("##ContentBrowserContextWindow", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
        if(ImGui::MenuItem("Create Folder")) {
            int count = 0;
            while(!FileUtils::CreateFolders(count ? (currentDirectory / std::filesystem::path{std::format("New folder ({})", count + 1)}) : (currentDirectory / "New folder"))) {
                ++count;
            }
            ImGui::CloseCurrentPopup();
        }
        if(ImGui::BeginMenu("Import Asset")) {
            if(ImGui::MenuItem("Texture")) {
                ImGui::CloseCurrentPopup();
                static const auto extension_list = StringUtils::Split(Image::GetSupportedExtensionsList());
                static const auto opf_str = [&]() {
                    std::string result;
                    for(auto e : extension_list) {
                        result.append(std::format("{0} file ({1})\0*{1}\0"sv, StringUtils::ToUpperCase(e.substr(1)), e));
                    }
                    result += "All Files (*.*)\0*.*\0\0"s;
                    return result;
                }();
                if(auto path = FileDialogs::OpenFile(opf_str.data()); !path.empty()) {
                    const auto asPath = std::filesystem::path{path};
                    const auto filename = asPath.filename();
                    std::filesystem::copy_file(path, currentDirectory / filename);
                }
            }
            if(ImGui::MenuItem("Font")) {
                ImGui::CloseCurrentPopup();
                static const auto opf_str = [&]() {
                    static const auto extension_list = StringUtils::Split(".ttf,.fnt");
                    static const auto description_list = StringUtils::Split("TrueType Font, BMFont");
                    GUARANTEE_OR_DIE(extension_list.size() == description_list.size(), "Font import extension list and description list size mis-match!")
                    static const auto size = extension_list.size();
                    std::string result;
                    for(std::size_t i = 0u; i < size;  ++i) {
                        const auto& ext = extension_list[i];
                        const auto& desc = description_list[i];
                        result.append(std::format("{0} file ({1})\0*{1}\0"sv, desc, ext.substr(1), ext));
                    }
                    result += "All Files (*.*)\0*.*\0\0"s;
                    return result;
                }();
                if(auto path = FileDialogs::OpenFile(opf_str.data()); !path.empty()) {
                    const auto asPath = std::filesystem::path{path};
                    const auto filename = asPath.filename();
                    std::filesystem::copy_file(path, currentDirectory / filename);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void ContentBrowserPanel::UpdateContentBrowserPaths() noexcept {
    std::scoped_lock<std::mutex> lock{m_cs};
    m_PathsCache.clear();
    for(const auto& p : std::filesystem::directory_iterator{currentDirectory}) {
        m_PathsCache.emplace_back(p.path());
    }
}

void ContentBrowserPanel::PollContentBrowserPaths() noexcept {
    if(m_UpdatePoll.CheckAndReset()) {
        UpdateContentBrowserPaths();
    }
}
