#ifndef _SI5351_H_
#define _SI5351_H_

/* Si5351 Driver by AZO */

#include <stdint.h>
#include <stdbool.h>

//#define SI5351_USE_BERKELEY_SOFTFLOAT_3

#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
#ifdef __cplusplus 
extern "C" {
#endif
#include <softfloat.h>
#ifdef __cplusplus 
}
#endif
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Si5351 type 0:A / 1:B / 2:C */
/*
  Si5351A—XTAL only
  Si5351B—XTAL + VCXO
  Si5351C—XTAL + CLKIN
*/
#define SI5351_TYPE 0

/* Si5351A number of CLK is 3 or 8 */
/* Si5351B/C number of CLK is 8 */
#define SI5351_CLKNUM 3

/* Si5351 input XTAL frequency 25MHz or 27MHz */
#define SI5351_XTAL_FREQ 25000000
#if SI5351_TYPE == 2
/* Si5351C input CLKIN frequency 10MHz - 100MHz */
#define SI5351_CLKIN_FREQ 25000000
#endif

/* check */
#ifndef SI5351_TYPE
#error Not defined SI5351_TYPE
#else
#if SI5351_TYPE != 0 && SI5351_TYPE != 1 && SI5351_TYPE != 2
#error Invalid SI5351_TYPE value
#endif
#endif

#ifndef SI5351_CLKNUM
#error Not defined SI5351_CLKNUM
#else
#if SI5351_TYPE == 0
#if SI5351_CLKNUM != 3 && SI5351_CLKNUM != 8
#error Invalid SI5351_CLKNUM value
#endif
#else
#if SI5351_CLKNUM != 8
#error Invalid SI5351_CLKNUM value
#endif
#endif
#endif

#ifndef SI5351_XTAL_FREQ
#error Not defined SI5351_XTAL_FREQ
#else
#if SI5351_XTAL_FREQ != 25000000 && SI5351_XTAL_FREQ != 27000000
#error Invalid SI5351_CLKNUM value
#endif
#endif

#if SI5351_TYPE == 2
#ifndef SI5351_CLKIN_FREQ
#error Not defined SI5351_CLKIN_FREQ
#else
#if SI5351_CLKIN_FREQ < 10000000 || SI5351_CLKIN_FREQ > 100000000
#error Invalid SI5351_CLKIN_FREQ value
#endif
#endif
#endif

typedef enum Si5351_CLKNo_t_ {
  SI5351_CLK0 = 0,
  SI5351_CLK1,
#if SI5351_CLKNUM <= 3
  SI5351_CLK2
#else
  SI5351_CLK2,
  SI5351_CLK3,
  SI5351_CLK4,
  SI5351_CLK5,
  SI5351_CLK6,
  SI5351_CLK7
#endif
} Si5351_CLKNo_t;

/*
  Status

  SYS_INIT : System Initialization Status
    0: System initialization is complete. Device is ready.
    1: Device is in system initialization mode.
  LOL_B : PLLB Loss Of Lock Status
    0: PLL B is locked.
    1: PLL B is unlocked.
      When the device is in this state it will trigger an interrupt
      causing the INTR pin to go low (Si5351C only).
  LOL_A : PLLA Loss Of Lock Status
    0: PLL A is locked.
    1: PLL A is unlocked.
      When the device is in this state it will trigger an interrupt
      causing the INTR pin to go low (Si5351C only).
  LOS : CLKIN Loss Of Signal (Si5351C Only)
    0: Valid clock signal at the CLKIN pin.
    1: Loss of signal detected at the CLKIN pin.
*/

typedef struct Si5351_Status_t_ {
  bool bSYS_INIT;
#if SI5351_TYPE == 2
  bool bLOL_B;
  bool bLOL_A;
  bool bLOS;
#endif
} Si5351_Status_t;

/* Control */

#if SI5351_TYPE == 2
/*
  CLKIN_DIV : CLKIN Input Divider
    00 : Divide by 1.
    01 : Divide by 2.
    10 : Divide by 4.
    11 : Divide by 8.
*/
typedef enum Si5351_CLKIN_DIV_t_ {
  SI5351_CLKIN_DIV_DIVBY1 = 0,
  SI5351_CLKIN_DIV_DIVBY2,
  SI5351_CLKIN_DIV_DIVBY4,
  SI5351_CLKIN_DIV_DIVBY8
} Si5351_CLKIN_DIV_t;
#endif

/* PLL_SRC : Input Source Select for PLLA/PLLB 0:XTAL / 1:CLKIN */
typedef enum Si5351_PLL_SRC_t_ {
#if SI5351_TYPE != 2
  SI5351_PLL_SRC_XTAL = 0
#else
  SI5351_PLL_SRC_XTAL = 0,
  SI5351_PLL_SRC_CLKIN
#endif
} Si5351_PLL_SRC_t;

/* MSx_INT : MultiSynth x Integer Mode 0:fractional division / 1:integer */
typedef enum Si5351_MS_INT_t_ {
  SI5351_MS_INT_FRACTIONAL_DIVISION = 0,
  SI5351_MS_INT_INTEGER
} Si5351_MS_INT_t;

/* MSx_SRC : MultiSynth Source Select for CLKx 0:PLLA / 1:PLLB or VCXO */
typedef enum Si5351_CLK_MS_SRC_t_ {
  SI5351_CLK_MS_SRC_PLLA = 0,
  SI5351_CLK_MS_SRC_PLLB_VCXO
} Si5351_CLK_MS_SRC_t;

/*
  CLKx_SRC  : Output Clock x Input Source

  CLK0_SRC :
    00:XTAL / 01:CLKIN / 10:(Reserved)   / 11:MultiSynth 0
  CLK1_SRC :
    00:XTAL / 01:CLKIN / 10:MultiSynth 0 / 11:MultiSynth 1
  CLK2_SRC :
    00:XTAL / 01:CLKIN / 10:MultiSynth 0 / 11:MultiSynth 2
  CLK3_SRC :
    00:XTAL / 01:CLKIN / 10:MultiSynth 0 / 11:MultiSynth 3
  CLK4_SRC :
    00:XTAL / 01:CLKIN / 10:(Reserved)   / 11:MultiSynth 4
  CLK5_SRC :
    00:XTAL / 01:CLKIN / 10:MultiSynth 4 / 11:MultiSynth 5
  CLK6_SRC :
    00:XTAL / 01:CLKIN / 10:MultiSynth 4 / 11:MultiSynth 6
  CLK7_SRC :
    00:XTAL / 01:CLKIN / 10:MultiSynth 4 / 11:MultiSynth 7
*/
typedef enum Si5351_CLK_SRC_t_ {
#if SI5351_TYPE != 2
  SI5351_CLK_SRC_XTAL = 0,
  SI5351_CLK_SRC_ALTMS = 2,
  SI5351_CLK_SRC_MS
#else
  SI5351_CLK_SRC_XTAL = 0,
  SI5351_CLK_SRC_CLKIN,
  SI5351_CLK_SRC_ALTMS,
  SI5351_CLK_SRC_MS
#endif
} Si5351_CLK_SRC_t;

/*
  CLKx_IDRV : CLKx Output Rise and Fall time / Drive Strength Control
    00:2mA / 01:4mA / 10:6mA / 11:8mA
*/
typedef enum Si5351_CLK_IDRV_t_ {
  SI5351_CLK_IDRV_2MA = 0,
  SI5351_CLK_IDRV_4MA,
  SI5351_CLK_IDRV_6MA,
  SI5351_CLK_IDRV_8MA
} Si5351_CLK_IDRV_t;

/*
  CLKx_DIS_STATE : Output Clock x Disable State
    00:CLKx is set to a LOW state when disabled.
    01:CLKx is set to a HIGH state when disabled.
    10:CLKx is set to a HIGH IMPEDANCE state when disabled.
    11:CLKx is NEVER DISABLED.
*/
typedef enum Si5351_CLK_DIS_STATE_t_ {
  SI5351_CLK_DIS_STATE_LOW_ = 0,
  SI5351_CLK_DIS_STATE_HIGH,
  SI5351_CLK_DIS_STATE_HIGH_IMP,
  SI5351_CLK_DIS_STATE_NEVER
} Si5351_CLK_DIS_STATE_t;

/*
  Multisynth Divider

  PLL:
    XTAL  : Freq_VCO = Freq_XTAL * (a+b/c)
    CLKIN : Freq_VCO = (Freq_CLKIN / CLKIN_DIV) * (a+b/c)  (only Si5351C)

    MSNx_P1[17:0] = 128 * a + int(128 * (b / c) - 512
    MSNx_P2[19:0] = 128 * b - c * int(128 * (b / c))
    MSNx_P3[19:0] = c

  Output:
    Freq_OUT = Freq_VCO / (MS * R)
*/
typedef enum Si5351_MS_DIV_t_ {
  SI5351_MS_DIV_BY1 = 0,
  SI5351_MS_DIV_BY2,
  SI5351_MS_DIV_BY4,
  SI5351_MS_DIV_BY8,
  SI5351_MS_DIV_BY16,
  SI5351_MS_DIV_BY32,
  SI5351_MS_DIV_BY64,
  SI5351_MS_DIV_BY128
} Si5351_MS_DIV_t;

typedef struct Si5351_MS_t_ {
  bool            bInteger;
  uint32_t        u32MSX_P1;
  uint32_t        u32MSX_P2;
  uint32_t        u32MSX_P3;
  Si5351_MS_DIV_t tDIV;
  bool            bDivBy4;
} Si5351_MS_t;

/*
  Spread Spectrum Parameters

  SSC_EN     : Spread Spectrum Mode 0:Disable / 1:Enable
  SSC_MODE   : Spread Spectrum Enable 0:Down Spread / 1:Center Spread
  SSDN_P1    : PLL A Spread Spectrum Down Parameter 1
  SSDN_P2    : PLL A Spread Spectrum Down Parameter 2
  SSDN_P3    : PLL A Spread Spectrum Down Parameter 3
  SSUDP      : PLL A Spread Spectrum Up/Down Parameter
  SSUP_P1    : PLL A Spread Spectrum Up Parameter 1
  SSUP_P2    : PLL A Spread Spectrum Up Parameter 2
  SSUP_P3    : PLL A Spread Spectrum Up Parameter 3
*/
typedef enum Si5351_SSP_Mode_t_ {
  SI5351_SSP_MODE_DOWN = 0,
  SI5351_SSP_MODE_CENTER
} Si5351_SSP_Mode_t;

typedef struct Si5351_SSP_t_ {
  Si5351_SSP_Mode_t tSSC_MODE;
  uint16_t          u16SSDN_P1;
  uint16_t          u16SSDN_P2;
  uint16_t          u16SSDN_P3;
  uint16_t          u16SSUDP;
  uint16_t          u16SSUP_P1;
  uint16_t          u16SSUP_P2;
  uint16_t          u16SSUP_P3;
} Si5351_SSP_t;

/*
  Crystal Internal Load Capacitance
    00: Reserved. Do not select this option.
    01: Internal CL = 6pF.
    10: Internal CL = 8pF.
    11: Internal CL = 10pF (default).
*/
typedef enum Si5351_XTALLoadCap_t_ {
  SI5351_XTALLOADCAP_6PF = 1,
  SI5351_XTALLOADCAP_8PF,
  SI5351_XTALLOADCAP_10PF
} Si5351_XTALLoadCap_t;


/* Access */
typedef uint8_t (*Si5351_Read_t)(void* pInstance, const uint8_t u8Reg);
typedef void (*Si5351_Write_t)(void* pInstance, const uint8_t u8Reg, uint8_t u8Value);

/* Si5351 */
typedef struct Si5351_t_ {
  void*          pInstance;
  Si5351_Read_t  tRead;
  Si5351_Write_t tWrite;
} Si5351_t;

/* Initialize */
bool Si5351_Initialize(Si5351_t* ptSi5351, void* pInstance, const Si5351_Read_t tRead, const Si5351_Write_t tWrite);
bool Si5351_InitDevice(const Si5351_t* ptSi5351);

/* Status */
bool Si5351_GetStatus(Si5351_Status_t* ptStatus, const Si5351_t* ptSi5351);
bool Si5351_GetStickyStatus(Si5351_Status_t* ptStatus, const Si5351_t* ptSi5351);
bool Si5351_SetStickyStatus(const Si5351_t* ptSi5351, const Si5351_Status_t* ptStatus);
bool Si5351_GetStatusMask(Si5351_Status_t* ptStatus, const Si5351_t* ptSi5351);
bool Si5351_SetStatusMask(const Si5351_t* ptSi5351, const Si5351_Status_t* ptStatus);

/* Control */

/* Output Enable Control */
bool Si5351_GetOutputEnable(uint8_t* pu8Disable, const Si5351_t* ptSi5351);
bool Si5351_SetOutputEnable(const Si5351_t* ptSi5351, const uint8_t u8Enable);
bool Si5351_SetOutputDisable(const Si5351_t* ptSi5351, const uint8_t u8Disable);
bool Si5351_GetOutputControlMask(uint8_t* pu8ControlMask, const Si5351_t* ptSi5351);
bool Si5351_SetOutputControlMask(const Si5351_t* ptSi5351, const uint8_t u8ControlMask);

#if SI5351_TYPE == 2
/* CLKIN_DIV : CLKIN Input Divider */
bool Si5351_GetCLKIN_DIV(Si5351_CLKIN_DIV_t* ptCLKIN_DIV, const Si5351_t* ptSi5351);
bool Si5351_SetCLKIN_DIV(const Si5351_t* ptSi5351, const Si5351_CLKIN_DIV_t tCLKIN_DIV);
#endif

/* PLL_SRC : Input Source Select for PLLA/PLLB */
bool Si5351_GetPLLA_SRC(Si5351_PLL_SRC_t* ptSrc, const Si5351_t* ptSi5351);
bool Si5351_SetPLLA_SRC(const Si5351_t* ptSi5351, const Si5351_PLL_SRC_t tSrc);
bool Si5351_GetPLLB_SRC(Si5351_PLL_SRC_t* ptSrc, const Si5351_t* ptSi5351);
bool Si5351_SetPLLB_SRC(const Si5351_t* ptSi5351, const Si5351_PLL_SRC_t tSrc);

/* CLKx_PDN : Clock x Power Down */
bool Si5351_GetClkPowerDown(bool* pbPowerDown, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetClkPowerDown(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const bool bPowerDown);

/* MSx_SRC : MultiSynth Source Select for CLKx */
bool Si5351_GetClkMSSource(Si5351_CLK_MS_SRC_t* ptSrc, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetClkMSSource(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_MS_SRC_t tSrc);

/* CLKx_INV : Output Clock x Invert */
bool Si5351_GetClkInvert(bool* pbInvert, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetClkInvert(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const bool bInvert);

/* CLKx_SRC : Output Clock x Input Source */
bool Si5351_GetClkSrc(Si5351_CLK_SRC_t* ptSrc, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetClkSrc(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_SRC_t tSrc);

/* CLKx_IDRV : CLKx Output Rise and Fall time / Drive Strength */
bool Si5351_GetClkIDrive(Si5351_CLK_IDRV_t* ptIDrv, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetClkIDrive(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_IDRV_t tIDrv);

/* CLKx_DIS_STATE : Output Clock x Disable State */
bool Si5351_GetClkDisableState(Si5351_CLK_DIS_STATE_t* ptDisState, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetClkDisableState(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_DIS_STATE_t tDisState);

/* Multisynth Divider */
bool Si5351_GetMSA(Si5351_MS_t* ptMS, const Si5351_t* ptSi5351);
bool Si5351_SetMSA(const Si5351_t* ptSi5351, const Si5351_MS_t* ptMS);
bool Si5351_GetMSB(Si5351_MS_t* ptMS, const Si5351_t* ptSi5351);
bool Si5351_SetMSB(const Si5351_t* ptSi5351, const Si5351_MS_t* ptMS);
bool Si5351_GetMS(Si5351_MS_t* ptMS, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetMS(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_MS_t* ptMS);

/* Spread Spectrum Parameters */
bool Si5351_GetSSPEnable(bool* pbEnable, const Si5351_t* ptSi5351);
bool Si5351_SetSSPEnable(const Si5351_t* ptSi5351);
bool Si5351_SetSSPDisable(const Si5351_t* ptSi5351);
bool Si5351_GetSSP(Si5351_SSP_t* ptSSP, const Si5351_t* ptSi5351);
bool Si5351_SetSSP(const Si5351_t* ptSi5351, const Si5351_SSP_t* ptSSP);

#if SI5351_TYPE == 1
/* VCXO */
/* VCXO_Param : VCXO Parameter */
bool Si5351_GetVCXO(uint32_t* pu32VCXO, const Si5351_t* ptSi5351);
bool Si5351_SetVCXO(const Si5351_t* ptSi5351, const uint32_t u32VCXO);
#endif

/* Phase Offset */
/* CLK0-5_PHOFF : Clock 0-5 Initial Phase Offset */
bool Si5351_GetPhaseOffset(uint8_t* pu8PhOff, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo);
bool Si5351_SetPhaseOffset(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const uint8_t u8PhOff);

/* PLL soft reset */
bool Si5351_PLLSoftReset(const Si5351_t* ptSi5351);

/* Crystal Internal Load Capacitance */
bool Si5351_GetXTALLoadCap(Si5351_XTALLoadCap_t* ptXTALLoadCap, const Si5351_t* ptSi5351);
bool Si5351_SetXTALLoadCap(const Si5351_t* ptSi5351, const Si5351_XTALLoadCap_t tXTALLoadCap);

/* Utility */
#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
bool Si5351_CalcMSPLL(Si5351_MS_t* ptMS, const float64_t f64BaseClock, float64_t f64Output);
bool Si5351_DecalcMSPLL(float64_t* pf64Output, const float64_t f64BaseClock, const Si5351_MS_t* ptMS);
bool Si5351_CalcMSClk(Si5351_MS_t* ptMS, const float64_t f64BaseClock, const float64_t f64Output);
bool Si5351_DecalcMSClk(float64_t* pf64Output, const float64_t f64BaseClock, const Si5351_MS_t* ptMS);

bool Si5351_CalcSSP(
  Si5351_SSP_t* ptSSP,
  const Si5351_SSP_Mode_t tSSC_MODE,
  const float64_t f64Freq_PFD,
  const float64_t f64Ratio,
  const float64_t f64SscAmp
);

#if SI5351_TYPE == 1
bool Si5351_CalcVCXO(uint32_t* pu32VCXO, const float64_t f64Ratio, const uint8_t u8APR);
#endif
#else  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */
bool Si5351_CalcMSPLL(Si5351_MS_t* ptMS, const double dBaseClock, const double dOutput);
bool Si5351_DecalcMSPLL(double* pdOutput, const double dBaseClock, const Si5351_MS_t* ptMS);
bool Si5351_CalcMSClk(Si5351_MS_t* ptMS, const double dBaseClock, const double dOutput);
bool Si5351_DecalcMSClk(double* pdOutput, const double dBaseClock, const Si5351_MS_t* ptMS);

bool Si5351_CalcSSP(
  Si5351_SSP_t* ptSSP,
  const Si5351_SSP_Mode_t tSSC_MODE,
  const double dFreq_PFD,
  const double dRatio,
  const double dSscAmp
);

#if SI5351_TYPE == 1
bool Si5351_CalcVCXO(uint32_t* pu32VCXO, const double dRatio, const uint8_t u8APR);
#endif
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SI5351_H_ */

