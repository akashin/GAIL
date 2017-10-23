//
// Created by acid on 10/23/17.
//

#ifndef GAIL_SIMULATOR_HPP
#define GAIL_SIMULATOR_HPP

#include <stdexcept>

#include "fantastic_four.hpp"

namespace gail {
namespace fantastic_four {

struct State {
  State(int winner, const Field& field)
      : winner(winner), field(field) {}

  int winner;
  Field field;
};

struct Action {
  Action(int player_id, int column)
      : player_id(player_id), column(column) {}

  int player_id;
  int column;
};

class Simulator {
public:
  Simulator() = default;

  explicit Simulator(const Field& field)
      : field(field) {
    expected_player_id = deduceExpectedPlayer(field);
  }

  void makeAction(const Action& action) {
    validateAction(action);

    int row = -1;
    while (row + 1 < H && field[row][action.column] == NO_PLAYER) {
      ++row;
    }
    if (row == -1) {
      throw std::logic_error("No free space left on column: " + std::to_string(action.column));
    }

    field[row][action.column] = action.player_id;
    ++turn;
    checkWinner();

    if (winner != NO_PLAYER) {
      expected_player_id = NO_PLAYER;
    } else if (expected_player_id == FIRST_PLAYER) {
      expected_player_id = SECOND_PLAYER;
    } else if (expected_player_id == SECOND_PLAYER) {
      expected_player_id = FIRST_PLAYER;
    }
  }

  State getState() {
    return State(winner, field);
  }

private:
  static int deduceExpectedPlayer(const Field& field) {
    std::array<int, 3> counts{};
    for (int row = 0; row < H; ++row) {
      for (int column = 0; column < W; ++column) {
        ++counts[field[row][column]];
      }
    }
    if (counts[FIRST_PLAYER] <= counts[SECOND_PLAYER]) {
      return FIRST_PLAYER;
    }
    return SECOND_PLAYER;
  }

  void validateAction(const Action& action) {
    if (action.player_id != expected_player_id) {
      throw std::logic_error("Wrong player id, expected: " + std::to_string(expected_player_id) +
                             ", found: " + std::to_string(action.player_id) + ".");
    }

    if (action.column < 0 or action.column >= W) {
      throw std::logic_error("Wrong action column: " + std::to_string(action.column));
    }
  }

  bool isValid(int row, int column) {
    return (row >= 0 && column >= 0 && row < H && column < W);
  }

  void checkWinner() {
    for (int row = 0; row < H; ++row) { for (int col = 0; col < W; ++col) {
        for (int d = 0; d < dRow.size(); ++d) {
          if (field[row][col] == NO_PLAYER) {
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

            if (field[nRow][nCol] != field[row][col]) {
              covered = false;
              break;
            }
          }

          if (covered) {
            winner = field[row][col];
            return;
          }
        }
      }
    }

    if (turn == H * W) {
      winner = DRAW;
    }
  }

  int turn = 0;
  int winner = NO_PLAYER;
  int expected_player_id = NO_PLAYER;
  Field field{};
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_SIMULATOR_HPP
