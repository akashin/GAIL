//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../../core/player.hpp"
#include "random_player.hpp"

#include <iostream>

using namespace gail::fantastic_four;

int main() {
  StreamGame game(std::cin, std::cout);
  RandomPlayer player;
  game.makeAction(player.takeAction(game.getState()));
  return 0;
}