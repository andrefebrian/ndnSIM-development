/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011,2012 University of California, Los Angeles
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
 *         Ilya Moiseenko <iliamo@cs.ucla.edu>
 */

#ifndef NDN_CONTENT_STORE_H
#define	NDN_CONTENT_STORE_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/ndn-common.h"

#include <boost/tuple/tuple.hpp>

namespace ns3 {

class Packet;

namespace ndn {

class ContentStore;

/**
 * @ingroup ndn
 * @defgroup ndn-cs Content Store
 */

/**
 * @ingroup ndn-cs
 * @brief Namespace for ContentStore operations
 */
namespace cs {

/**
 * @ingroup ndn-cs
 * @brief NDN content store entry
 */
class Entry : public SimpleRefCount<Entry>
{
public:
  /**
   * \brief Construct content store entry
   *
   * \param header Parsed Data header
   * \param packet Original Ndn packet
   *
   * The constructor will make a copy of the supplied packet and calls
   * RemoveHeader and RemoveTail on the copy.
   */
  Entry (Ptr<ContentStore> cs, shared_ptr<const Data> data);

  /**
   * \brief Get prefix of the stored entry
   * \returns prefix of the stored entry
   */
  const Name&
  GetName () const;

  /**
   * \brief Get Data of the stored entry
   * \returns Data of the stored entry
   */
  shared_ptr<const Data>
  GetData () const;

  /**
   * @brief Get pointer to access store, to which this entry is added
   */
  Ptr<ContentStore>
  GetContentStore ();

private:
  Ptr<ContentStore> m_cs; ///< \brief content store to which entry is added
  shared_ptr<const Data> m_data; ///< \brief non-modifiable Data
};

} // namespace cs


/**
 * @ingroup ndn-cs
 * \brief Base class for NDN content store
 *
 * Particular implementations should implement Lookup, Add, and Print methods
 */
class ContentStore : public Object
{
public:
  /**
   * \brief Interface ID
   *
   * \return interface ID
   */
  static
  TypeId GetTypeId ();

  /**
   * @brief Virtual destructor
   */
  virtual
  ~ContentStore ();

  /**
   * \brief Find corresponding CS entry for the given interest
   *
   * \param interest Interest for which matching content store entry
   * will be searched
   *
   * If an entry is found, it is promoted to the top of most recent
   * used entries index, \see m_contentStore
   */
  virtual shared_ptr<Data>
  Lookup (shared_ptr<const Interest> interest) = 0;

  /**
   * \brief Add a new content to the content store.
   *
   * \param header Fully parsed Data
   * \param packet Fully formed Ndn packet to add to content store
   * (will be copied and stripped down of headers)
   * @returns true if an existing entry was updated, false otherwise
   */
  virtual bool
  Add (shared_ptr<const Data> data) = 0;

  // /*
  //  * \brief Add a new content to the content store.
  //  *
  //  * \param header Interest header for which an entry should be removed
  //  * @returns true if an existing entry was removed, false otherwise
  //  */
  // virtual bool
  // Remove (Ptr<Interest> header) = 0;

  /**
   * \brief Print out content store entries
   */
  virtual void
  Print (std::ostream &os) const = 0;


  /**
   * @brief Get number of entries in content store
   */
  virtual uint32_t
  GetSize () const = 0;

  /**
   * @brief Return first element of content store (no order guaranteed)
   */
  virtual Ptr<cs::Entry>
  Begin () = 0;

  /**
   * @brief Return item next after last (no order guaranteed)
   */
  virtual Ptr<cs::Entry>
  End () = 0;

  /**
   * @brief Advance the iterator
   */
  virtual Ptr<cs::Entry>
  Next (Ptr<cs::Entry>) = 0;

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Static call to cheat python bindings
   */
  static inline Ptr<ContentStore>
  GetContentStore (Ptr<Object> node);

protected:
  TracedCallback<shared_ptr<const Interest>,
                 shared_ptr<const Data> > m_cacheHitsTrace; ///< @brief trace of cache hits

  TracedCallback<shared_ptr<const Interest> > m_cacheMissesTrace; ///< @brief trace of cache misses
};

inline std::ostream&
operator<< (std::ostream &os, const ContentStore &cs)
{
  cs.Print (os);
  return os;
}

inline Ptr<ContentStore>
ContentStore::GetContentStore (Ptr<Object> node)
{
  return node->GetObject<ContentStore> ();
}


} // namespace ndn
} // namespace ns3

#include <boost/functional/hash.hpp>
namespace boost
{
inline std::size_t
hash_value (const ::ndn::name::Component component)
{
  return boost::hash_range (component.wireEncode().wire(),
                            component.wireEncode().wire() + component.wireEncode().size());
}
}

#endif // NDN_CONTENT_STORE_H
