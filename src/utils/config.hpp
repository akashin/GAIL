//
// Created by acid on 11/9/17.
//

#ifndef GAIL_CONFIG_HPP
#define GAIL_CONFIG_HPP

#include <unordered_map>

#include "any.hpp"

namespace gail {

class Config {
public:
  template <typename T>
  T& get(std::string key) {
    return stx::any_cast<T>(values.at(key));
  }

  template <typename T>
  void set(std::string key, const T& value) {
    values.insert(key, value);
  }

private:
  std::unordered_map<std::string, stx::any> values;

};

}; // namespace gail

#endif //GAIL_CONFIG_HPP
