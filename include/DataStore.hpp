#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

/*
 * DataStore.hpp
 * Representation of a NETCONF/YANG datastore as an enum.
 */

#pragma once

// Use an enum class to represent supported datastores. Callers can use
// `running()` to obtain the canonical running datastore value.
enum class DataStore { Running, Candidate };
// No helper functions are provided here by design.
