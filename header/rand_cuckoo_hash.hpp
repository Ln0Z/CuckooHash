#ifndef RAND_CUCKOO_HASH
#define RAND_CUCKOO_HASH
#include "cuckoo_hash.hpp"

class RandCuckooHash : public CuckooHash {
    public:
        void insert(int key) override;
    protected:
        size_t hash_1(int key) override;
        size_t hash_2(int key) override;

        void rehash(size_t new_size) override;
    private:
        
};

#endif