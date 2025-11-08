#include "cuckoo_hash.hpp"
#include "rand_cuckoo_hash.hpp"
#include <gtest/gtest.h>

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

    for (size_t i = 0; i < h1_results.size(); ++i) {
        ASSERT_EQ(table.return_h1()[i], h1_results[i]);
        ASSERT_EQ(table.return_h2()[i], h2_results[i]);
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

// Universal hash family tests

TEST(universal_hash_family, test_single_hash_collision_rate) {
    // make table with capacity 1109, random hashes chosen
    RandCuckooHash table(6);

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int32_t> int32_range(-2'147'483'647, 2'147'483'647);

    int hash1_collisions = 0;
    int hash2_collisions = 0;

    int num_of_trials = 1000000;

    // count number of collisions for a million different key pairs for each hash
    for (int i = 0; i < num_of_trials; i++) {
        int key1 = int32_range(gen);
        int key2 = int32_range(gen);
        if (table.hash_1(key1) == table.hash_1(key2)) hash1_collisions++;
        if (table.hash_2(key1) == table.hash_2(key2)) hash2_collisions++;
    }

    // expect average collision rate to be about 1 / m
    float expected = 1 / (float) table.capacity();
    float hash_1_collision_rate = (float) hash1_collisions / (float) num_of_trials;
    float hash_2_collision_rate = (float) hash2_collisions / (float) num_of_trials;

    const double standard_error = std::sqrt(expected * (1 - expected) / (float) num_of_trials);
    const double acceptable_range = standard_error * 3;
    // ^ uses z-score of 3, so this test should have a 99.7% chance of passing

    std::cout << "Expected collision rate: " << expected << std::endl;
    std::cout << "h1 collision rate: " << hash_1_collision_rate << ", h2 collision rate: " << hash_2_collision_rate;

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

    std::cout << "Running hash family universality test..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_of_runs; i++) {
        // init new table, new hashes are randomly chosen
        // make table capacity 1109
        RandCuckooHash table(6, gen);

        if (table.hash_1(x1) == table.hash_1(y1)) pair1_collisions++;
        if (table.hash_1(x2) == table.hash_1(y2)) pair2_collisions++;
        if (table.hash_1(x3) == table.hash_1(y3)) pair3_collisions++;
//        table.reh
        // generator is copied not referenced by the constructor, so manually
        // advance generator by 4 since it is used 4 times in constructor
        gen.discard(4);

        // cout progress every 5%
        if (i % (num_of_runs / 20) == 0) std::cout << i / (num_of_runs / 100) << "% completed..." << std::endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::cout << "done in " << milliseconds << "ms \n";

    std::cout << pair1_collisions << std::endl;
    std::cout << pair2_collisions << std::endl;
    std::cout << pair3_collisions << std::endl;
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    //    ::testing::GTEST_FLAG(filter) = "basic_rehash_test.*";
    return RUN_ALL_TESTS();
}