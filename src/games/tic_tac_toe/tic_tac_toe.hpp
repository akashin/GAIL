//
// Created by acid on 10/22/17.
//

#ifndef GAIL_TIC_TAC_TOE_HPP
#define GAIL_TIC_TAC_TOE_HPP

#include <ostream>
#include <array>
#include "../../core/game.hpp"

namespace gail {
namespace tic_tac_toe {

const std::array<int, 4> dRow = {1, 0, 1, -1};
const std::array<int, 4> dCol = {0, 1, 1, 1};

const int NO_PLAYER = 0;
const int FIRST_PLAYER = 1;
const int SECOND_PLAYER = 2;
const int DRAW = 3;

const int RowCount = 3;
const int ColCount = 3;

using Field = std::array<std::array<int, ColCount>, RowCount>;

struct State {
  int turn = 0;
  int winner = NO_PLAYER;
  int expected_player_id = FIRST_PLAYER;
  Field field{};
};

std::ostream& operator<<(std::ostream& os, const State& state) {
  for (int row = 0; row < RowCount; ++row) {
    for (int col = 0; col < ColCount; ++col) {
      os << state.field[row][col] << " ";
    }
    os << std::endl;
  }
  return os;
}

struct Action {
  Action(int player_id, int row, int col)
      : player_id(player_id), row(row), col(col) {}

  int player_id;
  int row, col;
};

struct PlayerState {
  int player_id = NO_PLAYER;
  Field field{};
};

std::istream& operator>>(std::istream& is, PlayerState& state) {
  for (int row = 0; row < RowCount; ++row) {
    for (int col = 0; col < ColCount; ++col) {
      is >> state.field[row][col];
    }
  }
  return is;
}

struct PlayerAction {
  PlayerAction(int row, int col)
      : row(row), col(col) {}

  int row, col;
};

std::ostream& operator<<(std::ostream& os, const PlayerAction& action) {
  os << action.col << " " << action.row;
  return os;
}

}; // namespace tic_tac_toe
}; // namespace gail

#endif //GAIL_TIC_TAC_TOE_HPP
