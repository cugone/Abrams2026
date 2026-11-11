#pragma once

#include "Engine/Services/ISteamService.hpp"

#include "Engine/Core/TimeUtils.hpp"

class SteamOnlineSystem : public ISteamService {
public:
    SteamOnlineSystem() noexcept = default;
    SteamOnlineSystem(const SteamOnlineSystem& other) noexcept = delete;
    SteamOnlineSystem(SteamOnlineSystem&& rother) noexcept = delete;
    SteamOnlineSystem& operator=(const SteamOnlineSystem& rhs) noexcept = delete;
    SteamOnlineSystem& operator=(SteamOnlineSystem&& rother) noexcept = delete;
    virtual ~SteamOnlineSystem() noexcept;
    

    const uint32_t GetAppId() const noexcept override;
    bool Initialize(const uint32_t appId) noexcept override;
    void Shutdown() noexcept override;

    void BeginFrame() noexcept override;
    void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void EndFrame() noexcept override;

protected:
 private:
     bool m_isInitialized{false};
};
