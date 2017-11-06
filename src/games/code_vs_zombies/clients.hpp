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
#include "simulator.hpp"

namespace gail {
namespace code_vs_zombies {

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

class SimulatorClient : public Client<State, Action> {
public:
  explicit SimulatorClient(Simulator& simulator)
      : simulator(simulator) {}

  int getScore() override {
    return simulator.getScore();
  }

  bool isGameFinished() override {
    return simulator.isFinished();
  }

  State getState() override {
    return simulator.getState();
  }

  void makeAction(const Action& action) override {
    simulator.makeAction(action);
  }

private:
  Simulator& simulator;
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_CLIENTS_HPP
