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

namespace gail {
namespace fantastic_four {

namespace tree_search {
  const int NO_ACTION = -1;
  const int TIMEOUT_ACTION = -2;
}

class TreeSearchPlayer : public Player<PlayerState, PlayerAction> {
public:
  explicit TreeSearchPlayer(int player_id, Config config)
      : player_id(player_id) {
    scorer.reset(config.get<Scorer *>("scorer"));
    start_depth = config.get("start_depth", 1);
    end_depth = config.get("end_depth", 8 * 7);
    max_turn_time = config.get<int>("max_turn_time", 0);
    print_debug_info = config.get("print_debug_info", false);
  }

  PlayerAction takeAction(const PlayerState& state) override {
    PlayerAction best_action(-1);
    startTime = clock();
    for (int depth = start_depth; depth <= end_depth; ++depth) {
      auto actionWithScore = findBestAction(player_id, state, depth);
      if (actionWithScore.first.column < 0) break;
      best_action = actionWithScore.first;
      if (print_debug_info) {
        std::cerr << "[tsp]" <<
          " depth: " << depth <<
          " time: " << getTimeMs() <<
          " action: " << actionWithScore.first.column <<
          " score: " << actionWithScore.second << std::endl;
      }
    }
    return best_action;
  }

  int getTimeMs() const {
    clock_t currentTime = clock();
    return (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;
  }

private:
  std::pair<PlayerAction, int>
  findBestAction(int current_player_id, const PlayerState& state, int depth) {
    if (state.winner == player_id) {
      return std::make_pair(PlayerAction(tree_search::NO_ACTION), INF);
    } else if (state.winner == oppositePlayer(player_id)) {
      return std::make_pair(PlayerAction(tree_search::NO_ACTION), -INF);
    } else if (state.winner == DRAW) {
      return std::make_pair(PlayerAction(tree_search::NO_ACTION), 0);
    }
    if (depth == 0) {
      return std::make_pair(PlayerAction(tree_search::NO_ACTION), scorer->score(state.field, player_id));
    }
    if (max_turn_time > 0 && getTimeMs() > max_turn_time * 0.9) {
      return std::make_pair(PlayerAction(tree_search::TIMEOUT_ACTION), 0);
    }
    int next_player_id = oppositePlayer(current_player_id);
    std::pair<PlayerAction, int> bestActionWithScore(PlayerAction(tree_search::NO_ACTION), -INF);
    for (int column = 0; column < W; ++column) {
      Simulator simulator(state.field);
      Action action(current_player_id, column);
      if (simulator.isValidAction(action)) {
        simulator.makeAction(action);
        auto next_state = simulator.getState();
        auto actionWithScore = findBestAction(next_player_id,
                                              PlayerState(next_state.winner, player_id,
                                                          next_state.field),
                                              depth - 1);
        if (actionWithScore.first.column == tree_search::TIMEOUT_ACTION) {
          return actionWithScore;
        }

        if (player_id != current_player_id) {
          actionWithScore.second *= -1;
        }

        // actionWithScore.second -= 1; // win faster, lose slowly

        if (bestActionWithScore.first.column == tree_search::NO_ACTION ||
            actionWithScore.second > bestActionWithScore.second) {
          bestActionWithScore.first = PlayerAction(column);
          bestActionWithScore.second = actionWithScore.second;
        }
      }
    }
    if (player_id != current_player_id) {
      bestActionWithScore.second *= -1;
    }
    return bestActionWithScore;
  }

  int player_id;
  int max_turn_time;
  int start_depth;
  int end_depth;
  bool print_debug_info;
  std::unique_ptr<Scorer> scorer;
  clock_t startTime = 0;
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_TREE_SEARCH_PLAYER_HPP
