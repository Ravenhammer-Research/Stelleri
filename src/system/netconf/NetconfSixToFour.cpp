#if !defined(STELLERI_NETCONF) || STELLERI_NETCONF != 1
#error "netconf headers are for the STELLERI_NETCONF build only"
#endif

#include "Client.hpp"
#include "IetfInterfaces.hpp"
#include "NetconfConfigurationManager.hpp"

void NetconfConfigurationManager::CreateSixToFour(
    const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // 6to4 interfaces are created using the standard ietf-interfaces model
  // with type set to iana-if-type:tunnel (131)
  // Note: 6to4 is a tunneling mechanism for IPv6 over IPv4
  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  struct lyd_node *node = nullptr;

  const struct lys_module *mod =
      ly_ctx_get_module_implemented(yctx.get(), "ietf-interfaces");
  if (!mod)
    mod = ly_ctx_load_module(const_cast<struct ly_ctx *>(yctx.get()),
                             "ietf-interfaces", nullptr, nullptr);
  if (!mod)
    throw std::runtime_error("ietf-interfaces module not found");

  // Create the sixtofour interface entry
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(nullptr, yctx.get(), iface_path.c_str(), nullptr, 0,
                   &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Set type to iana-if-type:tunnel
  std::string type_path = std::format(
      "/ietf-interfaces:interfaces/interface[name='{}']/type", name);
  if (lyd_new_path(nullptr, yctx.get(), type_path.c_str(),
                   "iana-if-type:tunnel", 0, nullptr) != LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="create" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "create", false,
                   &meta) != LY_SUCCESS)
    throw YangError(yctx);

  // Create container and insert node
  struct lyd_node *iface_data = nullptr;
  if (lyd_new_inner(nullptr, mod, "interfaces", 0, &iface_data) != LY_SUCCESS)
    throw YangError(yctx);
  lyd_insert_child(iface_data, node);

  YangData data(iface_data);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Create);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
}

void NetconfConfigurationManager::SaveSixToFour(
    const SixToFourInterfaceConfig &t) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Convert to IetfInterfaces for the base interface config
  // Note: 6to4-specific settings would require a custom YANG model
  IetfInterfaces ii(t);
  struct lyd_node *node = ii.toLydNode(c->session().yangContext());
  if (!node)
    throw std::runtime_error(
        "Failed to convert SixToFourInterfaceConfig to YANG node");

  YangData data(node);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Merge);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr,
                       c->session().yangContext());
}

void NetconfConfigurationManager::DestroySixToFour(
    const std::string &name) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Delete the sixtofour interface using ietf-interfaces model
  YangContext yctx = c->session().yangContext();
  if (!yctx.get())
    throw YangError(yctx);

  struct lyd_node *node = nullptr;

  const struct lys_module *mod =
      ly_ctx_get_module_implemented(yctx.get(), "ietf-interfaces");
  if (!mod)
    mod = ly_ctx_load_module(const_cast<struct ly_ctx *>(yctx.get()),
                             "ietf-interfaces", nullptr, nullptr);
  if (!mod)
    throw std::runtime_error("ietf-interfaces module not found");

  // Create the interface entry with delete operation
  std::string iface_path =
      std::format("/ietf-interfaces:interfaces/interface[name='{}']", name);
  if (lyd_new_path(nullptr, yctx.get(), iface_path.c_str(), nullptr, 0,
                   &node) != LY_SUCCESS)
    throw YangError(yctx);

  // Set type to iana-if-type:tunnel
  std::string type_path = std::format(
      "/ietf-interfaces:interfaces/interface[name='{}']/type", name);
  if (lyd_new_path(nullptr, yctx.get(), type_path.c_str(),
                   "iana-if-type:tunnel", 0, nullptr) != LY_SUCCESS)
    throw YangError(yctx);

  // Add operation="delete" metadata
  struct lyd_meta *meta = nullptr;
  if (lyd_new_meta(nullptr, node, "ietf-netconf:operation", "delete", false,
                   &meta) != LY_SUCCESS)
    throw YangError(yctx);

  // Create container and insert node
  struct lyd_node *iface_data = nullptr;
  if (lyd_new_inner(nullptr, mod, "interfaces", 0, &iface_data) != LY_SUCCESS)
    throw YangError(yctx);
  lyd_insert_child(iface_data, node);

  YangData data(iface_data);
  std::unique_ptr<NetconfServerReply> reply =
      c->editConfig(data, NetconfEditConfigOperation::Delete);

  if (!reply || reply->isError())
    throw NetconfError(reply ? reply->toNcServerReply() : nullptr, yctx);
}

std::vector<SixToFourInterfaceConfig>
NetconfConfigurationManager::GetSixToFourInterfaces(
    const std::vector<InterfaceConfig> &bases) const {
  Client *c = Client::instance();
  if (!c)
    throw NetconfError(nullptr, YangContext());

  // Filter bases to only sixtofour interfaces
  std::vector<SixToFourInterfaceConfig> out;
  for (const auto &base : bases) {
    if (base.isSixToFour()) {
      SixToFourInterfaceConfig sif(base);
      out.push_back(sif);
    }
  }

  return out;
}
