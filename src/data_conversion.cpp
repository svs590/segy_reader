#include "data_conversion.h"

#include <boost/endian/conversion.hpp>
using namespace boost::endian;

const char e2a[] = {
  '@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@',
  '@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@',
  '@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@','@',
  '@','@','@','@',' ','@','@','@','@','@','@','@','@','@','[','.','<','(','+','|',
  '&','@','@','@','@','@','@','@','@','@',']','$','*',')',';','^','-','/','@','@',
  '@','@','@','@','@','@','|',',','%','_','>','?','@','@','@','@','@','@','@','@',
  '@','.',':','#','@','.','=','"',' ','a','b','c','d','e','f','g','h','i',' ',' ',
  ' ',' ',' ',' ',' ','j','k','l','m','n','o','p','q','r',' ',' ',' ',' ',' ',' ',
  ' ','~','s','t','u','v','w','x','y','z',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
  '@','@','@','@','@','@','@','@','@','@','@','@','{','A','B','C','D','E','F','G',
  'H','I',' ',' ',' ',' ',' ',' ','}','J','K','L','M','N','O','P','Q','R',' ',' ',
  ' ',' ',' ',' ','/',' ','S','T','U','V','W','X','Y','Z',' ',' ',' ',' ',' ',' ',
  '0','1','2','3','4','5','6','7','8','9',' ',' ',' ',' ',' ',' '
};

const short a2e[] = {
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,64,32,127,123,91,108,80,32,
  77,93,92,78,107,96,75,97,240,241,242,243,244,245,246,247,248,249,122,94,
  76,126,110,111,0,193,194,195,196,197,198,199,200,201,209,210,211,212,213,214,
  215,216,217,226,227,228,229,230,231,232,233,74,32,90,95,109,32,129,130,131,
  132,133,134,135,136,137,145,146,147,148,149,150,151,152,153,162,163,164,165,166,
  167,168,169,192,79,208,161,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32
};

std::string ebcdic_to_char(const std::string &str) {
	std::string res;
	res.resize(str.length());

	for (int i = 0; i < str.length(); i++) {
		short c = (short)str[i];
		if (c < 0) 
			c += 256;
		if (c < 0 || c > 255) 
			res[i] = ' ';
		else 
			res[i] = e2a[c];
	}
	return res;
}

/**
 * Convert from C char to EBCDIC code
 */
std::string char_to_ecdic(const std::string &str) {
	std::string res;
	res.resize(str.length());

	for (int i = 0; i < str.length(); i++) {
		int c = (int)str[i];

		res[i] = (char)a2e[c];
	}

	return res;
}

template <>
short byte_to_short<endian_order::big>(byte_t const* ptr) {
    short val = *reinterpret_cast<short const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
short byte_to_short<endian_order::little>(byte_t const* ptr) {
    short val = *reinterpret_cast<short const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
short byte_to_short<endian_order::mid_big>(byte_t const* ptr) {
    short val = *reinterpret_cast<short const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
short byte_to_short<endian_order::mid_little>(byte_t const* ptr) {
    short val = *reinterpret_cast<short const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
unsigned short byte_to_ushort<endian_order::big>(byte_t const* ptr) {
    unsigned short val = *reinterpret_cast<unsigned short const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
unsigned short byte_to_ushort<endian_order::little>(byte_t const* ptr) {
    unsigned short val = *reinterpret_cast<unsigned short const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
unsigned short byte_to_ushort<endian_order::mid_big>(byte_t const* ptr) {
    unsigned short val = *reinterpret_cast<unsigned short const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
unsigned short byte_to_ushort<endian_order::mid_little>(byte_t const* ptr) {
    unsigned short val = *reinterpret_cast<unsigned short const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
int byte_to_int<endian_order::big>(byte_t const* ptr) {
    int val = *reinterpret_cast<int const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
int byte_to_int<endian_order::little>(byte_t const* ptr) {
    int val = *reinterpret_cast<int const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
int byte_to_int<endian_order::mid_big>(byte_t const* ptr) {
    byte_t mid_big[4];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    int val = *reinterpret_cast<int const*>(mid_big);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
int byte_to_int<endian_order::mid_little>(byte_t const* ptr) {
    byte_t mid_big[4];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    int val = *reinterpret_cast<int const*>(mid_big);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
uint64_t byte_to_uint64<endian_order::big>(byte_t const* ptr) {
    uint64_t val = *reinterpret_cast<uint64_t const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
uint64_t byte_to_uint64<endian_order::little>(byte_t const* ptr) {
    uint64_t val = *reinterpret_cast<uint64_t const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
uint64_t byte_to_uint64<endian_order::mid_big>(byte_t const* ptr) {
    byte_t mid_big[8];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    mid_big[4] = ptr[5];
    mid_big[5] = ptr[4];
    mid_big[6] = ptr[7];
    mid_big[7] = ptr[6];
    uint64_t val = *reinterpret_cast<uint64_t const*>(mid_big);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
uint64_t byte_to_uint64<endian_order::mid_little>(byte_t const* ptr) {
    byte_t mid_big[8];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    mid_big[4] = ptr[5];
    mid_big[5] = ptr[4];
    mid_big[6] = ptr[7];
    mid_big[7] = ptr[6];
    uint64_t val = *reinterpret_cast<uint64_t const*>(mid_big);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
int64_t byte_to_int64<endian_order::big>(byte_t const* ptr) {
    int64_t val = *reinterpret_cast<int64_t const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
int64_t byte_to_int64<endian_order::little>(byte_t const* ptr) {
    int64_t val = *reinterpret_cast<int64_t const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
int64_t byte_to_int64<endian_order::mid_big>(byte_t const* ptr) {
    byte_t mid_big[8];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    mid_big[4] = ptr[5];
    mid_big[5] = ptr[4];
    mid_big[6] = ptr[7];
    mid_big[7] = ptr[6];
    int64_t val = *reinterpret_cast<int64_t const*>(mid_big);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
int64_t byte_to_int64<endian_order::mid_little>(byte_t const* ptr) {
    byte_t mid_big[8];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    mid_big[4] = ptr[5];
    mid_big[5] = ptr[4];
    mid_big[6] = ptr[7];
    mid_big[7] = ptr[6];
    int64_t val = *reinterpret_cast<int64_t const*>(mid_big);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
float byte_to_float<endian_order::big>(byte_t const* ptr) {
    int val = *reinterpret_cast<int const*>(ptr);
    val = conditional_reverse<order::big, order::native>(val);
    return *reinterpret_cast<float const*>(&val);
}

template <>
float byte_to_float<endian_order::little>(byte_t const* ptr) {
    int val = *reinterpret_cast<int const*>(ptr);
    val = conditional_reverse<order::little, order::native>(val);
    return *reinterpret_cast<float const*>(&val);
}

template <>
float byte_to_float<endian_order::mid_big>(byte_t const* ptr) {
    byte_t mid_big[4];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    int val = *reinterpret_cast<int const*>(ptr);
    val = conditional_reverse<order::big, order::native>(val);
    return *reinterpret_cast<float const*>(&val);
}

template <>
float byte_to_float<endian_order::mid_little>(byte_t const* ptr) {
    byte_t mid_big[4];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    int val = *reinterpret_cast<int const*>(ptr);
    val = conditional_reverse<order::little, order::native>(val);
    return *reinterpret_cast<float const*>(&val);
}

template <>
double byte_to_double<endian_order::big>( byte_t const* ptr ) {
    int64_t val = *reinterpret_cast<int64_t const*>(ptr);
    val = conditional_reverse<order::big, order::native>(val);
    return *reinterpret_cast<double const*>(&val);
}

template <>
double byte_to_double<endian_order::little>(byte_t const* ptr) {
    int64_t val = *reinterpret_cast<int64_t const*>(ptr);
    val = conditional_reverse<order::little, order::native>(val);
    return *reinterpret_cast<double const*>(&val);
}

template <>
double byte_to_double<endian_order::mid_big>(byte_t const* ptr) {
    byte_t mid_big[8];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    mid_big[4] = ptr[5];
    mid_big[5] = ptr[4];
    mid_big[6] = ptr[7];
    mid_big[7] = ptr[6];
    int64_t val = *reinterpret_cast<int64_t const*>(ptr);
    val = conditional_reverse<order::big, order::native>(val);
    return *reinterpret_cast<double const*>(&val);
}

template <>
double byte_to_double<endian_order::mid_little>(byte_t const* ptr) {
    byte_t mid_big[8];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    mid_big[4] = ptr[5];
    mid_big[5] = ptr[4];
    mid_big[6] = ptr[7];
    mid_big[7] = ptr[6];
    int64_t val = *reinterpret_cast<int64_t const*>(ptr);
    val = conditional_reverse<order::little, order::native>(val);
    return *reinterpret_cast<double const*>(&val);
}

void short_to_byte_swap(short value, byte_t* outPtr) {
	outPtr[1] = value & 0xff;
	outPtr[0] = (value & 0xff00) >> 8;
}

void short_to_byte(short value, byte_t* outPtr) {
	outPtr[0] = value & 0xff;
	outPtr[1] = (value & 0xff00) >> 8;
}

void int_to_byte(int value, byte_t* outPtr) {
	outPtr[0] = value & 0xff;
	outPtr[1] = (value & 0xff00) >> 8;
	outPtr[2] = (value & 0xff0000) >> 16;
	outPtr[3] = (value & 0xff000000) >> 24;
}

void int_to_byte_swap(int value, byte_t* outPtr) {
	outPtr[3] = value & 0xff;
	outPtr[2] = (value & 0xff00) >> 8;
	outPtr[1] = (value & 0xff0000) >> 16;
	outPtr[0] = (value & 0xff000000) >> 24;
}

void float_to_byte_swap(float value, byte_t* outPtr) {
	// ???
}

void float_to_byte(float value, byte_t* outPtr) {
	// ???
}
