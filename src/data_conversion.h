#pragma once

#include <string>

#include "data_types.h"

enum class endian_swap { none, reverse, pair };

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
short			byte_to_short(byte_t const* ptr);
short			byte_to_short_swap(byte_t const* ptr);
short			byte_to_short_swap_pair(byte_t const* ptr);
short			byte_to_short(byte_t const* ptr, endian_swap swap);

unsigned short	byte_to_ushort(byte_t const* ptr);
unsigned short	byte_to_ushort_swap(byte_t const* ptr);
unsigned short	byte_to_ushort_swap_pair(byte_t const* ptr);
unsigned short	byte_to_ushort(byte_t const* ptr, endian_swap swap);

uint64_t		byte_to_uint64(byte_t const* ptr);
uint64_t		byte_to_uint64_swap(byte_t const* ptr);
uint64_t		byte_to_uint64_swap_pair(byte_t const* ptr);
uint64_t		byte_to_uint64(byte_t const* ptr, endian_swap swap);

int64_t			byte_to_int64(byte_t const* ptr);
int64_t			byte_to_int64_swap(byte_t const* ptr);
int64_t			byte_to_int64_swap_pair(byte_t const* ptr);
int64_t			byte_to_int64(byte_t const* ptr, endian_swap swap);

int				byte_to_int_swap(byte_t const* ptr);
int				byte_to_int_swap_pair(byte_t const* ptr);
int				byte_to_int(byte_t const* ptr);
int				byte_to_int(byte_t const* ptr, endian_swap swap);

/**
 * Float conversion with or without swap endian
 */
float			byte_to_float(byte_t const* ptr);
float			byte_to_float_swap(byte_t const* ptr);
float			byte_to_float_swap_pair(byte_t const* ptr);
float			byte_to_float(byte_t const* ptr, endian_swap swap);

double			byte_to_double( byte_t const* ptr );
double			byte_to_double_swap( byte_t const* ptr );
double			byte_to_double_swap_pair(byte_t const* ptr);
double			byte_to_double(byte_t const* ptr, endian_swap swap);


/**
 * Inverse integer conversion with or without swap endian
 */
void			short_to_byte_swap(short value, byte_t* outPtr);
void			short_to_byte(short value, byte_t* outPtr);
void			int_to_byte(int value, byte_t* outPtr);
void			int_to_byte_swap(int value, byte_t* outPtr);
void			float_to_byte_swap(float value, byte_t* outPtr);
void			float_to_byte(float value, byte_t* outPtr);