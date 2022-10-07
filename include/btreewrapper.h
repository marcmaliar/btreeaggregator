
#pragma once

#include <vector>
#include "btree.h"
#include "record.h"
#include "spinlock.h"

/*#include <chrono>
typedef std::chrono::high_resolution_clock Clock2;
inline long long diffInNs2(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(end-start)).count();
}*/

#define B 64
#define USE_BTREE 1
#define SIMPLE 0

struct key_of_value {
        //! pull first out of pair
    static const uint32_t& get(const Record& r) { return r.key; }
};

struct traits
{
        static const bool       self_verify = false;
        static const bool       debug = false;

        static const int        leaf_slots = 4;
        static const int        inner_slots = 256;

        static const int        binsearch_threshold = 256;
};

using BTreeType = tlx::BTree<uint32_t, Record, key_of_value, std::less<uint32_t>, traits, 1, std::allocator<Record>>;

struct less_than_btree
{
    inline bool operator() (const BTreeType& btree1, const BTreeType& btree2)
    {
        return (btree1.size() < btree2.size());
    }
};

class BTreeWrapper {
    private:
        SpinLock lock;
        #if SIMPLE == 1
        void mergeIfNecessary() {}
        #else 
        void mergeIfNecessary() {
                        
            if (btrees.size() == B) {

                std::vector<BTreeType> copy = btrees;
                btrees.clear();

                sort(copy.begin(), copy.end(), less_than_btree());

                /*for (BTreeType btree: copy) {
                    std::cout << "Size: " << btree.size() << std::endl;
                }*/
                
                auto front = copy.begin();
                auto back = copy.end(); back--;

                while (true) {

                    BTreeType btree1 = *front;
                    BTreeType btree2 = *back;

                    std::vector<Record> r1;
                    for (auto it = btree1.begin(); it != btree1.end(); it++) {
                        r1.push_back(*it);
                    }
                    std::vector<Record> r2;
                    for (auto it = btree2.begin(); it != btree2.end(); it++) {
                        r2.push_back(*it);
                    }
                    std::vector<Record> r3;
                    r3.reserve(r1.size() + r2.size());
                    std::merge(r1.begin(), r1.end(),
                            r2.begin(), r2.end(),
                            std::back_inserter(r3));
                    BTreeType btree;
                    btree.bulk_load(r3.begin(), r3.end());
                    btrees.push_back(btree);

                    front++;
                    if (front == back) break;
                    else back--;
                }
            }    
        }
        #endif
    public:
        #if SIMPLE == 1
        BTreeType btree;
        void addPacket(Packet p) {
            #if USE_BTREE == 1
            lock.lock();
            for (Record r: p.records) {
                btree.insert(r);
            }
            lock.unlock();
            #endif
        }
        int read(uint32_t id) {
            #if USE_BTREE == 1
            if (lock.try_lock()) {
                if (btree.find(id) != btree.end()) { 
                    lock.unlock();
                    return 1; 
                };
                lock.unlock();
                return 0;
            }
            return 0;
            #else
            return 1;
            #endif
        }
        #else 
        std::vector<BTreeType> btrees;
        void addPacket(Packet p) {
            #if USE_BTREE == 1
            
            BTreeType btree;
            sort(p.records.begin(), p.records.end());
            
            btree.bulk_load(p.records.begin(), p.records.end());
            
            lock.lock();
            btrees.push_back(btree);
            mergeIfNecessary();
            lock.unlock();
            btree.clear();
            #endif
        }

        int read(uint32_t id) {
            #if USE_BTREE == 1
            if (lock.try_lock()) {
                for (BTreeType btree : btrees) {
                    if (btree.find(id) != btree.end()) { lock.unlock(); return 1; };
                } 
                lock.unlock();
                return 0;
            }
            return 0;
            #else
            return 1;
            #endif
        }
        #endif
};