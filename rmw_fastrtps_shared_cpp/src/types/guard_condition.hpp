// Copyright 2016-2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TYPES__GUARD_CONDITION_HPP_
#define TYPES__GUARD_CONDITION_HPP_

#include <array>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <utility>

#include "rcutils/event_types.h"
#include "rcpputils/thread_safety_annotations.hpp"

class GuardCondition
{
public:
  GuardCondition()
  : hasTriggered_(false),
    conditionMutex_(nullptr), conditionVariable_(nullptr) {}

  void
  trigger()
  {
    std::lock_guard<std::mutex> lock(internalMutex_);

    if (conditionMutex_ != nullptr) {
      std::unique_lock<std::mutex> clock(*conditionMutex_);
      // the change to hasTriggered_ needs to be mutually exclusive with
      // rmw_wait() which checks hasTriggered() and decides if wait() needs to
      // be called
      hasTriggered_ = true;
      clock.unlock();
      conditionVariable_->notify_one();
    } else {
      hasTriggered_ = true;
    }

    if(hook_set_) {
      for(uint64_t i = 0; i <= unread_count_; i++) {
        event_handle_.callback(event_handle_.context, { event_handle_.ros2_handle, GUARD_CONDITION_EVENT });
      }
      unread_count_ = 0;
    } else {
      unread_count_++;
    }
  }

  void
  attachCondition(std::mutex * conditionMutex, std::condition_variable * conditionVariable)
  {
    std::lock_guard<std::mutex> lock(internalMutex_);
    conditionMutex_ = conditionMutex;
    conditionVariable_ = conditionVariable;
  }

  void
  detachCondition()
  {
    std::lock_guard<std::mutex> lock(internalMutex_);
    conditionMutex_ = nullptr;
    conditionVariable_ = nullptr;
  }

  bool
  hasTriggered()
  {
    return hasTriggered_;
  }

  bool
  getHasTriggered()
  {
    return hasTriggered_.exchange(false);
  }

  // Provide handlers to perform an action when a
  // new event from this listener has ocurred
  void
  setCallback(
    const void * executor_context,
    Event_callback callback,
    const void * guard_condition_handle)
  {
    event_handle_ = {executor_context, guard_condition_handle, callback};
    hook_set_ = true;
  }

private:
  std::mutex internalMutex_;
  std::atomic_bool hasTriggered_;
  std::mutex * conditionMutex_ RCPPUTILS_TSA_GUARDED_BY(internalMutex_);
  std::condition_variable * conditionVariable_ RCPPUTILS_TSA_GUARDED_BY(internalMutex_);

  EventHandle event_handle_{nullptr, nullptr, nullptr};
  std::atomic_bool hook_set_{false};
  uint64_t unread_count_ = 0;
};

#endif  // TYPES__GUARD_CONDITION_HPP_
