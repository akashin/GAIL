//
// Created by acid on 10/25/17.
//

#include <fstream>
#include "clients.hpp"
#include "players.hpp"
#include "../../core/match.hpp"

using namespace gail::code_vs_zombies;

// TODO(akashin): Use const char[] here when we migrate to filesystem::path::concat below.
const std::string LEVEL_BASE_PATH = "../src/games/code_vs_zombies/data/";

void playStreamGame() {
  StreamClient client(std::cin, std::cout);
  PickSimulate player;
  (void) gail::playMatch<State, Action>({&client}, {&player});
}

State readLevel(const std::string& path) {
  std::ifstream ifs(path);
  State state;
  ifs >> state;
  return state;
}

void playSimulatorGame(const std::string& level_name) {
  // TODO(akashin): Replace string concatenation with filesystem::path::concat.
  State level = readLevel(LEVEL_BASE_PATH + level_name);
  Simulator simulator(level);
  SimulatorClient client(simulator);
  PickLastHumanPlayer player;
  auto result = gail::playMatch<State, Action>({&client}, {&player});
  for (int i = 0; i < result.scores.size(); ++i) {
    std::cout << "Score for player " << i << " is " << result.scores[i] << std::endl;
  }
}

int main() {
  playSimulatorGame("05.txt");
  return 0;
}