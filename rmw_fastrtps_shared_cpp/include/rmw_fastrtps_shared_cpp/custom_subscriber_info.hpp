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

#ifndef RMW_FASTRTPS_SHARED_CPP__CUSTOM_SUBSCRIBER_INFO_HPP_
#define RMW_FASTRTPS_SHARED_CPP__CUSTOM_SUBSCRIBER_INFO_HPP_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <set>
#include <utility>

#include "fastrtps/subscriber/Subscriber.h"
#include "fastrtps/subscriber/SubscriberListener.h"

#include "rcutils/event_types.h"
#include "rcpputils/thread_safety_annotations.hpp"

#include "rmw/impl/cpp/macros.hpp"

#include "rmw_fastrtps_shared_cpp/TypeSupport.hpp"
#include "rmw_fastrtps_shared_cpp/custom_event_info.hpp"


class SubListener;

struct CustomSubscriberInfo : public CustomEventInfo
{
  virtual ~CustomSubscriberInfo() = default;

  eprosima::fastrtps::Subscriber * subscriber_{nullptr};
  SubListener * listener_{nullptr};
  rmw_fastrtps_shared_cpp::TypeSupport * type_support_{nullptr};
  const void * type_support_impl_{nullptr};
  rmw_gid_t subscription_gid_{};
  const char * typesupport_identifier_{nullptr};

  RMW_FASTRTPS_SHARED_CPP_PUBLIC
  EventListenerInterface *
  getListener() const final;
};

class SubListener : public EventListenerInterface, public eprosima::fastrtps::SubscriberListener
{
public:
  explicit SubListener(CustomSubscriberInfo * info)
  : data_(0),
    deadline_changes_(false),
    liveliness_changes_(false),
    conditionMutex_(nullptr),
    conditionVariable_(nullptr)
  {
    // Field is not used right now
    (void)info;
  }

  // SubscriberListener implementation
  void
  onSubscriptionMatched(
    eprosima::fastrtps::Subscriber * sub, eprosima::fastrtps::rtps::MatchingInfo & info) final
  {
    {
      std::lock_guard<std::mutex> lock(internalMutex_);
      if (eprosima::fastrtps::rtps::MATCHED_MATCHING == info.status) {
        publishers_.insert(info.remoteEndpointGuid);
      } else if (eprosima::fastrtps::rtps::REMOVED_MATCHING == info.status) {
        publishers_.erase(info.remoteEndpointGuid);
      }
    }
    update_unread_count(sub);
  }

  void
  onNewDataMessage(eprosima::fastrtps::Subscriber * sub) final
  {
    update_unread_count(sub);

    // Add the service to the event queue
    // Check if we can comment the above `update_unread_count`
    // when using the events executor, to avoid the locking of the mutex.
    if(hook_set_) {
      for(uint64_t i = 0; i <= unread_count_; i++) {
        event_handle_.callback(event_handle_.context, { event_handle_.ros2_handle, SUBSCRIPTION_EVENT });
      }
      unread_count_ = 0;
    } else {
      unread_count_++;
    }
  }

  RMW_FASTRTPS_SHARED_CPP_PUBLIC
  void
  on_requested_deadline_missed(
    eprosima::fastrtps::Subscriber *,
    const eprosima::fastrtps::RequestedDeadlineMissedStatus &) final;

  RMW_FASTRTPS_SHARED_CPP_PUBLIC
  void
  on_liveliness_changed(
    eprosima::fastrtps::Subscriber *,
    const eprosima::fastrtps::LivelinessChangedStatus &) final;

  // EventListenerInterface implementation
  RMW_FASTRTPS_SHARED_CPP_PUBLIC
  bool
  hasEvent(rmw_event_type_t event_type) const final;

  RMW_FASTRTPS_SHARED_CPP_PUBLIC
  bool
  takeNextEvent(rmw_event_type_t event_type, void * event_info) final;

  // SubListener API
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
  hasData() const
  {
    return data_.load(std::memory_order_relaxed) > 0;
  }

  void
  update_unread_count(eprosima::fastrtps::Subscriber * sub)
  {
    // Make sure to call into Fast-RTPS before taking the lock to avoid an
    // ABBA deadlock between internalMutex_ and mutexes inside of Fast-RTPS.
#if FASTRTPS_VERSION_MAJOR == 1 && FASTRTPS_VERSION_MINOR < 9
    uint64_t unread_count = sub->getUnreadCount();
#else
    uint64_t unread_count = sub->get_unread_count();
#endif

    std::lock_guard<std::mutex> lock(internalMutex_);
    ConditionalScopedLock clock(conditionMutex_, conditionVariable_);
    data_.store(unread_count, std::memory_order_relaxed);
  }

  size_t publisherCount()
  {
    std::lock_guard<std::mutex> lock(internalMutex_);
    return publishers_.size();
  }

  // Provide handlers to perform an action when a
  // new event from this listener has ocurred
  void
  setCallback(
    const void * executor_context,
    Event_callback callback,
    const void * subscription_handle)
  {
    event_handle_ = {executor_context, subscription_handle, callback};
    hook_set_ = true;
  }

private:
  mutable std::mutex internalMutex_;

  std::atomic_size_t data_;

  std::atomic_bool deadline_changes_;
  eprosima::fastrtps::RequestedDeadlineMissedStatus requested_deadline_missed_status_
    RCPPUTILS_TSA_GUARDED_BY(internalMutex_);

  std::atomic_bool liveliness_changes_;
  eprosima::fastrtps::LivelinessChangedStatus liveliness_changed_status_
    RCPPUTILS_TSA_GUARDED_BY(internalMutex_);

  std::mutex * conditionMutex_ RCPPUTILS_TSA_GUARDED_BY(internalMutex_);
  std::condition_variable * conditionVariable_ RCPPUTILS_TSA_GUARDED_BY(internalMutex_);

  std::set<eprosima::fastrtps::rtps::GUID_t> publishers_ RCPPUTILS_TSA_GUARDED_BY(internalMutex_);

  EventHandle event_handle_{nullptr, nullptr, nullptr};
  std::atomic_bool hook_set_;
  uint64_t unread_count_ = 0;
};

#endif  // RMW_FASTRTPS_SHARED_CPP__CUSTOM_SUBSCRIBER_INFO_HPP_
