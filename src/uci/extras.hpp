#pragma once

#include <sstream>
#include <string>

class Board;

// Commands that are not part of UCI.
namespace Extras {

  // Returns true if the token was recognized and handled.
  bool dispatch(Board &board, const std::string &token, std::istringstream &args);

  void help();

} // namespace Extras
