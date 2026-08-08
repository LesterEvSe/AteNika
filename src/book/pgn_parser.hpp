#ifndef ATENIKA_PGN_PARSER_HPP
#define ATENIKA_PGN_PARSER_HPP

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
