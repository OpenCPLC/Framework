#include "max31865.h"

#define MAX31865_RTD_A 3.9083e-3
#define MAX31865_RTD_B -5.775e-7

/**
 * @brief Oblicza temperaturę w stopniach Celsjusza na podstawie
 * rezystancji termometru oporowego RTD za pomocą obliczeń rezystancji.
 * @param raw Wartość 16-bitowa bezpośrednio z rejestru RTD przekształcona na float
 * @param nominal Wartość nominalna rezystancji dla czujnika RTD (100 dla PT100, 1000 dla PT1000)
 * @param reference Wartość referencyjna rezystancji podłączona zewnętrznie
 * @return Wartość temperatury [°C]
 */
static float MAX31865_Calc(float raw, float nominal, float reference)
{
  float Z1, Z2, Z3, Z4, Rt, temp;
  Rt = raw;
  Rt /= 32768;
  Rt *= reference;
  Z1 = -MAX31865_RTD_A;
  Z2 = MAX31865_RTD_A * MAX31865_RTD_A - (4 * MAX31865_RTD_B);
  Z3 = (4 * MAX31865_RTD_B) / nominal;
  Z4 = 2 * MAX31865_RTD_B;
  temp = Z2 + (Z3 * Rt);
  temp = (sqrt(temp) + Z1) / Z4;
  if(temp >= 0) return temp;
  Rt /= nominal;
  Rt *= 100;
  float rpoly = Rt;
  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt;
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt;
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt;
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt;
  temp += 1.5243e-10 * rpoly;
  return temp;
}

void MAX31865_Init(MAX31865_t *rtd)
{
  rtd->ready->mode = GPIO_Mode_Input;
  GPIO_Init(rtd->ready);
  SPI_Master_Init(rtd->spi);
}

static status_t MAX31865_ReadData(MAX31865_t *rtd, uint8_t *buffer)
{
  if(timeout(50, WAIT_&GPIO_In, rtd->ready)) return ERR;
  SPI_Master_ReadSequence(rtd->spi, MAX31865_Reg_Read_RTD_MSBs, buffer, 2);
  if(timeout(50, WAIT_&SPI_Master_IsFree, rtd->spi)) return ERR;
  return OK;
}

/**
 * @brief Pętla obsługująca pomiar temperatury za pomocą czujnika RTD.
 * Wykonuje konfigurację, dokonuje pomiarów oraz oblicza temperaturę.
 * @param rtd Wskaźnik do struktury reprezentującej układ MAX31865.
 * @return Status wykonanej operacji.
 * ERR `1` w przypadku niepowodzenia, OK `0` w przypadku powodzenia.
*/
status_t MAX31865_Loop(MAX31865_t *rtd)
{
  if(SPI_Master_IsBusy(rtd->spi)) return ERR;
  uint8_t buffer[2];
  float raw;
  bool auto_conversion = !rtd->oversampling;
  buffer[0] = MAX31865_Reg_Write_Configuration;
  buffer[1] = MAX31865_CFG_BIAS | (auto_conversion << 6) | (rtd->wire << 4) | MAX31865_CFG_FSCLR | rtd->reject;
  SPI_Master_Write(rtd->spi, buffer, 2);
  if(timeout(50, WAIT_&SPI_Master_IsFree, rtd->spi)) return ERR;
  delay(15);
  if(rtd->oversampling) {
    raw = 0;
    for(uint8_t i = 0; i < rtd->oversampling; i++) {
      if(MAX31865_ReadData(rtd, buffer)) return ERR;
      raw += (float)(((uint16_t)buffer[0] << 8) | buffer[1]);
    }
  }
  else {
    buffer[0] = MAX31865_Reg_Write_Configuration;
    buffer[1] = MAX31865_CFG_BIAS | MAX31865_CFG_SHOT | (rtd->wire << 4) | rtd->reject;
    SPI_Master_Write(rtd->spi, buffer, 2);
    if(timeout(50, WAIT_&SPI_Master_IsFree, rtd->spi)) return ERR;
    if(MAX31865_ReadData(rtd, buffer)) return ERR;
    raw = (float)(((uint16_t)buffer[0] << 8) | buffer[1]);
  }
  if(rtd->type == MAX31865_Type_PT100) rtd->temperature = MAX31865_Calc(raw, 100, 400);
  else rtd->temperature = MAX31865_Calc(raw, 1000, 4000);
  buffer[0] = MAX31865_Reg_Write_Configuration;
  buffer[1] = (rtd->wire << 4) | MAX31865_CFG_FSCLR | rtd->reject;
  return OK;
}