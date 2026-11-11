#include "Engine/Online/SteamOnlineSystem.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAppService.hpp"
#include "Engine/Services/IFileLoggerService.hpp"


#if defined(ONLINE_STEAM)
    #include <Thirdparty/steam/steam_api.h>
#endif


#include <filesystem>
#include <format>

constexpr const uint32_t invalid_steam_app_id = 0u;
constexpr const uint32_t debug_steam_app_id = 480; //SpaceWar!
constexpr const uint32_t my_steam_app_id = STEAM_APP_ID;

SteamOnlineSystem::~SteamOnlineSystem() noexcept {
#if defined(ONLINE_STEAM)
    Shutdown();
#endif
}

const uint32_t SteamOnlineSystem::GetAppId() const noexcept {
#if defined(ONLINE_STEAM)
    const auto app_id = []() -> const uint32_t {
    #if defined(DEBUG_BUILD)
        if(std::filesystem::exists("steam_appid.txt")) {
            if(auto buffer = FileUtils::ReadStringBufferFromFile("steam_appid.txt"); buffer.has_value()) {
                try {
                    return static_cast<uint32_t>(std::stoull(*buffer));
                } catch(...) {
                    return 0u;
                }
            } else {
                return debug_steam_app_id;
            }
        }
        return debug_steam_app_id;
    #else
        return my_steam_app_id;
    #endif
    }();
    return app_id;
#else
    return 0u;
#endif
}

bool SteamOnlineSystem::Initialize([[maybe_unused]] const uint32_t appId) noexcept {
#if defined(PLATFORM_WINDOWS)
    if(HMODULE steam_module = ::LoadLibraryA("steam_api64.dll"); steam_module == nullptr) {
        auto* logger = ServiceLocator::get<IFileLoggerService>();
        logger->LogErrorLine(std::format("Failed to load required Steam dynamic library."));
        return false;
    }
#else
    return false;
#endif
#if defined(ONLINE_STEAM)
    if(SteamAPI_RestartAppIfNecessary(GetAppId())) {
        auto* logger = ServiceLocator::get<IFileLoggerService>();
        logger->LogErrorLine(std::format("This game requires Steam to be running to function."));
        auto* app = ServiceLocator::get<IAppService>();
        app->SetIsQuitting(true);
        return false;
    }
    if(SteamErrMsg msg; SteamAPI_InitEx(&msg) != k_ESteamAPIInitResult_OK) {
        auto* logger = ServiceLocator::get<IFileLoggerService>();
        logger->LogErrorLine(std::format("Failed to initialize Steam: \"{:s}\"", msg));
        return false;
    }
    m_isInitialized = true;
    return true;
#else
    return false;
#endif
}

void SteamOnlineSystem::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
#if defined(ONLINE_STEAM)
    if(!m_isInitialized) {
        return;
    }
#endif
}

void SteamOnlineSystem::Shutdown() noexcept {
#if defined(ONLINE_STEAM)
    if(m_isInitialized) {
        m_isInitialized = false;
        SteamAPI_Shutdown();
    }
#endif
}

void SteamOnlineSystem::BeginFrame() noexcept {
#if defined(ONLINE_STEAM)
    if(!m_isInitialized) {
        return;
    }
#endif
}

void SteamOnlineSystem::EndFrame() noexcept {
#if defined(ONLINE_STEAM)
    if(!m_isInitialized) {
        SteamAPI_RunCallbacks();
        return;
    }
#endif
}
