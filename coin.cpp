#include "header.h"
#include <vector>
#include <algorithm>
#include <execution>
static std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
std::uniform_real_distribution<double> distr(0.00000000, 1.000000);


struct CoinsReel {
    std::map<int, std::vector<int>> reels;
    std::map<int, int> reel_info;  // reel number and number of coins
};




//bool GetAxeWithProbability(std::vector<std::vector<double>>& distribution,
//                           int pos,
//                           CoinsReel& ReelInfo) {
//
//    double axe_probability = distribution[ReelInfo.reel_info[pos]][pos] ;
//
//    double randValue;
//    bool win = false;
//
//    int new_unlocks = 0;
//    for (int i = 0; i < 7; ++i) {
//
//        if (ReelInfo.reels[pos][i] == 0) {
//            randValue = distr(gen);
//            if (randValue <= axe_probability) {
//                ReelInfo.reels[pos][i] = 1;
//                win  = true;
//                ++new_unlocks;
//                ++ReelInfo.reel_info[pos];
//            }
//        }
//    }
//
//    int look = 0;
//    while ( (ReelInfo.reel_info[pos] < 7)  && (new_unlocks >= 1 ) && (look <= 7)) {
//         if (ReelInfo.reels[pos][look] == -1) {
//            ReelInfo.reels[pos][look] = 0;
//            new_unlocks -= 1;
//         }
//         ++look;
//    }
//    return win;
//}


int getRandomValueByWeight(
                           const std::vector<int> &weights,
                           const std::vector<int> &values
                           ) {
    int sum = std::accumulate(weights.begin(), weights.end(), 0);

    double randValue = distr(gen);
    double targetWeight = sum * randValue;
    double currentWeight = 0;
    for (int i =0; i < values.size(); ++i ){
        currentWeight += weights[i];
        if (currentWeight >= targetWeight) {
            return values[i];
        }

    }
    throw std::runtime_error("No value found for the generated random number");


}



CoinGamesResults CoinGames(std::vector<std::vector<int>> &initial_game_view,
                           CoinConfigs &CoinConfigs) {

    CoinGamesResults Results;
    int totalPoints = 0;
    int totalCoinPoints = 0;
    int totalJackpotPoints = 0;
    std::vector<int> reelPoints = {0,0,0,0,0};
    std::vector<int> coinReelCount = {0, 0, 0, 0, 0};
    std::vector<std::vector<int>> CoinGrid(5, std::vector<int>(7, -1));

    for (int i = 0; i < 5; ++i) {
//        std::cout << i << std::endl;

        int extraUnlocked = 0;
        for (int j = 0; j < 3; ++j) {
            int symbol = initial_game_view[i][j];
            if (symbol == 11) {
                int coinValue = getRandomValueByWeight(
                        CoinConfigs.base_coin_payout_weights[i],
                                CoinConfigs.coin_multipliers
                                                       ) * 50;
                CoinGrid[i][j] = coinValue;
                reelPoints[i] += coinValue;
                Results.coins.push_back(coinValue);
                Results.coin_sum[i] += coinValue/50;

                totalPoints += coinValue;
                totalCoinPoints += coinValue;

                ++extraUnlocked;
                CoinGrid[i][2 + extraUnlocked] = 0;
                coinReelCount[i] += 1;
            } else {
                CoinGrid[i][j] = 0;
            }
        }
    }

    int rounds = 3;
    int rounds_played = 0;
    bool reset;
    bool win;

//    for (int i = 0; i < 5; ++i) {
//        for (int j = 0; j < 7; ++j) {
//            std::cout << CoinGrid[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;
//    std::cout << std::endl;



    while (rounds_played < rounds) {

        // Single Round
        for (int i = 0; i < 5; ++i) {

            double coinLandProb = CoinConfigs.coin_probability_distributions[coinReelCount[i]][i];

            int colToExpand = 0;

            for (int j = 0; j < 7; ++j) {
                if (CoinGrid[i][j] == 0) {
                    double randValue = distr(gen);
                    if (randValue <= coinLandProb) {
                        int coinValue = getRandomValueByWeight(
                                                CoinConfigs.feature_coin_payout_weights[i],
                                                CoinConfigs.coin_multipliers
                                                               ) * 50;
                        CoinGrid[i][j] = coinValue;
                        reelPoints[i] += coinValue;

                        totalPoints += coinValue;
                        totalCoinPoints += coinValue;

                        reset = true;

                        ++Results.total_coins_per_reel[i];
                        Results.coins.push_back(coinValue);
                        Results.coin_sum[i] += coinValue/50;
                        ++colToExpand;
                        coinReelCount[i] += 1;

                    }
                }
            }


            for (int j = 0; j < 7; ++j) {
                while (colToExpand > 0 && CoinGrid[i][j] == -1) {
                    CoinGrid[i][j] = 0;
                    --colToExpand;
                }
            }

        }

//        for (int i = 0; i < 5; ++i) {
//            for (int j = 0; j < 7; ++j) {
//                std::cout << CoinGrid[i][j] << " ";
//            }
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;

        ++rounds_played;

        if (reset) {
            rounds_played = 0;
            reset = false;
        }
    }

    for (int i = 0; i < 5; ++i) {
        if (coinReelCount[i] == 7) {
            int pointsJackpot = CoinConfigs.JackpotMultipliers[i] * 50;
            totalJackpotPoints += pointsJackpot;
            totalPoints += pointsJackpot;
            reelPoints[i] += pointsJackpot;

            Results.JackpotHits[i] += 1;
            Results.JackpotPoints[i] += pointsJackpot;
        }
        if (coinReelCount[i] == 6) {
            Results.SixStackOnReelHit[i] += 1;
        }
        if (coinReelCount[i] == 5) {
            Results.FiveStackOnReelHit[i] += 1;
        }


        Results.UnlockedPositions[i] = coinReelCount[i];
        Results.ReelPoints[i] += reelPoints[i];
    }


//    std::vector<std::vector<int>> game_view;
//    for (auto &val: ReelInfo.reels) {
//        game_view.push_back(val.second);
//    }

//    Results.logs["game_view"] = game_view;
    int coin_num = 0;
    for (int i = 0; i < 5; ++i) {
        coin_num += coinReelCount[i];
    }
    Results.EndingCoins = coin_num;

    Results.TotalPoints = totalPoints;
    Results.TotalNormalPoints = totalCoinPoints;
    Results.TotalJackpotPoints = totalJackpotPoints;


    Results.plays += 1;
//    Results.logs["points"] = Results.TotalPoints;



    return Results;


}