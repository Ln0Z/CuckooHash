#include <gtest/gtest.h>
#include <random>
#include "cuckoo_hash.hpp"
#include <limits.h>
#include <iostream>
#include <algorithm>

namespace{
    std::vector<int> random_set(int total_numbers, int low_rage, int high_range){
        std::random_device rand;
        std::uniform_int_distribution<int> seed_dist(1, 50);
        int seed = seed_dist(rand);
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(low_rage, high_range);
        std::vector<int> random_set;

        for (size_t i = 0; i < total_numbers; ++i){
            random_set.push_back(dist(rng));
        }
        return random_set;
    }
}

TEST(basic_func_test, load_factor_calc){
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), (float)i/26);
    table.insert(values[i]);
  }
}

TEST(insert_test, insert_10_elements) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
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
}

TEST(basic_func_test, insert_cause_rehash) {
  std::vector<int> values{1, 34, 3, 23, 12, 38, 53, 45, 2, 11, 8, 5, 6, 43};
  CuckooHash table;

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
  ASSERT_TRUE(table.empty());

  table.insert(15);

  table.erase(15);

  ASSERT_EQ(table.contains(15), -1);
  ASSERT_TRUE(table.empty());
  ASSERT_FALSE(table.erase(15));
}

TEST(erase_test, insert_and_erase_10_elements) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
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

  for (size_t i = 0; i < values.size(); ++i) {
    table.erase(values[i]);
  }

  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.h1_bucket()[i], std::nullopt);
    ASSERT_EQ(table.h2_bucket()[i], std::nullopt);
  }
}

// Tests for cuckoo hash table,

TEST(basic_func_test, is_empty){
  CuckooHash table;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
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

// Insert 1 element
TEST(insert_test, insert_single_element){
  CuckooHash table;
  
  ASSERT_EQ(table.contains(4), -1);

  table.insert(4);
  ASSERT_EQ(table.contains(4), 1);
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.empty());

  size_t idx1 = table.get_hash_1(4);
  ASSERT_EQ(table.h1_bucket()[idx1].value(), 4);
}

// Insert negative elements
TEST(insert_test, insert_negative_element){
  CuckooHash table;

  table.insert(-12);
  ASSERT_EQ(table.contains(-12), 1);
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.empty());
  size_t idx1 = table.get_hash_1(-12);
  ASSERT_EQ(table.h1_bucket()[idx1].value(), -12);
}

// Insert 2 Elements where they clash
TEST(insert_test, insert_2_clashing_elements){
  CuckooHash table;

  table.insert(1);
  table.insert(14);
  ASSERT_EQ(table.size(), 2);
  ASSERT_FALSE(table.empty());

  ASSERT_TRUE(table.contains(1));
  ASSERT_TRUE(table.contains(14));
  
  size_t idx_h1_x = table.get_hash_1(1);
  size_t idx_h1_y = table.get_hash_1(14);

  size_t idx_h2_x = table.get_hash_2(1);
  size_t idx_h2_y = table.get_hash_2(14);

  //Hashed values should be equal using first hash method
  ASSERT_EQ(idx_h1_x, idx_h1_y);

  ASSERT_EQ(table.h1_bucket()[idx_h1_x], 14);
  ASSERT_EQ(table.h2_bucket()[idx_h2_y], 1);
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

  for(size_t i = 0; i < values.size(); ++i){
    table.insert(values[i]);
  }

  ASSERT_EQ(table.capacity(), 58);
  ASSERT_EQ(table.size(), 8);

  for(int v : values){
    ASSERT_TRUE(table.contains(v) == 1 || table.contains(v) == 2);
  }
}

TEST(insert_test, inserts_random_values) {
    CuckooHash table;
    std::vector<int> values = random_set(100'000, 0, 100'000);
    for (int v : values) {
        table.insert(v);
    }
   
    for (int v : values) {
        ASSERT_TRUE(table.contains(v) == 1 || table.contains(v) == 2);
    }

    ASSERT_GE(table.capacity() / 2, 100);
}



int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "insert_test.exceeding_max_steps_rehashes";
  return RUN_ALL_TESTS();
}