#ifndef OURS_H
#define OURS_H

#include "Abstract.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstring>

#define CELLNUM 16 // 增加单元格数量以提高精度

template <uint32_t HASH_NUM>
class Ours : public Abstract {
public:

    struct Bucket {
        struct Cell {
            DATA_TYPE item;
            uint32_t interval;
            COUNT_TYPE count;
        };

        Cell cells[CELLNUM];
        COUNT_TYPE fail;
    };

    Ours(uint32_t _MEMORY, double ratio = 0.2, std::string _name = "Ours") { // 调整ratio比例
        NAME = _name;
        MEMORY = _MEMORY;

        LIGHT_LENGTH = _MEMORY * ratio / HASH_NUM / sizeof(COUNT_TYPE);
        HEAVY_LENGTH = _MEMORY * (1 - ratio) / sizeof(Bucket);

        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            counter[i] = new TIME_TYPE[LIGHT_LENGTH];
            memset(counter[i], 0, sizeof(TIME_TYPE) * LIGHT_LENGTH);
        }

        buckets = new Bucket[HEAVY_LENGTH];
        memset(buckets, 0, sizeof(Bucket) * HEAVY_LENGTH);
    }

    ~Ours() {
        for (uint32_t i = 0; i < HASH_NUM; ++i)
            delete[] counter[i];
        delete[] buckets;
    }

    void Insert(const ItemPair& item) {
        TIME_TYPE minTime = UINT64_MAX;

        // 初始CMS处理
        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item.item, i) % LIGHT_LENGTH;
            minTime = std::min(minTime, counter[i][position]);
            counter[i][position] = item.time;
        }

        // 如果时间间隔大于等于500，则不处理 (调整阈值)
        ItemPair temp(item.time - minTime, item.item);
        if (temp.time >= 500)
            return;

        // 二次CMS处理
        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(temp.item, i) % HEAVY_LENGTH;

            bool found = false;
            for (uint32_t j = 0; j < CELLNUM; ++j) {
                if (buckets[position].cells[j].item == temp.item &&
                    buckets[position].cells[j].interval == temp.time) {
                    buckets[position].cells[j].count += 1;
                    found = true;
                    break;
                }
            }
            if (found)
                continue;

            // 找到计数最小的单元格
            uint32_t min_pos = 0;
            COUNT_TYPE min_count = INT32_MAX;
            for (uint32_t j = 0; j < CELLNUM; ++j) {
                if (buckets[position].cells[j].count < min_count) {
                    min_count = buckets[position].cells[j].count;
                    min_pos = j;
                }
            }

            // 替换计数最小的单元格
            buckets[position].cells[min_pos].item = temp.item;
            buckets[position].cells[min_pos].interval = temp.time;
            buckets[position].cells[min_pos].count = 1; // 重置计数器
        }
    }

    HashMap Report(COUNT_TYPE HIT) {
        HashMap ret;

        for (uint32_t i = 0; i < HEAVY_LENGTH; ++i) {
            for (uint32_t j = 0; j < CELLNUM; ++j) {
                if (buckets[i].cells[j].count > HIT) {
                    ret[ItemPair(buckets[i].cells[j].interval,
                        buckets[i].cells[j].item)]
                        = buckets[i].cells[j].count;
                }
            }
        }

        return ret;
    }

private:
    uint32_t LIGHT_LENGTH;
    uint32_t HEAVY_LENGTH;

    TIME_TYPE* counter[HASH_NUM];
    Bucket* buckets;
};

#endif
