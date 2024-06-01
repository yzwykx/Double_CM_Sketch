#ifndef OURS_H
#define OURS_H

#include "Abstract.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <random>

#define CELLNUM 8

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

    Ours(uint32_t _MEMORY, double ratio = 0.15, std::string _name = "Ours") {
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

        // Second level CMS initialization
        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            secondCounter[i] = new COUNT_TYPE[LIGHT_LENGTH];
            memset(secondCounter[i], 0, sizeof(COUNT_TYPE) * LIGHT_LENGTH);
        }
    }

    ~Ours() {
        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            delete[] counter[i];
            delete[] secondCounter[i];
        }
        delete[] buckets;
    }

    void Insert(const ItemPair& item) {
        // First level CMS
        TIME_TYPE minTime = UINT64_MAX;
        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item.item, i) % LIGHT_LENGTH;
            minTime = MIN(minTime, counter[i][position]);
            counter[i][position] = item.time;
        }

        // Second level CMS using the updated counter array from the first level CMS
        TIME_TYPE minTimeNested = UINT64_MAX;
        for (uint32_t i = 0; i < HASH_NUM; ++i) {
            uint32_t position = hash(item.item, i) % LIGHT_LENGTH;
            minTimeNested = MIN(minTimeNested, counter[i][position]);
            secondCounter[i][position]++;
        }

        // Estimate frequency using second level CMS result
        if (item.time - minTime < 1000) {
            uint32_t position = hash(item.item, HASH_NUM) % HEAVY_LENGTH, min_pos = 0;
            COUNT_TYPE min_count = INT32_MAX;

            for (uint32_t j = 0; j < CELLNUM; ++j) {
                if (buckets[position].cells[j].item == item.item &&
                    buckets[position].cells[j].interval == item.time - minTime) {
                    buckets[position].cells[j].count += 1;
                    return;
                }

                if (buckets[position].cells[j].count < min_count) {
                    min_count = buckets[position].cells[j].count;
                    min_pos = j;
                }
            }

            if (rng() % (2 * min_count - buckets[position].fail + 1) == 0) {
                buckets[position].cells[min_pos].item = item.item;
                buckets[position].cells[min_pos].interval = item.time - minTime;
                if (min_count == 0)
                    buckets[position].cells[min_pos].count = 1;
                else
                    buckets[position].cells[min_pos].count += buckets[position].fail / min_count;
                buckets[position].fail = 0;
            }
            else {
                buckets[position].fail += 1;
            }
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
    COUNT_TYPE* secondCounter[HASH_NUM]; // Second level CMS
    Bucket* buckets;

    uint32_t hash(DATA_TYPE item, uint32_t seed) const {
        // Hash function implementation here
        // return a suitable hash value
        return std::hash<DATA_TYPE>{}(item) ^ (seed * 0x9e3779b9);
    }

    uint32_t hash(const ItemPair& item, uint32_t seed) const {
        // Hash function implementation here
        // return a suitable hash value
        return std::hash<DATA_TYPE>{}(item.item) ^ (item.time * 0x9e3779b9) ^ (seed * 0x9e3779b9);
    }

    uint32_t rng() const {
        // Random number generator implementation here
        // return a suitable random value
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, INT32_MAX);
        return dis(gen);
    }
};

#endif
