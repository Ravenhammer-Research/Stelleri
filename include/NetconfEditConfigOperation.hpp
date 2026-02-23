/*
 * NetconfEditConfigOperation.hpp
 * Representation of an edit-config operation for NETCONF
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

// This header used to define a class wrapper; the project expects a
// simple enum describing the edit-config operation.

enum class NetconfEditConfigOperation {
  Merge,
  Replace,
  Create,
  Delete,
  Remove
};
