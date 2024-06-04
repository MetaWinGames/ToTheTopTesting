#include "header.h"
#include "Useful.h"
#include <tabulate/table.hpp>
#include <tabulate/markdown_exporter.hpp>
#include <nlohmann/json.hpp>
#include <queue>

using namespace std;
using namespace tabulate;
using json = nlohmann::json;


Stats CompressThreadStats(vector<Stats> &GameStats) {
    Stats CleanedStats;

    for (Stats &vec: GameStats) {

        CleanedStats.FreeGameTotalRounds += vec.FreeGameTotalRounds;
        CleanedStats.FreeGameHit += vec.FreeGameHit;

        CleanedStats.PointsArray.insert(CleanedStats.PointsArray.begin(),
                                        vec.PointsArray.begin(),
                                        vec.PointsArray.end());
        CleanedStats.BasePointsArray.insert(CleanedStats.BasePointsArray.begin(),
                                        vec.BasePointsArray.begin(),
                                        vec.BasePointsArray.end());
        CleanedStats.PointsArrayNoBonus.insert(CleanedStats.PointsArrayNoBonus.begin(),
                                            vec.PointsArrayNoBonus.begin(),
                                            vec.PointsArrayNoBonus.end());
        CleanedStats.FreeGamesPointsArray.insert(CleanedStats.FreeGamesPointsArray.begin(),
                                        vec.FreeGamesPointsArray.begin(),
                                        vec.FreeGamesPointsArray.end());
        CleanedStats.TotalCoinPointsArray.insert(CleanedStats.TotalCoinPointsArray.begin(),
                                        vec.TotalCoinPointsArray.begin(),
                                        vec.TotalCoinPointsArray.end());
        CleanedStats.CoinPointsArray.insert(CleanedStats.CoinPointsArray.begin(),
                                        vec.CoinPointsArray.begin(),
                                        vec.CoinPointsArray.end());
        CleanedStats.JackpotCoinsPointsArray.insert(CleanedStats.JackpotCoinsPointsArray.begin(),
                                        vec.JackpotCoinsPointsArray.begin(),
                                        vec.JackpotCoinsPointsArray.end());
        CleanedStats.CPlay += vec.CPlay;
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < 9 ; ++i) {
            for (int j =0; j < 5; ++j) {
                CleanedStats.ThreadBaseSymbolPoints[i][j] += vec.ThreadBaseSymbolPoints[i][j];
                CleanedStats.ThreadBaseSymbolHit[i][j] += vec.ThreadBaseSymbolHit[i][j];
                CleanedStats.ThreadFreeGamesSymbolPoints[i][j] += vec.ThreadFreeGamesSymbolPoints[i][j];
                CleanedStats.ThreadFreeGamesSymbolHit[i][j] += vec.ThreadFreeGamesSymbolHit[i][j];

            }
        }
        for (int i = 0; i < 5; ++i) {
            CleanedStats.JackpotHits[i] += vec.JackpotHits[i];
            CleanedStats.JackpotRTP[i] += vec.JackpotRTP[i];
            CleanedStats.UnlockedPositions[i] += vec.UnlockedPositions[i];
            CleanedStats.SixStackOnReelHit[i] += vec.SixStackOnReelHit[i];
            CleanedStats.FiveStackOnReelHit[i] += vec.FiveStackOnReelHit[i];
            CleanedStats.coin_sum[i] += vec.coin_sum[i];
            CleanedStats.coin_sum_base[i] += vec.coin_sum_base[i];
            CleanedStats.coin_sum_feature[i] += vec.coin_sum_feature[i];
            CleanedStats.total_coins_per_reel[i] += vec.total_coins_per_reel[i];
            CleanedStats.total_coins_per_reel_base[i] += vec.total_coins_per_reel_base[i];
            CleanedStats.total_coins_per_reel_feature[i] += vec.total_coins_per_reel_feature[i];
            CleanedStats.ReelPoints[i] += vec.ReelPoints[i];
        }
        for (int i =0; i < 11; ++i) {
            CleanedStats.FreeGamesEndingCoins[i] += vec.FreeGamesEndingCoins[i];
            CleanedStats.FreeGamesEndingCoinsRTP[i] += vec.FreeGamesEndingCoinsRTP[i];
        }

        for (const auto& pair : vec.CoinsEndingPointsDict) {
            CleanedStats.CoinsEndingPointsDict[ pair.first] += pair.second;
        }
        for (const auto& pair : vec.CoinsEndingHitDict) {
            CleanedStats.CoinsEndingHitDict[ pair.first] += pair.second;
        }
        for (const auto& pair : vec.FreeGamesEndingWaysPointsDict) {
            CleanedStats.FreeGamesEndingWaysPointsDict[ pair.first] += pair.second;
        }
        for (const auto& pair : vec.FreeGamesEndingWaysHitDict) {
            CleanedStats.FreeGamesEndingWaysHitDict[ pair.first] += pair.second;
        }

    }
    return CleanedStats;
}

long long int GetArraySum(vector<int> &wins_array) {

    long long int winnigns = std::accumulate(wins_array.begin(), wins_array.end(), 0LL);
    return winnigns;
}
std::pair<double, double> GetStats(const std::vector<int>& wins, long long sum_wins, long long n, double default_bet) {

    double sum_x_sq = 0;
    for (const auto& win : wins) {
        sum_x_sq += pow(static_cast<double>(win) / default_bet, 2) ;
    }

    long double sum_wins_sq = pow( sum_wins / default_bet, 2) ;

    double var = (1.0 / (n - 1)) * (sum_x_sq - (sum_wins_sq / static_cast<double>(n)));
    double std_dev = std::sqrt(var);

    return {var, std_dev};
}

int GetHits(const std::vector<int>& vec) {
    int count = 0;
    for (int num : vec) {
        if (num > 0) {
            ++count;
        }
    }
    return count;
}

GameReport GetReport(Stats &CleanedStats,
                     long long int Iterations,
                     int Timetaken_ms,
                     int Timetaken_min,
                     int Defaultbet,
                     std::vector<int> &JackpotMultipliers) {

    GameReport report;
    report.GameSummary.Iterations = Iterations;
    report.GameSummary.Timetaken_ms = Timetaken_ms;
    report.GameSummary.Timetaken_min = Timetaken_min;
    report.GameSummary.Defaultbet = Defaultbet;
    report.GameSummary.TotalBet = Iterations * Defaultbet;

    report.FeatureSummary.CPlays += CleanedStats.CPlay;

//    int normal_bet = 50;

    // Total
    long long int _TotalWon = GetArraySum(CleanedStats.PointsArray);
    std::pair<double, double> _game_stats = GetStats(CleanedStats.PointsArray, _TotalWon, CleanedStats.PointsArray.size(), Defaultbet );
    double var = _game_stats.first;
    double std = _game_stats.second;

    report.GameSummary.TotalWon = _TotalWon;
    report.GameSummary.RTP = _TotalWon / static_cast<double>(Iterations * Defaultbet);
    report.FeatureSummary.STDBonus = std;
    report.FeatureSummary.GameVarianceBonus = var;
    report.FeatureSummary.HitFrequency = GetHits(CleanedStats.PointsArray) / static_cast<double>(Iterations) ;
    report.FeatureSummary.HitFrequencyIntoFreeGames = static_cast<double>(CleanedStats.FreeGamesPointsArray.size()) / static_cast<double>(Iterations);
    report.FeatureSummary.HitFrequencyIntoCoinGames = static_cast<double>(CleanedStats.TotalCoinPointsArray.size()) / static_cast<double>(Iterations);

    std::array<long long int,8> HighWins = GetHighWins(CleanedStats.PointsArray, Defaultbet);

    report.GameSummary.wins_h1 = HighWins[1] / static_cast<double>(Iterations  * Defaultbet);
    report.GameSummary.wins_h1_hit = (HighWins[0] / static_cast<double>(Iterations))*100;

    report.GameSummary.wins_h2 = HighWins[3] / static_cast<double>(Iterations * Defaultbet);
    report.GameSummary.wins_h2_hit = (HighWins[2]/ static_cast<double>(Iterations))*100;

    report.GameSummary.wins_low = HighWins[5] / static_cast<double>(Iterations * Defaultbet);
    report.GameSummary.wins_low_hit =( HighWins[4] / static_cast<double>(Iterations ))*100;

    report.GameSummary.wins_lose = HighWins[7] / static_cast<double>(Iterations * Defaultbet);
    report.GameSummary.wins_lose_hit =( HighWins[6] / static_cast<double>(Iterations ))*100;

    cout << "NVS "  << CleanedStats.PointsArrayNoBonus.size() << " " << CleanedStats.PointsArray.size()<<endl;
    cout << "NVS "  << std::accumulate(CleanedStats.PointsArrayNoBonus.begin(), CleanedStats.PointsArrayNoBonus.end(), 0LL) << " " << std::accumulate(CleanedStats.PointsArray.begin(), CleanedStats.PointsArray.end(), 0LL) << endl;



    // POINTS NO BONUS
    long long int _TotalWonNB = GetArraySum(CleanedStats.PointsArrayNoBonus);
    std::pair<double, double> _game_stats_nb = GetStats(CleanedStats.PointsArrayNoBonus, _TotalWonNB, CleanedStats.PointsArrayNoBonus.size(), Defaultbet );
    double var_nb = _game_stats_nb.first;
    double std_nb = _game_stats_nb.second;
    report.FeatureSummary.GameVariance = var_nb;
    report.FeatureSummary.STD = std_nb;
    report.GameSummary.upper_level = report.GameSummary.RTP + (1.96 * (std_nb / sqrt(Iterations)));
    report.GameSummary.lower_level = report.GameSummary.RTP - (1.96 * (std_nb / sqrt(Iterations)));
    report.GameSummary.risk = var_nb / (1 - report.GameSummary.RTP);

    // Total Base
    long long int _TotalWonBASE = GetArraySum(CleanedStats.BasePointsArray);
    std::pair<double, double> _game_stats_base = GetStats(CleanedStats.BasePointsArray, _TotalWonBASE, CleanedStats.BasePointsArray.size(), Defaultbet );
    double var_base = _game_stats_base.first;
    double std_base = _game_stats_base.second;
    report.FeatureSummary.BaseRTP = _TotalWonBASE / static_cast<double>(Iterations * Defaultbet);
    report.FeatureSummary.BaseVariance = var_base;
    report.FeatureSummary.BaseSTD = std_base;
    report.FeatureSummary.BaseHitFrequency = GetHits(CleanedStats.BasePointsArray) / static_cast<double>(Iterations);

    // Total Free Games
    long long int _TotalWonFG = GetArraySum(CleanedStats.FreeGamesPointsArray);
    cout << _TotalWonFG << endl;
    std::pair<double, double> _game_stats_fg = GetStats(CleanedStats.FreeGamesPointsArray, _TotalWonFG, CleanedStats.FreeGamesPointsArray.size(), Defaultbet );
    double var_fg = _game_stats_fg.first;
    double std_fg = _game_stats_fg.second;
    report.FeatureSummary.FGRTP = _TotalWonFG / static_cast<double>(Iterations * Defaultbet);
    report.FeatureSummary.FGVariance = var_fg;
    report.FeatureSummary.FGSTD = std_fg;
    report.FeatureSummary.FGHitFrequency = CleanedStats.FreeGameHit / static_cast<double>(CleanedStats.FreeGameTotalRounds);


    // Total Coins
    long long int _TotalWonC = GetArraySum(CleanedStats.TotalCoinPointsArray);
    std::pair<double, double> _game_stats_c = GetStats(CleanedStats.TotalCoinPointsArray, _TotalWonC, CleanedStats.TotalCoinPointsArray.size(), Defaultbet );
    double var_c = _game_stats_c.first;
    double std_c = _game_stats_c.second;
    report.FeatureSummary.CoinRTP = _TotalWonC / static_cast<double>(Iterations * Defaultbet);
    report.FeatureSummary.CoinVarianceBonus = var_c;
    report.FeatureSummary.CoinSTDBonus = std_c;

    // Total Coins NO BONUS
    long long int _TotalWonC_NB = GetArraySum(CleanedStats.CoinPointsArray);
    std::pair<double, double> _game_stats_cNB = GetStats(CleanedStats.CoinPointsArray, _TotalWonC_NB, CleanedStats.CoinPointsArray.size(), Defaultbet );
    double var_cNB = _game_stats_cNB.first;
    double std_cNB = _game_stats_cNB.second;
    report.FeatureSummary.CoinBaseRTP = _TotalWonC_NB / static_cast<double>(Iterations * Defaultbet);
    report.FeatureSummary.CoinVariance = var_cNB;
    report.FeatureSummary.CoinSTD = std_cNB;


    long long int _TotalWonC_B = GetArraySum(CleanedStats.JackpotCoinsPointsArray);
    report.FeatureSummary.CoinJackpotRTP = _TotalWonC_B / static_cast<double>(Iterations * Defaultbet);
    std::cout << "JACKPOTS" << std::endl;
    std::cout << static_cast<double>(CleanedStats.TotalCoinPointsArray.size()) << std::endl;
    std::cout << CleanedStats.JackpotHits[0] << std::endl;
    std::cout << CleanedStats.JackpotHits[1] << std::endl;
    std::cout << CleanedStats.JackpotHits[2] << std::endl;
    std::cout << CleanedStats.JackpotHits[3] << std::endl;
    std::cout << CleanedStats.JackpotHits[4] << std::endl;
    std::cout  << std::endl;
    std::cout  << std::endl;

    for (int i = 0 ; i < 5; ++i) {
        report.FeatureSummary.JackpotRTP[i] += CleanedStats.JackpotRTP[i] / static_cast<double>(Iterations * Defaultbet) ;
        report.FeatureSummary.JackpotHitFrequency[i] =  CleanedStats.JackpotHits[i] / static_cast<double>(Iterations);
        report.FeatureSummary.UnlockedPositions[i] += CleanedStats.UnlockedPositions[i] /  static_cast<double>(CleanedStats.TotalCoinPointsArray.size());
        report.FeatureSummary.SixStackOnReelHit[i] += static_cast<double>(CleanedStats.SixStackOnReelHit[i]) /  static_cast<double>(CleanedStats.TotalCoinPointsArray.size());
        report.FeatureSummary.FiveStackOnReelHit[i] += static_cast<double>(CleanedStats.FiveStackOnReelHit[i])  /  static_cast<double>(CleanedStats.TotalCoinPointsArray.size());
        report.FeatureSummary.coin_sum[i] += CleanedStats.coin_sum[i] / static_cast<double_t>(CleanedStats.total_coins_per_reel[i]);
        report.FeatureSummary.coin_sum_base[i] += CleanedStats.coin_sum_base[i] / static_cast<double_t>(CleanedStats.total_coins_per_reel_base[i]);
        report.FeatureSummary.coin_sum_feature[i] += CleanedStats.coin_sum_feature[i] / static_cast<double_t>(CleanedStats.total_coins_per_reel_feature[i]);
        report.FeatureSummary.ReelPoints[i] += CleanedStats.ReelPoints[i] / static_cast<double>(Iterations * Defaultbet);

    }

    std::priority_queue<int> pq(CleanedStats.PointsArray.begin(), CleanedStats.PointsArray.end());
    // Extract the top 10 elements
    std::vector<int> top10;
    for (int i = 0; i < 10 && !pq.empty(); ++i) {
        top10.push_back(pq.top());
        pq.pop();
    }
    report.FeatureSummary.TopTenPoints = top10;

    std::priority_queue<int> pqfg(CleanedStats.FreeGamesPointsArray.begin(), CleanedStats.FreeGamesPointsArray.end());
    // Extract the top 10 elements
    std::vector<int> top10FG;
    for (int i = 0; i < 10 && !pqfg.empty(); ++i) {
        top10FG.push_back(pqfg.top());
        pqfg.pop();
    }
    report.FeatureSummary.TopTenPointsFG = top10FG;


    std::string filename = "individual_values.csv";

    // Create and open the CSV file for writing
    std::ofstream csv_file(filename);
    if (!csv_file) {
        std::cerr << "Error: Could not open the CSV file for writing.\n";
    }

    // Write CSV headers
    csv_file << "values" << ",\n";

    // Write the individual values to the CSV file
    for (size_t i = 0; i < CleanedStats.PointsArray.size(); ++i) {
        csv_file << CleanedStats.PointsArray[i] /50 << ",\n";
    }

    // Close the CSV file
    csv_file.close();
    std::cout << "Individual values have been saved to " << filename << "\n";





    std::priority_queue<int> pqcp(CleanedStats.TotalCoinPointsArray.begin(), CleanedStats.TotalCoinPointsArray.end());
    std::vector<int> top10CP;
    for (int i = 0; i < 10 && !pqcp.empty(); ++i) {
        top10CP.push_back(pqcp.top());
        pqcp.pop();
    }
    report.FeatureSummary.TopTenPointsCG = top10CP;



    std::priority_queue<int> pqbp(CleanedStats.BasePointsArray.begin(), CleanedStats.BasePointsArray.end());
    std::vector<int> top10BP;
    for (int i = 0; i < 10 && !pqbp.empty(); ++i) {
        top10BP.push_back(pqbp.top());
        pqbp.pop();
    }
    report.FeatureSummary.TopTenPointsBase = top10BP;


    report.FeatureSummary.HitFrequencyIntoFreeGames =  CleanedStats.FreeGamesPointsArray.size() / static_cast<double>(Iterations);
    report.FeatureSummary.HitFrequencyIntoCoinGames =  CleanedStats.TotalCoinPointsArray.size() / static_cast<double>(Iterations);

    for (int i =0; i < 11; ++i) {
        report.FeatureSummary.FreeGamesEndingCoins[i] += CleanedStats.FreeGamesEndingCoins[i] / static_cast<double>(CleanedStats.FreeGamesPointsArray.size());
        report.FeatureSummary.FreeGamesEndingCoinsRTP[i] += CleanedStats.FreeGamesEndingCoinsRTP[i] / static_cast<double>(Iterations * Defaultbet);
    }


    for (int i = 0; i < 9 ; ++i) {
        for (int j =0; j < 5; ++j) {
            report.FeatureSummary.BaseSymbolHit[i][j] += CleanedStats.ThreadBaseSymbolHit[i][j]  / static_cast<double>(Iterations) ;
            report.FeatureSummary.BaseSymbolRTP[i][j] += CleanedStats.ThreadBaseSymbolPoints[i][j]/ static_cast<double>(Iterations * Defaultbet) ;
            report.FeatureSummary.FGSymbolHits[i][j] += CleanedStats.ThreadFreeGamesSymbolHit[i][j]/ static_cast<double>(CleanedStats.FreeGameTotalRounds) ;
            report.FeatureSummary.FGSymbolRTP[i][j] += CleanedStats.ThreadFreeGamesSymbolPoints[i][j]/ static_cast<double>(Iterations * Defaultbet) ;

        }
    }


    // POINTS
    for (const auto& pair : CleanedStats.CoinsEndingPointsDict) {
        report.FeatureSummary.CoinsEndingRTPDict[ pair.first] += pair.second / static_cast<double>(Iterations * Defaultbet) ;
    }
    for (const auto& pair : CleanedStats.CoinsEndingHitDict) {
        report.FeatureSummary.CoinsEndingHitFreqDict[ pair.first] += pair.second /static_cast<double>(CleanedStats.CoinPointsArray.size()) ;
    }
    for (const auto& pair : CleanedStats.FreeGamesEndingWaysPointsDict) {
        report.FeatureSummary.FreeGamesEndingWaysRTPDict[ pair.first] += pair.second/ static_cast<double>(Iterations * Defaultbet);
    }
    for (const auto& pair : CleanedStats.FreeGamesEndingWaysHitDict) {
        report.FeatureSummary.FreeGamesEndingWaysHitFreqDict[ pair.first] += pair.second / static_cast<double>(CleanedStats.FreeGamesPointsArray.size());
    }


//    std::ofstream out("output.bin", std::ios::binary);
//    out.write(reinterpret_cast<const char*>(CleanedStats.PointsArray.data()), CleanedStats.PointsArray.size() * sizeof(int));
//    out.close();




    return report;
}

void PrintScatterReport(GameReport &report) {

    Table ReportTable;
    ReportTable.format().font_style({FontStyle::bold}).font_align(FontAlign::center).width(130);

    Table game_stats_table;
    game_stats_table.add_row({"Iterations", formatWithCommas(report.GameSummary.Iterations)});
    game_stats_table.add_row({"Time taken ms/mins", to_string(report.GameSummary.Timetaken_ms) + "/" + to_string(report.GameSummary.Timetaken_min)});
    game_stats_table.add_row({"Total Bet", formatWithCommas(report.GameSummary.TotalBet)});
    game_stats_table.add_row({"Total Won", formatWithCommas(report.GameSummary.TotalWon)});
    game_stats_table.add_row({"Default Bet", formatWithCommas(report.GameSummary.Defaultbet)});
    game_stats_table.add_row({"RTP", to_string(report.GameSummary.RTP*100)});
    game_stats_table.add_row({"Upper CL", to_string(report.GameSummary.upper_level*100)});
    game_stats_table.add_row({"Lower CL", to_string(report.GameSummary.lower_level*100)});
    game_stats_table.add_row({"Risk", to_string(report.GameSummary.risk)});


    Table rtp_breakdown;
    rtp_breakdown.add_row({"Multiplier Range", "RTP", "Hit"});
    rtp_breakdown.add_row({" <- 12x ",  to_string(report.GameSummary.wins_low*100),  to_string(report.GameSummary.wins_low_hit)});
    rtp_breakdown.add_row({" 12x <-> 40x ",  to_string(report.GameSummary.wins_h1*100),  to_string(report.GameSummary.wins_h1_hit)});
    rtp_breakdown.add_row({"40x -> ",  to_string(report.GameSummary.wins_h2*100),  to_string(report.GameSummary.wins_h2_hit)});


    Table top_feature;
    Table top_stats;
    top_stats.add_row({"Top Feature","RTP", "Variance", "Variance\nw/Bonus",
                       "Standard Dev", "Standard Dev\nw/Bonus", "Hit Frequency"});
    top_stats.add_row({"Entire Game",
                       to_string(report.GameSummary.RTP*100),
                       to_string(  report.FeatureSummary.GameVariance),
                       "",
                       to_string(  report.FeatureSummary.STD),
                       "",
                       to_string( report.FeatureSummary.HitFrequency)});


    top_stats.add_row({"Free Games",
                       to_string(report.FeatureSummary.FGRTP*100),
                       to_string(report.FeatureSummary.FGVariance),
                       " ",
                       to_string(report.FeatureSummary.FGSTD),
                       " ",
                       to_string(report.FeatureSummary.FGHitFrequency)});










    Table top_points;
    top_points.add_row({"Feature", "Points: x Bet"});

    top_points.add_row({"Free Game",
                        to_string(report.FeatureSummary.TopTenPointsFG[0]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[1]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[2]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[3]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[4]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[5]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[6]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[7]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[8]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[9]/50)
                       });


    Table feature_occurance;
    feature_occurance.add_row({"Feature", "Hit Into Rate"});
    feature_occurance.add_row({"Free Games", "1 in " + to_string(1.0/ report.FeatureSummary.HitFrequencyIntoFreeGames)});

    cout << report.FeatureSummary.JackpotRTP[0] << endl << report.FeatureSummary.JackpotHitFrequency[0] << endl;

    ReportTable.add_row({game_stats_table});
    ReportTable.add_row({rtp_breakdown});
    ReportTable.add_row({top_stats});
    ReportTable.add_row({top_points});
    ReportTable.add_row({feature_occurance});
    cout << ReportTable << endl;
    double checkrtp_ = 0;
    cout << endl;
    cout << setw(34) << "Hit Frequency (%)"<< setw(43) << "RTP" << endl;


    cout << "Free Games" << endl;
    for (int l = 0; l < 10; ++l) {
        cout <<  std::fixed << std::setprecision(0);

        cout << setw(2) << l << "   ";
        cout <<  std::fixed << std::setprecision(5);

        for (int k = 0; k < 5; ++k) {
            double val =  report.FeatureSummary.FGSymbolHits[l][k];
            if (val > 0) {
                cout << setw(8) << report.FeatureSummary.FGSymbolHits[l][k] * 100<< " "; // Base rtp values
            }
            else {
                cout<<  setw(8) << "       " << " ";
            }

        }

        cout << "    ";
        for (int k = 0; k < 5; ++k) {
            double val =  report.FeatureSummary.FGSymbolRTP[l][k];
            if (val > 0) {
                cout << setw(8) << val* 100 << " ";
            }
            else {
                cout<<  setw(8) << "       " << " ";
            }
        }
        cout << endl;

    }
    cout << endl << endl;



    cout << "Free Games"<< endl;
    double testFGRTP = 0;
    double testFGHit = 0;

    double checkrtpfg = 0;
    double cum_freq = 0;
    cout << setw(8) << "HF" <<setw(12) << "HF%" << setw(10) <<"CHF" << setw(10) << "RTP"<< endl;

    for (int i =0; i < 11;++i) {
        cum_freq +=  report.FeatureSummary.FreeGamesEndingCoins[i] * 100;
        cout <<i <<"   1 in " <<      static_cast<
                int>(1 / report.FeatureSummary.FreeGamesEndingCoins[i]) << "  " << report.FeatureSummary.FreeGamesEndingCoins[i] * 100 << "%  "  <<  cum_freq  << "  ";
        checkrtpfg += report.FeatureSummary.FreeGamesEndingCoinsRTP[i] * 100;
        cout << report.FeatureSummary.FreeGamesEndingCoinsRTP[i] * 100  << endl;
    }
    cout << endl;
    cout << checkrtpfg;



}

void PrintCoinReport(GameReport &report) {

    double div = 1.0;
    Table rep;
    std::cout << " Wnetered cons " << std::endl;
    rep.format().font_style({FontStyle::bold}).font_align(FontAlign::center).width(130);

    Table info;
    info.add_row({"Time", to_string(report.GameSummary.Timetaken_ms)});
    info.add_row({"Iterations", to_string(report.GameSummary.Iterations)});
    info.add_row({"Total Bet", to_string(report.GameSummary.TotalBet)});
    info.add_row({"Total Won", to_string(report.GameSummary.TotalWon)});
    info.add_row({"RTP", to_string((report.GameSummary.RTP*100))});
    info.add_row({"Upper bounds", to_string(report.GameSummary.upper_level*100)});
    info.add_row({"Lower bounds", to_string(report.GameSummary.lower_level*100)});
    info.add_row({"Risk", to_string(report.GameSummary.risk)});
    std::cout << " Wnetered cons " << std::endl;


    Table rtp_breakdown;
    rtp_breakdown.add_row({"Multiplier Range", "RTP", "Hit"});
    rtp_breakdown.add_row({" <- 12x ",  to_string(report.GameSummary.wins_low*100/div),  to_string(report.GameSummary.wins_low_hit)});
    rtp_breakdown.add_row({" 12x <-> 40x ",  to_string(report.GameSummary.wins_h1*100/div),  to_string(report.GameSummary.wins_h1_hit)});
    rtp_breakdown.add_row({"40x -> ",  to_string(report.GameSummary.wins_h2*100/div),  to_string(report.GameSummary.wins_h2_hit)});

    Table jackpots;
    jackpots.add_row({"Values", "20", "100", "2,000", "500", "50"} );

    jackpots.add_row({"RTP",
                      to_string(report.FeatureSummary.JackpotRTP[0]*100/div  ),
                      to_string(report.FeatureSummary.JackpotRTP[1]*100/div  ),
                      to_string(report.FeatureSummary.JackpotRTP[2]*100/div  ),
                      to_string(report.FeatureSummary.JackpotRTP[3]*100/div  ) ,
                      to_string(report.FeatureSummary.JackpotRTP[4]*100/div )} );
    jackpots.add_row({"Reel\nRTP",
                      to_string(report.FeatureSummary.ReelPoints[0]*100/div  ),
                      to_string(report.FeatureSummary.ReelPoints[1]*100/div  ),
                      to_string(report.FeatureSummary.ReelPoints[2]*100/div  ),
                      to_string(report.FeatureSummary.ReelPoints[3]*100/div  ) ,
                      to_string(report.FeatureSummary.ReelPoints[4]*100/div )} );
    jackpots.add_row({"HF",
                      "1 in " + to_string(static_cast<int>(1 / report.FeatureSummary.JackpotHitFrequency[0])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[1])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[2])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[3])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[4]))});
    jackpots.add_row({"HF 6",
                      "1 in " + to_string(static_cast<int>(1 / report.FeatureSummary.SixStackOnReelHit[0])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[1])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[2])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[3])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[4]))});
    jackpots.add_row({"HF 5",
                      "1 in " + to_string(static_cast<int>(1 / report.FeatureSummary.FiveStackOnReelHit[0])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[1])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[2])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[3])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[4]))});

//    jackpots.add_row({"Coin Sum",
//                      to_string(report.FeatureSummary.coin_sum[0]),
//                      to_string(report.FeatureSummary.coin_sum[1]),
//                      to_string(report.FeatureSummary.coin_sum[2]),
//                      to_string(report.FeatureSummary.coin_sum[3]),
//                      to_string(report.FeatureSummary.coin_sum[4])});
//
//
//    jackpots.add_row({"Coin Sum \nBase",
//                      to_string(report.FeatureSummary.coin_sum_base[0]),
//                      to_string(report.FeatureSummary.coin_sum_base[1]),
//                      to_string(report.FeatureSummary.coin_sum_base[2]),
//                      to_string(report.FeatureSummary.coin_sum_base[3]),
//                      to_string(report.FeatureSummary.coin_sum_base[4])});
//
//    jackpots.add_row({"Coin Sum\nFeature",
//                      to_string(report.FeatureSummary.coin_sum_feature[0]),
//                      to_string(report.FeatureSummary.coin_sum_feature[1]),
//                      to_string(report.FeatureSummary.coin_sum_feature[2]),
//                      to_string(report.FeatureSummary.coin_sum_feature[3]),
//                      to_string(report.FeatureSummary.coin_sum_feature[4])});

    Table top_points;
    top_points.add_row({"Feature", "Points"});



    top_points.add_row({"Game",
                        to_string(report.FeatureSummary.TopTenPoints[0]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[1]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[2]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[3]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[4]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[5]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[6]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[7]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[8]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[9]/50)
                       });




    double ave_coins = 0.0;
    for (int i = 0; i < 5; ++i) {
        ave_coins += report.FeatureSummary.UnlockedPositions[i];
    }
    jackpots.add_row({"ANC",to_string(ave_coins),"","","",""});
    jackpots.add_row({"P(J)","1 in "+to_string(
            static_cast<int>(1 / (report.FeatureSummary.JackpotHitFrequency[0] +
                                  report.FeatureSummary.JackpotHitFrequency[1] +
                                  report.FeatureSummary.JackpotHitFrequency[2] +
                                  report.FeatureSummary.JackpotHitFrequency[3] +
                                  report.FeatureSummary.JackpotHitFrequency[4])
            )
    ),"","","",""});



    Table top_stats;
    top_stats.add_row({"Top Feature","RTP", "Variance", "Variance\nw/Bonus",
                       "Standard Dev", "Standard Dev\nw/Bonus", "Hit Frequency"});
    top_stats.add_row({"Entire Game",
                       to_string(report.GameSummary.RTP*100),
                       to_string(  report.FeatureSummary.GameVariance),
                       to_string(  report.FeatureSummary.GameVarianceBonus),
                       to_string(  report.FeatureSummary.STD),
                       to_string(  report.FeatureSummary.STDBonus),
                       to_string( report.FeatureSummary.HitFrequency)});
    top_stats.add_row({"Coins",
                       to_string(report.FeatureSummary.CoinRTP*100/div),
                       to_string(report.FeatureSummary.CoinVariance),
                       to_string(report.FeatureSummary.CoinVarianceBonus),
                       to_string(report.FeatureSummary.CoinSTD),
                       to_string(report.FeatureSummary.CoinSTDBonus),
                       to_string(1)});

    top_stats.add_row({"Coins Stand alone",
                       to_string(report.FeatureSummary.CoinBaseRTP*100/div),
                       "",
                       to_string(report.FeatureSummary.CoinVariance),
                       "",
                       to_string(report.FeatureSummary.CoinSTD)
                      });


    top_stats.add_row({"JackPots",
                       to_string(report.FeatureSummary.CoinJackpotRTP*100/div),
                       "",
                       "",
                       "",
                       "",
                      });

    rep.add_row({info});
    rep.add_row({rtp_breakdown});
    rep.add_row({top_stats});
    rep.add_row({jackpots});
    rep.add_row({top_points});
    cout << rep << endl;
    cout << report.FeatureSummary.CPlays << endl;

    cout <<  std::fixed << std::setprecision(5);

    cout << "Coin Games" << endl;
    cout << "Average Unlocked positions : ";
    for (int i = 0; i < 5; ++i) {
        cout << report.FeatureSummary.UnlockedPositions[i] << " ";
    }

    cout << endl;
    double testCoinsRTP = 0;
    double testCoinsHit = 0;
    int breaking = 0;

    for (const auto& pair : report.FeatureSummary.CoinsEndingRTPDict) {
        testCoinsRTP +=pair.second;
        testCoinsHit += report.FeatureSummary.CoinsEndingHitFreqDict[ pair.first];
        int dec = static_cast<int>(1.0 / report.FeatureSummary.CoinsEndingHitFreqDict[ pair.first]);
        cout << pair.first;
        if (pair.first < 10){
            cout << "   ";
        } else {
            cout << "  ";
        }
        cout << " RTP: " << pair.second*100 ;
        cout <<  " HF: 1 in " << dec ;


        if (dec < 10) {
            cout << "    | ";
        }
        if (dec >= 10 && dec < 100) {
            cout << "   | ";

        }
        if (dec >= 100 && dec < 1000) {
            cout << "  | ";

        }
        if (dec >= 1000 && dec < 10000) {
            cout << " | ";
        }
        if (dec >= 10000 && dec < 100000) {
            cout << " | ";
        }

        ++breaking;
        if (breaking == 3) {
            cout << endl;
            breaking = 0;
        }

    }
    cout << endl;
    cout << testCoinsRTP << " " << testCoinsHit << endl;
    cout << endl << endl ;



    json CG_log;
    std::ofstream _outJson("C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\coin_Data.json");

    CG_log["hit"] = report.FeatureSummary.CoinsEndingHitFreqDict;
    CG_log["rtp"]= report.FeatureSummary.CoinsEndingRTPDict;
    std::string _jsonString = CG_log.dump();
    _outJson << _jsonString << std::endl;



}

void PrintReport(GameReport &report) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    Table ReportTable;
    ReportTable.format().font_style({FontStyle::bold}).font_align(FontAlign::center).width(130);

    Table game_stats_table;
    game_stats_table.add_row({"Iterations", formatWithCommas(report.GameSummary.Iterations)});
    game_stats_table.add_row({"Time taken ms/mins", to_string(report.GameSummary.Timetaken_ms) + "/" + to_string(report.GameSummary.Timetaken_min)});
    game_stats_table.add_row({"Total Bet", formatWithCommas(report.GameSummary.TotalBet)});
    game_stats_table.add_row({"Total Won", formatWithCommas(report.GameSummary.TotalWon)});
    game_stats_table.add_row({"Default Bet", formatWithCommas(report.GameSummary.Defaultbet)});
    game_stats_table.add_row({"RTP", to_string(report.GameSummary.RTP*100)});
    game_stats_table.add_row({"Upper CL", to_string(report.GameSummary.upper_level*100)});
    game_stats_table.add_row({"Lower CL", to_string(report.GameSummary.lower_level*100)});
    game_stats_table.add_row({"Risk", to_string(report.GameSummary.risk)});


    Table rtp_breakdown;
    rtp_breakdown.add_row({"Multiplier Range", "RTP", "Hit"});
    rtp_breakdown.add_row({" 0x <-> 1x ",  to_string(report.GameSummary.wins_lose*100),  to_string(report.GameSummary.wins_lose_hit)});
    rtp_breakdown.add_row({" 1x <-> 12x ",  to_string(report.GameSummary.wins_low*100),  to_string(report.GameSummary.wins_low_hit)});
    rtp_breakdown.add_row({" 12x <-> 40x ",  to_string(report.GameSummary.wins_h1*100),  to_string(report.GameSummary.wins_h1_hit)});
    rtp_breakdown.add_row({"40x -> ",  to_string(report.GameSummary.wins_h2*100),  to_string(report.GameSummary.wins_h2_hit)});


    Table top_feature;
    Table top_stats;
    top_stats.add_row({"Top Feature","RTP", "Variance", "Variance\nw/Bonus",
                       "Standard Dev", "Standard Dev\nw/Bonus", "Hit Frequency"});
    top_stats.add_row({"Entire Game",
                       to_string(report.GameSummary.RTP*100),
                       to_string(  report.FeatureSummary.GameVariance),
                       to_string(  report.FeatureSummary.GameVarianceBonus),
                       to_string(  report.FeatureSummary.STD),
                       to_string(  report.FeatureSummary.STDBonus),
                       to_string( report.FeatureSummary.HitFrequency)});

    top_stats.add_row({"Base Game",
                       to_string(report.FeatureSummary.BaseRTP*100),
                       to_string(report.FeatureSummary.BaseVariance),
                       "",
                       to_string(report.FeatureSummary.BaseSTD),
                       "",
                       to_string(report.FeatureSummary.BaseHitFrequency)});

    top_stats.add_row({"Free Games",
                       to_string(report.FeatureSummary.FGRTP*100),
                       to_string(report.FeatureSummary.FGVariance),
                       " ",
                       to_string(report.FeatureSummary.FGSTD),
                       " ",
                       to_string(report.FeatureSummary.FGHitFrequency)});



    top_stats.add_row({"Coins",
                       to_string(report.FeatureSummary.CoinRTP*100),
                       to_string(report.FeatureSummary.CoinVariance),
                       to_string(report.FeatureSummary.CoinVarianceBonus),
                       to_string(report.FeatureSummary.CoinSTD),
                       to_string(report.FeatureSummary.CoinSTDBonus),
                       to_string(1)});

    top_stats.add_row({"Coins Stand alone",
                       to_string(report.FeatureSummary.CoinBaseRTP*100),
                       to_string(report.FeatureSummary.CoinVariance),
                       "",
                       to_string(report.FeatureSummary.CoinSTD),
                       ""
                      });


    top_stats.add_row({"JackPots",
                       to_string(report.FeatureSummary.CoinJackpotRTP*100),
                       "",
                       "",
                       "",
                       "",
                       "1 in "+to_string(
                               static_cast<int>(1 / (report.FeatureSummary.JackpotHitFrequency[0] +
                                                     report.FeatureSummary.JackpotHitFrequency[1] +
                                                     report.FeatureSummary.JackpotHitFrequency[2] +
                                                     report.FeatureSummary.JackpotHitFrequency[3] +
                                                     report.FeatureSummary.JackpotHitFrequency[4])
                               )),
                      });

    Table jackpots;
    jackpots.add_row({"Values", "20", "100", "2,000", "500", "50"} );
    jackpots.add_row({"RTP",
                      to_string(report.FeatureSummary.JackpotRTP[0]*100  ),
                      to_string(report.FeatureSummary.JackpotRTP[1]*100  ),
                      to_string(report.FeatureSummary.JackpotRTP[2]*100  ),
                      to_string(report.FeatureSummary.JackpotRTP[3]*100  ) ,
                      to_string(report.FeatureSummary.JackpotRTP[4]*100 )} );
    jackpots.add_row({"Reel\nRTP",
                      to_string(report.FeatureSummary.ReelPoints[0]*100  ),
                      to_string(report.FeatureSummary.ReelPoints[1]*100  ),
                      to_string(report.FeatureSummary.ReelPoints[2]*100  ),
                      to_string(report.FeatureSummary.ReelPoints[3]*100  ) ,
                      to_string(report.FeatureSummary.ReelPoints[4]*100 )} );
    jackpots.add_row({"HF",
                      "1 in " + to_string(static_cast<int>(1 / report.FeatureSummary.JackpotHitFrequency[0])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[1])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[2])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[3])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.JackpotHitFrequency[4]))});
    jackpots.add_row({"HF 6",
                      "1 in " + to_string(static_cast<int>(1 / report.FeatureSummary.SixStackOnReelHit[0])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[1])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[2])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[3])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.SixStackOnReelHit[4]))});
    jackpots.add_row({"HF 5",
                      "1 in " + to_string(static_cast<int>(1 / report.FeatureSummary.FiveStackOnReelHit[0])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[1])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[2])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[3])),
                      "1 in " + to_string(static_cast<int>(1/ report.FeatureSummary.FiveStackOnReelHit[4]))});

    jackpots.add_row({"Coin Sum",
                      to_string(report.FeatureSummary.coin_sum[0]),
                      to_string(report.FeatureSummary.coin_sum[1]),
                      to_string(report.FeatureSummary.coin_sum[2]),
                      to_string(report.FeatureSummary.coin_sum[3]),
                      to_string(report.FeatureSummary.coin_sum[4])});





    double ave_coins = 0.0;
    for (int i = 0; i < 5; ++i) {
        ave_coins += report.FeatureSummary.UnlockedPositions[i];
    }
    jackpots.add_row({"ANC",to_string(ave_coins),"","","",""});
    jackpots.add_row({"P(J)","1 in "+to_string(
            static_cast<int>(1 / (report.FeatureSummary.JackpotHitFrequency[0] +
                            report.FeatureSummary.JackpotHitFrequency[1] +
                             report.FeatureSummary.JackpotHitFrequency[2] +
                             report.FeatureSummary.JackpotHitFrequency[3] +
                             report.FeatureSummary.JackpotHitFrequency[4])
                            )
    ),"","","",""});



    Table top_points;
    top_points.add_row({"Feature", "Points x Bet"});



    top_points.add_row({"Game",
                        to_string(report.FeatureSummary.TopTenPoints[0]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[1]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[2]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[3]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[4]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[5]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[6]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[7]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[8]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPoints[9]/50)
                       });
    top_points.add_row({"Free Game",
                        to_string(report.FeatureSummary.TopTenPointsFG[0]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[1]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[2]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[3]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[4]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[5]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[6]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[7]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[8]/50)+ " , " +
                        to_string(report.FeatureSummary.TopTenPointsFG[9]/50)
                       });

    top_points.add_row({"Coin Game",
                        to_string(report.FeatureSummary.TopTenPointsCG[0]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[1]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[2]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[3]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[4]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[5]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[6]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[7]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[8]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsCG[9]/50)
                       });

    top_points.add_row({"Base Game",
                        to_string(report.FeatureSummary.TopTenPointsBase[0]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[1]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[2]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[3]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[4]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[5]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[6]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[7]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[8]/50) + " , " +
                        to_string(report.FeatureSummary.TopTenPointsBase[9]/50)
                       });


    Table feature_occurance;
    feature_occurance.add_row({"Feature", "Hit Into Rate"});
    feature_occurance.add_row({"Free Games", "1 in " + to_string(1.0/ report.FeatureSummary.HitFrequencyIntoFreeGames)});
    feature_occurance.add_row({"Coin Games", "1 in " +  to_string(1.0/ report.FeatureSummary.HitFrequencyIntoCoinGames)});

    std::cout << " Average Return bet for free games " << report.FeatureSummary.FGRTP / (report.FeatureSummary.HitFrequencyIntoFreeGames) << std::endl;
    std::cout << " Free Games Price " << (report.FeatureSummary.FGRTP / (report.FeatureSummary.HitFrequencyIntoFreeGames)) / 0.95 << std::endl;

    std::cout << " Average Return bet for coin games " << report.FeatureSummary.FGRTP / (report.FeatureSummary.HitFrequencyIntoFreeGames) << std::endl;
    std::cout << " Coin Games Price " << (report.FeatureSummary.CoinRTP / (report.FeatureSummary.HitFrequencyIntoCoinGames)) / 0.95 << std::endl;


    ReportTable.add_row({game_stats_table});
    ReportTable.add_row({rtp_breakdown});
    ReportTable.add_row({top_stats});
    ReportTable.add_row({jackpots});
    ReportTable.add_row({top_points});
    ReportTable.add_row({feature_occurance});
    cout << ReportTable << endl;
    double checkrtp_ = 0;
    cout << endl;
    cout << setw(34) << "Hit Frequency (%)"<< setw(43) << "RTP" << endl;
    cout << "Base" << endl;
    for (int l = 0; l < 10; ++l) {

        cout <<  std::fixed << std::setprecision(0);

        cout << setw(2) << l << "   ";
        cout <<  std::fixed << std::setprecision(5);

        for (int k = 0; k < 5; ++k) {
            double val = report.FeatureSummary.BaseSymbolRTP[l][k];
            if (val > 0) {

            cout << setw(8) << report.FeatureSummary.BaseSymbolHit[l][k] * 100 << " "; // Base Hit values
            }
            else {
                cout<<  setw(8) << "       " << " ";
            }
        }
        cout << "    ";

        for (int k = 0; k < 5; ++k) {
            double val = report.FeatureSummary.BaseSymbolRTP[l][k];
            checkrtp_ += val;
            if (val > 0) {
                cout << setw(8) <<  val* 100 << " "; // Base rtp values
            }
            else {
                cout<<  setw(8) << "       " << " ";
            }
        }
        cout << endl;

    }
    cout << checkrtp_*100 << endl;
    cout << endl << endl;

    cout << "Free Games" << endl;
    for (int l = 0; l < 10; ++l) {
        cout <<  std::fixed << std::setprecision(0);

        cout << setw(2) << l << "   ";
        cout <<  std::fixed << std::setprecision(5);

        for (int k = 0; k < 5; ++k) {
            double val =  report.FeatureSummary.FGSymbolHits[l][k];
            if (val > 0) {
                cout << setw(8) << report.FeatureSummary.FGSymbolHits[l][k] * 100<< " "; // Base rtp values
            }
            else {
                cout<<  setw(8) << "       " << " ";
            }

        }

        cout << "    ";
        for (int k = 0; k < 5; ++k) {
            double val =  report.FeatureSummary.FGSymbolRTP[l][k];
            if (val > 0) {
                cout << setw(8) << val* 100 << " ";
            }
            else {
                cout<<  setw(8) << "       " << " ";
            }
        }
        cout << endl;

    }
    cout << endl << endl;



    cout << "Coin Games" << endl;
    cout << "Average Unlocked positions : ";
    for (int i = 0; i < 5; ++i) {
        cout << report.FeatureSummary.UnlockedPositions[i] << " ";
    }
    cout << endl;
    double testCoinsRTP = 0;
    double testCoinsHit = 0;
    int breaking = 0;
    for (const auto& pair : report.FeatureSummary.CoinsEndingRTPDict) {
        testCoinsRTP +=pair.second;
        testCoinsHit += report.FeatureSummary.CoinsEndingHitFreqDict[ pair.first];


        int dec = static_cast<int>(1.0 / report.FeatureSummary.CoinsEndingHitFreqDict[ pair.first]);
        cout << pair.first;

        if (pair.first < 10){
            cout << "   ";
        } else {
            cout << "  ";
        }

        cout << " RTP: " << pair.second*100 ;
        cout <<  " HF: 1 in " << dec ;


        if (dec < 10) {
            cout << "    | ";
        }
        if (dec >= 10 && dec < 100) {
            cout << "   | ";

        }
        if (dec >= 100 && dec < 1000) {
            cout << "  | ";

        }
        if (dec >= 1000 && dec < 10000) {
            cout << " | ";

        }

        ++breaking;
        if (breaking == 3) {
            cout << endl;
            breaking = 0;
        }
    }
    cout << endl;
    cout << testCoinsRTP*100 << " " << testCoinsHit << endl;
    cout << endl << endl ;
    breaking = 0;


    cout << "Free Games"<< endl;
    double testFGRTP = 0;
    double testFGHit = 0;
//    for (const auto& pair : report.FeatureSummary.FreeGamesEndingWaysRTPDict) {
//        testFGRTP += pair.second;
//        testFGHit += report.FeatureSummary.FreeGamesEndingWaysHitFreqDict[ pair.first];
//        int dec = static_cast<int>(1.0/report.FeatureSummary.FreeGamesEndingWaysHitFreqDict[ pair.first]);
//        cout << pair.first;
//
//        if (pair.first < 1000){
//            cout << "  ";
//        } else {
//            cout << " ";
//        }
//        cout << "RTP: " << pair.second*100 ;
//        cout << " HF: " <<  report.FeatureSummary.FreeGamesEndingWaysHitFreqDict[ pair.first] * 100 ;
//
//        if (dec < 10) {
//            cout << "    | ";
//        }
//        if (dec >= 10 && dec < 100) {
//            cout << "   | ";
//        }
//        if (dec >= 100 && dec < 1000) {
//            cout << "  | ";
//        }
//        if (dec >= 1000 && dec < 10000) {
//            cout << " | ";
//        }
//
//
//        ++breaking;
//        if (breaking == 3) {
//            cout << endl;
//            breaking = 0;
//        }
//    }
//    cout << endl;
//    cout << testFGRTP << " " << testFGHit << endl;
    double checkrtpfg = 0;
    double cum_freq = 0;
    cout << setw(8) << "HF" <<setw(12) << "HF%" << setw(10) <<"CHF" << setw(10) << "RTP"<< endl;

    for (int i =0; i < 11;++i) {
        cum_freq +=  report.FeatureSummary.FreeGamesEndingCoins[i] * 100;
        cout <<i <<"   1 in " <<      static_cast<
                int>(1 / report.FeatureSummary.FreeGamesEndingCoins[i]) << "  " << report.FeatureSummary.FreeGamesEndingCoins[i] * 100 << "%  "  <<  cum_freq  << "  ";
        checkrtpfg += report.FeatureSummary.FreeGamesEndingCoinsRTP[i] * 100;
        cout << report.FeatureSummary.FreeGamesEndingCoinsRTP[i] * 100  << endl;
    }
    cout << endl;
    cout << checkrtpfg;

}














