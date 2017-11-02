//
// Created by acid on 11/3/17.
//

#ifndef GAIL_SCORERS_HPP
#define GAIL_SCORERS_HPP

#include "fantastic_four.hpp"

namespace gail {
namespace fantastic_four {

const int LARGE_SCORE = static_cast<int>(1e9);

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

class VeryCleverScorer
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
          for (int id = 1; id <= 2; ++id) {
            field[h][w] = id;
            isWinning[h][w][id - 1] = (posIsWinning(field, h, w) == id);
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


class CleverScorer : public Scorer {
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

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_SCORERS_HPP
