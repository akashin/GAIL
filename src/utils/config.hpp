//
// Created by acid on 11/9/17.
//

#ifndef GAIL_CONFIG_HPP
#define GAIL_CONFIG_HPP

#include <unordered_map>

#include "../third_party/stx/any.hpp"

namespace gail {

class Config {
public:
  Config() = default;

  Config(std::initializer_list<std::pair<const std::string, stx::any>> values)
      : values(values) {}

  template <typename T>
  T get(const std::string& key, T default_value = T()) {
    auto it = values.find(key);
    if (it == values.end()) {
      return default_value;
    }
    return stx::any_cast<T>(it->second);
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
