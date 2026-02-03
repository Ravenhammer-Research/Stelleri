#include "ConfigurationManager.hpp"
#include "DeleteToken.hpp"
#include "InterfaceToken.hpp"
#include "Parser.hpp"
#include "RouteToken.hpp"
#include "SetToken.hpp"
#include "ShowToken.hpp"
#include "VRFToken.hpp"
#include <iostream>

namespace netcli {

  void Parser::executeCommand(const std::shared_ptr<Token> &head,
                              ConfigurationManager *mgr) const {
    if (!head)
      return;

    if (dynamic_cast<ShowToken *>(head.get())) {
      auto next = head->getNext();
      if (!next)
        return;
      if (auto iftok = dynamic_cast<InterfaceToken *>(next.get())) {
        executeShowInterface(*iftok, mgr);
        return;
      }
      if (auto rt = dynamic_cast<RouteToken *>(next.get())) {
        executeShowRoute(*rt, mgr);
        return;
      }
      if (auto vt = dynamic_cast<VRFToken *>(next.get())) {
        executeShowVRF(*vt, mgr);
        return;
      }
    }

    if (dynamic_cast<SetToken *>(head.get())) {
      auto next = head->getNext();
      if (!next)
        return;
      if (auto iftok = dynamic_cast<InterfaceToken *>(next.get())) {
        executeSetInterface(*iftok, mgr);
        return;
      }
      if (auto rt = dynamic_cast<RouteToken *>(next.get())) {
        executeSetRoute(*rt, mgr);
        return;
      }
      if (auto vt = dynamic_cast<VRFToken *>(next.get())) {
        executeSetVRF(*vt, mgr);
        return;
      }
    }

    if (dynamic_cast<DeleteToken *>(head.get())) {
      auto next = head->getNext();
      if (!next)
        return;
      if (auto iftok = dynamic_cast<InterfaceToken *>(next.get())) {
        executeDeleteInterface(*iftok, mgr);
        return;
      }
      if (auto rt = dynamic_cast<RouteToken *>(next.get())) {
        executeDeleteRoute(*rt, mgr);
        return;
      }
      if (auto vt = dynamic_cast<VRFToken *>(next.get())) {
        executeDeleteVRF(*vt, mgr);
        return;
      }
    }

    std::cout << "execute: unknown or unsupported command\n";
  }

} // namespace netcli
