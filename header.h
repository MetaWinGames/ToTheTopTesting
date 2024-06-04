#ifndef TOTHETOPTESTING_HEADER_H
#define TOTHETOPTESTING_HEADER_H

#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <execution>
#include <future>
#include <thread>
#include <set>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include <utility>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <windows.h>
#include <map>
#include <nlohmann/json.hpp>

struct WaysInfo {
    std::map<int, int> symbol_occurance;
    std::map<int, int> symbol_multipliers;
    std::map<int,int> symbol_points;
    std::vector<int> wilds_vector = {0,0,0,0,0};
};

struct Stats {

    std::vector<int> PointsArray;
    std::vector<int> PointsArrayNoBonus;

    std::vector<int> BasePointsArray;

    std::vector<int> FreeGamesPointsArray;


    std::vector<int> TotalCoinPointsArray;

    std::vector<int> CoinPointsArray;
    std::vector<int> JackpotCoinsPointsArray;

    std::map<int, long long int> CoinsEndingPointsDict;
    std::map<int, long long int> CoinsEndingHitDict;

    long long int SixStackOnReelHit[5] = {0};
    long long int FiveStackOnReelHit[5] = {0};

    // Base Game
    long long int ThreadBaseSymbolPoints[9][5] = {0};
    long long int ThreadBaseSymbolHit[9][5] = {0};


    // FreeGames Ways Games
    long long int ThreadFreeGamesSymbolPoints[10][5] = {0};
    long long int ThreadFreeGamesSymbolHit[10][5] = {0};
    long long int FreeGamesEndingCoins[11] = {0};
    long long int FreeGamesEndingCoinsRTP[11] = {0};
    long long int FreeGameHit = 0;
    long long int FreeGameTotalRounds = 0;


    long long int JackpotRTP[5] = {0};
    long long int JackpotHits[5] = {0};
    std::map<int, long long int> FreeGamesEndingWaysPointsDict;
    std::map<int, long long int> FreeGamesEndingWaysHitDict;


    long long int UnlockedPositions[5] = {0};
    long long int CPlay = 0;

    long long int coin_sum[5] = {0};
    long long int coin_sum_base[5] = {0};
    long long int coin_sum_feature[5]= {0};

    long long int total_coins_per_reel[5] = {0};
    long long int total_coins_per_reel_base[5] = {0};
    long long int total_coins_per_reel_feature[5] = {0};

    long long int ReelPoints[5] = {0};
};

struct CoinConfigs {
    std::vector<int> coin_multipliers;
    std::vector<std::vector<double>> coin_probability_distributions;
    std::vector<std::vector<int>> coin_payout_weights;
    std::vector<int> coin_payout_total_weights;

    int default_bet = 0;
    int play_bet = 0;

    std::vector<std::shared_ptr<std::uniform_int_distribution<>>> distributions;
    std::vector<std::shared_ptr<std::uniform_int_distribution<>>> base_coin_weight_distribution;
    std::vector<std::shared_ptr<std::uniform_int_distribution<>>> feature_coin_weight_distribution;

    std::vector<int> JackpotMultipliers;
    std::vector<std::vector<int>> cumulativeWeights;

    std::vector<std::vector<int>> coin_trigger_states;
    std::vector<int> coin_trigger_weights;
    long long int coin_triggering_weight = 0;
    std::shared_ptr<std::uniform_int_distribution<>> coin_trigger_state_dist;



    std::map<int, std::vector<int>> base_coin_payout_weights;
    std::map<int, std::vector<int>> base_cum_weights;
    int sum_base_weights = 0;


    std::map<int, std::vector<int>> feature_coin_payout_weights;
    std::map<int, std::vector<int>> feature_cum_weights;
    int sum_feature_weights = 0;


};

struct ConfigIns {
    std::map<std::string, std::vector<std::vector<int>>> Reels;
    std::vector<std::vector<int>> base_reel;
    std::vector<std::vector<int>> Paylines;
    std::vector<std::vector<int>> Paytable;
    int default_bet;
};

struct Settings {
    ConfigIns Configs;
    CoinConfigs CoinConfigs;
};


struct BaseGameResults {
    int TotalPoints =0;

    int Scatters =0;
    int Coins=0;

    int SymbolHit[9][5] = {0};
    int SymbolPoints[9][5] = {0};
    nlohmann::json logs;

};
struct FreeGamesResults {
    int TotalPoints = 0;
    int EndingWays=0;
    long long int SymbolHit[10][5] = {0};
    long long int FreeGameHit = 0;
    long long int FreeGameTotalRounds = 0;
    long long int SymbolPoints[10][5] = {0};
    long long int FreeGamesEndingCoins[11] = {0};
    long long int FreeGamesEndingCoinsRTP[11] = {0};
    nlohmann::json logs;

};
struct CoinGamesResults {
    int plays = 0;
    std::vector<int> coins;
    long long int TotalPoints = 0;
    long long int TotalNormalPoints = 0;
    int TotalJackpotPoints = 0;
    int EndingCoins = 0;
    int JackpotHits[5] = {0};
    int JackpotPoints[5] = {0};
    int UnlockedPositions[5] = {0};

    int FullReelHits[5] = {0};
    int SixStackOnReelHit[5] = {0};
    int FiveStackOnReelHit[5] = {0};

    long long int coin_sum[5] = {0};
    long long int coin_sum_base[5] = {0};
    long long int coin_sum_feature[5]= {0};


    long long int total_coins_per_reel[5] = {0};
    long long int total_coins_per_reel_base[5] = {0};
    long long int total_coins_per_reel_feature[5] = {0};

    long long int ReelPoints[5] = {0};


    nlohmann::json logs;


};


struct GameSummaryBlock {
    int Iterations = 0;
    int Timetaken_ms = 0;
    int Timetaken_min = 0;
    long long int TotalBet = 0;
    long long int TotalWon = 0;
    int Defaultbet = 0;
    double RTP = 0;
    double upper_level = 0;
    double lower_level =0;
    double risk =0;
    double wins_h1 = 0;
    double wins_h2 = 0;
    double wins_h1_hit = 0;
    double wins_h2_hit = 0;
    double wins_low = 0;
    double wins_low_hit = 0;
    double wins_lose = 0;
    double wins_lose_hit = 0;
};
// h1_hit, h1_points, h2_hit, h2_points


struct FeatureSummaryBlock {
    // Game
    double GameVariance =0;                //
    double GameVarianceBonus = 0;          //
    double STD = 0;                        //
    double STDBonus = 0;                   //
    double HitFrequency = 0;               //
    double HitFrequencyIntoFreeGames = 0;  //
    double HitFrequencyIntoCoinGames = 0;  //
    // Base
    double BaseRTP = 0;             //
    double BaseVariance = 0;        //
    double BaseSTD = 0;             //
    double BaseHitFrequency = 0;    //
    double BaseSymbolHit[10][5] = {0};
    double BaseSymbolRTP[10][5] = {0};

    // Free Games
    double FGRTP = 0;              //
    double FGVariance = 0;         //
    double FGSTD = 0;              //
    double FGHitFrequency =0;      //
    double FGSymbolRTP[10][5] = {0};   //
    double FGSymbolHits[10][5] = {0};  //
    double FreeGamesEndingCoins[11] = {0};
    double FreeGamesEndingCoinsRTP[11] = {0};
    std::map<int, double> FreeGamesEndingWaysRTPDict;   //
    std::map<int, double> FreeGamesEndingWaysHitFreqDict;   //

    // Coin Games
    double CoinRTP = 0;    //
    double CoinBaseRTP = 0;  //
    double CoinJackpotRTP = 0;  //
    long long int CPlays = 0;
    double CoinSTD = 0;      //
    double CoinSTDBonus = 0;  //
    double CoinVariance = 0;  //
    double CoinVarianceBonus = 0;//

    double UnlockedPositions[5] = {0};  //
    double FullReelHits[5] = {0};
    double SixStackOnReelHit[5] = {0};
    double FiveStackOnReelHit[5] = {0};

    double coin_sum[5] = {0};
    double coin_sum_base[5] = {0};
    double coin_sum_feature[5]= {0};

    long long int total_coins_per_reel[5] = {0};
    long long int total_coins_per_reel_base[5] = {0};
    long long int total_coins_per_reel_feature[5] = {0};

    std::map<int, double> CoinsEndingRTPDict;   //
    std::map<int, double> CoinsEndingHitFreqDict;   //


    double JackpotRTP[5] = {0};
    double JackpotHitFrequency[5] = {0};

    double ReelPoints[5] = {0};

    std::vector<int> TopTenPoints;
    std::vector<int> TopTenPointsFG;
    std::vector<int> TopTenPointsCG;
    std::vector<int> TopTenPointsBase;


};

struct GameReport {
    GameSummaryBlock GameSummary;
    FeatureSummaryBlock FeatureSummary;

};


struct LogsData {
    nlohmann::json logs;
};


BaseGameResults BaseGame(std::vector<std::vector<int>> &game_view,
              std::vector<std::vector<int>> &paylines,
              std::vector<std::vector<int>> &paytable);

FreeGamesResults FreeGames(std::map<std::string, std::vector<std::vector<int>>> &AllReels,
                           std::vector<std::vector<int>> &paytable);


CoinGamesResults CoinGames(std::vector<std::vector<int>> &initial_game_view,
                           CoinConfigs &CoinConfigs );

Stats CompressThreadStats(std::vector<Stats> &GameStats);
GameReport GetReport(Stats &CleanedStats,
                     long long int Iterations,
                     int Timetaken_ms,
                     int Timetaken_min,
                     int Defaultbet,
                     std::vector<int> &JackpotMultipliers);



void PrintReport(GameReport &report);
void PrintScatterReport(GameReport &report);
void PrintCoinReport(GameReport &report);
#endif //TOTHETOPTESTING_HEADER_H
