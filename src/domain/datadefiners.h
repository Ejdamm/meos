#pragma once

#include <string>

/**
 * Used to track the last SQL modification time and counter for an object.
 */
struct SqlUpdated {
  std::string updated; // ISO 8601 or similar timestamp
  int counter = 0;
  bool changed = false;

  void reset() {
    updated.clear();
    changed = false;
    counter = 0;
  }
};
