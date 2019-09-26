/* Si5351 Driver by AZO */

#include "si5351.h"

/*
  1. PLL Selection

    If spread spectrum(SSP) is not enabled, either of the two PLLs may be used as the source.
    If both XTAL and CLKIN input options are used simultaneously (Si5351C only),
    then one PLL must be reserved for use with CLKIN and one for use with the XTAL.
    PLLA must be used for any SSP-enabled outputs. PLLB must be used for any VCXO outputs.

  2. Selecting VCO Frequencies and Divide Ratios

    Valid Multisynth(MS) divider ratios are 4, 6, 8.

    8+1/1048575 <= Freq_VCO <= 900+0/1048575

    If possible, make both output and feedback MS ratios integers.
    Try to select as many integer output MS ratios as possible.

  3. Output Clock Pin Assignment (Optional)

    Some user may wish to have full control of output clock pin assignment.
    Isolate unique output frequencies on individual VDDO bank if possible.

  4. PLL Input Source

    XTAL : 25MHz or 27MHz
    CMOS : 10MHz <= Freq_CLKIN <= 40MHz  (Si5351C only)

    Freq_VCO = Freq_XTAL * (a+b/c)
    Freq_VCO = (Freq_CLKIN / CLKIN_DIV) * (a+b/c)  (Si5351C only)

    15+0/1048575 <= (a+b/c) <= 90+0/1048575

    MSNA_P1[17:0] or MSNB_P1[17:0] = 128 * a + floor(128 * (b / c)) - 512
    MSNA_P2[19:0] or MSNB_P2[19:0] = 128 * b - c * floor(128 * (b / c))
    MSNA_P3[19:0] or MSNB_P3[19:0] = c

    SSP is only supported by PLLA.
    VCXO functionality is only supported by PLLB.
    When using the VCXO function, set the MSNB divide c = 106.

    If (a+b/c) is an even integer,
    integer mode may be enabled for PLLA or PLLB
    by setting parameter FBA_INT or FBB_INT respectively.

  5. Output Multisynth(MS) 0-7 Settings

    * Freq_OUT <= 150MHz

      MS0-MS5 : fractional divides
      MS6-MS7 : even-integer dividers

      Each of these dividers can be set to use PLLA or PLLB

      6 <= (a+b/c) <= 1800

      MS0-5_P1[17:0] = 128 * a + floor(128 * (b / c)) - 512
      MS0-5_P2[19:0] = 128 * b - c * floor(128 * (b / c))
      MS0-5_P3[19:0] = c

      MS6 and MS7 are integer-only dividers.
      The valid range is all even integers between 6 and 254 inclusive.

    * 150MHz < Freq_OUT <= 200MHz

      Output frequencies greater than 150MHz are available on MS0-5.
      For this frequency range a divide value of 4 must be used by setting
      MS0-5_P1 = 0
      MS0-5_P2 = 0
      MS0-5_P3 = 1
      MS0-5_INT = 1
      MS0-5_DIVBY4[1:0] = 11b.

  6. Output Driver Settings

    The R dividers can be used to generate frequencies below about 500 kHz.
    R divider can be set to 1, 2, 4, 8,....128
    by writing the proper setting for R0-7_DIV.

    When a clock is disabled via the OEB pin or OEB control register,
    the output driver may be set to present
    logic low, logic high, or high-impedance at the device pin.

    Any unused clock outputs may be powered down by setting CLK0-7_PDN=1.

  7. Configuring Spread Spectrum(SSP)

    SSP can be enabled on any MS output that uses PLLA as its reference.

    include –0.1% to –2.5% down spread and
    up to plus-minus 1.5% center spread.

    Freq_PFDA : input frequency to PLLA
    (a+b/c)   : PLLA MS radio
    sscAMP    : Spread amplitude
      (down or center spread amplitude of 1%, sscAmp = 0.01)

    * Down Spread

      Up/Down Parameter
        SSUDP[11:0] = floor(Freq_PFDA / 4 * 31500)
      Intermediate Equation
        SSDN = 64 * (a+b/c) * sscAMP / ((1+sscAMP) * SSUDP)
      Up-Spread Parameters
        SSUP_P1 = 0
        SSUP_P2 = 0
        SSUP_P3 = 1
      Down-Spread Parameters
        SSDN_P1[11:0] = floor(SSDN)
        SSDN_P2[14:0] = 32767 * (SSDN - SSDN_P1)
        SSDN_P3[14:0] = 32767

    * Center Spread

      Up/Down Parameter
        SSUDP[11:0] = floor(Freq_PFDA / 4 * 31500)
      Intermediate Equation
        SSUP = 128 * (a+b/c) * sscAMP / ((1+sscAMP) * SSUDP)
        SSDN = 128 * (a+b/c) * sscAMP / ((1+sscAMP) * SSUDP)
      Up-Spread Parameters
        SSUP_P1[11:0] = floor(SSUP)
        SSUP_P2[14:0] = 32767 * (SSUP - SSUP_P1)
        SSUP_P3[14:0] = 32767
      Down-Spread Parameters
        SSDN_P1[11:0] = floor(SSDN)
        SSDN_P2[14:0] = 32767 * (SSDN - SSDN_P1)
        SSDN_P3[14:0] = 32767

    SSP Enable control pin is available.  (on Si5351A/B)
    SSP enable functionality is a logical OR of
    the SSEN pin and SSC_EN register bit.
    so for the SSEN pin to work properly,
    the SSC_EN register bit must be set to 0.

  8. Configuring Initial Phase Offset

    Outputs0-5 can be programmed with an independent initial phase offset.
    The phase offset parameter is an unsigned integer
    where each LSB represents a phase difference of a quarter of the VCO period,
    TVCO / 4.

    CLK0-5_PHOFF[4:0] = round(Offset(sec) * 4 * Freq_VCO)

  9. Configuring VCXO  (Si5351B only)

    The Si5351B combines free-running clock generation and a VCXO.
    The VCXO eliminates the need for an external pullable crystal.
    The "pulling" is done at PLLB.
    Only a standard, low cost, fixed-frequency(25or27 MHz) AT-cut crystal
    is required and is used as the reference source for both PLLA and PLLB.
    PLLB must be used as the source for any VCXO output clock.
    Feedback B MS divider ratio must be set such that the denominator,
    c(in the fractional divider (a+b/c)) is fixed to 10^6.
    Note that 1.03 is a margining factor to ensure the full desired
    pull range is achieved.
    For a desired pull-range of plus-minus 30ppm,
    the value APR in the equation below is 30,
    for plus-minus 60ppm APR is 60.

    VCXO_Param[21:0] = 1.03 * (128 * (a+b/c)) * APR
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Initialize */

bool Si5351_Initialize(Si5351_t* ptSi5351, void* pInstance, const Si5351_Read_t tRead, const Si5351_Write_t tWrite) {
  bool bValid = false;

  if(ptSi5351 && pInstance && tRead && tWrite) {
    bValid = true;
    ptSi5351->pInstance = pInstance;
    ptSi5351->tRead  = tRead;
    ptSi5351->tWrite = tWrite;
  }

  return bValid;
}

bool Si5351_InitDevice(const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Reg;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;

      /* Disable Outputs */
      /* Set CLKx_DIS high; Reg.3 = 0xFF */
      ptSi5351->tWrite(ptSi5351->pInstance, 0x03, 0xFF);

      /* Powerdown all output drivers */
      /* Reg.16,17,18,19,20,21,22,23 = 0x80 */
      for(u8Reg = 0x10; u8Reg <= 0x17; u8Reg++) {
        ptSi5351->tWrite(ptSi5351->pInstance, u8Reg, 0x80);
      }

      /* Set interrupt masks */
      ptSi5351->tWrite(ptSi5351->pInstance, 0x02, 0xF0);
    }
  }

/*
Write new configuration to device using
the contents of the register map. This
step also powers up the output drivers.
(Registers 15-92 and 149-170)
Apply PLLA and PLLB soft reset
Reg. 177 = 0xAC
Enable desired outputs
*/

  return bValid;
}

/* Status */

bool Si5351_GetStatus(Si5351_Status_t* ptStatus, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptStatus && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      u8Value = ptSi5351->tRead(ptSi5351->pInstance, 0x00);

      ptStatus->bSYS_INIT = ((u8Value >> 7) & 0x1) ? true : false;
#if SI5351_TYPE == 2
      ptStatus->bLOL_B    = ((u8Value >> 6) & 0x1) ? true : false;
      ptStatus->bLOL_A    = ((u8Value >> 5) & 0x1) ? true : false;
      ptStatus->bLOS      = ((u8Value >> 4) & 0x1) ? true : false;
#endif
    }
  }

  return bValid;
}

bool Si5351_GetStickyStatus(Si5351_Status_t* ptStatus, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptStatus && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      u8Value = ptSi5351->tRead(ptSi5351->pInstance, 0x01);

      ptStatus->bSYS_INIT = ((u8Value >> 7) & 0x1) ? true : false;
#if SI5351_TYPE == 2
      ptStatus->bLOL_B    = ((u8Value >> 6) & 0x1) ? true : false;
      ptStatus->bLOL_A    = ((u8Value >> 5) & 0x1) ? true : false;
      ptStatus->bLOS      = ((u8Value >> 4) & 0x1) ? true : false;
#endif
    }
  }

  return bValid;
}

bool Si5351_SetStickyStatus(const Si5351_t* ptSi5351, const Si5351_Status_t* ptStatus) {
  bool bValid = false;

  if(ptSi5351 && ptStatus) {
    if(ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x01,
#if SI5351_TYPE != 2
        (uint8_t)(ptStatus->bSYS_INIT ? 1 : 0) << 7
#else
        (uint8_t)(ptStatus->bSYS_INIT ? 1 : 0) << 7 |
        (uint8_t)(ptStatus->bLOL_B    ? 1 : 0) << 6 |
        (uint8_t)(ptStatus->bLOL_A    ? 1 : 0) << 5 |
        (uint8_t)(ptStatus->bLOS      ? 1 : 0) << 4
#endif
      );
    }
  }

  return bValid;
}

bool Si5351_GetStatusMask(Si5351_Status_t* ptStatus, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptStatus && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      u8Value = ptSi5351->tRead(ptSi5351->pInstance, 0x02);

      ptStatus->bSYS_INIT = ((u8Value >> 7) & 0x1) ? true : false;
#if SI5351_TYPE == 2
      ptStatus->bLOL_B    = ((u8Value >> 6) & 0x1) ? true : false;
      ptStatus->bLOL_A    = ((u8Value >> 5) & 0x1) ? true : false;
      ptStatus->bLOS      = ((u8Value >> 4) & 0x1) ? true : false;
#endif
    }
  }

  return bValid;
}

bool Si5351_SetStatusMask(const Si5351_t* ptSi5351, const Si5351_Status_t* ptStatus) {
  bool bValid = false;

  if(ptSi5351 && ptStatus) {
    if(ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x02,
#if SI5351_TYPE != 2
        (uint8_t)(ptStatus->bSYS_INIT ? 1 : 0) << 7
#else
        (uint8_t)(ptStatus->bSYS_INIT ? 1 : 0) << 7 |
        (uint8_t)(ptStatus->bLOL_B    ? 1 : 0) << 6 |
        (uint8_t)(ptStatus->bLOL_A    ? 1 : 0) << 5 |
        (uint8_t)(ptStatus->bLOS      ? 1 : 0) << 4
#endif
      );
    }
  }

  return bValid;
}

/* Control */

/* Reg.3 Output Enable Control */
bool Si5351_GetOutputEnable(uint8_t* pu8Disable, const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(pu8Disable && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      *pu8Disable = ptSi5351->tRead(ptSi5351->pInstance, 0x03);
    }
  }

  return bValid;
}

bool Si5351_SetOutputEnable(const Si5351_t* ptSi5351, const uint8_t u8Enable) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptSi5351) {
    if(ptSi5351->tWrite) {
      bValid = Si5351_GetOutputEnable(&u8Value, ptSi5351);
      if(bValid) {
#if SI5351_CLKNUM <= 3
        ptSi5351->tWrite(ptSi5351->pInstance, 0x03, (u8Value & ~(u8Enable & 0x7)));
#else
        ptSi5351->tWrite(ptSi5351->pInstance, 0x03, (u8Value & ~u8Enable));
#endif
      }
    }
  }

  return bValid;
}

bool Si5351_SetOutputDisable(const Si5351_t* ptSi5351, const uint8_t u8Disable) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptSi5351) {
    if(ptSi5351->tWrite) {
      bValid = Si5351_GetOutputEnable(&u8Value, ptSi5351);
      if(bValid) {
#if SI5351_CLKNUM <= 3
        ptSi5351->tWrite(ptSi5351->pInstance, 0x03, (u8Value | (u8Disable & 0x7)));
#else
        ptSi5351->tWrite(ptSi5351->pInstance, 0x03, (u8Value | u8Disable));
#endif
      }
    }
  }

  return bValid;
}

/* Reg.9 OEB Pin Enable Control Mask */
bool Si5351_GetOutputControlMask(uint8_t* pu8ControlMask, const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(pu8ControlMask && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
#if SI5351_CLKNUM <= 3
      *pu8ControlMask = (ptSi5351->tRead(ptSi5351->pInstance, 0x09) & 0x7);
#else
      *pu8ControlMask =  ptSi5351->tRead(ptSi5351->pInstance, 0x09);
#endif
    }
  }

  return bValid;
}

bool Si5351_SetOutputControlMask(const Si5351_t* ptSi5351, const uint8_t u8ControlMask) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tWrite) {
      bValid = true;
#if SI5351_CLKNUM <= 3
      ptSi5351->tWrite(ptSi5351->pInstance, 0x09, (u8ControlMask & 0x7));
#else
      ptSi5351->tWrite(ptSi5351->pInstance, 0x09,  u8ControlMask);
#endif
    }
  }

  return bValid;
}

/* Reg.15 PLL Input Source */

#if SI5351_TYPE == 2
/* CLKIN_DIV : CLKIN Input Divider */
bool Si5351_GetCLKIN_DIV(Si5351_CLKIN_DIV_t* ptCLKIN_DIV, const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptCLKIN_DIV && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      *ptCLKIN_DIV = ((ptSi5351->tRead(ptSi5351->pInstance, 0x0F) >> 6) & 0x3);
    }
  }

  return bValid;
}

bool Si5351_SetCLKIN_DIV(const Si5351_t* ptSi5351, const Si5351_CLKIN_DIV_t tCLKIN_DIV) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x0F,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x0F) & ~(0x3 << 6)) | (((uint8_t)tCLKIN_DIV & 0x3) << 6)
      );
    }
  }

  return bValid;
}
#endif

/* PLL_SRC : Input Source Select for PLLA/PLLB */
bool Si5351_GetPLLA_SRC(Si5351_PLL_SRC_t* ptSrc, const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptSrc && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      *ptSrc = (Si5351_PLL_SRC_t)((ptSi5351->tRead(ptSi5351->pInstance, 0x0F) >> 2) & 0x1);
    }
  }

  return bValid;
}

bool Si5351_SetPLLA_SRC(const Si5351_t* ptSi5351, const Si5351_PLL_SRC_t tSrc) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x0F,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x0F) & ~(0x1 << 2)) | (((uint8_t)tSrc & 0x1) << 2)
      );
    }
  }

  return bValid;
}

bool Si5351_GetPLLB_SRC(Si5351_PLL_SRC_t* ptSrc, const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptSrc && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      *ptSrc = (Si5351_PLL_SRC_t)((ptSi5351->tRead(ptSi5351->pInstance, 0x0F) >> 3) & 0x1);
    }
  }

  return bValid;
}

bool Si5351_SetPLLB_SRC(const Si5351_t* ptSi5351, const Si5351_PLL_SRC_t tSrc) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x0F,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x0F) & ~(0x1 << 3)) | (((uint8_t)tSrc & 0x1) << 3)
      );
    }
  }

  return bValid;
}

/* Reg.16-23 CLKx Control */

/* CLKx_PDN : Clock x Power Down */
bool Si5351_GetClkPowerDown(bool* pbPowerDown, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

  if(pbPowerDown && ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && tCLKNo < 8) {
#endif
      bValid = true;
      *pbPowerDown = ((ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) >> 7) & 0x1) ? true : false;
    }
  }

  return bValid;
}

bool Si5351_SetClkPowerDown(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const bool bPowerDown) {
  bool bValid = false;

  if(ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 8) {
#endif
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x10 + tCLKNo,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & ~(0x1 << 7)) | ((uint8_t)(bPowerDown ? 1 : 0) << 7)
      );
    }
  }

  return bValid;
}

/* MSx_SRC : MultiSynth Source Select for CLKx */
bool Si5351_GetClkMSSource(Si5351_CLK_MS_SRC_t* ptSrc, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

  if(ptSrc && ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && tCLKNo < 8) {
#endif
      bValid = true;
      *ptSrc = (Si5351_CLK_MS_SRC_t)((ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) >> 5) & 0x1);
    }
  }

  return bValid;
}

bool Si5351_SetClkMSSource(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_MS_SRC_t tSrc) {
  bool bValid = false;

  if(ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 8) {
#endif
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x10 + tCLKNo,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & ~(0x1 << 5)) | (((uint8_t)tSrc & 0x1) << 5)
      );
    }
  }

  return bValid;
}

/* CLKx_INV : Output Clock x Invert */
bool Si5351_GetClkInvert(bool* pbInvert, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

  if(pbInvert && ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && tCLKNo < 8) {
#endif
      bValid = true;
      *pbInvert = ((ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) >> 4) & 0x1) ? true : false;
    }
  }

  return bValid;
}

bool Si5351_SetClkInvert(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const bool bInvert) {
  bool bValid = false;

  if(ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 8) {
#endif
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x10 + tCLKNo,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & ~(0x1 << 4)) | ((uint8_t)(bInvert ? 1 : 0) << 4)
      );
    }
  }

  return bValid;
}

/* CLKx_SRC : Output Clock x Input Source */
bool Si5351_GetClkSrc(Si5351_CLK_SRC_t* ptSrc, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

  if(ptSrc && ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && tCLKNo < 8) {
#endif
      bValid = true;
      *ptSrc = (Si5351_CLK_SRC_t)((ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) >> 2) & 0x3);
    }
  }

  return bValid;
}

bool Si5351_SetClkSrc(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_SRC_t tSrc) {
  bool bValid = false;

  if(ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 8) {
#endif
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x10 + tCLKNo,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & ~(0x3 << 2)) | (((uint8_t)tSrc & 0x3) << 2)
      );
    }
  }

  return bValid;
}

/* CLKx_IDRV : CLK Output Rise and Fall time / Drive Strength */
bool Si5351_GetClkIDrive(Si5351_CLK_IDRV_t* ptIDrv, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

  if(ptIDrv && ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && tCLKNo < 8) {
#endif
      bValid = true;
      *ptIDrv = (Si5351_CLK_IDRV_t)(ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & 0x3);
    }
  }

  return bValid;
}

bool Si5351_SetClkIDrive(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_IDRV_t tIDrv) {
  bool bValid = false;

  if(ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 8) {
#endif
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x10 + tCLKNo,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & ~0x3) | (tIDrv & 0x3)
      );
    }
  }

  return bValid;
}

/* Reg.24,25 CLKx Disable State */
/* CLKx_DIS_STATE : Output Clock x Disable State */
bool Si5351_GetClkDisableState(Si5351_CLK_DIS_STATE_t* ptDisState, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

  if(ptDisState && ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && tCLKNo < 8) {
#endif
      bValid = true;
      *ptDisState = (Si5351_CLK_DIS_STATE_t)((ptSi5351->tRead(ptSi5351->pInstance, 0x18 + tCLKNo / 4) >> (2 * (tCLKNo % 4))) & 0x3);
    }
  }

  return bValid;
}

bool Si5351_SetClkDisableState(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_CLK_DIS_STATE_t tDisState) {
  bool bValid = false;

  if(ptSi5351) {
#if SI5351_CLKNUM <= 3
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 3) {
#else
    if(ptSi5351->tRead && ptSi5351->tWrite && tCLKNo < 8) {
#endif
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x18 + tCLKNo / 4,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x18 + tCLKNo / 4) & ~(0x3 << (2 * (tCLKNo % 4)))) | (((uint8_t)tDisState & 0x3) << (2 * (tDisState % 4)))
      );
    }
  }

  return bValid;
}

/* Reg.26-92 Multisynth Divider */
/*
  PLL:
    XTAL  : Freq_VCO = Freq_XTAL * (a+b/c)
    CLKIN : Freq_VCO = (Freq_CLKIN / CLKIN_DIV) * (a+b/c)  (only Si5351C)

    MSNx_P1[17:0] = 128 * a + floor(128 * (b / c) - 512
    MSNx_P2[19:0] = 128 * b - c * floor(128 * (b / c))
    MSNx_P3[19:0] = c

    Valid Multisynth divider ratios are 4,6,8.
    8 + 1 / 1048575 <= (a+b/c) <= 900 + 0 / 1048575
    15 <= (a+b/c) <= 90

  Output:
    Freq_OUT = Freq_VCO / (Multisynth * R)

    MSNx_P1[17:0] = 128 * a + floor(128 * (b / c) - 512
    MSNx_P2[19:0] = 128 * b - c * floor(128 * (b / c))
    MSNx_P3[19:0] = c

    6 <= (a+b/c) <= 1800

    Output frequencies greater than 150MHz are available on Multisynths 0-5.
    For this frequency range a divide value of 4 must be used by setting
    
    MSx_P1=0, MSx_P2=0, MSx_P3=1, MSx_INT=1, MSx_DIVBY4[1:0]=11b
*/

/* Reg.22,26-33 Multisynth NA Parameters */
bool Si5351_GetMSA(Si5351_MS_t* ptMS, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Reg;
  uint8_t u8Value[8];

  if(ptMS && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      ptMS->bInteger = ((ptSi5351->tRead(ptSi5351->pInstance, 0x16) >> 6) & 0x1) ? true : false;
      for(u8Reg = 0; u8Reg < 8; u8Reg++) {
        u8Value[u8Reg] = ptSi5351->tRead(ptSi5351->pInstance, 0x1A + u8Reg);
      }
      ptMS->u32MSX_P1 = ((uint32_t)(u8Value[2] & 0x03) << 16) | ((uint32_t)u8Value[3] << 8) | u8Value[4];
      ptMS->u32MSX_P2 = ((uint32_t)(u8Value[5] & 0x0F) << 16) | ((uint32_t)u8Value[6] << 8) | u8Value[7];
      ptMS->u32MSX_P3 = ((uint32_t)(u8Value[5] & 0xF0) << 12) | ((uint32_t)u8Value[0] << 8) | u8Value[1];
      ptMS->tDIV = SI5351_MS_DIV_BY1;
      ptMS->bDivBy4 = false;
    }
  }

  return bValid;
}

bool Si5351_SetMSA(const Si5351_t* ptSi5351, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptSi5351 && ptMS) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x16,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x16) & ~(0x1 << 6)) | ((uint8_t)(ptMS->bInteger ? 1 : 0) << 6)
      );
      ptSi5351->tWrite(ptSi5351->pInstance, 0x1A, ((ptMS->u32MSX_P3 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x1B, ( ptMS->u32MSX_P3        & 0xFF));
      u8Value = ptSi5351->tRead(ptSi5351->pInstance, 0x1C);
      ptSi5351->tWrite(ptSi5351->pInstance, 0x1C, (u8Value & ~0x3) | ((ptMS->u32MSX_P1 >> 16) & 0x3));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x1D, ((ptMS->u32MSX_P1 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x1E, ( ptMS->u32MSX_P1        & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x1F, ((ptMS->u32MSX_P3 >> 12) & 0xF0) | ((ptMS->u32MSX_P2 >> 16) & 0xF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x20, ((ptMS->u32MSX_P2 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x21, ( ptMS->u32MSX_P2        & 0xFF));
    }
  }

  return bValid;
}

/* Reg.23,34-41 Multisynth NB Parameters */
bool Si5351_GetMSB(Si5351_MS_t* ptMS, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Reg;
  uint8_t u8Value[8];

  if(ptMS && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      ptMS->bInteger = ((ptSi5351->tRead(ptSi5351->pInstance, 0x17) >> 6) & 0x1) ? true : false;
      for(u8Reg = 0; u8Reg < 8; u8Reg++) {
        u8Value[u8Reg] = ptSi5351->tRead(ptSi5351->pInstance, 0x22 + u8Reg);
      }
      ptMS->u32MSX_P1 = ((uint32_t)(u8Value[2] & 0x03) << 16) | ((uint32_t)u8Value[3] << 8) | u8Value[4];
      ptMS->u32MSX_P2 = ((uint32_t)(u8Value[5] & 0x0F) << 16) | ((uint32_t)u8Value[6] << 8) | u8Value[7];
      ptMS->u32MSX_P3 = ((uint32_t)(u8Value[5] & 0xF0) << 12) | ((uint32_t)u8Value[0] << 8) | u8Value[1];
      ptMS->tDIV = SI5351_MS_DIV_BY1;
      ptMS->bDivBy4 = false;
    }
  }

  return bValid;
}

bool Si5351_SetMSB(const Si5351_t* ptSi5351, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  uint8_t u8Value;

  if(ptSi5351 && ptMS) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x17,
        (ptSi5351->tRead(ptSi5351->pInstance, 0x17) & ~(0x1 << 6)) | ((uint8_t)(ptMS->bInteger ? 1 : 0) << 6)
      );
      ptSi5351->tWrite(ptSi5351->pInstance, 0x22, ((ptMS->u32MSX_P3 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x23, ( ptMS->u32MSX_P3        & 0xFF));
      u8Value = ptSi5351->tRead(ptSi5351->pInstance, 0x24);
      ptSi5351->tWrite(ptSi5351->pInstance, 0x24, (u8Value & ~0x3) | ((ptMS->u32MSX_P1 >> 16) & 0x3));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x25, ((ptMS->u32MSX_P1 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x26, ( ptMS->u32MSX_P1        & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x27, ((ptMS->u32MSX_P3 >> 12) & 0xF0) | ((ptMS->u32MSX_P2 >> 16) & 0xF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x28, ((ptMS->u32MSX_P2 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x29, ( ptMS->u32MSX_P2        & 0xFF));
    }
  }

  return bValid;
}

/* Reg.16-21,42-92 Multisynthx Parameters */
bool Si5351_GetMS(Si5351_MS_t* ptMS, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;
  uint8_t u8Reg;
  uint8_t u8Value[8];

#if SI5351_CLKNUM <= 3
  if(ptMS && ptSi5351 && tCLKNo < 3) {
#else
  if(ptMS && ptSi5351 && tCLKNo < 8) {
#endif
    if(ptSi5351->tRead) {
      bValid = true;
      if(tCLKNo < 6) {
        ptMS->bInteger = ((ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) >> 6) & 0x1) ? true : false;
      } else {
        ptMS->bInteger = true;
      }
      for(u8Reg = 0; u8Reg < 8; u8Reg++) {
        u8Value[u8Reg] = ptSi5351->tRead(ptSi5351->pInstance, 0x2A + 8 * tCLKNo + u8Reg);
      }
      ptMS->u32MSX_P1 = ((uint32_t)(u8Value[2] & 0x03) << 16) | ((uint32_t)u8Value[3] << 8) | u8Value[4];
      ptMS->u32MSX_P2 = ((uint32_t)(u8Value[5] & 0x0F) << 16) | ((uint32_t)u8Value[6] << 8) | u8Value[7];
      ptMS->u32MSX_P3 = ((uint32_t)(u8Value[5] & 0xF0) << 12) | ((uint32_t)u8Value[0] << 8) | u8Value[1];
      ptMS->tDIV = (Si5351_MS_DIV_t)((u8Value[2] >> 4) & 0x7);
      ptMS->bDivBy4 = (((u8Value[2] >> 2) & 0x3) == 0x3) ? true : false;
    }
  }

  return bValid;
}

bool Si5351_SetMS(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  uint8_t u8Value;

#if SI5351_CLKNUM <= 3
  if(ptSi5351 && ptMS && tCLKNo < 3) {
#else
  if(ptSi5351 && ptMS && tCLKNo < 8) {
#endif
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      if(tCLKNo < 6) {
        ptSi5351->tWrite(
          ptSi5351->pInstance, 0x10,
          (ptSi5351->tRead(ptSi5351->pInstance, 0x10 + tCLKNo) & ~(0x1 << 6)) | ((uint8_t)(ptMS->bInteger ? 1 : 0) << 6)
        );
      }
      ptSi5351->tWrite(ptSi5351->pInstance, 0x2A + 8 * tCLKNo, ((ptMS->u32MSX_P3 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x2B + 8 * tCLKNo, ( ptMS->u32MSX_P3        & 0xFF));
      u8Value = ptSi5351->tRead(ptSi5351->pInstance, 0x2C + 8 * tCLKNo);
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x2C + 8 * tCLKNo,
        (u8Value & ~0x7F) | ((ptMS->tDIV & 0x7) << 4) | ((ptMS->bDivBy4 ? 3 : 0) << 2) | ((ptMS->u32MSX_P1 >> 16) & 0x3)
      );
      ptSi5351->tWrite(ptSi5351->pInstance, 0x2D + 8 * tCLKNo, ((ptMS->u32MSX_P1 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x2E + 8 * tCLKNo, ( ptMS->u32MSX_P1        & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x2F + 8 * tCLKNo, ((ptMS->u32MSX_P3 >> 12) & 0xF0) | ((ptMS->u32MSX_P2 >> 16) & 0xF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x30 + 8 * tCLKNo, ((ptMS->u32MSX_P2 >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x31 + 8 * tCLKNo, ( ptMS->u32MSX_P2        & 0xFF));
    }
  }

  return bValid;
}

/* Reg.149-161 Spread Spectrum Parameters */
/*
  Reg.149[7]   : SSC_EN
  Reg.149[6:0] : SSDN_P2[14:8]
  Reg.150[7:0] : SSDN_P2[7:0]
  Reg.151[7]   : SSC_MODE
  Reg.151[6:0] : SSDN_P3[14:8]
  Reg.152[7:0] : SSDN_P3[7:0]
  Reg.153[7:0] : SSDN_P1[7:0]
  Reg.154[3:0] : SSDN_P1[11:8]
  Reg.154[7:4] : SSUDP[11:8]
  Reg.155[7:0] : SSUDP[7:0]
  Reg.156[7:0] : SSUP_P2[14:8]
  Reg.157[7:0] : SSUP_P2[7:0]
  Reg.158[7:0] : SSUP_P3[14:8]
  Reg.159[7:0] : SSUP_P3[7:0]
  Reg.160[7:0] : SSUP_P1[7:0]
  Reg.161[3:0] : SSUP_P1[11:8]
*/

bool Si5351_GetSSPEnable(bool* pbEnable, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Value;

  if(pbEnable && ptSi5351) {
    if(ptSi5351->tRead) {
      *pbEnable = (ptSi5351->tRead(ptSi5351->pInstance, 0x95) & 0x80) ? true : false;
    }
  }

  return bValid;
}

bool Si5351_SetSSPEnable(const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x95,
		ptSi5351->tRead(ptSi5351->pInstance, 0x95) | 0x80
      );
    }
  }

  return bValid;
}

bool Si5351_SetSSPDisable(const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x95,
		ptSi5351->tRead(ptSi5351->pInstance, 0x95) & ~0x80
      );
    }
  }

  return bValid;
}

bool Si5351_GetSSP(Si5351_SSP_t* ptSSP, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Reg;
  uint8_t au8Value[13];

  if(ptSSP && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      for(u8Reg = 0; u8Reg < 13; u8Reg++) {
        au8Value[u8Reg] = ptSi5351->tRead(ptSi5351->pInstance, 0x95 + u8Reg);
      }

      ptSSP->tSSC_MODE = (Si5351_SSP_Mode_t)((au8Value[2] >> 7) & 0x1);
      ptSSP->u16SSDN_P1 = ((uint16_t)(au8Value[5] & 0x0F) << 8) | au8Value[4];
      ptSSP->u16SSDN_P2 = ((uint16_t)(au8Value[0] & 0x7F) << 8) | au8Value[1];
      ptSSP->u16SSDN_P3 = ((uint16_t)(au8Value[2] & 0x7F) << 8) | au8Value[3];
      ptSSP->u16SSUDP = ((uint16_t)(au8Value[5] & 0xF0) << 4) | au8Value[6];
      ptSSP->u16SSUP_P1 = ((uint16_t)(au8Value[12] & 0x0F) << 8) | au8Value[11];
      ptSSP->u16SSUP_P2 = ((uint16_t)(au8Value[7] & 0x7F) << 8) | au8Value[8];
      ptSSP->u16SSUP_P3 = ((uint16_t)(au8Value[9] & 0x7F) << 8) | au8Value[10];
    }
  }

  return bValid;
}

bool Si5351_SetSSP(const Si5351_t* ptSi5351, const Si5351_SSP_t* ptSSP) {
  bool bValid = false;

  if(ptSi5351 && ptSSP) {
    if(ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(ptSi5351->pInstance, 0x96, (uint8_t)(ptSSP->u16SSDN_P2 & 0xFF));
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x97,
        (uint8_t)(((ptSSP->tSSC_MODE & 0x1) << 7) | ((ptSSP->u16SSDN_P3 >> 8) & 0x7F))
      );
      ptSi5351->tWrite(ptSi5351->pInstance, 0x98, (uint8_t)(ptSSP->u16SSDN_P3 & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x99, (uint8_t)(ptSSP->u16SSDN_P1 & 0xFF));
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0x9A,
        (uint8_t)(((ptSSP->u16SSUDP >> 4) & 0xF0) | ((ptSSP->u16SSDN_P1 >> 8) & 0x0F))
      );
      ptSi5351->tWrite(ptSi5351->pInstance, 0x9B, (uint8_t)(ptSSP->u16SSUDP & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x9C, (uint8_t)((ptSSP->u16SSUP_P2 >> 8) & 0x7F));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x9D, (uint8_t)(ptSSP->u16SSUP_P2 & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x9E, (uint8_t)((ptSSP->u16SSUP_P3 >> 8) & 0x7F));
      ptSi5351->tWrite(ptSi5351->pInstance, 0x9F, (uint8_t)(ptSSP->u16SSUP_P3 & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0xA0, (uint8_t)(ptSSP->u16SSUP_P1 & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0xA1, (uint8_t)((ptSSP->u16SSUP_P1 >> 8) & 0x0F));
    }
  }

  return bValid;
}

#if SI5351_TYPE == 1
/* Reg.162-164 VCXO Parameter */
/* VCXO_Param : VCXO Parameter */
bool Si5351_GetVCXO(uint32_t* pu32VCXO, const Si5351_t* ptSi5351) {
  bool bValid = false;
  uint8_t u8Reg;
  uint8_t au8Value[3];

  if(pu32VCXO && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      for(u8Reg = 0; u8Reg < 3; u8Reg++) {
        au8Value[u8Reg] = ptSi5351->tRead(ptSi5351->pInstance, 0xA2 + u8Reg);
      }

      *pu32VCXO = (((uint32_t)au8Value[2] & 0x3F) << 16) | ((uint32_t)au8Value[1] << 8) | au8Value[0];
    }
  }

  return bValid;
}

bool Si5351_SetVCXO(const Si5351_t* ptSi5351, const uint32_t u32VCXO) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(ptSi5351->pInstance, 0xA2, (uint8_t)( u32VCXO        & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0xA3, (uint8_t)((u32VCXO >>  8) & 0xFF));
      ptSi5351->tWrite(ptSi5351->pInstance, 0xA4, (uint8_t)((u32VCXO >> 16) & 0x3F));
    }
  }

  return bValid;
}
#endif

/* Reg.165-172 CLKx Initial Phase Offset */
/* CLK0-5_PHOFF : Clock 0-5 Initial Phase Offset */
bool Si5351_GetPhaseOffset(uint8_t* pu8PhOff, const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo) {
  bool bValid = false;

#if SI5351_CLKNUM <= 3
  if(pu8PhOff && ptSi5351 && tCLKNo < 3) {
#else
  if(pu8PhOff && ptSi5351 && tCLKNo < 6) {
#endif
    if(ptSi5351->tRead) {
      bValid = true;
      *pu8PhOff = (uint8_t)(ptSi5351->tRead(ptSi5351->pInstance, 0xA5 + tCLKNo) & 0x7F);
    }
  }

  return bValid;
}

bool Si5351_SetPhaseOffset(const Si5351_t* ptSi5351, const Si5351_CLKNo_t tCLKNo, const uint8_t u8PhOff) {
  bool bValid = false;

#if SI5351_CLKNUM <= 3
  if(ptSi5351 && tCLKNo < 3) {
#else
  if(ptSi5351 && tCLKNo < 6) {
#endif
    if(ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(ptSi5351->pInstance, 0xA5 + tCLKNo, (u8PhOff & 0x7F));
    }
  }

  return bValid;
}

/* PLL soft reset */
/* Reg.177 PLL soft reset */
bool Si5351_PLLSoftReset(const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(ptSi5351->pInstance, 0xB1, (ptSi5351->tRead(ptSi5351->pInstance, 0xB1) & 0x0F) | 0xA0);
    }
  }

  return bValid;
}

/* Crystal Internal Load Capacitance */
/* Reg.183 Crystal Internal Load Capacitance */
bool Si5351_GetXTALLoadCap(Si5351_XTALLoadCap_t* ptXTALLoadCap, const Si5351_t* ptSi5351) {
  bool bValid = false;

  if(ptXTALLoadCap && ptSi5351) {
    if(ptSi5351->tRead) {
      bValid = true;
      *ptXTALLoadCap = (Si5351_XTALLoadCap_t)((ptSi5351->tRead(ptSi5351->pInstance, 0xB7) >> 6) & 0x3);
    }
  }

  return bValid;
}

bool Si5351_SetXTALLoadCap(const Si5351_t* ptSi5351, const Si5351_XTALLoadCap_t tXTALLoadCap) {
  bool bValid = false;

  if(ptSi5351) {
    if(ptSi5351->tRead && ptSi5351->tWrite) {
      bValid = true;
      ptSi5351->tWrite(
        ptSi5351->pInstance, 0xB7,
        (ptSi5351->tRead(ptSi5351->pInstance, 0xB7) & ~(0x3 << 6)) | (((uint8_t)tXTALLoadCap & 0x3) << 6)
      );
    }
  }

  return bValid;
}

/* Utility */

#if defined(SI5351_USE_BERKELEY_SOFTFLOAT_3)
static uint64_t f64_floor(float64_t f64Value) {
  return f64_to_ui64(f64_add(f64Value, f64_div(ui64_to_f64(1), ui64_to_f64(2))), softfloat_round_minMag, false); 
}

bool Si5351_CalcMSPLL(Si5351_MS_t* ptMS, const float64_t f64BaseClock, const float64_t f64Output) {
  bool bValid = false;
  float64_t f64Ratio;
  float64_t f64After;
  uint32_t u32a;
  uint32_t u32b;

  if(ptMS && !f64_le(f64BaseClock, ui64_to_f64(0)) && !f64_le(f64Output, ui64_to_f64(0))) {
    f64Ratio = f64_div(f64Output, f64BaseClock);
    if(!f64_lt(f64Ratio, ui64_to_f64(15)) && f64_le(f64Ratio, ui64_to_f64(90))) {
      bValid = true;
      ptMS->bDivBy4 = false;
      ptMS->tDIV = SI5351_MS_DIV_BY1;
      f64After = f64_sub(f64Ratio, f64_roundToInt(f64Ratio, softfloat_round_minMag, false));
      u32a = f64_to_ui64(f64Ratio, softfloat_round_minMag, false);
      u32b = f64_to_ui64(f64_mul(f64After, ui64_to_f64(524288)), softfloat_round_minMag, false);
      if(u32b == 0) {
        ptMS->bInteger = true;
        ptMS->u32MSX_P1 = (u32a << 7) - 512;
        ptMS->u32MSX_P2 = 0;
        ptMS->u32MSX_P3 = 524288;
      } else {
        ptMS->bInteger = false;
        ptMS->u32MSX_P1 = (u32a << 7) + f64_floor(f64_mul(ui64_to_f64(128), f64After)) - 512;
        ptMS->u32MSX_P2 = (u32b << 7) - 524288 * f64_floor(f64_mul(ui64_to_f64(128), f64After));
        ptMS->u32MSX_P3 = 524288;
      }
    }
  }

  return bValid;
}

bool Si5351_DecalcMSPLL(float64_t* pf64Output, const float64_t f64BaseClock, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  float64_t f64Ratio;

  if(pf64Output && !f64_le(f64BaseClock, ui64_to_f64(0)) && ptMS) {
    if(
      ptMS->u32MSX_P1 < 0x40000  &&
      ptMS->u32MSX_P2 < 0x100000 &&
      ptMS->u32MSX_P3 < 0x100000 &&
      ptMS->u32MSX_P3 > 0
    ) {
      if(ptMS->bInteger) {
        if(ptMS->u32MSX_P2 == 0) {
          bValid = true;
          f64Ratio = ui64_to_f64((ptMS->u32MSX_P1 + 512) >> 7);
        }
      } else {
        bValid = true;
        f64Ratio = f64_add(f64_div(ui64_to_f64(ptMS->u32MSX_P1), ui64_to_f64(128)), ui64_to_f64(4));
        f64Ratio = f64_add(f64Ratio, f64_div(ui64_to_f64(ptMS->u32MSX_P2), ui64_to_f64(128 * 524288)));
      }
    }
    if(bValid) {
      if(!f64_lt(f64Ratio, ui64_to_f64(15)) && f64_le(f64Ratio, ui64_to_f64(90))) {
        *pf64Output = f64_mul(f64BaseClock, f64Ratio);
      } else {
        bValid = false;
      }
    }
  }

  return bValid;
}

bool Si5351_CalcMSClk(Si5351_MS_t* ptMS, const float64_t f64BaseClock, const float64_t f64Output) {
  bool bValid = false;
  float64_t f64Ratio, f64RatioTemp;
  float64_t f64After, f64AfterTemp;
  uint32_t u32a;
  uint32_t u32b;

  if(ptMS && !f64_le(f64BaseClock, ui64_to_f64(0)) && !f64_le(f64Output, ui64_to_f64(0))) {
    f64Ratio = f64_div(f64BaseClock, f64Output);
    if(!f64_lt(f64Ratio, ui64_to_f64(6)) && f64_le(f64Ratio, ui64_to_f64(1800))) {
      bValid = true;
      ptMS->bDivBy4 = false;
      ptMS->tDIV = SI5351_MS_DIV_BY1;
      for(u32a = 0; u32a < 8; u32a++) {
        f64RatioTemp = f64_div(f64Ratio, ui64_to_f64(1 << (u32a + 1 + (ptMS->bDivBy4 ? 2 : 0))));
        f64AfterTemp = f64_sub(f64RatioTemp, f64_roundToInt(f64RatioTemp, softfloat_round_minMag, false));
        if(f64_lt(f64RatioTemp, ui64_to_f64(4)) || f64_lt(ui64_to_f64(f64_to_ui64(f64_mul(f64AfterTemp, ui64_to_f64(524288)), softfloat_round_minMag, false) << 7), ui64_to_f64(524288 * f64_floor(f64_mul(ui64_to_f64(128), f64AfterTemp)))) || !f64_eq(f64Ratio, f64_mul(f64RatioTemp, ui64_to_f64(1 << (u32a + 1 + (ptMS->bDivBy4 ? 2 : 0)))))) {
          break;
        }
      }
      if(u32a == 8) {
        ptMS->tDIV = SI5351_MS_DIV_BY128;
      } else {
        ptMS->tDIV = (Si5351_MS_DIV_t)u32a;
      }
      f64Ratio = f64_div(f64Ratio, ui64_to_f64(1 << (ptMS->tDIV + (ptMS->bDivBy4 ? 2 : 0))));
      f64After = f64_sub(f64Ratio, f64_roundToInt(f64Ratio, softfloat_round_minMag, false));
      u32a = f64_to_ui64(f64Ratio, softfloat_round_minMag, false);
      u32b = f64_to_ui64(f64_mul(f64After, ui64_to_f64(524288)), softfloat_round_minMag, false);
      if(u32b == 0) {
        ptMS->bInteger = true;
        ptMS->u32MSX_P1 = (u32a << 7) - 512;
        ptMS->u32MSX_P2 = 0;
        ptMS->u32MSX_P3 = 524288;
      } else {
        ptMS->bInteger = false;
        ptMS->u32MSX_P1 = (u32a << 7) + f64_floor(f64_mul(ui64_to_f64(128), f64After)) - 512;
        ptMS->u32MSX_P2 = (u32b << 7) - 524288 * f64_floor(f64_mul(ui64_to_f64(128), f64After));
        ptMS->u32MSX_P3 = 524288;
      }
    }
  }

  return bValid;
}

bool Si5351_DecalcMSClk(float64_t* pf64Output, const float64_t f64BaseClock, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  float64_t f64Ratio;

  if(pf64Output && !f64_le(f64BaseClock, ui64_to_f64(0)) && ptMS) {
    if(
      ptMS->u32MSX_P1 < 0x40000  &&
      ptMS->u32MSX_P2 < 0x100000 &&
      ptMS->u32MSX_P3 < 0x100000 &&
      ptMS->u32MSX_P3 > 0
    ) {
      if(ptMS->bInteger) {
        if(ptMS->u32MSX_P2 == 0) {
          bValid = true;
          f64Ratio = ui64_to_f64((ptMS->u32MSX_P1 + 512) >> 7);
        }
      } else {
        bValid = true;
        f64Ratio = f64_add(f64_div(ui64_to_f64(ptMS->u32MSX_P1), ui64_to_f64(128)), ui64_to_f64(4));
        f64Ratio = f64_add(f64Ratio, f64_div(ui64_to_f64(ptMS->u32MSX_P2), ui64_to_f64(128 * 524288)));
      }
    }
    if(bValid) {
      f64Ratio = f64_mul(f64Ratio, ui64_to_f64(1 << (ptMS->tDIV + (ptMS->bDivBy4 ? 2 : 0))));
      if(!f64_lt(f64Ratio, ui64_to_f64(6)) && f64_le(f64Ratio, ui64_to_f64(1800))) {
        *pf64Output = f64_div(f64BaseClock, f64Ratio);
      } else {
        bValid = false;
      }
    }
  }

  return bValid;
}

bool Si5351_CalcSSP(
  Si5351_SSP_t* ptSSP,
  const Si5351_SSP_Mode_t tSSC_MODE,
  const float64_t f64Freq_PFD,
  const float64_t f64Ratio,
  const float64_t f64SscAmp
) {
  bool bValid = false;
  float64_t f64SSUPDN;

  if(ptSSP && !f64_lt(f64Freq_PFD, ui64_to_f64(0)) && f64_lt(f64_div(f64Freq_PFD, ui64_to_f64(4 * 35100)), ui64_to_f64(65536))) {
    ptSSP->tSSC_MODE = (Si5351_SSP_Mode_t)(tSSC_MODE & 1);
    ptSSP->u16SSUDP = f64_to_ui64(f64_add(f64_div(f64Freq_PFD, ui64_to_f64(4 * 35100)), f64_div(ui64_to_f64(1), ui64_to_f64(2))), softfloat_round_minMag, false);
    if(ptSSP->tSSC_MODE == SI5351_SSP_MODE_DOWN) {
      if(
        !f64_lt(f64SscAmp, f64_mul(i64_to_f64(-1), f64_div(ui64_to_f64(25), ui64_to_f64(1000)))) &&
         f64_le(f64SscAmp, f64_mul(i64_to_f64(-1), f64_div(ui64_to_f64(1), ui64_to_f64(1000))))
      ) {
        bValid = true;
        f64SSUPDN = f64_mul(ui64_to_f64(64), f64_mul(f64Ratio, f64_div(f64SscAmp, f64_mul(f64_add(ui64_to_f64(1), f64SscAmp), ui64_to_f64(ptSSP->u16SSUDP)))));
        ptSSP->u16SSDN_P1 = f64_to_ui64(f64_add(f64SSUPDN, f64_div(ui64_to_f64(1), ui64_to_f64(2))), softfloat_round_minMag, false);
        ptSSP->u16SSDN_P2 = f64_to_ui64(f64_mul(ui64_to_f64(32767), f64_sub(f64SSUPDN, ui64_to_f64(ptSSP->u16SSDN_P1))), softfloat_round_minMag, false);
        ptSSP->u16SSDN_P3 = 32767;
        ptSSP->u16SSUP_P1 = 0;
        ptSSP->u16SSUP_P2 = 0;
        ptSSP->u16SSUP_P3 = 1;
      }
    } else if(ptSSP->tSSC_MODE == SI5351_SSP_MODE_CENTER) {
      if(
        !f64_lt(f64SscAmp, f64_mul(i64_to_f64(-1), f64_div(ui64_to_f64(15), ui64_to_f64(1000)))) &&
         f64_le(f64SscAmp, f64_div(ui64_to_f64(15), ui64_to_f64(1000)))
      ) {
        bValid = true;
        f64SSUPDN = f64_mul(ui64_to_f64(128), f64_mul(f64Ratio, f64_div(f64SscAmp, f64_mul(f64_add(ui64_to_f64(1), f64SscAmp), ui64_to_f64(ptSSP->u16SSUDP)))));
        ptSSP->u16SSDN_P1 = f64_to_ui64(f64_add(f64SSUPDN, f64_div(ui64_to_f64(1), ui64_to_f64(2))), softfloat_round_minMag, false);
        ptSSP->u16SSDN_P2 = f64_to_ui64(f64_mul(ui64_to_f64(32767), f64_sub(f64SSUPDN, ui64_to_f64(ptSSP->u16SSDN_P1))), softfloat_round_minMag, false);
        ptSSP->u16SSDN_P3 = 32767;
        f64SSUPDN = f64_mul(ui64_to_f64(128), f64_mul(f64Ratio, f64_div(f64SscAmp, f64_mul(f64_sub(ui64_to_f64(1), f64SscAmp), ui64_to_f64(ptSSP->u16SSUDP)))));
        ptSSP->u16SSUP_P1 = f64_to_ui64(f64_add(f64SSUPDN, f64_div(ui64_to_f64(1), ui64_to_f64(2))), softfloat_round_minMag, false);
        ptSSP->u16SSUP_P2 = f64_to_ui64(f64_mul(ui64_to_f64(32767), f64_sub(f64SSUPDN, ui64_to_f64(ptSSP->u16SSDN_P1))), softfloat_round_minMag, false);
        ptSSP->u16SSUP_P3 = 32767;
      }
    }
  }

  return bValid;
}

#if SI5351_TYPE == 1
bool Si5351_CalcVCXO(uint32_t* pu32VCXO, const float64_t f64Ratio, const uint8_t u8APR) {
  bool bValid = false;

  if(pu32VCXO) {
    bValid = true;
    *pu32VCXO = ui64_to_f64(f64_add(f64_add(f64_div(u64_to_f64(103), u64_to_f64(100)), u64_to_f64(128 * u8APR)), f64Ratio), softfloat_round_minMag, false);
  }

  return bValid;
}
#endif
#else  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */
bool Si5351_CalcMSPLL(Si5351_MS_t* ptMS, const double dBaseClock, const double dOutput) {
  bool bValid = false;
  double dRatio;
  double dAfter;
  uint32_t u32a;
  uint32_t u32b;

  if(ptMS && dBaseClock > 0 && dOutput > 0) {
    dRatio = dOutput / dBaseClock;
    if(dRatio >= 15 && dRatio <= 90) {
      bValid = true;
      ptMS->bDivBy4 = false;
      ptMS->tDIV = SI5351_MS_DIV_BY1;
      dAfter = dRatio - (uint32_t)dRatio;
      u32a = (uint32_t)dRatio;
      u32b = (uint32_t)(dAfter * 524288);
      if(u32b == 0) {
        ptMS->bInteger = true;
        ptMS->u32MSX_P1 = (u32a << 7) - 512;
        ptMS->u32MSX_P2 = 0;
        ptMS->u32MSX_P3 = 524288;
      } else {
        ptMS->bInteger = false;
        ptMS->u32MSX_P1 = (u32a << 7) + (uint32_t)((128 * dAfter) + 0.5) - 512;
        ptMS->u32MSX_P2 = (u32b << 7) - 524288 * (uint32_t)((128 * dAfter) + 0.5);
        ptMS->u32MSX_P3 = 524288;
      }
    }
  }

  return bValid;
}

bool Si5351_DecalcMSPLL(double* pdOutput, const double dBaseClock, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  double dRatio;

  if(pdOutput && dBaseClock > 0 && ptMS) {
    if(
      ptMS->u32MSX_P1 < 0x40000  &&
      ptMS->u32MSX_P2 < 0x100000 &&
      ptMS->u32MSX_P3 < 0x100000 &&
      ptMS->u32MSX_P3 > 0
    ) {
      if(ptMS->bInteger) {
        if(ptMS->u32MSX_P2 == 0) {
          bValid = true;
          dRatio = (ptMS->u32MSX_P1 + 512) >> 7;
        }
      } else {
        bValid = true;
        dRatio = ptMS->u32MSX_P1 / 128 + 4;
        dRatio += (double)ptMS->u32MSX_P2 / (128 * 524288);
      }
    }
    if(bValid) {
      if(dRatio >= 15 && dRatio <= 90) {
        *pdOutput = dBaseClock * dRatio;
      } else {
        bValid = false;
      }
    }
  }

  return bValid;
}

bool Si5351_CalcMSClk(Si5351_MS_t* ptMS, const double dBaseClock, const double dOutput) {
  bool bValid = false;
  double dRatio, dRatioTemp;
  double dAfter, dAfterTemp;
  uint32_t u32a;
  uint32_t u32b;

  if(ptMS && dBaseClock >0 && dOutput > 0) {
    dRatio = dBaseClock / dOutput;
    if(dRatio >= 6 && dRatio <= 1800) {
      bValid = true;
      ptMS->bDivBy4 = false;
      ptMS->tDIV = SI5351_MS_DIV_BY1;
      for(u32a = 0; u32a < 8; u32a++) {
        dRatioTemp = dRatio / (1 << (u32a + 1 + (ptMS->bDivBy4 ? 2 : 0)));
        dAfterTemp = dRatioTemp - (uint32_t)dRatioTemp;
        if(dRatioTemp < 4 || (((uint32_t)dAfterTemp * 524288) << 7) < (524288 * (uint32_t)((128 * dAfterTemp) + 0.5)) || dRatio != dRatioTemp * (1 << (u32a + 1 + (ptMS->bDivBy4 ? 2 : 0)))) {
          break;
        }
      }
      if(u32a == 8) {
        ptMS->tDIV = SI5351_MS_DIV_BY128;
      } else {
        ptMS->tDIV = (Si5351_MS_DIV_t)u32a;
      }
      dRatio /= 1 << (ptMS->tDIV + (ptMS->bDivBy4 ? 2 : 0));
      dAfter = dRatio - (uint32_t)dRatio;
      u32a = (uint32_t)dRatio;
      u32b = (uint32_t)(dAfter * 524288);
      if(u32b == 0) {
        ptMS->bInteger = true;
        ptMS->u32MSX_P1 = (u32a << 7) - 512;
        ptMS->u32MSX_P2 = 0;
        ptMS->u32MSX_P3 = 524288;
      } else {
        ptMS->bInteger = false;
        ptMS->u32MSX_P1 = (u32a << 7) + (uint32_t)((128 * dAfter) + 0.5) - 512;
        ptMS->u32MSX_P2 = (u32b << 7) - 524288 * (uint32_t)((128 * dAfter) + 0.5);
        ptMS->u32MSX_P3 = 524288;
      }
    }
  }

  return bValid;
}

bool Si5351_DecalcMSClk(double* pdOutput, const double dBaseClock, const Si5351_MS_t* ptMS) {
  bool bValid = false;
  double dRatio;

  if(pdOutput && dBaseClock > 0 && ptMS) {
    if(
      ptMS->u32MSX_P1 < 0x40000  &&
      ptMS->u32MSX_P2 < 0x100000 &&
      ptMS->u32MSX_P3 < 0x100000 &&
      ptMS->u32MSX_P3 > 0
    ) {
      if(ptMS->bInteger) {
        if(ptMS->u32MSX_P2 == 0) {
          bValid = true;
          dRatio = (ptMS->u32MSX_P1 + 512) >> 7;
        }
      } else {
        bValid = true;
        dRatio = (double)ptMS->u32MSX_P1 / 128 + 4;
        dRatio += (double)(ptMS->u32MSX_P2 >> 7) / 524288;
      }
    }
    if(bValid) {
      dRatio *= 1 << (ptMS->tDIV + (ptMS->bDivBy4 ? 2 : 0));
      if(dRatio >= 6 && dRatio <= 1800) {
        *pdOutput = dBaseClock / dRatio;
      } else {
        bValid = false;
      }
    }
  }

  return bValid;
}

bool Si5351_CalcSSP(
  Si5351_SSP_t* ptSSP,
  const Si5351_SSP_Mode_t tSSC_MODE,
  const double dFreq_PFD,
  const double dRatio,
  const double dSscAmp
) {
  bool bValid = false;
  double dSSUPDN;

  if(ptSSP && dFreq_PFD >= 0 && (dFreq_PFD / (4 * 35100) < 65536)) {
    ptSSP->tSSC_MODE = (Si5351_SSP_Mode_t)(tSSC_MODE & 1);
    ptSSP->u16SSUDP = (uint16_t)(dFreq_PFD / (4 * 35100) + 0.5);
    if(ptSSP->tSSC_MODE == SI5351_SSP_MODE_DOWN) {
      if(dSscAmp >= -0.025 && dSscAmp <= -0.001) {
        bValid = true;
        dSSUPDN = 64 * dRatio * (dSscAmp / ((1 + dSscAmp) * ptSSP->u16SSUDP));
        ptSSP->u16SSDN_P1 = (uint16_t)(dSSUPDN + 0.5);
        ptSSP->u16SSDN_P2 = 32767 * (dSSUPDN - ptSSP->u16SSDN_P1);
        ptSSP->u16SSDN_P3 = 32767;
        ptSSP->u16SSUP_P1 = 0;
        ptSSP->u16SSUP_P2 = 0;
        ptSSP->u16SSUP_P3 = 1;
      }
    } else if(ptSSP->tSSC_MODE == SI5351_SSP_MODE_CENTER) {
      if(dSscAmp >= -0.015 && dSscAmp <=  0.015) {
        bValid = true;
        dSSUPDN = 128 * dRatio * (dSscAmp / ((1 + dSscAmp) * ptSSP->u16SSUDP));
        ptSSP->u16SSDN_P1 = (uint16_t)(dSSUPDN + 0.5);
        ptSSP->u16SSDN_P2 = 32767 * (dSSUPDN - ptSSP->u16SSDN_P1);
        ptSSP->u16SSDN_P3 = 32767;
        dSSUPDN = 128 * dRatio * (dSscAmp / ((1 - dSscAmp) * ptSSP->u16SSUDP));
        ptSSP->u16SSUP_P1 = (uint16_t)(dSSUPDN + 0.5);
        ptSSP->u16SSUP_P2 = 32767 * (dSSUPDN - ptSSP->u16SSUP_P1);
        ptSSP->u16SSUP_P3 = 32767;
      }
    }
  }

  return bValid;
}

#if SI5351_TYPE == 1
bool Si5351_CalcVCXO(uint32_t* pu32VCXO, const double dRatio, const uint8_t u8APR) {
  bool bValid = false;

  if(pu32VCXO) {
    bValid = true;
    *pu32VCXO = (uint32_t)(1.03 * (128 * dRatio) * u8APR);
  }

  return bValid;
}
#endif
#endif  /* SI5351_USE_BERKELEY_SOFTFLOAT_3 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

