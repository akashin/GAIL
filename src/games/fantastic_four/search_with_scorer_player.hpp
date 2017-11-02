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

namespace gail {
namespace fantastic_four {

// TODO(akashin): Pass this as the parameter to the player?
const bool MY_DEBUG = false;
const bool MY_DEBUG_DEPTH = false;

// TODO(akashin): This should be an input parameter.
const int MAX_TIME = 10;

const int NO_ACTION = -1;
const int TIMEOUT_ACTION = -2;

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
//const float MULTIPLE_SCORE_DISCOUNT = 0.4;
const float MULTIPLE_SCORE_DISCOUNT = 0.5f / (W - 1);

class SearchWithScorerPlayer : public Player<PlayerState, PlayerAction> {
public:
  SearchWithScorerPlayer(int depth, int player_id, std::unique_ptr<Scorer> scorer)
      : depth(depth), player_id(player_id), scorer(std::move(scorer)) {}

  PlayerAction takeAction(const PlayerState& state) override {
    return PlayerAction(solve(state.field, player_id));
  }

  std::string name() const {
    return "SearchStrategy("
           + std::to_string(depth)
           + ", decay = " + std::to_string(scoreDecay)
           + ", mdisc = " + std::to_string(multipleScoreDiscout)
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
    int d = depth;
    for (; d < 8 * 8; ++d) {
      scoreCache.clear();
      auto decision = search(field, id, d, availableW, availableSize, stateHash);
      if (decision.w != TIMEOUT_ACTION) {
        bestW = decision.w;
      } else {
        break;
      }
      assert(availableW.size() == availableSize);

      if (MY_DEBUG_DEPTH) {
        std::cerr << "w: " << decision.w << std::endl;
        std::cerr << "score: " << decision.score << std::endl;
        std::cerr << (clock() - startTime) * 1.0 / CLOCKS_PER_SEC << std::endl;
        std::cerr << "Depth: " << depth << std::endl;
        std::cerr << "---------" << std::endl;
      }
    }
    return bestW;
  }

  Decision search(Field& field, int id, int max_depth, std::vector<int>& availableW, int&
  availableSize,
                  HashType stateHash) {
    clock_t currentTime = clock();
    clock_t ms = (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;
    if (ms > MAX_TIME * 0.9) {
      return Decision(TIMEOUT_ACTION, INF);
    }

    if (availableSize == 0) {
      return Decision(NO_ACTION, 0);
    }

    if (withCache) {
      auto it = scoreCache.find(stateHash);
      if (it != scoreCache.end()) {
        return Decision(-3, it->second);
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
      //cerr << "Add hash " << addHash << endl;

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
      if (MY_DEBUG) {
        if (max_depth >= 5) {
          std::cerr << "depth: " << max_depth
                    << ", w: " << w
                    << ", score: " << decision.score
                    << ", opponent.w: " << decision.w
                    << std::endl;
          std::cerr << field;
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

      //if (decision.w == -3) {
      //cerr << "Got cached value " << decision.score << endl;
      //}

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
    float finalScore = -minScore * scoreDecay;
    finalScore *= (1 + (minScoreCount - 1) * multipleScoreDiscout);

    if (withCache) {
      scoreCache[stateHash] = finalScore;
    }

    return Decision(minW, finalScore);
  }

  void setWithCache(bool value) {
    withCache = value;
  }

  void setScoreDecay(float value) {
    scoreDecay = value;
  }

  void setMultipleScoreDiscount(float value) {
    multipleScoreDiscout = value;
  }

private:
  int player_id;
  int depth;
  bool withCache = false;
  float scoreDecay = SCORE_DECAY;
  float multipleScoreDiscout = MULTIPLE_SCORE_DISCOUNT;
  std::unique_ptr<Scorer> scorer;
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_STRATEGY_PLAYER_HPP
