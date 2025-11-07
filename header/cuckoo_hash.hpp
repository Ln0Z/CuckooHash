#ifndef CUCKOO_HASH
#define CUCKOO_HASH
#include <vector>
#include <optional>
#include <cmath>

class CuckooHash{
    public: 
        CuckooHash() : size_index(0), size_(0), capacity_(sizes[size_index]), max_load(0.5), max_steps(static_cast<size_t>(std::ceil(log2(sizes[size_index])))), h1(capacity_), h2(capacity_) {}

        CuckooHash(const std::initializer_list<int>& vals) : size_(0), capacity_(8), max_load(0.5), max_steps(10), h1(capacity_), h2(capacity_){
            for (int x : vals){
                insert(x);
            }
        }

        ~CuckooHash() = default;

        //Main functionality
        virtual void insert(int key);
        int contains(int key);
        bool erase(int key);
        void clear();
        bool empty();

        //Getter methods for tests
        const std::vector<std::optional<int>>& h1_bucket() const;
        const std::vector<std::optional<int>>& h2_bucket() const;
        size_t get_hash_1(int key);
        size_t get_hash_2(int key);
        float load_factor() const;
        size_t size() const;
        size_t capacity() const;

    protected: 
        const std::vector<size_t> sizes{13ul, 29ul, 59ul, 127ul, 257ul, 541ul,
            1'109ul, 2'357ul, 5'087ul, 10'273ul, 20'753ul, 42'043ul,
            85'229ul, 172'933ul, 351'061ul, 712'697ul, 1'447'153ul, 2'938'679ul
        };

        //Helper methods
        virtual void rehash(size_t new_size);       
        virtual size_t hash_1(int key);
        virtual size_t hash_2(int key);
        
        size_t size_index, size_, capacity_, max_steps;
        float max_load;
        std::vector<std::optional<int>> h1, h2;
        friend class CuckooHashTest;
};

#endif