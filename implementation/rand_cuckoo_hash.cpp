#include "rand_cuckoo_hash.hpp"
#include <iostream>

// create hash using Carter and Wegmans' ((ax+b) mod p) mod m

// TODO: normalize incoming negative keys
size_t RandCuckooHash::hash_1(int key) {
    return (((uint64_t)a1 * key + b1) % modulus_p) % capacity_;
}

size_t RandCuckooHash::hash_2(int key) {
    return (((uint64_t)a2 * key + b2) % modulus_p) % capacity_;
}

void RandCuckooHash::printHash1() {
    std::cout << "h1 = ((" << a1 << "k + " << b1 << ") mod " << modulus_p << ") mod " << capacity_ << std::endl;
}
void RandCuckooHash::printHash2() {
    std::cout << "h2 = ((" << a2 << "k + " << b2 << ") mod " << modulus_p << ") mod " << capacity_ << std::endl;
}

void RandCuckooHash::genNewHashes() {
    std::uniform_int_distribution<uint32_t> rangeA(1, modulus_p - 1);
    std::uniform_int_distribution<uint32_t> rangeB(0, modulus_p - 1);

    a1 = rangeA(generator);
    b1 = rangeB(generator);
    a2 = rangeA(generator);
    b2 = rangeB(generator);
}

void RandCuckooHash::rehash(size_t new_size) {
    genNewHashes();
    this->printHash1();
    this->printHash2();

    CuckooHash::rehash(new_size);
}