#include <gtest/gtest.h>
#include "cuckoo_hash.hpp"
#include "rand_cuckoo_hash.hpp"

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

  ASSERT_EQ(table.return_h1().size(), 13);
  ASSERT_EQ(table.return_h2().size(), 13);

    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.return_h1()[i], h1_results[i]);
        ASSERT_EQ(table.return_h2()[i], h2_results[i]);
    }
}
TEST(rand_insert_test, insert_10_elements) {
  std::vector<int> values{1, 34, -1, -5, 12, 39, -124, 2147483647, 2, 11, 2345 ,341 ,456, -123, -213, -3423, -23, 1343, 5676, 6755646, 2343243, 675465233, 3235436, 342353, 3435434, 23542, 223434565, 2132114412};
  RandCuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    table.insert(values[i]);
    ASSERT_EQ(table.size(), i + 1);
    ASSERT_TRUE(table.contains(values[i]) == 1 || table.contains(values[i]) == 2);
    ASSERT_EQ(table.contains(2315), -1);
  }

  ASSERT_FALSE(table.erase(2315));

  ASSERT_TRUE(table.erase(39));
  EXPECT_EQ(table.contains(39), -1);
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


  ASSERT_EQ(table.return_h1().size(), 13);
  ASSERT_EQ(table.return_h2().size(), 13);
  ASSERT_EQ(table.capacity(), 13);


  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.return_h1()[i], h1_results[i]);
    ASSERT_EQ(table.return_h2()[i], h2_results[i]);
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

  ASSERT_EQ(table.return_h1().size(), 13);
  ASSERT_EQ(table.return_h2().size(), 13);


  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.return_h1()[i], h1_results[i]);
    ASSERT_EQ(table.return_h2()[i], h2_results[i]);
  }

  for (size_t i = 0; i < values.size(); ++i) {
    table.erase(values[i]);
  }

  for (size_t i = 0; i < h1_results.size(); ++i) {
    ASSERT_EQ(table.return_h1()[i], std::nullopt);
    ASSERT_EQ(table.return_h2()[i], std::nullopt);
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
//    ::testing::GTEST_FLAG(filter) = "basic_rehash_test.*";
  return RUN_ALL_TESTS();
}