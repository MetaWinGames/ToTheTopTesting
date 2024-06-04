#include "header.h"

using namespace std;


// Base Game
std::array<int, 2>  analyseArray(const vector<int> &arr) {
    int wild = 9;
    if (arr[0] != 12 && arr[0] !=10 && arr[0] !=11) {
        int consecutive_counts = 0;
        int first_value = arr[0];
        for (int i=0; i < 5; ++i) {
            if (arr[i] == first_value ||  arr[i] == wild) {
                ++consecutive_counts;
            }
            else {
                break;
            }
        }
        return {consecutive_counts, first_value};
    }

    return {-1,-1};

}
vector<vector<int>> GetGameLines(vector<vector<int>> &game_view, vector<vector<int>> &paylines) {
    const size_t numLines = paylines.size();
    const size_t numElements = 5;

    std::vector<std::vector<int>> game_lines(numLines, std::vector<int>(numElements, 0));

    for (size_t line = 0; line < numLines; ++line) {
        for (size_t i = 0; i < numElements; ++i) {
            game_lines[line][i] = game_view[i][paylines[line][i]];
        }
    }

    return game_lines;
}

BaseGameResults BaseGame(vector<vector<int>> &game_view,
              vector<vector<int>> &paylines,
              vector<vector<int>> &paytable) {


    BaseGameResults GameResult;
    GameResult.TotalPoints = 0;
    GameResult.logs["game_view"] = game_view;
    GameResult.logs["wins"] = {};
    for (const auto& col : game_view) {
        for (int val: col) {
            if (val == 10) {
                ++GameResult.Scatters;
            }
            if (val == 11) {
                ++GameResult.Coins;
            }
        }
    }


    std::set<std::pair<int, int>> uniquePairs;
    vector<vector<int>> game_lines = GetGameLines(game_view,paylines);

    for (std::size_t i = 0; i < game_lines.size(); ++i) {
        std::array<int, 2> WinInfo = analyseArray(game_lines[i]);
        int occurance_index = WinInfo[0] - 1;
        int symbol = WinInfo[1];

        if (occurance_index > 0) {
            int points = paytable[symbol][occurance_index];
            GameResult.TotalPoints += points;
            GameResult.SymbolPoints[symbol][occurance_index] += points;

            auto pair = std::make_pair(symbol, occurance_index);
            if (uniquePairs.find(pair) == uniquePairs.end()) {
                GameResult.SymbolHit[symbol][occurance_index] += 1;
                uniquePairs.insert(pair);
            }

            if (points > 0) {
                GameResult.logs["wins"].push_back(to_string(symbol) + "~" + to_string(occurance_index+1) + "~"+ to_string(i));
            }


        }
    }
    GameResult.logs["points"] = GameResult.TotalPoints;

    return GameResult;

}

