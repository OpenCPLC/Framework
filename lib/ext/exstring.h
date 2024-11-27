#ifndef EXSTRING_H_
#define EXSTRING_H_

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include "new.h"

//-------------------------------------------------------------------------------------------------

typedef enum {
  CUT_BeforeFirst,
  CUT_BeforeLast,
  CUT_AfterFirst,
  CUT_AfterLast
} CUT_e;

extern const char LowerCase[];
extern const char UpperCase[];

//-------------------------------------------------------------------------------------------------

uint8_t itoa_base(int32_t nbr, char *str, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space);
char *itoa_int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space); // Convert integer to string with base
char *itoa_dec(int32_t nbr); // Convert signed decimal integer to string
char *itoa_udec(uint32_t nbr); // Convert unsigned decimal integer to string
char *itoa_hex8(uint32_t nbr); // Convert integer to 2-digit hexadecimal string
char *itoa_hex16(uint32_t nbr); // Convert integer to 4-digit hexadecimal string
char *itoa_hex32(uint32_t nbr); // Convert integer to 8-digit hexadecimal string

bool str2uint16_fault(const char *str); // Validate if string can be converted to uint16_t number
bool str2int16_fault(const char *str); // Validate if string can be converted to int16_t number
bool str2uint32_fault(const char *str); // Validate if string can be converted to uint32_t number
bool str2int32_fault(const char *str); // Validate if string can be converted to int32_t number
uint32_t str2nbr(const char *str); // Convert string to uint16_t|int16_t|uint32_t|int32_t
bool str2uint64_fault(const char *str); // Validate if string can be converted to uint64_t number
uint32_t str2nbr64(const char *str); // Convert string to uint64_t

bool str2float_fault(const char *str); // Validate if string can be converted to float
float str2float(const char *str); // Convert string to float
bool str2unitfloat_fault(const char *str); // Validate string with unit for conversion
float str2unitfloat(const char *str); // Convert string with unit to float

uint32_t hash(const char *str); // Compute hash of a string
char *substr(char *str, int16_t start, int16_t length); // Extract substring from a string
char *joinstr(unsigned int count, const char *str, ...); // Join multiple strings
char *strcopy(const char *str); // Copy string to newly allocated memory
char *str2lower_this(char *str); // Convert string to lowercase in-place
char *str2upper_this(char *str); // Convert string to uppercase in-place
char *str2lower(char *str); // Convert string to lowercase, returning a copy
char *str2upper(char *str); // Convert string to uppercase, returning a copy
char *reverse_this_string(char *str); // Reverse string in-place
char *reverse_string(const char *str_in); // Reverse string, returning a copy

int find(const char *str, const char *pattern); // Find first occurrence of pattern in string
int find_right(const char *str, const char *pattern); // Find last occurrence of pattern in string
char *cut_this(char *str, const char *pattern, CUT_e mode); // Modify string by cutting at pattern
char *cut(const char *str_in, const char *pattern, CUT_e mode); // Cut string and return a modified copy

char *ltrim_list(char *str, const char *list); // Trim specified characters from the start
char *ltrim(char *str); // Trim whitespace from the start
char *rtrim_list(char *str, const char *list); // Trim specified characters from the end
char *rtrim(char *str); // Trim whitespace from the end
char *trim_list(char *str, const char *list); // Trim specified characters from both ends
char *trim(char *str); // Trim whitespace from both ends

char *extraction(const char *str, char delimiter, int position); // Extract substring by delimiter and position
int explode(char ***arr_ptr, const char *str, char delimiter); // Split string by delimiter into array
char *replace_this_char(const char *pattern, char replacement, char *str); // Replace characters in-place
char *replace_char(const char *pattern, char replacement, const char *str); // Replace characters, returning a copy
char *replace_string(const char *pattern, const char *replacement, const char *original); // Replace substring, returning a copy

//-------------------------------------------------------------------------------------------------

#endif
