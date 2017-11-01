//
// Created by acid on 10/25/17.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include <vector>
#include <iostream>
#include <cmath>

#include "../../core/client.hpp"
#include "code_vs_zombies.hpp"

namespace gail {
namespace code_vs_zombies {

const int INVALID_ID = -1;

struct Point2D {
  Point2D(int x, int y)
      : x(x), y(y) {}

  Point2D(): Point2D(0, 0) {}

  int x, y;

  bool operator == (const Point2D& other) const {
    return x == other.x && y == other.y;
  }
};

int dist2(const Point2D& p) {
  return p.x * p.x + p.y * p.y;
}

int dist2(const Point2D& p, const Point2D& q) {
  return dist2(Point2D(p.x - q.x, p.y - q.y));
}

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

std::istream& operator >> (std::istream& is, Point2D& point) {
  is >> point.x >> point.y;
  return is;
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

class Action {
public:
  explicit Action(const Point2D& target)
      : target(target) {}

  Point2D target;
};

class StreamClient : public Client<State, Action> {
public:
  StreamClient(std::istream& state_input_stream, std::ostream& action_output_stream)
      : state_input_stream(state_input_stream), action_output_stream(action_output_stream) {}

  ~StreamClient() override = default;

  int getScore() override {
    return 0;
  }

  bool isGameFinished() override {
    return false;
  }

  State getState() override {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const Action& action) override {
    action_output_stream << action.target.x << " " << action.target.y << std::endl;
    state_refreshed = false;
  }

private:
  void readState() {
    state_input_stream >> state.me.x >> state.me.y;
    state_input_stream.ignore();
    int humanCount;
    state_input_stream >> humanCount;
    state_input_stream.ignore();
    state.humans.clear();
    for (int i = 0; i < humanCount; i++) {
      int humanId;
      Point2D humanPos;
      state_input_stream >> humanId >> humanPos;
      state_input_stream.ignore();
      state.humans.emplace_back(humanId, humanPos);
    }
    int zombieCount;
    state_input_stream >> zombieCount;
    state_input_stream.ignore();
    state.zombies.clear();
    for (int i = 0; i < zombieCount; i++) {
      int zombieId;
      Point2D zombiePos;
      Point2D zombieNextPos;
      state_input_stream >> zombieId >> zombiePos >> zombieNextPos;
      state_input_stream.ignore();
      state.zombies.emplace_back(zombieId, zombiePos, zombieNextPos);
    }
  }

  std::istream& state_input_stream;
  std::ostream& action_output_stream;
  bool state_refreshed = false;
  State state;
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_CLIENTS_HPP
