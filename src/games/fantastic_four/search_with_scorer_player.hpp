//
// Created by acid on 11/7/17.
//

#ifndef GAIL_STRATEGY_PLAYER_HPP
#define GAIL_STRATEGY_PLAYER_HPP

#include <bits/unique_ptr.h>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <vector>
#include "simulator.hpp"
#include "../../core/player.hpp"
#include "scorers.hpp"
#include "clients.hpp"
#include "../../utils/config.hpp"

namespace gail {
namespace fantastic_four {

const int NO_ACTION = -1;
const int TIMEOUT_ACTION = -2;
const int IN_CACHE_ACTION = -3;

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);
    return h1 ^ h2;
  }
};

int putInplace(Field& field, int w, int id) {
  if (field[0][w] != 0) {
    std::cerr << std::endl << w << std::endl << field;
    assert(field[0][w] == 0);
  }

  int h = 0;
  while (h + 1 < H && field[h + 1][w] == 0) {
    ++h;
  }
  field[h][w] = id;
  return h;
}

bool isFull(const Field& field, int w) {
  return field[0][w] != 0;
}

struct Decision {
  Decision() = default;

  Decision(int w, float score)
      : w(w), score(score) {}

  int w = 0;
  float score = 0;
};

const float SCORE_DECAY = 0.9f;
const float MULTIPLE_SCORE_DISCOUNT = 0.5f / (W - 1);

class SearchWithScorerPlayer : public Player<PlayerState, PlayerAction> {
public:
  SearchWithScorerPlayer(int player_id, Config config)
      : player_id(player_id) {
    start_depth = config.get("start_depth", 1);
    scorer.reset(config.get<Scorer *>("scorer"));
    score_decay = config.get("score_decay", SCORE_DECAY);
    multiple_score_discount = config.get("multiple_score_discount", MULTIPLE_SCORE_DISCOUNT);
    with_cache = config.get("with_cache", false);
    max_turn_time = config.get<int>("max_turn_time");
    print_debug_info = config.get("print_debug_info", false);
  }

  PlayerAction takeAction(const PlayerState& state) override {
    return PlayerAction(solve(state.field, player_id));
  }

  std::string name() const {
    return "SearchStrategy("
           + std::to_string(start_depth)
           + ", decay = " + std::to_string(score_decay)
           + ", mdisc = " + std::to_string(multiple_score_discount)
           + ")";
  }

  clock_t startTime = 0;

  using HashType = std::pair<long long, long long>;

  HashType combine(const HashType& h1, const HashType& h2) {
    return std::make_pair(h1.first | h2.first, h1.second | h2.second);
  }

  HashType hashMove(int h, int w, int id) const {
    int val = ((id - 1) + (h + w * 7) * 2);
    int part1 = val % 63;
    int part2 = val - (val % 63);

    return std::make_pair(1ll << part1, 1ll << part2);
  }

  std::unordered_map<HashType, float, pair_hash> scoreCache;

  int solve(Field field, int id) {
    startTime = clock();

    std::vector<int> availableW;
    int availableSize;
    for (int w = 0; w < W; ++w) {
      if (!isFull(field, w)) {
        availableW.push_back(w);
      }
    }
    availableSize = static_cast<int>(availableW.size());
    HashType stateHash;

    int bestW = NO_ACTION;
    int d = start_depth;
    for (; d < 8 * 8; ++d) {
      scoreCache.clear();
      auto decision = search(field, id, d, availableW, availableSize, stateHash);
      if (decision.w != TIMEOUT_ACTION) {
        bestW = decision.w;
      } else {
        break;
      }
      assert(availableW.size() == availableSize);

      if (print_debug_info) {
        std::cerr << "w: " << decision.w << std::endl;
        std::cerr << "score: " << decision.score << std::endl;
        std::cerr << (clock() - startTime) * 1.0 / CLOCKS_PER_SEC << std::endl;
        std::cerr << "Depth: " << start_depth << std::endl;
        std::cerr << "---------" << std::endl;
      }
    }
    return bestW;
  }

  Decision
  search(Field& field, int id, int max_depth, std::vector<int>& availableW, int& availableSize,
         HashType stateHash) {
    clock_t currentTime = clock();
    clock_t ms = (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;
    if (ms > max_turn_time * 0.9) {
      return Decision(TIMEOUT_ACTION, INF);
    }

    if (availableSize == 0) {
      return Decision(NO_ACTION, 0);
    }

    if (with_cache) {
      auto it = scoreCache.find(stateHash);
      if (it != scoreCache.end()) {
        return Decision(IN_CACHE_ACTION, it->second);
      }
    }

    if (availableSize > 1 && max_depth <= 0) {
      return Decision(NO_ACTION, scorer->score(field, id));
    }

    int enemy_id = oppositePlayer(id);
    int minW = NO_ACTION;
    float minScore = 0;
    int minScoreCount = 0;
    for (int iw = 0; iw < availableSize; ++iw) {
      int w = availableW[iw];

      // Change
      int h = putInplace(field, w, id);
      if (h == 0) {
        std::swap(availableW[iw], availableW[availableSize - 1]);
        --availableSize;
      }

      HashType addHash = hashMove(h, w, id);

      int winner = posIsWinning(field, h, w);
      Decision decision;
      if (winner == -1) {
        decision = search(field, enemy_id, max_depth - 1, availableW, availableSize,
                          combine(stateHash, addHash));
      } else {
        if (winner == id) {
          decision = Decision(NO_ACTION, -INF);
        } else {
          decision = Decision(NO_ACTION, INF);
        }
      }

      // Restore
      field[h][w] = 0;
      if (h == 0) {
        ++availableSize;
        std::swap(availableW[iw], availableW[availableSize - 1]);
      }

      if (decision.w == TIMEOUT_ACTION) {
        return Decision(TIMEOUT_ACTION, INF);
      }

      if (minW == NO_ACTION || decision.score <= minScore) {
        minW = w;
        if (decision.score == minScore) {
          ++minScoreCount;
        } else {
          minScore = decision.score;
          minScoreCount = 1;
        }
      }
    }
    float finalScore = -minScore * score_decay;
    finalScore *= (1 + (minScoreCount - 1) * multiple_score_discount);

    if (with_cache) {
      scoreCache[stateHash] = finalScore;
    }

    return Decision(minW, finalScore);
  }

private:
  int player_id;
  int start_depth;
  int max_turn_time;
  bool with_cache;
  bool print_debug_info;
  float score_decay;
  float multiple_score_discount;
  std::unique_ptr<Scorer> scorer;
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_STRATEGY_PLAYER_HPP
