#ifndef RTC_H_
#define RTC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "irq.h"
#include "main.h"

//-------------------------------------------------------------------------------------------------

#ifndef RTC_IQR_PRIORYTY
  #define RTC_IQR_PRIORYTY IRQ_Priority_Low
#endif

typedef enum {
  RTC_Weekday_Error = -1,
  RTC_Weekday_Everyday = 0,
  RTC_Weekday_Monday = 1,
  RTC_Weekday_Tuesday = 2,
  RTC_Weekday_Wednesday = 3,
  RTC_Weekday_Thursday = 4,
  RTC_Weekday_Friday = 5,
  RTC_Weekday_Saturday = 6,
  RTC_Weekday_Sunday = 7
} RTC_Weekday_e;

#ifndef RTC_WEEKDAYS_LONG
  #define RTC_WEEKDAYS_LONG 1
#endif

//-------------------------------------------------R------------------------------------------------

typedef enum {
  RTC_ALARM_A = 0,
  RTC_ALARM_B = 1
} RTC_ALARM_e;

typedef struct {
  uint8_t year;
  uint8_t month;
  uint8_t month_day;
  uint8_t week_day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint32_t ms;
} RTC_Datetime_t;

typedef struct {
  bool week;
  bool day_mask;
  uint8_t day;
  bool hour_mask;
  uint8_t hour;
  bool minute_mask;
  uint8_t minute;
  bool second_mask;
  uint8_t second;
} RTC_Alarm_t;

//-------------------------------------------------------------------------------------------------

void RTC_Init(void);

RTC_Datetime_t RTC_UnixToDatetime(uint64_t timestamp);
char *RTC_WeekDayString(void);
uint32_t RTC_DatetimeToUnix(RTC_Datetime_t *date);

RTC_Alarm_t RTC_DaystampToAlarm(uint32_t stamp);
RTC_Alarm_t RTC_WeekstampToAlarm(uint32_t stamp);
uint32_t RTC_AlarmToDaystamp(RTC_Alarm_t *alarm);
uint32_t RTC_AlarmToWeekstamp(RTC_Alarm_t *alarm);

bool RTC_DatetimeSyncIsCorrect(RTC_Datetime_t *date, int8_t time_zone);
void RTC_SetDatetime(RTC_Datetime_t *datetime);
void RTC_SetTimestamp(uint64_t timestamp);
void RTC_Reset(void);

RTC_Datetime_t RTC_Datetime(void);
uint32_t RTC_Timestamp(void);
uint64_t RTC_TimestampMs(void);
uint32_t RTC_Daystamp(void);
uint32_t RTC_Weekstamp(void);

RTC_Alarm_t RTC_Alarm(RTC_ALARM_e alarm);
RTC_Alarm_t RTC_Alarm_A(void);
uint32_t RTC_Daystamp_A(void);
RTC_Alarm_t RTC_Alarm_B(void);
uint32_t RTC_Daystamp_B(void);

bool RTC_Alarm_IsEnabled(RTC_ALARM_e alarm);
void RTC_Alarm_Enable(RTC_ALARM_e alarm, RTC_Alarm_t *set);
void RTC_Alarm_A_Enable(RTC_Alarm_t *set);
void RTC_Daystamp_A_Enable(uint32_t stamp);
void RTC_Weekstamp_A_Enable(uint32_t stamp);
void RTC_Interval_A_Enable(uint32_t interval);
void RTC_Alarm_B_Enable(RTC_Alarm_t *set);
void RTC_Daystamp_B_Enable(uint32_t stamp);
void RTC_Weekstamp_B_Enable(uint32_t stamp);
void RTC_Interval_B_Enable(uint32_t interval);

void RTC_Alarm_A_Disable(void);
void RTC_Alarm_B_Disable(void);
void RTC_WakeupTimer_Enable(uint32_t sec);
void RTC_WakeupTimer_Disable(void);

bool RTC_Check_Daystamp(uint32_t stamp_alarm, uint32_t offset_min_sec, uint32_t offset_max_sec);
bool RTC_Check_Weekstamp(uint32_t stamp_alarm, uint32_t offset_min_sec, uint32_t offset_max_sec);
bool RTC_Check_A(uint32_t offset_min_sec, uint32_t offset_max_sec);
bool RTC_Check_B(uint32_t offset_min_sec, uint32_t offset_max_sec);

bool RTC_Event_A(void);
bool RTC_Event_B(void);
bool RTC_Event_WakeupTimer(void);

void RTC_Force_A(void);
void RTC_Force_B(void);
void RTC_Force_WakeupTimer(void);

//-------------------------------------------------------------------------------------------------

extern const char *rtc_weakdays[];
extern const RTC_Datetime_t rtc_datetime_empty;
extern bool rtc_ready;
extern bool rtc_init;

//-------------------------------------------------------------------------------------------------
#endif
