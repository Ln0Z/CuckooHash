#include "../header/cuckoo_hash.hpp"

void CuckooHash::insert(int key){

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