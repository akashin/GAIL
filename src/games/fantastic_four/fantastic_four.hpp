//
// Created by acid on 10/23/17.
//

#ifndef GAIL_FANTASTIC_FOUR_HPP
#define GAIL_FANTASTIC_FOUR_HPP

#include <cstdint>
#include <array>
#include <istream>
#include <ostream>

namespace gail {
namespace fantastic_four {

const int H = 7;
const int W = 8;
using Field = std::array<std::array<int8_t, W>, H>;

const std::array<int, 4> dRow = {1, 0, 1, -1};
const std::array<int, 4> dCol = {0, 1, 1, 1};

const int NO_PLAYER = 0;
const int FIRST_PLAYER = 1;
const int SECOND_PLAYER = 2;
const int DRAW = 3;

struct PlayerState {
  PlayerState() = default;

  PlayerState(int winner, int player_id, const Field& field)
      : winner(winner), player_id(player_id), field(field) {}

  int winner = NO_PLAYER;
  int player_id = NO_PLAYER;
  Field field{};
};

struct PlayerAction {
  int column;
};

class StreamGame {
public:
  StreamGame(std::istream& state_input_stream, std::ostream& action_output_stream)
      : state_input_stream(state_input_stream), action_output_stream(action_output_stream) {
    readState();
    state_input_stream >> state.player_id;
    state_refreshed = true;
  }

  PlayerState getState() {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const PlayerAction& action) {
    action_output_stream << action.column;
  }

private:
  void readState() {
    for (int row = 0; row < H; ++row) {
      for (int col = 0; col < W; ++col) {
        state_input_stream >> state.field[row][col];
      }
    }
  }

  bool state_refreshed = false;
  PlayerState state;
  std::istream& state_input_stream;
  std::ostream& action_output_stream;

};

class Simulator {
public:
  Simulator() = default;

  explicit Simulator(const Field& field)
      : field(field) {}

  void makeAction(int8_t player_id, const PlayerAction& action) {
    validateAction(player_id, action);

    int row = -1;
    while (row + 1 < H && field[row][action.column] == NO_PLAYER) {
      ++row;
    }
    if (row == -1) {
      throw std::logic_error("No free space left on column: " + std::to_string(action.column));
    }

    field[row][action.column] = player_id;
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

  // TODO(akashin): Should this return PlayerState? Maybe only field and winner.
  PlayerState getState(int player_id) {
    return PlayerState(winner, player_id, field);
  }

private:
  void validateAction(int player_id, const PlayerAction& action) {
    if (player_id != expected_player_id) {
      throw std::logic_error("Wrong player id, expected: " + std::to_string(expected_player_id) +
                             ", found: " + std::to_string(player_id) + ".");
    }

    if (action.column < 0 or action.column >= W) {
      throw std::logic_error("Wrong action column: " + std::to_string(action.column));
    }
  }

  bool isValid(int row, int column) {
    return (row >= 0 && column >= 0 && row < H && column < W);
  }

  void checkWinner() {
    for (int row = 0; row < H; ++row) {
      for (int col = 0; col < W; ++col) {
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
  int expected_player_id = FIRST_PLAYER;
  Field field{};
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_FANTASTIC_FOUR_HPP
