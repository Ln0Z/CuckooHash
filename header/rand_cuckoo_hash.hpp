#ifndef RAND_CUCKOO_HASH
#define RAND_CUCKOO_HASH

#include "cuckoo_hash.hpp"
#include <random>

class RandCuckooHash : public CuckooHash {
public:
    RandCuckooHash() : CuckooHash(), generator(std::random_device{}()) {
        // call standard CuckooHash constructor and init random generator
        // then generate the hashes
        genNewHashes();

        this->printHash1();
        this->printHash2();
    }

    explicit RandCuckooHash(int size_index) : CuckooHash(size_index), generator(std::random_device{}()) {
        // call standard CuckooHash constructor and init random generator
        // then generate the hashes
        genNewHashes();

        this->printHash1();
        this->printHash2();
    }

    void printHash1();
    void printHash2();

    size_t hash_1(int key) override;
    size_t hash_2(int key) override;

protected:
    void rehash(size_t new_size) override;

private:
    // max_int for int32_t ints is prime, so it
    // can serve as p from Carter and Wegmans' equation
    static constexpr uint32_t modulus_p = 2'147'483'647;

    uint32_t a1{};
    uint32_t b1{};
    uint32_t a2{};
    uint32_t b2{};

    std::mt19937 generator;

    void genNewHashes();

    // c++'s % is really just a remainder operator, use
    // mathematical modulo to ensure universal hash family
    // when working with negative keys
    static uint32_t realModulo(int64_t k, int64_t p) {
        int64_t result = k % p;
        if (result < 0) result = result + p;
        return result;
    }
};

#endif