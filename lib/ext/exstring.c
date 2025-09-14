#include "exstring.h"

//-------------------------------------------------------------------------------------------------

static const char Int2Ascii[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * @brief Helper function to convert integer (64-bit) into reversed string using selected number base.
 * @param nbr Number to convert.
 * @param str Output buffer (string reversed).
 * @param base Numeric base (range 2–36).
 * @param sign Include sign (true = signed number, false = unsigned).
 * @param fill_zero Minimal digit count (filled with zeros).
 * @param fill_space Minimal string length (filled with spaces).
 * @return uint8_t Number of chars written to `str`.
 */
uint8_t itoa_base(int64_t nbr, char *str, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space)
{
  if(base < 2 || base > 36) return 0;
  uint8_t n = 0;
  bool is_negative = false;
  uint64_t unbr;
  if(!fill_zero) fill_zero = 1;
  if(fill_space < fill_zero) fill_space = fill_zero;
  if(sign && nbr < 0) {
    is_negative = true;
    unbr = (uint64_t)(-nbr);
  }
  else unbr = (uint64_t)nbr;
  do {
    str[n++] = Int2Ascii[unbr % base];
    unbr /= base;
  } while(unbr > 0);
  while(n < fill_zero - is_negative) {
    str[n++] = '0';
  }
  if(is_negative) str[n++] = '-';
  while(n < fill_space) str[n++] = ' ';
  return n;
}

/**
 * @brief Main function for converting 64-bit integer to string in selected number base.
 * @param nbr Number to convert.
 * @param base Numeric base (e.g. 10 for decimal, 16 for hex).
 * @param sign Include sign if true.
 * @param fill_zero Minimum number of digits (filled with zeros).
 * @param fill_space Minimum string length (filled with spaces).
 * @return char* Pointer to newly allocated string (must be freed manually).
 */
char *itoa_int64(int64_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space)
{
  char temp[65]; // Enough for 64-bit binary number + padding
  uint8_t len = itoa_base(nbr, temp, base, sign, fill_zero, fill_space);
  char *string = new(len + 1); // or malloc if using C
  if(!string) return NULL;
  for(uint8_t i = 0; i < len; i++) {
    string[i] = temp[len - 1 - i];
  }
  string[len] = '\0';
  return string;
}

/**
 * @brief Converts signed 64-bit integer to decimal string.
 * @param nbr Number to convert.
 * @return char* Pointer to result string (must be freed manually).
 */
char *itoa_dec(int64_t nbr)
{
  return itoa_int64(nbr, 10, true, 0, 0);
}

/**
 * @brief Converts unsigned 64-bit integer to decimal string.
 * @param nbr Number to convert.
 * @return char* Pointer to result string (must be freed manually).
 */
char *itoa_udec(uint64_t nbr)
{
  return itoa_int64((int64_t)nbr, 10, false, 0, 0);
}

/**
 * @brief Converts 8-bit unsigned number to hexadecimal string (2 digits).
 * @param nbr Value to convert.
 * @return char* Pointer to result string (must be freed manually).
 */
char *itoa_hex8(uint8_t nbr)
{
  return itoa_int64((int64_t)nbr, 16, false, 2, 2);
}

/**
 * @brief Converts 16-bit unsigned number to hexadecimal string (4 digits).
 * @param nbr Value to convert.
 * @return char* Pointer to result string (must be freed manually).
 */
char *itoa_hex16(uint16_t nbr)
{
  return itoa_int64((int64_t)nbr, 16, false, 4, 4);
}

/**
 * @brief Converts 32-bit unsigned number to hexadecimal string (8 digits).
 * @param nbr Value to convert.
 * @return char* Pointer to result string (must be freed manually).
 */
char *itoa_hex32(uint32_t nbr)
{
  return itoa_int64((int64_t)nbr, 16, false, 8, 8);
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na liczbę uint16_t.
 * Funkcja obsługuje liczby w systemie dziesiętnym, szesnastkowym (prefiks "0x") i binarnym (prefiks "0b").
 * Weryfikuje poprawność ciągu znaków oraz zakres liczby (maksymalnie 65535 dla uint16_t).
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny, pusty lub wykracza poza zakres uint16_t; false w przeciwnym razie.
 */
bool str2uint16_fault(const char *str) 
{
  if(!str || *str == '\0') return true;
  if(str[0] == '0' && str[1] == 'b' && str[2] != '\0') {
    str += 2;
    if(*str == '\0') return true;
    size_t bin_digits = 0;
    while(*str) {
      if(*str != '0' && *str != '1') return true;
      bin_digits++;
      str++;
    }
    if(bin_digits > 16) return true;
  }
  else if(str[0] == '0' && str[1] == 'x' && str[2] != '\0') {
    str += 2;
    size_t hex_digits = 0;
    while(*str) {
      if(!((*str >= '0' && *str <= '9') || (*str >= 'A' && *str <= 'F') || (*str >= 'a' && *str <= 'f'))) return true;
      hex_digits++;
      str++;
    }
    if(hex_digits > 4) return true;
  }
  else {
    size_t dec_digits = 0;
    while(*str) {
      if(*str < '0' || *str > '9') return true;
      dec_digits++;
      str++;
    }
    if(dec_digits > 5) return true;
    if(dec_digits == 5) {
      const char *start = str - dec_digits;
      if(strncmp(start, "65535", 5) > 0) return true;
    }
  }
  return false;
}

/**
 * @brief Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na liczbę int16_t.
 * Funkcja obsługuje liczby w systemie dziesiętnym. Weryfikuje poprawność ciągu znaków
 * oraz zakres liczby (od -32768 do 32767 dla int16_t).
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny, pusty lub wykracza poza zakres int16_t; false w przeciwnym razie.
 */
bool str2int16_fault(const char *str)
{
  if(!str || *str == '\0') return true;
  bool is_negative = false;
  if(*str == '-') {
    is_negative = true;
    str++;
  }
  size_t dec_digits = 0;
  while(*str) {
    if(*str < '0' || *str > '9') return true;
    dec_digits++;
    str++;
  }
  if(dec_digits > 5) return true;
  if(dec_digits == 5) {
    const char *start = str - dec_digits;
    if(!is_negative && strncmp(start, "32767", 5) > 0) return true;
    else if(is_negative && strncmp(start, "32768", 5) > 0) return true;
  }
  return false;
}

/**
 * @brief Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na liczbę uint32_t.
 * Funkcja obsługuje liczby w systemie dziesiętnym, szesnastkowym (prefiks "0x") i binarnym (prefiks "0b").
 * Weryfikuje poprawność ciągu znaków oraz zakres liczby (maksymalnie 4294967295 dla uint32_t).
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny, pusty lub wykracza poza zakres uint32_t; false w przeciwnym razie.
 */
bool str2uint32_fault(const char *str) 
{
  if(!str || *str == '\0') return true;
  if(str[0] == '0' && str[1] == 'b' && str[2] != '\0') {
    str += 2;
    if(*str == '\0') return true;
    size_t bin_digits = 0;
    while(*str) {
      if(*str != '0' && *str != '1') return true;
      bin_digits++;
      str++;
    }
    if(bin_digits > 32) return true;
  }
  else if(str[0] == '0' && str[1] == 'x' && str[2] != '\0') {
    str += 2;
    size_t hex_digits = 0;
    while(*str) {
      if(!((*str >= '0' && *str <= '9') || (*str >= 'A' && *str <= 'F') || (*str >= 'a' && *str <= 'f'))) return true;
      hex_digits++;
      str++;
    }
    if(hex_digits > 8) return true;
  }
  else {
    size_t dec_digits = 0;
    while(*str) {
      if(*str < '0' || *str > '9') return true;
      dec_digits++;
      str++;
    }
    if(dec_digits > 10) return true;
    if(dec_digits == 10) {
      const char *start = str - dec_digits;
      if(strncmp(start, "4294967295", 10) > 0) return true;
    }
  }
  return false;
}

/**
 * @brief Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na liczbę int32_t.
 * Funkcja obsługuje liczby w systemie dziesiętnym. Weryfikuje poprawność ciągu znaków
 * oraz zakres liczby (od -2147483648 do 2147483647 dla int32_t).
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny, pusty lub wykracza poza zakres int32_t; false w przeciwnym razie.
 */
bool str2int32_fault(const char *str) 
{
  if(!str || *str == '\0') return true;
  bool is_negative = false;
  if(*str == '-') {
    is_negative = true;
    str++;
  }
  size_t dec_digits = 0;
  while(*str) {
    if(*str < '0' || *str > '9') return true;
    dec_digits++;
    str++;
  }
  if(dec_digits > 10) return true;
  if(dec_digits == 10) {
    const char *start = str - dec_digits;
    if(!is_negative && strncmp(start, "2147483647", 10) > 0) return true;
    else if(is_negative && strncmp(start, "2147483648", 10) > 0) return true;
  }
  return false;
}

/**
 * @brief Konwertuje ciąg znaków na liczbę całkowitą.
 * Funkcja obsługuje formaty dziesiętne, szesnastkowe (prefiks "0x") i binarne (prefiks "0b").
 * Obsługiwane są zarówno liczby dodatnie, jak i ujemne (dla dziesiętnych).
 * Jeśli wynik wymaga typu int32_t, należy wykonać rzutowanie na odpowiedni typ.
 * @note Przed wywołaniem tej funkcji zaleca się użycie funkcji walidacji:
 * W zależności od typu: `str2uint16_fault` lub `str2int16_fault` lub `str2uint32_fault` lub `str2int32_fault`.
 * @param str Wskaźnik do ciągu znaków do konwersji.
 * @return Liczba całkowita uint32_t uzyskana z ciągu znaków.
 */
uint32_t str2nbr(const char *str) 
{
  int base = 10;
  bool is_negative = false;
  if(*str == '-') {
    is_negative = true;
    str++;
  }
  if(str[0] == '0' && str[1] == 'b') {
    base = 2;
    str += 2;
  } 
  else if(str[0] == '0' && str[1] == 'x') {
    base = 16;
    str += 2;
  }
  char *endptr;
  uint32_t result = strtol(str, &endptr, base);
  return is_negative ? -(int32_t)result : result;
}


/**
 * @brief Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na liczbę uint64_t.
 * Funkcja obsługuje liczby w systemie dziesiętnym, szesnastkowym (prefiks "0x") i binarnym (prefiks "0b").
 * Weryfikuje poprawność ciągu znaków oraz zakres liczby (maksymalnie 18446744073709551615 dla uint64_t).
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny, pusty lub wykracza poza zakres uint64_t; false w przeciwnym razie.
 */
bool str2uint64_fault(const char *str) 
{
  if(!str || *str == '\0') return true;
  if(str[0] == '0' && str[1] == 'b' && str[2] != '\0') {
    str += 2;
    if(*str == '\0') return true;
    size_t bin_digits = 0;
    while(*str) {
      if(*str != '0' && *str != '1') return true;
      bin_digits++;
      str++;
    }
    if(bin_digits > 64) return true;
  }
  else if(str[0] == '0' && str[1] == 'x' && str[2] != '\0') {
    str += 2;
    size_t hex_digits = 0;
    while(*str) {
      if(!((*str >= '0' && *str <= '9') || (*str >= 'A' && *str <= 'F') || (*str >= 'a' && *str <= 'f'))) return true;
      hex_digits++;
      str++;
    }
    if(hex_digits > 16) return true;
  }
  else {
    size_t dec_digits = 0;
    while(*str) {
      if(*str < '0' || *str > '9') return true;
      dec_digits++;
      str++;
    }
    if(dec_digits > 20) return true;
    if(dec_digits == 20) {
      const char *start = str - dec_digits;
      if(strncmp(start, "18446744073709551615", 20) > 0) return true;
    }
  }
  return false;
}

/**
 * @brief Konwertuje ciąg znaków na liczbę całkowitą.
 * Funkcja obsługuje formaty dziesiętne, szesnastkowe (prefiks "0x") i binarne (prefiks "0b").
 * Obsługiwane są zarówno liczby dodatnie, jak i ujemne (dla dziesiętnych).
 * Jeśli wynik wymaga typu int64_t, należy wykonać rzutowanie na odpowiedni typ.
 * @note Przed wywołaniem tej funkcji zaleca się użycie funkcji walidacji `str2uint64_fault`
 * @param str Wskaźnik do ciągu znaków do konwersji.
 * @return Liczba całkowita uint64_t uzyskana z ciągu znaków.
 */
uint32_t str2nbr64(const char *str) 
{
  int base = 10;
  bool is_negative = false;
  if(*str == '-') {
    is_negative = true;
    str++;
  }
  if(str[0] == '0' && str[1] == 'b') {
    base = 2;
    str += 2;
  } 
  else if(str[0] == '0' && str[1] == 'x') {
    base = 16;
    str += 2;
  }
  char *endptr;
  uint64_t result = strtoull(str, &endptr, base); // Użycie strtoull dla uint64_t
  return is_negative ? -(int64_t)result : result;
}

/**
 * Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na liczbę float.
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny lub pusty, false w przeciwnym razie.
 */
bool str2float_fault(const char *str)
{
  if(!str || *str == '\0') return true;
  bool dot = false;
  bool digit = false;
  if(*str == '-' || *str == '+') str++;
  while(*str) {
    if(*str >= '0' && *str <= '9') {
      digit = true;
    }
    else if(*str == '.') {
      if(dot) return true;
      dot = true;
    }
    else {
      return true;
    }
    str++;
  }
  return !digit;
}

/**
 * Konwertuje ciąg znaków na liczbę zmiennoprzecinkową typu float.
 * Wymaga uprzedniej walidacji przy użyciu funkcji 'str2float_fault'!
 * @param str Wskaźnik do ciągu znaków do konwersji.
 * @return Liczba zmiennoprzecinkowa typu float uzyskana z ciągu.
 */
float str2float(const char *str)
{
  char *endptr;
  float result = strtof(str, &endptr);
  return result;
}

/**
 * @brief Sprawdza, czy ciąg znaków może być poprawnie przekonwertowany na float z opcjonalnym przyrostkiem jednostki.
 * @param str Wskaźnik do ciągu znaków do sprawdzenia.
 * @return true, jeśli ciąg jest niepoprawny; false, jeśli ciąg jest poprawny.
 */
bool str2unitfloat_fault(const char *str)
{
  if(!str || *str == '\0') return true;
  const char *unit = str;
  while((*unit >= '0' && *unit <= '9') || *unit == '-' || *unit == '.') {
    unit++;
  }
  char nbr[unit - str + 1];
  strncpy(nbr, str, unit - str);
  nbr[unit - str] = '\0';
  return str2float_fault(nbr);
}


/**
 * @brief Konwertuje ciąg znaków na liczbę zmiennoprzecinkową typu float z obsługą jednostek.
 * Obsługiwane jednostki:
 * - 't': Tera
 * - 'g': Giga
 * - 'meg','e': Mega
 * - 'k': Kilo
 * - '%': Procent
 * - 'm': Mili
 * - 'u': Mikro
 * - 'n': Nano
 * - 'p': Piko
 * Jeśli jednostka nie jest rozpoznana, wartość pozostaje niezmieniona.
 * Wymaga uprzedniej walidacji przy użyciu funkcji 'str2unitfloat_fault'!
 * @param str Wskaźnik do ciągu znaków do konwersji.
 * @return Wartość zmiennoprzecinkowa typu float uzyskana z ciągu.
 *         Jeśli ciąg jest niepoprawny, zwraca 0.0.
 */
float str2unitfloat(const char *str)
{
  const char *unit = str;
  while((*unit >= '0' && *unit <= '9') || *unit == '-' || *unit == '.') {
    unit++;
  }
  char nbr[unit - str + 1];
  strncpy(nbr, str, unit - str);
  nbr[unit - str] = '\0';
  float value = str2float(nbr);
  switch(*unit) {
    case 't': value *= 1000000000000; break;
    case 'g': value *= 1000000000; break;
    case 'e': value *= 1000000; break;
    case 'k': value *= 1000; break;
    case '%': value /= 100; break;
    case 'm':
      if(*(str + 1) == 'e' && *(str + 2) == 'g') value *= 1000000;
      else value /= 1000;
      break;
    case 'u': value /= 1000000; break;
    case 'n': value /= 1000000000; break;
    case 'p': value /= 1000000000000; break;
    default: break;
  }
  return value;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Oblicza skrót (hash) dla ciągu znaków za pomocą algorytmu djb2.
 * @param str Wskaźnik na ciąg znaków (null-terminowany).
 * @return uint32_t Wartość skrótu obliczona na podstawie wejściowego ciągu.
 */
uint32_t hash(const char *str)
{
  uint32_t hash = 5381;
  uint32_t c;
  while((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash;
}

/**
 * @brief Wyodrębnia podciąg z zadanego ciągu znaków.
 * @param str Wskaźnik na wejściowy ciąg znaków (null-terminowany).
 * @param start Indeks początkowy podciągu (może być ujemny, aby liczyć od końca).
 * @param length Długość podciągu (jeśli <= 0, obliczana od `start` do końca).
 * @return Wskaźnik na nowo zaalokowany podciąg. Należy zwolnić pamięć po użyciu.
 */
char *substr(char *str, int16_t start, int16_t length)
{
  int16_t size = strlen(str);
  if(start < 0) start += size;
  if(length <= 0) length = size - start;
  if(start < 0 || start >= size || length <= 0 || start + length > size) {
    return NULL;
  }
  char *str_out = new(length + 1);
  memcpy(str_out, &str[start], length);
  str_out[length] = '\0';
  return str_out;
}

/**
 * @brief Łączy wiele ciągów znaków w jeden ciąg.
 * @param count Liczba ciągów do połączenia.
 * @param str Wskaźnik na pierwszy ciąg znaków.
 * @param ... Kolejne ciągi znaków do połączenia.
 * @return Wskaźnik na nowo zaalokowany ciąg wynikowy. Należy zwolnić pamięć po użyciu.
 */
char *joinstr(unsigned int count, const char *str, ...)
{
  if(!str || count == 0) return NULL;
  va_list args;
  unsigned int i;
  size_t total_size = 1;
  size_t sizes[count];
  va_start(args, str);
  const char *tmp = str;
  for(i = 0; i < count; i++) {
    sizes[i] = strlen(tmp);
    total_size += sizes[i];
    tmp = va_arg(args, char *);
  }
  va_end(args);
  char *strout = new(total_size);
  if(!strout) return NULL;
  strout[total_size - 1] = '\0';
  char *marker = strout;
  va_start(args, str);
  tmp = str;
  for(i = 0; i < count; i++) {
    memcpy(marker, tmp, sizes[i]);
    marker += sizes[i];
    tmp = va_arg(args, char *);
  }
  va_end(args);
  return strout;
}

/**
 * @brief Kopiuje ciąg znaków do nowo zaalokowanego bufora.
 * @param str Wskaźnik na wejściowy ciąg znaków (null-terminowany).
 * @return char* Wskaźnik na nowo zaalokowany ciąg znaków. Należy zwolnić pamięć po użyciu.
 */
char *strcopy(const char *str)
{
  if(!str) return NULL;
  size_t size = strlen(str) + 1;
  char *cp = new(size);
  if(!cp) return NULL;
  memcpy(cp, str, size);
  return cp;
}

const char LowerCase[256] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0xAB, 0x8E, 0x86,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x98, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA5, 0xA5, 0xA6, 0xA7, 0xA9, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBE, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0x88, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xA2, 0xE1, 0xE2, 0xE4, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

const char UpperCase[256] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x8F, 0x87, 0x9D, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xE0, 0xA3, 0xA4, 0xA4, 0xA6, 0xA7, 0xA8, 0xA8, 0xAA, 0x8D, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBD, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE3, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

/**
 * @brief Zamienia wszystkie znaki w ciągu na małe litery, modyfikując oryginalny ciąg.
 * @param str Wskaźnik na ciąg znaków (null-terminowany).
 * @return Wskaźnik na zmodifikowany ciąg.
 */
char *str2lower_this(char *str)
{
  if(!str) return NULL; // Obsługa błędnego wskaźnika
  char *p = str;
  while(*p) {
    *p = LowerCase[(uint8_t)*p];
    p++;
  }
  return str;
}

/**
 * @brief Zamienia wszystkie znaki w ciągu na wielkie litery, modyfikując oryginalny ciąg.
 * @param str Wskaźnik na ciąg znaków (null-terminowany).
 * @return Wskaźnik na zmodifikowany ciąg.
 */
char *str2upper_this(char *str)
{
  if(!str) return NULL; // Obsługa błędnego wskaźnika
  char *p = str;
  while(*p) {
    *p = UpperCase[(uint8_t)*p];
    p++;
  }
  return str;
}

/**
 * @brief Tworzy kopię ciągu z zamienionymi znakami na małe litery.
 * @param str Wskaźnik na wejściowy ciąg znaków (null-terminowany).
 * @return char* Wskaźnik na nowo zaalokowany ciąg znaków. Należy zwolnić pamięć po użyciu.
 */
char *str2lower(char *str)
{
  if(!str) return NULL; // Obsługa błędnego wskaźnika
  char *cp = strcopy(str);
  return str2lower_this(cp);
}

/**
 * @brief Tworzy kopię ciągu z zamienionymi znakami na wielkie litery.
 * @param str Wskaźnik na wejściowy ciąg znaków (null-terminowany).
 * @return Wskaźnik na nowo zaalokowany ciąg znaków. Należy zwolnić pamięć po użyciu.
 */
char *str2upper(char *str)
{
  if(!str) return NULL; // Obsługa błędnego wskaźnika
  char *cp = strcopy(str);
  return str2upper_this(cp);
}

/**
 * @brief Odwraca kolejność znaków w ciągu 'str' w miejscu (in-place).
 * @param str Wskaźnik do ciągu znaków, który zostanie odwrócony.
 * @return Wskaźnik do odwróconego ciągu.
 */
char *reverse_this_string(char *str) 
{
  if(!str) return NULL;
  size_t size = strlen(str);
  if(size <= 1) return str;
  char *begin = str;
  char *end = str + size - 1;
  char temp;
  while(begin < end) {
    temp = *begin;
    *begin = *end;
    *end = temp;
    begin++;
    end--;
  }
  return str;
}

/**
 * @brief Tworzy nową kopię ciągu 'str_in', w której znaki są odwrócone.
 * @param str_in Wskaźnik do oryginalnego ciągu znaków.
 * @return Wskaźnik do nowo zaalokowanego odwróconego ciągu lub NULL w przypadku błędu.
 * Pamięć na wynikowy ciąg musi być zwolniona przez użytkownika.
 */
char *reverse_string(const char *str_in) {
  if(!str_in) return NULL;
  size_t size = strlen(str_in);
  char *str_out = new(size + 1);
  if(!str_out) return NULL;
  memcpy(str_out, str_in, size + 1);
  return reverse_this_string(str_out);
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Znajduje pierwsze wystąpienie 'pattern' w 'str'.
 * @param str Ciąg, w którym należy szukać.
 * @param pattern Wzorzec do znalezienia.
 * @return Indeks pierwszego wystąpienia (0-based) lub -1, jeśli nie znaleziono.
 */
int find(const char *str, const char *pattern) 
{
  if(!str || !pattern || !*pattern) return -1;
  int marker = 0;
  int pointer = 0;
  while(*str) {
    if(*str == pattern[marker]) {
      marker++;
      if(!pattern[marker]) return pointer - marker + 1;
    }
    else marker = 0;
    pointer++;
    str++;
  }
  return -1;
}

/**
 * @brief Znajduje ostatnie wystąpienie 'pattern' w 'str'.
 * @param str Ciąg, w którym należy szukać.
 * @param pattern Wzorzec do znalezienia.
 * @return Indeks ostatniego wystąpienia (0-based) lub -1, jeśli nie znaleziono.
 */
int find_right(const char *str, const char *pattern) {
  if(!str || !pattern || !*pattern) return -1;
  int last_index = -1;
  int current_index = 0;
  while(*str) {
    if(strncmp(str, pattern, strlen(pattern)) == 0) {
      last_index = current_index;
    }
    str++;
    current_index++;
  }
  return last_index;
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Funkcja wycina część ciągu na podstawie trybu 'mode' i wzorca 'pattern'.
 * @param str: Ciąg wejściowy do modyfikacji.
 * @param pattern: Wzorzec do znalezienia.
 * @param mode: Tryb wycinania (CUT_e).
 * @return: Zmodyfikowany wskaźnik do ciągu lub oryginalny ciąg, jeśli 'pattern' nie znaleziono.
 */
char *cut_this(char *str, const char *pattern, CUT_e mode) {
  if(!str || !pattern || !*pattern) return str;
  int index;
  size_t pattern_len = strlen(pattern);
  switch (mode) {
    case CUT_BeforeFirst: {
      index = find(str, pattern);
      if(index >= 0) str[index] = '\0';
      break;
    }
    case CUT_BeforeLast: {
      index = find_right(str, pattern);
      if(index >= 0) str[index] = '\0';
      break;
    }
    case CUT_AfterFirst: {
      index = find(str, pattern);
      if(index >= 0) str += index + pattern_len;
      break;
    }
    case CUT_AfterLast: {
      index = find_right(str, pattern);
      if(index >= 0) str += index + pattern_len;
      break;
    }
    default: break;
  }
  return str;
}

/**
 * @brief Tworzy nowy ciąg na podstawie oryginału, wycinając fragment zgodnie z trybem 'mode'.
 * @param str_in: Oryginalny ciąg znaków.
 * @param pattern: Wzorzec do znalezienia.
 * @param mode: Tryb wycinania (CUT_e).
 * @return: Nowo zaalokowany ciąg z wynikiem lub NULL w przypadku błędu.
 */
char *cut(const char *str_in, const char *pattern, CUT_e mode) {
  if(!str_in || !pattern) return NULL;
  size_t size = strlen(str_in) + 1;
  char *str_out = new(size);
  if(!str_out) return NULL;
  strcpy(str_out, str_in);
  return cut_this(str_out, pattern, mode);
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief Sprawdza, czy znak 'byte' znajduje się w ciągu 'list'.
 * @param byte Znak do sprawdzenia.
 * @param list Lista znaków, w której należy szukać.
 * @return Pozycja (indeks od 1) znaku w liście lub 0, jeśli znak nie został znaleziony.
 */
static int find_char(char byte, const char *list)
{
  int pointer = 1;
  while(*list) {
    if(byte == *list) return pointer;
    pointer++;
    list++;
  }
  return 0;
}

/**
 * @brief Usuwa wszystkie znaki z początku ciągu 'str', które znajdują się w liście 'list'.
 * @param str Ciąg wejściowy do modyfikacji.
 * @param list Lista znaków do usunięcia z początku ciągu.
 * @return Wskaźnik do przyciętego ciągu.
 */
char *ltrim_list(char *str, const char *list)
{
  while(find_char(*str, list)) str++;
  return str;
}

/**
 * @brief Usuwa wszystkie białe znaki z początku ciągu 'str'.
 * @param str Ciąg wejściowy do modyfikacji.
 * @return Wskaźnik do przyciętego ciągu.
 */
char *ltrim(char *str)
{
  while(isspace((unsigned char)*str)) str++;
  return str;
}

/**
 * @brief Usuwa wszystkie znaki z końca ciągu 'str', które znajdują się w liście 'list'.
 * @param str Ciąg wejściowy do modyfikacji.
 * @param list Lista znaków do usunięcia z końca ciągu.
 * @return Wskaźnik do przyciętego ciągu.
 */
char *rtrim_list(char *str, const char *list)
{
  char *back = str + strlen(str);
  while(back > str && find_char(*--back, list));
  *(back + 1) = '\0';
  return str;
}

/**
 * @brief Usuwa wszystkie białe znaki z końca ciągu 'str'.
 * @param str Ciąg wejściowy do modyfikacji.
 * @return Wskaźnik do przyciętego ciągu.
 */
char *rtrim(char *str)
{
  char *back = str + strlen(str);
  while(back > str && isspace((unsigned char)*--back));
  *(back + 1) = '\0';
  return str;
}

/**
 * @brief Usuwa wszystkie znaki z początku i końca ciągu 'str', które znajdują się w liście 'list'.
 * @param str Ciąg wejściowy do modyfikacji.
 * @param list Lista znaków do usunięcia z początku i końca ciągu.
 * @return Wskaźnik do przyciętego ciągu.
 */
char *trim_list(char *str, const char *list)
{
  return ltrim_list(rtrim_list(str, list), list);
}

/**
 * @brief Usuwa wszystkie białe znaki z początku i końca ciągu 'str'.
 * @param str Ciąg wejściowy do modyfikacji.
 * @return Wskaźnik do przyciętego ciągu.
 */
char *trim(char *str)
{
  return ltrim(rtrim(str));
}

//-------------------------------------------------------------------------------------------------

/**
 * @brief wyciąga dany łańcuch znaków pomiędzy wskazanymi znakami separującymi.
 * @param str Wejściowy łańcuch znaków.
 * @param delimiter Znak, który jest separatorem.
 * @param position Wskazuje, który z kolei fragment ma zostać zwrócony (indeks zaczyna się od 0).
 * @return Fragment łańcucha znaków lub NULL, jeśli fragment nie istnieje.
 */
char *extraction(const char *str, char delimiter, int position)
{
  if(!str || position < 0) return NULL;
  const char *start = str;
  size_t size = 0;
  while(*str) {
    if(*str == delimiter) {
      if(position == 0) break;
      position--;
      start = str + 1;
    }
    str++;
  }
  if(position > 0) return NULL;
  size = str - start;
  char *str_out = new(size + 1);
  if(!str_out) return NULL;
  memcpy(str_out, start, size);
  str_out[size] = '\0';
  return str_out;
}


/**
 * @brief Funkcja dzieli ciąg znaków na fragmenty na podstawie delimitera.
 * Wszystkie fragmenty i tablica wskaźników są przechowywane w jednej alokacji pamięci
 * i zarządzane przez niestandardowy mechanizm 'new'.
 * @param arr_ptr Wskaźnik na tablicę wynikową (zostanie zaalokowana w tej funkcji).
 * @param str Ciąg wejściowy do podzielenia. Nie może być NULL.
 * @param delimiter Znak, który rozdziela fragmenty.
 * @return Liczba fragmentów w tablicy lub -1 w przypadku błędu.
 */
int explode(char ***arr_ptr, const char *str, char delimiter)
{
  if(!arr_ptr || !str) return -1;
  char *src = (char *)str, *end, *dst;
  int size = 1;
  int i;
  while((end = strchr(src, delimiter)) != NULL) {
    ++size;
    src = end + 1;
  }
  char **arr = new(size * sizeof(char *) + (strlen(str) + 1) * sizeof(char));
  if(!arr) return -1;
  src = (char *)str;
  dst = (char *)arr + size * sizeof(char *);
  for(i = 0; i < size; ++i) {
    if((end = strchr(src, delimiter)) == NULL) end = src + strlen(src);
    arr[i] = dst;
    strncpy(dst, src, end - src);
    dst[end - src] = '\0';
    dst += end - src + 1;
    src = end + 1;
  }
  *arr_ptr = arr;
  return size;
}


//-------------------------------------------------------------------------------------------------

/**
 * @brief Funkcja zamienia wszystkie wystąpienia znaków z 'pattern' na 'replacement' w ciągu 'str'.
 * @param pattern Ciąg znaków określających znaki do zamiany.
 * @param replacement Znak, na który mają być zamieniane znaki.
 * @param str Ciąg wejściowy do modyfikacji (zostanie zmodyfikowany).
 * @return Zwraca wskaźnik do oryginalnego początku ciągu 'str'.
 */
char *replace_this_char(const char *pattern, char replacement, char *str)
{
  if(!pattern || !str) return str;
  char *original = str;
  while(*str) {
    const char *ptr = pattern;
    while(*ptr) {
      if(*str == *ptr) {
        *str = replacement;
        break;
      }
      ptr++;
    }
    str++;
  }
  return original; // Zwróć wskaźnik do początku ciągu
}

/**
 * @brief Funkcja tworzy kopię ciągu 'original' i zamienia w niej wszystkie znaki z 'pattern' na 'replacement'.
 * @param pattern Ciąg znaków określających znaki do zamiany.
 * @param replacement Znak, na który mają być zamieniane znaki.
 * @param original Ciąg wejściowy, który pozostaje niemodyfikowany.
 * @return: Wskaźnik do nowo zaalokowanej kopii zmodyfikowanego ciągu.
 */
char *replace_char(const char *pattern, char replacement, const char *original)
{
  if(!pattern || !original) return NULL;
  size_t size = strlen(original) + 1;
  char *str = new(size);
  if(!str) return NULL;
  memcpy(str, original, size);
  return replace_this_char(pattern, replacement, str);
}

//-------------------------------------------------------------------------------------------------

/**
 * Funkcja zamienia wszystkie wystąpienia podciągu 'pattern' na 'replacement' w ciągu 'original'.
 * @param pattern Ciąg znaków do zamiany.
 * @param replacement Ciąg znaków, który zastępuje 'pattern'.
 * @param original Ciąg wejściowy, który pozostaje niemodyfikowany.
 * @return: Wskaźnik do nowo zaalokowanego ciągu z dokonanymi zmianami lub NULL w przypadku błędu.
 */
char *replace_string(const char *pattern, const char *replacement, const char *original)
{
  if(!pattern || !replacement || !original || *pattern == '\0') return NULL;
  size_t replen = strlen(replacement);
  size_t patlen = strlen(pattern);
  size_t orilen = strlen(original);
  size_t patcnt = 0;
  const char *oriptr = original;
  const char *patloc;
  while((patloc = strstr(oriptr, pattern)) != NULL) {
    patcnt++;
    oriptr = patloc + patlen;
  }
  size_t retlen = orilen + patcnt * (replen - patlen);
  char *returned = new(sizeof(char) * (retlen + 1));
  if(!returned) return NULL;
  char *retptr = returned;
  oriptr = original;
  while((patloc = strstr(oriptr, pattern)) != NULL) {
    size_t skplen = patloc - oriptr;
    memcpy(retptr, oriptr, skplen);
    retptr += skplen;
    memcpy(retptr, replacement, replen);
    retptr += replen;
    oriptr = patloc + patlen;
  }
  strcpy(retptr, oriptr);
  returned[retlen] = '\0';
  return returned;
}

//-------------------------------------------------------------------------------------------------



// /**
//  * @brief Processes a raw input buffer and handles backspace, arrows, and delete keys.
//  * @param buffer Pointer to the character buffer to process.
//  * @param length Length of the input buffer (number of bytes).
//  * @return New length of the processed buffer (after editing).
//  */
// uint16_t process_buffer(char *buffer, uint16_t length)
// {
//   uint16_t cursor = 0; // current cursor position in the output buffer
//   uint16_t current_length = 0; // current text length in the output buffer
//   uint16_t i = 0; // input buffer iterator index
//   while(i < length) {
//     char c = buffer[i];
//     if(c == '\b' || c == 0x7F) { // Backspace (BS, ASCII 8) – remove character before the cursor if exists
//       if(cursor > 0) { // Shift all characters left from current cursor position
//         memmove(buffer + cursor - 1, buffer + cursor, current_length - cursor);
//         current_length--;
//         cursor--;
//       }
//       i++; // If cursor == 0 (start of line), ignore backspace
//       continue;
//     }
//     else if(c == 0x1B) { // Escape (ASCII 27) – potential start of control sequence
//       if(i + 1 < length && buffer[i + 1] == '[') { // Sequence starts with ESC [
//         if(i + 2 < length) {
//           unsigned char seq = buffer[i + 2];
//           if(seq == 'A' || seq == 'B') { // Up or down arrow – ignore (no text or cursor change)
//             i += 3; continue;
//           }
//           else if(seq == 'C') { // Right arrow – move cursor right if possible
//             if(cursor < current_length) cursor++;
//             i += 3; continue;
//           }
//           else if(seq == 'D') { // Left arrow – move cursor left if possible
//             if(cursor > 0) cursor--;        
//             i += 3; continue;
//           }
//           else if(seq == '3') { // Potential Delete sequence: check next character
//             if(i + 3 < length && buffer[i + 3] == '~') { // Delete key – remove character under cursor if exists
//               if(cursor < current_length) {
//                 memmove(buffer + cursor, buffer + cursor + 1, current_length - cursor - 1);
//                 current_length--; // (cursor stays at the same position)
//               }
//               i += 4; continue;
//             }
//             else { i += 3; continue; } // ESC [ 3 without full '~' termination – skip as unknown sequence
//           } else { i += 3; continue; } // Other unhandled ESC [ X sequence – skip three characters (ESC, '[', code) 
//         }
//         else { i += 2;  continue; } // ESC [ at the end of buffer without code – skip available characters
//       }
//       else { i++; continue; } // Single ESC without known sequence – skip it
//     }
//     else if((c < 0x20 && c != '\n' && c != '\r') || c == 0x7F) { i++; continue; } // Not a normal character, skip
//     else { // Normal character – insert into output buffer at cursor position
//       if(cursor < current_length) {
//         // If inserting inside text, shift existing characters right to make space
//         memmove(buffer + cursor + 1, buffer + cursor, current_length - cursor);
//       }
//       // Insert character at cursor
//       buffer[cursor] = c;
//       current_length++;
//       cursor++;
//       i++;
//       continue;
//     }
//   }
//   // Terminate resulting text with null if the buffer is to be treated as a C-string
//   if(current_length < length) {
//     buffer[current_length] = '\0';
//   }
//   return current_length;
// }
