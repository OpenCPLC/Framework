#include "ary.h"

// /**
//  * @brief Initializes the array with a user-provided buffer.
//  * @param arr Pointer to the ary_t structure.
//  * @param buffer Pointer to the pre-allocated buffer.
//  * @param limit Buffer capacity (number of elements).
//  * @param element_size Size of the element type (in bytes).
//  */
// inline void ary_init(ary_t *arr, void *buffer, uint16_t limit, uint16_t element_size)
// {
//   arr->value = buffer;
//   arr->count = 0;
//   arr->limit = limit;
//   arr->element_size = element_size;
// }

/**
 * @brief Clears the array (resets count to zero).
 * @param arr Pointer to the ary_t structure.
 */
inline void ary_clear(ary_t *arr)
{
  arr->count = 0;
}

/**
 * @brief Removes the last element from the array and copies its value out.
 * @param arr Pointer to the ary_t structure.
 * @param out Pointer to memory where the removed element will be copied (can be NULL to ignore value).
 * @return true if an element was removed, false if array was already empty.
 */
bool ary_pop(ary_t *arr, void *out)
{
  if(arr->count == 0) return false;
  arr->count--;
  if(out) {
    uint8_t *src = (uint8_t*)arr->value + arr->count * arr->element_size;
    memcpy(out, src, arr->element_size);
  }
  return true;
}

/**
 * @brief Appends an element to the end of the array.
 * @param ary Pointer to the ary_t structure.
 * @param element Pointer to the new element to add.
 * @return true on success, false if array is full.
 */
bool ary_push(ary_t *ary, const void *element)
{
  if(ary->count >= ary->limit) return false;
  uint8_t *dest = (uint8_t*)ary->value + (ary->count * ary->element_size);
  memcpy(dest, element, ary->element_size);
  ary->count++;
  return true;
}

/**
 * @brief Removes the first element from the array and copies its value out.
 * @param ary Pointer to the ary_t structure.
 * @param out Pointer to memory where the removed element will be copied (can be NULL).
 * @return true on success, false if array is empty.
 */
bool ary_shift(ary_t *ary, void *out)
{
  if(ary->count == 0) return false;
  uint8_t *base = (uint8_t*)ary->value;
  if(out) {
    memcpy(out, base, ary->element_size);
  }
  memmove(base, base + ary->element_size, (ary->count - 1) * ary->element_size);
  ary->count--;
  return true;
}

/**
 * @brief Inserts an element at the beginning of the array.
 * @param ary Pointer to the ary_t structure.
 * @param element Pointer to the new element to add.
 * @return true on success, false if array is full.
 */
bool ary_unshift(ary_t *ary, const void *element)
{
  if(ary->count >= ary->limit) return false;
  uint8_t *base = (uint8_t*)ary->value;
  memmove(base + ary->element_size, base, ary->count * ary->element_size);
  memcpy(base, element, ary->element_size);
  ary->count++;
  return true;
}

/**
 * @brief Returns a pointer to the element at the given index.
 * @param ary Pointer to the ary_t structure.
 * @param index Element index.
 * @return Pointer to the element, or NULL if out of range.
 */
void *ary_get(ary_t *ary, uint16_t index)
{
  if(index >= ary->count) {
    return NULL;
  }
  return (uint8_t*)ary->value + (index * ary->element_size);
}

/**
 * @brief Overwrites the element at the given index.
 * @param ary Pointer to the ary_t structure.
 * @param index Index of the element to overwrite.
 * @param element Pointer to the new value.
 * @return true on success, false if index is out of range.
 */
bool ary_set(ary_t *ary, uint16_t index, const void *element)
{
  if(index >= ary->count) {
    return false;
  }
  uint8_t *dest = (uint8_t*)ary->value + (index * ary->element_size);
  memcpy(dest, element, ary->element_size);
  return true;
}

/**
 * @brief Inserts a new element at the given index, shifting elements right.
 * @param ary Pointer to the ary_t structure.
 * @param index Index at which to insert the new element.
 * @param element Pointer to the new value.
 * @return true on success, false if array is full or index is out of range.
 */
bool ary_insert(ary_t *ary, uint16_t index, const void *element)
{
  if(index > ary->count || ary->count >= ary->limit) return false;
  uint8_t *base = (uint8_t*)ary->value;
  memmove(base + (index + 1) * ary->element_size, base + index * ary->element_size, (ary->count - index) * ary->element_size);
  memcpy(base + index * ary->element_size, element, ary->element_size);
  ary->count++;
  return true;
}

/**
 * @brief Removes the element at the given index and copies its value out.
 * @param ary Pointer to the ary_t structure.
 * @param index Index of the element to remove.
 * @param out Pointer to memory where the removed element will be copied (can be NULL).
 * @return true on success, false if index is out of range.
 */
bool ary_remove(ary_t *ary, uint16_t index, void *out)
{
  if(index >= ary->count) return false;
  uint8_t *base = (uint8_t*)ary->value;
  if(out) {
    memcpy(out, base + index * ary->element_size, ary->element_size);
  }
  memmove(base + index * ary->element_size, base + (index + 1) * ary->element_size, (ary->count - index - 1) * ary->element_size);
  ary->count--;
  return true;
}

//-------------------------------------------------------------------------------------------------

// #include "ary.h"
// #include <stdio.h>

// int main(void) {
//   // Declare a static int array with up to 5 elements
//   ary_new(myArr, int, 5);
//   // Add elements
//   int val = 42;
//   ary_push(&myArr, &val);
//   val = 99;
//   ary_push(&myArr, &val);
//   // Access and modify elements
//   int *ptr = (int*)ary_get(&myArr, 1);
//   if(ptr) {
//     printf("Element[1] = %d\n", *ptr);  // prints 99
//   }
//   val = 123;
//   ary_set(&myArr, 0, &val); // change first element to 123
//   // Insert and remove
//   val = 55;
//   ary_insert(&myArr, 1, &val); // [123, 55, 99]
//   ary_remove(&myArr, 2);       // removes 99, leaves [123, 55]
//   // Print all elements
//   for(uint16_t i = 0; i < myArr.count; i++) {
//     int *elem = (int*)ary_get(&myArr, i);
//     printf("myArr[%d] = %d\n", i, *elem);
//   }
//   return 0;
// }
