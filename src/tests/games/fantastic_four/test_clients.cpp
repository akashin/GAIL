//
// Created by acid on 11/24/17.
//

#include <gtest/gtest.h>

#include "../../../games/fantastic_four/clients.hpp"
#include "../../../core/match.hpp"

using namespace gail::fantastic_four;

class SequencePlayer : public gail::Player<PlayerState, PlayerAction> {
public:
  explicit SequencePlayer(std::vector<int> actions): actions_(std::move(actions)) {}

  PlayerAction takeAction(const PlayerState& state) override {
    return PlayerAction(actions_.at(turn_++));
  }

  std::vector<int> actions_;
  size_t turn_ = 0;
};

TEST(ClientsTest, SimulatorClientMatchFirstWins) { // NOLINT
  Simulator simulator;
  SimulatorClient first_client(FIRST_PLAYER, simulator);
  SimulatorClient second_client(SECOND_PLAYER, simulator);

  SequencePlayer first_player({0, 0, 0, 0});
  SequencePlayer second_player({1, 1, 1, 1});

  auto match_results = gail::playMatch<PlayerState, PlayerAction>({&first_client, &second_client},
                                                                  {&first_player, &second_player});

  EXPECT_EQ(5, match_results.turn_count);
  EXPECT_EQ(+1, match_results.scores[0]);
  EXPECT_EQ(-1, match_results.scores[1]);
}

TEST(ClientsTest, SimulatorClientMatchSecondWins) { // NOLINT
  Simulator simulator;
  SimulatorClient first_client(FIRST_PLAYER, simulator);
  SimulatorClient second_client(SECOND_PLAYER, simulator);

  SequencePlayer first_player({0, 0, 0, 1, 1});
  SequencePlayer second_player({4, 5, 6, 7, 6});

  auto match_results = gail::playMatch<PlayerState, PlayerAction>({&first_client, &second_client},
                                                                  {&first_player, &second_player});

  EXPECT_EQ(5, match_results.turn_count);
  EXPECT_EQ(-1, match_results.scores[0]);
  EXPECT_EQ(+1, match_results.scores[1]);
}

