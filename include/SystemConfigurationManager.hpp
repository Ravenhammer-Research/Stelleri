/**
 * @file SystemConfigurationManager.hpp
 * @brief FreeBSD system configuration implementation
 */

#pragma once

#include "BridgeInterfaceConfig.hpp"
#include "ConfigurationManager.hpp"
#include "InterfaceType.hpp"
#include "LaggConfig.hpp"
#include "VLANConfig.hpp"
#include <vector>

/**
 * @brief FreeBSD-specific configuration manager
 *
 * Implements network configuration using FreeBSD system calls:
 * - Interface creation via SIOCIFCREATE
 * - Address/MTU configuration via ioctls
 * - Route management via PF_ROUTE sockets
 * - Bridge/LAGG/VLAN/Tunnel configuration via specialized ioctls
 *
 * All FreeBSD-specific headers are isolated to the implementation file.
 */
class SystemConfigurationManager : public ConfigurationManager {
public:
  SystemConfigurationManager() = default;
  ~SystemConfigurationManager() override = default;

  void set(const std::string &path, const ConfigData &data) override;
  void delete_config(const std::string &path) override;

protected:
  std::optional<ConfigData>
  getInterface(const std::string &name) const override;
  std::vector<ConfigData> getInterfaces() const override;
  std::vector<ConfigData> getRoutes() const override;

private:
  /** @brief Create a virtual interface using SIOCIFCREATE */
  void createInterface(const std::string &name, InterfaceType type);

  /** @brief Configure bridge members and STP via BRDGADD/BRDGSIFFLGS ioctls */
  void configureBridge(const std::string &name,
                       const BridgeInterfaceConfig &config);

  /** @brief Configure LAGG protocol and members via SIOCSLAGG/SIOCSLAGGPORT */
  void configureLagg(const std::string &name, const LaggConfig &config);

  /** @brief Configure VLAN parent and tag via SIOCSETVLAN */
  void configureVLAN(const std::string &name, const VLANConfig &config);

  /** @brief Configure GIF tunnel endpoints and FIB via
   * SIOCSIFPHYADDR/SIOCSIFFIB */
  void configureGIFTunnel(const std::string &name, const std::string &source,
                          const std::string &destination,
                          const std::optional<int> &tunnel_fib);
};
