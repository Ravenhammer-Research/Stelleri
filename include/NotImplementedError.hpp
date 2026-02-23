/*
 * NotImplementedError.hpp
 * Simple exception type to indicate unimplemented APIs.
 */
#pragma once

#include <stdexcept>

struct NotImplementedError : public std::logic_error {
  NotImplementedError() : std::logic_error("Not implemented") {}
};
