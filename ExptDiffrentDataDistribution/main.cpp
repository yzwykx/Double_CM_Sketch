#include <bits/stdc++.h>

#include "basic.h"
#include "doubleCM.h"
#include "evaluate.h"

int main() {

    // Preparation

    // Define some constants that control the generation
    const int range = 30;
    const int length = 1e6;
    const int type = 5;
    const int type_start = 0;
    const int attention = 50; // The parameter attention should be greater than 50, otherwise you should modify something in evaluate.h to make the parameter accustomed.

    // Generate prime numbers, used for hash function
    for (int i = 3001; i < 110000; ++i) {
        if (isPrime(i)) {
            Prime.push_back(i);
        }
    }


    // Generation part

    std::vector<int> numbers {};

    for (int i = type_start; i < type; i++) { // Generate random data with different types
        numbers = {};
        generateNumbers(numbers, length, range, i);
        std::ostringstream oss;
        oss << "num_" << i << "_prob.txt";
        std::ofstream outFile(oss.str());
        WriteResult(numbers, outFile, length);
    }

    for (int i = type_start; i < type; i++) { // Count the ground truth number of different intervals for each type
        std::ostringstream oss_in;
        std::ostringstream oss_out_all;
        std::ostringstream oss_out_att;
        oss_in << "num_" << i << "_prob.txt";
        oss_out_all << "gt_" << i << "_prob_all.txt";
        oss_out_att << "gt_" << i << "_prob_att.txt";
        std::ifstream inFile(oss_in.str());
        std::ofstream outFile_all(oss_out_all.str());
        std::ofstream outFile_att(oss_out_att.str());
        generateGT(inFile, outFile_all, outFile_att, range, attention);
    }


    // Apply Double CM Sketch Algorithm

    // Define some constants that control the size of Cover-Min Sketch and Count-Min Sketch
    const int num_bucket_cover = 13; // Be much less than range of data
    const int num_array_cover = 5; // Note that num_array_cover times num_bucket_cover should be less than the range of data
    const int num_bucket_count = 1003; // Be much less than range of data
    const int num_array_count = 15; // Note that num_array_count times num_bucket_count should be less than the range of data

    for (int i = type_start; i < type; i++) { // Count the number of different intervals for each type using dcm algorithm
        std::ostringstream oss_in;
        std::ostringstream oss_out;
        oss_in << "num_" << i << "_prob.txt";
        oss_out << "dcm_" << i << "_att.txt";
        std::ifstream inFile(oss_in.str());
        std::ofstream outFile(oss_out.str());
        std::vector<std::vector<int>> possible_result {};
        CoverMin_and_CountMin(inFile, range, num_array_cover, num_bucket_cover, num_array_count, num_bucket_count, attention, possible_result);
        int final_time = MAX_VALUE;
        for (int i = 0; i < attention; i++) {
            outFile << possible_result[i][0] << " " << possible_result[i][1] << " " << possible_result[i][2] << std::endl;
            if (possible_result[i][2] < final_time) final_time = possible_result[i][2];
        }

        if (attention == possible_result.size()) {
            outFile.close();
            return 0;
        }
        int index = attention;
        while (index < possible_result.size() && possible_result[index][2] == final_time) {
            outFile << possible_result[index][0] << " " << possible_result[index][1] << " " << possible_result[index][2] << std::endl;
            index++;
        }

        outFile.close();
    }


    // Evaluate part

    for (int i = type_start; i < type; i++) { //
        std::ostringstream oss_in_gt;
        std::ostringstream oss_in_gt_all;
        std::ostringstream oss_in_dcm;
        oss_in_gt << "gt_" << i << "_prob_att.txt";
        oss_in_gt_all << "gt_" << i << "_prob_all.txt";
        oss_in_dcm << "dcm_" << i << "_att.txt";
        std::ifstream inFile_gt(oss_in_gt.str());
        std::ifstream inFile_gt_all(oss_in_gt_all.str());
        std::ifstream inFile_dcm(oss_in_dcm.str());
        std::vector<double> res = Recall_and_Average_Error(inFile_gt, inFile_gt_all, inFile_dcm);
        // std::cout << res[0] << " " << res[1] << std::endl;
    }

    return 0;
}
