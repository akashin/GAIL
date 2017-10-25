//
// Created by acid on 10/25/17.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include <vector>
#include <iostream>
#include "../../core/client.hpp"

namespace gail {
namespace code_vs_zombies {

struct Point2D {
  Point2D() = default;

  Point2D(int x, int y)
      : x(x), y(y) {}

  int x, y;
};

std::istream& operator >> (std::istream& is, Point2D& point) {
  is >> point.x >> point.y;
  return is;
}

struct Human {
  Human() = default;

  Human(int id, const Point2D& pos)
      : id(id), pos(pos) {}

  int id;
  Point2D pos;
};

struct Zombie {
  Zombie() = default;

  Zombie(int id, const Point2D& pos, const Point2D& next_pos)
      : id(id), pos(pos), next_pos(next_pos) {}

  int id;
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
