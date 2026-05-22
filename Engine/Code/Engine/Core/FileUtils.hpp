#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <cstdlib>
#include <cstdint>
#include <filesystem>
#include <format>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

template<>
class std::formatter<std::filesystem::path> : public std::formatter<std::string> {
public:
    auto format(const std::filesystem::path& p, auto& ctx) const {
        return std::formatter<std::string>::format(p.string(), ctx);
    }

private:
};

namespace FileUtils {

enum class KnownPathID {
    None,
    Windows_AppDataRoaming,
    Windows_AppDataLocal,
    Windows_AppDataLocalLow,
    Windows_ProgramFiles,
    Windows_ProgramFilesx86,
    Windows_ProgramFilesx64,
    Windows_Documents,
    Windows_CommonDocuments,
    Windows_SavedGames,
    Windows_UserProfile,
    Windows_CommonProfile,
    Windows_CurrentUserDesktop,
    Windows_CommonDesktop,
    TempDirectory,
    GameData,
    GameConfig,
    GameFonts,
    GameMaterials,
    GameLogs,
    EngineConfig,
    EngineData,
    EngineFonts,
    EngineMaterials,
    EngineLogs,
    EditorContent,
    Max
};

enum class ArchiveCompressionLevel {
    None
    , Speed
    , Size
    , Default
};

struct ArchiveBinaryFileDesc {
    std::filesystem::path filepath;
    std::vector<uint8_t> buffer;
};

[[nodiscard]] bool WriteBufferToFile(void* buffer, std::streamsize size, std::filesystem::path filepath) noexcept;
[[nodiscard]] bool WriteBufferToFile(const std::string& buffer, std::filesystem::path filepath) noexcept;
[[nodiscard]] std::optional<std::vector<uint8_t>> ReadBinaryBufferFromFile(std::filesystem::path filepath) noexcept;
[[nodiscard]] std::optional<std::string> ReadStringBufferFromFile(std::filesystem::path filepath) noexcept;
[[nodiscard]] std::optional<std::string> ReadSomeBinaryBufferFromFile(std::filesystem::path filepath, std::streampos pos, std::streamsize count = 0u) noexcept;
[[nodiscard]] std::optional<std::string> ReadSomeBinaryBufferFromFile(std::ifstream& ifs, std::streampos pos, std::streamsize count = 0u) noexcept;
[[nodiscard]] std::optional<std::string> ReadSomeStringBufferFromFile(std::filesystem::path filepath, std::streampos pos, std::streamsize count = 0u) noexcept;
[[nodiscard]] std::optional<std::string> ReadSomeStringBufferFromFile(std::ifstream& ifs, std::streampos pos, std::streamsize count = 0u) noexcept;
[[nodiscard]] bool CreateFolders(const std::filesystem::path& filepath) noexcept;
[[nodiscard]] bool IsSystemPathId(const KnownPathID& pathid) noexcept;
[[nodiscard]] bool IsContentPathId(const KnownPathID& pathid) noexcept;
[[nodiscard]] std::filesystem::path GetKnownFolderPath(const KnownPathID& pathid) noexcept;
[[nodiscard]] std::filesystem::path GetExePath() noexcept;
[[nodiscard]] std::filesystem::path GetWorkingDirectory() noexcept;
[[nodiscard]] std::filesystem::path GetTempDirectory() noexcept;
[[nodiscard]] void SetWorkingDirectory(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool IsSafeWritePath(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool IsSafeReadPath(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool HasWritePermissions(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool HasReadPermissions(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool HasDeletePermissions(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool HasExecuteOrSearchPermissions(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool HasExecutePermissions(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool HasSearchPermissions(const std::filesystem::path& p) noexcept;
[[nodiscard]] bool IsParentOf(const std::filesystem::path& p, const std::filesystem::path& child) noexcept;
[[nodiscard]] bool IsSiblingOf(const std::filesystem::path& p, const std::filesystem::path& sibling) noexcept;
[[nodiscard]] bool IsChildOf(const std::filesystem::path& p, const std::filesystem::path& parent) noexcept;

[[nodiscard]] std::size_t CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false) noexcept;
void RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, std::size_t mostRecentCountToKeep, const std::string& validExtensionList = std::string{}) noexcept;
[[nodiscard]] std::vector<std::filesystem::path> GetAllPathsInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false) noexcept;
[[nodiscard]] bool ArchiveFiles(const std::filesystem::path& output, const std::vector<std::filesystem::path>& filepaths, FileUtils::ArchiveCompressionLevel compression = FileUtils::ArchiveCompressionLevel::Default) noexcept;
[[nodiscard]] bool ArchiveFilesInFolder(const std::filesystem::path& output, const std::filesystem::path& folderpath, FileUtils::ArchiveCompressionLevel compression = FileUtils::ArchiveCompressionLevel::Default, bool recursive = false) noexcept;
[[nodiscard]] bool ArchiveFilesFromMemory(const std::filesystem::path& output, const std::vector<ArchiveBinaryFileDesc>& buffersToArchive, FileUtils::ArchiveCompressionLevel compression = ArchiveCompressionLevel::Default) noexcept;
[[nodiscard]] bool AddFileToArchiveFromMemory(const std::filesystem::path& archiveToAppend, const std::vector<uint8_t>& buffer, FileUtils::ArchiveCompressionLevel compression = ArchiveCompressionLevel::Default) noexcept;

namespace detail {

template<typename DirectoryIteratorType, typename Callable>
void ForEachFileInFolders(const std::filesystem::path& preferred_folderpath, std::vector<std::string> validExtensions, Callable&& callback) noexcept {
    if(validExtensions.empty()) {
        std::for_each(DirectoryIteratorType{preferred_folderpath}, DirectoryIteratorType{},
                      [&callback](const std::filesystem::directory_entry& entry) {
                          const auto& cur_path = entry.path();
                          bool is_file = std::filesystem::is_regular_file(cur_path);
                          if(is_file) {
                              std::invoke(callback, cur_path);
                          }
                      });
        return;
    }
    std::for_each(DirectoryIteratorType{preferred_folderpath}, DirectoryIteratorType{},
                  [&validExtensions, &callback](const std::filesystem::directory_entry& entry) {
                      const auto& cur_path = entry.path();
                      bool is_file = std::filesystem::is_regular_file(cur_path);
                      std::string my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
                      if(is_file) {
                          if(bool valid_file_by_extension = std::find(std::begin(validExtensions), std::end(validExtensions), my_extension) != std::end(validExtensions); valid_file_by_extension) {
                              std::invoke(callback, cur_path);
                          }
                      }
                  });
}
} // namespace detail

template<typename Callable>
void ForEachFileInFolder(
const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, Callable&& callback = [](const std::filesystem::path&) {}, bool recursive = false) noexcept {
    const auto exists = std::filesystem::exists(folderpath);
    if(!exists) {
        return;
    }
    std::filesystem::path preferred_folderpath = folderpath;
    {
        std::error_code ec{};
        preferred_folderpath = std::filesystem::canonical(preferred_folderpath, ec);
        if(ec) {
            return;
        }
    }
    preferred_folderpath.make_preferred();
    const auto is_directory = std::filesystem::is_directory(preferred_folderpath);
    const auto is_folder = exists && is_directory;
    if(!is_folder) {
        return;
    }
    const auto validExtensions = StringUtils::Split(StringUtils::ToLowerCase(validExtensionList));
    if(!recursive) {
        detail::ForEachFileInFolders<std::filesystem::directory_iterator>(preferred_folderpath, validExtensions, callback);
    } else {
        detail::ForEachFileInFolders<std::filesystem::recursive_directory_iterator>(preferred_folderpath, validExtensions, callback);
    }
}

} // namespace FileUtils