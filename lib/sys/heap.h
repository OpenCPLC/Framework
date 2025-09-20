#ifndef HEAP_H_
#define HEAP_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef HEAP_SIZE
  #define HEAP_SIZE 8192
#endif

#ifndef HEAP_ALIGN
  #define HEAP_ALIGN 8
#endif

/**
 * @brief Heap memory block structure.
 * @param size Size of the allocated data area (in bytes), not including the header
 * @param next Pointer to next `heap_block_t` in `FreeList`
 * @param free `true` if free, `false` if in use
 */
typedef struct heap_block {
  size_t size;
  struct heap_block  *next;
  bool free;
} heap_block_t;

void heap_init(void);
void *heap_alloc(size_t size);
void *heap_reloc(void *ptr, size_t size);
void heap_free(void *ptr);

//-------------------------------------------------------------------------------------------------

#ifndef HEAP_INCLUDE_VRTS
  #define HEAP_INCLUDE_VRTS 1
#endif

#if(HEAP_INCLUDE_VRTS)
  #include"vrts.h"
#endif

#ifndef HEAP_NEW_BLOCK
  #define HEAP_NEW_BLOCK 16
#endif

/**
 * @brief Memory stack for garbage collector, one per thread.
 * @param var Pointers to dynamically allocated variables
 * @param count Current number of variables in use
 * @param limit Current allocated capacity of `var` (auto-expands)
 */
typedef struct {
  void **var;
  uint16_t count;
  uint16_t limit;
} heap_new_t;

void *heap_new(size_t size);
void heap_clear(void);

//-------------------------------------------------------------------------------------------------
#endif

// #include <stdio.h>
// #include <string.h>
// #include "heap.h"

// // Prosta funkcja pomocnicza do wypisywania wyników testu
// static void test(const char *name, int cond)
// {
//   printf("[%-30s] %s\n", name, cond ? "OK" : "FAIL");
// }

// int main(void)
// {
//   printf("===== HEAP / GC TEST START =====\n");

//   // 1. Inicjalizacja sterty
//   heap_init();
//   printf("Heap initialized with size = %d bytes (aligned %d)\n",
//          HEAP_SIZE, HEAP_ALIGN);

//   // 2. Test heap_alloc
//   void *a = heap_alloc(32);
//   test("heap_alloc(32)", a != NULL);

//   // 3. Test heap_reloc (powiększanie)
//   void *a2 = heap_reloc(a, 64);
//   test("heap_reloc grow (32 -> 64)", a2 != NULL && a2 != a);

//   // 4. Test heap_reloc (zmniejszanie)
//   void *a3 = heap_reloc(a2, 16);
//   test("heap_reloc shrink (64 -> 16)", a3 == a2);

//   // 5. Test heap_free
//   heap_free(a3);
//   test("heap_free", 1); // brak crasha = OK

//   // 6. Test heap_new (alokacje śledzone)
//   void *n1 = heap_new(24);
//   void *n2 = heap_new(40);
//   test("heap_new allocations", n1 && n2);

//   // 7. Sprawdź czy stos GC się rozszerza
//   for(int i = 0; i < 20; i++) {
//     void *tmp = heap_new(8);
//     if(!tmp) {
//       printf("heap_new failed at iteration %d\n", i);
//       break;
//     }
//   }
//   test("heap_new auto-expand", 1); // brak crasha = OK

//   // 8. Wyzerowanie GC stacka
//   heap_clear();
//   test("heap_clear", 1);

//   // 9. Edge-case: heap_new(0)
//   void *n3 = heap_new(0);
//   test("heap_new(0)", n3 == NULL);

//   // 10. Edge-case: heap_free(NULL)
//   heap_free(NULL);
//   test("heap_free(NULL)", 1);

//   printf("===== HEAP / GC TEST END =====\n");
//   return 0;
// }