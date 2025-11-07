#include <gtest/gtest.h>
#include <random>
#include "cuckoo_hash.hpp"
#include <limits.h>;

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

  std::vector<std::optional<int>> h1_results{39, 2, std::nullopt, 45, 34, 23, 12, 53, 3, std::nullopt, std::nullopt,
                                             std::nullopt, 11};
  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 1,
                                             std::nullopt, std::nullopt};

  ASSERT_EQ(table.h1_bucket().size(), 13);
  ASSERT_EQ(table.h2_bucket().size(), 13);

    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
        ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
    }
}

TEST(basic_rehash_test, insert_cause_rehash) {
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
  }
  std::vector<std::optional<int>> h1_results{39, 2, std::nullopt, 45, 34, 23, 12, 53, 3, std::nullopt, std::nullopt,
                                             std::nullopt, 11};
  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 1,
                                             std::nullopt, std::nullopt};


  ASSERT_EQ(table.h1_bucket().size(), 13);
  ASSERT_EQ(table.h2_bucket().size(), 13);
  ASSERT_EQ(table.capacity(), 13);


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

  std::vector<std::optional<int>> h1_results{39, 2, std::nullopt, 45, 34, 23, 12, 53, 3, std::nullopt, std::nullopt,
                                             std::nullopt, 11};
  std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                                             std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 1,
                                             std::nullopt, std::nullopt};

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
  
  ASSERT_FALSE(table.contains(4));

  table.insert(4);
  ASSERT_TRUE(table.contains(4));
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.empty());

  size_t idx1 = table.get_hash_1(4);
  ASSERT_EQ(table.h1_bucket()[idx1].value(), 4);
}

// Insert negative elements
TEST(insert_test, insert_negative_element){
  CuckooHash table;

  table.insert(-12);
  ASSERT_TRUE(table.contains(-12));
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.empty());
  size_t idx1 = table.get_hash_1(-12);
  ASSERT_EQ(table.h1_bucket()[idx1].value(), -12);
}

// Insert 2 Elements where they clash
TEST(insert_test, insert_2_clashing_elements){
  CuckooHash table;

  ASSERT_TRUE(table.empty());

  table.insert(1);
  table.insert(14);
  ASSERT_EQ(table.size(), 2);
  ASSERT_FALSE(table.empty());

  ASSERT_TRUE(table.contains(1));
  ASSERT_TRUE(table.contains(13));
  
  size_t idx_h1_1 = table.get_hash_1(1);
  size_t idx_h1_13 = table.get_hash_1(13);

  size_t idx_h2_1 = table.get_hash_2(1);
  size_t idx_h2_13 = table.get_hash_2(13);

  //Hashed values should be equal using first hash method but not in the second
  ASSERT_EQ(idx_h1_1, idx_h1_13);
  ASSERT_NE(idx_h2_1, idx_h2_13);

  ASSERT_EQ(table.h1_bucket()[idx_h1_1], 1);
  ASSERT_EQ(table.h2_bucket()[idx_h2_13], 13);
  
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
//    ::testing::GTEST_FLAG(filter) = "basic_rehash_test.*";
  return RUN_ALL_TESTS();
}