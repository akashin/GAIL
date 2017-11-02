//
// Created by acid on 11/7/17.
//

#ifndef GAIL_CHALLENGE_HPP
#define GAIL_CHALLENGE_HPP

namespace gail {

//struct ChallengeResult {
//  ChallengeResult(int opponentCount) {
//    for (int i = 0; i < 2; ++i) {
//      gameResults[i].resize(opponentCount);
//    }
//  }
//
//  std::vector<GameResult> gameResults[2];
//};

//void challengeStrategy(
//    Strategy& strategy,
//    const vector<unique_ptr<Strategy>>& opponentStrategies,
//    bool detailed = false) {
//  auto challengeResult = ChallengeResult(opponentStrategies.size());
//
//  int wins = 0;
//  for (int i = 0; i < opponentStrategies.size(); ++i) {
//    cerr << i << " ";
//    if (detailed) {
//      cerr << "Playing against " << opponentStrategies[i]->name() << " ..." << endl;
//    }
//    auto *firstPlayer = &strategy;
//    auto *secondPlayer = opponentStrategies[i].get();
//    for (int match = 0; match <= 1; ++match) {
//      if (match == 1) {
//        swap(firstPlayer, secondPlayer);
//      }
//      auto gameResult = play(Field{}, *firstPlayer, *secondPlayer);
//      challengeResult.gameResults[match][i] = gameResult;
//      if (gameResult.winner == match + 1) {
//        ++wins;
//      }
//    }
//    if (detailed) {
//      cerr << "Current win rate: " << wins / (2.0 * (i + 1)) << endl;
//    }
//  }
//  cerr << endl;
//
//  cout << "Challenge results for " << strategy.name() << ": " << endl;
//  cout << "Win rate: " << wins / (2.0 * opponentStrategies.size()) << endl;
//
//  if (detailed) {
//    for (int match = 0; match <= 1; ++match) {
//      cout << "Playing " << (match ? "second" : "first") << endl;
//      for (int i = 0; i < challengeResult.gameResults[match].size(); ++i) {
//        auto& result = challengeResult.gameResults[match][i];
//        cout << "Winner: " << result.winner << ", turn_count: " << result.steps << endl;
//        cout << result.field;
//      }
//    }
//  }
//}

}; // namespace gail

#endif //GAIL_CHALLENGE_HPP
