#include "../header/cuckoo_hash.hpp"

void CuckooHash::insert(int key){
    if (contains(key)) return;

    size_t hash = hash_1(key);
    ++size_;
    bool is_hash_1 = true;
    int cuckoo;
    
    while(true){

        if (is_hash_1){
            if (h1[hash] == 0){
                h1[hash] = key;
                break;
            } else{
                cuckoo = h1[hash];
                h1[hash] = key;
                hash = hash_1(cuckoo);
                is_hash_1 = false;
            }
        } else{
            if (h2[hash] == 0){
                h2[hash] = key;
                break;
            } else{
                cuckoo = h2[hash];
                h2[hash] = key;
                hash = hash_2(cuckoo);
                is_hash_1 = true;
            }
        }
    }
}

bool CuckooHash::contains(int key){

}

bool CuckooHash::erase(int key){

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

}
size_t CuckooHash::hash_2(int key){

}