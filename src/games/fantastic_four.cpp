//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../core/player.hpp"

#include <iostream>
#include <random>

namespace gail {
namespace fantastic_four {

class RandomPlayer : public Player<PlayerState, PlayerAction> {
public:
  RandomPlayer(): generator(random_device()), action_distribution(0, 7) {}

  PlayerAction takeAction(const PlayerState& state) override {
    return PlayerAction{action_distribution(generator)};
  }

private:
  std::random_device random_device;
  std::mt19937 generator;
  std::uniform_int_distribution<int> action_distribution;
};

}; // namespace fantastic_four
}; // namespace gail

using namespace gail::fantastic_four;

int main() {
  StreamGame game(std::cin, std::cout);
  RandomPlayer player;
  game.makeAction(player.takeAction(game.getState()));
  return 0;
}