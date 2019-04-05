#pragma once

#include <string>

#include "data_types.h"

enum class endian_order { big, little, mid_big, mid_little };

/**
 * Convert from EBCDIC code to C char
 */
std::string ebcdic_to_char(const std::string &str);

/**
 * Convert from C char to EBCDIC code
 */
std::string char_to_ecdic(const std::string &str);


/**
 * Integer conversion with or without swap endian
 */
template <endian_order From>
short			byte_to_short(byte_t const* ptr);

template <endian_order From>
unsigned short	byte_to_ushort(byte_t const* ptr);

template <endian_order From>
uint64_t		byte_to_uint64(byte_t const* ptr);

template <endian_order From>
int64_t			byte_to_int64(byte_t const* ptr);

template <endian_order From>
int				byte_to_int(byte_t const* ptr);


/**
 * Float conversion with or without swap endian
 */
template <endian_order From>
float			byte_to_float(byte_t const* ptr);

template <endian_order From>
double			byte_to_double( byte_t const* ptr );


/**
 * Inverse integer conversion with or without swap endian
 */
void			short_to_byte_swap(short value, byte_t* outPtr);
void			short_to_byte(short value, byte_t* outPtr);
void			int_to_byte(int value, byte_t* outPtr);
void			int_to_byte_swap(int value, byte_t* outPtr);
void			float_to_byte_swap(float value, byte_t* outPtr);
void			float_to_byte(float value, byte_t* outPtr);