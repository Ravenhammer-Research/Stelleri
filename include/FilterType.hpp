/*
 * FilterType.hpp
 * Simple enum describing NETCONF filter types used by the server.
 */

#pragma once

#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

// Minimal filter type enum. Add more values if needed later.
enum FilterType { None = 0, SubTree };
