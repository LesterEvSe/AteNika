#ifndef ATENICA_UCI_HPP
#define ATENICA_UCI_HPP

#include "search.hpp"

namespace Uci {
    void init(std::string book_path = "");
    void start();
}

#endif //ATENICA_UCI_HPP
