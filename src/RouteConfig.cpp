/*
 * RouteConfig system operations (destroy)
 */

#include "RouteConfig.hpp"
#include "SystemConfigurationManager.hpp"

void RouteConfig::save() const { SystemConfigurationManager scm; scm.AddRoute(*this); }

void RouteConfig::destroy() const { SystemConfigurationManager scm; scm.DeleteRoute(*this); }
