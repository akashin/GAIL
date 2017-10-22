//
// Created by acid on 10/22/17.
//
#include <iostream>
#include <array>
#include "../core/game.hpp"

using namespace std;

const std::array<int, 4> dRow = {1, 0, 1, -1};
const std::array<int, 4> dCol = {0, 1, 1, -1};

const int NO_PLAYER = 0;
const int FIRST_PLAYER = 1;
const int SECOND_PLAYER = 2;

const int RowCount = 3;
const int ColCount = 3;

struct TicTacToeState {
  TicTacToeState() {
    for (int i = 0; i < RowCount; ++i) {
      for (int j = 0; j < ColCount; ++j) {
        field[i][j] = NO_PLAYER;
      }
    }
  }

  int winner = NO_PLAYER;
  int expected_player_id = FIRST_PLAYER;
  array<array<int, ColCount>, RowCount> field;
};

struct TicTacToeAction {
  TicTacToeAction(int player_id, int row, int col) : player_id(player_id), row(row), col(col) {}

  int player_id;
  int row, col;
};

class TicTacToeGame : public Game<TicTacToeState, TicTacToeAction> {
public:
  virtual ~TicTacToeGame() override = default;

  const TicTacToeState& getState() override {
    return state;
  }

  void makeAction(const TicTacToeAction& action) override {
    validateAction(action);

    state.field[action.row][action.col] = action.player_id;
    checkWinner();

    if (state.expected_player_id == FIRST_PLAYER) {
      state.expected_player_id = SECOND_PLAYER;
    } else {
      state.expected_player_id = FIRST_PLAYER;
    }
  }

private:
  bool isValid(int row, int col) const {
    if (row < 0 || col < 0 || row >= RowCount || col >= ColCount) {
      return false;
    }
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
  }

  void validateAction(const TicTacToeAction& action) {
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

  TicTacToeState state;
};

int main() {
  TicTacToeGame game;
  cout << "Hello, world" << endl;
  return 0;
}