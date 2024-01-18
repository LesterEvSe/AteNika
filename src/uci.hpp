#ifndef ATENIKA_UCI_HPP
#define ATENIKA_UCI_HPP

#include "search.hpp"

namespace Uci {
    void init(std::string book_path = "");
    void start();
}

#endif //ATENIKA_UCI_HPP
