#pragma once
#include <functional>
#include <set>

template <typename... Args>
class Event {
 public:
  class Listener;

  Event() = default;
  ~Event();

  void operator()(Args... args);

  // Listener constructor, syntactic sugar for member function callbacks
  // Example Usage: auto Listener = Event.CreateListener(&instance, &Class::Callback);
  template <typename Class>
  [[nodiscard]] Listener CreateListener(Class* instance, void (Class::*callback_raw)(Args...));
  // Listener constructor
  [[nodiscard]] Listener CreateListener(std::function<void(Args...)> callback);

  // The Listener owns its callback function and manages its scope
  class Listener {
    friend class Event<Args...>;

   public:
    ~Listener();
    void SetEnabled(bool enabled);
    bool IsValid() const;

   private:
    Listener(std::function<void(Args...)> callback, Event<Args...>* event);
    void operator()(Args... args);

    std::function<void(Args...)> callback_;
    bool enabled_ = true;
    Event<Args...>* event_ = nullptr;
  };

 private:
  std::set<Listener*> listeners_;
};

#include "Event_Impl.h"
