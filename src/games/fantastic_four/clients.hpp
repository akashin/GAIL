//
// Created by acid on 10/23/17.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include "fantastic_four.hpp"

namespace gail {
namespace fantastic_four {

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

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_CLIENTS_HPP
