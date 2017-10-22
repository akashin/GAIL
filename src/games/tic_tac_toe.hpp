//
// Created by acid on 10/22/17.
//

#ifndef GAIL_TIC_TAC_TOE_HPP
#define GAIL_TIC_TAC_TOE_HPP

#include <ostream>
#include <array>
#include "../core/game.hpp"

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

struct State {
  State() {
    for (int i = 0; i < RowCount; ++i) {
      for (int j = 0; j < ColCount; ++j) {
        field[i][j] = NO_PLAYER;
      }
    }
  }

  int turn = 0;
  int winner = NO_PLAYER;
  int expected_player_id = FIRST_PLAYER;
  std::array <std::array<int, ColCount>, RowCount> field;
};

struct Action {
  Action(int player_id, int row, int col) : player_id(player_id), row(row), col(col) {}

  int player_id;
  int row, col;
};

std::ostream &operator<<(std::ostream &os, const State &state) {
  for (int row = 0; row < RowCount; ++row) {
    for (int col = 0; col < ColCount; ++col) {
      os << state.field[row][col] << " ";
    }
    os << std::endl;
  }
  return os;
}

class Simulator : public Game<State, Action> {
public:
  ~Simulator() override = default;

  const State& getState() override {
    return state;
  }

  void makeAction(const Action &action) override {
    validateAction(action);

    state.field[action.row][action.col] = action.player_id;
    ++state.turn;
    checkWinner();

    if (state.expected_player_id == FIRST_PLAYER) {
      state.expected_player_id = SECOND_PLAYER;
    } else {
      state.expected_player_id = FIRST_PLAYER;
    }
  }

private:
  bool isValid(int row, int col) const {
    return !(row < 0 || col < 0 || row >= RowCount || col >= ColCount);
  }

  void checkWinner() {
    for (int row = 0; row < RowCount; ++row) {
      for (int col = 0; col < ColCount; ++col) {
        for (int d = 0; d < dRow.size(); ++d) {
          if (state.field[row][col] == NO_PLAYER) {
            continue;
          }

          bool covered = true;
          for (int k = 0; k < 3; ++k) {
            int nRow = row + k * dRow[d];
            int nCol = col + k * dCol[d];

            if (!isValid(nRow, nCol)) {
              covered = false;
              break;
            }

            if (state.field[nRow][nCol] != state.field[row][col]) {
              covered = false;
              break;
            }
          }

          if (covered) {
            state.winner = state.field[row][col];
            return;
          }
        }
      }
    }

    if (state.turn == RowCount * ColCount) {
      state.winner = DRAW;
    }
  }

  void validateAction(const Action &action) {
    if (state.winner != NO_PLAYER) {
      throw std::logic_error("Game is already completed.");
    }

    if (action.player_id != state.expected_player_id) {
      throw std::logic_error("Invalid player id, expected " +
                             std::to_string(state.expected_player_id) +
                             ",found " + std::to_string(action.player_id) + ".");
    }

    if (!isValid(action.row, action.col)) {
      throw std::logic_error("Invalid move out of the field.");
    }

    if (state.field[action.row][action.col] != NO_PLAYER) {
      throw std::logic_error("Cell is already occupied.");
    }
  }

  State state;
};

}; // namespace tic_tac_toe
}; // namespace gail

#endif //GAIL_TIC_TAC_TOE_HPP
