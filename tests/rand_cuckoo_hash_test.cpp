#include "cuckoo_hash.hpp"
#include "rand_cuckoo_hash.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <limits.h>
#include <random>

// RandCuckooHash table tests

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

// Universal hash family tests

TEST(universal_hash_family, test_single_hash_collision_rate) {
    // make table with capacity 1109, random hashes chosen
    RandCuckooHash table(6, true);

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
    RandCuckooHash table(6, true);
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