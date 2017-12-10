#ifndef GAIL_STRATEGY_MCTS_PLAYER_HPP
#define GAIL_STRATEGY_MCTS_PLAYER_HPP

#include "../simulator.hpp"
#include "../../../core/player.hpp"
#include "scorers.hpp"
#include "../clients.hpp"
#include "../../../utils/config.hpp"

namespace gail {
namespace fantastic_four {

template<typename RndGenerator>
class SearchWithScorerPlayer : public Player<PlayerState, PlayerAction> {
public:
    SearchWithScorerPlayer(int player_id, Config config)
        : player_id(player_id) {
        scorer.reset(config.get<Scorer *>("scorer"));
        max_turn_time = config.get<int>("max_turn_time");
        max_turn_sims = config.get<int>("max_turn_sims");
        print_debug_info = config.get("print_debug_info", false);
    }

    PlayerAction takeAction(const PlayerState &state) override {
        return PlayerAction(solve(state.field, player_id));
    }

    std::string name() const {
        return "MCTSPlayer("
               " max_turn_time = " + std::to_string(max_turn_time)
               + ", max_turn_sims = " + std::to_string(max_turn_sims)
               + ")";
    }
private:
    int player_id;
    int max_turn_time;
    int max_turn_sims;
    bool print_debug_info;
    std::unique_ptr<Scorer> scorer;
    clock_t startTime = 0;
};

} // namespace fantastic_four
} // namespace gail

#endif //GAIL_STRATEGY_MCTS_PLAYER_HPP
