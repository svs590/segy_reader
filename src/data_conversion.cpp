#include "data_conversion.h"



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

		res[i] = a2e[c];
	}

	return res;
}

short byte_to_short(byte_t const* ptr) {
	return ((short)ptr[1] << 8) + (short)ptr[0];
}

short byte_to_short_swap(byte_t const* ptr) {
	return ((short)ptr[0] << 8) + (short)ptr[1];
}

short byte_to_short_swap_pair(byte_t const* ptr) {
	return ((short)ptr[0] << 8) + (short)ptr[1];
}

short byte_to_short(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_short(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_short_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_short_swap_pair(ptr);
		break;
	default:
		break;
	}
}

unsigned short byte_to_ushort(byte_t const* ptr) {
	return ((unsigned short)ptr[1] << 8) + (unsigned short)ptr[0];
}

unsigned short byte_to_ushort_swap(byte_t const* ptr) {
	return ((unsigned short)ptr[0] << 8) + (unsigned short)ptr[1];
}

unsigned short byte_to_ushort_swap_pair(byte_t const* ptr) {
	return ((unsigned short)ptr[0] << 8) + (unsigned short)ptr[1];
}

unsigned short byte_to_ushort(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_ushort(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_ushort_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_ushort_swap_pair(ptr);
		break;
	default:
		break;
	}
}

int byte_to_int(byte_t const* ptr) {
	return
		((int)ptr[3] << 24) + ((int)ptr[2] << 16) + ((int)ptr[1] << 8) + (int)ptr[0];
}

int byte_to_int_swap(byte_t const* ptr) {
	return
		((int)ptr[0] << 24) + ((int)ptr[1] << 16) + ((int)ptr[2] << 8) + (int)ptr[3];
}

int byte_to_int_swap_pair(byte_t const* ptr) {
	return
		((int)ptr[0] << 16) + ((int)ptr[1] << 24) + (int)ptr[2] + ((int)ptr[3] << 8);
}

int byte_to_int(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_int(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_int_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_int_swap_pair(ptr);
		break;
	default:
		break;
	}
}

uint64_t byte_to_uint64(byte_t const* ptr) {
	return
		((uint64_t)ptr[7] << 56) + ((uint64_t)ptr[6] << 48) + ((uint64_t)ptr[5] << 40) + ((uint64_t)ptr[4] << 32) +
		((uint64_t)ptr[3] << 24) + ((uint64_t)ptr[2] << 16) + ((uint64_t)ptr[1] << 8) + (uint64_t)ptr[0];
}

uint64_t byte_to_uint64_swap(byte_t const* ptr) {
	return
		((uint64_t)ptr[0] << 56) + ((uint64_t)ptr[1] << 48) + ((uint64_t)ptr[2] << 40) + ((uint64_t)ptr[3] << 32) +
		((uint64_t)ptr[4] << 24) + ((uint64_t)ptr[5] << 16) + ((uint64_t)ptr[6] << 8) + (uint64_t)ptr[7];
}

uint64_t byte_to_uint64_swap_pair(byte_t const* ptr) {
	return
		((uint64_t)ptr[1] << 56) + ((uint64_t)ptr[0] << 48) + ((uint64_t)ptr[3] << 40) + ((uint64_t)ptr[2] << 32) +
		((uint64_t)ptr[5] << 24) + ((uint64_t)ptr[4] << 16) + ((uint64_t)ptr[7] << 8) + (uint64_t)ptr[6];
}

uint64_t byte_to_uint64(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_uint64(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_uint64_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_uint64_swap_pair(ptr);
		break;
	default:
		break;
	}
}

int64_t byte_to_int64(byte_t const* ptr) {
	return
		((int64_t)ptr[7] << 56) + ((int64_t)ptr[6] << 48) + ((int64_t)ptr[5] << 40) + ((int64_t)ptr[4] << 32) +
		((int64_t)ptr[3] << 24) + ((int64_t)ptr[2] << 16) + ((int64_t)ptr[1] << 8) + (int64_t)ptr[0];
}

int64_t byte_to_int64_swap(byte_t const* ptr) {
	return
		((int64_t)ptr[0] << 56) + ((int64_t)ptr[1] << 48) + ((int64_t)ptr[2] << 40) + ((int64_t)ptr[3] << 32) +
		((int64_t)ptr[4] << 24) + ((int64_t)ptr[5] << 16) + ((int64_t)ptr[6] << 8) + (int64_t)ptr[7];
}

int64_t byte_to_int64_swap_pair(byte_t const* ptr) {
	return
		((int64_t)ptr[1] << 56) + ((int64_t)ptr[0] << 48) + ((int64_t)ptr[3] << 40) + ((int64_t)ptr[2] << 32) +
		((int64_t)ptr[5] << 24) + ((int64_t)ptr[4] << 16) + ((int64_t)ptr[7] << 8) + (int64_t)ptr[6];
}

int64_t byte_to_int64(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_int64(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_int64_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_int64_swap_pair(ptr);
		break;
	default:
		break;
	}
}

float byte_to_float(byte_t const* ptr) {
	float f;
	memcpy(&f, ptr, 4);
	return f;
}

float byte_to_float_swap(byte_t const* ptr) {
	char c[4];
	c[0] = ptr[3];
	c[1] = ptr[2];
	c[2] = ptr[1];
	c[3] = ptr[0];
	float f;
	memcpy(&f, c, 4);
	return f;
}

float byte_to_float_swap_pair(byte_t const* ptr) {
	char c[4];
	c[0] = ptr[2];
	c[1] = ptr[3];
	c[2] = ptr[0];
	c[3] = ptr[1];
	float f;
	memcpy(&f, c, 4);
	return f;
}

float byte_to_float(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_float(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_float_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_float_swap_pair(ptr);
		break;
	default:
		break;
	}
}

double byte_to_double( byte_t const* ptr ) {
	double f;
	memcpy(&f, ptr, 8);
	return f;
}

double byte_to_double_swap( byte_t const* ptr ) {
	char c[8];
	c[0] = ptr[7];
	c[1] = ptr[6];
	c[2] = ptr[5];
	c[3] = ptr[4];
	c[4] = ptr[3];
	c[5] = ptr[2];
	c[6] = ptr[1];
	c[7] = ptr[0];
	double f;
	memcpy(&f, c, 8);
	return f;
}

double byte_to_double_swap_pair(byte_t const* ptr) {
	char c[8];
	c[0] = ptr[6];
	c[1] = ptr[7];
	c[2] = ptr[4];
	c[3] = ptr[5];
	c[4] = ptr[2];
	c[5] = ptr[3];
	c[6] = ptr[0];
	c[7] = ptr[1];
	double f;
	memcpy(&f, c, 8);
	return f;
}

double byte_to_double(byte_t const* ptr, endian_swap swap) {
	switch (swap)
	{
	case endian_swap::none:
		return byte_to_double(ptr);
		break;
	case endian_swap::reverse:
		return byte_to_double_swap(ptr);
		break;
	case endian_swap::pair:
		return byte_to_double_swap_pair(ptr);
		break;
	default:
		break;
	}
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
