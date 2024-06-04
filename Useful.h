
#ifndef TOTHETOPTESTING_USEFUL_H
#define TOTHETOPTESTING_USEFUL_H
#include <iostream>
#include <random>
#include <vector>
#include <chrono>


//int seed = 123;
static std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

std::vector<std::vector<int>> GameView(std::vector<std::vector<int>>& reel);
std::string formatWithCommas(long long int value);
std::array<long long int, 8> GetHighWins(const std::vector<int>& flat_array, int default_bet);

#endif //TOTHETOPTESTING_USEFUL_H
