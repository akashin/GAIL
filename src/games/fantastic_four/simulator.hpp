//
// Created by acid on 10/23/17.
//

#ifndef GAIL_SIMULATOR_HPP
#define GAIL_SIMULATOR_HPP

#include <stdexcept>

#include "fantastic_four.hpp"
#include "utils.hpp"

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
  explicit Simulator(const Field& field)
      : field(field) {
    expected_player_id = deduceExpectedPlayer(field);
  }

  Simulator(): Simulator(Field{}) {}

  void makeAction(const Action& action) {
    validateAction(action);

    int row = -1;
    while (row + 1 < H && field[row + 1][action.column] == NO_PLAYER) {
      ++row;
    }
    if (row == -1) {
      // TODO(akashin): Should we always include field in debug messages?
      std::cerr << field;
      throw std::logic_error("No free space left on column: " + std::to_string(action.column));
    }

    field[row][action.column] = static_cast<int8_t>(action.player_id);
    ++turn;
    winner = getWinner(field);

    if (winner != NO_PLAYER) {
      expected_player_id = NO_PLAYER;
    } else if (expected_player_id == FIRST_PLAYER) {
      expected_player_id = SECOND_PLAYER;
    } else if (expected_player_id == SECOND_PLAYER) {
      expected_player_id = FIRST_PLAYER;
    }
  }

  bool isValidAction(const Action& action) {
    // TODO(akashin): Implement this function.
    return field[0][action.column] == NO_PLAYER;
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
      std::cerr << field;
      throw std::logic_error("Wrong player id, expected: " + std::to_string(expected_player_id) +
                             ", found: " + std::to_string(action.player_id) + ".");
    }

    if (action.column < 0 or action.column >= W) {
      std::cerr << field;
      throw std::logic_error("Wrong action column: " + std::to_string(action.column));
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
