#ifndef ATENIKA_PGN_PARSER_HPP
#define ATENIKA_PGN_PARSER_HPP
#define _CRT_SECURE_NO_WARNINGS // to prevent MSVC compiler error with strerror

#include <string>

namespace PGNParser {
  void parse(const std::string &path);
} // namespace PGNParser

// PGN - Portable Game Notation
namespace PGNParser::detail {
  extern std::string processed_file;

  void first_processing(const std::string &path);
  void second_processing();

} // namespace PGNParser::detail

#endif // ATENIKA_PGN_PARSER_HPP
