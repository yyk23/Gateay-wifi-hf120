/**
 * 
 * 
 */

#ifndef __RA_TYPES_H__
#define __RA_TYPES_H__
typedef enum
{
	RA_SUCCESS = 0,
	RA_ERROR_INTERNAL = -1,
	RA_ERROR_OUT_OF_MEMORY = -2,
	RA_ERROR_INVALID_ARGUMENTS = -3
} ra_error_t;


typedef unsigned char       ra_uint8_t;
typedef signed char         ra_int8_t;
typedef unsigned short      ra_uint16_t;
typedef signed short        ra_int16_t;
typedef unsigned long       ra_uint32_t;
typedef signed long         ra_int32_t;
typedef signed long long    ra_int64_t;
typedef unsigned long long  ra_uint64_t;
typedef float               real32_t;
typedef double              real64_t;
typedef ra_uint32_t 		ra_size_t;

typedef enum 
{
	ra_false = 0,
	ra_true
} ra_bool;
#endif