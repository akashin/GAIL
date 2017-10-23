//
// Created by acid on 10/22/17.
//
#include <iostream>
#include <array>
#include <sstream>
#include "../core/game.hpp"
#include "tic_tac_toe.hpp"
#include "../core/player.hpp"

using namespace std;

namespace gail {
namespace tic_tac_toe {

struct PlayerGameState {
  int player_id = NO_PLAYER;
  array<array<int, ColCount>, RowCount> field{};
};

struct PlayerAction {
  PlayerAction(int row, int col) : row(row), col(col) {}

  int row, col;
};

//class PlayerTicTacToeGame : public Game<PlayerGameState, PlayerAction> {
//public:
//  PlayerTicTacToeGame(int player_id) : player_id(player_id) {}
//
//  virtual void makeAction(const PlayerAction& action) override {
//    // makeAction(Action(player_id, action.row, action.col));
//  }
//
//private:
//  int player_id;
//};

class StreamBasedGame {
public:
  StreamBasedGame(std::istream &state_input_stream, std::ostream &action_output_stream)
      : state_input_stream(state_input_stream)
      , action_output_stream(action_output_stream) {
    state_input_stream >> state.player_id;
    readField();
    state_refreshed = true;
  }

  // TODO(akashin): Think whether we need this function.
  bool isFinished() const {
    return false;
  }

  const PlayerGameState& getState() {
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
  PlayerGameState state;
};

class SimulatorBasedGame {
public:
  SimulatorBasedGame(int player_id, Simulator& game) : player_id(player_id), game(game) {
    state.player_id = player_id;
  }

  // TODO(akashin): Think whether we need this function.
  bool isFinished() const {
    return game.getState().winner != NO_PLAYER;
  }

  const PlayerGameState& getState() {
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
  PlayerGameState state;

  int player_id;
  Simulator& game;
};

class SimplePlayer : public Player<PlayerGameState, PlayerAction> {
public:
  PlayerAction takeAction(const PlayerGameState& state) override {
    for (int row = 0; row < RowCount; ++row) {
      for (int col = 0; col < ColCount; ++col) {
        if (state.field[row][col] == NO_PLAYER) {
          return PlayerAction(row, col);
        }
      }
    }
    throw std::logic_error("No possible action.");
  }
};

}; // namespace tic_tac_toe
}; // namespace gail

using namespace gail::tic_tac_toe;

void playWithStream() {
  std::stringstream action_input_stream;
  std::stringstream state_output_stream;

  StreamBasedGame game(action_input_stream, state_output_stream);
  SimplePlayer player;

  for (int turn = 0; !game.isFinished(); ++turn) {
    game.makeAction(player.takeAction(game.getState()));
  }
}

void playWithSimulator() {
  gail::tic_tac_toe::Simulator game;

  SimulatorBasedGame firstPlayerGame(FIRST_PLAYER, game);
  SimulatorBasedGame secondPlayerGame(SECOND_PLAYER, game);
  SimplePlayer firstPlayer;
  SimplePlayer secondPlayer;

  for (int turn = 0; game.getState().winner == NO_PLAYER; ++turn) {
    if (turn % 2 == 0) {
      firstPlayerGame.makeAction(firstPlayer.takeAction(firstPlayerGame.getState()));
    } else {
      secondPlayerGame.makeAction(secondPlayer.takeAction(secondPlayerGame.getState()));
    }
    cout << game.getState() << endl;
  }
}

int main() {
  playWithSimulator();
  return 0;
}
