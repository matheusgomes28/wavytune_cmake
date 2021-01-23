// #include <data_structures/byte_array.h>
#include <data_structures/bytes.h>
#include <matrix/matrix.h>
// #include <gl_abstractions/vao.h>
#include <iostream>

int main()
{
	// VAO vao{"vao testing"};
	wt::Matrix<double> hello{2,2};
	// ByteArray hello2(20);

	wt::ByteArray<10> hello1{ 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 };

    std::cout << "Hello World!" << std::endl;
	return 0;
}
