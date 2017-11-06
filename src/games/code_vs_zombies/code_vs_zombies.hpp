//
// Created by acid on 11/6/17.
//

#ifndef GAIL_CODE_VS_ZOMBIES_HPP
#define GAIL_CODE_VS_ZOMBIES_HPP

#include "point2d.hpp"

namespace gail {
namespace code_vs_zombies {

const int W = 16000;
const int H = 9000;
const int ASH_SPEED = 1000;
const int ZOMBIE_SPEED = 400;
const int WEAPON_RANGE = 2000;

const int INVALID_ID = -1;

Point2D roundTo(Point2D from, Point2D to, int max_distance) {
  Point2D res(to.x - from.x, to.y - from.y);
  int d2 = dist2(res);
  if (d2 > max_distance * max_distance) {
    res.x *= static_cast<double>(max_distance) / std::sqrt(d2);
    res.y *= static_cast<double>(max_distance) / std::sqrt(d2);
  }
  res.x += from.x;
  res.y += from.y;
  if (res.x < 0) res.x = 0;
  if (res.x >= W) res.x = W - 1;
  if (res.y < 0) res.y = 0;
  if (res.y >= H) res.y = H - 1;
  return res;
}

struct Human {
  Human() = default;

  Human(int id, const Point2D& pos)
      : id(id), pos(pos) {}

  int id = INVALID_ID;
  Point2D pos;
};

struct Zombie {
  Zombie() = default;

  Zombie(int id, const Point2D& pos, const Point2D& next_pos)
      : id(id), pos(pos), next_pos(next_pos) {}

  int id = INVALID_ID;
  Point2D pos;
  Point2D next_pos;
};

class State {
public:
  Point2D me;
  std::vector<Human> humans;
  std::vector<Zombie> zombies;
};

std::istream& operator >> (std::istream& is, State& state) {
  is >> state.me.x >> state.me.y;
  is.ignore();
  int humanCount;
  is >> humanCount;
  is.ignore();
  state.humans.clear();
  for (int i = 0; i < humanCount; i++) {
    int humanId;
    Point2D humanPos;
    is >> humanId >> humanPos;
    is.ignore();
    state.humans.emplace_back(humanId, humanPos);
  }
  int zombieCount;
  is >> zombieCount;
  is.ignore();
  state.zombies.clear();
  for (int i = 0; i < zombieCount; i++) {
    int zombieId;
    Point2D zombiePos;
    Point2D zombieNextPos;
    is >> zombieId >> zombiePos >> zombieNextPos;
    is.ignore();
    state.zombies.emplace_back(zombieId, zombiePos, zombieNextPos);
  }
  return is;
}

class Action {
public:
  explicit Action(const Point2D& target)
      : target(target) {}

  Point2D target;
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_CODE_VS_ZOMBIES_HPP
