#include <iostream>
#include <memory>
#include "header.h"
#include "Useful.h"
#include "config.h"
#include <tabulate/table.hpp>
#include <tabulate/markdown_exporter.hpp>
#include <nlohmann/json.hpp>

#include "settings.h"
using json = nlohmann::json;

using namespace std;
using namespace tabulate;

vector<int> PlayGame(Settings &setting, LogsData& Logs) {
    vector<int> points(4,0);
    int TotalPoints = 0;
    int base_points = 0;
    int scatter_points = 0;
    int total_coin_points = 0;

    std::vector<std::vector<int>> game_view = GameView(setting.Configs.base_reel);
    Logs.logs["logs"]["gmv"] = game_view;
    // BASE GAME
    BaseGameResults BaseGameResult = BaseGame(game_view, setting.Configs.Paylines, setting.Configs.Paytable);
    base_points = BaseGameResult.TotalPoints;
    TotalPoints += base_points;

    {
        Logs.logs["logs"]["base"] = BaseGameResult.logs;

    }

    // SCATTER GAME (Free Games)
    if (BaseGameResult.Scatters == 3) {
        FreeGamesResults FreeGameResult = FreeGames(setting.Configs.Reels, setting.Configs.Paytable);
        scatter_points = FreeGameResult.TotalPoints;
        TotalPoints += scatter_points;

        {
            Logs.logs["logs"]["scatter"] = FreeGameResult.logs;

        }


    }

    // COIN GAME
    if (BaseGameResult.Coins >= 6) {
        CoinGamesResults CoinGameResult = CoinGames(game_view,
                                                    setting.CoinConfigs);
        total_coin_points = CoinGameResult.TotalPoints;
        TotalPoints += total_coin_points;
        { Logs.logs["logs"]["coins"] = CoinGameResult.logs; }
    }

    points[0] = TotalPoints;
    points[1] = base_points;
    points[2] = scatter_points;
    points[3] = total_coin_points;

    return points;
}

void ScatterGame(int start, int end,
                 Settings &setting,
                Stats &GameStats) {
    for (int k = start; k < end; ++k) {
        int TotalPoints = 0;
        int scatter_points = 0;
        FreeGamesResults FreeGameResult;
        while (scatter_points < (23 * 50)) {
            FreeGameResult = FreeGames(setting.Configs.Reels,setting.Configs.Paytable);
            scatter_points = FreeGameResult.TotalPoints;
        }
        GameStats.FreeGamesPointsArray.push_back(scatter_points);
        GameStats.FreeGamesEndingWaysPointsDict[FreeGameResult.EndingWays] += scatter_points;
        GameStats.FreeGamesEndingWaysHitDict[FreeGameResult.EndingWays] += 1;
        GameStats.FreeGameTotalRounds += FreeGameResult.FreeGameTotalRounds;
        GameStats.FreeGameHit += FreeGameResult.FreeGameHit;
        TotalPoints += scatter_points;
        for (int i = 0; i < 9 ; ++i) {
            for (int j =0; j < 5; ++j) {
                GameStats.ThreadFreeGamesSymbolPoints[i][j] += FreeGameResult.SymbolPoints[i][j];
                GameStats.ThreadFreeGamesSymbolHit[i][j] += FreeGameResult.SymbolHit[i][j];
            }
        }
        for (int i =0; i < 11; ++i) {
            GameStats.FreeGamesEndingCoins[i] += FreeGameResult.FreeGamesEndingCoins[i];
            GameStats.FreeGamesEndingCoinsRTP[i] += FreeGameResult.FreeGamesEndingCoinsRTP[i];
        }
        GameStats.PointsArray.push_back(TotalPoints);
        GameStats.PointsArrayNoBonus.push_back(TotalPoints);

    }


}

void CoinGame(int start, int end,
              Settings &setting,
              Stats &GameStats,
              bool& rounds) {
    std::vector<int> win;
    for (int k = start; k < end; ++k)  {
        long long int TotalPoints = 0;
        long long int NoJackpotPoints = 0;

        std::vector<std::vector<int>> game_view;

        if (coin_num == 6) {
            game_view = {
                    {1,2,3},
                    {11,11,11},
                    {3,4,5},
                    {11,11,11},
                    {7,5,3}
            };
        }
        if (coin_num == 9) {
            game_view = {
                    {1,2,3},
                    {11,11,11},
                    {11,11,11},
                    {11,11,11},
                    {2,3,4}
            };
        }
        if (coin_num == 12) {
            game_view = {
                    {11,11,11},
                    {11,11,11},
                    {11,11,11},
                    {11,11,11},
                    {1,2,3}
            };
        }
        if (coin_num == 15) {
            game_view = {
                    {11,11,11},
                    {11,11,11},
                    {11,11,11},
                    {11,11,11},
                    {11,11,11}
            };
        }


        CoinGamesResults CoinGameResult = CoinGames(game_view,
                                                    setting.CoinConfigs );

        int total_coin_points = CoinGameResult.TotalPoints;
        int coin_points = CoinGameResult.TotalNormalPoints;
        int jackpot_points = CoinGameResult.TotalJackpotPoints;
        GameStats.CoinsEndingPointsDict[CoinGameResult.EndingCoins] += coin_points;
        GameStats.CoinsEndingHitDict[CoinGameResult.EndingCoins] += 1;

        if (jackpot_points > 0) {
            for (int i = 0; i < 5; ++i) {
                GameStats.JackpotHits[i] += CoinGameResult.JackpotHits[i];
                GameStats.JackpotRTP[i] += CoinGameResult.JackpotPoints[i];
            }
        }

        for (int i = 0; i < 5; ++i) {
            GameStats.UnlockedPositions[i] += CoinGameResult.UnlockedPositions[i];
            GameStats.JackpotHits[i] += CoinGameResult.JackpotHits[i];
            GameStats.SixStackOnReelHit[i] += CoinGameResult.SixStackOnReelHit[i];
            GameStats.FiveStackOnReelHit[i] += CoinGameResult.FiveStackOnReelHit[i];
            GameStats.coin_sum[i] += CoinGameResult.coin_sum[i];
            GameStats.coin_sum_base[i] += CoinGameResult.coin_sum_base[i];
            GameStats.coin_sum_feature[i] += CoinGameResult.coin_sum_feature[i];
            GameStats.total_coins_per_reel[i] += CoinGameResult.total_coins_per_reel[i];
            GameStats.total_coins_per_reel_base[i] += CoinGameResult.total_coins_per_reel_base[i];
            GameStats.total_coins_per_reel_feature[i] += CoinGameResult.total_coins_per_reel_feature[i];
            GameStats.ReelPoints[i] += CoinGameResult.ReelPoints[i];

        }

        GameStats.TotalCoinPointsArray.push_back(total_coin_points);
        GameStats.CoinPointsArray.push_back(coin_points);
        GameStats.JackpotCoinsPointsArray.push_back(jackpot_points);


        NoJackpotPoints += TotalPoints + coin_points;
        TotalPoints += total_coin_points;


        GameStats.PointsArray.push_back(TotalPoints);
        GameStats.PointsArrayNoBonus.push_back(NoJackpotPoints);

    }

}

void GameRuns(int start, int end,
               Settings &setting,
               Stats &GameStats) {
    for (int k = start; k < end; ++k) {
        int TotalPoints = 0;
        int NoJackpotPoints = 0;

        std::vector<std::vector<int>> game_view = GameView(setting.Configs.base_reel);
//        std::vector<std::vector<int>> game_view = {
//                {11,10,2},
//                {11,11},
//                {11,11,11},
//                {3,3,3},
//                {3,3,10},
//        };

        // BASE GAME
        BaseGameResults BaseGameResult =  BaseGame(game_view,
                                                   setting.Configs.Paylines,
                                                   setting.Configs.Paytable);


        int base_points = BaseGameResult.TotalPoints;

        TotalPoints += base_points;
        GameStats.BasePointsArray.push_back(base_points);
        for (int i = 0; i < 9 ; ++i) {
            for (int j =0; j < 5; ++j) {
                GameStats.ThreadBaseSymbolPoints[i][j] += BaseGameResult.SymbolPoints[i][j];
                GameStats.ThreadBaseSymbolHit[i][j] += BaseGameResult.SymbolHit[i][j];

            }
        }


        // SCATTER GAME (Free Games)
        if (BaseGameResult.Scatters == 3) {
            int scatter_points = 0;
            FreeGamesResults FreeGameResult;
            while (scatter_points < (72.2 * 50)) {
                FreeGameResult = FreeGames(setting.Configs.Reels,setting.Configs.Paytable);
                scatter_points = FreeGameResult.TotalPoints;
            }



            GameStats.FreeGamesPointsArray.push_back(scatter_points);
            GameStats.FreeGamesEndingWaysPointsDict[FreeGameResult.EndingWays] += scatter_points;
            GameStats.FreeGamesEndingWaysHitDict[FreeGameResult.EndingWays] += 1;
            GameStats.FreeGameTotalRounds += FreeGameResult.FreeGameTotalRounds;
            GameStats.FreeGameHit += FreeGameResult.FreeGameHit;
            TotalPoints += scatter_points;
            for (int i = 0; i < 9 ; ++i) {
                for (int j =0; j < 5; ++j) {
                    GameStats.ThreadFreeGamesSymbolPoints[i][j] += FreeGameResult.SymbolPoints[i][j];
                    GameStats.ThreadFreeGamesSymbolHit[i][j] += FreeGameResult.SymbolHit[i][j];
                }
            }
            for (int i =0; i < 11; ++i) {
                GameStats.FreeGamesEndingCoins[i] += FreeGameResult.FreeGamesEndingCoins[i];
                GameStats.FreeGamesEndingCoinsRTP[i] += FreeGameResult.FreeGamesEndingCoinsRTP[i];
            }
        }



        // COIN GAME
        NoJackpotPoints += TotalPoints;

        if (BaseGameResult.Coins >= 6 ){
            CoinGamesResults CoinGameResult = CoinGames(game_view,setting.CoinConfigs );

            int total_coin_points = CoinGameResult.TotalPoints;
            int coin_points = CoinGameResult.TotalNormalPoints;
            int jackpot_points = CoinGameResult.TotalJackpotPoints;
            GameStats.CoinsEndingPointsDict[CoinGameResult.EndingCoins] += coin_points;
            GameStats.CoinsEndingHitDict[CoinGameResult.EndingCoins] += 1;

            if (jackpot_points > 0) {
                for (int i = 0; i < 5; ++i) {
                    GameStats.JackpotHits[i] += CoinGameResult.JackpotHits[i];
                    GameStats.JackpotRTP[i] += CoinGameResult.JackpotPoints[i];
                }
            }

            for (int i = 0; i < 5; ++i) {
                GameStats.UnlockedPositions[i] += CoinGameResult.UnlockedPositions[i];
                GameStats.SixStackOnReelHit[i] += CoinGameResult.SixStackOnReelHit[i];
                GameStats.FiveStackOnReelHit[i] += CoinGameResult.FiveStackOnReelHit[i];
                GameStats.coin_sum[i] += CoinGameResult.coin_sum[i];
                GameStats.coin_sum_base[i] += CoinGameResult.coin_sum_base[i];
                GameStats.coin_sum_feature[i] += CoinGameResult.coin_sum_feature[i];
                GameStats.total_coins_per_reel[i] += CoinGameResult.total_coins_per_reel[i];
                GameStats.total_coins_per_reel_base[i] += CoinGameResult.total_coins_per_reel_base[i];
                GameStats.total_coins_per_reel_feature[i] += CoinGameResult.total_coins_per_reel_feature[i];
                GameStats.ReelPoints[i] += CoinGameResult.ReelPoints[i];
            }


            GameStats.TotalCoinPointsArray.push_back(total_coin_points);
            GameStats.CoinPointsArray.push_back(coin_points);
            GameStats.JackpotCoinsPointsArray.push_back(jackpot_points);

            NoJackpotPoints +=  coin_points;
            TotalPoints += total_coin_points;

        }


        GameStats.PointsArray.push_back(TotalPoints);
        GameStats.PointsArrayNoBonus.push_back(NoJackpotPoints);

    }

}

std::vector<int> precomputeCumulativeWeights(const std::vector<int>& weights) {
    std::vector<int> cumulativeWeights(weights.size());
    std::partial_sum(weights.begin(), weights.end(), cumulativeWeights.begin());
    return cumulativeWeights;
}

int main() {
    std::string file = "C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\config.yaml";
    Config config(file);
    CoinBuyFeature CoinBuy = config.CoinBuy;

    Settings setting;

    setting.Configs.Reels = config.Reels();
    setting.Configs.base_reel = config.GetWheel("wheel_base");
    setting.Configs.Paylines =  config.GetPaylines();
    setting.Configs.Paytable = config.paytable;

    setting.Configs.default_bet = 50;
    setting.CoinConfigs.default_bet = 50;
    setting.CoinConfigs.play_bet = 50 * 30;

    setting.CoinConfigs.JackpotMultipliers = config.jackpot_multipliers;
    setting.CoinConfigs.coin_multipliers = config.coin_multipliers;
    setting.CoinConfigs.coin_probability_distributions = config.coin_probability_distributions;
    setting.CoinConfigs.coin_payout_weights = config.coin_payout_weights;
    setting.CoinConfigs.coin_payout_total_weights = config.coin_payout_total_weights;

    setting.CoinConfigs.coin_trigger_states = config.coin_trigger_states;
    setting.CoinConfigs.coin_trigger_weights = config.coin_trigger_weights;
    setting.CoinConfigs.coin_triggering_weight = config.coin_triggering_weight;

    int default_bet = 50;

    setting.CoinConfigs.coin_trigger_state_dist = std::make_shared<std::uniform_int_distribution<>>(0, config.coin_triggering_weight -1);
    setting.CoinConfigs.base_coin_payout_weights = config.base_coin_payout_weights;
    setting.CoinConfigs.feature_coin_payout_weights = config.feature_coin_payout_weights;

    for (int i = 0; i <5; ++i) {
        setting.CoinConfigs.base_cum_weights[i] = precomputeCumulativeWeights(config.base_coin_payout_weights[i]);
        setting.CoinConfigs.feature_cum_weights[i] = precomputeCumulativeWeights(config.feature_coin_payout_weights[i]);


        int twb = std::accumulate(config.base_coin_payout_weights[i].begin(), config.base_coin_payout_weights[i].end(),0);
        int twf = std::accumulate(config.feature_coin_payout_weights[i].begin(), config.feature_coin_payout_weights[i].end(),0);

        setting.CoinConfigs.base_coin_weight_distribution.push_back(std::make_shared<std::uniform_int_distribution<>>(0, twb));
        setting.CoinConfigs.feature_coin_weight_distribution.push_back(std::make_shared<std::uniform_int_distribution<>>(0, twf));
    }

    setting.CoinConfigs.coin_trigger_states = config.coin_trigger_states;
    setting.CoinConfigs.coin_trigger_weights = config.coin_trigger_weights;
    setting.CoinConfigs.coin_triggering_weight = config.coin_triggering_weight;

    setting.CoinConfigs.coin_trigger_state_dist = std::make_shared<std::uniform_int_distribution<>>(0, config.coin_triggering_weight -1);



    cout << "Config Loaded..." << endl;
    if (choice == 0 || all==true) {
        std::vector<std::thread> threads;
        vector<Stats> GameStats(num_threads);
        auto start_time = std::chrono::high_resolution_clock::now();;
        const int elements_per_thread = num_elements / num_threads;
        cout << "Simulation Begins..." << endl;

        for (int i = 0; i < num_threads; ++i) {
            int start = i * elements_per_thread;
            int end = (i == num_threads - 1) ? num_elements : start + elements_per_thread;
            threads.emplace_back(GameRuns,
                                 start, end,
                                 std::ref(setting),
                                 std::ref(GameStats[i])

            );


        }
        for (auto &thread: threads) {
            thread.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        auto duration_mins = std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time);
        std::cout << "T: " << duration.count() << "  Mins: " << duration_mins.count() << std::endl;


        Stats CleanedStats = CompressThreadStats(GameStats);
        GameReport report = GetReport(CleanedStats, num_elements,
                                      duration.count(),
                                      duration_mins.count(),
                                      default_bet,
                                      setting.CoinConfigs.JackpotMultipliers);


        PrintReport(report);

    }

    if (choice == 1) {
        LogsData Logs;
        int play;
//        std::getline(std::cin, play);
        double Total_points = 0.01 ;
        double total_bet =0.0;
        double base_per = 0.0;
        double scatter_per = 0.0;
        double coins_per = 0.0;

        int iters = 1;
        while (iters <100 ) {
            vector<int> points = PlayGame(setting, Logs);
            total_bet += default_bet;
            Total_points += points[0];
            base_per += points[1];
            scatter_per += points[2];
            coins_per += points[3];
            std::cout <<  points[0]/50.0 << " " << points[1]/50.0 << " " << points[2] /50.0<< " " << points[3]/50.0 << std::endl;
//            std::cout << (Total_points / total_bet )<< "," << (base_per/Total_points )<< "," <<scatter_per/Total_points << "," <<coins_per/Total_points << std::endl;

            ++iters;
//            std::cin >> play;

            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Simulate work
        }


    }

    if (choice == 2 || all==true) {
        setting.CoinConfigs.default_bet = payment_for_fg;
        std::vector<std::thread> threads;
        vector<Stats> GameStats(num_threads);
        int elements_per_thread = num_elements_scatter / num_threads;
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_threads; ++i) {
            int start = i * elements_per_thread;
            int end = (i == num_threads - 1) ? num_elements_scatter : start + elements_per_thread;
            threads.emplace_back(ScatterGame,
                                 start, end,
                                 std::ref(setting),
                                 std::ref(GameStats[i])
            );


        }
        for (auto &thread: threads) {
            thread.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        auto duration_mins = std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time);
        std::cout << "T: " << duration.count() << "  Mins: " << duration_mins.count() << std::endl;


        Stats CleanedStats = CompressThreadStats(GameStats);
        GameReport report = GetReport(CleanedStats, num_elements_scatter,
                                      duration.count(), duration_mins.count(), setting.CoinConfigs.default_bet,
                                      setting.CoinConfigs.JackpotMultipliers);

        PrintScatterReport(report);

    }

    if (choice == 3 || all==true) {
        setting.CoinConfigs.default_bet = payment_for_coins ;

        std::vector<std::thread> threads;
        vector<Stats> GameStats(num_threads);
        auto start_time = std::chrono::high_resolution_clock::now();
        int elements_per_thread = num_elements_coin / num_threads;
        bool rounds = true;
        for (int i = 0; i < num_threads; ++i) {
            int start = i * elements_per_thread;
            int end = (i == num_threads - 1) ? num_elements_coin : start + elements_per_thread;
            threads.emplace_back(CoinGame,
                                 start, end,
                                 std::ref(setting),
                                 std::ref(GameStats[i]),
                                 std::ref(rounds)
            );

        }

        for (auto &thread: threads) {
            thread.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        auto duration_mins = std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time);
        std::cout << "T: " << duration.count() << "  Mins: " << duration_mins.count() << std::endl;
        std::cout << "Ahh1" << endl;

        Stats CleanedStats = CompressThreadStats(GameStats);
        std::cout << "Ahh2" << endl;

        GameReport report = GetReport(CleanedStats,
                                      num_elements_coin,
                                      duration.count(),
                                      duration_mins.count(),
                                      50,
                                      setting.CoinConfigs.JackpotMultipliers);

        PrintCoinReport(report);

        std::cout << "Ahh3s" << endl;


    }
    if (choice == 4) {
        std::map<int, vector<int>> Asset;
        std::map<int, int> Asset_match;
        int num_runs = 100;
        int Balance = 5000;
        int initial_bet = 50;
        vector<int> run_lengths;
        vector<int> balance_array;

        int iters;
        json runInfo;
        json runLog;

        for (int i =0; i < num_runs; ++i) {
            LogsData Logs;
            iters = 0;
            Balance = 5000;
            std::string val = to_string(i);
            while (Balance > initial_bet) {
                LogsData Logs;

                vector<int> points = PlayGame(setting, Logs);
                Balance += points[0] - initial_bet;
                balance_array.push_back(Balance);
                runLog[val]["data"][iters] = Logs.logs;
                runLog[val]["data"][iters]["balance"] = Balance;
                ++iters;
            }
            runLog[val]["length"] = iters;
            Asset[iters] = balance_array;
            Asset_match[iters] = i;
            run_lengths.push_back(iters);
            balance_array.clear();


        }

        json ExtremeGamesLog;
        std::ofstream outJson("C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\logs.json");
        std::string jsonString = runLog.dump();
        outJson << jsonString << std::endl;

        std::vector<int> highestKeys;
        std::vector<int> LowestKeys;

        int N = 30;
        for (auto it = Asset.rbegin(); it != Asset.rend() && N > 0; ++it, --N) {
            highestKeys.push_back(it->first);

        }
        int x = 0;
        for (auto it : Asset) {
            LowestKeys.push_back(it.first); // Add the key to the vector
            if (x==N-1) {
                break;
            }
            ++x;
        }


        std::ofstream outFile("C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\High.csv");
        if (outFile.is_open()) {
            int rank = 0;
            for (int iters : highestKeys) {
                for (size_t i = 0; i < iters; ++i) {
                    outFile << Asset[iters][i] << ",";
                }
                outFile << "\n";

                ExtremeGamesLog["logs"]["top"][to_string(rank)]["balance"] = Asset[iters];
                ExtremeGamesLog["logs"]["top"][to_string(rank)]["game_recorded"] = Asset_match[iters];

                runInfo["high"][rank] = iters;
                ++rank;
            }

            outFile.close();
            std::cout << "Data written to High.csv" << std::endl;
        } else {
            std::cerr << "Unable to open file" << std::endl;
        }


        std::ofstream outFileSmall("C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\Low.csv");
        if (outFileSmall.is_open()) {
            int rank = 0;
            for (int iters : LowestKeys) {
                for (size_t i = 0; i < iters; ++i) {
                    outFileSmall << Asset[iters][i] << ",";
                }
                outFileSmall << "\n";
                ExtremeGamesLog["logs"]["low"][to_string(rank)]["balance"] = Asset[iters];
                ExtremeGamesLog["logs"]["low"][to_string(rank)]["game_recorded"] = Asset_match[iters];

                runInfo["low"][rank] = iters;
                ++rank;
            }

            outFileSmall.close();
            std::cout << "Data written to Low.csv" << std::endl;
        } else {
            std::cerr << "Unable to open file" << std::endl;
        }


        std::ofstream _outJson("C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\data.json");
        std::string _jsonString = runInfo.dump();
        _outJson << _jsonString << std::endl;

        std::ofstream outrankjson("C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\ranks.json");
        std::string outra4nkjsonstr = ExtremeGamesLog.dump();
        outrankjson << outra4nkjsonstr << std::endl;


    }







    return 0;
}
