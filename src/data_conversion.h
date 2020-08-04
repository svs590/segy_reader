#pragma once

#include <string>

#include "data_types.h"
#include "segy_file.h"
#include "common.h"


endian_order native_order();

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
char			byte_to_char(byte_t const* ptr, endian_order order);

int8_t			byte_to_int8_t(byte_t const* ptr, endian_order order);
uint8_t			byte_to_uint8_t(byte_t const* ptr, endian_order order);

template <endian_order From>
short			byte_to_short(byte_t const* ptr);
short			byte_to_short(byte_t const* ptr, endian_order order);
#define         byte_to_int16_t byte_to_short

template <endian_order From>
unsigned short	byte_to_ushort(byte_t const* ptr);
unsigned short  byte_to_ushort(byte_t const* ptr, endian_order order);
#define         byte_to_uint16_t byte_to_ushort

template <endian_order From>
int				byte_to_int(byte_t const* ptr);
int             byte_to_int(byte_t const* ptr, endian_order order);
#define         byte_to_int32_t byte_to_int

template <endian_order From>
uint32_t		byte_to_uint(byte_t const* ptr);
uint32_t        byte_to_uint(byte_t const* ptr, endian_order order);
#define         byte_to_uint32_t byte_to_uint

template <endian_order From>
int64_t			byte_to_int64(byte_t const* ptr);
int64_t         byte_to_int64(byte_t const* ptr, endian_order order);
#define         byte_to_int64_t byte_to_int64

template <endian_order From>
uint64_t		byte_to_uint64(byte_t const* ptr);
uint64_t        byte_to_uint64(byte_t const* ptr, endian_order order);
#define         byte_to_uint64_t byte_to_uint64

/**
 * Float conversion with or without swap endian
 */
template <endian_order From>
float			byte_to_float(byte_t const* ptr);
float           byte_to_float(byte_t const* ptr, endian_order order);

template <endian_order From>
double			byte_to_double( byte_t const* ptr );
double          byte_to_double(byte_t const* ptr, endian_order order);

nullptr_t       byte_to_nullptr_t(byte_t const* ptr, endian_order order);


/**
 * Inverse integer conversion with or without swap endian
 */
void			char_to_byte(char value, byte_t *ptr, endian_order order);

void			int8_t_to_byte(int8_t value, byte_t *ptr, endian_order order);
void			uint8_t_to_byte(uint8_t value, byte_t *ptr, endian_order order);

template <endian_order From>
void			short_to_byte(short value, byte_t* outPtr);
void			short_to_byte(short value, byte_t* outPtr, endian_order order);
#define         int16_t_to_byte short_to_byte

template <endian_order From>
void			ushort_to_byte(unsigned short value, byte_t* outPtr);
void			ushort_to_byte(unsigned short value, byte_t* outPtr, endian_order order);
#define         uint16_t_to_byte ushort_to_byte

template <endian_order From>
void			int_to_byte(int value, byte_t* outPtr);
void			int_to_byte(int value, byte_t* outPtr, endian_order order);
#define         int32_t_to_byte int_to_byte

template <endian_order From>
void			uint_to_byte(uint32_t value, byte_t* outPtr);
void			uint_to_byte(uint32_t value, byte_t* outPtr, endian_order order);
#define         uint32_t_to_byte uint_to_byte

template <endian_order From>
void			int64_to_byte(int64_t value, byte_t* outPtr);
void			int64_to_byte(int64_t value, byte_t* outPtr, endian_order order);
#define         int64_t_to_byte int64_to_byte

template <endian_order From>
void			uint64_to_byte(uint64_t value, byte_t* outPtr);
void			uint64_to_byte(uint64_t value, byte_t* outPtr, endian_order order);
#define         uint64_t_to_byte uint64_to_byte

template <endian_order From>
void			double_to_byte(double value, byte_t* outPtr);
void			double_to_byte(double value, byte_t* outPtr, endian_order order);

template <endian_order From>
void			float_to_byte(float value, byte_t* outPtr);
void			float_to_byte(float value, byte_t* outPtr, endian_order order);

void			nullptr_t_to_byte(nullptr_t value, byte_t* outPtr, endian_order order);

template <segy_data_format From>
seismic_variant_vector  segy_data_to_native(const byte_t *buffer, int buffer_size, endian_order order);


template <segy_data_format To>
std::vector<byte_t>     native_to_segy_data(seismic_variant_vector data, endian_order order);

seismic_variant_value byte_to_seismic_variant_value(
    seismic_data_type       type,
    const byte_t            *ptr,
    endian_order            endian
);

void seismic_variant_value_to_byte(
    seismic_data_type       type,
    seismic_variant_value   &val,
    byte_t                  *ptr,
    endian_order            endian
);

void swap_endian(
    seismic_data_type       type,
    byte_t                  *ptr,
    endian_order            old_endian,
    endian_order            new_endian
);