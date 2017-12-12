#ifndef GAIL_SCORE_UTILS_HPP
#define GAIL_SCORE_UTILS_HPP

namespace gail {
namespace fantastic_four {

const PlayerAction NO_ACTION{-1};
const PlayerAction TIMEOUT_ACTION{-2};
const PlayerAction IN_CACHE_ACTION{-3};

struct ActionWithScore {
  PlayerAction action;
  int score;
  ActionWithScore() :action(TIMEOUT_ACTION), score(0) {}
  ActionWithScore(int score) : action(NO_ACTION), score(score) {}
  ActionWithScore(PlayerAction action, int score) : action(action), score(score) {}
  bool operator < (const ActionWithScore& o) const {
    return score < o.score;
  }
};

} // namespace fantastic_four
} // namespace gail

#endif //GAIL_SCORE_UTILS_HPP
