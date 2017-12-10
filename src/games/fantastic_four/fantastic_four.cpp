//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../../core/player.hpp"
#include "../../core/match.hpp"
#include "players/random_player.hpp"
#include "players/tree_search_player.hpp"
#include "players/scorers.hpp"
#include "players/search_with_scorer_player.hpp"

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

  TreeSearchPlayer first_player(FIRST_PLAYER, gail::Config {
    {"start_depth",        3},
    {"max_turn_time",      100},
    {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
    // {"scorer",           static_cast<Scorer*>(new OpportunityScorer())},
    {"print_debug_info",   false},
  });
  SearchWithScorerPlayer second_player(SECOND_PLAYER, gail::Config{
      {"start_depth",      1},
      {"scorer",           static_cast<Scorer*>(new OpportunityScorer())},
      {"max_turn_time",    1000},
      {"print_debug_info", false},
  });

  std::vector<PlayerAction> actions;
  auto match_results = gail::playMatch<PlayerState, PlayerAction>({&first_client, &second_client},
                                                                  {&first_player, &second_player},
                                                                  &actions);

  // Game length: 8
  // Score for player 0 is 1 time 637 ms
  // Score for player 1 is -1 time 5406 ms
  // Final board state:
  // 0 0 0 0 0 0 0 0
  // 0 0 0 0 0 0 0 0
  // 0 0 0 0 0 0 0 0
  // 0 0 0 0 0 0 0 0
  // 0 0 0 2 2 0 0 0
  // 0 1 1 1 1 2 0 0
  // 1 2 1 1 2 2 0 0

  3 5 3 3 2 4 4 4 0 1 2 5 1

  std::cout << "Game length: " << match_results.turn_count << std::endl;
  for (int i = 0; i < match_results.scores.size(); ++i) {
    std::cout << "Score for player " << i <<
      " is " << match_results.scores[i] <<
      " time " << match_results.total_time[i] * 1000 / CLOCKS_PER_SEC << " ms" <<
      std::endl;
  }
  std::cout << "Final board state:" << std::endl;
  std::cout << simulator.getState().field << std::endl;
  for (int i = 0; i < actions.size(); ++i) {
    if (i) std::cout << ' ';
    std::cout << actions[i].column;
  }
  std::cout << std::endl;
}

int main() {
  playMatchGame();
  return 0;
}