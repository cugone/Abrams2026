#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Platform/Win.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IFileLoggerService.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <format>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <sstream>

#include <Thirdparty/zlib/zlib.h>
#include <Thirdparty/zlib/unzip.h>
#include <Thirdparty/zlib/zip.h>


namespace FileUtils {

void ArchiveFile_helper(zipFile zipf, const std::filesystem::path& output, const std::vector<std::filesystem::path>& filepaths, FileUtils::ArchiveCompressionLevel compression = ArchiveCompressionLevel::Default) noexcept;
void ArchiveFile_binaryhelper(zipFile zipf, const std::filesystem::path& output, const std::vector<ArchiveBinaryFileDesc>& fileBuffers, FileUtils::ArchiveCompressionLevel compression = ArchiveCompressionLevel::Default) noexcept;
int ArchiveCompressionLevelToZLibCompressionLevel(FileUtils::ArchiveCompressionLevel compression) noexcept;

GUID GetKnownPathIdForOS(const KnownPathID& pathid) noexcept;

bool WriteBufferToFile(void* buffer, std::size_t size, std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    filepath = FS::absolute(filepath);
    filepath.make_preferred();
    const auto not_valid_path = FS::is_directory(filepath);
    const auto invalid = not_valid_path;
    if(invalid) {
        return false;
    }

    if(std::ofstream ofs{filepath, std::ios_base::binary}; ofs.write(reinterpret_cast<const char*>(buffer), size)) {
        return true;
    }
    return false;
}

bool WriteBufferToFile(const std::string& buffer, std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    filepath = FS::absolute(filepath);
    filepath.make_preferred();
    const auto not_valid_path = FS::is_directory(filepath);
    const auto invalid = not_valid_path;
    if(invalid) {
        return false;
    }

    if(std::ofstream ofs{filepath}; ofs << buffer) {
        return true;
    }
    return false;
}

std::optional<std::vector<uint8_t>> ReadBinaryBufferFromFile(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    const auto path_not_exist = !FS::exists(filepath);
    if(path_not_exist) {
        return {};
    }
    {
        std::error_code ec{};
        if(filepath = FS::canonical(filepath, ec); ec) {
            auto* logger = ServiceLocator::get<IFileLoggerService>();
            logger->LogErrorLine(std::format("File: {:s} is inaccessible.", filepath));
            return {};
        }
    }
    filepath.make_preferred();
    const auto path_is_directory = FS::is_directory(filepath);
    const auto not_valid_path = path_is_directory || path_not_exist;
    if(not_valid_path) {
        return {};
    }

    const auto byte_size = FS::file_size(filepath);
    std::vector<uint8_t> out_buffer{};
    out_buffer.resize(byte_size);
    if(std::ifstream ifs{filepath, std::ios_base::binary}; ifs.read(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size())) {
        return out_buffer;
    }
    return {};
}

std::optional<std::string> ReadStringBufferFromFile(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    const auto initial_path_not_exist = !FS::exists(filepath);
    if(initial_path_not_exist) {
        return {};
    }
    {
        std::error_code ec{};
        if(filepath = FS::canonical(filepath, ec); ec) {
            auto* logger = ServiceLocator::get<IFileLoggerService>();
            logger->LogErrorLine(std::format("File: {:s} is inaccessible.", filepath));
            return {};
        }
    }
    filepath.make_preferred();
    const auto canonical_path_not_exist = !FS::exists(filepath);
    const auto path_is_directory = FS::is_directory(filepath);
    const auto not_valid_path = path_is_directory || canonical_path_not_exist;
    if(not_valid_path) {
        return {};
    }

    if(std::ifstream ifs{filepath}; ifs) {
        return std::string(static_cast<const std::stringstream&>(std::stringstream() << ifs.rdbuf()).str());
    }
    return {};
}

//This version of ReadSome is intended for one-time-only reads of a portion of a TEXT file.
//If you want multiple reads use the ifstream version.
[[nodiscard]] std::optional<std::string> ReadSomeStringBufferFromFile(std::filesystem::path filepath, std::size_t pos, std::size_t count /*= 0u*/) noexcept {
    namespace FS = std::filesystem;
    const auto initial_path_not_exist = !FS::exists(filepath);
    if(initial_path_not_exist) {
        return {};
    }
    {
        std::error_code ec{};
        if(filepath = FS::canonical(filepath, ec); ec) {
            auto* logger = ServiceLocator::get<IFileLoggerService>();
            logger->LogErrorLine(std::format("File: {:s} is inaccessible.", filepath));
            return {};
        }
    }
    filepath.make_preferred();
    const auto canonical_path_not_exist = !FS::exists(filepath);
    const auto path_is_directory = FS::is_directory(filepath);
    const auto not_valid_path = path_is_directory || canonical_path_not_exist;
    if(not_valid_path) {
        return {};
    }
    std::ifstream ifs{filepath};
    return ReadSomeStringBufferFromFile(ifs, pos, count);
}

//This version of ReadSome is intended for continuous or multiple reads of a portion of a TEXT file.
//If you want a one-time-only single read of a portion of a TEXT file use the filepath version.
[[nodiscard]] std::optional<std::string> ReadSomeStringBufferFromFile(std::ifstream& ifs, std::streampos pos, std::streamsize count /*= 0u*/) noexcept {
    if(!(ifs && ifs.is_open())) {
        return {};
    }
    ifs.seekg(pos, std::ios::beg); // MSVC ifstream::seekg doesn't set the fail bit, so can't early-out until the get call.
    char ch{};
    std::string result{};
    result.reserve(count);
    bool readsome{false}; //If nothing read, make std::optional::has_value false.
    while(ifs && ifs.get(ch) && count > 0) {
        result.append(1, ch);
        --count;
        readsome |= true;
    }
    return readsome ? std::make_optional(result) : std::nullopt;
}

//This version of ReadSome is intended for one-time-only reads of a portion of a BINARY file.
//If you want multiple reads use the ifstream version.
//If you do, make sure it is set to binary mode.
[[nodiscard]] std::optional<std::string> ReadSomeBinaryBufferFromFile(std::filesystem::path filepath, std::size_t pos, std::size_t count /*= 0u*/) noexcept {
    namespace FS = std::filesystem;
    const auto initial_path_not_exist = !FS::exists(filepath);
    if(initial_path_not_exist) {
        return {};
    }
    {
        std::error_code ec{};
        if(filepath = FS::canonical(filepath, ec); ec) {
            auto* logger = ServiceLocator::get<IFileLoggerService>();
            logger->LogErrorLine(std::format("File: {:s} is inaccessible.", filepath));
            return {};
        }
    }
    filepath.make_preferred();
    const auto canonical_path_not_exist = !FS::exists(filepath);
    const auto path_is_directory = FS::is_directory(filepath);
    const auto not_valid_path = path_is_directory || canonical_path_not_exist;
    if(not_valid_path) {
        return {};
    }
    std::ifstream ifs(filepath, std::ios_base::binary);
    return ReadSomeBinaryBufferFromFile(ifs, pos, count);
}

//This version of ReadSome is intended for continuous or multiple reads of a portion of a BINARY file.
//Make sure the file stream is set to binary mode.
//If you want a one-time-only single read of a portion of a BINARY file use the filepath version.
[[nodiscard]] std::optional<std::string> ReadSomeBinaryBufferFromFile(std::ifstream& ifs, std::streampos pos, std::streamsize count /*= 0u*/) noexcept {
    if(!(ifs && ifs.is_open())) {
        return {};
    }
    ifs.seekg(pos);
    auto result = std::string(count, '\0');
    ifs.read(result.data(), count);
    if(ifs.gcount()) {
        return result;
    }
    return {};
}

[[nodiscard]] bool CreateFolders(const std::filesystem::path& filepath) noexcept {
    namespace FS = std::filesystem;
    std::filesystem::path p = filepath;
    p.make_preferred();
    std::error_code ec{};
    return FS::create_directories(p, ec);
}

bool IsContentPathId(const KnownPathID& pathid) noexcept {
    if(!IsSystemPathId(pathid)) {
        switch(pathid) {
        case KnownPathID::GameConfig: return true;
        case KnownPathID::GameData: return true;
        case KnownPathID::GameFonts: return true;
        case KnownPathID::GameMaterials: return true;
        case KnownPathID::GameLogs: return true;
        case KnownPathID::EngineConfig: return true;
        case KnownPathID::EngineData: return true;
        case KnownPathID::EngineFonts: return true;
        case KnownPathID::EngineMaterials: return true;
        case KnownPathID::EngineLogs: return true;
        case KnownPathID::EditorContent: return true;
        case KnownPathID::None: return false;
        case KnownPathID::Max: return false;
        default:
            ERROR_AND_DIE("UNSUPPORTED KNOWNPATHID")
        }
    }
    return false;
}

bool IsSystemPathId(const KnownPathID& pathid) noexcept {
    switch(pathid) {
    case KnownPathID::None: return false;
    case KnownPathID::GameConfig: return false;
    case KnownPathID::GameData: return false;
    case KnownPathID::GameFonts: return false;
    case KnownPathID::GameMaterials: return false;
    case KnownPathID::GameLogs: return false;
    case KnownPathID::EngineConfig: return false;
    case KnownPathID::EngineData: return false;
    case KnownPathID::EngineFonts: return false;
    case KnownPathID::EngineMaterials: return false;
    case KnownPathID::EngineLogs: return false;
    case KnownPathID::EditorContent: return false;
    case KnownPathID::Max: return false;
    case KnownPathID::TempDirectory: return true;
#if defined(PLATFORM_WINDOWS)
    case KnownPathID::Windows_AppDataRoaming: return true;
    case KnownPathID::Windows_AppDataLocal: return true;
    case KnownPathID::Windows_AppDataLocalLow: return true;
    case KnownPathID::Windows_ProgramFiles: return true;
    case KnownPathID::Windows_ProgramFilesx86: return true;
    case KnownPathID::Windows_ProgramFilesx64: return true;
    case KnownPathID::Windows_Documents: return true;
    case KnownPathID::Windows_CommonDocuments: return true;
    case KnownPathID::Windows_SavedGames: return true;
    case KnownPathID::Windows_UserProfile: return true;
    case KnownPathID::Windows_CommonProfile: return true;
    case KnownPathID::Windows_CurrentUserDesktop: return true;
#endif
    default:
        ERROR_AND_DIE("UNSUPPORTED KNOWNPATHID")
    }
}

std::filesystem::path GetKnownFolderPath(const KnownPathID& pathid) noexcept {
    namespace FS = std::filesystem;
    FS::path p{};
    if(!(IsSystemPathId(pathid) || IsContentPathId(pathid))) {
        return p;
    }
    if(pathid == KnownPathID::GameConfig) {
        p = GetWorkingDirectory() / FS::path{"Data/Config/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        } else {
            (void)FileUtils::CreateFolders(GetWorkingDirectory() / FS::path{"Data/Config/"});
            p = GetKnownFolderPath(pathid);
        }
    } else if(pathid == KnownPathID::GameData) {
        p = GetWorkingDirectory() / FS::path{"Data/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::GameMaterials) {
        p = GetWorkingDirectory() / FS::path{"Data/Materials/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::GameFonts) {
        p = GetWorkingDirectory() / FS::path{"Data/Fonts/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::GameLogs) {
        p = GetWorkingDirectory() / FS::path{"Data/Logs/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::EngineConfig) {
        p = GetWorkingDirectory() / FS::path{"Engine/Config/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        } else {
            (void)FileUtils::CreateFolders(GetWorkingDirectory() / FS::path{"Engine/Config/"});
            p = GetKnownFolderPath(pathid);
        }
    } else if(pathid == KnownPathID::EngineData) {
        p = GetWorkingDirectory() / FS::path{"Engine/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::EngineMaterials) {
        p = GetWorkingDirectory() / FS::path{"Engine/Materials/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::EngineFonts) {
        p = GetWorkingDirectory() / FS::path{"Engine/Fonts/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::EngineLogs) {
        p = GetWorkingDirectory() / FS::path{"Engine/Logs/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::EditorContent) {
        p = GetWorkingDirectory() / FS::path{"Content"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        } else {
            (void)FileUtils::CreateFolders(GetWorkingDirectory() / FS::path{"Content"});
            p = GetKnownFolderPath(pathid);
        }
    } else if(pathid == KnownPathID::TempDirectory) {
        p = GetTempDirectory();
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else {
#ifdef PLATFORM_WINDOWS
        {
            IKnownFolderManager* knownfoldermgr = nullptr;
            if(const auto kfmhr = ::CoCreateInstance(CLSID_KnownFolderManager, nullptr, CLSCTX_INPROC_SERVER, IID_IKnownFolderManager, reinterpret_cast<void**>(&knownfoldermgr)); SUCCEEDED(kfmhr)) {
                IKnownFolder* knownfolder = nullptr;
                if(const auto kfhr = knownfoldermgr->GetFolder(GetKnownPathIdForOS(pathid), &knownfolder); SUCCEEDED(kfhr)) {
                    PWSTR ppszPath = nullptr;
                    if(const auto hr_path = knownfolder->GetPath(0, &ppszPath); SUCCEEDED(hr_path)) {
                        p = FS::path(ppszPath);
                        ::CoTaskMemFree(ppszPath);
                        p = FS::canonical(p);
                        if(knownfolder) {
                            knownfolder->Release();
                            knownfolder = nullptr;
                        }
                    }
                    if(knownfoldermgr) {
                        knownfoldermgr->Release();
                        knownfoldermgr = nullptr;
                    }
                }
            }
        }
#endif
    }
    p.make_preferred();
    return p;
}

GUID GetKnownPathIdForOS(const KnownPathID& pathid) noexcept {
    switch(pathid) {
    case KnownPathID::Windows_AppDataRoaming:
        return FOLDERID_RoamingAppData;
    case KnownPathID::Windows_AppDataLocal:
        return FOLDERID_LocalAppData;
    case KnownPathID::Windows_AppDataLocalLow:
        return FOLDERID_LocalAppDataLow;
    case KnownPathID::Windows_ProgramFiles:
        return FOLDERID_ProgramFiles;
    case KnownPathID::Windows_ProgramFilesx86:
        return FOLDERID_ProgramFilesX86;
#if defined(_WIN64)
    case KnownPathID::Windows_ProgramFilesx64:
        return FOLDERID_ProgramFiles;
#elif defined(_WIN32)
    case KnownPathID::Windows_ProgramFilesx64:
        return FOLDERID_ProgramFiles;
#else
        ERROR_AND_DIE("Unknown known folder path id.");
        break;
#endif
    case KnownPathID::Windows_SavedGames:
        return FOLDERID_SavedGames;
    case KnownPathID::Windows_UserProfile:
        return FOLDERID_Profile;
    case KnownPathID::Windows_CommonProfile:
        return FOLDERID_Public;
    case KnownPathID::Windows_CurrentUserDesktop:
        return FOLDERID_Desktop;
    case KnownPathID::Windows_CommonDesktop:
        return FOLDERID_PublicDesktop;
    case KnownPathID::Windows_Documents:
        return FOLDERID_Documents;
        break;
    case KnownPathID::Windows_CommonDocuments:
        return FOLDERID_PublicDocuments;
        break;
    default:
        ERROR_AND_DIE("Unknown known folder path id.");
        break;
    }
}

std::filesystem::path GetWorkingDirectory() noexcept {
    namespace FS = std::filesystem;
    return FS::current_path();
}

void SetWorkingDirectory(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    FS::current_path(p);
}

std::filesystem::path GetTempDirectory() noexcept {
    return std::filesystem::temp_directory_path();
}

bool HasDeletePermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    const auto parent_path = p.parent_path();
    const auto parent_status = FS::status(parent_path);
    const auto parent_perms = parent_status.permissions();
    if(FS::perms::none == (parent_perms & (FS::perms::owner_write | FS::perms::group_write | FS::perms::others_write))) {
        return false;
    }
    return true;
}

bool HasExecuteOrSearchPermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(FS::is_directory(p)) {
        return HasSearchPermissions(p);
    } else {
        return HasExecutePermissions(p);
    }
}

bool HasExecutePermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(FS::is_directory(p)) {
        return false;
    }
    const auto status = FS::status(p);
    const auto perms = status.permissions();
    if(FS::perms::none == (perms & (FS::perms::owner_exec | FS::perms::group_exec | FS::perms::others_exec))) {
        return false;
    }
    return true;
}

bool HasSearchPermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(!FS::is_directory(p)) {
        return false;
    }
    const auto parent_path = p.parent_path();
    const auto parent_status = FS::status(parent_path);
    const auto parent_perms = parent_status.permissions();
    if(FS::perms::none == (parent_perms & (FS::perms::owner_exec | FS::perms::group_exec | FS::perms::others_exec))) {
        return false;
    }
    return true;
}

bool HasWritePermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    const auto my_status = FS::status(p);
    const auto my_perms = my_status.permissions();
    if(FS::perms::none == (my_perms & (FS::perms::owner_write | FS::perms::group_write | FS::perms::others_write))) {
        return false;
    }
    return true;
}

bool HasReadPermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    const auto my_status = FS::status(p);
    const auto my_perms = my_status.permissions();
    if(FS::perms::none == (my_perms & (FS::perms::owner_read | FS::perms::group_read | FS::perms::others_read))) {
        return false;
    }
    return true;
}

bool IsSafeWritePath(const std::filesystem::path& p) noexcept {
    //Check for any write permissions on the file and parent directory
    if(!(HasWritePermissions(p) || HasDeletePermissions(p))) {
        return false;
    }

    try {
        const auto is_in_working_dir = IsChildOf(p, GetWorkingDirectory());
        const auto is_in_gamedata_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::GameData));
        const auto is_in_enginedata_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::EngineData));
        const auto is_in_editorcontent_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::EditorContent));
        const auto is_temp_dir = IsChildOf(p, GetTempDirectory());
        const auto is_next_to_exe = IsSiblingOf(p, GetExePath());
        const auto is_known_OS_dir = false;

        const auto safe = is_in_working_dir || is_in_gamedata_dir || is_in_enginedata_dir || is_in_editorcontent_dir || is_temp_dir || is_next_to_exe || is_known_OS_dir;
        return safe;
    } catch(const std::filesystem::filesystem_error& e) {
        DebuggerPrintf(std::format("\nFilesystem Error:\nWhat: {:s}\nCode: {}\nPath1: {:s}\nPath2: {:s}\n", e.what(), e.code().value(), e.path1(), e.path2()));
        return false;
    } catch(...) {
        DebuggerPrintf(std::format("\nUnspecified error trying to determine if path:\n{:s}\n is a safe write path.", p));
        return false;
    }
}

bool IsSafeReadPath(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(p)) {
        DebuggerPrintf(std::format("Filesystem Error: {:s} does not exist.\n", p));
        return false;
    }
    //Check for any read permissions on the file and parent directory
    if(!(HasReadPermissions(p) || HasExecuteOrSearchPermissions(p))) {
        DebuggerPrintf(std::format("Filesystem Error: {:s} is inaccessible.\n", p));
        return false;
    }

    try {
        const auto is_in_working_dir = IsChildOf(p, GetWorkingDirectory());
        const auto is_in_gamedata_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::GameData));
        const auto is_in_enginedata_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::EngineData));
        const auto is_in_editorcontent_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::EditorContent));
        const auto is_next_to_exe = IsSiblingOf(p, GetExePath());
        const auto is_known_OS_dir = false;

        if(const auto safe = is_in_working_dir || is_in_gamedata_dir || is_in_enginedata_dir || is_in_editorcontent_dir || is_next_to_exe || is_known_OS_dir; !safe) {
            const auto p_str = p.string();
            const auto workingdir_str = GetWorkingDirectory().string();
            const auto kfpGameDatadir_str = GetKnownFolderPath(KnownPathID::GameData).string();
            const auto kfpEngineDatadir_str = GetKnownFolderPath(KnownPathID::EngineData).string();
            const auto kfpEditorContentdir_str = GetKnownFolderPath(KnownPathID::EditorContent).string();
            DebuggerPrintf(std::vformat("Filesystem Error: {:s} is not a safe read location. File must exist in or be a child of:\n\"{:s}\"\n\"{:s}\"\n\"{:s}\"\n\"{:s}\"\n: ", std::make_format_args(p_str, workingdir_str, kfpGameDatadir_str, kfpEngineDatadir_str, kfpEditorContentdir_str)));
            return false;
        }
        return true;
    } catch(const std::filesystem::filesystem_error& e) {
        DebuggerPrintf(std::format("\nFilesystem Error:\nWhat: {:s}\nCode: {}\nPath1: {:s}\nPath2: {:s}\n", e.what(), e.code().value(), e.path1(), e.path2()));
        return false;
    } catch(...) {
        DebuggerPrintf(std::format("\nUnspecified error trying to determine if path:\n{:s}\n is a safe read path.", p));
        return false;
    }
}

bool IsParentOf(const std::filesystem::path& p, const std::filesystem::path& child) noexcept {
    namespace FS = std::filesystem;
    std::error_code ec{};
    if(const auto p_canon = FS::canonical(p, ec); !ec) {
        if(const auto child_canon = FS::canonical(child, ec); !ec) {
            for(auto iter = FS::recursive_directory_iterator{ p_canon }; iter != FS::recursive_directory_iterator{}; ++iter) {
                const auto& entry = *iter;
                const std::filesystem::path sub_p = entry.path();
                if(sub_p == child_canon) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsSiblingOf(const std::filesystem::path& p, const std::filesystem::path& sibling) noexcept {
    namespace FS = std::filesystem;
    std::error_code ec{};
    if(const auto my_parent_path = FS::canonical(p.parent_path(), ec); !ec) {
        if(const auto sibling_parent_path = FS::canonical(sibling.parent_path(), ec); !ec) {
            return my_parent_path == sibling_parent_path;
        }
    }
    return false;
}

bool IsChildOf(const std::filesystem::path& p, const std::filesystem::path& parent) noexcept {
    namespace FS = std::filesystem;
    std::error_code ec{};
    if(const auto parent_canon = FS::canonical(parent, ec); !ec) {
        if(const auto p_canon = FS::canonical(p, ec); !ec) {
            for(auto iter = FS::recursive_directory_iterator{parent_canon}; iter != FS::recursive_directory_iterator{}; iter = iter.increment(ec)) {
                if(ec) {
                    continue;
                }
                const auto& entry = *iter;
                const std::filesystem::path sub_p = entry.path();
                if(sub_p == p_canon) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::size_t CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, bool recursive /*= false*/) noexcept {
    namespace FS = std::filesystem;
    auto count = std::size_t{0u};
    const auto cb = [&count](const FS::path&) -> void { ++count; };
    ForEachFileInFolder(folderpath, validExtensionList, cb, recursive);
    return count;
}

std::vector<std::filesystem::path> GetAllPathsInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, bool recursive /*= false*/) noexcept {
    return [&](){
        std::vector<std::filesystem::path> paths;
        paths.reserve(CountFilesInFolders(folderpath, validExtensionList, recursive));
        const auto add_path_cb = [&paths](const std::filesystem::path& p) { paths.push_back(p); };
        ForEachFileInFolder(folderpath, validExtensionList, add_path_cb, recursive);
        return paths;
    }();
}

/**
 * Archive a collection of bytes representing files.
 * Writes output file to disk and returns true if successful.
 */
bool ArchiveFilesFromMemory(const std::filesystem::path& output, const std::vector<ArchiveBinaryFileDesc>& buffersToArchive, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::Default*/) noexcept {
    if(auto zipf = zipOpen64(output.string().c_str(), APPEND_STATUS_CREATE); zipf != nullptr) {
        ArchiveFile_binaryhelper(zipf, output, buffersToArchive, compression);
        zipClose(zipf, nullptr);
        return true;
    }
    return false;
}

/**
 * Add a file to an already existing archive.
 */
bool AddFileToArchiveFromMemory(const std::filesystem::path& archiveToAppend, const std::filesystem::path& filepath, const std::vector<uint8_t>& buffer, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::Default*/) noexcept {
    if(std::filesystem::exists(archiveToAppend)) {
        if(auto zipf = zipOpen64(archiveToAppend.string().c_str(), APPEND_STATUS_ADDINZIP); zipf != nullptr) {
            const std::vector<ArchiveBinaryFileDesc> fileBuffers{ ArchiveBinaryFileDesc{filepath, buffer} };
            ArchiveFile_binaryhelper(zipf, archiveToAppend, fileBuffers, compression);
            zipClose(zipf, nullptr);
            return true;
        }
    }
    return false;
}

/**
 * Add a collection of file buffers to an already existing archive.
 */
bool AddFilesToArchiveFromMemory(const std::filesystem::path& archiveToAppend, const std::vector<ArchiveBinaryFileDesc>& fileBuffers, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::Default*/) noexcept {
    if(std::filesystem::exists(archiveToAppend)) {
        if(auto zipf = zipOpen64(archiveToAppend.string().c_str(), APPEND_STATUS_ADDINZIP); zipf != nullptr) {
            ArchiveFile_binaryhelper(zipf, archiveToAppend, fileBuffers, compression);
            zipClose(zipf, nullptr);
            return true;
        }
    }
    return false;
}

/**
 * Archive a collection of individual files.
 * Writes output file to disk and returns true if successful.
 */
bool ArchiveFiles(const std::filesystem::path& output, const std::vector<std::filesystem::path>& filepaths, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::None*/) noexcept {
    if(auto zipf = zipOpen64(output.string().c_str(), APPEND_STATUS_CREATE); zipf != nullptr) {
        ArchiveFile_helper(zipf, output, filepaths, compression);
        zipClose(zipf, nullptr);
        return true;
    }
    auto* logger = ServiceLocator::get<IFileLoggerService>();
    logger->LogWarnLine(std::format("ArchiveFiles: Failed to create an empty archive file from {}.", output));
    return false;
}

/**
 * Archive all files in a folder.
 * Writes output file to disk and returns true if successful.
 */
bool ArchiveFilesInFolder(const std::filesystem::path& output, const std::filesystem::path& folderpath, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::None*/, bool recursive /*= false*/) noexcept {
    auto* logger = ServiceLocator::get<IFileLoggerService>();
    if(std::filesystem::is_directory(folderpath)) {
        if(auto zipf = zipOpen64(output.string().c_str(), APPEND_STATUS_CREATE); zipf != nullptr) {
            const auto filepaths = FileUtils::GetAllPathsInFolders(folderpath, std::string{}, recursive);
            ArchiveFile_helper(zipf, output, filepaths, compression);
            zipClose(zipf, nullptr);
            return true;
        }
        logger->LogWarnLine(std::format("ArchiveFiles: Failed to create an empty archive file from {}.", output));
    }
    logger->LogWarnLine(std::format("ArchiveFilesInFolder: {} is not a directory.", folderpath));
    return false;
}

void FileUtils::RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, std::size_t mostRecentCountToKeep, const std::string& validExtensionList /*= std::string{}*/) noexcept {
    if(!IsSafeWritePath(folderpath)) {
        return;
    }
    namespace FS = std::filesystem;
    if(mostRecentCountToKeep < CountFilesInFolders(folderpath, validExtensionList)) {
        auto paths = GetAllPathsInFolders(folderpath);
        const auto sort_pred = [](const FS::path& a, const FS::path& b) { return FS::last_write_time(a) > FS::last_write_time(b); };
        std::sort(std::begin(paths), std::end(paths), sort_pred);
        if(mostRecentCountToKeep > 0) {
            const auto erase_end = std::begin(paths) + mostRecentCountToKeep;
            paths.erase(std::begin(paths), erase_end);
        }
        for(auto& p : paths) {
            FS::remove(p);
        }
    }
}

void ArchiveFile_helper(zipFile zipf, const std::filesystem::path& output, const std::vector<std::filesystem::path>& filepaths, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::Default*/) noexcept {
    const auto compression_level = ArchiveCompressionLevelToZLibCompressionLevel(compression);
    auto* logger = ServiceLocator::get<IFileLoggerService>();
    for(const auto& cur_file : filepaths) {
        const auto& buffer = FileUtils::ReadBinaryBufferFromFile(cur_file);
        const auto& cur_filepath_as_string = cur_file.string();
        const auto& cur_file_filename = cur_file.filename();
        zip_fileinfo zipfi{};
        if(const auto open_result = zipOpenNewFileInZip64(zipf, cur_filepath_as_string.c_str(), &zipfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, compression_level, true); open_result != ZIP_OK) {
            logger->LogWarnLine(std::format("minizip could not inject file {} to {}", cur_file_filename, output));
        }
        if(buffer.has_value()) {
            if(const auto write_result = zipWriteInFileInZip(zipf, buffer->data(), static_cast<unsigned int>(buffer->size())); write_result != ZIP_OK) {
                logger->LogWarnLine(std::format("minizip could not append file {} to {}", cur_file_filename, output));
            }
        }
        zipCloseFileInZip(zipf);
    }
}

int ArchiveCompressionLevelToZLibCompressionLevel(FileUtils::ArchiveCompressionLevel compression) noexcept {
    switch(compression) {
    case ArchiveCompressionLevel::None: return Z_NO_COMPRESSION;
    case ArchiveCompressionLevel::Size: return Z_BEST_COMPRESSION;
    case ArchiveCompressionLevel::Speed: return Z_BEST_SPEED;
    case ArchiveCompressionLevel::Default: return Z_DEFAULT_COMPRESSION;
    default: return Z_DEFAULT_COMPRESSION;
    }
}

void ArchiveFile_binaryhelper(zipFile zipf, const std::filesystem::path& output, const std::vector<ArchiveBinaryFileDesc>& fileBuffers, FileUtils::ArchiveCompressionLevel compression /*= ArchiveCompressionLevel::Default*/) noexcept {
    const auto compression_level = ArchiveCompressionLevelToZLibCompressionLevel(compression);
    auto* logger = ServiceLocator::get<IFileLoggerService>();
    for(const auto& cur_file : fileBuffers) {
        const auto& buffer = cur_file.buffer;
        const auto& cur_filepath_as_string = cur_file.filepath.string();
        const auto& cur_file_filename = cur_file.filepath.filename();
        zip_fileinfo zipfi{};
        if(const auto open_result = zipOpenNewFileInZip64(zipf, cur_filepath_as_string.c_str(), &zipfi, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, compression_level, true); open_result != ZIP_OK) {
            logger->LogWarnLine(std::format("minizip could not inject file {} to {}", cur_file_filename, output));
        }
        if(const auto write_result = zipWriteInFileInZip(zipf, buffer.data(), static_cast<unsigned int>(buffer.size())); write_result != ZIP_OK) {
            logger->LogWarnLine(std::format("minizip could not append file {} to {}", cur_file_filename, output));
        }
        zipCloseFileInZip(zipf);
    }
}

} // namespace FileUtils