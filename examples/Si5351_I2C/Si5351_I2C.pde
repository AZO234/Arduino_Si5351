#include <stdint.h>
#include <stdbool.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

//#define SI5351_I2C_USE_TINYWIREM

#if !defined(SI5351_I2C_USE_TINYWIREM)
#include <Wire.h>
#else
#include <TinyWireM.h>
#endif  /* SI5351_I2C_USE_TINYWIREM */

#include <si5351_i2c.h>

Si5351_I2C g_oSi5351;

void* pLock = NULL;

static void Si5351_I2C_BeginTransmission(const uint8_t u8Address) {
#if !defined(SI5351_I2C_USE_TINYWIREM)
  Wire.beginTransmission(u8Address);
#else
  TinyWireM.beginTransmission(u8Address);
#endif  /* SI5351_I2C_USE_TINYWIREM */
}

static void Si5351_I2C_RequestFrom(const uint8_t u8Address, const uint8_t u8Count) {
#if !defined(SI5351_I2C_USE_TINYWIREM)
  Wire.requestFrom(u8Address, u8Count);
#endif  /* SI5351_I2C_USE_TINYWIREM */
}

static uint8_t Si5351_I2C_Read(const uint8_t u8Address) {
#if !defined(SI5351_I2C_USE_TINYWIREM)
  (void)u8Address;
  return Wire.read();
#else
  /* Must endTransmission before this call */
  TinyWireM.requestFrom(u8Address, 1);
  return TinyWireM.receive();
#endif  /* SI5351_I2C_USE_TINYWIREM */
}

static void Si5351_I2C_Write(const uint8_t u8Value) {
#if !defined(SI5351_I2C_USE_TINYWIREM)
  Wire.write(u8Value);
#else
  TinyWireM.send(u8Value);
#endif  /* SI5351_I2C_USE_TINYWIREM */
}

static void Si5351_I2C_EndTransmission(void) {
#if !defined(SI5351_I2C_USE_TINYWIREM)
  Wire.endTransmission();
#else
  TinyWireM.endTransmission();
#endif  /* SI5351_I2C_USE_TINYWIREM */
}

static void Si5351_MemoryBarrier(void) {
}

void setup() {
#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
  float64_t f64PLL, f64Clk;
#else
  double dPLL, dClk;
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */
  Si5351_MS_t tMSPLL, tMSClk;

  g_oSi5351.initialize(
    Si5351_I2C_BeginTransmission,
    Si5351_I2C_RequestFrom,
    Si5351_I2C_Read,
    Si5351_I2C_Write,
    Si5351_I2C_EndTransmission,
    Si5351_MemoryBarrier,
    &pLock
  );

#if !defined(SI5351_I2C_USE_TINYWIREM)
  Wire.begin();
  Wire.setClock(400000);
#else
  TinyWireM.begin();
#endif  /* SI5351_I2C_USE_TINYWIREM */

  g_oSi5351.initDevice();
  g_oSi5351.setSSPDisable();

  /* XTAL -> PLLA -> MS0 -> CLK0 */

  g_oSi5351.setPLLA_SRC(SI5351_PLL_SRC_XTAL);
#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
  f64PLL = ui64_to_f64(500000000);  /* PLL: 500MHz*/
  if(Si5351_I2C::calcMSPLL(&tMSPLL, ui64_to_f64(SI5351_XTAL_FREQ), f64PLL)) {
#else
  dPLL = 500000000;  /* PLL: 500MHz*/
  if(Si5351_I2C::calcMSPLL(&tMSPLL, SI5351_XTAL_FREQ, dPLL)) {
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */
    g_oSi5351.setMSA(&tMSPLL);
    g_oSi5351.PLLSoftReset();

    g_oSi5351.setClkPowerDown(SI5351_CLK0, false);
    g_oSi5351.setClkMSSource(SI5351_CLK0, SI5351_CLK_MS_SRC_PLLA);
    g_oSi5351.setClkSrc(SI5351_CLK0, SI5351_CLK_SRC_MS);
#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
    f64Clk = ui64_to_f64(10000000);  /* Clk: 10MHz*/
    if(Si5351_I2C::calcMSClk(&tMSClk, ui64_to_f64(500000000), f64Clk)) {
#else
    dClk = 10000000;  /* Clk: 10MHz*/
    if(Si5351_I2C::calcMSClk(&tMSClk, 500000000, dClk)) {
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */
      g_oSi5351.setMS(SI5351_CLK0, &tMSClk);
      g_oSi5351.setOutputEnable(1 << SI5351_CLK0);
    }
  }
}

void loop() {
}

