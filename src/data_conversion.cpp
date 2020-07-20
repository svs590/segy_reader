#include "data_conversion.h"

#include <Eigen/Dense>

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

int8_t byte_to_int8_t(byte_t const* ptr, endian_order order) {
    return static_cast<int8_t>(*ptr);
}

uint8_t byte_to_uint8_t(byte_t const* ptr, endian_order order) {
    return static_cast<uint8_t>(*ptr);
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
uint32_t byte_to_uint<endian_order::big>(byte_t const* ptr) {
    uint32_t val = *reinterpret_cast<uint32_t const*>(ptr);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
uint32_t byte_to_uint<endian_order::little>(byte_t const* ptr) {
    uint32_t val = *reinterpret_cast<uint32_t const*>(ptr);
    return conditional_reverse<order::little, order::native>(val);
}

template <>
uint32_t byte_to_uint<endian_order::mid_big>(byte_t const* ptr) {
    byte_t mid_big[4];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    uint32_t val = *reinterpret_cast<uint32_t const*>(mid_big);
    return conditional_reverse<order::big, order::native>(val);
}

template <>
uint32_t byte_to_uint<endian_order::mid_little>(byte_t const* ptr) {
    byte_t mid_big[4];
    mid_big[0] = ptr[1];
    mid_big[1] = ptr[0];
    mid_big[2] = ptr[3];
    mid_big[3] = ptr[2];
    uint32_t val = *reinterpret_cast<uint32_t const*>(mid_big);
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

void int8_t_to_byte(int8_t value, byte_t *ptr, endian_order order) {
    *ptr = static_cast<byte_t>(value);
}

void uint8_t_to_byte(uint8_t value, byte_t *ptr, endian_order order) {
    *ptr = static_cast<byte_t>(value);
}

template <>
void short_to_byte<endian_order::big>(short value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(short));
}

template <>
void short_to_byte<endian_order::little>(short value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::little>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(short));
}


template <>
void ushort_to_byte<endian_order::big>(unsigned short value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(unsigned short));
}

template <>
void ushort_to_byte<endian_order::little>(unsigned short value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::little>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(unsigned short));
}

template <>
void int_to_byte<endian_order::big>(int value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(int));
}

template <>
void int_to_byte<endian_order::little>(int value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::little>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(int));
}

template <>
void int_to_byte<endian_order::mid_big>(int value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
}

template <>
void int_to_byte<endian_order::mid_little>(int value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::little>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
}

template <>
void float_to_byte<endian_order::big>(float value, byte_t* outPtr) {
    int val = *reinterpret_cast<int const*>(&value);
    val = conditional_reverse<order::native, order::big>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    memcpy(outPtr, ptr, sizeof(int));
}

template <>
void float_to_byte<endian_order::little>(float value, byte_t* outPtr) {
    int val = *reinterpret_cast<int const*>(&value);
    val = conditional_reverse<order::native, order::little>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    memcpy(outPtr, ptr, sizeof(int));
}

template <>
void float_to_byte<endian_order::mid_big>(float value, byte_t* outPtr) {
    int val = *reinterpret_cast<int const*>(&value);
    val = conditional_reverse<order::native, order::big>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
}

template <>
void float_to_byte<endian_order::mid_little>(float value, byte_t* outPtr) {
    int val = *reinterpret_cast<int const*>(&value);
    val = conditional_reverse<order::native, order::little>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
}

template <>
void int64_to_byte<endian_order::big>(int64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(int64_t));
}

template <>
void int64_to_byte<endian_order::little>(int64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(int64_t));
}

template <>
void int64_to_byte<endian_order::mid_big>(int64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    
    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
    outPtr[4] = ptr[5];
    outPtr[5] = ptr[4];
    outPtr[6] = ptr[7];
    outPtr[7] = ptr[6];
}

template <>
void int64_to_byte<endian_order::mid_little>(int64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);

    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
    outPtr[4] = ptr[5];
    outPtr[5] = ptr[4];
    outPtr[6] = ptr[7];
    outPtr[7] = ptr[6];
}

template <>
void uint64_to_byte<endian_order::big>(uint64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(uint64_t));
}

template <>
void uint64_to_byte<endian_order::little>(uint64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);
    memcpy(outPtr, ptr, sizeof(uint64_t));
}

template <>
void uint64_to_byte<endian_order::mid_big>(uint64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);

    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
    outPtr[4] = ptr[5];
    outPtr[5] = ptr[4];
    outPtr[6] = ptr[7];
    outPtr[7] = ptr[6];
}

template <>
void uint64_to_byte<endian_order::mid_little>(uint64_t value, byte_t* outPtr) {
    value = conditional_reverse<order::native, order::big>(value);
    byte_t *ptr = reinterpret_cast<byte_t*>(&value);

    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
    outPtr[4] = ptr[5];
    outPtr[5] = ptr[4];
    outPtr[6] = ptr[7];
    outPtr[7] = ptr[6];
}

template <>
void double_to_byte<endian_order::big>(double value, byte_t* outPtr) {
    int64_t val = *reinterpret_cast<int64_t const*>(&value);
    val = conditional_reverse<order::native, order::big>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    memcpy(outPtr, ptr, sizeof(int64_t));
}

template <>
void double_to_byte<endian_order::little>(double value, byte_t* outPtr) {
    int64_t val = *reinterpret_cast<int64_t const*>(&value);
    val = conditional_reverse<order::native, order::little>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    memcpy(outPtr, ptr, sizeof(int64_t));
}

template <>
void double_to_byte<endian_order::mid_big>(double value, byte_t* outPtr) {
    int64_t val = *reinterpret_cast<int64_t const*>(&value);
    val = conditional_reverse<order::native, order::big>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);
    
    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
    outPtr[4] = ptr[5];
    outPtr[5] = ptr[4];
    outPtr[6] = ptr[7];
    outPtr[7] = ptr[6];
}

template <>
void double_to_byte<endian_order::mid_little>(double value, byte_t* outPtr) {
    int64_t val = *reinterpret_cast<int64_t const*>(&value);
    val = conditional_reverse<order::native, order::little>(val);
    byte_t *ptr = reinterpret_cast<byte_t*>(&val);

    outPtr[0] = ptr[1];
    outPtr[1] = ptr[0];
    outPtr[2] = ptr[3];
    outPtr[3] = ptr[2];
    outPtr[4] = ptr[5];
    outPtr[5] = ptr[4];
    outPtr[6] = ptr[7];
    outPtr[7] = ptr[6];
}

short byte_to_short(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_short<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_short<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_short<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_short<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

unsigned short byte_to_ushort(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_ushort<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_ushort<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_ushort<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_ushort<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

int byte_to_int(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_int<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_int<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_int<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_int<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

uint32_t byte_to_uint(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_uint<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_uint<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_uint<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_uint<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

int64_t byte_to_int64(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_int64<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_int64<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_int64<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_int64<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

uint64_t byte_to_uint64(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_uint64<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_uint64<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_uint64<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_uint64<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

float byte_to_float(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_float<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_float<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_float<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_float<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

double byte_to_double(byte_t const* ptr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return byte_to_double<endian_order::big>(ptr);
        break;
    case endian_order::little:
        return byte_to_double<endian_order::little>(ptr);
        break;
    case endian_order::mid_big:
        return byte_to_double<endian_order::mid_big>(ptr);
        break;
    case endian_order::mid_little:
        return byte_to_double<endian_order::mid_little>(ptr);
        break;
    default:
        break;
    }
}

void short_to_byte(short value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return short_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return short_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return short_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return short_to_byte<endian_order::little>(value, outPtr);
        break;
    default:
        break;
    }
}

void ushort_to_byte(unsigned short value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return ushort_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return ushort_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return ushort_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return ushort_to_byte<endian_order::little>(value, outPtr);
        break;
    default:
        break;
    }
}

void int_to_byte(int value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return int_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return int_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return int_to_byte<endian_order::mid_big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return int_to_byte<endian_order::mid_little>(value, outPtr);
        break;
    default:
        break;
    }
}

void float_to_byte(float value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return float_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return float_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return float_to_byte<endian_order::mid_big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return float_to_byte<endian_order::mid_little>(value, outPtr);
        break;
    default:
        break;
    }
}

void int64_to_byte(int64_t value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return int64_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return int64_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return int64_to_byte<endian_order::mid_big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return int64_to_byte<endian_order::mid_little>(value, outPtr);
        break;
    default:
        break;
    }
}

void uint64_to_byte(uint64_t value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return uint64_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return uint64_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return uint64_to_byte<endian_order::mid_big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return uint64_to_byte<endian_order::mid_little>(value, outPtr);
        break;
    default:
        break;
    }
}

void double_to_byte(double value, byte_t* outPtr, endian_order order) {
    switch (order)
    {
    case endian_order::big:
        return double_to_byte<endian_order::big>(value, outPtr);
        break;
    case endian_order::little:
        return double_to_byte<endian_order::little>(value, outPtr);
        break;
    case endian_order::mid_big:
        return double_to_byte<endian_order::mid_big>(value, outPtr);
        break;
    case endian_order::mid_little:
        return double_to_byte<endian_order::mid_little>(value, outPtr);
        break;
    default:
        break;
    }
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::float32_ibm>(
    const byte_t *buffer, 
    int buffer_size, 
    endian_order order) {

    unsigned fraction;
    byte_t *f = new byte_t[4];
    int exponent;
    int signum;

    Eigen::VectorXf res(buffer_size / 4);

    for (int i = 0; i < buffer_size; i += 4) {
        fraction = byte_to_uint(&buffer[i], order);

        signum = fraction >> 31;
        fraction <<= 1;
        exponent = fraction >> 25;
        fraction <<= 7;

        if (fraction == 0) {
            exponent = 0;
        }
        else {
            exponent = (exponent << 2) - 130;

            while (fraction < 0x80000000) {
                --exponent;
                fraction <<= 1;
            }

            if (exponent <= 0) {
                if (exponent < -24) {
                    fraction = 0;
                }
                else {
                    fraction >>= -exponent;
                }
                exponent = 0;
            }
            else if (exponent >= 255) {
                fraction = 0;
                exponent = 255;
            }
            else {
                fraction <<= 1;
            }
        }

        fraction = (fraction >> 9) | (exponent << 23) | (signum << 31);
        //memcpy(&buffer[i], &fraction, 4);
        res[i / 4] = *reinterpret_cast<float*>(&fraction);
    }

    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::float32>(
    const byte_t *buffer,
    int buffer_size, 
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::float32);
    Eigen::VectorXf res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_float(&buffer[i], order);
    }
    return res;
}


template <>
seismic_variant_vector segy_data_to_native<segy_data_format::float64>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::float64);
    Eigen::VectorXd res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_double(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::int64_2complement>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::int64_2complement);
    Eigen::Matrix<int64_t, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_int64(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::int32_2complement>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::int32_2complement);
    Eigen::VectorXi res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_int(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::int16_2complement>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::int16_2complement);
    Eigen::Matrix<short, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_short(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::int8_2complement>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::int8_2complement);
    Eigen::Matrix<short, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = (short)*reinterpret_cast<const char*>(&buffer[i]);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::uint64>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::uint64);
    Eigen::Matrix<uint64_t, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_uint64(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::uint32>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::uint32);
    Eigen::Matrix<uint32_t, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_uint(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::uint16>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::uint16);
    Eigen::Matrix<unsigned short, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = byte_to_ushort(&buffer[i], order);
    }
    return res;
}

template <>
seismic_variant_vector segy_data_to_native<segy_data_format::uint8>(
    const byte_t *buffer,
    int buffer_size,
    endian_order order) {

    short byte_size = segy_data_format_size(segy_data_format::uint8);
    Eigen::Matrix<unsigned short, -1, 1> res(buffer_size / byte_size);
    for (int i = 0; i < buffer_size; i += byte_size) {
        res[i / byte_size] = (unsigned short)buffer[i];
    }
    return res;
}


//template <>
//void native_to_segy_data<segy_data_format::float32_ibm>(const byte_t *buffer, int buffer_size, endian_order order) {
//    unsigned fraction;
//    int exponent;
//    int signum;
//
//    for (int i = 0; i < buffer_size; i += 4) {
//        memcpy(&fraction, &buffer[i], 4);
//        signum = fraction >> 31;
//        fraction <<= 1;
//        exponent = fraction >> 24;
//        fraction <<= 8;
//
//        if (exponent > 0 && exponent != 255) {
//            fraction = (fraction >> 1) | 0x80000000;
//            exponent += 130;
//            fraction >>= -exponent & 3;
//            exponent = (exponent + 3) >> 2;
//
//            while (fraction < 0x10000000) {
//                --exponent;
//                fraction <<= 4;
//            }
//        }
//        else { // fraction == 0 || fraction == 255
//            if (exponent == 255) {
//                fraction = 0xffffff00;
//                exponent = 0x7f;
//            }
//        }
//
//        fraction = (fraction >> 8) | (exponent << 24) | (signum << 31);
//        memcpy(&buffer[i], &fraction, 4);
//    }
//}