#include <gtest/gtest.h>
#include <random>
#include "cuckoo_hash.hpp"
#include <limits.h>

namespace{
    std::vector<int> random_set(int total_numbers){
        std::random_device rand;
        std::uniform_int_distribution<int> seed_dist(1, 50);
        int seed = seed_dist(rand);
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(1, 100);
        std::vector<int> random_set;

        for (size_t i = 0; i < total_numbers; ++i){
            random_set.push_back(dist(rng));
        }
        return random_set;
    }
}

TEST(basic_insert_test, insert_10_elements) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
  }

    std::vector<std::optional<int>> h1_results{std::nullopt, 39, 1, 23, 12, 34, std::nullopt, 11, std::nullopt, 2, 
                                             std::nullopt, 53, std::nullopt};

  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, 3, std::nullopt, std::nullopt, 
                                             std::nullopt, 45, std::nullopt};


  ASSERT_EQ(table.h1_bucket().size(), 13);
  ASSERT_EQ(table.h2_bucket().size(), 13);

    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
        ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
    }
}

TEST(basic_rehash_test, insert_cause_rehash) {
  std::vector<int> values{1, 34, 3, 23, 12, 38, 53, 45, 2, 11, 8, 5, 6, 43};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
  }

  std::vector<std::optional<int>> h1_results(29, std::nullopt);
  std::vector<std::optional<int>> h2_results(29, std::nullopt);

  h1_results[2]  = 53;
  h1_results[4]  = 45;
  h1_results[5]  = 6;
  h1_results[6]  = 2;
  h1_results[11] = 11;
  h1_results[13] = 3;
  h1_results[19] = 8;
  h1_results[23] = 23;
  h1_results[27] = 5;
  h1_results[28] = 34;

  h2_results[7]  = 12;
  h2_results[5]  = 38;
  h2_results[10] = 1;
  h2_results[14] = 43;


  ASSERT_EQ(table.h1_bucket().size(), 29);
  ASSERT_EQ(table.h2_bucket().size(), 29);
  ASSERT_EQ(table.capacity(), 29);


  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
    ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
  }
}

TEST(basic_remove_test, insert_10_elements) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
  }
  
  std::vector<std::optional<int>> h1_results{std::nullopt, 39, 1, 23, 12, 34, std::nullopt, 11, std::nullopt, 2, 
                                             std::nullopt, 53, std::nullopt};

  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, 3, std::nullopt, std::nullopt, 
                                             std::nullopt, 45, std::nullopt};

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
  ASSERT_EQ(table.capacity(), 13);
}

// Confirm hashing stays within bucket boundaries i.e < capacity
  TEST(basic_func_test, hash_below_capacity){
    CuckooHash table;

    int large_number = INT_MAX / 2;

    size_t idx_h1 = table.get_hash_1(large_number);
    size_t idx_h2 = table.get_hash_2(large_number);

    ASSERT_TRUE(idx_h1 < table.capacity());
    ASSERT_TRUE(idx_h2 < table.capacity());
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

  table.insert(19);
  table.insert(26);
  ASSERT_EQ(table.size(), 2);
  ASSERT_FALSE(table.empty());

  ASSERT_TRUE(table.contains(19));
  ASSERT_TRUE(table.contains(26));
  
  size_t idx_h1_19 = table.get_hash_1(19);
  size_t idx_h1_26 = table.get_hash_1(26);

  size_t idx_h2_19 = table.get_hash_2(19);
  size_t idx_h2_26 = table.get_hash_2(26);

  //Hashed values should be equal using first hash method
  ASSERT_EQ(idx_h1_19, idx_h1_26);
  ASSERT_NE(idx_h2_19, idx_h2_26);

  ASSERT_EQ(table.h1_bucket()[idx_h1_26], 26);
  ASSERT_EQ(table.h2_bucket()[idx_h2_19], 19);
}

//Duplicate elements cannot be present in the structure.
// TEST(insert_test, no_duplicate_elements){
//   CuckooHash table;

//   table.insert(12);

//   ASSERT_TRUE(table.contains(12));
//   ASSERT_EQ(table.size(), 1);
//   ASSERT_

  
// }

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  //::testing::GTEST_FLAG(filter) = "basic_rehash_test.*";
  return RUN_ALL_TESTS();
}