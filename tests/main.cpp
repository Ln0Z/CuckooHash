#include "cuckoo_hash.hpp"
#include "rand_cuckoo_hash.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <limits.h>
#include <random>
#include <unordered_set>

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


// <-----------------------------------------------------------------HASH TESTS-------------------------------------------------------------->

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

// <-----------------------------------------------------------------INSERT TESTS-------------------------------------------------------------->

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

// <-----------------------------------------------------------------ERASE TESTS-------------------------------------------------------------->

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

  for(auto x : values){
    table.erase(x);
    standard.erase(x);
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

TEST(erase_test, insert_and_erase_random_values_stress) {
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

    for (int x : values) {
      table.erase(x);
      standard.erase(x);
    }

    ASSERT_EQ(standard.size(), table.size());
}

// <-----------------------------------------------------------------BASIC FUNCTIONALITY TESTS-------------------------------------------------------------->

TEST(basic_func_test, load_factor_calc){
  std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), static_cast<float>(i) / static_cast<float>(26));
    table.insert(values[i]);
  }
}

TEST(basic_func_test, load_factor_calc_after_rehash){
  std::vector<int> values{1, 34, 3, 23, 12, 38, 53, 45, 2, 11, 8, 5, 6, 43};
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), static_cast<float>(i) / static_cast<float>(table.capacity()));
    table.insert(values[i]);
  }
}

TEST(basic_func_test, load_factor_calc_after_multiple_rehash){
  std::unordered_set<int> values = random_set(200, 0, 500);
  CuckooHash table;

  for (size_t i = 0; i < values.size(); ++i) {
    ASSERT_EQ(table.load_factor(), static_cast<float>(i) / static_cast<float>(table.capacity()));
    table.insert(i);
  }
}

TEST(basic_func_test, is_empty){
  CuckooHash table;
  std::unordered_set<int> standard;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), standard.size());
  ASSERT_EQ(table.capacity(), 26);
}

TEST(basic_func_test, handle_0_insertion){
  CuckooHash table;
  std::unordered_set<int> standard;

  ASSERT_EQ(table.contains(0) == 1 || table.contains(0) == 2, standard.contains(0));

  table.insert(0);
  standard.insert(0);

  ASSERT_EQ(*table.find(0), *standard.find(0));
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

// <----------------------------------------------------------RAND BASIC FUNCTIONALITY TESTS-------------------------------------------------------------->

TEST(rand_cuckoo_tests, basic_functionality_test) {
    std::vector<int> values{1, 34, -1, -5, 12, 39, -124, 2147483647, 2, 11, 2345, 341, 456, -123, -213, -3423, -23, 1343, 5676, 6755646, 2343243, 675465233, 3235436, 342353, 3435434, 23542, 223434565, 2132114412};
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

TEST(rand_cuckoo_tests, hash_below_capacity) {
    RandCuckooHash table;

    int large_number = INT_MAX / 2;

    size_t idx_h1 = table.get_hash_1(large_number);
    size_t idx_h2 = table.get_hash_2(large_number);

    ASSERT_TRUE(idx_h1 < table.capacity());
    ASSERT_TRUE(idx_h2 < table.capacity());
    ASSERT_NE(idx_h1, idx_h2);
}

TEST(rand_cuckoo_tests, insert_single_element) {
    RandCuckooHash table;

    ASSERT_EQ(table.contains(4), -1);

    table.insert(4);
    ASSERT_EQ(table.contains(4), 1);
    ASSERT_EQ(table.size(), 1);
    ASSERT_FALSE(table.empty());

    size_t idx1 = table.get_hash_1(4);
    ASSERT_EQ(table.h1_bucket()[idx1].value(), 4);
}

TEST(rand_cuckoo_tests, insert_negative_element) {
    RandCuckooHash table;

    table.insert(-12);
    ASSERT_EQ(table.contains(-12), 1);
    ASSERT_EQ(table.size(), 1);
    ASSERT_FALSE(table.empty());
    size_t idx1 = table.get_hash_1(-12);
    ASSERT_EQ(table.h1_bucket()[idx1].value(), -12);
}

TEST(rand_cuckoo_tests, no_duplicate_elements) {
    RandCuckooHash table;

    table.insert(12);

    ASSERT_EQ(table.contains(12), 1);
    ASSERT_EQ(table.size(), 1);
    table.insert(12);

    ASSERT_EQ(table.contains(12), 1);
    ASSERT_EQ(table.size(), 1);
}

// <-----------------------------------------------------------------UHF PROPERTIES TESTS-------------------------------------------------------------->

TEST(universal_hash_family, test_single_hash_collision_rate) {
    // make table with capacity 1109, random hashes chosen
    RandCuckooHash table(6, true);

    // arbitrary seed chosen for reproducibility. 99.7% of seeds should pass the test
    std::mt19937 gen(1388230758);// NOLINT(cert-msc51-cpp)
    std::uniform_int_distribution<int32_t> int32_range(-2'147'483'647, 2'147'483'647);

    int hash1_collisions = 0;
    int hash2_collisions = 0;

    int num_of_runs = 1000000;

    // count number of collisions for a million different key pairs for each hash
    for (int i = 0; i < num_of_runs; i++) {
        int key1 = int32_range(gen);
        int key2 = int32_range(gen);
        if (table.hash_1(key1) == table.hash_1(key2)) hash1_collisions++;
        if (table.hash_2(key1) == table.hash_2(key2)) hash2_collisions++;
    }

    // expect average collision rate to be about 1 / m
    // divide by two because table.capacity is for both arrays
    // but we just want collision rate for one
    float expected = 1 / ((float) table.capacity() / 2);
    float hash_1_collision_rate = (float) hash1_collisions / (float) num_of_runs;
    float hash_2_collision_rate = (float) hash2_collisions / (float) num_of_runs;

    const double standard_error = std::sqrt(expected * (1 - expected) / (float) num_of_runs);
    const double acceptable_range = standard_error * 3;
    // ^ uses z-score of 3, so this test should have a 99.7% chance of passing

    std::cout << "\n\nRandom hash collision rate test:\n\nExpected collision rate: " << expected << std::endl;
    std::cout << "h1 collision rate: " << hash_1_collision_rate << ", h2 collision rate: " << hash_2_collision_rate << std::endl;

    // expect collision rate to be = 1/m on average, account for probability
    EXPECT_NEAR(expected, hash_1_collision_rate, acceptable_range);
    EXPECT_NEAR(expected, hash_2_collision_rate, acceptable_range);
}

TEST(universal_hash_family, test_family_universality) {
    // pick a few arbitrary fixed pairs to test across many hashes
    int32_t x1 = -685415863, y1 = 815978995;
    int32_t x2 = 145536895, y2 = -1307052590;
    int32_t x3 = 2013660739, y3 = 902458331;

    int pair1_collisions = 0;
    int pair2_collisions = 0;
    int pair3_collisions = 0;

    int num_of_runs = 1000000;
    // for each pair, count number of collisions after a
    // million different hashes from the universal set

    // init new table, hashes are randomly chosen
    // make table capacity 1109, use arbitrary seed
    RandCuckooHash table(6, 1388210758, true);
    for (int i = 0; i < num_of_runs; i++) {

        if (table.hash_1(x1) == table.hash_1(y1)) pair1_collisions++;
        if (table.hash_1(x2) == table.hash_1(y2)) pair2_collisions++;
        if (table.hash_1(x3) == table.hash_1(y3)) pair3_collisions++;

        // use helper function to avoid
        table.genNewHashes();
    }

    // just like last test, expect average collision rate to be about 1 / m
    float expected = 1 / ((float) table.capacity() / 2);
    float pair_1_collision_rate = (float) pair1_collisions / (float) num_of_runs;
    float pair_2_collision_rate = (float) pair2_collisions / (float) num_of_runs;
    float pair_3_collision_rate = (float) pair3_collisions / (float) num_of_runs;

    const double standard_error = std::sqrt(expected * (1 - expected) / (float) num_of_runs);
    const double acceptable_range = standard_error * 3;

    std::cout << "\n\nHash family universality test:\n\nExpected collision rate: " << expected << std::endl;

    std::cout << "Pair 1 collision rate: " << pair_1_collision_rate << std::endl;
    std::cout << "Pair 2 collision rate: " << pair_2_collision_rate << std::endl;
    std::cout << "Pair 3 collision rate: " << pair_3_collision_rate << std::endl;

    EXPECT_NEAR(expected, pair_1_collision_rate, acceptable_range);
    EXPECT_NEAR(expected, pair_2_collision_rate, acceptable_range);
    EXPECT_NEAR(expected, pair_3_collision_rate, acceptable_range);
}

// <-----------------------------------------------------------------DETERMINISTIC AND UNIVERSAL HASH COMPARISON TESTS-------------------------------------------------------------->

TEST(compare_deterministic_and_universal, random_keys) {
    // create a deterministic and random cuckoo hash table
    CuckooHash table(6);
    RandCuckooHash rand_table(6, 1388210758, true);

    // arbitrary seed chosen for reproducibility. 99.7% of seeds should pass the test
    std::mt19937 gen(1388230758);// NOLINT(cert-msc51-cpp)
    std::uniform_int_distribution<int32_t> int32_range(-2'147'483'647, 2'147'483'647);

    int deterministic_hash_collisions = 0;
    int random_hash_collisions = 0;

    int num_of_runs = 1000000;

    // count number of collisions for a million different key pairs for each hash
    for (int i = 0; i < num_of_runs; i++) {
        int key1 = int32_range(gen);
        int key2 = int32_range(gen);
        if (table.hash_1(key1) == table.hash_1(key2)) deterministic_hash_collisions++;
        if (rand_table.hash_1(key1) == rand_table.hash_1(key2)) random_hash_collisions++;
    }

    float deterministic_collision_rate = (float) deterministic_hash_collisions / (float) num_of_runs;
    float random_collision_rate = (float) random_hash_collisions / (float) num_of_runs;

    const double standard_error = std::sqrt(deterministic_collision_rate * (1 - deterministic_collision_rate) / (float) num_of_runs);
    const double acceptable_range = standard_error * 3;

    std::cout << "\n\nCompare deterministic and universal hash collision rates:\n"
              << std::endl;
    std::cout << "Deterministic collision rate: " << deterministic_collision_rate << ", Universal collision rate: " << random_collision_rate << std::endl;

    // expect collision rate to be effectively the same for both implementations, explanation in report
    EXPECT_NEAR(deterministic_collision_rate, random_collision_rate, acceptable_range);
}


TEST(compare_deterministic_and_universal, adversarial_keys) {
    // pick a key to base adversarial set on
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int32_t> int32_range(-2'147'483'647, 2'147'483'647);
    int32_t key = int32_range(gen);

    // create a deterministic and random cuckoo hash table
    CuckooHash table(0);
    RandCuckooHash rand_table(0, 1388210758, true);

    int num_of_runs = 25;

    // insert a sequence
    for (int i = 0; i < num_of_runs; i++) {
        // increase key by capacity of one array - adversely
        // affects deterministic hashes, but not universally hashed ones
        key += (int) table.capacity() / 2;
        table.insert(key);
        rand_table.insert(key);
    }

    // expect that deterministic table has had to rehash 12 times
    // while non-deterministic only has had to once to increase capacity
    EXPECT_EQ(table.times_rehashed(), 12);
    EXPECT_EQ(rand_table.times_rehashed(), 1);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}