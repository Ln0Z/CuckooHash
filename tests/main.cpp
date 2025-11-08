#include <gtest/gtest.h>
#include <random>
#include "cuckoo_hash.hpp"
#include <unordered_set>
#include <limits.h>
#include <iostream>
#include <algorithm>

namespace{
    std::unordered_set<int> random_set(int total_numbers, int low_rage, int high_range){
        std::random_device rand;
        std::uniform_int_distribution<int> seed_dist(1, 50);
        int seed = seed_dist(rand);
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(low_rage, high_range);
        std::unordered_set<int> random_set;

        for (size_t i = 0; i < total_numbers; ++i){
            random_set.insert(dist(rng));
        }
        return random_set;
    }
}

TEST(hash_test, load_factor_calc){
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), static_cast<float>(i) / static_cast<float>(26));
    table.insert(values[i]);
  }
}

TEST(hash_test, load_factor_calc_after_rehash){
  std::vector<int> values{1, 34, 3, 23, 12, 38, 53, 45, 2, 11, 8, 5, 6, 43};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), static_cast<float>(i) / static_cast<float>(table.capacity()));
    table.insert(values[i]);
  }
}

TEST(hash_test, load_factor_calc_after_multiple_rehash){
  std::unordered_set<int> values = random_set(200, 0, 500);
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), static_cast<float>(i) / static_cast<float>(table.capacity()));
    table.insert(i);
  }
}

TEST(hash_test, hash_distribution) {
    CuckooHash table;

    std::vector<int> keys;
    for (int i = 0; i < 1000; ++i){
      keys.push_back(i);
    }
    
    std::vector<int> counts_h1(table.capacity() / 2, 0);
    std::vector<int> counts_h2(table.capacity() / 2, 0);

    for (int key : keys) {
        size_t idx1 = table.get_hash_1(key);
        size_t idx2 = table.get_hash_2(key);

        counts_h1[idx1]++;
        counts_h2[idx2]++;
    }

    int max_load_h1 = *std::max_element(counts_h1.begin(), counts_h1.end());
    int max_load_h2 = *std::max_element(counts_h2.begin(), counts_h2.end());

    float avg_load = (float)keys.size() / counts_h1.size();
    ASSERT_LE(max_load_h1, 2 * avg_load);
    ASSERT_LE(max_load_h2, 2 * avg_load);
}

// Insert 1 element
TEST(insert_test, insert_single_element){
  CuckooHash table;
  std::unordered_set<int> standard;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.contains(4) == -1, !standard.contains(4));

  table.insert(4);
  standard.insert(4);

  ASSERT_EQ(table.contains(4) == 1, standard.contains(4));
  ASSERT_EQ(table.size(), standard.size());
  ASSERT_EQ(table.empty(), standard.empty());

  size_t idx1 = table.get_hash_1(4);
  ASSERT_EQ(*table.find(4), *standard.find(4));
}

// Insert negative elements
TEST(insert_test, insert_negative_element){
  CuckooHash table;
  std::unordered_set<int> standard;

  table.insert(-12);
  standard.insert(-12);

  ASSERT_EQ(table.contains(-12) == 1, standard.contains(-12));
  ASSERT_EQ(table.size(), standard.size());
  ASSERT_EQ(table.empty(), standard.empty());
  ASSERT_EQ(*table.find(-12), *standard.find(-12));
}

TEST(insert_test, insert_10_elements) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;
  std::unordered_set<int> standard;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
    standard.insert(values[i]);
  }

  std::vector<std::optional<int>> h1_results{23, 12, 53, 3, std::nullopt, std::nullopt, std::nullopt, 11, 39, 2, 
                                             std::nullopt, 45, 34};

  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 
                                             1, std::nullopt, std::nullopt};


  ASSERT_EQ(table.h1_bucket().size(), 13);
  ASSERT_EQ(table.h2_bucket().size(), 13);

  for (size_t i = 0; i < h1_results.size(); ++i){
      ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
      ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
  }
  
  for (auto x : standard){
    ASSERT_EQ(*table.find(x), x);
  }
}

TEST(insert_test, insert_cause_rehash) {
  std::vector<int> values{1, 34, 3, 23, 12, 38, 53, 45, 2, 11, 8, 5, 6, 43};
  CuckooHash table;

  ASSERT_LT(table.capacity(), 58);

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
    if(i == 12){
      //Next insert will trigger rehash
      ASSERT_EQ(table.load_factor(), 0.5f);
    }
  }

  ASSERT_EQ(table.capacity(), 58);

  std::vector<std::optional<int>> h1_results(29, std::nullopt);
  std::vector<std::optional<int>> h2_results(29, std::nullopt);

  h1_results[3]  = 43;
  h1_results[5]  = 6;
  h1_results[6]  = 2;
  h1_results[8]  = 23;
  h1_results[11] = 11;
  h1_results[13] = 3;
  h1_results[15] = 53;
  h1_results[17] = 45;
  h1_results[18] = 12;
  h1_results[19] = 8;
  h1_results[26]  = 38;
  h1_results[27]  = 34;
  h1_results[28] = 1;
  h2_results[1] = 5;


  ASSERT_EQ(table.h1_bucket().size(), 29);
  ASSERT_EQ(table.h2_bucket().size(), 29);
  ASSERT_EQ(table.capacity(), 58);


  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
    ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
  }
}

TEST(erase_test, erase_same_key_twice){
  CuckooHash table;
  std::unordered_set<int> standard;
  ASSERT_TRUE(table.empty());

  table.insert(15);
  standard.insert(15);

  ASSERT_EQ(table.size(), standard.size());
  
  table.erase(15);
  standard.erase(15);

  ASSERT_EQ(table.contains(15) == -1, !standard.contains(15));
  ASSERT_EQ(table.empty(), standard.empty());
  ASSERT_FALSE(table.erase(15));
}

TEST(erase_test, insert_and_erase_10_elements) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;
  std::unordered_set<int> standard;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
    standard.insert(values[i]);
  }
  
  std::vector<std::optional<int>> h1_results{23, 12, 53, 3, std::nullopt, std::nullopt, std::nullopt, 11, 39, 2, 
                                             std::nullopt, 45, 34};

  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 
                                             1, std::nullopt, std::nullopt};

  ASSERT_EQ(table.h1_bucket().size(), 13);
  ASSERT_EQ(table.h2_bucket().size(), 13);


  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
    ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
  }

  for (int x : values){
    ASSERT_EQ(*table.find(x), *standard.find(x));
  }

  ASSERT_EQ(table.size(), standard.size());

  for(auto x : table){
    table.erase(*x);
    standard.erase(*x);
  }

  ASSERT_EQ(table.size(), standard.size());

  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.h1_bucket()[i], std::nullopt);
    ASSERT_EQ(table.h2_bucket()[i], std::nullopt);
  }
}

TEST(erase_test, size_decrement_works){
  CuckooHash table;
  std::unordered_set<int> standard;

  for (size_t i = 0; i < 100; ++i){
    table.insert(i);
    standard.insert(i);  
  }

  ASSERT_EQ(table.size(), standard.size());
  
  for (size_t i = 0; i < 100; ++i){
    table.erase(i);
    standard.erase(i);
  }
  ASSERT_EQ(table.size(), standard.size());
}

TEST(basic_func_test, is_empty){
  CuckooHash table;
  std::unordered_set<int> standard;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), standard.size());
  ASSERT_EQ(table.capacity(), 26);
}

// Confirm hashing stays within bucket boundaries i.e < capacity
TEST(basic_func_test, hash_below_capacity){
  CuckooHash table;

  int large_number = INT_MAX / 2;

  size_t idx_h1 = table.get_hash_1(large_number);
  size_t idx_h2 = table.get_hash_2(large_number);

  ASSERT_TRUE(idx_h1 < table.capacity() / 2);
  ASSERT_TRUE(idx_h2 < table.capacity() / 2);
  ASSERT_NE(idx_h1, idx_h2);
}

TEST(insert_test, size_increment_works){
  CuckooHash table;
  std::unordered_set<int> standard;

  for (size_t i = 0; i < 100; ++i){
    table.insert(i);
    standard.insert(i);  
  }

  ASSERT_EQ(table.size(), standard.size());
}

// Insert 2 Elements where they clash
TEST(insert_test, insert_2_clashing_elements){
  CuckooHash table;

  table.insert(1);
  table.insert(14);
  ASSERT_EQ(table.size(), 2);
  ASSERT_FALSE(table.empty());

  ASSERT_EQ(table.contains(1), 2);
  ASSERT_EQ(table.contains(14), 1);
  
  size_t idx_h1_x = table.get_hash_1(1);
  size_t idx_h1_y = table.get_hash_1(14);

  size_t idx_h2_x = table.get_hash_2(1);
  size_t idx_h2_y = table.get_hash_2(14);

  //Hashed values should be equal using first hash method
  ASSERT_EQ(idx_h1_x, idx_h1_y);

  ASSERT_EQ(table.h1_bucket()[idx_h1_x], 14);
  ASSERT_EQ(table.h2_bucket()[idx_h2_y], 1);

  ASSERT_EQ(table.find(14).value(), 14);
  ASSERT_EQ(table.find(1).value(), 1);
}

//Duplicate elements cannot be present in the structure.
TEST(insert_test, no_duplicate_elements){
  CuckooHash table;

  table.insert(12);

  ASSERT_EQ(table.contains(12), 1);
  ASSERT_EQ(table.size(), 1);
  table.insert(12);
  
  ASSERT_EQ(table.contains(12), 1);
  ASSERT_EQ(table.size(), 1);
}

TEST(insert_test, exceeding_max_steps_rehashes){
  CuckooHash table;

  std::vector<int> values{1, 14, 27, 41, 54, 61, 81, 88};

  ASSERT_EQ(table.capacity(), 26);

  for(size_t i = 0; i < values.size(); ++i){
    table.insert(values[i]);
  }

  //Previous load factor prior to rehash does not exceed the threshold
  ASSERT_LT(static_cast<float>(table.size() / 26), 0.5f);
  ASSERT_EQ(table.capacity(), 58);
  ASSERT_EQ(table.size(), 8);

  for(int v : values){
    ASSERT_TRUE(table.contains(v) == 1 || table.contains(v) == 2);
  }
}

TEST(insert_test, inserts_random_values_stress) {
    CuckooHash table;
    std::unordered_set<int> standard;

    std::unordered_set<int> values = random_set(100'000, 0, 100'000);
    for (auto x : values) {
        table.insert(x);
        standard.insert(x);
    }
   
    for (int x : values) {
        ASSERT_TRUE(table.contains(x) == 1 || table.contains(x) == 2);
        ASSERT_EQ(*standard.find(x), *table.find(x));
    }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "insert_test.exceeding_max_steps_rehashes";
  return RUN_ALL_TESTS();
}