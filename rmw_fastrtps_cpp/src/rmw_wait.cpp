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

#include "rmw/error_handling.h"
#include "rmw/rmw.h"

#include "rmw_fastrtps_shared_cpp/rmw_common.hpp"
#include "rmw_fastrtps_cpp/identifier.hpp"

extern "C"
{
rmw_ret_t
rmw_wait(
  rmw_subscriptions_t * subscriptions,
  rmw_guard_conditions_t * guard_conditions,
  rmw_services_t * services,
  rmw_clients_t * clients,
  rmw_events_t * events,
  rmw_wait_set_t * wait_set,
  const rmw_time_t * wait_timeout)
{
  return rmw_fastrtps_shared_cpp::__rmw_wait(
    eprosima_fastrtps_identifier, subscriptions, guard_conditions, services, clients, events,
    wait_set, wait_timeout);
}

rmw_ret_t
rmw_set_subscription_callback(
  const void * executor_context,
  Event_callback callback,
  const void * subscription_handle,
  void * rmw_subscription)
{
  return rmw_fastrtps_shared_cpp::__rmw_set_subscription_callback(
    executor_context,
    callback,
    subscription_handle,
    rmw_subscription);
}

rmw_ret_t
rmw_set_service_callback(
  const void * executor_context,
  Event_callback callback,
  const void * service_handle,
  void * rmw_service)
{
  return rmw_fastrtps_shared_cpp::__rmw_set_service_callback(
    executor_context,
    callback,
    service_handle,
    rmw_service);
}

rmw_ret_t
rmw_set_client_callback(
  const void * executor_context,
  Event_callback callback,
  const void * client_handle,
  void * rmw_client)
{
  return rmw_fastrtps_shared_cpp::__rmw_set_client_callback(
    executor_context,
    callback,
    client_handle,
    rmw_client);
}

rmw_ret_t
rmw_set_guard_condition_callback(
  const void * executor_context,
  Event_callback callback,
  const void * guard_condition_handle,
  void * rmw_guard_condition)
{
  return rmw_fastrtps_shared_cpp::__rmw_set_guard_condition_callback(
    executor_context,
    callback,
    guard_condition_handle,
    rmw_guard_condition);
}

}  // extern "C"
