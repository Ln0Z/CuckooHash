#ifndef CUCKOO_HASH
#define CUCKOO_HASH
#include <vector>

class CuckooHash{

    public: 
        CuckooHash() : size_(0), capacity(1), max_load(1.0) {};

        CuckooHash(const std::initializer_list<int>& vals) : size_(0), capacity(1), max_load(1.0){
            for (int x : vals){
                insert(x);
            }
        }

        //Main functionality
        void insert(int key);

        bool contains(int key);
        
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
        
        std::vector<int> h1;
        std::vector<int> h2;
        size_t size_;
        size_t capacity;
        float max_load;
};

#endif