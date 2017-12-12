//
// Created by acid on 10/23/17.
//

#ifndef GAIL_TREE_SEARCH_PLAYER_HPP
#define GAIL_TREE_SEARCH_PLAYER_HPP

#include <iostream>
#include <memory>
#include <ctime>

#include "../../../core/player.hpp"
#include "../clients.hpp"
#include "../simulator.hpp"
#include "../../../utils/config.hpp"
#include "scorers.hpp"
#include "score_utils.hpp"
#include "hasher.hpp"

namespace gail {
namespace fantastic_four {

class TreeSearchPlayer : public Player<PlayerState, PlayerAction> {
public:
  explicit TreeSearchPlayer(int player_id, Config config)
      : player_id(player_id) {
    scorer.reset(config.get<Scorer *>("scorer"));
    start_depth = config.get("start_depth", 1);
    end_depth = config.get("end_depth", 8 * 7);
    max_turn_time = config.get<int>("max_turn_time", 0);
    print_debug_info = config.get("print_debug_info", false);
    cache_max_size = config.get("cache_max_size", 0);
    cache_min_depth = config.get("cache_min_depth", 0);
  }

  PlayerAction takeAction(const PlayerState& state) override {
    PlayerAction best_action(NO_ACTION);
    startTime = clock();
    std::tuple<int, int, ActionWithScore, size_t> dbg;
    HashField hf(state.field);
    for (int depth = start_depth; depth <= end_depth; ++depth) {
      cache.clear();
      auto actionWithScore = findBestAction(player_id, state, hf, depth);
      if (!actionWithScore.action.isCorrect()) break;
      if (print_debug_info) {
        dbg = std::make_tuple(depth, getTimeMs(), actionWithScore, cache.size());
      }
      if (actionWithScore.score < -INF / 2) break;
      best_action = actionWithScore.action;
      if (actionWithScore.score > INF / 2) break;
    }
    if (print_debug_info) {
      std::cerr << "[tsp]" <<
                " depth: " << std::get<0>(dbg) <<
                " time: " << std::get<1>(dbg) <<
                " action: " << std::get<2>(dbg).action.column <<
                " best: " << best_action.column <<
                " score: " << std::get<2>(dbg).score <<
                " cache: " << std::get<3>(dbg) << std::endl;
    }
    if (best_action == NO_ACTION) {
      best_action.column = firstPossibleMove(state.field);
    }
    return best_action;
  }

  int getTimeMs() const {
    clock_t currentTime = clock();
    return (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;
  }

private:
  ActionWithScore
  findBestAction(int current_player_id, const PlayerState& state, const HashField& hf, int depth) {
    if (state.winner == player_id) {
      return {INF};
    } else if (state.winner == oppositePlayer(player_id)) {
      return {-INF};
    } else if (state.winner == DRAW) {
      return {0};
    }
    ActionWithScore best{current_player_id == player_id ? -INF : INF};
    auto it = cache.end();
    bool found = false;
    if (depth >= cache_min_depth && cache.size() < cache_max_size) {
      if (depth == 0) {
        best = {scorer->score(state.field, player_id)};
        cache.emplace(hf, best.score);
        return best;
      } else {
        auto pit = cache.emplace(hf, best.score);
        it = pit.first;
        found = !pit.second;
      }
    } else if (cache_max_size) {
      it = cache.find(hf);
      found = it != cache.end();
    }
    if (found) return {it->second};
    if (depth == 0) return {scorer->score(state.field, player_id)};
    if (max_turn_time > 0 && getTimeMs() > max_turn_time * 0.9) {
      return {};
    }
    int next_player_id = oppositePlayer(current_player_id);
    for (int column = 0; column < W; ++column) {
      Simulator simulator(state.field);
      Action action(current_player_id, column);
      if (simulator.isValidAction(action)) {
        simulator.makeAction(action);
        auto next_state = simulator.getState();
        int row = lastRow(next_state.field, column);
        HashField nhf = hf;
        if (cache_max_size && depth >= cache_min_depth)
          nhf = hf.make(current_player_id, row, column);
        auto counter = findBestAction(next_player_id,
                                      PlayerState(next_state.winner, player_id,
                                                  next_state.field),
                                      (cache_max_size && depth >= cache_min_depth) ? hf.make
                                          (current_player_id, row, column) : hf,
                                      depth - 1);
        if (counter.action == TIMEOUT_ACTION) {
          return {};
        }
        if (best.action == NO_ACTION ||
            (current_player_id == player_id && counter.score > best.score) &&
            (next_player_id == player_id && counter.score < best.score)) {
          best = {PlayerAction(column), counter.score};
        }
      }
    }
    return best;
  }

  int player_id;
  int max_turn_time;
  int start_depth;
  int end_depth;
  bool print_debug_info;
  std::unique_ptr<Scorer> scorer;
  clock_t startTime = 0;
  std::unordered_map<HashField, int, Hash> cache;
  size_t cache_max_size;
  int cache_min_depth;
};

} // namespace fantastic_four
} // namespace gail

#endif //GAIL_TREE_SEARCH_PLAYER_HPP
