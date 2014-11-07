/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014  Regents of the University of California,
 *                     Arizona Board of Regents,
 *                     Colorado State University,
 *                     University Pierre & Marie Curie, Sorbonne University,
 *                     Washington University in St. Louis,
 *                     Beijing Institute of Technology
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef NFD_DAEMON_FACE_UDP_FACE_HPP
#define NFD_DAEMON_FACE_UDP_FACE_HPP

#include "ns3/datagram-face.hpp"
#include "ns3/scheduler.hpp"

namespace nfd {

/**
 * \brief Implementation of Face abstraction that uses UDP
 *        as underlying transport mechanism
 */
class UdpFace : public DatagramFace<boost::asio::ip::udp>
{
public:
  UdpFace(const shared_ptr<protocol::socket>& socket,
          bool isOnDemand,
          const time::seconds& idleTimeout);

  virtual
  ~UdpFace();

  virtual ndn::nfd::FaceStatus
  getFaceStatus() const;

private:
  void
  closeIfIdle();

private:
  EventId m_closeIfIdleEvent;
  time::seconds m_idleTimeout;
  time::steady_clock::TimePoint m_lastIdleCheck;
};

} // namespace nfd

#endif // NFD_DAEMON_FACE_UDP_FACE_HPP
