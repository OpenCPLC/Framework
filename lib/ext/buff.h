#ifndef BUFF_H_
#define BUFF_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "heap.h"

// Max number of messages
#ifndef BUFF_MESSAGE_LIMIT
  #define BUFF_MESSAGE_LIMIT 32
#endif

//-------------------------------------------------------------------------------------------------

/**
 * @brief Configuration parameters (must be set during initialization):
 * @param memory Pointer to buffer memory
 * @param size Buffer size in bytes
 * @param console_mode Enable console input mode (interprets ESC, Enter, Ctrl+C, etc.)
 * @param Overflow (Optional) Overflow handler function
 */
typedef struct {
  uint8_t *memory;                        // Buffer memory start
  uint16_t size;                          // Buffer size in bytes
  bool console_mode;                      // Console mode: enable special key handling
  uint8_t esc;                            // ESC sequence state (internal)
  void (*Overflow)(void);                 // Overflow handler, called on overflow (optional)
  uint8_t *end_memory;                    // Pointer to memory + size (set by init)
  uint8_t *tail;                          // Read pointer (set by init)
  uint8_t *head;                          // Write pointer (set by init)
  uint8_t *echo;                          // Echo pointer (set by init)
  uint16_t msg_counter;                   // Bytes in current message (internal)
  uint16_t msg_size[BUFF_MESSAGE_LIMIT];  // Size of each message (internal)
  uint16_t msg_head;                      // Message head index (set by init)
  uint16_t msg_tail;                      // Message tail index (set by init)
  bool break_allow;                       // Allow message break (internal)
} BUFF_t;

void BUFF_Init(BUFF_t *buff);                       // Init buffer
bool BUFF_Break(BUFF_t *buff);                      // End current message
uint16_t BUFF_Size(BUFF_t *buff);                   // Current message size
bool BUFF_Append(BUFF_t *buff, uint8_t value);      // Add byte
bool BUFF_Echo(BUFF_t *buff, char *value);          // Peek buffer input (no pop)
bool BUFF_Pop(BUFF_t *buff, uint8_t *value);        // Remove last byte
bool BUFF_Push(BUFF_t *buff, uint8_t value);        // Add byte + break on '\n'
uint16_t BUFF_Array(BUFF_t *buff, uint8_t *array);  // Copy message to array
bool BUFF_Skip(BUFF_t *buff);                       // Skip message
char *BUFF_String(BUFF_t *buff);                    // Message as string
void BUFF_Clear(BUFF_t *buff);                      // Clear all messages

//-------------------------------------------------------------------------------------------------
#endif
