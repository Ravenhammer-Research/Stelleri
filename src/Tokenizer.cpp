#include "Tokenizer.hpp"
#include <string>
#include <vector>

namespace netcli {
  std::vector<std::string> tokenize(std::string_view input) {
    std::vector<std::string> out;
    std::string cur;
    enum State { Normal, InQuote, Escape } state = Normal;
    for (size_t i = 0; i < input.size(); ++i) {
      char c = input[i];
      switch (state) {
      case Normal:
        if (c == '\'') {
          state = InQuote; // enter single-quote mode
        } else if (isspace(static_cast<unsigned char>(c))) {
          if (!cur.empty()) {
            out.push_back(cur);
            cur.clear();
          }
        } else {
          cur.push_back(c);
        }
        break;
      case InQuote:
        if (c == '\'') {
          state = Normal;
        } else if (c == '\\') {
          state = Escape;
        } else {
          cur.push_back(c);
        }
        break;
      case Escape:
        cur.push_back(c);
        state = InQuote;
        break;
      }
    }
    if (!cur.empty())
      out.push_back(cur);
    return out;
  }
} // namespace netcli
