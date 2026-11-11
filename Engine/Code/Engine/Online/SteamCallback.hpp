#pragma once

#include <functional>
#include <memory>

template<typename T>
class SteamCallback {
public:

    using Callback = std::function<void(T*)>;

    SteamCallback(Callback callback);
    ~SteamCallback();

protected:
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};


#define REGISTER_STEAM_CALLBACK_WRAPPER(Type) template class SteamCallback<Type>;
