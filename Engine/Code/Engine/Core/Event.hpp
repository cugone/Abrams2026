#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <functional>
#include <vector>

template<typename... Args>
class Event {
public:
    // Subscription handle
    struct Subscription {
        std::size_t id{};
        bool operator==(const Subscription&) const = default;
    };

    Event() = default;
    ~Event() = default;

    // -----------------------------------------------------
    // Subscribe any invocable: lambda, functor, free function, std::bind, etc.
    // -----------------------------------------------------
    template<std::invocable<Args...> F>
    Subscription subscribe(F&& f) {
        std::size_t id = next_id_++;
        subscribers_.push_back({id, std::function<void(Args...)>(std::forward<F>(f))});
        return {id};
    }

    // -----------------------------------------------------
    // Subscribe to a member function: event.subscribe(obj, &T::method)
    // -----------------------------------------------------
    template<typename T>
    Subscription subscribe(T* obj, void (T::*method)(Args...)) {
        return subscribe([obj, method](Args... a) {
            (obj->*method)(a...);
        });
    }

    // -----------------------------------------------------
    // Unsubscribe using subscription token
    // -----------------------------------------------------
    void unsubscribe(const Subscription& sub) noexcept {
        subscribers_.erase(
        std::remove_if(subscribers_.begin(), subscribers_.end(),
                       [&](const auto& s) { return s.id == sub.id; }),
        subscribers_.end());
    }

    // -----------------------------------------------------
    // Trigger the event
    // -----------------------------------------------------
    void trigger(Args... args) const noexcept {
        for(const auto& s : subscribers_) {
            s.cb(args...);
        }
    }

private:
    struct Entry {
        std::size_t id;
        std::function<void(Args...)> cb;
    };

    std::vector<Entry> subscribers_;
    std::size_t next_id_{0};
};

// ------------------------------------------------------
// Optional RAII auto-unsubscribe helper
// ------------------------------------------------------
template<typename... Args>
class ScopedSubscription {
public:
    ScopedSubscription(Event<Args...>& e, typename Event<Args...>::Subscription s)
    : evt(e)
    , sub(s) {
    }

    ScopedSubscription(const ScopedSubscription& other) noexcept = default;
    ScopedSubscription(ScopedSubscription&& rother) noexcept = default;
    ScopedSubscription& operator=(const ScopedSubscription& other) noexcept = default;
    ScopedSubscription& operator=(ScopedSubscription&& other) noexcept = default;

    ~ScopedSubscription() noexcept {
        evt.unsubscribe(sub);
    }

private:
    Event<Args...>& evt;
    typename Event<Args...>::Subscription sub;
};



//#pragma once
//
//#include <algorithm>
//#include <vector>
//
//template<typename... ARGS>
//class Event {
//public:
//    struct event_sub_t;
//    using cb_t = void (*)(event_sub_t*, ARGS...);
//    using cb_with_arg_t = void (*)(void*, ARGS...);
//
//    struct event_sub_t {
//        cb_t cb;
//        void* secondary_cb;
//        void* user_arg;
//    };
//
//    Event() = default;
//    ~Event() = default;
//
//    void Subscribe(void* user_arg, cb_with_arg_t cb) noexcept {
//        event_sub_t sub;
//        sub.cb = FunctionWithArgumentCallback;
//        sub.secondary_cb = cb;
//        sub.user_arg = user_arg;
//        m_subscriptions.push_back(sub);
//    }
//
//    void Unsubscribe(void* user_arg, void* cb) noexcept {
//        m_subscriptions.erase(std::remove_if(std::begin(m_subscriptions),
//                                           std::end(m_subscriptions),
//                                           [&cb, &user_arg](const event_sub_t& sub) {
//                                               return (sub.secondary_cb == cb) && (sub.user_arg == user_arg);
//                                           }),
//                            std::end(m_subscriptions));
//    }
//
//    void Unsubscribe_by_argument(void* user_arg) noexcept {
//        m_subscriptions.erase(std::remove_if(std::begin(m_subscriptions),
//                                           std::end(m_subscriptions),
//                                           [&user_arg](const event_sub_t& sub) {
//                                               return sub.user_arg == user_arg;
//                                           }),
//                            std::end(m_subscriptions));
//    }
//
//    template<typename T>
//    void Subscribe_method(T* obj, void (T::*mcb)(ARGS...)) noexcept {
//        event_sub_t sub;
//        sub.cb = MethodCallback<T, decltype(mcb)>;
//        sub.secondary_cb = *(void**)(&mcb);
//        sub.user_arg = obj;
//        m_subscriptions.push_back(sub);
//    }
//
//    template<typename T>
//    void Unsubscribe_method(T* obj, void (T::*mcb)(ARGS...)) noexcept {
//        Unsubscribe(obj, *(void**)&mcb);
//    }
//
//    template<typename T>
//    void Unsubscribe_object(T* obj) noexcept {
//        Unsubscribe_by_argument(obj);
//    }
//
//    void Trigger(ARGS... args) const noexcept {
//        for(const auto& sub : m_subscriptions) {
//            sub.cb(&sub, args...);
//        }
//    }
//
//    void Trigger(ARGS... args) noexcept {
//        for(auto& sub : m_subscriptions) {
//            sub.cb(&sub, args...);
//        }
//    }
//
//private:
//    std::vector<event_sub_t> m_subscriptions;
//
//    static void FunctionWithArgumentCallback(event_sub_t* sub, ARGS... args) noexcept;
//
//    template<typename T, typename MCB>
//    static void MethodCallback(event_sub_t* sub, ARGS... args) noexcept;
//};
//
//template<typename... ARGS>
//void Event<ARGS...>::FunctionWithArgumentCallback(event_sub_t* sub, ARGS... args) noexcept {
//    cb_with_arg_t cb = (cb_with_arg_t)(sub->secondary_cb);
//    cb(sub->user_arg, args...);
//}
//
//template<typename... ARGS>
//template<typename T, typename MCB>
//void Event<ARGS...>::MethodCallback(event_sub_t* sub, ARGS... args) noexcept {
//    MCB mcb = *(MCB*)&(sub->secondary_cb);
//    T* obj = (T*)(sub->user_arg);
//    (obj->*mcb)(args...);
//}
