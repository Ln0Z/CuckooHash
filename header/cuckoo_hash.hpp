#ifndef CUCKOO_HASH
#define CUCKOO_HASH
#include <vector>

class CuckooHash{

    private: 
        std::vector<int> h1;
        std::vector<int> h2;
        size_t size_;
        float max_load;

    public: 
        CuckooHash(){};

        CuckooHash(std::initializer_list<int> vals){
            for (int x : vals){
                insert(x);
            }
        }

        void insert(int key);

        bool contains();
        
        void erase();

        void rehash();

        size_t size() const;

        float load_factor() const;
};

#endif