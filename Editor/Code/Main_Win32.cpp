#include "Engine/Core/BuildConfig.hpp"

#if defined(PLATFORM_WINDOWS)

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Platform/Win.hpp"

#include "Editor/Editor.hpp"

#include <string>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 28251)
#endif

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PWSTR /*pCmdLine*/, int /*nCmdShow*/) {
    Engine<Editor>::Initialize(std::string{"Abrams Game Engine 2026"});
    Engine<Editor>::Run();
    Engine<Editor>::Shutdown();
}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#endif