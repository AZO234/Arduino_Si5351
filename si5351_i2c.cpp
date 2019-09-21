#include <stdio.h>
#include "si5351_i2c.h"

#define SI5351_I2C_ADDRESS 0x60

Si5351_I2C::Si5351_I2C() {
}

bool Si5351_I2C::initialize(
  const Si5351_I2C_BeginTransmission_t tBeginTransmission,
  const Si5351_I2C_RequestFrom_t       tRequestFrom,
  const Si5351_I2C_Read_t              tRead,
  const Si5351_I2C_Write_t             tWrite,
  const Si5351_I2C_EndTransmission_t   tEndTransmission
) {
  bool bValid = false;

  this->tBeginTransmission = NULL;
  this->tRequestFrom = NULL;
  this->tRead = NULL;
  this->tWrite = NULL;
  this->tEndTransmission = NULL;

  if(tBeginTransmission && tRequestFrom && tRead && tWrite && tEndTransmission) {
    this->tBeginTransmission = tBeginTransmission;
    this->tRequestFrom = tRequestFrom;
    this->tRead = tRead;
    this->tWrite = tWrite;
    this->tEndTransmission = tEndTransmission;

    bValid = Si5351_Initialize(&this->tSi5351, this, Si5351_I2C::gen_read, Si5351_I2C::gen_write);
  }

  return bValid;
}

/* Access */
uint8_t Si5351_I2C::gen_read(void* pSi5351_I2C, const uint8_t u8Reg) {
  uint8_t u8Value = 0;

  if(pSi5351_I2C) {
    if(
      ((Si5351_I2C*)pSi5351_I2C)->tBeginTransmission &&
      ((Si5351_I2C*)pSi5351_I2C)->tRequestFrom &&
      ((Si5351_I2C*)pSi5351_I2C)->tRead &&
      ((Si5351_I2C*)pSi5351_I2C)->tWrite &&
      ((Si5351_I2C*)pSi5351_I2C)->tEndTransmission
    ) {
      ((Si5351_I2C*)pSi5351_I2C)->tBeginTransmission(SI5351_I2C_ADDRESS);
      ((Si5351_I2C*)pSi5351_I2C)->tWrite(u8Reg);
      ((Si5351_I2C*)pSi5351_I2C)->tEndTransmission();
      ((Si5351_I2C*)pSi5351_I2C)->tRequestFrom(SI5351_I2C_ADDRESS, 1);
      u8Value = ((Si5351_I2C*)pSi5351_I2C)->tRead(SI5351_I2C_ADDRESS);
    }
  }

  return u8Value;
}

void Si5351_I2C::gen_write(void* pSi5351_I2C, const uint8_t u8Reg, const uint8_t u8Value) {
  if(pSi5351_I2C) {
    if(
      ((Si5351_I2C*)pSi5351_I2C)->tBeginTransmission &&
      ((Si5351_I2C*)pSi5351_I2C)->tWrite &&
      ((Si5351_I2C*)pSi5351_I2C)->tEndTransmission
    ) {
      ((Si5351_I2C*)pSi5351_I2C)->tBeginTransmission(SI5351_I2C_ADDRESS);
      ((Si5351_I2C*)pSi5351_I2C)->tWrite(u8Reg);
      ((Si5351_I2C*)pSi5351_I2C)->tWrite(u8Value);
      ((Si5351_I2C*)pSi5351_I2C)->tEndTransmission();
    }
  }
}

bool Si5351_I2C::initDevice(void) {
  return Si5351_InitDevice(&this->tSi5351);
}

/* Status */

/* Reg.0-2 Status */
bool Si5351_I2C::getStatus(Si5351_Status_t* ptStatus) {
  return Si5351_GetStatus(ptStatus, &this->tSi5351);
}

bool Si5351_I2C::getStickyStatus(Si5351_Status_t* ptStatus) {
  return Si5351_GetStickyStatus(ptStatus, &this->tSi5351);
}

bool Si5351_I2C::setStickyStatus(const Si5351_Status_t* ptStatus) {
  return Si5351_SetStickyStatus(&this->tSi5351, ptStatus);
}

bool Si5351_I2C::getStatusMask(Si5351_Status_t* ptStatus) {
  return Si5351_GetStatusMask(ptStatus, &this->tSi5351);
}

bool Si5351_I2C::setStatusMask(const Si5351_Status_t* ptStatus) {
  return Si5351_SetStatusMask(&this->tSi5351, ptStatus);
}

/* Control */

/* Reg.3 Output Enable Control */
bool Si5351_I2C::getOutputEnable(uint8_t* pu8Disable) {
  return Si5351_GetOutputEnable(pu8Disable, &this->tSi5351);
}

bool Si5351_I2C::setOutputEnable(const uint8_t u8Enable) {
  return Si5351_SetOutputEnable(&this->tSi5351, u8Enable);
}

bool Si5351_I2C::setOutputDisable(const uint8_t u8Disable) {
  return Si5351_SetOutputDisable(&this->tSi5351, u8Disable);
}

bool Si5351_I2C::getOutputControlMask(uint8_t* pu8ControlMask) {
  return Si5351_GetOutputControlMask(pu8ControlMask, &this->tSi5351);
}

bool Si5351_I2C::setOutputControlMask(const uint8_t u8ControlMask) {
  return Si5351_SetOutputControlMask(&this->tSi5351, u8ControlMask);
}

/* Reg.15 PLL Input Source */
#if SI5351_TYPE == 2
/* CLKIN_DIV : CLKIN Input Divider */
bool Si5351_I2C::getCLKIN_DIV(Si5351_CLKIN_DIV_t* ptCLKIN_DIV) {
  return Si5351_GetCLKIN_DIV(ptCLKIN_DIV, &this->tSi5351);
}

bool Si5351_I2C::setCLKIN_DIV(const Si5351_CLKIN_DIV_t tCLKIN_DIV) {
  return Si5351_SetCLKIN_DIV(&this->tSi5351, tCLKIN_DIV);
}
#endif

/* PLL_SRC : Input Source Select for PLLA/PLLB */
bool Si5351_I2C::getPLLA_SRC(Si5351_PLL_SRC_t* ptSrc) {
  return Si5351_GetPLLA_SRC(ptSrc, &this->tSi5351);
}

bool Si5351_I2C::setPLLA_SRC(const Si5351_PLL_SRC_t tSrc) {
  return Si5351_SetPLLA_SRC(&this->tSi5351, tSrc);
}

bool Si5351_I2C::getPLLB_SRC(Si5351_PLL_SRC_t* ptSrc) {
  return Si5351_GetPLLB_SRC(ptSrc, &this->tSi5351);
}

bool Si5351_I2C::setPLLB_SRC(const Si5351_PLL_SRC_t tSrc) {
  return Si5351_SetPLLB_SRC(&this->tSi5351, tSrc);
}

/* Reg.16-23 Control */
/* CLKx_PDN : Clock x Power Down */
bool Si5351_I2C::getClkPowerDown(bool* pbPowerDown, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetClkPowerDown(pbPowerDown, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setClkPowerDown(const Si5351_CLKNo_t tCLKNo, const bool bPowerDown) {
  return Si5351_SetClkPowerDown(&this->tSi5351, tCLKNo, bPowerDown);
}

/* MSx_SRC : MultiSynth Source Select for CLKx */
bool Si5351_I2C::getClkMSSource(Si5351_CLK_MS_SRC_t* ptSrc, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetClkMSSource(ptSrc, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setClkMSSource(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_MS_SRC_t tSrc) {
  return Si5351_SetClkMSSource(&this->tSi5351, tCLKNo, tSrc);
}

/* CLKx_INV : Output Clock x Invert */
bool Si5351_I2C::getClkInvert(bool* pbInvert, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetClkInvert(pbInvert, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setClkInvert(const Si5351_CLKNo_t tCLKNo, const bool bInvert) {
  return Si5351_SetClkInvert(&this->tSi5351, tCLKNo, bInvert);
}

/* CLKx_SRC : Output Clock x Input Source */
bool Si5351_I2C::getClkSrc(Si5351_CLK_SRC_t* ptSrc, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetClkSrc(ptSrc, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setClkSrc(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_SRC_t tSrc) {
  return Si5351_SetClkSrc(&this->tSi5351, tCLKNo, tSrc);
}

/* CLKx_IDRV : CLKx Output Rise and Fall time / Drive Strength */
bool Si5351_I2C::getClkIDrive(Si5351_CLK_IDRV_t* ptIDrv, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetClkIDrive(ptIDrv, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setClkIDrive(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_IDRV_t tIDrv) {
  return Si5351_SetClkIDrive(&this->tSi5351, tCLKNo, tIDrv);
}

/* Reg.24,25 CLKx Disable State */
/* CLKx_DIS_STATE : Output Clock x Disable State */
bool Si5351_I2C::getClkDisableState(Si5351_CLK_DIS_STATE_t* ptDisState, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetClkDisableState(ptDisState, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setClkDisableState(const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_DIS_STATE_t tDisState) {
  return Si5351_SetClkDisableState(&this->tSi5351, tCLKNo, tDisState);
}

/* Multisynth Divider */

/* Reg.22,26-33 Multisynth NA Parameters */
bool Si5351_I2C::getMSA(Si5351_MS_t* ptMS) {
  return Si5351_GetMSA(ptMS, &this->tSi5351);
}

bool Si5351_I2C::setMSA(const Si5351_MS_t* ptMS) {
  return Si5351_SetMSA(&this->tSi5351, ptMS);
}

bool Si5351_I2C::getMSB(Si5351_MS_t* ptMS) {
  return Si5351_GetMSB(ptMS, &this->tSi5351);
}

bool Si5351_I2C::setMSB(const Si5351_MS_t* ptMS) {
  return Si5351_SetMSB(&this->tSi5351, ptMS);
}

/* Reg.16-21,42-92 Multisynthx Parameters */
bool Si5351_I2C::getMS(Si5351_MS_t* ptMS, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetMS(ptMS, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setMS(const Si5351_CLKNo_t tCLKNo, const Si5351_MS_t* ptMS) {
  return Si5351_SetMS(&this->tSi5351, tCLKNo, ptMS);
}

/* Spread Spectrum Parameters */

/* Reg.149-161 Spread Spectrum Parameters */
bool Si5351_I2C::getSSPEnable(bool* pbEnable) {
  return Si5351_GetSSPEnable(pbEnable, &this->tSi5351);
}

bool Si5351_I2C::setSSPEnable(void) {
  return Si5351_SetSSPEnable(&this->tSi5351);
}

bool Si5351_I2C::setSSPDisable(void) {
  return Si5351_SetSSPDisable(&this->tSi5351);
}

bool Si5351_I2C::getSSP(Si5351_SSP_t* ptSSP) {
  return Si5351_GetSSP(ptSSP, &this->tSi5351);
}

bool Si5351_I2C::setSSP(const Si5351_SSP_t* ptSSP) {
  return Si5351_SetSSP(&this->tSi5351, ptSSP);
}

#if SI5351_TYPE == 1
/* VCXO */

/* Reg.162-164 VCXO Parameter */
bool Si5351_I2C::getVCXO(uint32_t* pu32VCXO) {
  return Si5351_GetVCXO(pu32VCXO, &this->tSi5351);
}

bool Si5351_I2C::setVCXO(const uint32_t u32VCXO) {
  return Si5351_SetVCXO(&this->tSi5351, u32VCXO);
}
#endif

/* Phase Offset */

/* Reg.165-172 CLKx Initial Phase Offset */
/* CLK0-5_PHOFF : Clock 0-5 Initial Phase Offset */
bool Si5351_I2C::getPhaseOffset(uint8_t* pu8PhOff, const Si5351_CLKNo_t tCLKNo) {
  return Si5351_GetPhaseOffset(pu8PhOff, &this->tSi5351, tCLKNo);
}

bool Si5351_I2C::setPhaseOffset(const Si5351_CLKNo_t tCLKNo, const uint8_t u8PhOff) {
  return Si5351_SetPhaseOffset(&this->tSi5351, tCLKNo, u8PhOff);
}

/* Reg.177 PLL soft reset */
bool Si5351_I2C::PLLSoftReset(void) {
  return Si5351_PLLSoftReset(&this->tSi5351);
}

/* Crystal Internal Load Capacitance */

/* Reg.183 Crystal Internal Load Capacitance */
bool Si5351_I2C::getXTALLoadCap(Si5351_XTALLoadCap_t* ptXTALLoadCap) {
  return Si5351_GetXTALLoadCap(ptXTALLoadCap, &this->tSi5351);
}

bool Si5351_I2C::setXTALLoadCap(const Si5351_XTALLoadCap_t tXTALLoadCap) {
  return Si5351_SetXTALLoadCap(&this->tSi5351, tXTALLoadCap);
}

/* Utility */

bool Si5351_I2C::calcMSPLL(Si5351_MS_t* ptMS, double dBaseClock, double dOutput) {
  return Si5351_CalcMSPLL(ptMS, dBaseClock, dOutput);
}

bool Si5351_I2C::decalcMSPLL(double* pdOutput, double dBaseClock, const Si5351_MS_t* ptMS) {
  return Si5351_DecalcMSPLL(pdOutput, dBaseClock, ptMS);
}

bool Si5351_I2C::calcMSClk(Si5351_MS_t* ptMS, double dBaseClock, double dOutput) {
  return Si5351_CalcMSClk(ptMS, dBaseClock, dOutput);
}

bool Si5351_I2C::decalcMSClk(double* pdOutput, double dBaseClock, const Si5351_MS_t* ptMS) {
  return Si5351_DecalcMSClk(pdOutput, dBaseClock, ptMS);
}

bool Si5351_I2C::calcSSP(
  Si5351_SSP_t* ptSSP,
  const bool bEnable,
  const Si5351_SSP_Mode_t tSSC_MODE,
  const double dFreq_PFD,
  const double dRatio,
  const double dSscAmp
) {
  return Si5351_CalcSSP(ptSSP, bEnable, tSSC_MODE, dFreq_PFD, dRatio, dSscAmp);
}

#if SI5351_TYPE == 1
bool Si5351_I2C::calcVCXO(uint32_t* pu32VCXO, const double dRatio, const uint8_t u8APR) {
  return Si5351_CalcVCXO(pu32VCXO, dRatio, u8APR);
}
#endif

