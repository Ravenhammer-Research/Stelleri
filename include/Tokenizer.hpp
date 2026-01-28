#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace netcli {
  std::vector<std::string> tokenize(std::string_view input);
}
