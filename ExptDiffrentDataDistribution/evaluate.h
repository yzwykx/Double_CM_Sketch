#ifndef EVALUATE_H_INCLUDED
#define EVALUATE_H_INCLUDED

#include <bits/stdc++.h>

std::vector<double> Recall_and_Average_Error(std::ifstream& inFile_gt_att, std::ifstream& inFile_gt_all, std::ifstream& inFile_dcm) {

    int currentNum;
    int index = 0;
    int tmp = 0;
    std::vector<std::vector<int>> dcm_result {};
    std::vector<int> dcm_times {};
    std::vector<std::vector<int>> gt_result {};
    std::vector<std::vector<int>> gt_result_all {};
    std::vector<int> gt_times {};

    while (inFile_dcm >> currentNum) {
        if (index == 2) {
            index = 0;
            dcm_times.push_back(currentNum);
        }
        else {
            if (index == 0) tmp = currentNum;
            else dcm_result.push_back({tmp, currentNum});
            index++;
        }
    }

    index = 0;
    while (inFile_gt_att >> currentNum) {
        if (index == 2) {
            index = 0;
            // std::cout << currentNum << std::endl;
        }
        else {
            if (index == 0) tmp = currentNum;
            else gt_result.push_back({tmp, currentNum});
            index++;
        }
    }

    index = 0;
    while (inFile_gt_all >> currentNum) {
        if (index == 2) {
            index = 0;
            gt_times.push_back(currentNum);

        }
        else {
            if (index == 0) tmp = currentNum;
            else gt_result_all.push_back({tmp, currentNum});
            index++;
        }
    }

    double count_num = 0.0;
    double error = 0.0;
    index = 0;
    for (auto dcm_tmp : dcm_result) {
        auto it = std::find(gt_result.begin(), gt_result.end(), dcm_tmp);
        if (it != gt_result.end()) {
            count_num += 1;
        }
        auto it_gt = std::find(gt_result_all.begin(), gt_result_all.end(), dcm_tmp);
        int index_gt = int(std::distance(gt_result_all.begin(), it_gt));
        error += std::abs(dcm_times[index] / gt_times[index_gt] - 1);
        index++;
        if (index == 5 || index == 10 || index == 20 || index == 50) {
            double res1_tmp = 100.0 * count_num / double(index);
            double res2_tmp = double(error) / (index + 1e-7);
            std::cout << "RR_" << index << ": " << res1_tmp << ", AAE_" << index << ": " << res2_tmp << std::endl;
        }
    }

    double res1 = count_num / double(dcm_result.size());
    double res2 = double(error) / (dcm_result.size() + 1e-7);
    //std::cout << res1 << " " << res2 << std::endl;
    std::vector<double> res(2);
    res[0] = res1; res[1] = res2;
    return res;
}

#endif // EVALUATE_H_INCLUDED
