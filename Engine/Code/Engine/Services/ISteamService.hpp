#pragma once

#include "Engine/Services/IService.hpp"

#include "Engine/Core/TimeUtils.hpp"

#include <cstdint>

class ISteamService : public IService {
public:
    virtual ~ISteamService() noexcept {};

    [[nodiscard]] virtual const uint32_t GetAppId() const noexcept = 0;
    [[nodiscard]] virtual bool Initialize(uint32_t appId) noexcept = 0;
    [[nodiscard]] virtual void Shutdown() noexcept = 0;

    virtual void BeginFrame() noexcept = 0;
    virtual void Update(TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void EndFrame() noexcept = 0;

protected:
private:
    
};

class NullSteamService : public ISteamService {
public:
    virtual ~NullSteamService() noexcept {};

    [[nodiscard]] const uint32_t GetAppId() const noexcept override { return uint32_t{0u}; } //Default to invalid ID
    [[nodiscard]] bool Initialize([[maybe_unused]] uint32_t appId) noexcept override { return false; }
    [[nodiscard]] void Shutdown() noexcept override {}

    [[nodiscard]] void BeginFrame() noexcept override {}
    [[nodiscard]] void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {};
    [[nodiscard]] void EndFrame() noexcept override {}

protected:
private:

};
