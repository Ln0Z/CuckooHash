#ifndef CUCKOO_HASH
#define CUCKOO_HASH
#include <vector>

class CuckooHash{

    private: 
        std::vector<int> h1;
        std::vector<int> h2;
        size_t size_;
        size_t capacity;
        float max_load;

    public: 
        CuckooHash() : size_(0), capacity(0), max_load(1.0) {};

        CuckooHash(const std::initializer_list<int>& vals) : size_(0), capacity(0), max_load(1.0){
            for (int x : vals){
                insert(x);
            }
        }

        void insert(int key);

        bool contains(int key);
        
        bool erase(int key);

        void rehash();

        void clear();

        bool empty();

        size_t size() const;

        float load_factor() const;
};

#endif