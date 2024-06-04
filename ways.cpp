#include "header.h"


#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

using std::accumulate;
using std::cout;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

static mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());




void GetGameViewWays(const std::vector<std::vector<int>>& base_vec,std::vector<std::vector<int>>& game_view,int pos_unlocked[5], int &rounds) {
    for (int i = 0; i < 5; ++i) {
        if (i >= base_vec.size() || base_vec[i].size() < 3) {
            continue;
        }
        auto row = base_vec[i];
        size_t length_of_wheel = row.size();
        row.push_back(row[0]);
        row.push_back(row[1]);
        row.push_back(row[2]);
        row.push_back(row[3]);
        uniform_int_distribution<> distrib(0, length_of_wheel-1);
        int random_index = distrib(gen);
        int single_wheel[5] = {row[random_index],
                               row[random_index + 1],
                               row[random_index + 2],
                               row[random_index + 3],
                               row[random_index + 4]};

        int j = 0;
        bool update_round = false;
        while (j <  pos_unlocked[i] ) {
            if (pos_unlocked[i] == 5 && single_wheel[j] == 11) {
                game_view[i][j] = 9;
            }
            else {
                game_view[i][j] = single_wheel[j];
            }

            if (pos_unlocked[i] < 5 && single_wheel[j] == 11) {
                pos_unlocked[i] += 1;
                update_round=true;
            }

            ++j;
        }
        if (update_round) {
            rounds++;
        }

    }
}


FreeGamesResults FreeGames(std::map<std::string, std::vector<std::vector<int>>> &AllReels,
                           std::vector<std::vector<int>> &paytable) {

    // SET UP
    FreeGamesResults Results;
    int round = 0;
    int numRows = 5;
    int numCols = 5;
    int round_points;
    int free_games_points = 0;
    int game_rounds = 10;
    int price = 45 * 50;
    int pos_unlcoked[5] = {3,3,3,3,3};
    std::vector<std::vector<int>> feature_game_view(numRows, std::vector<int>(numCols, -1));


    while (round < game_rounds) {
        round_points = 0;
        std::vector<std::vector<int>> Reel = AllReels[std::to_string(0)];
        GetGameViewWays(Reel, feature_game_view,pos_unlcoked, game_rounds );

        std::vector<std::vector<int>> pay_vector(12,std::vector<int>(5, 0));

        for (int i =0; i <5; ++i) {
            int j = 0;
            for (int val : feature_game_view[i]) {
                if (val!=-1) {
                    pay_vector[feature_game_view[i][j]][i] += 1;
                }
                j+=1;
            }
        }
        for (int symbol = 0 ; symbol < 9; ++symbol) {
            int repeats = 0;
            int symbolPoints = 0;
            int multiplier = 1;
            bool symbolAccept = false;
            while (pay_vector[symbol][repeats] > 0 || pay_vector[9][repeats] > 0) {
                if (pay_vector[symbol][repeats] > 0) {
                    symbolAccept = true;
                }
                multiplier *= pay_vector[symbol][repeats] + pay_vector[9][repeats];
                symbolPoints = (paytable[symbol][repeats] * multiplier) ;

                if (repeats==4) {
                    repeats += 1;

                    break;
                }
                repeats += 1;
            }

            if (symbolPoints > 0 && symbolAccept) {
                repeats -= 1;
                Results.logs[std::to_string(round)]["points"].push_back(
                        std::to_string(symbol) + "~" + std::to_string(repeats) + "~" + std::to_string(multiplier));
                round_points += symbolPoints;
                free_games_points += symbolPoints;
                Results.SymbolPoints[symbol][repeats] += symbolPoints;
                Results.TotalPoints += symbolPoints;
                Results.SymbolHit[symbol][repeats] += 1;
            }
        }
        if( round_points > 0) {
            Results.FreeGameHit+=1;
        }
        Results.FreeGamesEndingCoinsRTP[game_rounds - 10] += round_points;
        Results.logs[std::to_string(round)]["round_points"] = round_points;
        ++round;
    }
    Results.FreeGameTotalRounds = game_rounds;
    Results.FreeGamesEndingCoins[game_rounds - 10] += 1;
    return Results;
}