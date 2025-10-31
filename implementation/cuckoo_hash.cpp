#include "../header/cuckoo_hash.hpp"

void CuckooHash::insert(int key){
    if (contains(key)) return;

    size_t hash = hash_1(key);
    ++size_;
    bool is_hash_1 = true;
    int cuckoo;
    int counter{0};
    
    while(counter < max_steps){
        if (is_hash_1){
            if (!(h1[hash].has_value())){
                h1[hash] = key;
                break;
            } else{
                cuckoo = h1[hash].value();
                h1[hash] = key;
                hash = hash_1(cuckoo);
                is_hash_1 = false;
            }
        } else{
            if (h2[hash].has_value()){
                h2[hash] = key;
                break;
            } else{
                cuckoo = h2[hash].value();
                h2[hash] = key;
                hash = hash_2(cuckoo);
                is_hash_1 = true;
            }
        }
        ++counter;
    }
}

bool CuckooHash::contains(int key){
    size_t key_1 = hash_1(key);
    size_t key_2 = hash_2(key);

    if (h1[key_1] && *h1[key_1] == key){
        return true;
    } else if (h2[key_2] && *h2[key_2] == key){
        return true;
    }
    return false;
}


bool CuckooHash::erase(int key){
    //Hash both key for both vectors.
    size_t key_1 = hash_1(key);
    size_t key_2 = hash_2(key);

    //Check if value is in vec h1 and resets to default std::optional<int>
    if (h1[key_1] && *h1[key_1] == key){
        h1[key_1].reset();
        -size_;
        return true;
    } 
    // Check if value is in vec h2 and resets to default std::optional<int>
    else if (h2[key_2] && *h2[key_2] == key){
        h2[key_2].reset();
        -size_;
        return true;
    }
    return false;
}

//Helpter methods
void CuckooHash::rehash(){

}

void CuckooHash::clear(){

}

bool CuckooHash::empty(){
    return size_ == 0;
}

size_t CuckooHash::size() const {
    return size_;
}

float CuckooHash::load_factor() const{

}

size_t CuckooHash::hash_1(int key){
    return (key * 7) % capacity;
}
size_t CuckooHash::hash_2(int key){
    return (5 * key + 1) % capacity;
}