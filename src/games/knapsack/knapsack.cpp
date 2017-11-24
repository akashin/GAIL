//
// Created by Andrei Kashin on 24/11/2017.
//

#include "clients.hpp"
#include "players.hpp"
#include "../../core/match.hpp"

using namespace gail::knapsack;

State generateState(int itemCount, int capacity) {
  State state;

  state.capacity = capacity;
  for (int i = 0; i < itemCount; ++i) {

  }

  return state;
}

int main() {
  OneStateClient client(generateState(100, 1000));
  GreedyPlayer player;

  auto results = gail::playMatch<State, Action>({&client}, {&player});

  std::cout << "Total score: " << results.scores[0] << std::endl;
}
