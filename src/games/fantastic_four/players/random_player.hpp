//
// Created by acid on 10/23/17.
//

#ifndef GAIL_RANDOM_PLAYER_HPP
#define GAIL_RANDOM_PLAYER_HPP

#include "../fantastic_four.hpp"
#include "../../../core/player.hpp"
#include "../clients.hpp"

#include <random>

namespace gail {
namespace fantastic_four {

class RandomPlayer : public Player<PlayerState, PlayerAction> {
public:
  RandomPlayer(): generator(random_device()), action_distribution(0, 7) {}

  PlayerAction takeAction(const PlayerState& state) override {
    while (true) {
      int column = action_distribution(generator);
      if (state.field[0][column] == NO_PLAYER) {
        return PlayerAction(column);
      }
    }
  }

private:
  std::random_device random_device;
  std::mt19937 generator;
  std::uniform_int_distribution<int> action_distribution;
};

}; // namespace fantastic_four
}; // namespace gail


#endif //GAIL_RANDOM_PLAYER_HPP
