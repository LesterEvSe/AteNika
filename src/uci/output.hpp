#ifndef ATENIKA_OUTPUT_HPP
#define ATENIKA_OUTPUT_HPP

#include <iostream>
#include <print>
#include <utility>

namespace Output {

  // On a pipe, how every GUI runs us. std::cout is fully buffered and
  // println does not flush, so without this a "bestmove" waits in the buffer
  // until the GUI flags us on time. So use std::cout.flush() to fix it.
  template <class... Args> void reply(std::format_string<Args...> fmt, Args &&...args) {
    std::println(std::cout, fmt, std::forward<Args>(args)...);
    std::cout.flush();
  }

} // namespace Output

#endif // ATENIKA_OUTPUT_HPP
