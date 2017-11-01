//
// Created by acid on 10/25/17.
//

#include "clients.hpp"
#include "players.hpp"
#include "../../core/match.hpp"

using namespace gail::code_vs_zombies;

void playStreamGame() {
  StreamClient client(std::cin, std::cout);
  PickSimulate player;
  (void) gail::playMatch<State, Action>({&client}, {&player});
}

int main() {
  playStreamGame();
  return 0;
}