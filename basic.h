#ifndef BASIC_H_INCLUDED
#define BASIC_H_INCLUDED

#include <bits/stdc++.h>

// Definition of global variables
const int MAX_VALUE = 1e7; // Bound of the top attention times

bool isPrime(int num) {
    if (num <= 1) {
        return false;
    }
    if (num <= 3) {
        return true;
    }
    if (num % 2 == 0 || num % 3 == 0) {
        return false;
    }
    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

void generateNumbers(std::vector<int>& numbers, int n, int range, int type) {

    std::random_device rd;
    std::mt19937 gen(rd());

    // Probability distribution vector
    std::vector<double> prob_dist;
    for (int i = 1; i <= range; ++i) {
        if (type == 0) prob_dist.push_back(1.0);
        if (type == 1) prob_dist.push_back(1.0 / i);
        if (type == 2) prob_dist.push_back(1.0 / i / i);
        if (type == 3) {
            if (i == 1) prob_dist.push_back(range - 1);
            else prob_dist.push_back(1.0);
        }
        if (type == 4) {
            if (i <= 3) prob_dist.push_back(range / 3.0 - 1.0);
            else prob_dist.push_back(1.0);
        }
    }

    // Generate cumulative distribution
    std::partial_sum(prob_dist.begin(), prob_dist.end(), prob_dist.begin());

    // Generate random numbers according to the distribution
    std::uniform_real_distribution<> dis(0, prob_dist.back());
    for (int i = 0; i < n; ++i) {
        double random = dis(gen);
        int index = std::lower_bound(prob_dist.begin(), prob_dist.end(), random) - prob_dist.begin();
        numbers.push_back(index);
    }
}

bool vector_cmp(std::vector<int>& a, std::vector<int>& b) { // Assert that a.size() = b.size() = 3
    return a[2] > b[2];
}

void generateGT(std::ifstream& inFile, std::ofstream& outFile_all, std::ofstream& outFile_att, const int range, int attention) {

    std::unordered_map<int, std::vector<int>> intervals; // Map to store intervals for each number

    int currentNum;
    int num_index = 0;
    while (inFile >> currentNum) {
        num_index++;
        if (intervals[currentNum].size() == 0) intervals[currentNum] = {num_index};
        else {
            intervals[currentNum].push_back(num_index);
        }
    }

    std::unordered_map<int, int> interval_num[range + 1]; // Map to store intervals for each number

    // Compute the intervals for each number
    for (const auto& pair : intervals) {
        int num = pair.first;
        const std::vector<int>& intervalCounts = pair.second;
        for (int i = 1; i < intervalCounts.size(); i++) {
            interval_num[num][intervalCounts[i] - intervalCounts[i - 1]]++;
        }
    }

    std::vector<std::vector<int>> sorted_list {};
    for (int i = 0; i <= range; i++) {
        for (const auto& interval_s_num : interval_num[i]) {
            sorted_list.push_back({i, interval_s_num.first, interval_s_num.second});
            outFile_all << i << " " << interval_s_num.first << " " << interval_s_num.second << std::endl;
        }
    }

    outFile_all.close();

    std::cout << "Successfully generate the ground truth!" << std::endl;

    // Sort the result and generate top attention items
    std::sort(sorted_list.begin(), sorted_list.end(), vector_cmp);
    int final_time = MAX_VALUE;
    for (int i = 0; i < attention; i++) {
        outFile_att << sorted_list[i][0] << " " << sorted_list[i][1] << " " << sorted_list[i][2] << std::endl;
        if (sorted_list[i][2] < final_time) final_time = sorted_list[i][2];
    }

    if (attention == sorted_list.size()) {
        outFile_att.close();
        return;
    }
    int index = attention;
    while (index < sorted_list.size() && sorted_list[index][2] == final_time) {
        outFile_att << sorted_list[index][0] << " " << sorted_list[index][1] << " " << sorted_list[index][2] << std::endl;
        index++;
    }

    outFile_att.close();
}

void WriteResult(std::vector<int>& numbers, std::ofstream& outFile, const int length) {
    for (int i = 0; i < length; ++i) {
        outFile << numbers[i];
        if (i != length - 1) outFile << " ";
    }
    outFile.close();
}


std::vector<int> Prime {}; // Generate prime number list

int hashFunction_cover(int key, int range_num, int range_bucket, int type) {
    /*
    if (type == 0) { // Multiplicative Hashing
        const int a = 7;
        return (a * key) % range_bucket;
    }
    if (type == 1) { // Bitwise XOR Hashing
        key ^= (key << 13);
        key ^= (key >> 17);
        key ^= (key << 5);
        return key % range_bucket;
    }
    else {
    */
    // Division Hashing
    int p = Prime[type % Prime.size()]; // Should be a prime number greater than range_num
    return (key * (key + 3) * (key + 7)) % p % range_bucket;

}

int hashFunction_count(int key, int interval, int range_num, int range_bucket, int type) {
    int p = Prime[type % Prime.size()];
    return ((key * (key + 3) * (key + 7)) + interval * (interval + 3)) % p % range_bucket;
}


#endif // BASIC_H_INCLUDED
