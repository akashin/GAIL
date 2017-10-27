//
// Created by acid on 10/22/17.
//
#include <iostream>
#include <array>
#include <sstream>
#include "../../core/game.hpp"
#include "tic_tac_toe.hpp"
#include "../../core/player.hpp"
#include "../../core/client.hpp"
#include "../../core/match.hpp"
#include "clients.hpp"
#include "players.hpp"

using namespace std;
using namespace gail::tic_tac_toe;

void playWithStream() {
  std::stringstream action_input_stream;
  std::stringstream state_output_stream;

  StreamClient game(action_input_stream, state_output_stream);
  SimplePlayer player;

  for (int turn = 0; !game.isGameFinished(); ++turn) {
    game.makeAction(player.takeAction(game.getState()));
  }
}

void playWithSimulator() {
  gail::tic_tac_toe::Simulator game;

  SimulatorClient firstClient(FIRST_PLAYER, game);
  SimulatorClient secondClient(SECOND_PLAYER, game);
  SimplePlayer firstPlayer;
  SimplePlayer secondPlayer;

  gail::playMatch<PlayerState, PlayerAction>({&firstClient, &secondClient},
                                             {&firstPlayer, &secondPlayer});
}

int main() {
  playWithSimulator();
  return 0;
}
