//
// Created by acid on 11/14/17.
//

#ifndef GAIL_SIMULATOR_HPP
#define GAIL_SIMULATOR_HPP

#include "tic_tac_toe.hpp"

namespace gail {
namespace tic_tac_toe {

class Simulator : public Game<State, Action> {
public:
  ~Simulator() override = default;

  const State& getState() override {
    return state;
  }

  void makeAction(const Action& action) override {
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

  void validateAction(const Action& action) {
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

#endif //GAIL_SIMULATOR_HPP
