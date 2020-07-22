#include <fourier/matrix.h>
#include <gtest/gtest.h>

template <typename T>
bool eps_equal(const T& a, const T& b, const T& eps)
{
	T diff = a - b; // Simple epsilon equals
	if (-eps <= diff &&  eps >= diff)
	{
		return true;
	}
	return false;
}

TEST(MatrixTests, get_n_rows)
{
	Matrix<int> matrix1{ 1, 2 };
	EXPECT_EQ(matrix1.n_rows(), 1);

	Matrix<float> matrix2{ 2, 3 };
	EXPECT_EQ(matrix2.n_rows(), 2);

	Matrix<std::complex<int>> matrix3{ 20, 2 };
	EXPECT_EQ(matrix3.n_rows(), 20);
}

TEST(MatrixTests, get_n_cols)
{
	Matrix<int> matrix1{2, 1};
	EXPECT_EQ(matrix1.n_cols(), 1);

	Matrix<float> matrix2{ 1, 10 };
	EXPECT_EQ(matrix2.n_cols(), 10);

	Matrix<std::complex<float>> matrix3{ 1, 64 };
	EXPECT_EQ(matrix3.n_cols(), 64);
}

TEST(MatrixTests, gets_valid_rows)
{
	Matrix<std::complex<int>> matrix1{ 2, 2 };
	EXPECT_NO_THROW(matrix1[0]);
	EXPECT_NO_THROW(matrix1[1]);


	Matrix<float> matrix2{ 7, 10 };
	EXPECT_NO_THROW(matrix2[0]);
	EXPECT_NO_THROW(matrix2[1]);
	EXPECT_NO_THROW(matrix2[2]);
	EXPECT_NO_THROW(matrix2[3]);
	EXPECT_NO_THROW(matrix2[4]);
	EXPECT_NO_THROW(matrix2[5]);
	EXPECT_NO_THROW(matrix2[6]);
}

TEST(MatrixTests, fails_invalid_rows)
{
	Matrix<float> matrix1{ 5, 1 };
	EXPECT_THROW(matrix1[5], MatrixException);
	EXPECT_THROW(matrix1[6], MatrixException);

	Matrix<double> matrix2{ 18, 20 };
	double test;
	EXPECT_THROW(matrix2[18], MatrixException);
	EXPECT_THROW(test = matrix2[19][0], MatrixException);
}

TEST(MatrixTests, creates_2_by_2_zero_int)
{
	Matrix<int> matrix{ 2, 2 };
	EXPECT_EQ(0, matrix[0][0]);
	EXPECT_EQ(0, matrix[0][1]);
	EXPECT_EQ(0, matrix[1][0]);
	EXPECT_EQ(0, matrix[1][1]);
}

TEST(MatrixTests, creates_3_by_3_zero_int)
{
	Matrix<int> matrix{ 3, 3 };
	EXPECT_EQ(0, matrix[0][0]);
	EXPECT_EQ(0, matrix[0][1]);
	EXPECT_EQ(0, matrix[0][2]);

	EXPECT_EQ(0, matrix[1][0]);
	EXPECT_EQ(0, matrix[1][1]);
	EXPECT_EQ(0, matrix[1][2]);

	EXPECT_EQ(0, matrix[2][0]);
	EXPECT_EQ(0, matrix[2][1]);
	EXPECT_EQ(0, matrix[2][2]);
}

TEST(MatrixTests, creates_2_by_2_zero_double)
{
	Matrix<double> matrix{ 2, 2 };
    EXPECT_EQ(eps_equal(matrix[0][0], 0.0, 0.000001), true);
	EXPECT_EQ(eps_equal(matrix[0][1], 0.0, 0.000001), true);
	EXPECT_EQ(eps_equal(matrix[1][0], 0.0, 0.000001), true);
	EXPECT_EQ(eps_equal(matrix[1][1], 0.0, 0.000001), true);
}

TEST(MatrixTests, creates_3_by_3_zero_double)
{
	Matrix<double> matrix{ 3, 3 };
	EXPECT_EQ(eps_equal(matrix[0][0], 0.0, 0.000001), true);
	EXPECT_EQ(eps_equal(matrix[0][1], 0.0, 0.000001), true);
	EXPECT_EQ(eps_equal(matrix[0][2], 0.0, 0.000001), true);
	EXPECT_TRUE(eps_equal(matrix[1][0], 0.0, 0.000001));
	EXPECT_TRUE(eps_equal(matrix[1][1], 0.0, 0.000001));
	EXPECT_TRUE(eps_equal(matrix[1][2], 0.0, 0.000001));
	EXPECT_TRUE(eps_equal(matrix[2][0], 0.0, 0.000001));
	EXPECT_TRUE(eps_equal(matrix[2][1], 0.0, 0.000001));
	EXPECT_TRUE(eps_equal(matrix[2][2], 0.0, 0.000001));
}

// TODO : Random large check

TEST(MatrixTests, creates_2_by_2_list_int)
{
	Matrix<int> matrix{ 2, 2 };
	matrix = { 1, 2, 3, 4 };
	
	EXPECT_EQ(matrix[0][0], 1);
	EXPECT_EQ(matrix[0][1], 2);
	EXPECT_EQ(matrix[1][0], 3);
	EXPECT_EQ(matrix[1][1], 4);
}

TEST(MatrixTests, creates_3_by_5_list_int)
{
	Matrix<int> matrix{ 3, 5 };
	matrix = {
		 1,  2,  3,  4,  5,
		 6,  7,  8,  9, 10,
		11, 12, 13, 14, 15
	};

	EXPECT_EQ(matrix[0][0], 1);
	EXPECT_EQ(matrix[0][1], 2);
	EXPECT_EQ(matrix[0][2], 3);
	EXPECT_EQ(matrix[0][3], 4);
	EXPECT_EQ(matrix[0][4], 5);

	EXPECT_EQ(matrix[1][0], 6);
	EXPECT_EQ(matrix[1][1], 7);
	EXPECT_EQ(matrix[1][2], 8);
	EXPECT_EQ(matrix[1][3], 9);
	EXPECT_EQ(matrix[1][4], 10);

	EXPECT_EQ(matrix[2][0], 11);
	EXPECT_EQ(matrix[2][1], 12);
	EXPECT_EQ(matrix[2][2], 13);
	EXPECT_EQ(matrix[2][3], 14);
	EXPECT_EQ(matrix[2][4], 15);
}

// Testing copy / move operators
TEST(MatrixTests, assign_copies_2_by_2_int)
{
	Matrix<int>* matrix1 = new Matrix<int>(2, 2);
	Matrix<int>* matrix2 = new Matrix<int>(2, 2);
	*matrix2 = { 1, 2, 3, 4 };
	
	EXPECT_NO_THROW(*matrix2 = *matrix1);
	EXPECT_EQ((*matrix1)[0][0], (*matrix2)[0][0]);
	EXPECT_EQ((*matrix1)[0][1], (*matrix2)[0][1]);
	EXPECT_EQ((*matrix1)[1][0], (*matrix2)[1][0]);
	EXPECT_EQ((*matrix1)[1][0], (*matrix2)[1][1]);
}

TEST(MatrixTests, assign_deletes_2_by_2_double_copies)
{
	Matrix<double>* matrix1 = new Matrix<double>(2, 2);
	Matrix<double>* matrix2 = new Matrix<double>(2, 2);

	EXPECT_NO_FATAL_FAILURE(*matrix2 = *matrix1);
	EXPECT_NO_FATAL_FAILURE(delete matrix2);
	EXPECT_NO_FATAL_FAILURE(delete matrix1);
}

TEST(MatrixTests, construct_copies_2_by_2_double_copies)
{
	Matrix<double>* matrix1 = new Matrix<double>(2, 2);
	*matrix1 = { 0.1, 0.02, 0.3, 0.0004 };
	
	Matrix<double> matrix2{ *matrix1 };
	EXPECT_EQ((*matrix1)[0][0], matrix2[0][0]);
	EXPECT_EQ((*matrix1)[0][1], matrix2[0][1]);
	EXPECT_EQ((*matrix1)[1][0], matrix2[1][0]);
	EXPECT_EQ((*matrix1)[1][1], matrix2[1][1]);
}

TEST(MatrixTests, construct_deletes_2_by_2_double_copies)
{
	Matrix<double>* matrix1 = new Matrix<double>(2, 2);
	Matrix<double> matrix2{ *matrix1 };

	EXPECT_NO_FATAL_FAILURE(delete matrix1);
}


TEST(MatrixTests, assign_moves_3_by_3_float)
{
	Matrix<float>* matrix1 = new Matrix<float>(3, 3);
	*matrix1 = {
		0.1f, 0.2f, 0.3f,
		0.4f, 0.5f, 0.6f,
		0.7f, 0.8f, 0.9f
	};

	Matrix<float> matrix2 = std::move(*matrix1);
	EXPECT_EQ(matrix2[0][0], 0.1f);
	EXPECT_EQ(matrix2[0][1], 0.2f);
	EXPECT_EQ(matrix2[0][2], 0.3f);
	EXPECT_EQ(matrix2[1][0], 0.4f);
	EXPECT_EQ(matrix2[1][1], 0.5f);
	EXPECT_EQ(matrix2[1][2], 0.6f);
	EXPECT_EQ(matrix2[2][0], 0.7f);
	EXPECT_EQ(matrix2[2][1], 0.8f);
	EXPECT_EQ(matrix2[2][2], 0.9f);
	
	EXPECT_NO_FATAL_FAILURE(delete(matrix1));
}

TEST(MatrixTests, construct_moves_3_by_5_float)
{
	Matrix<float>* matrix1 = new Matrix<float>(3, 5);
	*matrix1 = {
		 0.1f,  0.2f,  0.3f,  0.4f,  0.5f,
		 0.6f,  0.7f,  0.8f,  0.9f, 0.10f,
		0.11f, 0.12f, 0.13f, 0.14f, 0.15f
	};


	Matrix<float>* matrix2 = new Matrix<float>{ std::move(*matrix1) };
	EXPECT_EQ((*matrix2)[0][0], 0.1f);
	EXPECT_EQ((*matrix2)[0][1], 0.2f);
	EXPECT_EQ((*matrix2)[0][2], 0.3f);
	EXPECT_EQ((*matrix2)[0][3], 0.4f);
	EXPECT_EQ((*matrix2)[0][4], 0.5f);

	EXPECT_EQ((*matrix2)[1][0], 0.6f);
	EXPECT_EQ((*matrix2)[1][1], 0.7f);
	EXPECT_EQ((*matrix2)[1][2], 0.8f);
	EXPECT_EQ((*matrix2)[1][3], 0.9f);
	EXPECT_EQ((*matrix2)[1][4], 0.10f);

	EXPECT_EQ((*matrix2)[2][0], 0.11f);
	EXPECT_EQ((*matrix2)[2][1], 0.12f);
	EXPECT_EQ((*matrix2)[2][2], 0.13f);
	EXPECT_EQ((*matrix2)[2][3], 0.14f);
	EXPECT_EQ((*matrix2)[2][4], 0.15f);
}

TEST(MatrixTests, construct_deletes_3_by_5_moved)
{
	Matrix<float>* matrix1 = new Matrix<float>(3, 5);
	*matrix1 = {
		 0.1f,  0.2f,  0.3f,  0.4f,  0.5f,
		 0.6f,  0.7f,  0.8f,  0.9f, 0.10f,
		0.11f, 0.12f, 0.13f, 0.14f, 0.15f
	};

	Matrix<float>* matrix2 = new Matrix<float>{ std::move(*matrix1) };
	EXPECT_NO_FATAL_FAILURE(delete matrix2);
	EXPECT_NO_FATAL_FAILURE(delete matrix1);
}

TEST(MatrixTests, multiples_by_scalar)
{
	Matrix<int> matrix1{ 5, 5 };
	matrix1 = {
		1,  2,  3,  4,  5,
		6,  7,  8,  9,  10,
		11, 12, 13, 14, 15,
		16, 17, 18, 19, 20,
		21, 22, 23, 24, 25,
	};
	Matrix<int> result11 = matrix1 * 7;
	Matrix<int> result12 = 7 * matrix1;

	for (std::size_t row = 0; row < matrix1.n_rows(); ++row)
	{
		for (std::size_t col = 0; col < matrix1.n_cols(); ++col)
		{
			EXPECT_EQ(result11[row][col], matrix1[row][col] * 7);
			EXPECT_EQ(result12[row][col], 7 * matrix1[row][col]);
		}
	}
}

TEST(MatrixTests, multiplies_2_by_2_id_double)
{
	Matrix<double> matrix1{ 2,2 };
	matrix1 = {
		1.0, 0.0,
		0.0, 1.0
	};

	Matrix<double> matrix2{ 2, 2 };
	matrix2 = {
		2.13, 3.13,
		5.17, 10.1
	};

	Matrix<double> result = matrix1 * matrix2;

	EXPECT_TRUE(eps_equal(result[0][0], matrix2[0][0], 0.00001));
	EXPECT_TRUE(eps_equal(result[0][1], matrix2[0][1], 0.00001));
	EXPECT_TRUE(eps_equal(result[1][0], matrix2[1][0], 0.00001));
	EXPECT_TRUE(eps_equal(result[1][1], matrix2[1][1], 0.00001));
}

TEST(MatrixTests, multiplies_3_by_3_id_double)
{
	Matrix<double> matrix1{ 3, 3 };
	matrix1 = {
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0
	};

	Matrix<double> matrix2{ 3, 3 };
	matrix2 = {
		2.13, 3.13, 4.12,
		5.17, 10.1, 5.2933,
		1.11, 12.30, 2.3002
	};

	Matrix<double> result = matrix1 * matrix2;

	EXPECT_TRUE(eps_equal(result[0][0], matrix2[0][0], 0.00001));
	EXPECT_TRUE(eps_equal(result[0][1], matrix2[0][1], 0.00001));
	EXPECT_TRUE(eps_equal(result[0][2], matrix2[0][2], 0.00001));
	EXPECT_TRUE(eps_equal(result[1][0], matrix2[1][0], 0.00001));
	EXPECT_TRUE(eps_equal(result[1][1], matrix2[1][1], 0.00001));
	EXPECT_TRUE(eps_equal(result[1][2], matrix2[1][2], 0.00001));
	EXPECT_TRUE(eps_equal(result[2][0], matrix2[2][0], 0.00001));
	EXPECT_TRUE(eps_equal(result[2][1], matrix2[2][1], 0.00001));
	EXPECT_TRUE(eps_equal(result[2][2], matrix2[2][2], 0.00001));
}

TEST(MatrixTests, multiplies_2_by_2_vector)
{
	Matrix<double> matrix{ 2, 2 };
	matrix = {
		1.0, 0.0,
		0.0, 1.0
	};


	std::vector<double> column{2.1234, 3.6789};
	Matrix<double> result = matrix * column;

	EXPECT_TRUE(eps_equal(result[0][0], column[0], 0.00001));
	EXPECT_TRUE(eps_equal(result[1][0], column[1], 0.00001));
}

TEST(MatrixTests, adds_4_by_5)
{
	Matrix<double> A{ 4, 5 };
	A = {
		1.1,   2.2,   3.3,   4.4,   5.5,
		6.6,   7.7,   8.8,   9.9,   10.10,
		11.11, 12.12, 13.13, 14.14, 15.15,
		16.16, 17.17, 18.18, 19.19, 20.20
	};
	Matrix<double> B = A;

	Matrix<double> C = A + B;
	EXPECT_TRUE(eps_equal(C[0][0], 2 * A[0][0], 0.00001));
	EXPECT_TRUE(eps_equal(C[0][1], 2 * A[0][1], 0.00001));
	EXPECT_TRUE(eps_equal(C[0][2], 2 * A[0][2], 0.00001));
	EXPECT_TRUE(eps_equal(C[0][3], 2 * A[0][3], 0.00001));
	EXPECT_TRUE(eps_equal(C[0][4], 2 * A[0][4], 0.00001));

	EXPECT_TRUE(eps_equal(C[1][0], 2 * A[1][0], 0.00001));
	EXPECT_TRUE(eps_equal(C[1][1], 2 * A[1][1], 0.00001));
	EXPECT_TRUE(eps_equal(C[1][2], 2 * A[1][2], 0.00001));
	EXPECT_TRUE(eps_equal(C[1][3], 2 * A[1][3], 0.00001));
	EXPECT_TRUE(eps_equal(C[1][4], 2 * A[1][4], 0.00001));

	EXPECT_TRUE(eps_equal(C[2][0], 2 * A[2][0], 0.00001));
	EXPECT_TRUE(eps_equal(C[2][1], 2 * A[2][1], 0.00001));
	EXPECT_TRUE(eps_equal(C[2][2], 2 * A[2][2], 0.00001));
	EXPECT_TRUE(eps_equal(C[2][3], 2 * A[2][3], 0.00001));
	EXPECT_TRUE(eps_equal(C[2][4], 2 * A[2][4], 0.00001));

	EXPECT_TRUE(eps_equal(C[3][0], 2 * A[3][0], 0.00001));
	EXPECT_TRUE(eps_equal(C[3][1], 2 * A[3][1], 0.00001));
	EXPECT_TRUE(eps_equal(C[3][2], 2 * A[3][2], 0.00001));
	EXPECT_TRUE(eps_equal(C[3][3], 2 * A[3][3], 0.00001));
	EXPECT_TRUE(eps_equal(C[3][4], 2 * A[3][4], 0.00001));
}

TEST(MatrixTests, subtracts_3_by_4)
{
	Matrix<int> matrix1{ 3, 4 };
	matrix1 = {
		1,  2,  3,  4,
		5,  6,  7,  8,
		9, 10, 11, 12
	};

	Matrix<int> matrix2 = matrix1;
	Matrix<int> result = matrix1 - matrix2;


	EXPECT_EQ(result[0][0], 0);
	EXPECT_EQ(result[0][1], 0);
	EXPECT_EQ(result[0][2], 0);
	EXPECT_EQ(result[0][3], 0);

	EXPECT_EQ(result[1][0], 0);
	EXPECT_EQ(result[1][1], 0);
	EXPECT_EQ(result[1][2], 0);
	EXPECT_EQ(result[1][3], 0);

	EXPECT_EQ(result[2][0], 0);
	EXPECT_EQ(result[2][1], 0);
	EXPECT_EQ(result[2][2], 0);
	EXPECT_EQ(result[2][3], 0);
}
