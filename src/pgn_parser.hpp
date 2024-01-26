#ifndef ATENIKA_PGN_PARSER_HPP
#define ATENIKA_PGN_PARSER_HPP

#include <string>

// PGN - Portable Game Notation
namespace PGNParser {
namespace hidden {
    extern std::string processed_file;

    void first_processing(const std::string &path);
    void second_processing();

} // hidden
    void parse(const std::string &path);

} // PGNParser

#endif //ATENIKA_PGN_PARSER_HPP
