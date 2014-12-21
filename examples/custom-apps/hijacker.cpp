/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2014  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// hijacker.cpp

#include "hijacker.h"
#include "ns3/ndn-fib-helper.hpp"

NS_LOG_COMPONENT_DEFINE("Hijacker");

namespace ns3 {

using nfd::ControlParameters;

// Necessary if you are planning to use ndn::AppHelper
NS_OBJECT_ENSURE_REGISTERED(Hijacker);

TypeId
Hijacker::GetTypeId()
{
  static TypeId tid = TypeId("Hijacker").SetParent<ndn::App>().AddConstructor<Hijacker>();

  return tid;
}

Hijacker::Hijacker()
{
}

void
Hijacker::OnInterest(shared_ptr<const Interest> interest)
{
  ndn::App::OnInterest(interest); // forward call to perform app-level tracing
  // do nothing else (hijack interest)

  NS_LOG_DEBUG("Do nothing for incoming interest for" << interest->getName());
}

void
Hijacker::StartApplication()
{
  App::StartApplication();

  // equivalent to setting interest filter for "/" prefix
  ControlParameters parameters;
  parameters.setName("/");
  parameters.setFaceId(m_face->getId());
  parameters.setCost(0);

  ndn::FibHelper fibHelper;
  Ptr<Node> node = GetNode();
  fibHelper.AddNextHop(parameters, node);
}

void
Hijacker::StopApplication()
{
  App::StopApplication();
}

} // namespace ns3
