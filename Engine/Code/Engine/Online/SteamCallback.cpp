#include "Engine/Online/SteamCallback.hpp"

#include <type_traits>

#include <Thirdparty/steam/steam_api.h>

template<typename T>
class SteamCallback<T>::Impl {
public:
    Impl(Callback callback) : m_callback(callback) {}

    STEAM_CALLBACK(Impl, OnSteamEvent, T);

protected:
private:
    Callback m_callback;
};

template<typename T>
void SteamCallback<T>::Impl::OnSteamEvent(T* pParam) {
    if(m_callback) {
        std::invoke(m_callback, pParam);
    }
}


template<typename T>
SteamCallback<T>::SteamCallback(Callback callback)
: pImpl(std::make_unique<Impl>(callback)) {
    /* DO NOTHING */
}

template<typename T>
SteamCallback<T>::~SteamCallback() = default;

REGISTER_STEAM_CALLBACK_WRAPPER(GameOverlayActivated_t);
