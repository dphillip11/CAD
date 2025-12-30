#include "Event.h"

template <typename... Args>
Event<Args...>::~Event() {
  for (Listener* Listener : listeners_) {
    if (Listener) {
      Listener->event_ = nullptr;
    }
  }
}

template <typename... Args>
void Event<Args...>::operator()(Args... args) {
  for (Listener* Listener : listeners_) {
    (*Listener)(args...);
  }
}

template <typename... Args>
template <typename Class>
typename Event<Args...>::Listener Event<Args...>::CreateListener(
    Class* instance, void (Class::*callback_raw)(Args...)) {
  if (instance && callback_raw) {
    auto callback = [instance, callback_raw](Args... args) { (instance->*callback_raw)(args...); };

    return Listener(callback, this);
  }
  return Listener(nullptr, this);
}

template <typename... Args>
typename Event<Args...>::Listener Event<Args...>::CreateListener(
    std::function<void(Args...)> callback) {
  return Listener(callback, this);
}

template <typename... Args>
Event<Args...>::Listener::Listener(std::function<void(Args...)> callback, Event<Args...>* Event)
    : callback_(callback), event_(Event) {
  if (event_) {
    event_->listeners_.insert(this);
  }
}

template <typename... Args>
Event<Args...>::Listener::~Listener() {
  if (event_) {
    event_->listeners_.erase(this);
  }
}

template <typename... Args>
void Event<Args...>::Listener::SetEnabled(bool enabled) {
  enabled_ = enabled;
}

template <typename... Args>
bool Event<Args...>::Listener::IsValid() const {
  return callback_ && event_;
}

template <typename... Args>
void Event<Args...>::Listener::operator()(Args... args) {
  if (callback_ && enabled_) {
    callback_(args...);
  }
}