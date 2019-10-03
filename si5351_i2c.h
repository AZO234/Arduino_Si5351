#ifndef _SI5351_I2C_H_
#define _SI5351_I2C_H_

/* Si5351 I2C Driver by AZO */

#include "si5351.h"

typedef void (*Si5351_I2C_BeginTransmission_t)(const uint8_t u8Address);
typedef void (*Si5351_I2C_RequestFrom_t)(const uint8_t u8Address, const uint8_t u8Count);
typedef uint8_t (*Si5351_I2C_Read_t)(const uint8_t u8Address);
typedef void (*Si5351_I2C_Write_t)(const uint8_t u8Value);
typedef void (*Si5351_I2C_EndTransmission_t)(void);

/* Si5351 I2C class */
class Si5351_I2C {
public:
  /* Initialize */
  Si5351_I2C();
  bool initialize(
    Si5351_I2C_BeginTransmission_t tBeginTransmission,
    Si5351_I2C_RequestFrom_t       tRequestFrom,
    Si5351_I2C_Read_t              tRead,
    Si5351_I2C_Write_t             tWrite,
    Si5351_I2C_EndTransmission_t   tEndTransmission,
    Si5351_MemoryBarrier_t         tMemoryBarrier,
    void** ppLock
  );
  bool initDevice(void);

  /* Status */
  bool getStatus(Si5351_Status_t* ptStatus);
  bool getStickyStatus(Si5351_Status_t* ptStatus);
  bool setStickyStatus(const Si5351_Status_t* ptStatus);
  bool getStatusMask(Si5351_Status_t* ptStatus);
  bool setStatusMask(const Si5351_Status_t* ptStatus);

  /* Control */

  /* Output Enable Control */
  bool getOutputEnable(uint8_t* pu8Disable);
  bool setOutputEnable(const uint8_t u8Enable);
  bool setOutputDisable(const uint8_t u8Disable);
  bool getOutputControlMask(uint8_t* pu8ControlMask);
  bool setOutputControlMask(const uint8_t u8ControlMask);

#if SI5351_TYPE == 2
  /* CLKIN_DIV : CLKIN Input Divider */
  bool getCLKIN_DIV(Si5351_CLKIN_DIV_t* ptCLKIN_DIV);
  bool setCLKIN_DIV(const Si5351_CLKIN_DIV_t tCLKIN_DIV);
#endif

  /* PLL_SRC : Input Source Select for PLLA/PLLB */
  bool getPLLA_SRC(Si5351_PLL_SRC_t* ptSrc);
  bool setPLLA_SRC(const Si5351_PLL_SRC_t tSrc);
  bool getPLLB_SRC(Si5351_PLL_SRC_t* ptSrc);
  bool setPLLB_SRC(const Si5351_PLL_SRC_t tSrc);

  /* CLKx_PDN : Clock x Power Down */
  bool getClkPowerDown(bool* pbPowerDown, const Si5351_CLKNo_t tCLKNo);
  bool setClkPowerDown(const Si5351_CLKNo_t tCLKNo, const bool bPowerDown);

  /* MSx_SRC : MultiSynth Source Select for CLKx */
  bool getClkMSSource(Si5351_CLK_MS_SRC_t* ptSrc, const Si5351_CLKNo_t tCLKNo);
  bool setClkMSSource(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_MS_SRC_t tSrc);

  /* CLKx_INV : Output Clock x Invert */
  bool getClkInvert(bool* pbInvert, const Si5351_CLKNo_t tCLKNo);
  bool setClkInvert(const Si5351_CLKNo_t tCLKNo, const bool bInvert);

  /* CLKx_SRC : Output Clock x Input Source */
  bool getClkSrc(Si5351_CLK_SRC_t* ptSrc, const Si5351_CLKNo_t tCLKNo);
  bool setClkSrc(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_SRC_t tSrc);

  /* CLKx_IDRV : CLKx Output Rise and Fall time / Drive Strength */
  bool getClkIDrive(Si5351_CLK_IDRV_t* ptIDrv, const Si5351_CLKNo_t tCLKNo);
  bool setClkIDrive(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_IDRV_t tIDrv);

  /* CLKx_DIS_STATE : Output Clock x Disable State */
  bool getClkDisableState(Si5351_CLK_DIS_STATE_t* ptDisState, const Si5351_CLKNo_t tCLKNo);
  bool setClkDisableState(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_DIS_STATE_t tDisState);

  /* Multisynth Divider */
  bool getMSA(Si5351_MS_t* ptMS);
  bool setMSA(const Si5351_MS_t* ptMS);
  bool getMSB(Si5351_MS_t* ptMS);
  bool setMSB(const Si5351_MS_t* ptMS);
  bool getMS(Si5351_MS_t* ptMS, const Si5351_CLKNo_t tCLKNo);
  bool setMS(const Si5351_CLKNo_t tCLKNo, const Si5351_MS_t* ptMS);

  /* Spread Spectrum Parameters */
  bool getSSPEnable(bool* pbEnable);
  bool setSSPEnable(void);
  bool setSSPDisable(void);
  bool getSSP(Si5351_SSP_t* ptSSP);
  bool setSSP(const Si5351_SSP_t* ptSSP);

#if SI5351_TYPE == 1
  /* VCXO */
  bool getVCXO(uint32_t* pu32VCXO);
  bool setVCXO(const uint32_t u32VCXO);
#endif

  /* Phase Offset */
  bool getPhaseOffset(uint8_t* pu8PhOff, const Si5351_CLKNo_t tCLKNo);
  bool setPhaseOffset(const Si5351_CLKNo_t tCLKNo, const uint8_t u8PhOff);

  /* PLL soft reset */
  bool PLLSoftReset(void);

  /* Crystal Internal Load Capacitance */
  bool getXTALLoadCap(Si5351_XTALLoadCap_t* ptXTALLoadCap);
  bool setXTALLoadCap(const Si5351_XTALLoadCap_t tXTALLoadCap);

  /* Utility */
#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
  static bool calcMSPLL(Si5351_MS_t* ptMS, const float64_t f64BaseClock, const float64_t f64Output);
  static bool decalcMSPLL(float64_t* pf64Output, const float64_t f64BaseClock, const Si5351_MS_t* ptMS);
  static bool calcMSClk(Si5351_MS_t* ptMS, const float64_t f64BaseClock, const float64_t f64Output);
  static bool decalcMSClk(float64_t* pf64Output, const float64_t f64BaseClock, const Si5351_MS_t* ptMS);

  static bool calcSSP(
    Si5351_SSP_t* ptSSP,
    const Si5351_SSP_Mode_t tSSC_MODE,
    const float64_t f64Freq_PFD,
    const float64_t f64Ratio,
    const float64_t f64SscAmp
  );

#if SI5351_TYPE == 1
  static bool calcVCXO(uint32_t* pu32VCXO, const float64_t f64Ratio, const uint8_t u8APR);
#endif
#else  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */
  static bool calcMSPLL(Si5351_MS_t* ptMS, const double dBaseClock, const double dOutput);
  static bool decalcMSPLL(double* pdOutput, const double dBaseClock, const Si5351_MS_t* ptMS);
  static bool calcMSClk(Si5351_MS_t* ptMS, const double dBaseClock, const double dOutput);
  static bool decalcMSClk(double* pdOutput, const double dBaseClock, const Si5351_MS_t* ptMS);

  static bool calcSSP(
    Si5351_SSP_t* ptSSP,
    const Si5351_SSP_Mode_t tSSC_MODE,
    const double dFreq_PFD,
    const double dRatio,
    const double dSscAmp
  );

#if SI5351_TYPE == 1
  static bool calcVCXO(uint32_t* pu32VCXO, const double dRatio, const uint8_t u8APR);
#endif
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */

  /* Access (Don't use) */
  static uint8_t gen_read(void* pSi5351_I2C, const uint8_t u8Reg);
  static void gen_write(void* pSi5351_I2C, const uint8_t u8Reg, const uint8_t u8Value);

private:
  Si5351_t tSi5351;
  Si5351_I2C_BeginTransmission_t tBeginTransmission;
  Si5351_I2C_RequestFrom_t       tRequestFrom;
  Si5351_I2C_Read_t              tRead;
  Si5351_I2C_Write_t             tWrite;
  Si5351_I2C_EndTransmission_t   tEndTransmission;
};

#endif  /* _SI5351_I2C_H_ */

