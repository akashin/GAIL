//
// Created by acid on 10/23/17.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include "fantastic_four.hpp"
#include "simulator.hpp"
#include "../../core/client.hpp"

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
  explicit PlayerAction(int column)
      : column(column) {}

  int column;
};

class StreamClient : public Client<PlayerState, PlayerAction> {
public:
  StreamClient(std::istream& state_input_stream, std::ostream& action_output_stream)
      : state_input_stream(state_input_stream), action_output_stream(action_output_stream) {
    readState();
    state_input_stream >> state.player_id;
    state_refreshed = true;
  }

  ~StreamClient() override = default;

  int getScore() override {
    if (state.winner == NO_PLAYER) {
      return 0;
    }

    if (state.winner == state.player_id) {
      return 1;
    }
    return -1;
  }

  bool isGameFinished() override {
    return state.winner != NO_PLAYER;
  }

  PlayerState getState() override {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
   
    return state;
  }

  void makeAction(const PlayerAction& action) override {
    action_output_stream << action.column;
    state_refreshed = false;
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

class SimulatorClient : public Client<PlayerState, PlayerAction> {
public:
  SimulatorClient(int player_id, Simulator& simulator)
      : player_id(player_id), simulator(simulator) {
    state.player_id = player_id;
    readState();
    state_refreshed = true;
  }

  ~SimulatorClient() override = default;


  int getScore() override {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
    }
    if (state.winner == NO_PLAYER) {
      return 0;
    }

    if (state.winner == state.player_id) {
      return 1;
    }
    return -1;
  }

  bool isGameFinished() override {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
    }
    return state.winner != NO_PLAYER;
  }

  PlayerState getState() override {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const PlayerAction& action) override {
    simulator.makeAction(Action(player_id, action.column));
    state_refreshed = false;
  }

private:
  void readState() {
    state.winner = simulator.getState().winner;
    state.field = simulator.getState().field;
  }

  bool state_refreshed = false;
  PlayerState state;
  int player_id;
  Simulator& simulator;
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_CLIENTS_HPP
