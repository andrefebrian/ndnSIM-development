/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014,  Regents of the University of California,
 *                      Arizona Board of Regents,
 *                      Colorado State University,
 *                      University Pierre & Marie Curie, Sorbonne University,
 *                      Washington University in St. Louis,
 *                      Beijing Institute of Technology,
 *                      The University of Memphis
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
 */

#ifndef NFD_COMMON_HPP
#define NFD_COMMON_HPP

// #include "config.hpp"

#ifdef WITH_TESTS
#define VIRTUAL_WITH_TESTS virtual
#define PUBLIC_WITH_TESTS_ELSE_PROTECTED public
#define PUBLIC_WITH_TESTS_ELSE_PRIVATE public
#define PROTECTED_WITH_TESTS_ELSE_PRIVATE protected
#else
#define VIRTUAL_WITH_TESTS
#define PUBLIC_WITH_TESTS_ELSE_PROTECTED protected
#define PUBLIC_WITH_TESTS_ELSE_PRIVATE private
#define PROTECTED_WITH_TESTS_ELSE_PRIVATE private
#endif

#include <cstddef>
#include <list>
#include <set>
#include <queue>
#include <vector>

#include <ndn-cxx/common.hpp>
#include <ndn-cxx/interest.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/util/event-emitter.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/scoped_ptr.hpp>

namespace nfd {

using std::size_t;

using boost::noncopyable;
using boost::scoped_ptr;

using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::bad_weak_ptr;
using std::make_shared;
using std::enable_shared_from_this;

using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::const_pointer_cast;

using std::function;
using std::bind;
using std::ref;
using std::cref;

using ndn::Interest;
using ndn::Data;
using ndn::Name;
using ndn::Exclude;
using ndn::Block;
using ndn::util::EventEmitter;

namespace tlv {
// Don't write "namespace tlv = ndn::tlv", because NFD can add other members into this namespace.
using namespace ndn::tlv;
} // namespace tlv

namespace name = ndn::name;
namespace time = ndn::time;

} // namespace nfd

#endif // NFD_COMMON_HPP