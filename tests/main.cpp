#include <gtest/gtest.h>
#include "cuckoo_hash.hpp"

TEST(basic_insert_test, insert_10_elements){
    std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
    CuckooHash table;

    for (size_t i = 0; i < values.size(); ++i){
        table.insert(values[i]);
    }

    std::vector<std::optional<int>> h1_results{std::nullopt, 39, std::nullopt, 45, 12, 11, 2, 1};
    std::vector<std::optional<int>> h2_results{23, std::nullopt, std::nullopt, std::nullopt, 3, std::nullopt, 53, 34};

    ASSERT_EQ(table.return_h1().size(), 8);
    ASSERT_EQ(table.return_h2().size(), 8);
    
    
    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.return_h1()[i], h1_results[i]);
        ASSERT_EQ(table.return_h2()[i], h2_results[i]);
    }
}

TEST(basic_rehash_test, insert_cause_rehash){
    std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
    CuckooHash table;

    for (size_t i = 0; i < values.size(); ++i){
        table.insert(values[i]);
    }

    std::vector<std::optional<int>> h1_results{std::nullopt, 39, std::nullopt, 53, 12, 3, 
        std::nullopt, 1, std::nullopt, std::nullopt, std::nullopt, 45, std::nullopt, 11, 34, std::nullopt};
        
    std::vector<std::optional<int>> h2_results{std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 
        std::nullopt, std::nullopt, 23, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, 2};
        

    ASSERT_EQ(table.return_h1().size(), 16);
    ASSERT_EQ(table.return_h2().size(), 16);
    ASSERT_EQ(table.capacity(), 16);
    
    
    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.return_h1()[i], h1_results[i]);
        ASSERT_EQ(table.return_h2()[i], h2_results[i]);
    }
}

TEST(basic_remove_test, insert_10_elements){
    std::vector<int> values{1, 34, 3, 23, 12, 39, 53, 45, 2, 11};
    CuckooHash table;

    for (size_t i = 0; i < values.size(); ++i){
        table.insert(values[i]);
    }

    std::vector<std::optional<int>> h1_results{std::nullopt, 39, std::nullopt, 45, 12, 11, 2, 1};
    std::vector<std::optional<int>> h2_results{23, std::nullopt, std::nullopt, std::nullopt, 3, std::nullopt, 53, 34};

    ASSERT_EQ(table.return_h1().size(), 8);
    ASSERT_EQ(table.return_h2().size(), 8);
    
    
    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.return_h1()[i], h1_results[i]);
        ASSERT_EQ(table.return_h2()[i], h2_results[i]);
    }

    for (size_t i = 0; i < values.size(); ++i){
        table.erase(values[i]);
    }

    for (size_t i = 0; i < h1_results.size(); ++i){
        ASSERT_EQ(table.return_h1()[i], std::nullopt);
        ASSERT_EQ(table.return_h2()[i], std::nullopt);
    }
}

int main(int argc, char* argv[]){
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::GTEST_FLAG(filter) = "basic_rehash_test.*";
    return RUN_ALL_TESTS();
}