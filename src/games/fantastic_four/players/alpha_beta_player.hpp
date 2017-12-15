#ifndef GAIL_ALPHA_BETTA_PLAYER_HPP
#define GAIL_ALPHA_BETTA_PLAYER_HPP

#include <iostream>
#include <memory>
#include <ctime>
#include <cassert>

#include "../../../core/player.hpp"
#include "../clients.hpp"
#include "../simulator.hpp"
#include "../../../utils/config.hpp"
#include "scorers.hpp"
#include "score_utils.hpp"
#include "simple_estimator.hpp"
#include "hasher.hpp"

namespace gail {
namespace fantastic_four {

class AlphaBettaPlayer : public Player<PlayerState, PlayerAction> {
public:
  explicit AlphaBettaPlayer(int player_id, Config config)
      : player_id(player_id) {
    scorer.reset(config.get<Scorer *>("scorer"));
    start_depth = config.get("start_depth", 1);
    end_depth = config.get("end_depth", 8 * 7);
    max_turn_time = config.get<int>("max_turn_time", 0);
    print_debug_info = config.get("print_debug_info", false);
    cache_max_size = config.get("cache_max_size", 0);
    cache_min_depth = config.get("cache_min_depth", 0);
    me_min_depth = config.get("me_min_depth", 0);
    deterministic = config.get("deterministic", true);
  }

  PlayerAction takeAction(const PlayerState& state) override {
    PlayerAction best_action(NO_ACTION);
    startTime = clock();
    std::tuple<int, int, ActionWithScore, size_t> dbg =
        std::make_tuple(-1, getTimeMs(), ActionWithScore{0}, cache.size());
    HashField hf(state.field);
    MoveEstimator moveEstimator(state.field, state.player_id);
    int last_score = 0;
    for (int depth = start_depth; depth <= end_depth; ++depth) {
      cache.clear();
      auto actionWithScore = findBestAction(player_id, state, hf,
                                            moveEstimator, depth, -INF, INF);
      last_score = actionWithScore.score;
      if (!actionWithScore.action.isCorrect()) break;
      if (print_debug_info) {
        dbg = std::make_tuple(depth, getTimeMs(), actionWithScore, cache.size());
      }
      if (actionWithScore.score < -INF / 2) break;
      best_action = actionWithScore.action;
      if (actionWithScore.score > INF / 2) break;
    }
    if (print_debug_info) {
      std::cerr << "[abp]" <<
                " depth: " << std::get<0>(dbg) <<
                " time: " << std::get<1>(dbg) <<
                " action: " << std::get<2>(dbg).action.column <<
                " best: " << best_action.column <<
                " score: " << std::get<2>(dbg).score <<
                " last_score: " << last_score <<
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
  findBestAction(int current_player_id, const PlayerState& state,
                 const HashField& hf, const MoveEstimator& moveEstimator,
                 int depth, int alpha, int betta) {
    if (state.winner == current_player_id) {
      return {INF};
    } else if (state.winner == oppositePlayer(current_player_id)) {
      return {-INF};
    } else if (state.winner == DRAW) {
      return {0};
    }
    ActionWithScore best{-INF};
    auto it = cache.end();
    if (depth >= cache_min_depth && cache.size() < cache_max_size) {
      if (depth == 0) {
        best = {scorer->score(state.field, current_player_id)};
        cache.emplace(hf, std::make_pair(best.score, best.score));
        return best;
      } else {
        it = cache.emplace(hf, std::make_pair(-INF, INF)).first;
      }
    } else if (cache_max_size) {
      it = cache.find(hf);
    }
    if (depth == 0) {
      best = {scorer->score(state.field, current_player_id)};
      if (it != cache.end()) it->second = std::make_pair(best.score, best.score);
      return best;
    }
    if (max_turn_time > 0 && getTimeMs() > max_turn_time * 0.9) {
      return {};
    }
    int next_player_id = oppositePlayer(current_player_id);
    if (it != cache.end()) {
      auto ab = it->second;
      if (ab.second <= alpha) return {ab.second};
      if (ab.first >= betta) return {ab.first};
      if (ab.second < betta) betta = ab.second;
      if (ab.first > alpha) alpha = ab.first;
      if (alpha == betta) return {alpha};
    }
    int alpha0 = alpha;
    std::array<PlayerAction, W> order;
    int order_size = 0;
    if (me_min_depth && depth >= me_min_depth) {
      moveEstimator.prediction(order);
    } else {
      for (int column = 0; column < W; ++column) {
        order[column] = PlayerAction(column);
      }
    }
    for (int num_move = 0; num_move < order.size(); ++num_move) {
      int column = order[num_move].column;
      Simulator simulator(state.field);
      Action action(current_player_id, column);
      if (simulator.isValidAction(action)) {
        simulator.makeAction(action);
        auto next_state = simulator.getState();
        int row = lastRow(next_state.field, column);
        auto counter = findBestAction(next_player_id,
          PlayerState(next_state.winner, player_id, next_state.field),
          cache_max_size ? hf.make(current_player_id, row, column) : hf,
          me_min_depth ? moveEstimator.make(current_player_id, row, column) : moveEstimator,
          depth - 1, -betta, -alpha + (deterministic ? 1 : 0));
        if (counter.action == TIMEOUT_ACTION) {
          return {};
        }
        counter.score *= -1;
        if (counter.score > best.score || (deterministic &&
            counter.score == best.score && column < best.action.column)) {
          best = {PlayerAction(column), counter.score};
          if (alpha < counter.score) {
            alpha = counter.score;
            if (alpha >= betta)
              break;
          }
        }
      }
    }
    it = cache.find(hf);
    if (it != cache.end()) {
      if (best.score <= alpha0) {
        if (best.score < it->second.second) it->second.second = best.score;
      } else if (best.score >= betta) {
        if (best.score > it->second.first) it->second.first = best.score;
      } else {
        it->second.first = best.score;
        it->second.second = best.score;
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
  std::unordered_map<HashField, std::pair<int, int>, Hash> cache;
  size_t cache_max_size;
  int cache_min_depth;
  int me_min_depth;
  bool deterministic;
};

} // namespace fantastic_four
} // namespace gail

#endif //GAIL_ALPHA_BETTA_PLAYER_HPP
