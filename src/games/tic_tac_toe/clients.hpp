//
// Created by acid on 10/27/17.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include <iostream>
#include "tic_tac_toe.hpp"
#include "../../core/client.hpp"

namespace gail {
namespace tic_tac_toe {

class StreamClient : public Client<PlayerState, PlayerAction> {
public:
  StreamClient(std::istream& state_input_stream, std::ostream& action_output_stream)
      : state_input_stream(state_input_stream)
        , action_output_stream(action_output_stream) {
    state_input_stream >> state.player_id;
    readField();
    state_refreshed = true;
  }

  ~StreamClient() override = default;

  int getScore() override {
    return 0;
  }

  bool isGameFinished() override {
    return false;
  }

  PlayerState getState() override {
    if (!state_refreshed) {
      readField();
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const PlayerAction& action) {
    action_output_stream << action.col << " " << action.row << std::endl;
  }

private:
  void readField() {
    for (int row = 0; row < RowCount; ++row) {
      for (int col = 0; col < ColCount; ++col) {
        state_input_stream >> state.field[row][col];
      }
    }
  }

  std::istream& state_input_stream;
  std::ostream& action_output_stream;

  bool state_refreshed = false;
  PlayerState state;
};

class SimulatorClient : public Client<PlayerState, PlayerAction> {
public:
  SimulatorClient(int player_id, Simulator& game) : player_id(player_id), game(game) {
    state.player_id = player_id;
  }

  ~SimulatorClient() override = default;

  int getScore() override {
    if (game.getState().winner == NO_PLAYER) {
      return 0;
    }
    if (game.getState().winner == state.player_id) {
      return 1;
    }
    return -1;
  }

  bool isGameFinished() override {
    return game.getState().winner != NO_PLAYER;
  }

  PlayerState getState() override {
    if (!state_refreshed) {
      readField();
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const PlayerAction& action) {
    game.makeAction(Action(player_id, action.row, action.col));
    state_refreshed = false;
  }

private:
  void readField() {
    state.field = game.getState().field;
  }

  bool state_refreshed = false;
  PlayerState state;

  int player_id;
  Simulator& game;
};
}; // namespace tic_tac_toe
}; // namespace gail

#endif //GAIL_CLIENTS_HPP
