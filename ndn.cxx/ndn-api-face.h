/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2013 University of California, Los Angeles
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
 *         Zhenkai Zhu <zhenkai@cs.ucla.edu>
 *         Chaoyi Bian <bcy@pku.edu.cn>
 */

#ifndef NDN_API_FACE_H
#define NDN_API_FACE_H

#include <ns3/ptr.h>
#include <ns3/node.h>
#include <ns3/callback.h>
#include <ns3/ndn-face.h>
#include <ns3/ndn-name.h>

namespace ns3 {
namespace ndn {

class ApiFacePriv;

/**
 * \ingroup sync
 * @brief A handler for NDN; clients of this code do not need to deal
 * with NDN API directly
 */
class ApiFace
  : public ns3::ndn::Face
{
public:
  typedef Callback<void, Ptr<const Name>,     Ptr<const Interest> > InterestCallback;
  typedef Callback<void, Ptr<const Interest>, Ptr<const ContentObject> > DataCallback;
  typedef Callback<void, Ptr<const Interest> > TimeoutCallback;

  /**
   * @brief initialize the handler; a lot of things needs to be done. 1) init
   * keystore 2) init keylocator 3) start a thread to hold a loop of ccn_run
   *
   */
  ApiFace (Ptr<Node> node);
  ~ApiFace ();

  /**
   * @brief Express Interest
   *
   * @param name the Interest name
   * @param onData the callback function to deal with the returned data
   * @param onTimeout the callback function to deal with timeouts
   */
  void
  ExpressInterest (Ptr<Interest> interest,
                   DataCallback onData,
                   TimeoutCallback onTimeout); // = MakeNullCallback< void, Ptr<Interest> > ()

  /**
   * @brief set Interest filter (specify what interest you want to receive)
   *
   * @param prefix the prefix of Interest
   * @param onInterest the callback function to deal with the returned data
   */
  void
  SetInterestFilter (Ptr<const Name> prefix, InterestCallback onInterest);

  /**
   * @brief clear Interest filter
   * @param prefix the prefix of Interest
   */
  void
  ClearInterestFilter (Ptr<const Name> prefix);

  /**
   * @brief Publish data
   * @param data Data packet to publish
   */
  void
  Put (Ptr<ContentObject> data);

public:
  virtual bool
  SendInterest (Ptr<const Interest> interest);

  virtual bool
  SendData (Ptr<const ContentObject> data);

  virtual std::ostream&
  Print (std::ostream &os) const;

private:
  ApiFace () : Face (0) {}
  ApiFace (const ApiFace &) : Face (0) {}
  ApiFace& operator= (const ApiFace &) { return *this; }

private:
  ApiFacePriv *m_this;
};

}
}

#endif // NDN_API_HANDLER_H
