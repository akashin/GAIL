//
// Created by acid on 10/25/17.
//

#include "clients.hpp"
#include "players.hpp"

using namespace gail::code_vs_zombies;

int main() {
  StreamClient client(std::cin, std::cout);
  PickLastHumanPlayer player;
  while (!client.isGameFinished()) {
    client.makeAction(player.takeAction(client.getState()));
  }
  return 0;
}