//
// Created by acid on 10/23/17.
//

#include "fantastic_four.hpp"
#include "../../core/player.hpp"
#include "../../core/match.hpp"
#include "players/random_player.hpp"
#include "players/tree_search_player.hpp"
#include "players/alpha_beta_player.hpp"
#include "players/mcts_player.hpp"
#include "players/scorers.hpp"
#include "players/search_with_scorer_player.hpp"
#include <random>

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

  if(0) { int p = 2;
    for (int m: {0, 2, 0, 3, 0}) {
      simulator.makeAction(Action(p = 3 - p, m));
    }
  }

  SimulatorClient first_client(FIRST_PLAYER, simulator);
  SimulatorClient second_client(SECOND_PLAYER, simulator);

  AlphaBettaPlayer first_player(FIRST_PLAYER, gail::Config {
    {"start_depth",        3},
    // {"end_depth",          7},
    {"max_turn_time",      1000},
    {"cache_max_size",     32 * 1024},
    {"cache_min_depth",    2},
    {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
    {"print_debug_info",   true},
  });
  int seed = 0;
  std::minstd_rand rnd0(seed);
  auto sc = new MCScorerOpt<std::minstd_rand> (rnd0);
  MCTSPlayer<std::mt19937> second_player(SECOND_PLAYER, gail::Config {
    {"max_turn_time",      100},
    {"max_turn_sims",      0},
    {"rnd",                new std::mt19937(seed)},
    {"scorer",             static_cast<Scorer*>(sc)},
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

void testMCTS(Simulator simulator, int seed = 0) {
  SimulatorClient client(FIRST_PLAYER, simulator);
  std::minstd_rand rnd0(seed);
  auto sc = new MCScorerOpt<std::minstd_rand> (rnd0);
  MCTSPlayer<std::mt19937> player(FIRST_PLAYER, gail::Config {
      {"max_turn_time",      0},
      {"max_turn_sims",      10000},
      {"rnd",                new std::mt19937(seed)},
      {"scorer",             static_cast<Scorer*>(sc)},
      {"print_debug_info",   true},
  });
  auto action = player.takeAction(client.getState());
}

void testMC(Field field) {
  std::mt19937 rnd(0);
  int s = 0, n = 240000;
  for (int i = 0; i < n; ++i) {
    std::minstd_rand rnd0(rnd());
    // MCScorer<std::minstd_rand> scorer(rnd0);
    MCScorerOpt<std::minstd_rand> scorer(rnd0);
    int sc = scorer.score(field, FIRST_PLAYER);
    s += sc / LARGE_SCORE;
  }
  std::cerr << double(s) / n << std::endl;
  std::cerr << clock() * 1000.0 / CLOCKS_PER_SEC << std::endl;
}

void test(Simulator simulator) {
  SimulatorClient first_client(FIRST_PLAYER, simulator);
  TreeSearchPlayer first_player(FIRST_PLAYER, gail::Config {
      {"start_depth",        7},
      {"end_depth",          7},
      // {"max_turn_time",      1000},
      {"cache_max_size",     16 * 1024},
      {"cache_min_depth",    2},
      {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
      {"print_debug_info",   true},
  });
  first_player.takeAction(first_client.getState());
}

int main() {
  Simulator simulator;
  if(0) { int p = 2;
    for (int m: {0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 7, 1, 3, 2, 3, 2, 6}) {
      simulator.makeAction(Action(p = 3 - p, m));
    }
  }
  playMatchGame();
  // testMC(simulator.getState().field);
  // testMCTS(simulator);
  // test();
  return 0;
}