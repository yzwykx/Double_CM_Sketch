#ifndef DOUBLECM_H_INCLUDED
#define DOUBLECM_H_INCLUDED

#include <bits/stdc++.h>

void CoverMin_and_CountMin(std::ifstream& inFile, const int range, const int num_array_cover, const int num_bucket_cover, const int num_array_count, const int num_bucket_count, int attention, std::vector<std::vector<int>>& possible_result) {
    std::vector<int> cover_min_table[num_array_cover];
    std::vector<int> count_min_table[num_array_count];
    possible_result = {};
    for (int i = 0; i < num_array_cover; i++) {
        cover_min_table[i].resize(num_bucket_cover);
        for (int j = 0; j < num_bucket_cover; j++) {
            cover_min_table[i][j] = -1;
        }
    }
    for (int i = 0; i < num_array_count; i++) {
        count_min_table[i].resize(num_bucket_count);
    }

    int num_index = 0;
    int currentNum;
    while (inFile >> currentNum) {
        num_index++;
        // Cover Min Sketch
        int min_index = MAX_VALUE;
        for (int i = 0; i < num_array_cover; i++) {
            int hash_key = hashFunction_cover(currentNum, range, num_bucket_cover, i);
            if (cover_min_table[i][hash_key] == -1) cover_min_table[i][hash_key] = num_index;
            else {
                min_index = std::min(min_index, cover_min_table[i][hash_key]);
                cover_min_table[i][hash_key] = num_index;
            }
        }
        // Count Min Sketch
        if (min_index != MAX_VALUE) {
            std::vector<int> tmp = {currentNum, num_index - min_index, 0};
            if (std::find(possible_result.begin(), possible_result.end(), tmp) == possible_result.end()) {
                possible_result.push_back(tmp);
            }
            for (int i = 0; i < num_array_count; i++) {
                int hash_key = hashFunction_count(currentNum, num_index - min_index, range, num_bucket_count, i);
                count_min_table[i][hash_key]++;
            }
        }
    }

    for (int i = 0; i < possible_result.size(); i++) {
        int tmp = MAX_VALUE;
        for (int j = 0; j < num_array_count; j++) {
            int hash_key = hashFunction_count(possible_result[i][0], possible_result[i][1], range, num_bucket_count, j);
            tmp = std::min(tmp, count_min_table[j][hash_key]);
        }
        possible_result[i][2] = tmp;
    }

    std::sort(possible_result.begin(), possible_result.end(), vector_cmp);

    std::cout << "Successfully generate the dcm result!" << std::endl;
}

#endif // DOUBLECM_H_INCLUDED
