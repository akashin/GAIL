//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../../core/player.hpp"
#include "../../core/match.hpp"
#include "players/random_player.hpp"
#include "players/tree_search_player.hpp"
#include "players/alpha_beta_player.hpp"
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

  if(0){int p = 1;
  for(int m : {3,1,3,1,1,6,2,6,2,6,6,2}) {
    simulator.makeAction(Action(p, m)); p = 3 - p;
  }}


  SimulatorClient first_client(FIRST_PLAYER, simulator);
  SimulatorClient second_client(SECOND_PLAYER, simulator);

  TreeSearchPlayer first_player(FIRST_PLAYER, gail::Config {
    {"start_depth",        3},
    {"max_turn_time",      1000},
    {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
    {"print_debug_info",   true},
  });
  AlphaBettaPlayer second_player(SECOND_PLAYER, gail::Config {
    {"start_depth",        3},
    {"max_turn_time",      100},
    {"cache_max_size",     16 * 1024},
    {"cache_min_depth",    2},
    {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
    {"print_debug_info",   true},
  });

  std::vector<PlayerAction> actions;
  auto match_results = gail::playMatch<PlayerState, PlayerAction>({&first_client, &second_client},
                                                                  {&first_player, &second_player},
                                                                  &actions);

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