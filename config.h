#ifndef TOTHETOPTESTING_CONFIG_H
#define TOTHETOPTESTING_CONFIG_H

#include <vector>
#include <chrono>
#include <future>
#include <thread>
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <map>


struct DEBUG {
    long long int statistics_iteration;
    long long int free_games_statistics_iteration;
    long long int coin_games_statistics_iteration;

};

struct CoinBuyFeature {
    std::map<std::string,  std::vector<std::vector<int>>> BuyOptions;
};


class Config {
public:
    CoinBuyFeature CoinBuy;
    Config(const std::string& file);
    DEBUG Debug;
    std::map<std::string, std::vector<std::vector<int>>> Reels() const;
    std::vector<std::vector<int>> GetWheel(const std::string& name);
    std::vector<std::vector<int>> GetPaylines() const;
    std::map<std::string, std::vector<std::vector<int>>> AllReels;
    std::vector<std::vector<int>> paylines;
    std::vector<std::vector<int>> paytable;
    std::vector<int> jackpot_multipliers;
    std::vector<int> coin_multipliers;
    std::vector<std::vector<double>> coin_probability_distributions;
    std::vector<std::vector<int>> coin_payout_weights;
    std::vector<int> coin_payout_total_weights;

    std::map<int, std::vector<int>> base_coin_payout_weights;
    std::vector<int> sum_base_coin_payout_weights = {0,0,0,0,0};

    std::map<int, std::vector<int>> feature_coin_payout_weights;
    std::vector<int> sum_feature_coin_payout_weights = {0,0,0,0,0};

    long long int coin_triggering_weight = 0;
    std::vector<std::vector<int>> coin_trigger_states;
    std::vector<int> coin_trigger_weights;



private:
    YAML::Node config;
};








#endif //TOTHETOPTESTING_CONFIG_H
