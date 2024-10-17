#include "buff.h"

//-------------------------------------------------------------------------------------------------

/** 
 * @brief Inicjalizuje bufor kołowy.
 * @param buff Wskaźnik do struktury bufora.
 */
inline void BUFF_Init(BUFF_t *buff)
{
  buff->head = buff->mem;
  buff->tail = buff->mem;
  buff->end = buff->mem + buff->size;
  buff->msg_head = 0;
  buff->msg_tail = 0;
}

/** 
 * @brief Oznacza koniec bieżącej wiadomości, jeśli ustawiona jest przerwa.
 * @param buff Wskaźnik do struktury bufora.
 */
inline void BUFF_Break(BUFF_t *buff)
{
  if(buff->break_allow) {
    buff->msg_size[buff->msg_head] = buff->msg_counter;
    buff->msg_counter = 0;
    buff->msg_head++;
    if(buff->msg_head >= BUFF_MESSAGE_LIMIT) buff->msg_head = 0;
    buff->break_allow = false;
  }
}

/** 
 * @brief Dodaje bajt `value` do bufora kołowego.
 * @param buff Wskaźnik do struktury bufora.
 * @param value Wartość do dodania.
 */
inline void BUFF_Push(BUFF_t *buff, uint8_t value)
{
  *buff->head = value;
  buff->msg_counter++;
  buff->head++;
  if(buff->head >= buff->end) buff->head = buff->mem;
  // if(buff->head == buff->tail) HardFault_Handler();
  buff->break_allow = true;
  if(buff->newline_break && value == '\n') {
    BUFF_Break(buff);
  }
  
}

/** 
 * @brief Zwraca rozmiar bieżącej wiadomości w buforze.
 * @param buff Wskaźnik do struktury bufora.
 * @return Rozmiar bieżącej wiadomości.
 */
uint16_t BUFF_Size(BUFF_t *buff)
{
  if(buff->msg_head != buff->msg_tail) return buff->msg_size[buff->msg_tail];
  return 0;
}

/** 
 * @brief Kopiuje zawartość bieżącej wiadomości do zewnętrznej tablicy.
 * @param buff Wskaźnik do struktury bufora.
 * @param array Wskaźnik do tablicy docelowej.
 * @return Rozmiar skopiowanej wiadomości.
 */
uint16_t BUFF_Array(BUFF_t *buff, uint8_t *array)
{
  uint16_t size = BUFF_Size(buff);
  if(size) {
    while(buff->msg_size[buff->msg_tail]) {
      *array = *buff->tail;
      array++;
      buff->tail++;
      buff->msg_size[buff->msg_tail]--;
      if(buff->tail >= buff->end) buff->tail = buff->mem;
    }
    buff->msg_tail++;
    if(buff->msg_tail >= BUFF_MESSAGE_LIMIT) buff->msg_tail = 0;
  }
  return size;
}

/** 
 * @brief Tworzy dynamiczny ciąg znaków z zawartością bieżącej wiadomości w buforze.
 * @param buff Wskaźnik do struktury bufora.
 * @return Wskaźnik do dynamicznie alokowanego ciągu znaków.
 */
char *BUFF_String(BUFF_t *buff)
{
  char *string = NULL;
  uint16_t size = BUFF_Size(buff);
  if(size) {
    string = new(size + 1);
    BUFF_Array(buff, (uint8_t *)string);
    string[size] = 0;
  }
  return string;
}

/** 
 * @brief Pomija bieżącą wiadomość w buforze.
 * @param buff Wskaźnik do struktury bufora.
 * @return Zwraca `true`, jeżeli wiadomość zostałą pominięta/
 */
bool BUFF_Skip(BUFF_t *buff)
{
  uint16_t size = BUFF_Size(buff);
  if(size) {
    while(buff->msg_size[buff->msg_tail]) {
      buff->tail++;
      buff->msg_size[buff->msg_tail]--;
      if(buff->tail >= buff->end) buff->tail = buff->mem;
    }
    buff->msg_tail++;
    if(buff->msg_tail >= BUFF_MESSAGE_LIMIT) buff->msg_tail = 0;
    return true;
  }
  return false;
}

/** 
 * @brief Czyści cały bufor przez pomijanie wiadomości, dopóki nie ma więcej wiadomości.
 * @param buff Wskaźnik do struktury bufora.
 */
void BUFF_Clear(BUFF_t *buff)
{
  while(BUFF_Skip(buff));
}

//-------------------------------------------------------------------------------------------------
