//
// Created by acid on 10/25/17.
//

#include "clients.hpp"
#include "players.hpp"
#include "../../core/match.hpp"

using namespace gail::code_vs_zombies;

int main() {
  StreamClient client(std::cin, std::cout);
  PickLastHumanPlayer player;
  (void) gail::playMatch<State, Action>({&client}, {&player});
  return 0;
}