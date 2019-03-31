#pragma once

#include <string>

#include "data_types.h"

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
inline int				byte_to_short_swap(byte_t const* ptr);
inline int				byte_to_short(byte_t const* ptr);
inline unsigned short	byte_to_ushort_swap(byte_t const* ptr);
inline unsigned short	byte_to_ushort(byte_t const* ptr);
inline int				byte_to_int(byte_t const* ptr);
inline uint64_t			byte_to_uint64(byte_t const* ptr);
inline int				byte_to_int_swap(byte_t const* ptr);

/**
 * Float conversion with or without swap endian
 */
inline float			byte_to_float(byte_t const* ptr);
//inline double			byte_to_double( byte_t const* ptr );
inline float			byte_to_float_swap(byte_t const* ptr);
//inline double			byte_to_double_swap( byte_t const* ptr );


/**
 * Inverse integer conversion with or without swap endian
 */
inline void				short_to_byte_swap(short value, byte_t* outPtr);
inline void				short_to_byte(short value, byte_t* outPtr);
inline void				int_to_byte(int value, byte_t* outPtr);
inline void				int_to_byte_swap(int value, byte_t* outPtr);
inline void				float_to_byte_swap(float value, byte_t* outPtr);
inline void				float_to_byte(float value, byte_t* outPtr);