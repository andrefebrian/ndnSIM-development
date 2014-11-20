/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
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
 *
 */

#include "ndn-face.h"

#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/assert.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/random-variable.h"
#include "ns3/pointer.h"
#include "ns3/ptr.h"

#include "ns3/ndn-fw-hop-count-tag.h"

#include <boost/ref.hpp>

NS_LOG_COMPONENT_DEFINE ("ndn.Face");

namespace ns3 {
namespace ndn {

using ::ndn::util::FaceUri;
using ::ndn::nfd::FaceEventNotification;
using ::ndn::nfd::FaceStatus;

NS_OBJECT_ENSURE_REGISTERED (Face);

TypeId
Face::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ndn::Face")
    .SetParent<Object> ()
    .SetGroupName ("Ndn")
    .AddAttribute ("Id", "Face id (unique integer for the Ndn stack on this node)",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0),
                   MakeUintegerAccessor (&Face::m_id),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

Face::Face(const FaceUri& remoteUri, const FaceUri& localUri, bool isLocal)
  : m_idNfd(INVALID_FACEID)
  , m_remoteUri(remoteUri)
  , m_localUri(localUri)
  , m_isLocal(isLocal)
  , m_isOnDemand(false)
  , m_isFailed(false)
{
  // GetNode();
  onReceiveInterest += [this](const Interest&) { ++m_counters.getNInInterests(); };
  onReceiveData     += [this](const Data&) {     ++m_counters.getNInDatas(); };
  onSendInterest    += [this](const Interest&) { ++m_counters.getNOutInterests(); };
  onSendData        += [this](const Data&) {     ++m_counters.getNOutDatas(); };
}

FaceId
Face::getId() const
{
  return m_idNfd;
}

// this method is private and should be used only by the FaceTable
void
Face::setId(FaceId faceId)
{
  m_idNfd = faceId;
}

void
Face::setDescription(const std::string& description)
{
  m_description = description;
}

const std::string&
Face::getDescription() const
{
  return m_description;
}

bool
Face::isMultiAccess() const
{
  return false;
}

bool
Face::isUp() const
{
  return true;
}

bool
Face::decodeAndDispatchInput(const Block& element)
{
  try {
    /// \todo Ensure lazy field decoding process

    if (element.type() == ::ndn::tlv::Interest)
      {
        shared_ptr<Interest> i = make_shared<Interest>();
        i->wireDecode(element);
        this->onReceiveInterest(*i);
      }
    else if (element.type() == ::ndn::tlv::Data)
      {
        shared_ptr<Data> d = make_shared<Data>();
        d->wireDecode(element);
        this->onReceiveData(*d);
      }
    else
      return false;

    return true;
  }
  catch (::ndn::tlv::Error&) {
    return false;
  }
}

void
Face::fail(const std::string& reason)
{
  if (m_isFailed) {
    return;
  }

  m_isFailed = true;
  this->onFail(reason);

  this->onFail.clear();
}

void
Face::close ()
{
}

template<typename FaceTraits>
void
Face::copyStatusTo(FaceTraits& traits) const
{
  traits.setFaceId(getId())
    .setRemoteUri(getRemoteUri().toString())
    .setLocalUri(getLocalUri().toString());

  if (isLocal()) {
    traits.setFaceScope(::ndn::nfd::FACE_SCOPE_LOCAL);
  }
  else {
    traits.setFaceScope(::ndn::nfd::FACE_SCOPE_NON_LOCAL);
  }

  if (isOnDemand()) {
    traits.setFacePersistency(::ndn::nfd::FACE_PERSISTENCY_ON_DEMAND);
  }
  else {
    traits.setFacePersistency(::ndn::nfd::FACE_PERSISTENCY_PERSISTENT);
  }
}

template void
Face::copyStatusTo<FaceStatus>(FaceStatus&) const;

template void
Face::copyStatusTo<FaceEventNotification>(FaceEventNotification&) const;

FaceStatus
Face::getFaceStatus() const
{
  FaceStatus status;
  copyStatusTo(status);

  this->getCounters().copyTo(status);

  return status;
}

/**
 * By default, Ndn face are created in the "down" state
 *  with no IP addresses.  Before becoming useable, the user must
 * invoke SetUp on them once an Ndn address and mask have been set.
 */
Face::Face (Ptr<Node> node)
  : m_idNfd(INVALID_FACEID)
  , m_remoteUri("ns3://face")
  , m_localUri("ns3://face")
  , m_node (node)
  , m_ifup (false)
  , m_id ((uint32_t)-1)
  , m_metric (0)
  , m_flags (0)
    /*
  , m_idNfd (INVALID_FACEID)
  , m_remoteUri(0)
  , m_localUri(0)
  , m_isLocal(false) //only App face should be local */
{
  NS_LOG_FUNCTION (this << node);

  NS_ASSERT_MSG (node != 0, "node cannot be NULL. Check the code");
}
/*
Face::Face (nfd::FaceUri remoteUri, nfd::FaceUri localUri, bool isLocal)
  : m_node (0) // TO DO: Do we need to know the node number?
  , m_upstreamInterestHandler (MakeNullCallback< void, Ptr<Face>, ::ndn::shared_ptr< ::ndn::Interest> > ())
  , m_upstreamDataHandler (MakeNullCallback< void, Ptr<Face>, ::ndn::shared_ptr< ::ndn::Data> > ())
  , m_ifup (false)
  , m_id ((uint32_t)-1)
  , m_metric (0)
  , m_flags (0)
  , m_idNfd (INVALID_FACEID)
  , m_remoteUri(0)
  , m_localUri(0)
  , m_isLocal(isLocal)
{
  onReceiveInterest += [this](const ::ndn::Interest&) { ++m_counters.getNInInterests(); };
  onReceiveData     += [this](const ::ndn::Data&) {     ++m_counters.getNInDatas(); };
  onSendInterest    += [this](const ::ndn::Interest&) { ++m_counters.getNOutInterests(); };
  onSendData        += [this](const ::ndn::Data&) {     ++m_counters.getNOutDatas(); };
}

Face::Face (const Face &)
{
  onReceiveInterest += [this](const ::ndn::Interest&) { ++m_counters.getNInInterests(); };
  onReceiveData     += [this](const ::ndn::Data&) {     ++m_counters.getNInDatas(); };
  onSendInterest    += [this](const ::ndn::Interest&) { ++m_counters.getNOutInterests(); };
  onSendData        += [this](const ::ndn::Data&) {     ++m_counters.getNOutDatas(); };
}

*/
Face::~Face ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Face& Face::operator= (const Face &)
{
  return *this;
}

Ptr<Node>
Face::GetNode () const
{
  return m_node;
}

bool
Face::SendInterest (shared_ptr<const Interest> interest)
{
  NS_LOG_FUNCTION (this << boost::cref (*this) << interest->getName ());

  if (!IsUp ())
    {
      return false;
    }
  // I assume that this should work...

  Ptr<Packet> packet = Create <Packet> ();
  Block block = interest->wireEncode ();
  Convert::Convert::InterestToPacket (make_shared <Block> (block), packet);
  return Send (packet);

  //return Send (Wire::FromInterest (interest));
}

bool
Face::SendData (shared_ptr<const Data> data)
{
  NS_LOG_FUNCTION (this << data);

  if (!IsUp ())
    {
      return false;
    }
  // I assume that this should work..

  Ptr<Packet> packet = Create <Packet> ();
  Block block = data->wireEncode ();
  Convert::Convert::InterestToPacket (make_shared <Block> (block), packet);
  return Send (packet);

  //return Send (Wire::FromData (data));
}

bool
Face::Send (Ptr<Packet> packet)
{
  FwHopCountTag hopCount;
  bool tagExists = packet->RemovePacketTag (hopCount);
  if (tagExists)
    {
      hopCount.Increment ();
      packet->AddPacketTag (hopCount);
    }

  return true;
}

bool
Face::Receive (Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (this << p << p->GetSize ());

  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }
  Ptr<Packet> packet = p->Copy (); // give upper layers a rw copy of the packet
  try
    {
      //Let's see..
      Block block = Convert::Convert::FromPacket (packet);
      decodeAndDispatchInput(block);

     //  uint32_t type = block.type();
     //  if (type == 0x05) {
     //    Interest interest;
     //    interest.wireDecode (block);
     //    ReceiveInterest (make_shared <Interest> (interest));
     //  }
     // else
     //   if (type == 0x06) {
     //     Data data;
     //     data.wireDecode (block);
     //     ReceiveData (make_shared <Data> (data));
     //   }
    }
  catch (::ndn::UnknownHeaderException)
    {
      NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
      return false;
    }

  return false;
}

bool
Face::ReceiveInterest (shared_ptr<Interest> interest)
{
  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }

  // m_upstreamInterestHandler (this, interest);
  return true;
}

bool
Face::ReceiveData (shared_ptr<Data> data)
{
  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }

  // m_upstreamDataHandler (this, data);
  return true;
}

void
Face::SetMetric (uint16_t metric)
{
  NS_LOG_FUNCTION (metric);
  m_metric = metric;
}

uint16_t
Face::GetMetric (void) const
{
  return m_metric;
}

void
Face::SetFlags (uint32_t flags)
{
  m_flags = flags;
}

bool
Face::operator== (const Face &face) const
{
  NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
                 "Faces of different nodes should not be compared to each other: " << *this << " == " << face);

  return (m_id == face.m_id);
}

bool
Face::operator< (const Face &face) const
{
  NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
                 "Faces of different nodes should not be compared to each other: " << *this << " == " << face);

  return (m_id < face.m_id);
}

std::ostream&
Face::Print (std::ostream &os) const
{
  os << "id=" << GetId ();
  return os;
}

std::ostream&
operator<< (std::ostream& os, const Face &face)
{
  face.Print (os);
  return os;
}

} // namespace ndn
} // namespace ns3
