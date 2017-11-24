//
// Created by Andrei Kashin on 24/11/2017.
//

#ifndef GAIL_PLAYERS_HPP
#define GAIL_PLAYERS_HPP

#include "knapsack.hpp"
#include "../../core/player.hpp"
namespace gail {
namespace knapsack {

// Picks items greedily in order given by the ratio `utility / weight`.
class GreedyPlayer : public Player<State, Action> {

public:
  Action takeAction(const State &state) override {
    std::vector<Item> sortedItems = state.items;
    std::sort(sortedItems.begin(), sortedItems.end(), [] (const Item& lhs, const Item& rhs) {
      return lhs.utility * rhs.weight > rhs.utility * lhs.weight;
    });

    Action action;
    int leftoverCapacity = state.capacity;
    for (const auto& item : sortedItems) {
      if (item.weight > leftoverCapacity) {
          continue;
      }

      leftoverCapacity -= item.weight;
      action.takenItems.push_back(item.index);
    }

    return action;
  }

};

};  // namespace knapsack
};  // namespace gail

#endif //GAIL_PLAYERS_HPP
