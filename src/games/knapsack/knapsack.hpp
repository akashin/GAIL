//
// Created by Andrei Kashin on 24/11/2017.
//

#ifndef GAIL_KNAPSACK_HPP
#define GAIL_KNAPSACK_HPP

#include <vector>

namespace gail {
namespace knapsack {

struct Item {
  Item(int index, int weight, int utility) : index(index), weight(weight), utility(utility) {}

  int index;
  int weight;
  int utility;
};

struct State {
  int capacity = 0;
  std::vector<Item> items{};
};

struct Action {
  std::vector<int> takenItems{};
};

};  // namespace gail
};  // namespace knapsack


#endif //GAIL_KNAPSACK_HPP
