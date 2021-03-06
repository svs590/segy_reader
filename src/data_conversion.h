#pragma once

#include <string>

#include "data_types.h"
#include "segy_file.h"

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
short			byte_to_short(byte_t const* ptr, endian_order order);

template <endian_order From>
unsigned short	byte_to_ushort(byte_t const* ptr);
unsigned short  byte_to_ushort(byte_t const* ptr, endian_order order);

template <endian_order From>
int				byte_to_int(byte_t const* ptr);
int             byte_to_int(byte_t const* ptr, endian_order order);

template <endian_order From>
uint32_t		byte_to_uint(byte_t const* ptr);
uint32_t        byte_to_uint(byte_t const* ptr, endian_order order);

template <endian_order From>
int64_t			byte_to_int64(byte_t const* ptr);
int64_t         byte_to_int64(byte_t const* ptr, endian_order order);

template <endian_order From>
uint64_t		byte_to_uint64(byte_t const* ptr);
uint64_t        byte_to_uint64(byte_t const* ptr, endian_order order);

/**
 * Float conversion with or without swap endian
 */
template <endian_order From>
float			byte_to_float(byte_t const* ptr);
float           byte_to_float(byte_t const* ptr, endian_order order);

template <endian_order From>
double			byte_to_double( byte_t const* ptr );
double          byte_to_double(byte_t const* ptr, endian_order order);


/**
 * Inverse integer conversion with or without swap endian
 */
template <endian_order From>
void			short_to_byte(short value, byte_t* outPtr);
void			short_to_byte(short value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			ushort_to_byte(unsigned short value, byte_t* outPtr);
void			ushort_to_byte(unsigned short value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			int_to_byte(int value, byte_t* outPtr);
void			int_to_byte(int value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			int64_to_byte(int64_t value, byte_t* outPtr);
void			int64_to_byte(int64_t value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			uint64_to_byte(uint64_t value, byte_t* outPtr);
void			uint64_to_byte(uint64_t value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			double_to_byte(double value, byte_t* outPtr);
void			double_to_byte(double value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			float_to_byte(float value, byte_t* outPtr);
void			float_to_byte(float value, byte_t* outPtr, endian_order order);

template <segy_data_format From>
seismic_variant_vector  segy_data_to_native(const byte_t *buffer, int buffer_size, endian_order order);


template <segy_data_format To>
void                    native_to_segy_data(const byte_t *buffer, int buffer_size, endian_order order);