#include "cuckoo_hash.hpp"
#include "rand_cuckoo_hash.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <limits.h>
#include <random>

namespace {
    std::vector<int> random_set(int total_numbers) {
        std::random_device rand;
        std::uniform_int_distribution<int> seed_dist(1, 50);
        int seed = seed_dist(rand);
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(1, 100);
        std::vector<int> random_set;

        for (size_t i = 0; i < total_numbers; ++i) {
            random_set.push_back(dist(rng));
        }
        return random_set;
    }
}// namespace

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

    for (size_t i = 0; i < h1_results.size(); ++i) {
        ASSERT_EQ(table.h1_bucket()[i], h1_results[i]);
        ASSERT_EQ(table.h2_bucket()[i], h2_results[i]);
    }
}

TEST(basic_rehash_test, insert_cause_rehash) {
    std::vector<int> values{1, 34, 3, 23, 12, 38, 53, 45, 2, 11, 8, 5, 6, 43};
    CuckooHash table;

    for (size_t i = 0; i < values.size(); ++i) {
        table.insert(values[i]);
        if (i == 12) {
            //Next insert will trigger rehash
            ASSERT_EQ(table.load_factor(), 0.5f);
        }
    }

    ASSERT_EQ(table.capacity(), 29);

    std::vector<std::optional<int>> h1_results(29, std::nullopt);
    std::vector<std::optional<int>> h2_results(29, std::nullopt);

    h1_results[2] = 53;
    h1_results[4] = 45;
    h1_results[5] = 6;
    h1_results[6] = 2;
    h1_results[11] = 11;
    h1_results[13] = 3;
    h1_results[19] = 8;
    h1_results[23] = 23;
    h1_results[27] = 5;
    h1_results[28] = 34;

    h2_results[7] = 12;
    h2_results[5] = 38;
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

TEST(basic_func_test, is_empty) {
    CuckooHash table;

    ASSERT_TRUE(table.empty());
    ASSERT_EQ(table.size(), 0);
    ASSERT_EQ(table.capacity(), 13);
}

// Confirm hashing stays within bucket boundaries i.e < capacity
TEST(basic_func_test, hash_below_capacity) {
    CuckooHash table;

    int large_number = INT_MAX / 2;

    size_t idx_h1 = table.get_hash_1(large_number);
    size_t idx_h2 = table.get_hash_2(large_number);

    ASSERT_TRUE(idx_h1 < table.capacity());
    ASSERT_TRUE(idx_h2 < table.capacity());
    ASSERT_NE(idx_h1, idx_h2);
}

// Insert 1 element

TEST(insert_test, insert_single_element) {
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
TEST(insert_test, insert_negative_element) {
    CuckooHash table;

    table.insert(-12);
    ASSERT_EQ(table.contains(-12), 1);
    ASSERT_EQ(table.size(), 1);
    ASSERT_FALSE(table.empty());
    size_t idx1 = table.get_hash_1(-12);
    ASSERT_EQ(table.h1_bucket()[idx1].value(), -12);
}

// Insert 2 Elements where they clash
TEST(insert_test, insert_2_clashing_elements) {
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

    //Hashed values should be equal using first hash method but different with the second hash method
    ASSERT_EQ(idx_h1_19, idx_h1_26);
    ASSERT_NE(idx_h2_19, idx_h2_26);

    ASSERT_EQ(table.h1_bucket()[idx_h1_26], 26);
    ASSERT_EQ(table.h2_bucket()[idx_h2_19], 19);
}

//Duplicate elements cannot be present in the structure.
TEST(insert_test, no_duplicate_elements) {
    CuckooHash table;

    table.insert(12);

    ASSERT_EQ(table.contains(12), 1);
    ASSERT_EQ(table.size(), 1);
    table.insert(12);

    ASSERT_EQ(table.contains(12), 1);
    ASSERT_EQ(table.size(), 1);
}

TEST(insert_test, exceeding_max_steps_rehashes) {
    CuckooHash table;

    std::vector<int> values{7, 14, 34, 41, 54, 61, 81, 88};

    for (size_t i = 0; i < values.size(); ++i) {
        table.insert(values[i]);
    }

    ASSERT_EQ(table.capacity(), 29);
    ASSERT_EQ(table.size(), 8);

    for (int v: values) {
        ASSERT_TRUE(table.contains(v) == 1 || table.contains(v) == 2);
    }
}

TEST(rand_insert_test, insert_10_elements) {
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

// Universal hash family tests

TEST(universal_hash_family, test_single_hash_collision_rate) {
    // make table with capacity 1109, random hashes chosen
    RandCuckooHash table(6);

    std::mt19937 gen(std::random_device{}());
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
    float expected = 1 / (float) table.capacity();
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

    std::mt19937 gen(std::random_device{}());

    int num_of_runs = 1000000;
    // for each pair, count number of collisions after a
    // million different hashes from the universal set

    // init new table, hashes are randomly chosen
    // make table capacity 1109
    RandCuckooHash table(6);
    for (int i = 0; i < num_of_runs; i++) {

        if (table.hash_1(x1) == table.hash_1(y1)) pair1_collisions++;
        if (table.hash_1(x2) == table.hash_1(y2)) pair2_collisions++;
        if (table.hash_1(x3) == table.hash_1(y3)) pair3_collisions++;

        // use helper function to avoid
        table.genNewHashes();
    }

    // just like last test, expect average collision rate to be about 1 / m
    float expected = 1 / (float) table.capacity();
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


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    //  ::testing::GTEST_FLAG(filter) = "insert_test.exceeding_max_steps_rehashes";
    return RUN_ALL_TESTS();
}