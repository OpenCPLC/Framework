/**
 * @file  main.h
 * @brief W tym pliku należy umieścić parametry konfiguracyjne '#define', które chce się nadpisać.
 *        Wiele z bibliotek OpenCPLC załącza ten plik '#include', zatem musi on istnieć, nawet jeśli będzie pusty.
 *        Dzięki takiemu rozwiązaniu można nadpisać różnego rodzaju definicje (zmienne) konfiguracyjne.
 *        Biblioteki OpenCPLC w pierwszej kolejności będą pobierały zawarte tutaj zmienne,
 *        zamiast korzystać z domyślnych wartości zawartych we własnych plikach nagłówkowych '.h'.
 * @date  2024-11-11
 */

#define SYS_CLOCK_FREQ 59904000