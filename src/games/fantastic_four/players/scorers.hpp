//
// Created by acid on 11/3/17.
//

#ifndef GAIL_SCORERS_HPP
#define GAIL_SCORERS_HPP

#include <cassert>
#include "../fantastic_four.hpp"
#include "hasher.hpp"

namespace gail {
namespace fantastic_four {

const int LARGE_SCORE = static_cast<int>(1e6);

struct Scorer {
  virtual int score(const Field& field, int id) = 0;
};

class ZeroScorer : public Scorer {
public:
  int score(const Field& field, int id) override {
    return 0;
  }
};

class SimpleScorer : public Scorer {
public:
  int score(const Field& field, int id) override {
    int score = 0;
    for (int h = 0; h < H; ++h) {
      for (int w = 0; w < W; ++w) {
        score += posScore(field, h, w, id);
      }
    }
    return score;
  }

private:
  int posScore(const Field& field, int h, int w, int id) {
    int score = 0;
    for (int dh = -1; dh <= 1; ++dh) {
      for (int dw = -1; dw <= 1; ++dw) {
        if (dh == 0 && dw == 0) {
          continue;
        }

        int cell_id = field[h][w];
        if (cell_id == 0) {
          continue;
        }

        int k = 0;
        int nh = h;
        int nw = w;
        while (k < 4
               && isOnField(nh, nw)
               && field[nh][nw] == field[h][w]) {
          ++k;
          nh = h + k * dh;
          nw = w + k * dw;
        }
        int sign = (cell_id == id) ? 1 : -1;
        int dirScore = 0;
        if (k >= 4) {
          dirScore = LARGE_SCORE;
        } else {
          if (isOnField(nh, nw) && field[nh][nw] == 0) {
            dirScore += k * k * k;
          }
        }
        score += dirScore * sign;
      }
    }
    return score;
  }
};

class OpportunityScorer
    : public Scorer {
public:
  int isWinning[H][W][2];
  bool blocked[H][W];

  int score(const Field& field_, int id) override {
    // TODO(akashin): Wierd that we need to copy that.
    Field field = field_;
    for (int h = 0; h < H; ++h) {
      for (int w = 0; w < W; ++w) {
        if (field[h][w] == 0) {
          for (int nId = 1; nId <= 2; ++nId) {
            field[h][w] = nId;
            isWinning[h][w][nId - 1] = (cellHasWinner(field, h, w) == nId);
          }
          field[h][w] = 0;
        } else {
          isWinning[h][w][0] = isWinning[h][w][1] = false;
        }
      }
    }

    int zid = id - 1;
    int winOpportunityScore = 0;
    for (int w = 0; w < W; ++w) {
      bool haveBlock = false;
      for (int h = H - 1; h >= 0; --h) {
        blocked[h][w] = haveBlock;

        if (field[h][w] == 0 && !haveBlock) {
          winOpportunityScore += isWinning[h][w][zid] * (h + 1);
          winOpportunityScore -= isWinning[h][w][1 - zid] * (h + 1);

          if (isWinning[h][w][0] && isWinning[h][w][1]) {
            haveBlock = true;
          }
          if (h < H - 1) {
            if ((isWinning[h][w][0] && isWinning[h + 1][w][0])
                || (isWinning[h][w][1] && isWinning[h + 1][w][1])) {
              haveBlock = true;
            }
          }
        }
      }
    }
    int wosSign = 1;
    if (winOpportunityScore < 0) {
      wosSign = -1;
    }

    int posScores = 0;
    for (int h = 0; h < H; ++h) {
      for (int w = 0; w < W; ++w) {
        if (!blocked[h][w]) {
          posScores += posScore(field, h, w, id);
        }
      }
    }
    return posScores + winOpportunityScore * winOpportunityScore * wosSign;
  }

private:
  int posScore(const Field& field, int h, int w, int id) {
    int cell_id = field[h][w];

    bool infinite_reward_given = false;
    int score = 0;
    for (int dh = -1; dh <= 1; ++dh) {
      for (int dw = 0; dw <= 1; ++dw) {
        if (dh == 0 && dw == 0) {
          continue;
        }

        for (int look_id = 1; look_id <= 2; ++look_id) {
          int k1 = 1;
          {
            int nh = h + dh;
            int nw = w + dw;
            while (isOnField(nh, nw) && field[nh][nw] == look_id) {
              ++k1;
              nh += dh;
              nw += dw;
            }
          }
          int k2 = 1;
          {
            int nh = h - dh;
            int nw = w - dw;
            while (isOnField(nh, nw) && field[nh][nw] == look_id) {
              ++k2;
              nh -= dh;
              nw -= dw;
            }
          }
          int nh_p = h + k1 * dh;
          int nw_p = w + k1 * dw;
          int nh_m = h - k2 * dh;
          int nw_m = w - k2 * dw;

          bool canPlace_p =
              isOnField(nh_p, nw_p) && (field[nh_p][nw_p] == 0) && !blocked[nh_p][nw_p];
          bool canPlace_m =
              isOnField(nh_m, nw_m) && (field[nh_m][nw_m] == 0) && !blocked[nh_m][nw_m];
          //bool canPlace_p = isOnField(nh_p, nw_p) && (field[nh_p][nw_p] == 0);
          //bool canPlace_m = isOnField(nh_m, nw_m) && (field[nh_m][nw_m] == 0);

          int sign = (look_id == id) ? 1 : -1;
          int dirScore = 0;

          int len = (k1 + k2 - 1);

          if (cell_id == look_id) {
            if (len >= 4) {
              return sign * LARGE_SCORE;
            } else {
              int canPlaceBonus = (canPlace_p + canPlace_m) * (canPlace_p + canPlace_m);
              dirScore = len * len * len * canPlaceBonus;
            }
          } else if (cell_id == 0) {
            dirScore = (len - 1) * (len - 1) * (1 + canPlace_p + canPlace_m);
          }
          score += dirScore * sign;
        }
      }
    }
    return score;
  }
};


class LineLengthScorer : public Scorer {
public:
  int score(const Field& field, int id) override {
    int score = 0;
    for (int h = 0; h < H; ++h) {
      for (int w = 0; w < W; ++w) {
        score += posScore(field, h, w, id);
      }
    }
    return score;
  }

private:
  int posScore(const Field& field, int h, int w, int id) {
    int score = 0;
    for (int dh = -1; dh <= 1; ++dh) {
      for (int dw = 0; dw <= 1; ++dw) {
        if (dh == 0 && dw == 0) {
          continue;
        }

        int cell_id = field[h][w];

        for (int look_id = 1; look_id <= 2; ++look_id) {
          int k1 = 1;
          {
            int nh = h + dh;
            int nw = w + dw;
            while (isOnField(nh, nw) && field[nh][nw] == look_id) {
              ++k1;
              nh += dh;
              nw += dw;
            }
          }
          int k2 = 1;
          {
            int nh = h - dh;
            int nw = w - dw;
            while (isOnField(nh, nw) && field[nh][nw] == look_id) {
              ++k2;
              nh -= dh;
              nw -= dw;
            }
          }
          int nh_p = h + k1 * dh;
          int nw_p = w + k1 * dw;
          int nh_m = h - k2 * dh;
          int nw_m = w - k2 * dw;

          bool canPlace_p = isOnField(nh_p, nw_p) && (field[nh_p][nw_p] == 0);
          bool canPlace_m = isOnField(nh_m, nw_m) && (field[nh_m][nw_m] == 0);

          int sign = (look_id == id) ? 1 : -1;
          int dirScore = 0;

          int len = (k1 + k2 - 1);

          if (cell_id == look_id) {
            if (len >= 4) {
              dirScore = LARGE_SCORE;
            } else {
              dirScore = len * len * len * len * (canPlace_p + canPlace_m);
            }
          } else if (cell_id == 0) {
            dirScore = (len - 1) * (len - 1) * (len - 1) * (1 + canPlace_p + canPlace_m);
          }
          score += dirScore * sign;
        }
      }
    }
    return score;
  }
};

template<typename Rnd>
class MCScorer : public Scorer {
protected:
  Rnd& rnd;
  virtual size_t rnd_index(size_t n) {
    return std::uniform_int_distribution<size_t>(0, n - 1)(rnd);
  }
public:
  MCScorer(Rnd& rnd) : Scorer(), rnd(rnd) {}
  virtual ~MCScorer() = default;
  int score(const Field& field, int id) override {
    Simulator sim(field);
    std::array<int, W> moves;
    int p = sim.deduceExpectedPlayer(field);
    while (sim.getState().winner == NO_PLAYER) {
      size_t cmoves = 0;
      for (int i = 0; i < W; ++i) {
        if (sim.isValidAction(Action(p, i))) {
          moves[cmoves++] = i;
        }
      }
      int move = moves[rnd_index(cmoves)];
      sim.makeAction({p, move});
      p = oppositePlayer(p);
    }
    int w = sim.getState().winner;
    if (w == id) return LARGE_SCORE;
    if (w == oppositePlayer(id)) return -LARGE_SCORE;
    assert(w == DRAW);
    return 0;
  }
};

template<typename Rnd>
class MCScorerOpt : public MCScorer<Rnd> {
protected:
  using MCScorer<Rnd>::rnd;
  typename Rnd::result_type data;
  int data_bits;
  typename Rnd::result_type get_bits(int bits) {
    if (data_bits < bits) {
      auto r = data & ((1 << data_bits) - 1);
      data = rnd();
      r |= (data & ((1 << (bits - data_bits)) - 1)) << (data_bits);
      data >>= bits - data_bits;
      data_bits = data_bits + sizeof(data) * 8 - bits;
      return r;
    }
    auto r = (data) & ((1 << bits) - 1);
    data >>= bits;
    data_bits -= bits;
    return r;
  }

  virtual size_t rnd_index(size_t n) override {
    int bt = 0;
    while ((1 << bt) < n) ++bt;
    size_t i;
    do {
      i = get_bits(bt);
    } while (i >= n);
    return i;
  }
public:
  MCScorerOpt(Rnd& rnd) : MCScorer<Rnd>(rnd), data(), data_bits(0) {
  }
};

template<typename Rnd>
class MCPPAScorer : public Scorer { // Playout Policy Adaptation with Move Features
protected:
  Rnd& rnd;
  std::array<std::array<double, W * H>, 2> policy;
  std::array<std::array<double, W * H>, 2> policy_exp;
  double alpha;
  void adapt(const Field& field,
      std::array<std::pair<int, int>, W * H> history,
      size_t chistory,
      int player,
      int winner) {
    if(winner != 1 && winner != 2)
      return;
    Simulator sim(field);
    std::array<double, W * H> polp = policy[winner - 1];
    for (int i = 0; i < chistory; ++i) {
      auto move = history[i];
      auto code = move.first + move.second * W;
      if (player != winner) { // WTF
        polp[code] += alpha;
        std::array<int, W> codes;
        size_t ncodes = 0;
        for (int j = 0; j < W; ++j) {
          if (sim.isValidAction(Action(player, j))) {
            codes[ncodes++] = j + upperRow(sim.getState().field, j) * W;
          }
        }
        double z = 0;
        for (int j = 0; j < ncodes; ++j) {
          z += policy_exp[winner - 1][codes[j]];
        }
        for (int j = 0; j < ncodes; ++j) {
          polp[codes[j]] -= alpha * policy_exp[winner - 1][codes[j]] / z;
        }
      }
      sim.makeAction(Action(player, history[i].first));
      player = oppositePlayer(player);
    }
    for (int i = 0; i < polp.size(); ++i) {
      policy[winner - 1][i] = polp[i];
      policy_exp[winner - 1][i] = exp(polp[i]);
    }
  }
  int get_move(std::array<std::pair<int, int>, W> moves, size_t cmoves, int player) {
    // return std::uniform_int_distribution<int>(0, cmoves -1)(rnd);
    double z = 0;
    for (int i = 0; i < cmoves; ++i) {
      int code = moves[i].first + moves[i].second;
      z += policy_exp[player - 1][code];
    }
    double w = std::uniform_real_distribution<double> (0, z)(rnd);
    for (int i = 0; i < cmoves; ++i) {
      int code = moves[i].first + moves[i].second;
      double p = policy_exp[player - 1][code];
      if (w < p) return i;
      w -= p;
    }
    return cmoves - 1;
  }
public:
  MCPPAScorer(Rnd& rnd, double alpha = 0.01) : Scorer(), rnd(rnd), alpha(alpha) {
    reset();
  }
  void reset() {
    policy[0].fill(0.0);
    policy[1].fill(0.0);
    policy_exp[0].fill(1.0);
    policy_exp[1].fill(1.0);
  }
  virtual ~MCPPAScorer() = default;
  int score(const Field& field, int id) override {
    Simulator sim(field);
    std::array<std::pair<int, int>, W> moves;
    int p = sim.deduceExpectedPlayer(field);
    assert(p == id);
    std::array<std::pair<int, int>, W * H> history;
    size_t chistory = 0;
    while (sim.getState().winner == NO_PLAYER) {
      size_t cmoves = 0;
      for (int i = 0; i < W; ++i) {
        if (sim.isValidAction(Action(p, i))) {
          moves[cmoves] = {i, upperRow(sim.getState().field, i)};
          cmoves += 1;
        }
      }
      int i = get_move(moves, cmoves, p);
      history[chistory++] = moves[i];
      sim.makeAction({p, moves[i].first});
      p = oppositePlayer(p);
    }
    int w = sim.getState().winner;
    adapt(field, history, chistory, id, w);
    if (w == id) return LARGE_SCORE;
    if (w == oppositePlayer(id)) return -LARGE_SCORE;
    assert(w == DRAW);
    return 0;
  }
};

template<typename Rnd>
class MCIPPScorer : public Scorer { // donts works well yet
protected:
  Rnd& rnd;
  virtual size_t rnd_index(size_t n) {
    return std::uniform_int_distribution<size_t>(0, n - 1)(rnd);
  }
public:
  MCIPPScorer(Rnd& rnd) : Scorer(), rnd(rnd) {}
  virtual ~MCIPPScorer() = default;
  int score(const Field& field, int id) override {
    Simulator sim(field), psim;
    std::array<int, W> moves;
    std::array<int, W> pmoves;
    size_t cpmoves = 0;
    int p = sim.deduceExpectedPlayer(field);
    int pmove;
    while (sim.getState().winner == NO_PLAYER) {
      int move = -1;
      size_t cmoves = 0;
      for (int i = 0; i < W; ++i) {
        if (sim.isValidAction(Action(p, i))) {
          Simulator s(sim);
          s.makeAction(Action(p, i));
          if (s.getState().winner == p) {
            move = i;
            break;
          } else if (true || s.getState().winner != oppositePlayer(p)) {
            // impossible to lose
            moves[cmoves++] = i;
          }
        }
      }
      if (move == -1) {
        move = moves[rnd_index(cmoves)];
      } else if (cpmoves > 1) {
        int i = std::find(pmoves.begin(), pmoves.begin() +
            cpmoves, pmove) - pmoves.begin();
        assert(i < cpmoves);
        std::swap(pmoves[i], pmoves[--cpmoves]);
        pmove = pmoves[rnd_index(cpmoves)];
        p = oppositePlayer(p);
        sim = psim;
        sim.makeAction(Action(p, pmove));
        p = oppositePlayer(p);
        continue;
      }
      std::copy(moves.begin(), moves.begin() + cmoves, pmoves.begin());
      cpmoves = cmoves;
      psim = sim;
      pmove = move;
      sim.makeAction({p, move});
      p = oppositePlayer(p);
    }
    int w = sim.getState().winner;
    if (w == id) return LARGE_SCORE;
    if (w == oppositePlayer(id)) return -LARGE_SCORE;
    assert(w == DRAW);
    return 0;
  }
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_SCORERS_HPP
