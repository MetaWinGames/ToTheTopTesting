#include "config.h"

Config::Config(const std::string& file) {
    config = YAML::LoadFile(file);
    std::vector<int> payline;

    // REELS
    if (config["wheels"] && config["wheels"].IsMap()) {
        for (const auto &node: config["wheels"]) {
            std::string wheelName = node.first.as<std::string>();   // Reel Name
            std::vector<std::vector<int>> wheelsData;               // Reel Data

            for (const auto &wheel: config["wheels"][wheelName]) {
                std::vector<int> wheelVec;
                for (const auto &value: wheel) {
                    wheelVec.push_back(value.as<int>());
                }
                wheelsData.push_back(wheelVec);
            }
            AllReels[wheelName] = wheelsData;
        }
    }
    // PAYLINES
    for (const auto &line: config["pay_lines"]) {
        std::vector<int> lineVec;
        for (const auto &value: line) {
            lineVec.push_back(value.as<int>());
        }
        paylines.push_back(lineVec);
    }
    // PAYTABLE
    if (config["pay_table"]) {
        for (const auto &entry: config["pay_table"]) {
            for (const auto &value: entry.second) {
                payline.push_back(value.as<int>());
            }
            paytable.push_back(payline);
            payline.clear();

        }
    }

    // JACKPOT MULTIPLIERS
    jackpot_multipliers = config["feature"]["jackpot_multipliers"].as<std::vector<int>>();
    // COINS
    coin_multipliers = config["feature"]["coin_payouts"]["multiplier"].as<std::vector<int>>();
    // COIN PAYOUT WEIGHTS
    if (config["feature"]["coin_payout_weights"]) {
        for (const auto& entry: config["feature"]["coin_payout_weights"]) {
            int total_weight= 0;

            for (const auto& value : entry.second) {
                total_weight += value.as<int>();
                payline.push_back(value.as<int>());

            }
            coin_payout_total_weights.push_back(total_weight);
            coin_payout_weights.push_back(payline);
            payline.clear();
        }
    }
    // COIN PROBABILITY DISTRIBUTIONS
    // COIN PROBABILITY DISTRIBUTIONS
    if (config["feature"]["coin_probability_distribution"]) {
        for (const auto& entry: config["feature"]["coin_probability_distribution"]) {
            std::vector<double> probline;
            for (const auto& value : entry.second) {
                probline.push_back(value.as<double>());
            }
            coin_probability_distributions.push_back(probline);
            probline.clear();


        }
    }


    if (config["feature"]["base_coin_payout_weights"] && config["feature"]["base_coin_payout_weights"].IsMap()) {
        for (const auto &node: config["feature"]["base_coin_payout_weights"]) {
            int reel_id = node.first.as<int>();   // Reel Name

            std::vector<int> wheelVec;

            for (const auto &value: node.second) {
                wheelVec.push_back(value.as<int>());
                sum_base_coin_payout_weights[reel_id] += value.as<int>();

            }
            base_coin_payout_weights[reel_id] = wheelVec;
            wheelVec.clear();

        }
    }

    if (config["feature"]["feature_coin_payout_weights"] && config["feature"]["feature_coin_payout_weights"].IsMap()) {
        for (const auto &node: config["feature"]["feature_coin_payout_weights"]) {
            int reel_id = node.first.as<int>();
            std::vector<int> wheelVec;

            for (const auto &value: node.second) {
                wheelVec.push_back(value.as<int>());
                sum_feature_coin_payout_weights[reel_id] += value.as<int>();
            }
            feature_coin_payout_weights[reel_id] = wheelVec;
            wheelVec.clear();

        }
    }

    // Coin trigger states & trigger weights
    if (config["coin_trigger_states"]) {
        for (const auto &entry: config["coin_trigger_states"]) {
            std::vector<int> state;
            int max_count = 0;
            for (const auto &value: entry) {
                if (max_count <= 4) {
                    state.push_back(value.as<int>());
                } else if (max_count == 5) {
                    coin_triggering_weight += value.as<long long int>();
                    coin_trigger_weights.push_back(value.as<int>());
                }
                max_count++;
            }
            coin_trigger_states.push_back(state);
            state.clear();
        }

    }



    if (config["DEBUG"]["statistics_iteration"]) {
        Debug.statistics_iteration = config["DEBUG"]["statistics_iteration"].as<long long int>();
    }
    if (config["DEBUG"]["free_games_statistics_iteration"]) {
        Debug.free_games_statistics_iteration = config["DEBUG"]["free_games_statistics_iteration"].as<long long int>();
    }
    if (config["DEBUG"]["coin_games_statistics_iteration"]) {
        Debug.coin_games_statistics_iteration = config["DEBUG"]["coin_games_statistics_iteration"].as<long long int>();
    }


}

std::map<std::string, std::vector<std::vector<int>>> Config::Reels() const {
    return AllReels;
}

std::vector<std::vector<int>> Config::GetWheel(const std::string& name) {
    for (const auto& pair: AllReels) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    throw std::invalid_argument("Reel name " + name +    " cannot be found");
}

std::vector<std::vector<int>> Config::GetPaylines() const {
    return paylines;
}




