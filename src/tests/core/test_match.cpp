//
// Created by acid on 10/31/17.
//

#include <gtest/gtest.h>

#include "../../core/player.hpp"
#include "../../core/client.hpp"
#include "../../core/match.hpp"

namespace gail {
namespace {

struct State {
  int value = 0;
};

struct Action {
  int value = 0;
};

class TestClient : public Client<State, Action> {
public:
  int getScore() override {
    return last_action_value;
  }

  bool isGameFinished() override {
    return (last_action_value != 0);
  }

  State getState() override {
    return State{41};
  }

  void makeAction(const Action& action) override {
    last_action_value = action.value;
  }

  int last_action_value = 0;
};

class TestPlayer : public Player<State, Action> {
public:
  Action takeAction(const State& state) override {
    return Action{state.value + 1};
  }
};

TEST(MatchTest, SimpleRun) { // NOLINT
  TestClient client;
  TestPlayer player;
  auto scores = playMatch<State, Action>({&client}, {&player});
  EXPECT_EQ(42, scores.back());
}

};
}; // namespace gail

