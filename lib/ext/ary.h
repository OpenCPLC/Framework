#ifndef ARY_H
#define ARY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/**
 * @brief Generic array structure.
 * @param value Pointer to element buffer.
 * @param count Current number of elements.
 * @param limit Maximum capacity (elements).
 * @param element_size Size of single element (in bytes).
 */
typedef struct {
  void *value;
  uint16_t count;
  uint16_t limit;
  uint16_t element_size;
} ary_t;

/**
 * @brief Helper macro for static array declaration.
 * Example: `ary_new(myArr, int, 10);`
 * Declares: `int myArr_storage[10]; ary_t myArr = { myArr_storage, 0, 10, sizeof(int) };`
 * @param Name Name of the `ary_t` variable.
 * @param Type Data type of the elements.
 * @param Limit Array capacity (number of elements).
 */
#define ary_new(Name, Type, Limit) \
  Type Name##_storage[Limit]; \
  ary_t Name = { .value = Name##_storage, .count = 0, .limit = Limit, .element_size = sizeof(Type) }

/**
 * @brief Helper macro for static array declaration with initial values.
 * Example: `ary_init_values(myArr, int, 10, {1, 2, 3});`
 * Declares: `int myArr_storage[10] = {1, 2, 3}; ary_t myArr = { myArr_storage, 3, 10, sizeof(int) };`
 * @param Name Name of the ary_t variable.
 * @param Type Data type of the elements.
 * @param Limit Array capacity (number of elements).
 * @param ... Initial values in braces (e.g. `{1,2,3}`).
 */
#define ary_init(Name, Type, Limit, ...) \
  Type Name##_storage[Limit] = __VA_ARGS__; \
  ary_t Name = { \
    .value = Name##_storage, \
    .count = (uint16_t)(sizeof((Type[])__VA_ARGS__) / sizeof(Type)), \
    .limit = (Limit), \
    .element_size = sizeof(Type) \
  }

/**
 * @brief Declares a constant array and a constant ary_t structure with fixed length.
 * Example: `ary_init_const(myArr, int, {1, 2, 3});`
 * Declares: `const int myArr_storage[3] = {1, 2, 3}; const ary_t myArr = {myArr_storage, 4, 4, sizeof(int)};`
 * @param Name Name of the ary_t variable.
 * @param Type Data type.
 * @param ... Initial values in braces (e.g. `{1, 2, 3}`).
 */
#define ary_const(Name, Type, ...) \
  const Type Name##_storage[] = __VA_ARGS__; \
  enum { Name##_limit = sizeof(Name##_storage)/sizeof(Type) }; \
  const ary_t Name = { \
    .value = (void *)Name##_storage, \
    .count = Name##_limit, \
    .limit = Name##_limit, \
    .element_size = sizeof(Type) \
  }

void ary_clear(ary_t *arr);
bool ary_pop(ary_t *arr, void *out);
bool ary_push(ary_t *ary, const void *element);
bool ary_shift(ary_t *ary, void *out);
bool ary_unshift(ary_t *ary, const void *element);
void *ary_get(ary_t *ary, uint16_t index);
bool ary_set(ary_t *ary, uint16_t index, const void *element);
bool ary_insert(ary_t *ary, uint16_t index, const void *element);
bool ary_remove(ary_t *ary, uint16_t index, void *out);

#endif