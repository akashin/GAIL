//
// Created by acid on 10/25/17.
//

#ifndef GAIL_PLAYERS_HPP
#define GAIL_PLAYERS_HPP

#include <algorithm>
#include <ctime>
#include <cmath>
#include <limits>
#include <random>

#include "clients.hpp"
#include "simulator.hpp"
#include "../../core/player.hpp"
#include "code_vs_zombies.hpp"
#include "point2d.hpp"

namespace gail {
namespace code_vs_zombies {

class PickSimulate : public Player<State, Action> {
public:
  Action takeAction(const State& state) override {
    std::mt19937 rnd;
    std::uniform_int_distribution<int> width_distribution(0, W - 1);
    std::uniform_int_distribution<int> hight_distribution(0, H - 1);
    std::vector<Action> best;
    int best_score = 0;
    auto up_best = [&](const std::vector<Action>& actions) {
      std::vector<Action> a;
      a.emplace_back(Action(state.me));
      for (const Action& action: actions) {
        while (!(a.back().target == action.target)) {
          a.emplace_back(Action(roundTo(a.back().target, action.target, ASH_SPEED)));
        }
      }
      a.erase(a.begin());
      if (a.empty())
        return;
      Simulator sim(state);
      int s0 = sim.getScore();
      int turns = 0;
      for (size_t i = 0; i < a.size(); ++i) {
        if (sim.isFinished()) {
          a.erase(a.begin() + i, a.end());
        } else {
          turns += 1;
          sim.makeAction(a[i]);
        }
      }
      while (!sim.isFinished()) {
        if (++turns >= 50)
          return;
        sim.makeAction(a.back());
      }
      int score = sim.getScore();
      if (score > best_score) {
        best_score = score;
        best = a;
      }
    };
    for (const auto& human: state.humans) {
      up_best({Action(human.pos)});
    }
    for (int probe = 0; probe < 200; ++probe) {
      up_best({Action(Point2D(width_distribution(rnd), hight_distribution(rnd)))});
    }
    for (int probe = 0; probe < 150; ++probe) {
      up_best({
                  Action(Point2D(width_distribution(rnd), hight_distribution(rnd))),
                  Action(Point2D(width_distribution(rnd), hight_distribution(rnd)))
              });
    }
    for (int probe = 0; probe < 100; ++probe) {
      up_best({
                  Action(Point2D(width_distribution(rnd), hight_distribution(rnd))),
                  Action(Point2D(width_distribution(rnd), hight_distribution(rnd))),
                  Action(Point2D(width_distribution(rnd), hight_distribution(rnd)))
              });
    }
    std::cerr << "best_score = " << best_score << std::endl;
    std::cerr << "clock = " << clock() / (double) CLOCKS_PER_SEC << std::endl;
    return best.empty() ? Action(state.me) : best[0];
  }
};

class PickSafe : public Player<State, Action> {
public:
  Action takeAction(const State& state) override {
    std::vector<double> keys(state.humans.size());
    std::transform(state.humans.begin(), state.humans.end(), keys.begin(), [&](const Human& human) {
      int d2 = std::numeric_limits<int>::max();
      for (const auto& z: state.zombies) {
        int u2 = dist2(z.pos, human.pos);
        if (u2 < d2) d2 = u2;
      }
      return std::sqrt(dist2(human.pos, state.me)) / ASH_SPEED - std::sqrt(d2) / ZOMBIE_SPEED;
    });
    auto it = std::min_element(keys.begin(), keys.end());
    return Action(state.humans[it - keys.begin()].pos);
  }
};

class PickLastHumanPlayer : public Player<State, Action> {
public:
  Action takeAction(const State& state) override {
    return Action(state.humans.back().pos);
  }
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_PLAYERS_HPP
