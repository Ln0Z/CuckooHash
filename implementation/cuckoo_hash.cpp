#include <iostream>
#include "cuckoo_hash.hpp"

void CuckooHash::insert(int key){
    if (contains(key) == 1 || contains(key) == 2) return;

    //Initialise variables
    size_t hash = hash_1(key);
    ++size_;
    bool is_hash_1 = true;
    int cuckoo;
    int counter{0};
    float load_factor_ = load_factor();
    
    //Run a loop where the check will alternatively check each vector to see if the hashed key value has a stored value in the bucket
    //If a value is contained in the bucket, evict the value and then rehash the value into the other bucket
    //Loop will continue to evict and rehash until a vacant bucket is found or the predefined max steps is reached and will then trigger a rehash.
    while(counter < max_steps){
        if (is_hash_1){
            if (!(h1[hash].has_value())){
                h1[hash] = key;
                break;
            } else{
                cuckoo = h1[hash].value();
                h1[hash] = key;
                hash = hash_2(cuckoo);
                is_hash_1 = false;
            }
        } else{
            if (!(h2[hash].has_value())){
                h2[hash] = cuckoo;
                break;
            } else{
                key = h2[hash].value();
                h2[hash] = cuckoo;
                hash = hash_1(key);
                is_hash_1 = true;
            }
        }
        ++counter;
    }
    if (load_factor() > max_load || counter == max_steps){
        std::cout << "Triggering Rehash" << "\n";
        ++size_index;
        rehash(sizes[size_index]);
    }
}

//Contains returns the int of which bucket the value belongs in for check in erase method and it returns -1 if it does not belong to a bucket.
int CuckooHash::contains(int key){
    //Hash both key for both vectors.
    size_t key_1 = hash_1(key);
    size_t key_2 = hash_2(key);
    
    //Check if value is in vec h1 and resets to default std::optional<int>
    if (h1[key_1] && *h1[key_1] == key){
        return 1;
    } else if (h2[key_2] && *h2[key_2] == key){
        return 2;
    }
    return -1;
}


bool CuckooHash::erase(int key){
    //Hash both key for both vectors.
    size_t key_1 = hash_1(key);
    size_t key_2 = hash_2(key);
    int bucket = contains(key);

    //Check if value is in vec h1 and resets to default std::optional<int>
    if (bucket == 1){
        h1[key_1].reset();
        --size_;
        return true;
    } 
    // Check if value is in vec h2 and resets to default std::optional<int>
    else if (bucket == 2){
        h2[key_2].reset();
        --size_;
        return true;
    }
    return false;
}

//Helper methods
void CuckooHash::rehash(size_t new_size){

    //Create values vector to store all the values in the cuckoo hash table.
    std::vector<int> values;
    values.reserve((new_size));
    for(size_t i = 0; i < h1.size(); ++i){
        if (h1[i]) values.push_back(h1[i].value());
        if (h2[i]) values.push_back(h2[i].value());
    }

    capacity_ = new_size;
    size_ = 0;

    h1.assign(capacity_, std::nullopt);
    h2.assign(capacity_, std::nullopt);

    //Re-insert values into the newly sized hash table as the new size will change the hash location.
    for (int x : values){
        insert(x);
    }
}

void CuckooHash::clear(){
    h1.clear();
    h2.clear();
    size_ = 0;
}

bool CuckooHash::empty(){
    return size_ == 0;
}

size_t CuckooHash::size() const {
    return size_;
}

size_t CuckooHash::capacity() const {
    return capacity_;
}

float CuckooHash::load_factor() const{
    return (float)size_ / (2 * capacity_);
}

std::vector<std::optional<int>> CuckooHash::return_h1(){
    return h1;
}

std::vector<std::optional<int>> CuckooHash::return_h2(){
    return h2;
}

//Basic hash functions to be overloaded in Deterministic and Randomised child classes for approach implementation.
size_t CuckooHash::hash_1(int key){
    return (key * 7) % capacity_;
}
size_t CuckooHash::hash_2(int key){
    return (5 * (key + 1)) % capacity_;
}