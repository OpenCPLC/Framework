#include "buff.h"
#include "stream.h"

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
bool BUFF_Break(BUFF_t *buff)
{
  if(!buff->break_allow) return false;
  buff->msg_size[buff->msg_head] = buff->msg_counter;
  buff->msg_counter = 0;
  buff->msg_head++;
  if(buff->msg_head >= BUFF_MESSAGE_LIMIT) buff->msg_head = 0;
  buff->break_allow = false;
  return true;
}

bool BUFF_BreakConsole(BUFF_t *buff)
{
  if(BUFF_Break(buff)) {
    if(buff->console->cursor) buff->console->cursor = 0;
    buff->console->Enter();
    return true;
  }
  return false;
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
 * @brief Dodaje bajt 'value' do bufora kołowego.
 * @param buff Wskaźnik do struktury bufora.
 * @param value Wartość do dodania.
 */
static inline void BUFF_Add(BUFF_t *buff, uint8_t value)
{
  *buff->head = value;
  buff->msg_counter++;
  buff->head++;
  if(buff->head >= buff->end) buff->head = buff->mem;
  if(buff->head == buff->tail) BUFF_Skip(buff);
  buff->break_allow = true;
}

bool BUFF_Pop(BUFF_t *buff)
{
  if(buff->msg_counter) {
    buff->msg_counter--;
    if(!buff->msg_counter) buff->break_allow = false;
    if(buff->head == buff->mem) buff->head = buff->end;
    buff->head--;
    return true;
  }
  return false;
}

static uint16_t BUFF_LoadHistory(BUFF_t *buff, int16_t index)
{
  if(index < 0) index = 0;
  uint16_t msg = buff->msg_tail;
  uint16_t limit = BUFF_MESSAGE_LIMIT;
  uint8_t *p = buff->tail;
  uint8_t *data = NULL;
  int16_t cursor = 0;
  while(index) {
    if(!msg) msg = BUFF_MESSAGE_LIMIT - 1;
    else msg--;
    p -= buff->msg_size[msg];
    if(p < buff->mem) data += buff->size;
    if(buff->msg_history[msg]) {
      data = p;
      index--;
      cursor++;
    }
    limit--;
    if(!limit) return cursor;
  }
  if(!data) return cursor;
  uint16_t size = buff->msg_size[msg];
  buff->console->Echo('\r');
  buff->console->Echo(0x1B);
  buff->console->Echo('[');
  buff->console->Echo('2');
  buff->console->Echo('K');
  BUFF_Break(buff);
  BUFF_Skip(buff);
  buff->console->Run(true);
  buff->console->Run(false);
  while(size) {
    BUFF_Add(buff, *data);
    buff->console->Echo(*data);
    data++;
    if(data >= buff->end) data = buff->mem;
    size--;
  }
  return cursor;
}

static bool BUFF_ConsoleMode(BUFF_t *buff, uint8_t value)
{
  if(buff->console->esc) {
    if(buff->console->esc == 2) {
      if(value == 'A') buff->console->cursor = BUFF_LoadHistory(buff, ++buff->console->cursor);
      else if(value == 'B') buff->console->cursor = BUFF_LoadHistory(buff, --buff->console->cursor);
      buff->console->esc = 0;
    }
    if(value == '[') buff->console->esc = 2;
    else buff->console->esc = 0;
    return true;
  }
  if(!buff->console) return false;
  if(value == '\r' || value == '\n') {
    if(buff->break_allow) buff->console->Execute();
    if(BUFF_BreakConsole(buff)) buff->console->Run(true);
    return true;
  }
  if(value == '\f' || value == 0x03) {
    if(!buff->break_allow) return true;
    buff->console->Kill();
    if(BUFF_BreakConsole(buff)) {
      BUFF_Skip(buff);
      buff->console->Run(true);
    }
    return true;
  }
  if(value == '\b' || value == 0x7F) {
    if(BUFF_Pop(buff)) buff->console->Echo(value);
    return true;
  }
  if(value == 0x13) {
    buff->console->Stop();
    return true;
  }
  if(value == 0x1B) {
    buff->console->esc = 1;
    return true;
  }
  buff->console->Run(false);
  buff->console->Echo(value);
  return false;
}

/**
 * @brief Dodaje bajt 'value' do bufora kołowego.
 * @param buff Wskaźnik do struktury bufora.
 * @param value Wartość do dodania.
 */
inline void BUFF_Push(BUFF_t *buff, uint8_t value)
{
  if(BUFF_ConsoleMode(buff, value)) return;
  BUFF_Add(buff, value);
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
  if(!size) return 0;
  buff->msg_history[buff->msg_tail] = array ? true : false;
  uint16_t n = size;
  while(n) {
    if(array) {
      *array = *buff->tail;
      array++;
    }
    buff->tail++;
    if(buff->tail >= buff->end) buff->tail = buff->mem;
    n--;
  }
  buff->msg_tail++;
  if(buff->msg_tail >= BUFF_MESSAGE_LIMIT) buff->msg_tail = 0;
  buff->msg_size[buff->msg_tail] = 0;
  return size;
}

/**
 * @brief Pomija bieżącą wiadomość w buforze.
 * @param buff Wskaźnik do struktury bufora.
 * @return Zwraca 'true', jeżeli wiadomość zostałą pominięta/
 */
bool BUFF_Skip(BUFF_t *buff)
{
  return BUFF_Array(buff, NULL) ? true : false;
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
 * @brief Czyści cały bufor przez pomijanie wiadomości, dopóki nie ma więcej wiadomości.
 * @param buff Wskaźnik do struktury bufora.
 */
void BUFF_Clear(BUFF_t *buff)
{
  while(BUFF_Skip(buff));
}

//-------------------------------------------------------------------------------------------------
