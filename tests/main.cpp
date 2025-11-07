#include <gtest/gtest.h>
#include <random>
#include "cuckoo_hash.hpp"

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


int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
//    ::testing::GTEST_FLAG(filter) = "basic_rehash_test.*";
  return RUN_ALL_TESTS();
}