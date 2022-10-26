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

#include "fastcdr/Cdr.h"
#include "fastcdr/FastBuffer.h"

#include "rmw/allocators.h"
#include "rmw/error_handling.h"
#include "rmw/rmw.h"

#include "rmw_fastrtps_shared_cpp/rmw_common.hpp"

#include "rmw_fastrtps_cpp/identifier.hpp"
#include "rmw_fastrtps_cpp/get_participant.hpp"

extern "C"
{
rmw_ret_t
rmw_publish(
  const rmw_publisher_t * publisher,
  const void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  return rmw_fastrtps_shared_cpp::__rmw_publish(
    eprosima_fastrtps_identifier, publisher, ros_message, allocation);
}

rmw_ret_t
rmw_publish_serialized_message(
  const rmw_publisher_t * publisher,
  const rmw_serialized_message_t * serialized_message,
  rmw_publisher_allocation_t * allocation)
{
  return rmw_fastrtps_shared_cpp::__rmw_publish_serialized_message(
    eprosima_fastrtps_identifier, publisher, serialized_message, allocation);
}

rmw_ret_t
rmw_publish_loaned_message(
  const rmw_publisher_t * publisher,
  void * ros_message,
  rmw_publisher_allocation_t * allocation)
{
  return rmw_fastrtps_shared_cpp::__rmw_publish_loaned_message(
    eprosima_fastrtps_identifier, publisher, ros_message, allocation);
}

rmw_ret_t
rmw_notify_participant_dynamic_network_interface(rmw_node_t * node)
{
  eprosima::fastdds::dds::DomainParticipant * participant = rmw_fastrtps_cpp::get_domain_participant(node);

  std::cout << "Pre set_qos" << std::endl;
  std::cout << "Name: " << participant->get_qos().name().to_string() << std::endl;
  std::cout << "ID: " << participant->get_qos().wire_protocol().participant_id << std::endl;
  std::cout << "builtin.unicastMetatraffic size: " << participant->get_qos().wire_protocol().builtin.metatrafficUnicastLocatorList() << std::endl;
  std::cout << "builtin.multicastMetatraffic size: " << participant->get_qos().wire_protocol().builtin.metatrafficMulticastLocatorList() << std::endl;
  std::cout << "portBase: " << participant->get_qos().wire_protocol().port.portBase << std::endl;
  std::cout << "domainIdGain: " << participant->get_qos().wire_protocol().port.domainIDGain << std::endl;
  std::cout << "participantIdGain: " << participant->get_qos().wire_protocol().port.participantIDGain << std::endl;
  std::cout << "default unicast list size: " << dpqos.wire_protocol().default_unicast_locator_list.size() << std::endl;
  std::cout << "default multicast list size: " << dpqos.wire_protocol().default_multicast_locator_list.size() << std::endl;
  std::cout << "user transport size: " << dpqos.transport().user_transports.size() << std::endl;

  if (nullptr == participant)
  {
    return RMW_RET_ERROR;
  }

  participant->set_qos(eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);

  //eprosima::fastdds::dds::DomainParticipantQos dpqos = participant->get_qos();

  std::cout << "After set_qos" << std::endl;
  std::cout << "Name: " << participant->get_qos().name().to_string() << std::endl;
  std::cout << "ID: " << participant->get_qos().wire_protocol().participant_id << std::endl;
  std::cout << "builtin.unicastMetatraffic size: " << participant->get_qos().wire_protocol().builtin.metatrafficUnicastLocatorList() << std::endl;
  std::cout << "builtin.multicastMetatraffic size: " << participant->get_qos().wire_protocol().builtin.metatrafficMulticastLocatorList() << std::endl;
  std::cout << "portBase: " << participant->get_qos().wire_protocol().port.portBase << std::endl;
  std::cout << "domainIdGain: " << participant->get_qos().wire_protocol().port.domainIDGain << std::endl;
  std::cout << "participantIdGain: " << participant->get_qos().wire_protocol().port.participantIDGain << std::endl;
  std::cout << "default unicast list size: " << participant->get_qos().wire_protocol().default_unicast_locator_list.size() << std::endl;
  std::cout << "default multicast list size: " << participant->get_qos().wire_protocol().default_multicast_locator_list.size() << std::endl;
  std::cout << "user transport size: " << participant->get_qos().transport().user_transports.size() << std::endl;
  
  return RMW_RET_OK;

}

}  // extern "C"
