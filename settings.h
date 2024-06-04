#include <thread>


#ifndef TOTHETOPTESTING_SETTINGS_H
#define TOTHETOPTESTING_SETTINGS_H

//    0 - Entire Game
//    2 - Scatter Game Statistics
//    3 - Coin Game Statistics
//    4 - Get plot data


constexpr int num_threads = 20;

constexpr long long int num_elements = 1'000'0000;   // Aproximate time 30s for 10 mill
constexpr long long int num_elements_scatter = 10'000'000; // Aproximate time 45s for 1 mill
constexpr long long int num_elements_coin = 50'000'000;  // Aproximate time 30s for 2 mill

constexpr int choice = 0;
constexpr bool all = false;



const int coin_num = 6;//  values 6 , 9 , 12, 15

// This has not been 100% tested
const int payment_for_coins = 50 * 20;
const int payment_for_fg = 50 * 1;

#endif //TOTHETOPTESTING_SETTINGS_H
