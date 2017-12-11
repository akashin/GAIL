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

template <typename P1, typename P2>
void analyzeMatch(Simulator& sim,
                  const gail::Config& c1, const gail::Config& c2,
                  const gail::Config& c1h, const gail::Config& c2h
) {
  std::vector<PlayerAction> actions;
  P1 p1(FIRST_PLAYER, c1);
  P1 p1helper(SECOND_PLAYER, c1h);
  P2 p2(SECOND_PLAYER, c2);
  P2 p2helper(FIRST_PLAYER, c2h);
  SimulatorClient cl1(FIRST_PLAYER, sim);
  SimulatorClient cl2(SECOND_PLAYER, sim);
  bool end = false;
  std::array<clock_t, 2> total_time {0, 0};
  while (!end) {
    end = true;
    if (!cl1.isGameFinished()) {
      clock_t t1 = clock();
      PlayerAction a1 = p1.takeAction(cl1.getState());
      total_time[0] += clock() - t1;
      PlayerAction a1helper = p2helper.takeAction(cl1.getState());
      cl1.makeAction(a1);
      actions.emplace_back(a1);
      end = false;
    }
    if (!cl2.isGameFinished()) {
      clock_t t2 = clock();
      PlayerAction a2 = p2.takeAction(cl2.getState());
      total_time[1] += clock() - t2;
      PlayerAction a2helper = p1helper.takeAction(cl2.getState());
      cl2.makeAction(a2);
      actions.emplace_back(a2);
      end = false;
    }
  }
  std::cout << "Score for player 1 is " << cl1.getScore() <<
    " time " << total_time[0] * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;
  std::cout << "Score for player 2 is " << cl2.getScore() <<
    " time " << total_time[1] * 1000 / CLOCKS_PER_SEC << " ms" << std::endl;
    std::cout << sim.getState().field << std::endl;
  for (int i = 0; i < actions.size(); ++i) {
    if (i) std::cout << ' ';
    std::cout << actions[i].column;
  }
  std::cout << std::endl;
}

void playMatchGame() {
  Simulator simulator;

  if(0){int p = 1;
    for(int m : {3,7,2}) {
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
    {"end_depth",          6},
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