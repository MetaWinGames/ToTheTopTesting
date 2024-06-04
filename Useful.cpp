
#include "Useful.h"

std::vector<std::vector<int>> GameView(std::vector<std::vector<int>>& reel) {
    std::vector<std::vector<int>> game_view(5, std::vector<int>(3, 0));
    for (int i = 0; i < 5; ++i) {
        if (i >= reel.size() || reel[i].size() < 3) {
            continue;
        }
        auto row = reel[i];
        size_t length_of_wheel = row.size();
        row.push_back(row[0]);
        row.push_back(row[1]);
        row.push_back(row[2]);
        std::uniform_int_distribution<> distrib(0, length_of_wheel - 1);
        int random_index = distrib(gen);

        int single_wheel[3] = {row[random_index], row[random_index + 1], row[random_index + 2]};

        for (int j = 0; j < 3; ++j) {
            game_view[i][j] = single_wheel[j];
        }
    }
    return game_view;
}

std::string formatWithCommas(long long int value) {
    std::string number = std::to_string(value);
    int insertPosition = number.length() - 3;

    while (insertPosition > 0) {
        number.insert(insertPosition, ",");
        insertPosition -= 3;
    }

    return number;
}

std::array<long long int, 8> GetHighWins(const std::vector<int>& flat_array, int default_bet) {
    long long int h1l = static_cast<long long int>(default_bet) * 12;
    long long int h1h = static_cast<long long int>(default_bet) * 40;
    std::array<long long int, 8> win_frequency_count = {0,0,0,0,0,0,0,0};

    // h1_hit, h1_points, h2_hit, h2_points
    for (const int val : flat_array) {
        if (val < h1h && val >= h1l) {
            ++win_frequency_count[0];
            win_frequency_count[1] += val;
        } else if (val >= h1h) {
            ++win_frequency_count[2];
            win_frequency_count[3] += val;
        }
        else if (val < h1l && val >= static_cast<long long int>(default_bet)){
            ++win_frequency_count[4];
            win_frequency_count[5] += val;
        }
        else if (val < static_cast<long long int>(default_bet) ) {
            ++win_frequency_count[6];
            win_frequency_count[7] += val;

        }
    }

    return win_frequency_count;
}
