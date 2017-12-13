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
#include <fstream>
#include <sstream>

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

  SimulatorClient first_client(FIRST_PLAYER, simulator);
  SimulatorClient second_client(SECOND_PLAYER, simulator);

  int depth = 8;
  AlphaBettaPlayer first_player(FIRST_PLAYER, gail::Config {
    {"start_depth",        depth},
    {"end_depth",          depth},
    {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
    {"cache_max_size",     32 * 1024},
    {"me_min_depth",       2},
    {"cache_min_depth",    2},
    {"print_debug_info",   true},
    {"deterministic",      true},
  });
  AlphaBettaPlayer second_player(SECOND_PLAYER, gail::Config {
    {"start_depth",        depth},
    {"end_depth",          depth},
    {"scorer",             static_cast<Scorer*>(new SimpleScorer())},
    {"cache_max_size",     32 * 1024},
    {"cache_min_depth",    2},
    {"me_min_depth",       2},
    {"print_debug_info",   true},
    {"deterministic",      true},
  });
  // uncached
  // Game length: 25
  // Score for player 0 is -1 time 5354 ms
  // Score for player 1 is 1 time 5018 ms
  // uncached me
  // Game length: 25
  // Score for player 0 is -1 time 4894 ms
  // Score for player 1 is 1 time 3021 ms
  // cached
  // Score for player 0 is -1 time 2415 ms
  // Score for player 1 is 1 time 2196 ms
  // cached me
  // Game length: 25
  // Score for player 0 is -1 time 2187 ms
  // Score for player 1 is 1 time 864 ms
  // cached me swap
  // Game length: 25
  // Score for player 0 is -1 time 851 ms
  // Score for player 1 is 1 time 2136 ms

  // cache usage
  // [abp] depth: 8 time: 534 action: 3 best: 3 score: -8 last_score: -8 cache: 16193
  // [abp] depth: 8 time: 378 action: 2 best: 2 score: -28 last_score: -28 cache: 11893
  // [abp] depth: 8 time: 350 action: 2 best: 2 score: -7 last_score: -7 cache: 11797
  // [abp] depth: 8 time: 260 action: 2 best: 2 score: -34 last_score: -34 cache: 8928
  // cache usage with me(2)-me(2)
  // [abp] depth: 8 time: 108 action: 3 best: 3 score: -8 last_score: -8 cache: 3788
  // [abp] depth: 8 time: 108 action: 2 best: 2 score: -28 last_score: -28 cache: 3355
  // [abp] depth: 8 time: 38 action: 2 best: 2 score: -7 last_score: -7 cache: 1918
  // [abp] depth: 8 time: 36 action: 2 best: 2 score: -34 last_score: -34 cache: 1741

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

void createDb(int seed, int games) {
  std::ofstream db("db.txt", std::ios_base::app);
  auto dump = [&db] (int id, const Field& field, const std::array<int, W>& prob) {
    std::ostringstream ss;
    ss << (id == FIRST_PLAYER ? 0 : 1);
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        ss << ' ' << (field[i][j] == FIRST_PLAYER ? '1' : '0');
        ss << ' ' << (field[i][j] == SECOND_PLAYER ? '1' : '0');
      }
    }
    int sprob = 0;
    for (int i = 0; i < W; ++i) sprob += prob[i];
    for (int i = 0; i < W; ++i) ss << ' ' << (double)prob[i] / sprob;
    ss << std::endl;
    db << ss.str() << std::flush;
  };
  std::mt19937 rnd(seed);
  for (int game = 0; game < games; ++game) {
    int id = FIRST_PLAYER;
    Simulator simulator;
    SimulatorClient c0(FIRST_PLAYER, simulator);
    SimulatorClient c1(SECOND_PLAYER, simulator);
    std::vector<PlayerAction> g;
    while (true) {
      auto& c = (id == FIRST_PLAYER) ? c0 : c1;
      if (c.isGameFinished()) break;
      std::minstd_rand rnd0(rnd());
      auto sc = new MCScorerOpt<std::minstd_rand>(rnd0);
      MCTSPlayer<std::mt19937> player(id, gail::Config {
          {"max_turn_time",    0},
          {"max_turn_sims",    10000},
          {"rnd",              new std::mt19937(rnd())},
          {"scorer",           static_cast<Scorer*>(sc)},
          {"print_debug_info", false},
      });
      std::array<int, 8> prob;
      auto state = c.getState();
      player.probAction(state, prob);
      dump(state.player_id, state.field, prob);
      PlayerAction action(std::max_element(begin(prob), end(prob)) - begin(prob));
      c.makeAction(action);
      g.emplace_back(action);
      id = oppositePlayer(id);
    }
    // emit game
    // for (auto m: g) std::cout << m << ' '; std::cout << std::endl;
  }
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