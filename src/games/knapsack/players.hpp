//
// Created by Andrei Kashin on 24/11/2017.
//

#ifndef GAIL_PLAYERS_HPP
#define GAIL_PLAYERS_HPP

#include <algorithm>
#include <random>
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

struct EvolutionParameters {
  std::vector<int> sample() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(0, 1);

    std::vector<int> result;
    for (int i = 0; i < probabilities.size(); ++i) {
      if (probabilities[i] > distribution(generator)) {
        result.push_back(i);
      }
    }
    return result;
  }

  std::vector<float> probabilities;
};

EvolutionParameters generateEvolutionParameters(State state) {
  EvolutionParameters parameters;
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<> distribution(0, 1);
  for (int i = 0; i < state.items.size(); ++i) {
    parameters.probabilities.push_back(distribution(generator));
  }
  return parameters;
}

class OneActionClient : public Player<State, Action> {
public:
  explicit OneActionClient(Action action)
      : action(std::move(action)) {}

  Action takeAction(const State& state) override {
    return action;
  }

private:
  Action action;
};

};  // namespace knapsack
};  // namespace gail

#endif //GAIL_PLAYERS_HPP
