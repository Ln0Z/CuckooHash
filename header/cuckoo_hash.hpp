#ifndef CUCKOO_HASH
#define CUCKOO_HASH
#include <vector>
#include <optional>

class CuckooHash{

    public: 
        CuckooHash() : size_(0), capacity_(8), max_load(0.5), max_steps(10), h1(capacity_), h2(capacity_) {}

        CuckooHash(const std::initializer_list<int>& vals) : size_(0), capacity_(8), max_load(0.5), max_steps(10), h1(capacity_), h2(capacity_){
            for (int x : vals){
                insert(x);
            }
        }

        ~CuckooHash() = default;

        //Main functionality
        virtual void insert(int key);
        int contains(int key);
        bool erase(int key);


        //Getter methods for tests
        std::vector<std::optional<int>> return_h1();
        std::vector<std::optional<int>> return_h2();
        size_t size() const;
        size_t capacity() const;

    private: 
        //Helper methods
        virtual void rehash();

        void clear();

        bool empty();

        float load_factor() const;
        
        virtual size_t hash_1(int key);
        virtual size_t hash_2(int key);
        
        size_t size_, capacity_, max_steps;
        float max_load;
        std::vector<std::optional<int>> h1, h2;
};

#endif