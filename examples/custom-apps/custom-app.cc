/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2012 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

// custom-app.cc

#include "custom-app.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"

#include "ns3/ndn-fib-helper.h"
#include "ns3/ndn-forwarder.h"
#include "ns3/ndn-app-face.h"

#include "ns3/random-variable.h"

NS_LOG_COMPONENT_DEFINE ("CustomApp");

namespace ns3 {

using nfd::ControlParameters;
using ndn::FibHelper;

NS_OBJECT_ENSURE_REGISTERED (CustomApp);

// register NS-3 type
TypeId
CustomApp::GetTypeId ()
{
  static TypeId tid = TypeId ("CustomApp")
    .SetParent<ndn::App> ()
    .AddConstructor<CustomApp> ()
    ;
  return tid;
}

CustomApp::CustomApp ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

// Processing upon start of the application
void
CustomApp::StartApplication ()
{
  // initialize ndn::App
  ndn::App::StartApplication ();

  Simulator::Schedule (Seconds (1.0), &CustomApp::SendInterest, this);
}

// Processing when application is stopped
void
CustomApp::StopApplication ()
{
  // cleanup ndn::App
  ndn::App::StopApplication ();
}

void
CustomApp::SendInterest ()
{
  /////////////////////////////////////
  // Sending one Interest packet out //
  /////////////////////////////////////

  shared_ptr<Name> prefix = make_shared<Name> ("/prefix/sub"); // another way to create name

  // Create and configure ndn::Interest
  shared_ptr<Interest> interest = make_shared<Interest> ();
  UniformVariable rand (0,std::numeric_limits<uint32_t>::max ());
  interest->setNonce            (rand.GetValue ());
  interest->setName             (*prefix);
  interest->setInterestLifetime (::ndn::time::seconds (uint64_t (1.0)));

  NS_LOG_DEBUG ("Sending Interest packet for " << *prefix);

  // Call trace (for logging purposes)
  m_transmittedInterests (interest, this, m_face);

  m_face->onReceiveInterest (*interest);
}

// Callback that will be called when Data arrives
void
CustomApp::OnData (shared_ptr<const Data> contentObject)
{
  NS_LOG_DEBUG ("Receiving Data packet for " << contentObject->getName ());

  std::cout << "DATA received for name " << contentObject->getName () << std::endl;
}

} // namespace ns3
