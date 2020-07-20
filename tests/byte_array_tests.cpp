// Includes from data_structures module
#include <data_structures/byte_array.h>

// Third party includes
#include <gtest/gtest.h>

TEST(ByteArrayTests, constructor_from_size)
{
    ByteArray ba{10};
    EXPECT_EQ(ba.get_size(), 10);
}

TEST(ByteArrayTests, constructor_from_pointer_size)
{
    int raw_data[15] {
         1,  2,  3,  4,  5,
         6,  7,  8,  9, 10,
        11, 12, 13, 14, 15,
    };
    ByteArray ba{reinterpret_cast<uint8_t*>(raw_data), 15 * sizeof(int)};

    EXPECT_EQ(ba.get_size(), 15 * sizeof(int));


    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 0 * sizeof(int)), 1);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 1 * sizeof(int)), 2);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 2 * sizeof(int)), 3);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 3 * sizeof(int)), 4);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 4 * sizeof(int)), 5);

    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 5 * sizeof(int)), 6);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 6 * sizeof(int)), 7);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 7 * sizeof(int)), 8);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 8 * sizeof(int)), 9);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 9 * sizeof(int)), 10);

    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 10 * sizeof(int)), 11);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 11 * sizeof(int)), 12);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 12 * sizeof(int)), 13);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 13 * sizeof(int)), 14);
    EXPECT_EQ(*reinterpret_cast<int*>(ba.get_data() + 14 * sizeof(int)), 15);
}