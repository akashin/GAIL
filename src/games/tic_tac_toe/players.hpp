//
// Created by acid on 10/27/17.
//

#ifndef GAIL_PLAYERS_HPP
#define GAIL_PLAYERS_HPP

#include "tic_tac_toe.hpp"
#include "../../core/player.hpp"

namespace gail {
namespace tic_tac_toe {

class SimplePlayer : public Player<PlayerState, PlayerAction> {
public:
  PlayerAction takeAction(const PlayerState& state) override {
    for (int row = 0; row < RowCount; ++row) {
      for (int col = 0; col < ColCount; ++col) {
        if (state.field[row][col] == NO_PLAYER) {
          return PlayerAction(row, col);
        }
      }
    }
    throw std::logic_error("No possible action.");
  }
};

}; // namespace tic_tac_toe
}; // namespace gail

#endif //GAIL_PLAYERS_HPP
