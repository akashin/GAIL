//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../../core/player.hpp"
#include "../../core/match.hpp"
#include "random_player.hpp"
#include "tree_search_player.hpp"
#include "scorers.hpp"
#include "search_with_scorer_player.hpp"

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
//  SearchWithScorerPlayer first_player(4, FIRST_PLAYER, std::make_unique<VeryCleverScorer>());
//   TODO(akashin): Find a way to pass this into the constructor.
//  first_player.setWithCache(true);
  SearchWithScorerPlayer second_player(1, SECOND_PLAYER, std::make_unique<VeryCleverScorer>());
  // TODO(akashin): Find a way to pass this into the constructor.
  second_player.setWithCache(true);

  auto match_results = gail::playMatch<PlayerState, PlayerAction>({&first_client, &second_client},
                                                                  {&first_player, &second_player});

  std::cout << "Game length: " << match_results.turn_count << std::endl;
  for (int i = 0; i < match_results.scores.size(); ++i) {
    std::cout << "Score for player " << i << " is " << match_results.scores[i] << std::endl;
  }
  std::cout << "Final board state:" << std::endl;
  std::cout << simulator.getState().field << std::endl;
}

int main() {
  playMatchGame();
  return 0;
}