#ifndef DATA_STRUCTURES_BYTE_ARRAY_H
#define DATA_STRUCTURES_BYTE_ARRAY_H
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>

using Byte = uint8_t;

class ByteArray
{
public:
	explicit ByteArray(std::size_t size);
	ByteArray(Byte* data, std::size_t size);
	explicit ByteArray(std::vector<Byte> data);

	std::size_t get_size() const;
	Byte* get_data();
	void set_data(Byte const* data, std::size_t data_size, std::size_t offset = 0);

private:
	std::vector<Byte> data_;
};

class ByteArrayException : public std::runtime_error
{
public: 
	explicit ByteArrayException(std::string err_message);
	const char* what() const noexcept override;
private:
	std::string message_;
};

#endif // DATA_STRUCTURES_BYTE_ARRAY_H
