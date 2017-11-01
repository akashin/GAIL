//
// Created by gchebanov on 31/10/2017.
//

#ifndef GAIL_SIMULATOR_HPP
#define GAIL_SIMULATOR_HPP

#include <algorithm>
#include <set>

#include "clients.hpp"

namespace gail {
namespace code_vs_zombies {

template <typename T, typename Funct>
int erase(std::vector<T>& v, Funct&& funct) {
  auto it = std::remove_if(v.begin(), v.end(), funct);
  int cnt = static_cast<int>(v.end() - it);
  v.erase(it, v.end());
  return cnt;
};

class Fibonnacci {
public:
  Fibonnacci() : f({1, 1}) {}
  int get(int n) const {
    while (f.size() <= n) {
      f.push_back(f[f.size() - 2] + f[f.size() - 1]);
    }
    return f.at(n);
  }
private:
  mutable std::vector<int> f;
};

struct Point2DCmp {
  bool operator ()(const Point2D& lhs, const Point2D rhs) const {
    return std::make_pair(lhs.x, lhs.y) <
           std::make_pair(rhs.x, rhs.y);
  }
};

class Simulator {
public:
  explicit Simulator(const State& state)
    : state(state)
    , score(0)
    , fibonnacci()
  {
  }
  Simulator() = delete;

  void makeAction(const Action& action) {
    validateAction(action);
    for (auto& zombie: state.zombies) {
      zombie.pos = zombie.next_pos;
    }
    state.me = action.target;
    int frags = erase(state.zombies, [=](const Zombie& zombie) {
      return dist2(state.me, zombie.pos) <= WEAPON_RANGE * WEAPON_RANGE;
    });
    {
      int score_mult = 0;
      for (int i = 0; i < frags; ++i) {
        score_mult += fibonnacci.get(i + 1);
      }
      score += score_mult * state.humans.size() * state.humans.size() * 10;
    }
    {
      zombie_pos.resize(state.zombies.size());
      for (size_t i = 0; i < state.zombies.size(); ++i) {
        zombie_pos[i]= state.zombies[i].pos;
      }
      sort(zombie_pos.begin(), zombie_pos.end(), Point2DCmp());
      erase(state.humans, [=](const Human& human) {
        return std::binary_search(zombie_pos.begin(), zombie_pos.end(), human.pos, Point2DCmp());
      });
    }
    for (auto& zombie: state.zombies) {
      Point2D next_pos(state.me);
      int d2 = dist2(state.me, zombie.pos);
      for (const auto& human: state.humans) {
        int u2 = dist2(human.pos, zombie.pos);
        if (u2 < d2) {
          next_pos = human.pos;
          d2 = u2;
        }
      }
      zombie.next_pos = roundTo(zombie.pos, next_pos, ZOMBIE_SPEED);
    }
  }

  bool isFinished() const {
    return state.humans.empty() || state.zombies.empty();
  }

  int getScore() const {
    int h = static_cast<int>(state.humans.size());
    if (h <= 0) return 0;
    return score;
  }

  State getState() const {
    return state;
  }

private:

  void validateAction(const Action& action) {
//    if (0 > action.target.x || action.target.x >= W)
//      throw std::logic_error("Wrong action target x: " + std::to_string(action.target.x));
//    if (0 > action.target.y || action.target.y >= H)
//      throw std::logic_error("Wrong action target y: " + std::to_string(action.target.y));
//    int d2 = dist2(action.target, state.me);
//    if (d2 > ASH_SPEED * ASH_SPEED)
//      throw std::logic_error("Wrong action target square dist is: " + std::to_string(d2));
  }

  State state;
  int score;
  Fibonnacci fibonnacci;
  std::vector<Point2D> zombie_pos;
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_SIMULATOR_HPP
