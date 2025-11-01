#ifndef CUCKOO_HASH
#define CUCKOO_HASH
#include <vector>
#include <optional>

class CuckooHash{

    public: 
        CuckooHash() : size_(0), capacity(1), max_load(1.0), h1(capacity), h2(capacity) {}

        CuckooHash(const std::initializer_list<int>& vals) : size_(0), capacity(1), max_load(1.0){
            for (int x : vals){
                insert(x);
            }
        }

        //Main functionality
        void insert(int key);

        int contains(int key);
        
        bool erase(int key);

    private: 
        //Helpter methods
        void rehash();

        void clear();

        bool empty();

        size_t size() const;

        float load_factor() const;

        size_t hash_1(int key);
        size_t hash_2(int key);
        
        std::vector<std::optional<int>> h1, h2;
        size_t size_, capacity, max_steps;
        float max_load;
};

#endif