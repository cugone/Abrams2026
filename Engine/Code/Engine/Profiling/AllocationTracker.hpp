#pragma once
//https://www.youtube.com/watch?v=e2ZQyYr0Oi0
//C++17 - The Best Features - Nicolai Josuttis [ACCU 2018]

#include "Engine/Core/BuildConfig.hpp"

#include <format>
#include <iostream>
#include <new>
#include <string>
#include <ostream>

class AllocationTracker {
public:
    struct status_t {
        std::size_t leaked_objs = 0u;
        std::size_t leaked_bytes = 0u;
        explicit operator bool() const noexcept {
            return leaked_objs || leaked_bytes;
        }
        friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]] const status_t& s) noexcept {
#ifdef TRACK_MEMORY
            os << std::vformat("Leaked objects {} for {} bytes.\n", std::make_format_args(s.leaked_objs, s.leaked_bytes));
#endif
            return os;
        }
    };
    struct status_frame_t {
        std::size_t frame_id = 0u;
        std::size_t leaked_objs = 0u;
        std::size_t leaked_bytes = 0u;
        explicit operator bool() const noexcept {
            return leaked_objs || leaked_bytes;
        }
        friend std::ostream& operator<<(std::ostream& os, [[maybe_unused]] const status_frame_t& s) noexcept {
#ifdef TRACK_MEMORY
            os << std::vformat("Frame {}: Leaked objects {} for {} bytes.\n", std::make_format_args(s.frame_id, s.leaked_objs, s.leaked_bytes));
#endif
            return os;
        }
    };

    [[nodiscard]] static void* allocate(std::size_t n) noexcept {
        if(is_enabled()) {
            ++frameCount;
            frameSize += n;
            ++allocCount;
            allocSize += n;
            if(maxSize < allocSize) {
                maxSize = allocSize;
            }
            if(maxCount < allocCount) {
                maxCount = allocCount;
            }
        }
        auto ptr = std::malloc(n);
        return ptr;
    }

    static void deallocate(void* ptr, std::size_t size) noexcept {
        if(is_enabled()) {
            ++framefreeCount;
            framefreeSize += size;
            ++freeCount;
            freeSize += size;
        }
        std::free(ptr);
    }

    static void enable([[maybe_unused]] bool e) noexcept {
#ifdef TRACK_MEMORY
        m_active = e;
        if(m_active) {
            resetallcounters();
        }
#endif
    }

    [[nodiscard]] static bool is_enabled() noexcept {
#ifdef TRACK_MEMORY
        return m_active;
#else
        return false;
#endif
    }

    static void trace([[maybe_unused]] bool doTrace) noexcept {
#ifdef TRACK_MEMORY
        m_trace = doTrace;
#endif
    }

    static void tick() noexcept {
#ifdef TRACK_MEMORY
        if(is_enabled()) {
            if(auto f = AllocationTracker::frame_status()) {
                std::cout << f << '\n';
            }
            ++frameCounter;
            resetframecounters();
        }
#endif
    }

    static void resetframecounters() noexcept {
#ifdef TRACK_MEMORY
        frameSize = 0u;
        frameCount = 0u;
        framefreeCount = 0u;
        framefreeSize = 0u;
#endif
    }

    static void resetstatuscounters() noexcept {
#ifdef TRACK_MEMORY
        maxSize = 0u;
        maxCount = 0u;
        allocSize = 0u;
        allocCount = 0u;
        freeCount = 0u;
        freeSize = 0u;
#endif
    }

    static void resetallcounters() noexcept {
#ifdef TRACK_MEMORY
        resetframecounters();
        resetstatuscounters();
#endif
    }

    [[nodiscard]] static status_t status() noexcept {
        return {allocCount - freeCount, allocSize - freeSize};
    }

    [[nodiscard]] static status_frame_t frame_status() noexcept {
        return {frameCounter, frameCount - framefreeCount, frameSize - framefreeSize};
    }

    inline static std::size_t maxSize = 0u;
    inline static std::size_t maxCount = 0u;
    inline static std::size_t allocSize = 0u;
    inline static std::size_t allocCount = 0u;
    inline static std::size_t frameSize = 0u;
    inline static std::size_t frameCount = 0u;
    inline static std::size_t frameCounter = 0u;
    inline static std::size_t freeCount = 0u;
    inline static std::size_t freeSize = 0u;
    inline static std::size_t framefreeCount = 0u;
    inline static std::size_t framefreeSize = 0u;

protected:
private:
    inline static bool m_active = false;
    inline static bool m_trace = false;
};

#ifdef TRACK_MEMORY

    #pragma warning(push)
    #pragma warning(disable : 28251)

void* operator new(std::size_t size);
void* operator new[](std::size_t size);
void operator delete(void* ptr, std::size_t size) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept;

    #pragma warning(pop)

#endif
