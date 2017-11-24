//
// Created by Andrei Kashin on 24/11/2017.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include "knapsack.hpp"
#include "../../core/client.hpp"

namespace gail {
namespace knapsack {

class OneStateClient : public Client<State, Action> {
public:
  explicit OneStateClient(State state) : state(std::move(state)) {}

  int getScore() override {
    int totalWeight = 0;
    int totalUtility = 0;
    for (int index : takenItems) {
      totalWeight += state.items[index].weight;
      totalUtility += state.items[index].utility;
    }
    if (totalWeight > state.capacity) {
      return -(totalWeight - state.capacity);
    }
    return totalUtility;
  }

  bool isGameFinished() override {
    return gameFinished;
  }

  State getState() override {
    return state;
  }

  void makeAction(const Action &action) override {
    takenItems = action.takenItems;
    gameFinished = true;
  }

private:
  State state;
  bool gameFinished = false;
  std::vector<int> takenItems;

};

};  // namespace knapsack
};  // namespace gail


#endif //GAIL_CLIENTS_HPP
