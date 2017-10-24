//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../../core/player.hpp"
#include "../../core/match.hpp"
#include "random_player.hpp"
#include "tree_search_player.hpp"

#include <iostream>

using namespace gail::fantastic_four;

void playStreamGame() {
  StreamClient client(std::cin, std::cout);
  RandomPlayer player;
  client.makeAction(player.takeAction(client.getState()));
}

void playMatchGame() {
  Simulator simulator;
  SimulatorClient first_client(FIRST_PLAYER, simulator);
  SimulatorClient second_client(SECOND_PLAYER, simulator);

  TreeSearchPlayer first_player(FIRST_PLAYER);
  TreeSearchPlayer second_player(SECOND_PLAYER);

  auto scores = gail::playMatch<PlayerState, PlayerAction>({&first_client, &second_client},
                                                           {&first_player, &second_player});
  for (int i = 0; i < scores.size(); ++i) {
    std::cout << "Score for player " << i << " is " << scores[i] << std::endl;
  }

  std::cout << simulator.getState().field << std::endl;
}

int main() {
  playMatchGame();
  return 0;
}