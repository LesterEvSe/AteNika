// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 Yevhenii Sekhin

#pragma once

#include <cstdio>
#include <print>
#include <utility>

namespace Output {

  // On a pipe, how every GUI runs us, stdout is fully buffered and println does
  // not flush, so without this a "bestmove" waits in the buffer until the GUI
  // flags us on time.
  //
  // Not the ostream overload of println: Apple's libc++ does not ship
  // print(ostream&, ...) at all, so that form fails to compile on macOS.
  template <class... Args> void reply(std::format_string<Args...> fmt, Args &&...args) {
    std::println(fmt, std::forward<Args>(args)...);
    std::fflush(stdout);
  }

} // namespace Output
