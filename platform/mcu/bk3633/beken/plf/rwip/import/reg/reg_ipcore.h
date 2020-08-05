#ifndef _REG_IPCORE_H_
#define _REG_IPCORE_H_

#include <stdint.h>
#include "_reg_ipcore.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#define REG_IPCORE_COUNT 98

#define REG_IPCORE_DECODING_MASK 0x000001FF

/**
 * @brief RWDMCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31      MASTER_SOFT_RST   0
 *     30    MASTER_TGSOFT_RST   0
 *     29         REG_SOFT_RST   0
 *     28   RADIOCNTL_SOFT_RST   0
 *     27            SWINT_REQ   0
 * </pre>
 */
#define ip_RWDMCNTL_ADDR   0x00820000
#define ip_RWDMCNTL_OFFSET 0x00000000
#define ip_RWDMCNTL_INDEX  0x00000000
#define ip_RWDMCNTL_RESET  0x00000000

__INLINE uint32_t ip_rwdmcntl_get(void)
{
    return REG_IP_RD(ip_RWDMCNTL_ADDR);
}

__INLINE void ip_rwdmcntl_set(uint32_t value)
{
    REG_IP_WR(ip_RWDMCNTL_ADDR, value);
}

// field definitions
#define ip_MASTER_SOFT_RST_BIT       ((uint32_t)0x80000000)
#define ip_MASTER_SOFT_RST_POS       31
#define ip_MASTER_TGSOFT_RST_BIT     ((uint32_t)0x40000000)
#define ip_MASTER_TGSOFT_RST_POS     30
#define ip_REG_SOFT_RST_BIT          ((uint32_t)0x20000000)
#define ip_REG_SOFT_RST_POS          29
#define ip_RADIOCNTL_SOFT_RST_BIT    ((uint32_t)0x10000000)
#define ip_RADIOCNTL_SOFT_RST_POS    28
#define ip_SWINT_REQ_BIT             ((uint32_t)0x08000000)
#define ip_SWINT_REQ_POS             27

#define ip_MASTER_SOFT_RST_RST       0x0
#define ip_MASTER_TGSOFT_RST_RST     0x0
#define ip_REG_SOFT_RST_RST          0x0
#define ip_RADIOCNTL_SOFT_RST_RST    0x0
#define ip_SWINT_REQ_RST             0x0

__INLINE void ip_rwdmcntl_pack(uint8_t mastersoftrst, uint8_t mastertgsoftrst, uint8_t regsoftrst, uint8_t radiocntlsoftrst, uint8_t swintreq)
{
    ASSERT_ERR((((uint32_t)mastersoftrst << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)mastertgsoftrst << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)regsoftrst << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)radiocntlsoftrst << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)swintreq << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_IP_WR(ip_RWDMCNTL_ADDR,  ((uint32_t)mastersoftrst << 31) | ((uint32_t)mastertgsoftrst << 30) | ((uint32_t)regsoftrst << 29) | ((uint32_t)radiocntlsoftrst << 28) | ((uint32_t)swintreq << 27));
}

__INLINE void ip_rwdmcntl_unpack(uint8_t* mastersoftrst, uint8_t* mastertgsoftrst, uint8_t* regsoftrst, uint8_t* radiocntlsoftrst, uint8_t* swintreq)
{
    uint32_t localVal = REG_IP_RD(ip_RWDMCNTL_ADDR);

    *mastersoftrst = (localVal & ((uint32_t)0x80000000)) >> 31;
    *mastertgsoftrst = (localVal & ((uint32_t)0x40000000)) >> 30;
    *regsoftrst = (localVal & ((uint32_t)0x20000000)) >> 29;
    *radiocntlsoftrst = (localVal & ((uint32_t)0x10000000)) >> 28;
    *swintreq = (localVal & ((uint32_t)0x08000000)) >> 27;
}

__INLINE uint8_t ip_rwdmcntl_master_soft_rst_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RWDMCNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_rwdmcntl_master_soft_rst_setf(uint8_t mastersoftrst)
{
    ASSERT_ERR((((uint32_t)mastersoftrst << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_RWDMCNTL_ADDR, (REG_IP_RD(ip_RWDMCNTL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)mastersoftrst << 31));
}

__INLINE uint8_t ip_rwdmcntl_master_tgsoft_rst_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RWDMCNTL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void ip_rwdmcntl_master_tgsoft_rst_setf(uint8_t mastertgsoftrst)
{
    ASSERT_ERR((((uint32_t)mastertgsoftrst << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_IP_WR(ip_RWDMCNTL_ADDR, (REG_IP_RD(ip_RWDMCNTL_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)mastertgsoftrst << 30));
}

__INLINE uint8_t ip_rwdmcntl_reg_soft_rst_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RWDMCNTL_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

__INLINE void ip_rwdmcntl_reg_soft_rst_setf(uint8_t regsoftrst)
{
    ASSERT_ERR((((uint32_t)regsoftrst << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_IP_WR(ip_RWDMCNTL_ADDR, (REG_IP_RD(ip_RWDMCNTL_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)regsoftrst << 29));
}

__INLINE uint8_t ip_rwdmcntl_radiocntl_soft_rst_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RWDMCNTL_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

__INLINE void ip_rwdmcntl_radiocntl_soft_rst_setf(uint8_t radiocntlsoftrst)
{
    ASSERT_ERR((((uint32_t)radiocntlsoftrst << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_IP_WR(ip_RWDMCNTL_ADDR, (REG_IP_RD(ip_RWDMCNTL_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)radiocntlsoftrst << 28));
}

__INLINE uint8_t ip_rwdmcntl_swint_req_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RWDMCNTL_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

__INLINE void ip_rwdmcntl_swint_req_setf(uint8_t swintreq)
{
    ASSERT_ERR((((uint32_t)swintreq << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_IP_WR(ip_RWDMCNTL_ADDR, (REG_IP_RD(ip_RWDMCNTL_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)swintreq << 27));
}

/**
 * @brief VERSION register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24                  TYP   0xA
 *  23:16                  REL   0x0
 *  15:08                  UPG   0x7
 *  07:00                BUILD   0x0
 * </pre>
 */
#define ip_VERSION_ADDR   0x00820004
#define ip_VERSION_OFFSET 0x00000004
#define ip_VERSION_INDEX  0x00000001
#define ip_VERSION_RESET  0x0A000700

__INLINE uint32_t ip_version_get(void)
{
    return REG_IP_RD(ip_VERSION_ADDR);
}

// field definitions
#define ip_TYP_MASK     ((uint32_t)0xFF000000)
#define ip_TYP_LSB      24
#define ip_TYP_WIDTH    ((uint32_t)0x00000008)
#define ip_REL_MASK     ((uint32_t)0x00FF0000)
#define ip_REL_LSB      16
#define ip_REL_WIDTH    ((uint32_t)0x00000008)
#define ip_UPG_MASK     ((uint32_t)0x0000FF00)
#define ip_UPG_LSB      8
#define ip_UPG_WIDTH    ((uint32_t)0x00000008)
#define ip_BUILD_MASK   ((uint32_t)0x000000FF)
#define ip_BUILD_LSB    0
#define ip_BUILD_WIDTH  ((uint32_t)0x00000008)

#define ip_TYP_RST      0xA
#define ip_REL_RST      0x0
#define ip_UPG_RST      0x7
#define ip_BUILD_RST    0x0

__INLINE void ip_version_unpack(uint8_t* typ, uint8_t* rel, uint8_t* upg, uint8_t* build)
{
    uint32_t localVal = REG_IP_RD(ip_VERSION_ADDR);

    *typ = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *rel = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *upg = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *build = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

__INLINE uint8_t ip_version_typ_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_VERSION_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

__INLINE uint8_t ip_version_rel_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_VERSION_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE uint8_t ip_version_upg_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_VERSION_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

__INLINE uint8_t ip_version_build_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_VERSION_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief INTCNTL0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16          ERRORINTMSK   0
 * </pre>
 */
#define ip_INTCNTL0_ADDR   0x0082000C
#define ip_INTCNTL0_OFFSET 0x0000000C
#define ip_INTCNTL0_INDEX  0x00000003
#define ip_INTCNTL0_RESET  0x00000000

__INLINE uint32_t ip_intcntl0_get(void)
{
    return REG_IP_RD(ip_INTCNTL0_ADDR);
}

__INLINE void ip_intcntl0_set(uint32_t value)
{
    REG_IP_WR(ip_INTCNTL0_ADDR, value);
}

// field definitions
#define IP_ERRORINTMSK_BIT    ((uint32_t)0x00010000)
#define ip_ERRORINTMSK_BIT    ((uint32_t)0x00010000)
#define ip_ERRORINTMSK_POS    16

#define ip_ERRORINTMSK_RST    0x0

__INLINE uint8_t ip_intcntl0_errorintmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00010000)) == 0);
    return (localVal >> 16);
}

__INLINE void ip_intcntl0_errorintmsk_setf(uint8_t errorintmsk)
{
    ASSERT_ERR((((uint32_t)errorintmsk << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_IP_WR(ip_INTCNTL0_ADDR, (uint32_t)errorintmsk << 16);
}

/**
 * @brief INTSTAT0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16         ERRORINTSTAT   0
 * </pre>
 */
#define ip_INTSTAT0_ADDR   0x00820010
#define ip_INTSTAT0_OFFSET 0x00000010
#define ip_INTSTAT0_INDEX  0x00000004
#define ip_INTSTAT0_RESET  0x00000000

__INLINE uint32_t ip_intstat0_get(void)
{
    return REG_IP_RD(ip_INTSTAT0_ADDR);
}

// field definitions
#define ip_ERRORINTSTAT_BIT    ((uint32_t)0x00010000)
#define ip_ERRORINTSTAT_POS    16

#define ip_ERRORINTSTAT_RST    0x0

__INLINE uint8_t ip_intstat0_errorintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00010000)) == 0);
    return (localVal >> 16);
}

/**
 * @brief INTACK0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     16          ERRORINTACK   0
 * </pre>
 */
#define ip_INTACK0_ADDR   0x00820014
#define ip_INTACK0_OFFSET 0x00000014
#define ip_INTACK0_INDEX  0x00000005
#define ip_INTACK0_RESET  0x00000000

__INLINE uint32_t ip_intack0_get(void)
{
    return REG_IP_RD(ip_INTACK0_ADDR);
}

__INLINE void ip_intack0_clear(uint32_t value)
{
    REG_IP_WR(ip_INTACK0_ADDR, value);
}

// field definitions
#define ip_ERRORINTACK_BIT    ((uint32_t)0x00010000)
#define ip_ERRORINTACK_POS    16

#define ip_ERRORINTACK_RST    0x0

__INLINE uint8_t ip_intack0_errorintack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00010000)) == 0);
    return (localVal >> 16);
}

__INLINE void ip_intack0_errorintack_clearf(uint8_t errorintack)
{
    ASSERT_ERR((((uint32_t)errorintack << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_IP_WR(ip_INTACK0_ADDR, (uint32_t)errorintack << 16);
}

/**
 * @brief INTCNTL1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  30:28         CLKNINTSRMSK   0x0
 *  27:24         CLKNINTSRVAL   0x0
 *     15           FIFOINTMSK   1
 *     06   TIMESTAMPTGT2INTMSK   0
 *     05   TIMESTAMPTGT1INTMSK   0
 *     04        FINETGTINTMSK   0
 *     03             SWINTMSK   0
 *     02          CRYPTINTMSK   0
 *     01            SLPINTMSK   1
 *     00           CLKNINTMSK   1
 * </pre>
 */
#define ip_INTCNTL1_ADDR   0x00820018
#define ip_INTCNTL1_OFFSET 0x00000018
#define ip_INTCNTL1_INDEX  0x00000006
#define ip_INTCNTL1_RESET  0x00008003

__INLINE uint32_t ip_intcntl1_get(void)
{
    return REG_IP_RD(ip_INTCNTL1_ADDR);
}

__INLINE void ip_intcntl1_set(uint32_t value)
{
    REG_IP_WR(ip_INTCNTL1_ADDR, value);
}

// field definitions
#define IP_CLKNINTSRMSK_MASK          ((uint32_t)0x70000000)
#define IP_CLKNINTSRMSK_LSB           28
#define IP_CLKNINTSRMSK_WIDTH         ((uint32_t)0x00000003)
#define IP_CLKNINTSRVAL_MASK          ((uint32_t)0x0F000000)
#define IP_CLKNINTSRVAL_LSB           24
#define IP_CLKNINTSRVAL_WIDTH         ((uint32_t)0x00000004)
#define IP_FIFOINTMSK_BIT             ((uint32_t)0x00008000)
#define IP_FIFOINTMSK_POS             15
#define IP_TIMESTAMPTGT2INTMSK_BIT    ((uint32_t)0x00000040)
#define IP_TIMESTAMPTGT2INTMSK_POS    6
#define IP_TIMESTAMPTGT1INTMSK_BIT    ((uint32_t)0x00000020)
#define IP_TIMESTAMPTGT1INTMSK_POS    5
#define IP_FINETGTINTMSK_BIT          ((uint32_t)0x00000010)
#define IP_FINETGTINTMSK_POS          4
#define IP_SWINTMSK_BIT               ((uint32_t)0x00000008)
#define IP_SWINTMSK_POS               3
#define IP_CRYPTINTMSK_BIT            ((uint32_t)0x00000004)
#define IP_CRYPTINTMSK_POS            2
#define IP_SLPINTMSK_BIT              ((uint32_t)0x00000002)
#define IP_SLPINTMSK_POS              1
#define IP_CLKNINTMSK_BIT             ((uint32_t)0x00000001)
#define IP_CLKNINTMSK_POS             0

#define ip_CLKNINTSRMSK_MASK          ((uint32_t)0x70000000)
#define ip_CLKNINTSRMSK_LSB           28
#define ip_CLKNINTSRMSK_WIDTH         ((uint32_t)0x00000003)
#define ip_CLKNINTSRVAL_MASK          ((uint32_t)0x0F000000)
#define ip_CLKNINTSRVAL_LSB           24
#define ip_CLKNINTSRVAL_WIDTH         ((uint32_t)0x00000004)
#define ip_FIFOINTMSK_BIT             ((uint32_t)0x00008000)
#define ip_FIFOINTMSK_POS             15
#define ip_TIMESTAMPTGT2INTMSK_BIT    ((uint32_t)0x00000040)
#define ip_TIMESTAMPTGT2INTMSK_POS    6
#define ip_TIMESTAMPTGT1INTMSK_BIT    ((uint32_t)0x00000020)
#define ip_TIMESTAMPTGT1INTMSK_POS    5
#define ip_FINETGTINTMSK_BIT          ((uint32_t)0x00000010)
#define ip_FINETGTINTMSK_POS          4
#define ip_SWINTMSK_BIT               ((uint32_t)0x00000008)
#define ip_SWINTMSK_POS               3
#define ip_CRYPTINTMSK_BIT            ((uint32_t)0x00000004)
#define ip_CRYPTINTMSK_POS            2
#define ip_SLPINTMSK_BIT              ((uint32_t)0x00000002)
#define ip_SLPINTMSK_POS              1
#define ip_CLKNINTMSK_BIT             ((uint32_t)0x00000001)
#define ip_CLKNINTMSK_POS             0

#define ip_CLKNINTSRMSK_RST           0x0
#define ip_CLKNINTSRVAL_RST           0x0
#define ip_FIFOINTMSK_RST             0x1
#define ip_TIMESTAMPTGT2INTMSK_RST    0x0
#define ip_TIMESTAMPTGT1INTMSK_RST    0x0
#define ip_FINETGTINTMSK_RST          0x0
#define ip_SWINTMSK_RST               0x0
#define ip_CRYPTINTMSK_RST            0x0
#define ip_SLPINTMSK_RST              0x1
#define ip_CLKNINTMSK_RST             0x1

__INLINE void ip_intcntl1_pack(uint8_t clknintsrmsk, uint8_t clknintsrval, uint8_t fifointmsk, uint8_t timestamptgt2intmsk, uint8_t timestamptgt1intmsk, uint8_t finetgtintmsk, uint8_t swintmsk, uint8_t cryptintmsk, uint8_t slpintmsk, uint8_t clknintmsk)
{
    ASSERT_ERR((((uint32_t)clknintsrmsk << 28) & ~((uint32_t)0x70000000)) == 0);
    ASSERT_ERR((((uint32_t)clknintsrval << 24) & ~((uint32_t)0x0F000000)) == 0);
    ASSERT_ERR((((uint32_t)fifointmsk << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)timestamptgt2intmsk << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)timestamptgt1intmsk << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)finetgtintmsk << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)swintmsk << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)cryptintmsk << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)slpintmsk << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)clknintmsk << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR,  ((uint32_t)clknintsrmsk << 28) | ((uint32_t)clknintsrval << 24) | ((uint32_t)fifointmsk << 15) | ((uint32_t)timestamptgt2intmsk << 6) | ((uint32_t)timestamptgt1intmsk << 5) | ((uint32_t)finetgtintmsk << 4) | ((uint32_t)swintmsk << 3) | ((uint32_t)cryptintmsk << 2) | ((uint32_t)slpintmsk << 1) | ((uint32_t)clknintmsk << 0));
}

__INLINE void ip_intcntl1_unpack(uint8_t* clknintsrmsk, uint8_t* clknintsrval, uint8_t* fifointmsk, uint8_t* timestamptgt2intmsk, uint8_t* timestamptgt1intmsk, uint8_t* finetgtintmsk, uint8_t* swintmsk, uint8_t* cryptintmsk, uint8_t* slpintmsk, uint8_t* clknintmsk)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);

    *clknintsrmsk = (localVal & ((uint32_t)0x70000000)) >> 28;
    *clknintsrval = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *fifointmsk = (localVal & ((uint32_t)0x00008000)) >> 15;
    *timestamptgt2intmsk = (localVal & ((uint32_t)0x00000040)) >> 6;
    *timestamptgt1intmsk = (localVal & ((uint32_t)0x00000020)) >> 5;
    *finetgtintmsk = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swintmsk = (localVal & ((uint32_t)0x00000008)) >> 3;
    *cryptintmsk = (localVal & ((uint32_t)0x00000004)) >> 2;
    *slpintmsk = (localVal & ((uint32_t)0x00000002)) >> 1;
    *clknintmsk = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_intcntl1_clknintsrmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

__INLINE void ip_intcntl1_clknintsrmsk_setf(uint8_t clknintsrmsk)
{
    ASSERT_ERR((((uint32_t)clknintsrmsk << 28) & ~((uint32_t)0x70000000)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x70000000)) | ((uint32_t)clknintsrmsk << 28));
}

__INLINE uint8_t ip_intcntl1_clknintsrval_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

__INLINE void ip_intcntl1_clknintsrval_setf(uint8_t clknintsrval)
{
    ASSERT_ERR((((uint32_t)clknintsrval << 24) & ~((uint32_t)0x0F000000)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x0F000000)) | ((uint32_t)clknintsrval << 24));
}

__INLINE uint8_t ip_intcntl1_fifointmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE void ip_intcntl1_fifointmsk_setf(uint8_t fifointmsk)
{
    ASSERT_ERR((((uint32_t)fifointmsk << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)fifointmsk << 15));
}

__INLINE uint8_t ip_intcntl1_timestamptgt2intmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

__INLINE void ip_intcntl1_timestamptgt2intmsk_setf(uint8_t timestamptgt2intmsk)
{
    ASSERT_ERR((((uint32_t)timestamptgt2intmsk << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)timestamptgt2intmsk << 6));
}

__INLINE uint8_t ip_intcntl1_timestamptgt1intmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE void ip_intcntl1_timestamptgt1intmsk_setf(uint8_t timestamptgt1intmsk)
{
    ASSERT_ERR((((uint32_t)timestamptgt1intmsk << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)timestamptgt1intmsk << 5));
}

__INLINE uint8_t ip_intcntl1_finetgtintmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE void ip_intcntl1_finetgtintmsk_setf(uint8_t finetgtintmsk)
{
    ASSERT_ERR((((uint32_t)finetgtintmsk << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)finetgtintmsk << 4));
}

__INLINE uint8_t ip_intcntl1_swintmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void ip_intcntl1_swintmsk_setf(uint8_t swintmsk)
{
    ASSERT_ERR((((uint32_t)swintmsk << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)swintmsk << 3));
}

__INLINE uint8_t ip_intcntl1_cryptintmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void ip_intcntl1_cryptintmsk_setf(uint8_t cryptintmsk)
{
    ASSERT_ERR((((uint32_t)cryptintmsk << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)cryptintmsk << 2));
}

__INLINE uint8_t ip_intcntl1_slpintmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void ip_intcntl1_slpintmsk_setf(uint8_t slpintmsk)
{
    ASSERT_ERR((((uint32_t)slpintmsk << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)slpintmsk << 1));
}

__INLINE uint8_t ip_intcntl1_clknintmsk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void ip_intcntl1_clknintmsk_setf(uint8_t clknintmsk)
{
    ASSERT_ERR((((uint32_t)clknintmsk << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_INTCNTL1_ADDR, (REG_IP_RD(ip_INTCNTL1_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)clknintmsk << 0));
}

/**
 * @brief INTSTAT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15          FIFOINTSTAT   0
 *     06   TIMESTAMPTGT2INTSTAT   0
 *     05   TIMESTAMPTGT1INTSTAT   0
 *     04       FINETGTINTSTAT   0
 *     03            SWINTSTAT   0
 *     02         CRYPTINTSTAT   0
 *     01           SLPINTSTAT   0
 *     00          CLKNINTSTAT   0
 * </pre>
 */
#define ip_INTSTAT1_ADDR   0x0082001C
#define ip_INTSTAT1_OFFSET 0x0000001C
#define ip_INTSTAT1_INDEX  0x00000007
#define ip_INTSTAT1_RESET  0x00000000

__INLINE uint32_t ip_intstat1_get(void)
{
    return REG_IP_RD(ip_INTSTAT1_ADDR);
}

// field definitions
#define IP_FIFOINTSTAT_BIT             ((uint32_t)0x00008000)
#define IP_FIFOINTSTAT_POS             15
#define IP_TIMESTAMPTGT2INTSTAT_BIT    ((uint32_t)0x00000040)
#define IP_TIMESTAMPTGT2INTSTAT_POS    6
#define IP_TIMESTAMPTGT1INTSTAT_BIT    ((uint32_t)0x00000020)
#define IP_TIMESTAMPTGT1INTSTAT_POS    5
#define IP_FINETGTINTSTAT_BIT          ((uint32_t)0x00000010)
#define IP_FINETGTINTSTAT_POS          4
#define IP_SWINTSTAT_BIT               ((uint32_t)0x00000008)
#define IP_SWINTSTAT_POS               3
#define IP_CRYPTINTSTAT_BIT            ((uint32_t)0x00000004)
#define IP_CRYPTINTSTAT_POS            2
#define IP_SLPINTSTAT_BIT              ((uint32_t)0x00000002)
#define IP_SLPINTSTAT_POS              1
#define IP_CLKNINTSTAT_BIT             ((uint32_t)0x00000001)
#define IP_CLKNINTSTAT_POS             0

#define ip_FIFOINTSTAT_BIT             ((uint32_t)0x00008000)
#define ip_FIFOINTSTAT_POS             15
#define ip_TIMESTAMPTGT2INTSTAT_BIT    ((uint32_t)0x00000040)
#define ip_TIMESTAMPTGT2INTSTAT_POS    6
#define ip_TIMESTAMPTGT1INTSTAT_BIT    ((uint32_t)0x00000020)
#define ip_TIMESTAMPTGT1INTSTAT_POS    5
#define ip_FINETGTINTSTAT_BIT          ((uint32_t)0x00000010)
#define ip_FINETGTINTSTAT_POS          4
#define ip_SWINTSTAT_BIT               ((uint32_t)0x00000008)
#define ip_SWINTSTAT_POS               3
#define ip_CRYPTINTSTAT_BIT            ((uint32_t)0x00000004)
#define ip_CRYPTINTSTAT_POS            2
#define ip_SLPINTSTAT_BIT              ((uint32_t)0x00000002)
#define ip_SLPINTSTAT_POS              1
#define ip_CLKNINTSTAT_BIT             ((uint32_t)0x00000001)
#define ip_CLKNINTSTAT_POS             0

#define ip_FIFOINTSTAT_RST             0x0
#define ip_TIMESTAMPTGT2INTSTAT_RST    0x0
#define ip_TIMESTAMPTGT1INTSTAT_RST    0x0
#define ip_FINETGTINTSTAT_RST          0x0
#define ip_SWINTSTAT_RST               0x0
#define ip_CRYPTINTSTAT_RST            0x0
#define ip_SLPINTSTAT_RST              0x0
#define ip_CLKNINTSTAT_RST             0x0

__INLINE void ip_intstat1_unpack(uint8_t* fifointstat, uint8_t* timestamptgt2intstat, uint8_t* timestamptgt1intstat, uint8_t* finetgtintstat, uint8_t* swintstat, uint8_t* cryptintstat, uint8_t* slpintstat, uint8_t* clknintstat)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);

    *fifointstat = (localVal & ((uint32_t)0x00008000)) >> 15;
    *timestamptgt2intstat = (localVal & ((uint32_t)0x00000040)) >> 6;
    *timestamptgt1intstat = (localVal & ((uint32_t)0x00000020)) >> 5;
    *finetgtintstat = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swintstat = (localVal & ((uint32_t)0x00000008)) >> 3;
    *cryptintstat = (localVal & ((uint32_t)0x00000004)) >> 2;
    *slpintstat = (localVal & ((uint32_t)0x00000002)) >> 1;
    *clknintstat = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_intstat1_fifointstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE uint8_t ip_intstat1_timestamptgt2intstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

__INLINE uint8_t ip_intstat1_timestamptgt1intstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE uint8_t ip_intstat1_finetgtintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE uint8_t ip_intstat1_swintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE uint8_t ip_intstat1_cryptintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE uint8_t ip_intstat1_slpintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE uint8_t ip_intstat1_clknintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTSTAT1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief INTACK1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15           FIFOINTACK   0
 *     06   TIMESTAMPTGT2INTACK   0
 *     05   TIMESTAMPTGT1INTACK   0
 *     04        FINETGTINTACK   0
 *     03             SWINTACK   0
 *     02          CRYPTINTACK   0
 *     01            SLPINTACK   0
 *     00           CLKNINTACK   0
 * </pre>
 */
#define ip_INTACK1_ADDR   0x00820020
#define ip_INTACK1_OFFSET 0x00000020
#define ip_INTACK1_INDEX  0x00000008
#define ip_INTACK1_RESET  0x00000000

__INLINE uint32_t ip_intack1_get(void)
{
    return REG_IP_RD(ip_INTACK1_ADDR);
}

__INLINE void ip_intack1_clear(uint32_t value)
{
    REG_IP_WR(ip_INTACK1_ADDR, value);
}

// field definitions
#define ip_FIFOINTACK_BIT             ((uint32_t)0x00008000)
#define ip_FIFOINTACK_POS             15
#define ip_TIMESTAMPTGT2INTACK_BIT    ((uint32_t)0x00000040)
#define ip_TIMESTAMPTGT2INTACK_POS    6
#define ip_TIMESTAMPTGT1INTACK_BIT    ((uint32_t)0x00000020)
#define ip_TIMESTAMPTGT1INTACK_POS    5
#define ip_FINETGTINTACK_BIT          ((uint32_t)0x00000010)
#define ip_FINETGTINTACK_POS          4
#define ip_SWINTACK_BIT               ((uint32_t)0x00000008)
#define ip_SWINTACK_POS               3
#define ip_CRYPTINTACK_BIT            ((uint32_t)0x00000004)
#define ip_CRYPTINTACK_POS            2
#define ip_SLPINTACK_BIT              ((uint32_t)0x00000002)
#define ip_SLPINTACK_POS              1
#define ip_CLKNINTACK_BIT             ((uint32_t)0x00000001)
#define ip_CLKNINTACK_POS             0

#define ip_FIFOINTACK_RST             0x0
#define ip_TIMESTAMPTGT2INTACK_RST    0x0
#define ip_TIMESTAMPTGT1INTACK_RST    0x0
#define ip_FINETGTINTACK_RST          0x0
#define ip_SWINTACK_RST               0x0
#define ip_CRYPTINTACK_RST            0x0
#define ip_SLPINTACK_RST              0x0
#define ip_CLKNINTACK_RST             0x0

__INLINE void ip_intack1_pack(uint8_t fifointack, uint8_t timestamptgt2intack, uint8_t timestamptgt1intack, uint8_t finetgtintack, uint8_t swintack, uint8_t cryptintack, uint8_t slpintack, uint8_t clknintack)
{
    ASSERT_ERR((((uint32_t)fifointack << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)timestamptgt2intack << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)timestamptgt1intack << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)finetgtintack << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)swintack << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)cryptintack << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)slpintack << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)clknintack << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR,  ((uint32_t)fifointack << 15) | ((uint32_t)timestamptgt2intack << 6) | ((uint32_t)timestamptgt1intack << 5) | ((uint32_t)finetgtintack << 4) | ((uint32_t)swintack << 3) | ((uint32_t)cryptintack << 2) | ((uint32_t)slpintack << 1) | ((uint32_t)clknintack << 0));
}

__INLINE void ip_intack1_unpack(uint8_t* fifointack, uint8_t* timestamptgt2intack, uint8_t* timestamptgt1intack, uint8_t* finetgtintack, uint8_t* swintack, uint8_t* cryptintack, uint8_t* slpintack, uint8_t* clknintack)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);

    *fifointack = (localVal & ((uint32_t)0x00008000)) >> 15;
    *timestamptgt2intack = (localVal & ((uint32_t)0x00000040)) >> 6;
    *timestamptgt1intack = (localVal & ((uint32_t)0x00000020)) >> 5;
    *finetgtintack = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swintack = (localVal & ((uint32_t)0x00000008)) >> 3;
    *cryptintack = (localVal & ((uint32_t)0x00000004)) >> 2;
    *slpintack = (localVal & ((uint32_t)0x00000002)) >> 1;
    *clknintack = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_intack1_fifointack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE void ip_intack1_fifointack_clearf(uint8_t fifointack)
{
    ASSERT_ERR((((uint32_t)fifointack << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)fifointack << 15);
}

__INLINE uint8_t ip_intack1_timestamptgt2intack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

__INLINE void ip_intack1_timestamptgt2intack_clearf(uint8_t timestamptgt2intack)
{
    ASSERT_ERR((((uint32_t)timestamptgt2intack << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)timestamptgt2intack << 6);
}

__INLINE uint8_t ip_intack1_timestamptgt1intack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE void ip_intack1_timestamptgt1intack_clearf(uint8_t timestamptgt1intack)
{
    ASSERT_ERR((((uint32_t)timestamptgt1intack << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)timestamptgt1intack << 5);
}

__INLINE uint8_t ip_intack1_finetgtintack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE void ip_intack1_finetgtintack_clearf(uint8_t finetgtintack)
{
    ASSERT_ERR((((uint32_t)finetgtintack << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)finetgtintack << 4);
}

__INLINE uint8_t ip_intack1_swintack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void ip_intack1_swintack_clearf(uint8_t swintack)
{
    ASSERT_ERR((((uint32_t)swintack << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)swintack << 3);
}

__INLINE uint8_t ip_intack1_cryptintack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void ip_intack1_cryptintack_clearf(uint8_t cryptintack)
{
    ASSERT_ERR((((uint32_t)cryptintack << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)cryptintack << 2);
}

__INLINE uint8_t ip_intack1_slpintack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void ip_intack1_slpintack_clearf(uint8_t slpintack)
{
    ASSERT_ERR((((uint32_t)slpintack << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)slpintack << 1);
}

__INLINE uint8_t ip_intack1_clknintack_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_INTACK1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void ip_intack1_clknintack_clearf(uint8_t clknintack)
{
    ASSERT_ERR((((uint32_t)clknintack << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_INTACK1_ADDR, (uint32_t)clknintack << 0);
}

/**
 * @brief ACTFIFOSTAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:28          SKIP_ET_IDX   0x0
 *  27:24       CURRENT_ET_IDX   0x0
 *     15              ACTFLAG   0
 *     06         ISORXINTSTAT   0
 *     05         ISOTXINTSTAT   0
 *     04            RXINTSTAT   0
 *     03            TXINTSTAT   0
 *     02       SKIPACTINTSTAT   0
 *     01        ENDACTINTSTAT   0
 *     00      STARTACTINTSTAT   0
 * </pre>
 */
#define ip_ACTFIFOSTAT_ADDR   0x00820024
#define ip_ACTFIFOSTAT_OFFSET 0x00000024
#define ip_ACTFIFOSTAT_INDEX  0x00000009
#define ip_ACTFIFOSTAT_RESET  0x00000000

__INLINE uint32_t ip_actfifostat_get(void)
{
    return REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
}

// field definitions
#define IP_SKIP_ET_IDX_MASK       ((uint32_t)0xF0000000)
#define IP_SKIP_ET_IDX_LSB        28
#define IP_SKIP_ET_IDX_WIDTH      ((uint32_t)0x00000004)
#define IP_CURRENT_ET_IDX_MASK    ((uint32_t)0x0F000000)
#define IP_CURRENT_ET_IDX_LSB     24
#define IP_CURRENT_ET_IDX_WIDTH   ((uint32_t)0x00000004)
#define IP_ACTFLAG_BIT            ((uint32_t)0x00008000)
#define IP_ACTFLAG_POS            15
#define IP_ISORXINTSTAT_BIT       ((uint32_t)0x00000040)
#define IP_ISORXINTSTAT_POS       6
#define IP_ISOTXINTSTAT_BIT       ((uint32_t)0x00000020)
#define IP_ISOTXINTSTAT_POS       5
#define IP_RXINTSTAT_BIT          ((uint32_t)0x00000010)
#define IP_RXINTSTAT_POS          4
#define IP_TXINTSTAT_BIT          ((uint32_t)0x00000008)
#define IP_TXINTSTAT_POS          3
#define IP_SKIPACTINTSTAT_BIT     ((uint32_t)0x00000004)
#define IP_SKIPACTINTSTAT_POS     2
#define IP_ENDACTINTSTAT_BIT      ((uint32_t)0x00000002)
#define IP_ENDACTINTSTAT_POS      1
#define IP_STARTACTINTSTAT_BIT    ((uint32_t)0x00000001)
#define IP_STARTACTINTSTAT_POS    0

#define ip_SKIP_ET_IDX_MASK       ((uint32_t)0xF0000000)
#define ip_SKIP_ET_IDX_LSB        28
#define ip_SKIP_ET_IDX_WIDTH      ((uint32_t)0x00000004)
#define ip_CURRENT_ET_IDX_MASK    ((uint32_t)0x0F000000)
#define ip_CURRENT_ET_IDX_LSB     24
#define ip_CURRENT_ET_IDX_WIDTH   ((uint32_t)0x00000004)
#define ip_ACTFLAG_BIT            ((uint32_t)0x00008000)
#define ip_ACTFLAG_POS            15
#define ip_ISORXINTSTAT_BIT       ((uint32_t)0x00000040)
#define ip_ISORXINTSTAT_POS       6
#define ip_ISOTXINTSTAT_BIT       ((uint32_t)0x00000020)
#define ip_ISOTXINTSTAT_POS       5
#define ip_RXINTSTAT_BIT          ((uint32_t)0x00000010)
#define ip_RXINTSTAT_POS          4
#define ip_TXINTSTAT_BIT          ((uint32_t)0x00000008)
#define ip_TXINTSTAT_POS          3
#define ip_SKIPACTINTSTAT_BIT     ((uint32_t)0x00000004)
#define ip_SKIPACTINTSTAT_POS     2
#define ip_ENDACTINTSTAT_BIT      ((uint32_t)0x00000002)
#define ip_ENDACTINTSTAT_POS      1
#define ip_STARTACTINTSTAT_BIT    ((uint32_t)0x00000001)
#define ip_STARTACTINTSTAT_POS    0

#define ip_SKIP_ET_IDX_MASK       ((uint32_t)0xF0000000)
#define ip_SKIP_ET_IDX_LSB        28
#define ip_SKIP_ET_IDX_WIDTH      ((uint32_t)0x00000004)
#define ip_CURRENT_ET_IDX_MASK    ((uint32_t)0x0F000000)
#define ip_CURRENT_ET_IDX_LSB     24
#define ip_CURRENT_ET_IDX_WIDTH   ((uint32_t)0x00000004)
#define ip_ACTFLAG_BIT            ((uint32_t)0x00008000)
#define ip_ACTFLAG_POS            15
#define ip_ISORXINTSTAT_BIT       ((uint32_t)0x00000040)
#define ip_ISORXINTSTAT_POS       6
#define ip_ISOTXINTSTAT_BIT       ((uint32_t)0x00000020)
#define ip_ISOTXINTSTAT_POS       5
#define ip_RXINTSTAT_BIT          ((uint32_t)0x00000010)
#define ip_RXINTSTAT_POS          4
#define ip_TXINTSTAT_BIT          ((uint32_t)0x00000008)
#define ip_TXINTSTAT_POS          3
#define ip_SKIPACTINTSTAT_BIT     ((uint32_t)0x00000004)
#define ip_SKIPACTINTSTAT_POS     2
#define ip_ENDACTINTSTAT_BIT      ((uint32_t)0x00000002)
#define ip_ENDACTINTSTAT_POS      1
#define ip_STARTACTINTSTAT_BIT    ((uint32_t)0x00000001)
#define ip_STARTACTINTSTAT_POS    0

#define ip_SKIP_ET_IDX_RST        0x0
#define ip_CURRENT_ET_IDX_RST     0x0
#define ip_ACTFLAG_RST            0x0
#define ip_ISORXINTSTAT_RST       0x0
#define ip_ISOTXINTSTAT_RST       0x0
#define ip_RXINTSTAT_RST          0x0
#define ip_TXINTSTAT_RST          0x0
#define ip_SKIPACTINTSTAT_RST     0x0
#define ip_ENDACTINTSTAT_RST      0x0
#define ip_STARTACTINTSTAT_RST    0x0

__INLINE void ip_actfifostat_unpack(uint8_t* skipetidx, uint8_t* currentetidx, uint8_t* actflag, uint8_t* isorxintstat, uint8_t* isotxintstat, uint8_t* rxintstat, uint8_t* txintstat, uint8_t* skipactintstat, uint8_t* endactintstat, uint8_t* startactintstat)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);

    *skipetidx = (localVal & ((uint32_t)0xF0000000)) >> 28;
    *currentetidx = (localVal & ((uint32_t)0x0F000000)) >> 24;
    *actflag = (localVal & ((uint32_t)0x00008000)) >> 15;
    *isorxintstat = (localVal & ((uint32_t)0x00000040)) >> 6;
    *isotxintstat = (localVal & ((uint32_t)0x00000020)) >> 5;
    *rxintstat = (localVal & ((uint32_t)0x00000010)) >> 4;
    *txintstat = (localVal & ((uint32_t)0x00000008)) >> 3;
    *skipactintstat = (localVal & ((uint32_t)0x00000004)) >> 2;
    *endactintstat = (localVal & ((uint32_t)0x00000002)) >> 1;
    *startactintstat = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_actfifostat_skip_et_idx_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0xF0000000)) >> 28);
}

__INLINE uint8_t ip_actfifostat_current_et_idx_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x0F000000)) >> 24);
}

__INLINE uint8_t ip_actfifostat_actflag_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE uint8_t ip_actfifostat_isorxintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

__INLINE uint8_t ip_actfifostat_isotxintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE uint8_t ip_actfifostat_rxintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE uint8_t ip_actfifostat_txintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE uint8_t ip_actfifostat_skipactintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE uint8_t ip_actfifostat_endactintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE uint8_t ip_actfifostat_startactintstat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTFIFOSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief ETPTR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:00                ETPTR   0x0
 * </pre>
 */
#define ip_ETPTR_ADDR   0x0082002C
#define ip_ETPTR_OFFSET 0x0000002C
#define ip_ETPTR_INDEX  0x0000000B
#define ip_ETPTR_RESET  0x00000000

__INLINE uint32_t ip_etptr_get(void)
{
    return REG_IP_RD(ip_ETPTR_ADDR);
}

__INLINE void ip_etptr_set(uint32_t value)
{
    REG_IP_WR(ip_ETPTR_ADDR, value);
}

// field definitions
#define ip_ETPTR_MASK   ((uint32_t)0x00003FFF)
#define ip_ETPTR_LSB    0
#define ip_ETPTR_WIDTH  ((uint32_t)0x0000000E)

#define ip_ETPTR_RST    0x0

__INLINE uint16_t ip_etptr_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ETPTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00003FFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_etptr_setf(uint16_t etptr)
{
    ASSERT_ERR((((uint32_t)etptr << 0) & ~((uint32_t)0x00003FFF)) == 0);
    REG_IP_WR(ip_ETPTR_ADDR, (uint32_t)etptr << 0);
}

/**
 * @brief DEEPSLCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31           EXTWKUPDSB   0
 *     15      DEEP_SLEEP_STAT   0
 *     03   DEEP_SLEEP_CORR_EN   0
 *     02        DEEP_SLEEP_ON   0
 *     01       RADIO_SLEEP_EN   0
 *     00         OSC_SLEEP_EN   0
 * </pre>
 */
#define ip_DEEPSLCNTL_ADDR   0x00820030
#define ip_DEEPSLCNTL_OFFSET 0x00000030
#define ip_DEEPSLCNTL_INDEX  0x0000000C
#define ip_DEEPSLCNTL_RESET  0x00000000

__INLINE uint32_t ip_deepslcntl_get(void)
{
    return REG_IP_RD(ip_DEEPSLCNTL_ADDR);
}

__INLINE void ip_deepslcntl_set(uint32_t value)
{
    REG_IP_WR(ip_DEEPSLCNTL_ADDR, value);
}

// field definitions
#define ip_EXTWKUPDSB_BIT            ((uint32_t)0x80000000)
#define ip_EXTWKUPDSB_POS            31
#define ip_DEEP_SLEEP_STAT_BIT       ((uint32_t)0x00008000)
#define ip_DEEP_SLEEP_STAT_POS       15
#define ip_DEEP_SLEEP_CORR_EN_BIT    ((uint32_t)0x00000008)
#define ip_DEEP_SLEEP_CORR_EN_POS    3
#define ip_DEEP_SLEEP_ON_BIT         ((uint32_t)0x00000004)
#define ip_DEEP_SLEEP_ON_POS         2
#define ip_RADIO_SLEEP_EN_BIT        ((uint32_t)0x00000002)
#define ip_RADIO_SLEEP_EN_POS        1
#define ip_OSC_SLEEP_EN_BIT          ((uint32_t)0x00000001)
#define ip_OSC_SLEEP_EN_POS          0

#define ip_EXTWKUPDSB_RST            0x0
#define ip_DEEP_SLEEP_STAT_RST       0x0
#define ip_DEEP_SLEEP_CORR_EN_RST    0x0
#define ip_DEEP_SLEEP_ON_RST         0x0
#define ip_RADIO_SLEEP_EN_RST        0x0
#define ip_OSC_SLEEP_EN_RST          0x0

__INLINE void ip_deepslcntl_pack(uint8_t extwkupdsb, uint8_t deepsleepcorren, uint8_t deepsleepon, uint8_t radiosleepen, uint8_t oscsleepen)
{
    ASSERT_ERR((((uint32_t)extwkupdsb << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)deepsleepcorren << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)deepsleepon << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)radiosleepen << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)oscsleepen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_DEEPSLCNTL_ADDR,  ((uint32_t)extwkupdsb << 31) | ((uint32_t)deepsleepcorren << 3) | ((uint32_t)deepsleepon << 2) | ((uint32_t)radiosleepen << 1) | ((uint32_t)oscsleepen << 0));
}

__INLINE void ip_deepslcntl_unpack(uint8_t* extwkupdsb, uint8_t* deepsleepstat, uint8_t* deepsleepcorren, uint8_t* deepsleepon, uint8_t* radiosleepen, uint8_t* oscsleepen)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);

    *extwkupdsb = (localVal & ((uint32_t)0x80000000)) >> 31;
    *deepsleepstat = (localVal & ((uint32_t)0x00008000)) >> 15;
    *deepsleepcorren = (localVal & ((uint32_t)0x00000008)) >> 3;
    *deepsleepon = (localVal & ((uint32_t)0x00000004)) >> 2;
    *radiosleepen = (localVal & ((uint32_t)0x00000002)) >> 1;
    *oscsleepen = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_deepslcntl_extwkupdsb_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_deepslcntl_extwkupdsb_setf(uint8_t extwkupdsb)
{
    ASSERT_ERR((((uint32_t)extwkupdsb << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_DEEPSLCNTL_ADDR, (REG_IP_RD(ip_DEEPSLCNTL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)extwkupdsb << 31));
}

__INLINE uint8_t ip_deepslcntl_deep_sleep_stat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE uint8_t ip_deepslcntl_deep_sleep_corr_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void ip_deepslcntl_deep_sleep_corr_en_setf(uint8_t deepsleepcorren)
{
    ASSERT_ERR((((uint32_t)deepsleepcorren << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_IP_WR(ip_DEEPSLCNTL_ADDR, (REG_IP_RD(ip_DEEPSLCNTL_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)deepsleepcorren << 3));
}

__INLINE uint8_t ip_deepslcntl_deep_sleep_on_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void ip_deepslcntl_deep_sleep_on_setf(uint8_t deepsleepon)
{
    ASSERT_ERR((((uint32_t)deepsleepon << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_IP_WR(ip_DEEPSLCNTL_ADDR, (REG_IP_RD(ip_DEEPSLCNTL_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)deepsleepon << 2));
}

__INLINE uint8_t ip_deepslcntl_radio_sleep_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void ip_deepslcntl_radio_sleep_en_setf(uint8_t radiosleepen)
{
    ASSERT_ERR((((uint32_t)radiosleepen << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_IP_WR(ip_DEEPSLCNTL_ADDR, (REG_IP_RD(ip_DEEPSLCNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)radiosleepen << 1));
}

__INLINE uint8_t ip_deepslcntl_osc_sleep_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void ip_deepslcntl_osc_sleep_en_setf(uint8_t oscsleepen)
{
    ASSERT_ERR((((uint32_t)oscsleepen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_DEEPSLCNTL_ADDR, (REG_IP_RD(ip_DEEPSLCNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)oscsleepen << 0));
}

/**
 * @brief DEEPSLWKUP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           DEEPSLTIME   0x0
 * </pre>
 */
#define ip_DEEPSLWKUP_ADDR   0x00820034
#define ip_DEEPSLWKUP_OFFSET 0x00000034
#define ip_DEEPSLWKUP_INDEX  0x0000000D
#define ip_DEEPSLWKUP_RESET  0x00000000

__INLINE uint32_t ip_deepslwkup_get(void)
{
    return REG_IP_RD(ip_DEEPSLWKUP_ADDR);
}

__INLINE void ip_deepslwkup_set(uint32_t value)
{
    REG_IP_WR(ip_DEEPSLWKUP_ADDR, value);
}

// field definitions
#define ip_DEEPSLTIME_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_DEEPSLTIME_LSB    0
#define ip_DEEPSLTIME_WIDTH  ((uint32_t)0x00000020)

#define ip_DEEPSLTIME_RST    0x0

__INLINE uint32_t ip_deepslwkup_deepsltime_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLWKUP_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_deepslwkup_deepsltime_setf(uint32_t deepsltime)
{
    ASSERT_ERR((((uint32_t)deepsltime << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(ip_DEEPSLWKUP_ADDR, (uint32_t)deepsltime << 0);
}

/**
 * @brief DEEPSLSTAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00            DEEPSLDUR   0x0
 * </pre>
 */
#define ip_DEEPSLSTAT_ADDR   0x00820038
#define ip_DEEPSLSTAT_OFFSET 0x00000038
#define ip_DEEPSLSTAT_INDEX  0x0000000E
#define ip_DEEPSLSTAT_RESET  0x00000000

__INLINE uint32_t ip_deepslstat_get(void)
{
    return REG_IP_RD(ip_DEEPSLSTAT_ADDR);
}

// field definitions
#define ip_DEEPSLDUR_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_DEEPSLDUR_LSB    0
#define ip_DEEPSLDUR_WIDTH  ((uint32_t)0x00000020)

#define ip_DEEPSLDUR_RST    0x0

__INLINE uint32_t ip_deepslstat_deepsldur_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEEPSLSTAT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief ENBPRESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:21                TWEXT   0xA0
 *  20:10                TWOSC   0xA0
 *  09:00                 TWRM   0x20
 * </pre>
 */
#define ip_ENBPRESET_ADDR   0x0082003C
#define ip_ENBPRESET_OFFSET 0x0000003C
#define ip_ENBPRESET_INDEX  0x0000000F
#define ip_ENBPRESET_RESET  0x14028020

__INLINE uint32_t ip_enbpreset_get(void)
{
    return REG_IP_RD(ip_ENBPRESET_ADDR);
}

__INLINE void ip_enbpreset_set(uint32_t value)
{
    REG_IP_WR(ip_ENBPRESET_ADDR, value);
}

// field definitions
#define ip_TWEXT_MASK   ((uint32_t)0xFFE00000)
#define ip_TWEXT_LSB    21
#define ip_TWEXT_WIDTH  ((uint32_t)0x0000000B)
#define ip_TWOSC_MASK   ((uint32_t)0x001FFC00)
#define ip_TWOSC_LSB    10
#define ip_TWOSC_WIDTH  ((uint32_t)0x0000000B)
#define ip_TWRM_MASK    ((uint32_t)0x000003FF)
#define ip_TWRM_LSB     0
#define ip_TWRM_WIDTH   ((uint32_t)0x0000000A)

#define ip_TWEXT_RST    0xA0
#define ip_TWOSC_RST    0xA0
#define ip_TWRM_RST     0x20

__INLINE void ip_enbpreset_pack(uint16_t twext, uint16_t twosc, uint16_t twrm)
{
    ASSERT_ERR((((uint32_t)twext << 21) & ~((uint32_t)0xFFE00000)) == 0);
    ASSERT_ERR((((uint32_t)twosc << 10) & ~((uint32_t)0x001FFC00)) == 0);
    ASSERT_ERR((((uint32_t)twrm << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_IP_WR(ip_ENBPRESET_ADDR,  ((uint32_t)twext << 21) | ((uint32_t)twosc << 10) | ((uint32_t)twrm << 0));
}

__INLINE void ip_enbpreset_unpack(uint16_t* twext, uint16_t* twosc, uint16_t* twrm)
{
    uint32_t localVal = REG_IP_RD(ip_ENBPRESET_ADDR);

    *twext = (localVal & ((uint32_t)0xFFE00000)) >> 21;
    *twosc = (localVal & ((uint32_t)0x001FFC00)) >> 10;
    *twrm = (localVal & ((uint32_t)0x000003FF)) >> 0;
}

__INLINE uint16_t ip_enbpreset_twext_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ENBPRESET_ADDR);
    return ((localVal & ((uint32_t)0xFFE00000)) >> 21);
}

__INLINE void ip_enbpreset_twext_setf(uint16_t twext)
{
    ASSERT_ERR((((uint32_t)twext << 21) & ~((uint32_t)0xFFE00000)) == 0);
    REG_IP_WR(ip_ENBPRESET_ADDR, (REG_IP_RD(ip_ENBPRESET_ADDR) & ~((uint32_t)0xFFE00000)) | ((uint32_t)twext << 21));
}

__INLINE uint16_t ip_enbpreset_twosc_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ENBPRESET_ADDR);
    return ((localVal & ((uint32_t)0x001FFC00)) >> 10);
}

__INLINE void ip_enbpreset_twosc_setf(uint16_t twosc)
{
    ASSERT_ERR((((uint32_t)twosc << 10) & ~((uint32_t)0x001FFC00)) == 0);
    REG_IP_WR(ip_ENBPRESET_ADDR, (REG_IP_RD(ip_ENBPRESET_ADDR) & ~((uint32_t)0x001FFC00)) | ((uint32_t)twosc << 10));
}

__INLINE uint16_t ip_enbpreset_twrm_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ENBPRESET_ADDR);
    return ((localVal & ((uint32_t)0x000003FF)) >> 0);
}

__INLINE void ip_enbpreset_twrm_setf(uint16_t twrm)
{
    ASSERT_ERR((((uint32_t)twrm << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_IP_WR(ip_ENBPRESET_ADDR, (REG_IP_RD(ip_ENBPRESET_ADDR) & ~((uint32_t)0x000003FF)) | ((uint32_t)twrm << 0));
}

/**
 * @brief FINECNTCORR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  09:00          FINECNTCORR   0x0
 * </pre>
 */
#define ip_FINECNTCORR_ADDR   0x00820040
#define ip_FINECNTCORR_OFFSET 0x00000040
#define ip_FINECNTCORR_INDEX  0x00000010
#define ip_FINECNTCORR_RESET  0x00000000

__INLINE uint32_t ip_finecntcorr_get(void)
{
    return REG_IP_RD(ip_FINECNTCORR_ADDR);
}

__INLINE void ip_finecntcorr_set(uint32_t value)
{
    REG_IP_WR(ip_FINECNTCORR_ADDR, value);
}

// field definitions
#define ip_FINECNTCORR_MASK   ((uint32_t)0x000003FF)
#define ip_FINECNTCORR_LSB    0
#define ip_FINECNTCORR_WIDTH  ((uint32_t)0x0000000A)

#define ip_FINECNTCORR_RST    0x0

__INLINE uint16_t ip_finecntcorr_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_FINECNTCORR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_finecntcorr_setf(uint16_t finecntcorr)
{
    ASSERT_ERR((((uint32_t)finecntcorr << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_IP_WR(ip_FINECNTCORR_ADDR, (uint32_t)finecntcorr << 0);
}

/**
 * @brief CLKNCNTCORR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31            ABS_DELTA   0
 *  27:00          CLKNCNTCORR   0x0
 * </pre>
 */
#define ip_CLKNCNTCORR_ADDR   0x00820044
#define ip_CLKNCNTCORR_OFFSET 0x00000044
#define ip_CLKNCNTCORR_INDEX  0x00000011
#define ip_CLKNCNTCORR_RESET  0x00000000

__INLINE uint32_t ip_clkncntcorr_get(void)
{
    return REG_IP_RD(ip_CLKNCNTCORR_ADDR);
}

__INLINE void ip_clkncntcorr_set(uint32_t value)
{
    REG_IP_WR(ip_CLKNCNTCORR_ADDR, value);
}

// field definitions
#define ip_ABS_DELTA_BIT      ((uint32_t)0x80000000)
#define ip_ABS_DELTA_POS      31
#define ip_CLKNCNTCORR_MASK   ((uint32_t)0x0FFFFFFF)
#define ip_CLKNCNTCORR_LSB    0
#define ip_CLKNCNTCORR_WIDTH  ((uint32_t)0x0000001C)

#define ip_ABS_DELTA_RST      0x0
#define ip_CLKNCNTCORR_RST    0x0

__INLINE void ip_clkncntcorr_pack(uint8_t absdelta, uint32_t clkncntcorr)
{
    ASSERT_ERR((((uint32_t)absdelta << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)clkncntcorr << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_CLKNCNTCORR_ADDR,  ((uint32_t)absdelta << 31) | ((uint32_t)clkncntcorr << 0));
}

__INLINE void ip_clkncntcorr_unpack(uint8_t* absdelta, uint32_t* clkncntcorr)
{
    uint32_t localVal = REG_IP_RD(ip_CLKNCNTCORR_ADDR);

    *absdelta = (localVal & ((uint32_t)0x80000000)) >> 31;
    *clkncntcorr = (localVal & ((uint32_t)0x0FFFFFFF)) >> 0;
}

__INLINE uint8_t ip_clkncntcorr_abs_delta_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_CLKNCNTCORR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_clkncntcorr_abs_delta_setf(uint8_t absdelta)
{
    ASSERT_ERR((((uint32_t)absdelta << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_CLKNCNTCORR_ADDR, (REG_IP_RD(ip_CLKNCNTCORR_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)absdelta << 31));
}

__INLINE uint32_t ip_clkncntcorr_clkncntcorr_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_CLKNCNTCORR_ADDR);
    return ((localVal & ((uint32_t)0x0FFFFFFF)) >> 0);
}

__INLINE void ip_clkncntcorr_clkncntcorr_setf(uint32_t clkncntcorr)
{
    ASSERT_ERR((((uint32_t)clkncntcorr << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_CLKNCNTCORR_ADDR, (REG_IP_RD(ip_CLKNCNTCORR_ADDR) & ~((uint32_t)0x0FFFFFFF)) | ((uint32_t)clkncntcorr << 0));
}

/**
 * @brief DIAGCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31             DIAG3_EN   0
 *  29:24                DIAG3   0x0
 *     23             DIAG2_EN   0
 *  21:16                DIAG2   0x0
 *     15             DIAG1_EN   0
 *  13:08                DIAG1   0x0
 *     07             DIAG0_EN   0
 *  05:00                DIAG0   0x0
 * </pre>
 */
#define ip_DIAGCNTL_ADDR   0x00820050
#define ip_DIAGCNTL_OFFSET 0x00000050
#define ip_DIAGCNTL_INDEX  0x00000014
#define ip_DIAGCNTL_RESET  0x00000000

__INLINE uint32_t ip_diagcntl_get(void)
{
    return REG_IP_RD(ip_DIAGCNTL_ADDR);
}

__INLINE void ip_diagcntl_set(uint32_t value)
{
    REG_IP_WR(ip_DIAGCNTL_ADDR, value);
}

// field definitions
#define ip_DIAG3_EN_BIT    ((uint32_t)0x80000000)
#define ip_DIAG3_EN_POS    31
#define ip_DIAG3_MASK      ((uint32_t)0x3F000000)
#define ip_DIAG3_LSB       24
#define ip_DIAG3_WIDTH     ((uint32_t)0x00000006)
#define ip_DIAG2_EN_BIT    ((uint32_t)0x00800000)
#define ip_DIAG2_EN_POS    23
#define ip_DIAG2_MASK      ((uint32_t)0x003F0000)
#define ip_DIAG2_LSB       16
#define ip_DIAG2_WIDTH     ((uint32_t)0x00000006)
#define ip_DIAG1_EN_BIT    ((uint32_t)0x00008000)
#define ip_DIAG1_EN_POS    15
#define ip_DIAG1_MASK      ((uint32_t)0x00003F00)
#define ip_DIAG1_LSB       8
#define ip_DIAG1_WIDTH     ((uint32_t)0x00000006)
#define ip_DIAG0_EN_BIT    ((uint32_t)0x00000080)
#define ip_DIAG0_EN_POS    7
#define ip_DIAG0_MASK      ((uint32_t)0x0000003F)
#define ip_DIAG0_LSB       0
#define ip_DIAG0_WIDTH     ((uint32_t)0x00000006)

#define ip_DIAG3_EN_RST    0x0
#define ip_DIAG3_RST       0x0
#define ip_DIAG2_EN_RST    0x0
#define ip_DIAG2_RST       0x0
#define ip_DIAG1_EN_RST    0x0
#define ip_DIAG1_RST       0x0
#define ip_DIAG0_EN_RST    0x0
#define ip_DIAG0_RST       0x0

__INLINE void ip_diagcntl_pack(uint8_t diag3en, uint8_t diag3, uint8_t diag2en, uint8_t diag2, uint8_t diag1en, uint8_t diag1, uint8_t diag0en, uint8_t diag0)
{
    ASSERT_ERR((((uint32_t)diag3en << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)diag3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    ASSERT_ERR((((uint32_t)diag2en << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)diag2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    ASSERT_ERR((((uint32_t)diag1en << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)diag1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint32_t)diag0en << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)diag0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR,  ((uint32_t)diag3en << 31) | ((uint32_t)diag3 << 24) | ((uint32_t)diag2en << 23) | ((uint32_t)diag2 << 16) | ((uint32_t)diag1en << 15) | ((uint32_t)diag1 << 8) | ((uint32_t)diag0en << 7) | ((uint32_t)diag0 << 0));
}

__INLINE void ip_diagcntl_unpack(uint8_t* diag3en, uint8_t* diag3, uint8_t* diag2en, uint8_t* diag2, uint8_t* diag1en, uint8_t* diag1, uint8_t* diag0en, uint8_t* diag0)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);

    *diag3en = (localVal & ((uint32_t)0x80000000)) >> 31;
    *diag3 = (localVal & ((uint32_t)0x3F000000)) >> 24;
    *diag2en = (localVal & ((uint32_t)0x00800000)) >> 23;
    *diag2 = (localVal & ((uint32_t)0x003F0000)) >> 16;
    *diag1en = (localVal & ((uint32_t)0x00008000)) >> 15;
    *diag1 = (localVal & ((uint32_t)0x00003F00)) >> 8;
    *diag0en = (localVal & ((uint32_t)0x00000080)) >> 7;
    *diag0 = (localVal & ((uint32_t)0x0000003F)) >> 0;
}

__INLINE uint8_t ip_diagcntl_diag3_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_diagcntl_diag3_en_setf(uint8_t diag3en)
{
    ASSERT_ERR((((uint32_t)diag3en << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)diag3en << 31));
}

__INLINE uint8_t ip_diagcntl_diag3_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x3F000000)) >> 24);
}

__INLINE void ip_diagcntl_diag3_setf(uint8_t diag3)
{
    ASSERT_ERR((((uint32_t)diag3 << 24) & ~((uint32_t)0x3F000000)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x3F000000)) | ((uint32_t)diag3 << 24));
}

__INLINE uint8_t ip_diagcntl_diag2_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

__INLINE void ip_diagcntl_diag2_en_setf(uint8_t diag2en)
{
    ASSERT_ERR((((uint32_t)diag2en << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)diag2en << 23));
}

__INLINE uint8_t ip_diagcntl_diag2_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x003F0000)) >> 16);
}

__INLINE void ip_diagcntl_diag2_setf(uint8_t diag2)
{
    ASSERT_ERR((((uint32_t)diag2 << 16) & ~((uint32_t)0x003F0000)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x003F0000)) | ((uint32_t)diag2 << 16));
}

__INLINE uint8_t ip_diagcntl_diag1_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE void ip_diagcntl_diag1_en_setf(uint8_t diag1en)
{
    ASSERT_ERR((((uint32_t)diag1en << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)diag1en << 15));
}

__INLINE uint8_t ip_diagcntl_diag1_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00003F00)) >> 8);
}

__INLINE void ip_diagcntl_diag1_setf(uint8_t diag1)
{
    ASSERT_ERR((((uint32_t)diag1 << 8) & ~((uint32_t)0x00003F00)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x00003F00)) | ((uint32_t)diag1 << 8));
}

__INLINE uint8_t ip_diagcntl_diag0_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

__INLINE void ip_diagcntl_diag0_en_setf(uint8_t diag0en)
{
    ASSERT_ERR((((uint32_t)diag0en << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)diag0en << 7));
}

__INLINE uint8_t ip_diagcntl_diag0_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000003F)) >> 0);
}

__INLINE void ip_diagcntl_diag0_setf(uint8_t diag0)
{
    ASSERT_ERR((((uint32_t)diag0 << 0) & ~((uint32_t)0x0000003F)) == 0);
    REG_IP_WR(ip_DIAGCNTL_ADDR, (REG_IP_RD(ip_DIAGCNTL_ADDR) & ~((uint32_t)0x0000003F)) | ((uint32_t)diag0 << 0));
}

/**
 * @brief DIAGSTAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:24            DIAG3STAT   0x0
 *  23:16            DIAG2STAT   0x0
 *  15:08            DIAG1STAT   0x0
 *  07:00            DIAG0STAT   0x0
 * </pre>
 */
#define ip_DIAGSTAT_ADDR   0x00820054
#define ip_DIAGSTAT_OFFSET 0x00000054
#define ip_DIAGSTAT_INDEX  0x00000015
#define ip_DIAGSTAT_RESET  0x00000000

__INLINE uint32_t ip_diagstat_get(void)
{
    return REG_IP_RD(ip_DIAGSTAT_ADDR);
}

// field definitions
#define ip_DIAG3STAT_MASK   ((uint32_t)0xFF000000)
#define ip_DIAG3STAT_LSB    24
#define ip_DIAG3STAT_WIDTH  ((uint32_t)0x00000008)
#define ip_DIAG2STAT_MASK   ((uint32_t)0x00FF0000)
#define ip_DIAG2STAT_LSB    16
#define ip_DIAG2STAT_WIDTH  ((uint32_t)0x00000008)
#define ip_DIAG1STAT_MASK   ((uint32_t)0x0000FF00)
#define ip_DIAG1STAT_LSB    8
#define ip_DIAG1STAT_WIDTH  ((uint32_t)0x00000008)
#define ip_DIAG0STAT_MASK   ((uint32_t)0x000000FF)
#define ip_DIAG0STAT_LSB    0
#define ip_DIAG0STAT_WIDTH  ((uint32_t)0x00000008)

#define ip_DIAG3STAT_RST    0x0
#define ip_DIAG2STAT_RST    0x0
#define ip_DIAG1STAT_RST    0x0
#define ip_DIAG0STAT_RST    0x0

__INLINE void ip_diagstat_unpack(uint8_t* diag3stat, uint8_t* diag2stat, uint8_t* diag1stat, uint8_t* diag0stat)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGSTAT_ADDR);

    *diag3stat = (localVal & ((uint32_t)0xFF000000)) >> 24;
    *diag2stat = (localVal & ((uint32_t)0x00FF0000)) >> 16;
    *diag1stat = (localVal & ((uint32_t)0x0000FF00)) >> 8;
    *diag0stat = (localVal & ((uint32_t)0x000000FF)) >> 0;
}

__INLINE uint8_t ip_diagstat_diag3stat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGSTAT_ADDR);
    return ((localVal & ((uint32_t)0xFF000000)) >> 24);
}

__INLINE uint8_t ip_diagstat_diag2stat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGSTAT_ADDR);
    return ((localVal & ((uint32_t)0x00FF0000)) >> 16);
}

__INLINE uint8_t ip_diagstat_diag1stat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGSTAT_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}

__INLINE uint8_t ip_diagstat_diag0stat_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DIAGSTAT_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

/**
 * @brief DEBUGADDMAX register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16           REG_ADDMAX   0x0
 *  15:00            EM_ADDMAX   0x0
 * </pre>
 */
#define ip_DEBUGADDMAX_ADDR   0x00820058
#define ip_DEBUGADDMAX_OFFSET 0x00000058
#define ip_DEBUGADDMAX_INDEX  0x00000016
#define ip_DEBUGADDMAX_RESET  0x00000000

__INLINE uint32_t ip_debugaddmax_get(void)
{
    return REG_IP_RD(ip_DEBUGADDMAX_ADDR);
}

__INLINE void ip_debugaddmax_set(uint32_t value)
{
    REG_IP_WR(ip_DEBUGADDMAX_ADDR, value);
}

// field definitions
#define ip_REG_ADDMAX_MASK   ((uint32_t)0xFFFF0000)
#define ip_REG_ADDMAX_LSB    16
#define ip_REG_ADDMAX_WIDTH  ((uint32_t)0x00000010)
#define ip_EM_ADDMAX_MASK    ((uint32_t)0x0000FFFF)
#define ip_EM_ADDMAX_LSB     0
#define ip_EM_ADDMAX_WIDTH   ((uint32_t)0x00000010)

#define ip_REG_ADDMAX_RST    0x0
#define ip_EM_ADDMAX_RST     0x0

__INLINE void ip_debugaddmax_pack(uint16_t regaddmax, uint16_t emaddmax)
{
    ASSERT_ERR((((uint32_t)regaddmax << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)emaddmax << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_IP_WR(ip_DEBUGADDMAX_ADDR,  ((uint32_t)regaddmax << 16) | ((uint32_t)emaddmax << 0));
}

__INLINE void ip_debugaddmax_unpack(uint16_t* regaddmax, uint16_t* emaddmax)
{
    uint32_t localVal = REG_IP_RD(ip_DEBUGADDMAX_ADDR);

    *regaddmax = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *emaddmax = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

__INLINE uint16_t ip_debugaddmax_reg_addmax_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEBUGADDMAX_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

__INLINE void ip_debugaddmax_reg_addmax_setf(uint16_t regaddmax)
{
    ASSERT_ERR((((uint32_t)regaddmax << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_IP_WR(ip_DEBUGADDMAX_ADDR, (REG_IP_RD(ip_DEBUGADDMAX_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)regaddmax << 16));
}

__INLINE uint16_t ip_debugaddmax_em_addmax_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEBUGADDMAX_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

__INLINE void ip_debugaddmax_em_addmax_setf(uint16_t emaddmax)
{
    ASSERT_ERR((((uint32_t)emaddmax << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_IP_WR(ip_DEBUGADDMAX_ADDR, (REG_IP_RD(ip_DEBUGADDMAX_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)emaddmax << 0));
}

/**
 * @brief DEBUGADDMIN register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:16           REG_ADDMIN   0x0
 *  15:00            EM_ADDMIN   0x0
 * </pre>
 */
#define ip_DEBUGADDMIN_ADDR   0x0082005C
#define ip_DEBUGADDMIN_OFFSET 0x0000005C
#define ip_DEBUGADDMIN_INDEX  0x00000017
#define ip_DEBUGADDMIN_RESET  0x00000000

__INLINE uint32_t ip_debugaddmin_get(void)
{
    return REG_IP_RD(ip_DEBUGADDMIN_ADDR);
}

__INLINE void ip_debugaddmin_set(uint32_t value)
{
    REG_IP_WR(ip_DEBUGADDMIN_ADDR, value);
}

// field definitions
#define ip_REG_ADDMIN_MASK   ((uint32_t)0xFFFF0000)
#define ip_REG_ADDMIN_LSB    16
#define ip_REG_ADDMIN_WIDTH  ((uint32_t)0x00000010)
#define ip_EM_ADDMIN_MASK    ((uint32_t)0x0000FFFF)
#define ip_EM_ADDMIN_LSB     0
#define ip_EM_ADDMIN_WIDTH   ((uint32_t)0x00000010)

#define ip_REG_ADDMIN_RST    0x0
#define ip_EM_ADDMIN_RST     0x0

__INLINE void ip_debugaddmin_pack(uint16_t regaddmin, uint16_t emaddmin)
{
    ASSERT_ERR((((uint32_t)regaddmin << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    ASSERT_ERR((((uint32_t)emaddmin << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_IP_WR(ip_DEBUGADDMIN_ADDR,  ((uint32_t)regaddmin << 16) | ((uint32_t)emaddmin << 0));
}

__INLINE void ip_debugaddmin_unpack(uint16_t* regaddmin, uint16_t* emaddmin)
{
    uint32_t localVal = REG_IP_RD(ip_DEBUGADDMIN_ADDR);

    *regaddmin = (localVal & ((uint32_t)0xFFFF0000)) >> 16;
    *emaddmin = (localVal & ((uint32_t)0x0000FFFF)) >> 0;
}

__INLINE uint16_t ip_debugaddmin_reg_addmin_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEBUGADDMIN_ADDR);
    return ((localVal & ((uint32_t)0xFFFF0000)) >> 16);
}

__INLINE void ip_debugaddmin_reg_addmin_setf(uint16_t regaddmin)
{
    ASSERT_ERR((((uint32_t)regaddmin << 16) & ~((uint32_t)0xFFFF0000)) == 0);
    REG_IP_WR(ip_DEBUGADDMIN_ADDR, (REG_IP_RD(ip_DEBUGADDMIN_ADDR) & ~((uint32_t)0xFFFF0000)) | ((uint32_t)regaddmin << 16));
}

__INLINE uint16_t ip_debugaddmin_em_addmin_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DEBUGADDMIN_ADDR);
    return ((localVal & ((uint32_t)0x0000FFFF)) >> 0);
}

__INLINE void ip_debugaddmin_em_addmin_setf(uint16_t emaddmin)
{
    ASSERT_ERR((((uint32_t)emaddmin << 0) & ~((uint32_t)0x0000FFFF)) == 0);
    REG_IP_WR(ip_DEBUGADDMIN_ADDR, (REG_IP_RD(ip_DEBUGADDMIN_ADDR) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)emaddmin << 0));
}

/**
 * @brief ERRORTYPESTAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     03   ACT_SCHDL_APFM_ERROR   0
 *     02   ACT_SCHDL_ENTRY_ERROR   0
 *     01         FIFOWRITEERR   0
 *     00    RADIO_EMACC_ERROR   0
 * </pre>
 */
#define ip_ERRORTYPESTAT_ADDR   0x00820060
#define ip_ERRORTYPESTAT_OFFSET 0x00000060
#define ip_ERRORTYPESTAT_INDEX  0x00000018
#define ip_ERRORTYPESTAT_RESET  0x00000000

__INLINE uint32_t ip_errortypestat_get(void)
{
    return REG_IP_RD(ip_ERRORTYPESTAT_ADDR);
}

// field definitions
#define ip_ACT_SCHDL_APFM_ERROR_BIT     ((uint32_t)0x00000008)
#define ip_ACT_SCHDL_APFM_ERROR_POS     3
#define ip_ACT_SCHDL_ENTRY_ERROR_BIT    ((uint32_t)0x00000004)
#define ip_ACT_SCHDL_ENTRY_ERROR_POS    2
#define ip_FIFOWRITEERR_BIT             ((uint32_t)0x00000002)
#define ip_FIFOWRITEERR_POS             1
#define ip_RADIO_EMACC_ERROR_BIT        ((uint32_t)0x00000001)
#define ip_RADIO_EMACC_ERROR_POS        0

#define ip_ACT_SCHDL_APFM_ERROR_RST     0x0
#define ip_ACT_SCHDL_ENTRY_ERROR_RST    0x0
#define ip_FIFOWRITEERR_RST             0x0
#define ip_RADIO_EMACC_ERROR_RST        0x0

__INLINE void ip_errortypestat_unpack(uint8_t* actschdlapfmerror, uint8_t* actschdlentryerror, uint8_t* fifowriteerr, uint8_t* radioemaccerror)
{
    uint32_t localVal = REG_IP_RD(ip_ERRORTYPESTAT_ADDR);

    *actschdlapfmerror = (localVal & ((uint32_t)0x00000008)) >> 3;
    *actschdlentryerror = (localVal & ((uint32_t)0x00000004)) >> 2;
    *fifowriteerr = (localVal & ((uint32_t)0x00000002)) >> 1;
    *radioemaccerror = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_errortypestat_act_schdl_apfm_error_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ERRORTYPESTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE uint8_t ip_errortypestat_act_schdl_entry_error_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ERRORTYPESTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE uint8_t ip_errortypestat_fifowriteerr_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ERRORTYPESTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE uint8_t ip_errortypestat_radio_emacc_error_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ERRORTYPESTAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

/**
 * @brief SWPROFILING register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31             SWPROF31   0
 *     30             SWPROF30   0
 *     29             SWPROF29   0
 *     28             SWPROF28   0
 *     27             SWPROF27   0
 *     26             SWPROF26   0
 *     25             SWPROF25   0
 *     24             SWPROF24   0
 *     23             SWPROF23   0
 *     22             SWPROF22   0
 *     21             SWPROF21   0
 *     20             SWPROF20   0
 *     19             SWPROF19   0
 *     18             SWPROF18   0
 *     17             SWPROF17   0
 *     16             SWPROF16   0
 *     15             SWPROF15   0
 *     14             SWPROF14   0
 *     13             SWPROF13   0
 *     12             SWPROF12   0
 *     11             SWPROF11   0
 *     10             SWPROF10   0
 *     09              SWPROF9   0
 *     08              SWPROF8   0
 *     07              SWPROF7   0
 *     06              SWPROF6   0
 *     05              SWPROF5   0
 *     04              SWPROF4   0
 *     03              SWPROF3   0
 *     02              SWPROF2   0
 *     01              SWPROF1   0
 *     00              SWPROF0   0
 * </pre>
 */
#define ip_SWPROFILING_ADDR   0x00820064
#define ip_SWPROFILING_OFFSET 0x00000064
#define ip_SWPROFILING_INDEX  0x00000019
#define ip_SWPROFILING_RESET  0x00000000

__INLINE uint32_t ip_swprofiling_get(void)
{
    return REG_IP_RD(ip_SWPROFILING_ADDR);
}

__INLINE void ip_swprofiling_set(uint32_t value)
{
    REG_IP_WR(ip_SWPROFILING_ADDR, value);
}

// field definitions
#define ip_SWPROF31_BIT    ((uint32_t)0x80000000)
#define ip_SWPROF31_POS    31
#define ip_SWPROF30_BIT    ((uint32_t)0x40000000)
#define ip_SWPROF30_POS    30
#define ip_SWPROF29_BIT    ((uint32_t)0x20000000)
#define ip_SWPROF29_POS    29
#define ip_SWPROF28_BIT    ((uint32_t)0x10000000)
#define ip_SWPROF28_POS    28
#define ip_SWPROF27_BIT    ((uint32_t)0x08000000)
#define ip_SWPROF27_POS    27
#define ip_SWPROF26_BIT    ((uint32_t)0x04000000)
#define ip_SWPROF26_POS    26
#define ip_SWPROF25_BIT    ((uint32_t)0x02000000)
#define ip_SWPROF25_POS    25
#define ip_SWPROF24_BIT    ((uint32_t)0x01000000)
#define ip_SWPROF24_POS    24
#define ip_SWPROF23_BIT    ((uint32_t)0x00800000)
#define ip_SWPROF23_POS    23
#define ip_SWPROF22_BIT    ((uint32_t)0x00400000)
#define ip_SWPROF22_POS    22
#define ip_SWPROF21_BIT    ((uint32_t)0x00200000)
#define ip_SWPROF21_POS    21
#define ip_SWPROF20_BIT    ((uint32_t)0x00100000)
#define ip_SWPROF20_POS    20
#define ip_SWPROF19_BIT    ((uint32_t)0x00080000)
#define ip_SWPROF19_POS    19
#define ip_SWPROF18_BIT    ((uint32_t)0x00040000)
#define ip_SWPROF18_POS    18
#define ip_SWPROF17_BIT    ((uint32_t)0x00020000)
#define ip_SWPROF17_POS    17
#define ip_SWPROF16_BIT    ((uint32_t)0x00010000)
#define ip_SWPROF16_POS    16
#define ip_SWPROF15_BIT    ((uint32_t)0x00008000)
#define ip_SWPROF15_POS    15
#define ip_SWPROF14_BIT    ((uint32_t)0x00004000)
#define ip_SWPROF14_POS    14
#define ip_SWPROF13_BIT    ((uint32_t)0x00002000)
#define ip_SWPROF13_POS    13
#define ip_SWPROF12_BIT    ((uint32_t)0x00001000)
#define ip_SWPROF12_POS    12
#define ip_SWPROF11_BIT    ((uint32_t)0x00000800)
#define ip_SWPROF11_POS    11
#define ip_SWPROF10_BIT    ((uint32_t)0x00000400)
#define ip_SWPROF10_POS    10
#define ip_SWPROF9_BIT     ((uint32_t)0x00000200)
#define ip_SWPROF9_POS     9
#define ip_SWPROF8_BIT     ((uint32_t)0x00000100)
#define ip_SWPROF8_POS     8
#define ip_SWPROF7_BIT     ((uint32_t)0x00000080)
#define ip_SWPROF7_POS     7
#define ip_SWPROF6_BIT     ((uint32_t)0x00000040)
#define ip_SWPROF6_POS     6
#define ip_SWPROF5_BIT     ((uint32_t)0x00000020)
#define ip_SWPROF5_POS     5
#define ip_SWPROF4_BIT     ((uint32_t)0x00000010)
#define ip_SWPROF4_POS     4
#define ip_SWPROF3_BIT     ((uint32_t)0x00000008)
#define ip_SWPROF3_POS     3
#define ip_SWPROF2_BIT     ((uint32_t)0x00000004)
#define ip_SWPROF2_POS     2
#define ip_SWPROF1_BIT     ((uint32_t)0x00000002)
#define ip_SWPROF1_POS     1
#define ip_SWPROF0_BIT     ((uint32_t)0x00000001)
#define ip_SWPROF0_POS     0

#define ip_SWPROF31_RST    0x0
#define ip_SWPROF30_RST    0x0
#define ip_SWPROF29_RST    0x0
#define ip_SWPROF28_RST    0x0
#define ip_SWPROF27_RST    0x0
#define ip_SWPROF26_RST    0x0
#define ip_SWPROF25_RST    0x0
#define ip_SWPROF24_RST    0x0
#define ip_SWPROF23_RST    0x0
#define ip_SWPROF22_RST    0x0
#define ip_SWPROF21_RST    0x0
#define ip_SWPROF20_RST    0x0
#define ip_SWPROF19_RST    0x0
#define ip_SWPROF18_RST    0x0
#define ip_SWPROF17_RST    0x0
#define ip_SWPROF16_RST    0x0
#define ip_SWPROF15_RST    0x0
#define ip_SWPROF14_RST    0x0
#define ip_SWPROF13_RST    0x0
#define ip_SWPROF12_RST    0x0
#define ip_SWPROF11_RST    0x0
#define ip_SWPROF10_RST    0x0
#define ip_SWPROF9_RST     0x0
#define ip_SWPROF8_RST     0x0
#define ip_SWPROF7_RST     0x0
#define ip_SWPROF6_RST     0x0
#define ip_SWPROF5_RST     0x0
#define ip_SWPROF4_RST     0x0
#define ip_SWPROF3_RST     0x0
#define ip_SWPROF2_RST     0x0
#define ip_SWPROF1_RST     0x0
#define ip_SWPROF0_RST     0x0

__INLINE void ip_swprofiling_pack(uint8_t swprof31, uint8_t swprof30, uint8_t swprof29, uint8_t swprof28, uint8_t swprof27, uint8_t swprof26, uint8_t swprof25, uint8_t swprof24, uint8_t swprof23, uint8_t swprof22, uint8_t swprof21, uint8_t swprof20, uint8_t swprof19, uint8_t swprof18, uint8_t swprof17, uint8_t swprof16, uint8_t swprof15, uint8_t swprof14, uint8_t swprof13, uint8_t swprof12, uint8_t swprof11, uint8_t swprof10, uint8_t swprof9, uint8_t swprof8, uint8_t swprof7, uint8_t swprof6, uint8_t swprof5, uint8_t swprof4, uint8_t swprof3, uint8_t swprof2, uint8_t swprof1, uint8_t swprof0)
{
    ASSERT_ERR((((uint32_t)swprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    ASSERT_ERR((((uint32_t)swprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)swprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    ASSERT_ERR((((uint32_t)swprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    ASSERT_ERR((((uint32_t)swprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    ASSERT_ERR((((uint32_t)swprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    ASSERT_ERR((((uint32_t)swprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    ASSERT_ERR((((uint32_t)swprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    ASSERT_ERR((((uint32_t)swprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    ASSERT_ERR((((uint32_t)swprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)swprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)swprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)swprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)swprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    ASSERT_ERR((((uint32_t)swprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    ASSERT_ERR((((uint32_t)swprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    ASSERT_ERR((((uint32_t)swprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    ASSERT_ERR((((uint32_t)swprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)swprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)swprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)swprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    ASSERT_ERR((((uint32_t)swprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    ASSERT_ERR((((uint32_t)swprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)swprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)swprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR,  ((uint32_t)swprof31 << 31) | ((uint32_t)swprof30 << 30) | ((uint32_t)swprof29 << 29) | ((uint32_t)swprof28 << 28) | ((uint32_t)swprof27 << 27) | ((uint32_t)swprof26 << 26) | ((uint32_t)swprof25 << 25) | ((uint32_t)swprof24 << 24) | ((uint32_t)swprof23 << 23) | ((uint32_t)swprof22 << 22) | ((uint32_t)swprof21 << 21) | ((uint32_t)swprof20 << 20) | ((uint32_t)swprof19 << 19) | ((uint32_t)swprof18 << 18) | ((uint32_t)swprof17 << 17) | ((uint32_t)swprof16 << 16) | ((uint32_t)swprof15 << 15) | ((uint32_t)swprof14 << 14) | ((uint32_t)swprof13 << 13) | ((uint32_t)swprof12 << 12) | ((uint32_t)swprof11 << 11) | ((uint32_t)swprof10 << 10) | ((uint32_t)swprof9 << 9) | ((uint32_t)swprof8 << 8) | ((uint32_t)swprof7 << 7) | ((uint32_t)swprof6 << 6) | ((uint32_t)swprof5 << 5) | ((uint32_t)swprof4 << 4) | ((uint32_t)swprof3 << 3) | ((uint32_t)swprof2 << 2) | ((uint32_t)swprof1 << 1) | ((uint32_t)swprof0 << 0));
}

__INLINE void ip_swprofiling_unpack(uint8_t* swprof31, uint8_t* swprof30, uint8_t* swprof29, uint8_t* swprof28, uint8_t* swprof27, uint8_t* swprof26, uint8_t* swprof25, uint8_t* swprof24, uint8_t* swprof23, uint8_t* swprof22, uint8_t* swprof21, uint8_t* swprof20, uint8_t* swprof19, uint8_t* swprof18, uint8_t* swprof17, uint8_t* swprof16, uint8_t* swprof15, uint8_t* swprof14, uint8_t* swprof13, uint8_t* swprof12, uint8_t* swprof11, uint8_t* swprof10, uint8_t* swprof9, uint8_t* swprof8, uint8_t* swprof7, uint8_t* swprof6, uint8_t* swprof5, uint8_t* swprof4, uint8_t* swprof3, uint8_t* swprof2, uint8_t* swprof1, uint8_t* swprof0)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);

    *swprof31 = (localVal & ((uint32_t)0x80000000)) >> 31;
    *swprof30 = (localVal & ((uint32_t)0x40000000)) >> 30;
    *swprof29 = (localVal & ((uint32_t)0x20000000)) >> 29;
    *swprof28 = (localVal & ((uint32_t)0x10000000)) >> 28;
    *swprof27 = (localVal & ((uint32_t)0x08000000)) >> 27;
    *swprof26 = (localVal & ((uint32_t)0x04000000)) >> 26;
    *swprof25 = (localVal & ((uint32_t)0x02000000)) >> 25;
    *swprof24 = (localVal & ((uint32_t)0x01000000)) >> 24;
    *swprof23 = (localVal & ((uint32_t)0x00800000)) >> 23;
    *swprof22 = (localVal & ((uint32_t)0x00400000)) >> 22;
    *swprof21 = (localVal & ((uint32_t)0x00200000)) >> 21;
    *swprof20 = (localVal & ((uint32_t)0x00100000)) >> 20;
    *swprof19 = (localVal & ((uint32_t)0x00080000)) >> 19;
    *swprof18 = (localVal & ((uint32_t)0x00040000)) >> 18;
    *swprof17 = (localVal & ((uint32_t)0x00020000)) >> 17;
    *swprof16 = (localVal & ((uint32_t)0x00010000)) >> 16;
    *swprof15 = (localVal & ((uint32_t)0x00008000)) >> 15;
    *swprof14 = (localVal & ((uint32_t)0x00004000)) >> 14;
    *swprof13 = (localVal & ((uint32_t)0x00002000)) >> 13;
    *swprof12 = (localVal & ((uint32_t)0x00001000)) >> 12;
    *swprof11 = (localVal & ((uint32_t)0x00000800)) >> 11;
    *swprof10 = (localVal & ((uint32_t)0x00000400)) >> 10;
    *swprof9 = (localVal & ((uint32_t)0x00000200)) >> 9;
    *swprof8 = (localVal & ((uint32_t)0x00000100)) >> 8;
    *swprof7 = (localVal & ((uint32_t)0x00000080)) >> 7;
    *swprof6 = (localVal & ((uint32_t)0x00000040)) >> 6;
    *swprof5 = (localVal & ((uint32_t)0x00000020)) >> 5;
    *swprof4 = (localVal & ((uint32_t)0x00000010)) >> 4;
    *swprof3 = (localVal & ((uint32_t)0x00000008)) >> 3;
    *swprof2 = (localVal & ((uint32_t)0x00000004)) >> 2;
    *swprof1 = (localVal & ((uint32_t)0x00000002)) >> 1;
    *swprof0 = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_swprofiling_swprof31_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_swprofiling_swprof31_setf(uint8_t swprof31)
{
    ASSERT_ERR((((uint32_t)swprof31 << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)swprof31 << 31));
}

__INLINE uint8_t ip_swprofiling_swprof30_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void ip_swprofiling_swprof30_setf(uint8_t swprof30)
{
    ASSERT_ERR((((uint32_t)swprof30 << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)swprof30 << 30));
}

__INLINE uint8_t ip_swprofiling_swprof29_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

__INLINE void ip_swprofiling_swprof29_setf(uint8_t swprof29)
{
    ASSERT_ERR((((uint32_t)swprof29 << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)swprof29 << 29));
}

__INLINE uint8_t ip_swprofiling_swprof28_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

__INLINE void ip_swprofiling_swprof28_setf(uint8_t swprof28)
{
    ASSERT_ERR((((uint32_t)swprof28 << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)swprof28 << 28));
}

__INLINE uint8_t ip_swprofiling_swprof27_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

__INLINE void ip_swprofiling_swprof27_setf(uint8_t swprof27)
{
    ASSERT_ERR((((uint32_t)swprof27 << 27) & ~((uint32_t)0x08000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)swprof27 << 27));
}

__INLINE uint8_t ip_swprofiling_swprof26_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

__INLINE void ip_swprofiling_swprof26_setf(uint8_t swprof26)
{
    ASSERT_ERR((((uint32_t)swprof26 << 26) & ~((uint32_t)0x04000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x04000000)) | ((uint32_t)swprof26 << 26));
}

__INLINE uint8_t ip_swprofiling_swprof25_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

__INLINE void ip_swprofiling_swprof25_setf(uint8_t swprof25)
{
    ASSERT_ERR((((uint32_t)swprof25 << 25) & ~((uint32_t)0x02000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x02000000)) | ((uint32_t)swprof25 << 25));
}

__INLINE uint8_t ip_swprofiling_swprof24_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

__INLINE void ip_swprofiling_swprof24_setf(uint8_t swprof24)
{
    ASSERT_ERR((((uint32_t)swprof24 << 24) & ~((uint32_t)0x01000000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x01000000)) | ((uint32_t)swprof24 << 24));
}

__INLINE uint8_t ip_swprofiling_swprof23_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

__INLINE void ip_swprofiling_swprof23_setf(uint8_t swprof23)
{
    ASSERT_ERR((((uint32_t)swprof23 << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)swprof23 << 23));
}

__INLINE uint8_t ip_swprofiling_swprof22_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

__INLINE void ip_swprofiling_swprof22_setf(uint8_t swprof22)
{
    ASSERT_ERR((((uint32_t)swprof22 << 22) & ~((uint32_t)0x00400000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00400000)) | ((uint32_t)swprof22 << 22));
}

__INLINE uint8_t ip_swprofiling_swprof21_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

__INLINE void ip_swprofiling_swprof21_setf(uint8_t swprof21)
{
    ASSERT_ERR((((uint32_t)swprof21 << 21) & ~((uint32_t)0x00200000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00200000)) | ((uint32_t)swprof21 << 21));
}

__INLINE uint8_t ip_swprofiling_swprof20_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

__INLINE void ip_swprofiling_swprof20_setf(uint8_t swprof20)
{
    ASSERT_ERR((((uint32_t)swprof20 << 20) & ~((uint32_t)0x00100000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00100000)) | ((uint32_t)swprof20 << 20));
}

__INLINE uint8_t ip_swprofiling_swprof19_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

__INLINE void ip_swprofiling_swprof19_setf(uint8_t swprof19)
{
    ASSERT_ERR((((uint32_t)swprof19 << 19) & ~((uint32_t)0x00080000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00080000)) | ((uint32_t)swprof19 << 19));
}

__INLINE uint8_t ip_swprofiling_swprof18_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

__INLINE void ip_swprofiling_swprof18_setf(uint8_t swprof18)
{
    ASSERT_ERR((((uint32_t)swprof18 << 18) & ~((uint32_t)0x00040000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00040000)) | ((uint32_t)swprof18 << 18));
}

__INLINE uint8_t ip_swprofiling_swprof17_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

__INLINE void ip_swprofiling_swprof17_setf(uint8_t swprof17)
{
    ASSERT_ERR((((uint32_t)swprof17 << 17) & ~((uint32_t)0x00020000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)swprof17 << 17));
}

__INLINE uint8_t ip_swprofiling_swprof16_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

__INLINE void ip_swprofiling_swprof16_setf(uint8_t swprof16)
{
    ASSERT_ERR((((uint32_t)swprof16 << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)swprof16 << 16));
}

__INLINE uint8_t ip_swprofiling_swprof15_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE void ip_swprofiling_swprof15_setf(uint8_t swprof15)
{
    ASSERT_ERR((((uint32_t)swprof15 << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)swprof15 << 15));
}

__INLINE uint8_t ip_swprofiling_swprof14_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

__INLINE void ip_swprofiling_swprof14_setf(uint8_t swprof14)
{
    ASSERT_ERR((((uint32_t)swprof14 << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)swprof14 << 14));
}

__INLINE uint8_t ip_swprofiling_swprof13_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

__INLINE void ip_swprofiling_swprof13_setf(uint8_t swprof13)
{
    ASSERT_ERR((((uint32_t)swprof13 << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)swprof13 << 13));
}

__INLINE uint8_t ip_swprofiling_swprof12_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

__INLINE void ip_swprofiling_swprof12_setf(uint8_t swprof12)
{
    ASSERT_ERR((((uint32_t)swprof12 << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)swprof12 << 12));
}

__INLINE uint8_t ip_swprofiling_swprof11_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

__INLINE void ip_swprofiling_swprof11_setf(uint8_t swprof11)
{
    ASSERT_ERR((((uint32_t)swprof11 << 11) & ~((uint32_t)0x00000800)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000800)) | ((uint32_t)swprof11 << 11));
}

__INLINE uint8_t ip_swprofiling_swprof10_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

__INLINE void ip_swprofiling_swprof10_setf(uint8_t swprof10)
{
    ASSERT_ERR((((uint32_t)swprof10 << 10) & ~((uint32_t)0x00000400)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000400)) | ((uint32_t)swprof10 << 10));
}

__INLINE uint8_t ip_swprofiling_swprof9_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

__INLINE void ip_swprofiling_swprof9_setf(uint8_t swprof9)
{
    ASSERT_ERR((((uint32_t)swprof9 << 9) & ~((uint32_t)0x00000200)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000200)) | ((uint32_t)swprof9 << 9));
}

__INLINE uint8_t ip_swprofiling_swprof8_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

__INLINE void ip_swprofiling_swprof8_setf(uint8_t swprof8)
{
    ASSERT_ERR((((uint32_t)swprof8 << 8) & ~((uint32_t)0x00000100)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000100)) | ((uint32_t)swprof8 << 8));
}

__INLINE uint8_t ip_swprofiling_swprof7_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

__INLINE void ip_swprofiling_swprof7_setf(uint8_t swprof7)
{
    ASSERT_ERR((((uint32_t)swprof7 << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)swprof7 << 7));
}

__INLINE uint8_t ip_swprofiling_swprof6_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

__INLINE void ip_swprofiling_swprof6_setf(uint8_t swprof6)
{
    ASSERT_ERR((((uint32_t)swprof6 << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)swprof6 << 6));
}

__INLINE uint8_t ip_swprofiling_swprof5_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE void ip_swprofiling_swprof5_setf(uint8_t swprof5)
{
    ASSERT_ERR((((uint32_t)swprof5 << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)swprof5 << 5));
}

__INLINE uint8_t ip_swprofiling_swprof4_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE void ip_swprofiling_swprof4_setf(uint8_t swprof4)
{
    ASSERT_ERR((((uint32_t)swprof4 << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)swprof4 << 4));
}

__INLINE uint8_t ip_swprofiling_swprof3_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void ip_swprofiling_swprof3_setf(uint8_t swprof3)
{
    ASSERT_ERR((((uint32_t)swprof3 << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)swprof3 << 3));
}

__INLINE uint8_t ip_swprofiling_swprof2_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void ip_swprofiling_swprof2_setf(uint8_t swprof2)
{
    ASSERT_ERR((((uint32_t)swprof2 << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)swprof2 << 2));
}

__INLINE uint8_t ip_swprofiling_swprof1_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void ip_swprofiling_swprof1_setf(uint8_t swprof1)
{
    ASSERT_ERR((((uint32_t)swprof1 << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)swprof1 << 1));
}

__INLINE uint8_t ip_swprofiling_swprof0_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SWPROFILING_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void ip_swprofiling_swprof0_setf(uint8_t swprof0)
{
    ASSERT_ERR((((uint32_t)swprof0 << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_SWPROFILING_ADDR, (REG_IP_RD(ip_SWPROFILING_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)swprof0 << 0));
}

/**
 * @brief RADIOCNTL0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  29:16               SPIPTR   0x0
 *     07               SPICFG   0
 *  05:04              SPIFREQ   0x0
 *     01              SPICOMP   1
 *     00                SPIGO   0
 * </pre>
 */
#define ip_RADIOCNTL0_ADDR   0x00820070
#define ip_RADIOCNTL0_OFFSET 0x00000070
#define ip_RADIOCNTL0_INDEX  0x0000001C
#define ip_RADIOCNTL0_RESET  0x00000002

__INLINE uint32_t ip_radiocntl0_get(void)
{
    return REG_IP_RD(ip_RADIOCNTL0_ADDR);
}

__INLINE void ip_radiocntl0_set(uint32_t value)
{
    REG_IP_WR(ip_RADIOCNTL0_ADDR, value);
}

// field definitions
#define ip_SPIPTR_MASK    ((uint32_t)0x3FFF0000)
#define ip_SPIPTR_LSB     16
#define ip_SPIPTR_WIDTH   ((uint32_t)0x0000000E)
#define ip_SPICFG_BIT     ((uint32_t)0x00000080)
#define ip_SPICFG_POS     7
#define ip_SPIFREQ_MASK   ((uint32_t)0x00000030)
#define ip_SPIFREQ_LSB    4
#define ip_SPIFREQ_WIDTH  ((uint32_t)0x00000002)
#define ip_SPICOMP_BIT    ((uint32_t)0x00000002)
#define ip_SPICOMP_POS    1
#define ip_SPIGO_BIT      ((uint32_t)0x00000001)
#define ip_SPIGO_POS      0

#define ip_SPIPTR_RST     0x0
#define ip_SPICFG_RST     0x0
#define ip_SPIFREQ_RST    0x0
#define ip_SPICOMP_RST    0x1
#define ip_SPIGO_RST      0x0

__INLINE void ip_radiocntl0_pack(uint16_t spiptr, uint8_t spicfg, uint8_t spifreq, uint8_t spigo)
{
    ASSERT_ERR((((uint32_t)spiptr << 16) & ~((uint32_t)0x3FFF0000)) == 0);
    ASSERT_ERR((((uint32_t)spicfg << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)spifreq << 4) & ~((uint32_t)0x00000030)) == 0);
    ASSERT_ERR((((uint32_t)spigo << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_RADIOCNTL0_ADDR,  ((uint32_t)spiptr << 16) | ((uint32_t)spicfg << 7) | ((uint32_t)spifreq << 4) | ((uint32_t)spigo << 0));
}

__INLINE void ip_radiocntl0_unpack(uint16_t* spiptr, uint8_t* spicfg, uint8_t* spifreq, uint8_t* spicomp, uint8_t* spigo)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL0_ADDR);

    *spiptr = (localVal & ((uint32_t)0x3FFF0000)) >> 16;
    *spicfg = (localVal & ((uint32_t)0x00000080)) >> 7;
    *spifreq = (localVal & ((uint32_t)0x00000030)) >> 4;
    *spicomp = (localVal & ((uint32_t)0x00000002)) >> 1;
    *spigo = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint16_t ip_radiocntl0_spiptr_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL0_ADDR);
    return ((localVal & ((uint32_t)0x3FFF0000)) >> 16);
}

__INLINE void ip_radiocntl0_spiptr_setf(uint16_t spiptr)
{
    ASSERT_ERR((((uint32_t)spiptr << 16) & ~((uint32_t)0x3FFF0000)) == 0);
    REG_IP_WR(ip_RADIOCNTL0_ADDR, (REG_IP_RD(ip_RADIOCNTL0_ADDR) & ~((uint32_t)0x3FFF0000)) | ((uint32_t)spiptr << 16));
}

__INLINE uint8_t ip_radiocntl0_spicfg_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

__INLINE void ip_radiocntl0_spicfg_setf(uint8_t spicfg)
{
    ASSERT_ERR((((uint32_t)spicfg << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_IP_WR(ip_RADIOCNTL0_ADDR, (REG_IP_RD(ip_RADIOCNTL0_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)spicfg << 7));
}

__INLINE uint8_t ip_radiocntl0_spifreq_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL0_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

__INLINE void ip_radiocntl0_spifreq_setf(uint8_t spifreq)
{
    ASSERT_ERR((((uint32_t)spifreq << 4) & ~((uint32_t)0x00000030)) == 0);
    REG_IP_WR(ip_RADIOCNTL0_ADDR, (REG_IP_RD(ip_RADIOCNTL0_ADDR) & ~((uint32_t)0x00000030)) | ((uint32_t)spifreq << 4));
}

__INLINE uint8_t ip_radiocntl0_spicomp_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE uint8_t ip_radiocntl0_spigo_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void ip_radiocntl0_spigo_setf(uint8_t spigo)
{
    ASSERT_ERR((((uint32_t)spigo << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_RADIOCNTL0_ADDR, (REG_IP_RD(ip_RADIOCNTL0_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)spigo << 0));
}

/**
 * @brief RADIOCNTL1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31          FORCEAGC_EN   0
 *     30              FORCEIQ   0
 *     29               RXDNSL   0
 *     28               TXDNSL   0
 *  27:16      FORCEAGC_LENGTH   0x0
 *     15      SYNC_PULSE_MODE   0
 *     14       SYNC_PULSE_SRC   0
 *     13            DPCORR_EN   0
 *     12           JEF_SELECT   0
 *  09:04               XRFSEL   0x0
 *  03:00           SUBVERSION   0x0
 * </pre>
 */
#define ip_RADIOCNTL1_ADDR   0x00820074
#define ip_RADIOCNTL1_OFFSET 0x00000074
#define ip_RADIOCNTL1_INDEX  0x0000001D
#define ip_RADIOCNTL1_RESET  0x00000000

__INLINE uint32_t ip_radiocntl1_get(void)
{
    return REG_IP_RD(ip_RADIOCNTL1_ADDR);
}

__INLINE void ip_radiocntl1_set(uint32_t value)
{
    REG_IP_WR(ip_RADIOCNTL1_ADDR, value);
}

// field definitions
#define ip_FORCEAGC_EN_BIT        ((uint32_t)0x80000000)
#define ip_FORCEAGC_EN_POS        31
#define ip_FORCEIQ_BIT            ((uint32_t)0x40000000)
#define ip_FORCEIQ_POS            30
#define ip_RXDNSL_BIT             ((uint32_t)0x20000000)
#define ip_RXDNSL_POS             29
#define ip_TXDNSL_BIT             ((uint32_t)0x10000000)
#define ip_TXDNSL_POS             28
#define ip_FORCEAGC_LENGTH_MASK   ((uint32_t)0x0FFF0000)
#define ip_FORCEAGC_LENGTH_LSB    16
#define ip_FORCEAGC_LENGTH_WIDTH  ((uint32_t)0x0000000C)
#define ip_SYNC_PULSE_MODE_BIT    ((uint32_t)0x00008000)
#define ip_SYNC_PULSE_MODE_POS    15
#define ip_SYNC_PULSE_SRC_BIT     ((uint32_t)0x00004000)
#define ip_SYNC_PULSE_SRC_POS     14
#define ip_DPCORR_EN_BIT          ((uint32_t)0x00002000)
#define ip_DPCORR_EN_POS          13
#define ip_JEF_SELECT_BIT         ((uint32_t)0x00001000)
#define ip_JEF_SELECT_POS         12
#define ip_XRFSEL_MASK            ((uint32_t)0x000003F0)
#define ip_XRFSEL_LSB             4
#define ip_XRFSEL_WIDTH           ((uint32_t)0x00000006)
#define ip_SUBVERSION_MASK        ((uint32_t)0x0000000F)
#define ip_SUBVERSION_LSB         0
#define ip_SUBVERSION_WIDTH       ((uint32_t)0x00000004)

#define ip_FORCEAGC_EN_RST        0x0
#define ip_FORCEIQ_RST            0x0
#define ip_RXDNSL_RST             0x0
#define ip_TXDNSL_RST             0x0
#define ip_FORCEAGC_LENGTH_RST    0x0
#define ip_SYNC_PULSE_MODE_RST    0x0
#define ip_SYNC_PULSE_SRC_RST     0x0
#define ip_DPCORR_EN_RST          0x0
#define ip_JEF_SELECT_RST         0x0
#define ip_XRFSEL_RST             0x0
#define ip_SUBVERSION_RST         0x0

__INLINE void ip_radiocntl1_pack(uint8_t forceagcen, uint8_t forceiq, uint8_t rxdnsl, uint8_t txdnsl, uint16_t forceagclength, uint8_t syncpulsemode, uint8_t syncpulsesrc, uint8_t dpcorren, uint8_t jefselect, uint8_t xrfsel, uint8_t subversion)
{
    ASSERT_ERR((((uint32_t)forceagcen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)forceiq << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)rxdnsl << 29) & ~((uint32_t)0x20000000)) == 0);
    ASSERT_ERR((((uint32_t)txdnsl << 28) & ~((uint32_t)0x10000000)) == 0);
    ASSERT_ERR((((uint32_t)forceagclength << 16) & ~((uint32_t)0x0FFF0000)) == 0);
    ASSERT_ERR((((uint32_t)syncpulsemode << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)syncpulsesrc << 14) & ~((uint32_t)0x00004000)) == 0);
    ASSERT_ERR((((uint32_t)dpcorren << 13) & ~((uint32_t)0x00002000)) == 0);
    ASSERT_ERR((((uint32_t)jefselect << 12) & ~((uint32_t)0x00001000)) == 0);
    ASSERT_ERR((((uint32_t)xrfsel << 4) & ~((uint32_t)0x000003F0)) == 0);
    ASSERT_ERR((((uint32_t)subversion << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR,  ((uint32_t)forceagcen << 31) | ((uint32_t)forceiq << 30) | ((uint32_t)rxdnsl << 29) | ((uint32_t)txdnsl << 28) | ((uint32_t)forceagclength << 16) | ((uint32_t)syncpulsemode << 15) | ((uint32_t)syncpulsesrc << 14) | ((uint32_t)dpcorren << 13) | ((uint32_t)jefselect << 12) | ((uint32_t)xrfsel << 4) | ((uint32_t)subversion << 0));
}

__INLINE void ip_radiocntl1_unpack(uint8_t* forceagcen, uint8_t* forceiq, uint8_t* rxdnsl, uint8_t* txdnsl, uint16_t* forceagclength, uint8_t* syncpulsemode, uint8_t* syncpulsesrc, uint8_t* dpcorren, uint8_t* jefselect, uint8_t* xrfsel, uint8_t* subversion)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);

    *forceagcen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *forceiq = (localVal & ((uint32_t)0x40000000)) >> 30;
    *rxdnsl = (localVal & ((uint32_t)0x20000000)) >> 29;
    *txdnsl = (localVal & ((uint32_t)0x10000000)) >> 28;
    *forceagclength = (localVal & ((uint32_t)0x0FFF0000)) >> 16;
    *syncpulsemode = (localVal & ((uint32_t)0x00008000)) >> 15;
    *syncpulsesrc = (localVal & ((uint32_t)0x00004000)) >> 14;
    *dpcorren = (localVal & ((uint32_t)0x00002000)) >> 13;
    *jefselect = (localVal & ((uint32_t)0x00001000)) >> 12;
    *xrfsel = (localVal & ((uint32_t)0x000003F0)) >> 4;
    *subversion = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

__INLINE uint8_t ip_radiocntl1_forceagc_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_radiocntl1_forceagc_en_setf(uint8_t forceagcen)
{
    ASSERT_ERR((((uint32_t)forceagcen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)forceagcen << 31));
}

__INLINE uint8_t ip_radiocntl1_forceiq_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void ip_radiocntl1_forceiq_setf(uint8_t forceiq)
{
    ASSERT_ERR((((uint32_t)forceiq << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)forceiq << 30));
}

__INLINE uint8_t ip_radiocntl1_rxdnsl_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

__INLINE void ip_radiocntl1_rxdnsl_setf(uint8_t rxdnsl)
{
    ASSERT_ERR((((uint32_t)rxdnsl << 29) & ~((uint32_t)0x20000000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x20000000)) | ((uint32_t)rxdnsl << 29));
}

__INLINE uint8_t ip_radiocntl1_txdnsl_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

__INLINE void ip_radiocntl1_txdnsl_setf(uint8_t txdnsl)
{
    ASSERT_ERR((((uint32_t)txdnsl << 28) & ~((uint32_t)0x10000000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x10000000)) | ((uint32_t)txdnsl << 28));
}

__INLINE uint16_t ip_radiocntl1_forceagc_length_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x0FFF0000)) >> 16);
}

__INLINE void ip_radiocntl1_forceagc_length_setf(uint16_t forceagclength)
{
    ASSERT_ERR((((uint32_t)forceagclength << 16) & ~((uint32_t)0x0FFF0000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x0FFF0000)) | ((uint32_t)forceagclength << 16));
}

__INLINE uint8_t ip_radiocntl1_sync_pulse_mode_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE void ip_radiocntl1_sync_pulse_mode_setf(uint8_t syncpulsemode)
{
    ASSERT_ERR((((uint32_t)syncpulsemode << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)syncpulsemode << 15));
}

__INLINE uint8_t ip_radiocntl1_sync_pulse_src_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

__INLINE void ip_radiocntl1_sync_pulse_src_setf(uint8_t syncpulsesrc)
{
    ASSERT_ERR((((uint32_t)syncpulsesrc << 14) & ~((uint32_t)0x00004000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x00004000)) | ((uint32_t)syncpulsesrc << 14));
}

__INLINE uint8_t ip_radiocntl1_dpcorr_en_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

__INLINE void ip_radiocntl1_dpcorr_en_setf(uint8_t dpcorren)
{
    ASSERT_ERR((((uint32_t)dpcorren << 13) & ~((uint32_t)0x00002000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x00002000)) | ((uint32_t)dpcorren << 13));
}

__INLINE uint8_t ip_radiocntl1_jef_select_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

__INLINE void ip_radiocntl1_jef_select_setf(uint8_t jefselect)
{
    ASSERT_ERR((((uint32_t)jefselect << 12) & ~((uint32_t)0x00001000)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x00001000)) | ((uint32_t)jefselect << 12));
}

__INLINE uint8_t ip_radiocntl1_xrfsel_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x000003F0)) >> 4);
}

__INLINE void ip_radiocntl1_xrfsel_setf(uint8_t xrfsel)
{
    ASSERT_ERR((((uint32_t)xrfsel << 4) & ~((uint32_t)0x000003F0)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x000003F0)) | ((uint32_t)xrfsel << 4));
}

__INLINE uint8_t ip_radiocntl1_subversion_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RADIOCNTL1_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

__INLINE void ip_radiocntl1_subversion_setf(uint8_t subversion)
{
    ASSERT_ERR((((uint32_t)subversion << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_IP_WR(ip_RADIOCNTL1_ADDR, (REG_IP_RD(ip_RADIOCNTL1_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)subversion << 0));
}

/**
 * @brief AESCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     01             AES_MODE   0
 *     00            AES_START   0
 * </pre>
 */
#define ip_AESCNTL_ADDR   0x008200B0
#define ip_AESCNTL_OFFSET 0x000000B0
#define ip_AESCNTL_INDEX  0x0000002C
#define ip_AESCNTL_RESET  0x00000000

__INLINE uint32_t ip_aescntl_get(void)
{
    return REG_IP_RD(ip_AESCNTL_ADDR);
}

__INLINE void ip_aescntl_set(uint32_t value)
{
    REG_IP_WR(ip_AESCNTL_ADDR, value);
}

// field definitions
#define ip_AES_MODE_BIT     ((uint32_t)0x00000002)
#define ip_AES_MODE_POS     1
#define ip_AES_START_BIT    ((uint32_t)0x00000001)
#define ip_AES_START_POS    0

#define ip_AES_MODE_RST     0x0
#define ip_AES_START_RST    0x0

__INLINE void ip_aescntl_pack(uint8_t aesmode, uint8_t aesstart)
{
    ASSERT_ERR((((uint32_t)aesmode << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)aesstart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_AESCNTL_ADDR,  ((uint32_t)aesmode << 1) | ((uint32_t)aesstart << 0));
}

__INLINE void ip_aescntl_unpack(uint8_t* aesmode, uint8_t* aesstart)
{
    uint32_t localVal = REG_IP_RD(ip_AESCNTL_ADDR);

    *aesmode = (localVal & ((uint32_t)0x00000002)) >> 1;
    *aesstart = (localVal & ((uint32_t)0x00000001)) >> 0;
}

__INLINE uint8_t ip_aescntl_aes_mode_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void ip_aescntl_aes_mode_setf(uint8_t aesmode)
{
    ASSERT_ERR((((uint32_t)aesmode << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_IP_WR(ip_AESCNTL_ADDR, (REG_IP_RD(ip_AESCNTL_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)aesmode << 1));
}

__INLINE uint8_t ip_aescntl_aes_start_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void ip_aescntl_aes_start_setf(uint8_t aesstart)
{
    ASSERT_ERR((((uint32_t)aesstart << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_IP_WR(ip_AESCNTL_ADDR, (REG_IP_RD(ip_AESCNTL_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)aesstart << 0));
}

/**
 * @brief AESKEY31_0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           AESKEY31_0   0x0
 * </pre>
 */
#define ip_AESKEY31_0_ADDR   0x008200B4
#define ip_AESKEY31_0_OFFSET 0x000000B4
#define ip_AESKEY31_0_INDEX  0x0000002D
#define ip_AESKEY31_0_RESET  0x00000000

__INLINE uint32_t ip_aeskey31_0_get(void)
{
    return REG_IP_RD(ip_AESKEY31_0_ADDR);
}

__INLINE void ip_aeskey31_0_set(uint32_t value)
{
    REG_IP_WR(ip_AESKEY31_0_ADDR, value);
}

// field definitions
#define ip_AESKEY31_0_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_AESKEY31_0_LSB    0
#define ip_AESKEY31_0_WIDTH  ((uint32_t)0x00000020)

#define ip_AESKEY31_0_RST    0x0

__INLINE uint32_t ip_aeskey31_0_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESKEY31_0_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey31_0_setf(uint32_t aeskey310)
{
    ASSERT_ERR((((uint32_t)aeskey310 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(ip_AESKEY31_0_ADDR, (uint32_t)aeskey310 << 0);
}

/**
 * @brief AESKEY63_32 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          AESKEY63_32   0x0
 * </pre>
 */
#define ip_AESKEY63_32_ADDR   0x008200B8
#define ip_AESKEY63_32_OFFSET 0x000000B8
#define ip_AESKEY63_32_INDEX  0x0000002E
#define ip_AESKEY63_32_RESET  0x00000000

__INLINE uint32_t ip_aeskey63_32_get(void)
{
    return REG_IP_RD(ip_AESKEY63_32_ADDR);
}

__INLINE void ip_aeskey63_32_set(uint32_t value)
{
    REG_IP_WR(ip_AESKEY63_32_ADDR, value);
}

// field definitions
#define ip_AESKEY63_32_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_AESKEY63_32_LSB    0
#define ip_AESKEY63_32_WIDTH  ((uint32_t)0x00000020)

#define ip_AESKEY63_32_RST    0x0

__INLINE uint32_t ip_aeskey63_32_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESKEY63_32_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey63_32_setf(uint32_t aeskey6332)
{
    ASSERT_ERR((((uint32_t)aeskey6332 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(ip_AESKEY63_32_ADDR, (uint32_t)aeskey6332 << 0);
}

/**
 * @brief AESKEY95_64 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00          AESKEY95_64   0x0
 * </pre>
 */
#define ip_AESKEY95_64_ADDR   0x008200BC
#define ip_AESKEY95_64_OFFSET 0x000000BC
#define ip_AESKEY95_64_INDEX  0x0000002F
#define ip_AESKEY95_64_RESET  0x00000000

__INLINE uint32_t ip_aeskey95_64_get(void)
{
    return REG_IP_RD(ip_AESKEY95_64_ADDR);
}

__INLINE void ip_aeskey95_64_set(uint32_t value)
{
    REG_IP_WR(ip_AESKEY95_64_ADDR, value);
}

// field definitions
#define ip_AESKEY95_64_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_AESKEY95_64_LSB    0
#define ip_AESKEY95_64_WIDTH  ((uint32_t)0x00000020)

#define ip_AESKEY95_64_RST    0x0

__INLINE uint32_t ip_aeskey95_64_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESKEY95_64_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey95_64_setf(uint32_t aeskey9564)
{
    ASSERT_ERR((((uint32_t)aeskey9564 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(ip_AESKEY95_64_ADDR, (uint32_t)aeskey9564 << 0);
}

/**
 * @brief AESKEY127_96 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         AESKEY127_96   0x0
 * </pre>
 */
#define ip_AESKEY127_96_ADDR   0x008200C0
#define ip_AESKEY127_96_OFFSET 0x000000C0
#define ip_AESKEY127_96_INDEX  0x00000030
#define ip_AESKEY127_96_RESET  0x00000000

__INLINE uint32_t ip_aeskey127_96_get(void)
{
    return REG_IP_RD(ip_AESKEY127_96_ADDR);
}

__INLINE void ip_aeskey127_96_set(uint32_t value)
{
    REG_IP_WR(ip_AESKEY127_96_ADDR, value);
}

// field definitions
#define ip_AESKEY127_96_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_AESKEY127_96_LSB    0
#define ip_AESKEY127_96_WIDTH  ((uint32_t)0x00000020)

#define ip_AESKEY127_96_RST    0x0

__INLINE uint32_t ip_aeskey127_96_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESKEY127_96_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aeskey127_96_setf(uint32_t aeskey12796)
{
    ASSERT_ERR((((uint32_t)aeskey12796 << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_IP_WR(ip_AESKEY127_96_ADDR, (uint32_t)aeskey12796 << 0);
}

/**
 * @brief AESPTR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:00               AESPTR   0x0
 * </pre>
 */
#define ip_AESPTR_ADDR   0x008200C4
#define ip_AESPTR_OFFSET 0x000000C4
#define ip_AESPTR_INDEX  0x00000031
#define ip_AESPTR_RESET  0x00000000

__INLINE uint32_t ip_aesptr_get(void)
{
    return REG_IP_RD(ip_AESPTR_ADDR);
}

__INLINE void ip_aesptr_set(uint32_t value)
{
    REG_IP_WR(ip_AESPTR_ADDR, value);
}

// field definitions
#define ip_AESPTR_MASK   ((uint32_t)0x00003FFF)
#define ip_AESPTR_LSB    0
#define ip_AESPTR_WIDTH  ((uint32_t)0x0000000E)

#define ip_AESPTR_RST    0x0

__INLINE uint16_t ip_aesptr_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_AESPTR_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00003FFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_aesptr_setf(uint16_t aesptr)
{
    ASSERT_ERR((((uint32_t)aesptr << 0) & ~((uint32_t)0x00003FFF)) == 0);
    REG_IP_WR(ip_AESPTR_ADDR, (uint32_t)aesptr << 0);
}

/**
 * @brief TXMICVAL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             TXMICVAL   0x0
 * </pre>
 */
#define ip_TXMICVAL_ADDR   0x008200C8
#define ip_TXMICVAL_OFFSET 0x000000C8
#define ip_TXMICVAL_INDEX  0x00000032
#define ip_TXMICVAL_RESET  0x00000000

__INLINE uint32_t ip_txmicval_get(void)
{
    return REG_IP_RD(ip_TXMICVAL_ADDR);
}

// field definitions
#define ip_TXMICVAL_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_TXMICVAL_LSB    0
#define ip_TXMICVAL_WIDTH  ((uint32_t)0x00000020)

#define ip_TXMICVAL_RST    0x0

__INLINE uint32_t ip_txmicval_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_TXMICVAL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief RXMICVAL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00             RXMICVAL   0x0
 * </pre>
 */
#define ip_RXMICVAL_ADDR   0x008200CC
#define ip_RXMICVAL_OFFSET 0x000000CC
#define ip_RXMICVAL_INDEX  0x00000033
#define ip_RXMICVAL_RESET  0x00000000

__INLINE uint32_t ip_rxmicval_get(void)
{
    return REG_IP_RD(ip_RXMICVAL_ADDR);
}

// field definitions
#define ip_RXMICVAL_MASK   ((uint32_t)0xFFFFFFFF)
#define ip_RXMICVAL_LSB    0
#define ip_RXMICVAL_WIDTH  ((uint32_t)0x00000020)

#define ip_RXMICVAL_RST    0x0

__INLINE uint32_t ip_rxmicval_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_RXMICVAL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief TIMGENCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  25:16   PREFETCHABORT_TIME   0x1DF
 *  08:00        PREFETCH_TIME   0xBD
 * </pre>
 */
#define ip_TIMGENCNTL_ADDR   0x008200E0
#define ip_TIMGENCNTL_OFFSET 0x000000E0
#define ip_TIMGENCNTL_INDEX  0x00000038
#define ip_TIMGENCNTL_RESET  0x01DF00BD

__INLINE uint32_t ip_timgencntl_get(void)
{
    return REG_IP_RD(ip_TIMGENCNTL_ADDR);
}

__INLINE void ip_timgencntl_set(uint32_t value)
{
    REG_IP_WR(ip_TIMGENCNTL_ADDR, value);
}

// field definitions
#define ip_PREFETCHABORT_TIME_MASK   ((uint32_t)0x03FF0000)
#define ip_PREFETCHABORT_TIME_LSB    16
#define ip_PREFETCHABORT_TIME_WIDTH  ((uint32_t)0x0000000A)
#define ip_PREFETCH_TIME_MASK        ((uint32_t)0x000001FF)
#define ip_PREFETCH_TIME_LSB         0
#define ip_PREFETCH_TIME_WIDTH       ((uint32_t)0x00000009)

#define ip_PREFETCHABORT_TIME_RST    0x1DF
#define ip_PREFETCH_TIME_RST         0xBD

__INLINE void ip_timgencntl_pack(uint16_t prefetchaborttime, uint16_t prefetchtime)
{
    ASSERT_ERR((((uint32_t)prefetchaborttime << 16) & ~((uint32_t)0x03FF0000)) == 0);
    ASSERT_ERR((((uint32_t)prefetchtime << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_IP_WR(ip_TIMGENCNTL_ADDR,  ((uint32_t)prefetchaborttime << 16) | ((uint32_t)prefetchtime << 0));
}

__INLINE void ip_timgencntl_unpack(uint16_t* prefetchaborttime, uint16_t* prefetchtime)
{
    uint32_t localVal = REG_IP_RD(ip_TIMGENCNTL_ADDR);

    *prefetchaborttime = (localVal & ((uint32_t)0x03FF0000)) >> 16;
    *prefetchtime = (localVal & ((uint32_t)0x000001FF)) >> 0;
}

__INLINE uint16_t ip_timgencntl_prefetchabort_time_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_TIMGENCNTL_ADDR);
    return ((localVal & ((uint32_t)0x03FF0000)) >> 16);
}

__INLINE void ip_timgencntl_prefetchabort_time_setf(uint16_t prefetchaborttime)
{
    ASSERT_ERR((((uint32_t)prefetchaborttime << 16) & ~((uint32_t)0x03FF0000)) == 0);
    REG_IP_WR(ip_TIMGENCNTL_ADDR, (REG_IP_RD(ip_TIMGENCNTL_ADDR) & ~((uint32_t)0x03FF0000)) | ((uint32_t)prefetchaborttime << 16));
}

__INLINE uint16_t ip_timgencntl_prefetch_time_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_TIMGENCNTL_ADDR);
    return ((localVal & ((uint32_t)0x000001FF)) >> 0);
}

__INLINE void ip_timgencntl_prefetch_time_setf(uint16_t prefetchtime)
{
    ASSERT_ERR((((uint32_t)prefetchtime << 0) & ~((uint32_t)0x000001FF)) == 0);
    REG_IP_WR(ip_TIMGENCNTL_ADDR, (REG_IP_RD(ip_TIMGENCNTL_ADDR) & ~((uint32_t)0x000001FF)) | ((uint32_t)prefetchtime << 0));
}

/**
 * @brief FINETIMTGT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:00           FINETARGET   0x0
 * </pre>
 */
#define ip_FINETIMTGT_ADDR   0x008200E4
#define ip_FINETIMTGT_OFFSET 0x000000E4
#define ip_FINETIMTGT_INDEX  0x00000039
#define ip_FINETIMTGT_RESET  0x00000000

__INLINE uint32_t ip_finetimtgt_get(void)
{
    return REG_IP_RD(ip_FINETIMTGT_ADDR);
}

__INLINE void ip_finetimtgt_set(uint32_t value)
{
    REG_IP_WR(ip_FINETIMTGT_ADDR, value);
}

// field definitions
#define ip_FINETARGET_MASK   ((uint32_t)0x0FFFFFFF)
#define ip_FINETARGET_LSB    0
#define ip_FINETARGET_WIDTH  ((uint32_t)0x0000001C)

#define ip_FINETARGET_RST    0x0

__INLINE uint32_t ip_finetimtgt_finetarget_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_FINETIMTGT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0FFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_finetimtgt_finetarget_setf(uint32_t finetarget)
{
    ASSERT_ERR((((uint32_t)finetarget << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_FINETIMTGT_ADDR, (uint32_t)finetarget << 0);
}

/**
 * @brief CLKNTGT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:00             CLKNTGT1   0x0
 * </pre>
 */
#define ip_CLKNTGT1_ADDR   0x008200E8
#define ip_CLKNTGT1_OFFSET 0x000000E8
#define ip_CLKNTGT1_INDEX  0x0000003A
#define ip_CLKNTGT1_RESET  0x00000000

__INLINE uint32_t ip_clkntgt1_get(void)
{
    return REG_IP_RD(ip_CLKNTGT1_ADDR);
}

__INLINE void ip_clkntgt1_set(uint32_t value)
{
    REG_IP_WR(ip_CLKNTGT1_ADDR, value);
}

// field definitions
#define ip_CLKNTGT1_MASK   ((uint32_t)0x0FFFFFFF)
#define ip_CLKNTGT1_LSB    0
#define ip_CLKNTGT1_WIDTH  ((uint32_t)0x0000001C)

#define ip_CLKNTGT1_RST    0x0

__INLINE uint32_t ip_clkntgt1_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_CLKNTGT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0FFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_clkntgt1_setf(uint32_t clkntgt1)
{
    ASSERT_ERR((((uint32_t)clkntgt1 << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_CLKNTGT1_ADDR, (uint32_t)clkntgt1 << 0);
}

/**
 * @brief HMICROSECTGT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  09:00        HMICROSECTGT1   0x0
 * </pre>
 */
#define ip_HMICROSECTGT1_ADDR   0x008200EC
#define ip_HMICROSECTGT1_OFFSET 0x000000EC
#define ip_HMICROSECTGT1_INDEX  0x0000003B
#define ip_HMICROSECTGT1_RESET  0x00000000

__INLINE uint32_t ip_hmicrosectgt1_get(void)
{
    return REG_IP_RD(ip_HMICROSECTGT1_ADDR);
}

__INLINE void ip_hmicrosectgt1_set(uint32_t value)
{
    REG_IP_WR(ip_HMICROSECTGT1_ADDR, value);
}

// field definitions
#define ip_HMICROSECTGT1_MASK   ((uint32_t)0x000003FF)
#define ip_HMICROSECTGT1_LSB    0
#define ip_HMICROSECTGT1_WIDTH  ((uint32_t)0x0000000A)

#define ip_HMICROSECTGT1_RST    0x0

__INLINE uint16_t ip_hmicrosectgt1_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_HMICROSECTGT1_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_hmicrosectgt1_setf(uint16_t hmicrosectgt1)
{
    ASSERT_ERR((((uint32_t)hmicrosectgt1 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_IP_WR(ip_HMICROSECTGT1_ADDR, (uint32_t)hmicrosectgt1 << 0);
}

/**
 * @brief CLKNTGT2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  27:00             CLKNTGT2   0x0
 * </pre>
 */
#define ip_CLKNTGT2_ADDR   0x008200F0
#define ip_CLKNTGT2_OFFSET 0x000000F0
#define ip_CLKNTGT2_INDEX  0x0000003C
#define ip_CLKNTGT2_RESET  0x00000000

__INLINE uint32_t ip_clkntgt2_get(void)
{
    return REG_IP_RD(ip_CLKNTGT2_ADDR);
}

__INLINE void ip_clkntgt2_set(uint32_t value)
{
    REG_IP_WR(ip_CLKNTGT2_ADDR, value);
}

// field definitions
#define ip_CLKNTGT2_MASK   ((uint32_t)0x0FFFFFFF)
#define ip_CLKNTGT2_LSB    0
#define ip_CLKNTGT2_WIDTH  ((uint32_t)0x0000001C)

#define ip_CLKNTGT2_RST    0x0

__INLINE uint32_t ip_clkntgt2_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_CLKNTGT2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0FFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_clkntgt2_setf(uint32_t clkntgt2)
{
    ASSERT_ERR((((uint32_t)clkntgt2 << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_CLKNTGT2_ADDR, (uint32_t)clkntgt2 << 0);
}

/**
 * @brief HMICROSECTGT2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  09:00        HMICROSECTGT2   0x0
 * </pre>
 */
#define ip_HMICROSECTGT2_ADDR   0x008200F4
#define ip_HMICROSECTGT2_OFFSET 0x000000F4
#define ip_HMICROSECTGT2_INDEX  0x0000003D
#define ip_HMICROSECTGT2_RESET  0x00000000

__INLINE uint32_t ip_hmicrosectgt2_get(void)
{
    return REG_IP_RD(ip_HMICROSECTGT2_ADDR);
}

__INLINE void ip_hmicrosectgt2_set(uint32_t value)
{
    REG_IP_WR(ip_HMICROSECTGT2_ADDR, value);
}

// field definitions
#define ip_HMICROSECTGT2_MASK   ((uint32_t)0x000003FF)
#define ip_HMICROSECTGT2_LSB    0
#define ip_HMICROSECTGT2_WIDTH  ((uint32_t)0x0000000A)

#define ip_HMICROSECTGT2_RST    0x0

__INLINE uint16_t ip_hmicrosectgt2_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_HMICROSECTGT2_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ip_hmicrosectgt2_setf(uint16_t hmicrosectgt2)
{
    ASSERT_ERR((((uint32_t)hmicrosectgt2 << 0) & ~((uint32_t)0x000003FF)) == 0);
    REG_IP_WR(ip_HMICROSECTGT2_ADDR, (uint32_t)hmicrosectgt2 << 0);
}

/**
 * @brief SLOTCLK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31                 SAMP   0
 *     30             CLKN_UPD   0
 *  27:00                 SCLK   0x0
 * </pre>
 */
#define ip_SLOTCLK_ADDR   0x008200F8
#define ip_SLOTCLK_OFFSET 0x000000F8
#define ip_SLOTCLK_INDEX  0x0000003E
#define ip_SLOTCLK_RESET  0x00000000

__INLINE uint32_t ip_slotclk_get(void)
{
    return REG_IP_RD(ip_SLOTCLK_ADDR);
}

__INLINE void ip_slotclk_set(uint32_t value)
{
    REG_IP_WR(ip_SLOTCLK_ADDR, value);
}

// field definitions
#define ip_SAMP_BIT        ((uint32_t)0x80000000)
#define ip_SAMP_POS        31
#define ip_CLKN_UPD_BIT    ((uint32_t)0x40000000)
#define ip_CLKN_UPD_POS    30
#define ip_SCLK_MASK       ((uint32_t)0x0FFFFFFF)
#define ip_SCLK_LSB        0
#define ip_SCLK_WIDTH      ((uint32_t)0x0000001C)

#define IP_SAMP_RST        0x0
#define ip_SAMP_RST        0x0
#define ip_CLKN_UPD_RST    0x0
#define ip_SCLK_RST        0x0

__INLINE void ip_slotclk_pack(uint8_t samp, uint8_t clknupd, uint32_t sclk)
{
    ASSERT_ERR((((uint32_t)samp << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)clknupd << 30) & ~((uint32_t)0x40000000)) == 0);
    ASSERT_ERR((((uint32_t)sclk << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_SLOTCLK_ADDR,  ((uint32_t)samp << 31) | ((uint32_t)clknupd << 30) | ((uint32_t)sclk << 0));
}

__INLINE void ip_slotclk_unpack(uint8_t* samp, uint8_t* clknupd, uint32_t* sclk)
{
    uint32_t localVal = REG_IP_RD(ip_SLOTCLK_ADDR);

    *samp = (localVal & ((uint32_t)0x80000000)) >> 31;
    *clknupd = (localVal & ((uint32_t)0x40000000)) >> 30;
    *sclk = (localVal & ((uint32_t)0x0FFFFFFF)) >> 0;
}

__INLINE uint8_t ip_slotclk_samp_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SLOTCLK_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_slotclk_samp_setf(uint8_t samp)
{
    ASSERT_ERR((((uint32_t)samp << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_SLOTCLK_ADDR, (REG_IP_RD(ip_SLOTCLK_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)samp << 31));
}

__INLINE uint8_t ip_slotclk_clkn_upd_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SLOTCLK_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

__INLINE void ip_slotclk_clkn_upd_setf(uint8_t clknupd)
{
    ASSERT_ERR((((uint32_t)clknupd << 30) & ~((uint32_t)0x40000000)) == 0);
    REG_IP_WR(ip_SLOTCLK_ADDR, (REG_IP_RD(ip_SLOTCLK_ADDR) & ~((uint32_t)0x40000000)) | ((uint32_t)clknupd << 30));
}

__INLINE uint32_t ip_slotclk_sclk_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_SLOTCLK_ADDR);
    return ((localVal & ((uint32_t)0x0FFFFFFF)) >> 0);
}

__INLINE void ip_slotclk_sclk_setf(uint32_t sclk)
{
    ASSERT_ERR((((uint32_t)sclk << 0) & ~((uint32_t)0x0FFFFFFF)) == 0);
    REG_IP_WR(ip_SLOTCLK_ADDR, (REG_IP_RD(ip_SLOTCLK_ADDR) & ~((uint32_t)0x0FFFFFFF)) | ((uint32_t)sclk << 0));
}

/**
 * @brief FINETIMECNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  09:00              FINECNT   0x0
 * </pre>
 */
#define ip_FINETIMECNT_ADDR   0x008200FC
#define ip_FINETIMECNT_OFFSET 0x000000FC
#define ip_FINETIMECNT_INDEX  0x0000003F
#define ip_FINETIMECNT_RESET  0x00000000

__INLINE uint32_t ip_finetimecnt_get(void)
{
    return REG_IP_RD(ip_FINETIMECNT_ADDR);
}

// field definitions
#define ip_FINECNT_MASK   ((uint32_t)0x000003FF)
#define ip_FINECNT_LSB    0
#define ip_FINECNT_WIDTH  ((uint32_t)0x0000000A)

#define ip_FINECNT_RST    0x0

__INLINE uint16_t ip_finetimecnt_finecnt_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_FINETIMECNT_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief ACTSCHCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31            START_ACT   0
 *  03:00            ENTRY_IDX   0x0
 * </pre>
 */
#define ip_ACTSCHCNTL_ADDR   0x00820100
#define ip_ACTSCHCNTL_OFFSET 0x00000100
#define ip_ACTSCHCNTL_INDEX  0x00000040
#define ip_ACTSCHCNTL_RESET  0x00000000

__INLINE uint32_t ip_actschcntl_get(void)
{
    return REG_IP_RD(ip_ACTSCHCNTL_ADDR);
}

__INLINE void ip_actschcntl_set(uint32_t value)
{
    REG_IP_WR(ip_ACTSCHCNTL_ADDR, value);
}

// field definitions
#define ip_START_ACT_BIT    ((uint32_t)0x80000000)
#define ip_START_ACT_POS    31
#define ip_ENTRY_IDX_MASK   ((uint32_t)0x0000000F)
#define ip_ENTRY_IDX_LSB    0
#define ip_ENTRY_IDX_WIDTH  ((uint32_t)0x00000004)

#define ip_START_ACT_RST    0x0
#define ip_ENTRY_IDX_RST    0x0

__INLINE void ip_actschcntl_pack(uint8_t startact, uint8_t entryidx)
{
    ASSERT_ERR((((uint32_t)startact << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)entryidx << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_IP_WR(ip_ACTSCHCNTL_ADDR,  ((uint32_t)startact << 31) | ((uint32_t)entryidx << 0));
}

__INLINE void ip_actschcntl_unpack(uint8_t* startact, uint8_t* entryidx)
{
    uint32_t localVal = REG_IP_RD(ip_ACTSCHCNTL_ADDR);

    *startact = (localVal & ((uint32_t)0x80000000)) >> 31;
    *entryidx = (localVal & ((uint32_t)0x0000000F)) >> 0;
}

__INLINE uint8_t ip_actschcntl_start_act_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTSCHCNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_actschcntl_start_act_setf(uint8_t startact)
{
    ASSERT_ERR((((uint32_t)startact << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_ACTSCHCNTL_ADDR, (REG_IP_RD(ip_ACTSCHCNTL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)startact << 31));
}

__INLINE uint8_t ip_actschcntl_entry_idx_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_ACTSCHCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000000F)) >> 0);
}

__INLINE void ip_actschcntl_entry_idx_setf(uint8_t entryidx)
{
    ASSERT_ERR((((uint32_t)entryidx << 0) & ~((uint32_t)0x0000000F)) == 0);
    REG_IP_WR(ip_ACTSCHCNTL_ADDR, (REG_IP_RD(ip_ACTSCHCNTL_ADDR) & ~((uint32_t)0x0000000F)) | ((uint32_t)entryidx << 0));
}

/**
 * @brief DFANCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     31     BTRXPRIMIDCNTLEN   0
 *  30:24        BTRXPRIMANTID   0x0
 *     23     BTTXPRIMIDCNTLEN   0
 *  22:16        BTTXPRIMANTID   0x0
 *     15     LERXPRIMIDCNTLEN   0
 *  14:08        LERXPRIMANTID   0x0
 *     07     LETXPRIMIDCNTLEN   0
 *  06:00        LETXPRIMANTID   0x0
 * </pre>
 */
#define ip_DFANCNTL_ADDR   0x00820184
#define ip_DFANCNTL_OFFSET 0x00000184
#define ip_DFANCNTL_INDEX  0x00000061
#define ip_DFANCNTL_RESET  0x00000000

__INLINE uint32_t ip_dfancntl_get(void)
{
    return REG_IP_RD(ip_DFANCNTL_ADDR);
}

__INLINE void ip_dfancntl_set(uint32_t value)
{
    REG_IP_WR(ip_DFANCNTL_ADDR, value);
}

// field definitions
#define ip_BTRXPRIMIDCNTLEN_BIT    ((uint32_t)0x80000000)
#define ip_BTRXPRIMIDCNTLEN_POS    31
#define ip_BTRXPRIMANTID_MASK      ((uint32_t)0x7F000000)
#define ip_BTRXPRIMANTID_LSB       24
#define ip_BTRXPRIMANTID_WIDTH     ((uint32_t)0x00000007)
#define ip_BTTXPRIMIDCNTLEN_BIT    ((uint32_t)0x00800000)
#define ip_BTTXPRIMIDCNTLEN_POS    23
#define ip_BTTXPRIMANTID_MASK      ((uint32_t)0x007F0000)
#define ip_BTTXPRIMANTID_LSB       16
#define ip_BTTXPRIMANTID_WIDTH     ((uint32_t)0x00000007)
#define ip_LERXPRIMIDCNTLEN_BIT    ((uint32_t)0x00008000)
#define ip_LERXPRIMIDCNTLEN_POS    15
#define ip_LERXPRIMANTID_MASK      ((uint32_t)0x00007F00)
#define ip_LERXPRIMANTID_LSB       8
#define ip_LERXPRIMANTID_WIDTH     ((uint32_t)0x00000007)
#define ip_LETXPRIMIDCNTLEN_BIT    ((uint32_t)0x00000080)
#define ip_LETXPRIMIDCNTLEN_POS    7
#define ip_LETXPRIMANTID_MASK      ((uint32_t)0x0000007F)
#define ip_LETXPRIMANTID_LSB       0
#define ip_LETXPRIMANTID_WIDTH     ((uint32_t)0x00000007)

#define ip_BTRXPRIMIDCNTLEN_RST    0x0
#define ip_BTRXPRIMANTID_RST       0x0
#define ip_BTTXPRIMIDCNTLEN_RST    0x0
#define ip_BTTXPRIMANTID_RST       0x0
#define ip_LERXPRIMIDCNTLEN_RST    0x0
#define ip_LERXPRIMANTID_RST       0x0
#define ip_LETXPRIMIDCNTLEN_RST    0x0
#define ip_LETXPRIMANTID_RST       0x0

__INLINE void ip_dfancntl_pack(uint8_t btrxprimidcntlen, uint8_t btrxprimantid, uint8_t bttxprimidcntlen, uint8_t bttxprimantid, uint8_t lerxprimidcntlen, uint8_t lerxprimantid, uint8_t letxprimidcntlen, uint8_t letxprimantid)
{
    ASSERT_ERR((((uint32_t)btrxprimidcntlen << 31) & ~((uint32_t)0x80000000)) == 0);
    ASSERT_ERR((((uint32_t)btrxprimantid << 24) & ~((uint32_t)0x7F000000)) == 0);
    ASSERT_ERR((((uint32_t)bttxprimidcntlen << 23) & ~((uint32_t)0x00800000)) == 0);
    ASSERT_ERR((((uint32_t)bttxprimantid << 16) & ~((uint32_t)0x007F0000)) == 0);
    ASSERT_ERR((((uint32_t)lerxprimidcntlen << 15) & ~((uint32_t)0x00008000)) == 0);
    ASSERT_ERR((((uint32_t)lerxprimantid << 8) & ~((uint32_t)0x00007F00)) == 0);
    ASSERT_ERR((((uint32_t)letxprimidcntlen << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)letxprimantid << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR,  ((uint32_t)btrxprimidcntlen << 31) | ((uint32_t)btrxprimantid << 24) | ((uint32_t)bttxprimidcntlen << 23) | ((uint32_t)bttxprimantid << 16) | ((uint32_t)lerxprimidcntlen << 15) | ((uint32_t)lerxprimantid << 8) | ((uint32_t)letxprimidcntlen << 7) | ((uint32_t)letxprimantid << 0));
}

__INLINE void ip_dfancntl_unpack(uint8_t* btrxprimidcntlen, uint8_t* btrxprimantid, uint8_t* bttxprimidcntlen, uint8_t* bttxprimantid, uint8_t* lerxprimidcntlen, uint8_t* lerxprimantid, uint8_t* letxprimidcntlen, uint8_t* letxprimantid)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);

    *btrxprimidcntlen = (localVal & ((uint32_t)0x80000000)) >> 31;
    *btrxprimantid = (localVal & ((uint32_t)0x7F000000)) >> 24;
    *bttxprimidcntlen = (localVal & ((uint32_t)0x00800000)) >> 23;
    *bttxprimantid = (localVal & ((uint32_t)0x007F0000)) >> 16;
    *lerxprimidcntlen = (localVal & ((uint32_t)0x00008000)) >> 15;
    *lerxprimantid = (localVal & ((uint32_t)0x00007F00)) >> 8;
    *letxprimidcntlen = (localVal & ((uint32_t)0x00000080)) >> 7;
    *letxprimantid = (localVal & ((uint32_t)0x0000007F)) >> 0;
}

__INLINE uint8_t ip_dfancntl_btrxprimidcntlen_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

__INLINE void ip_dfancntl_btrxprimidcntlen_setf(uint8_t btrxprimidcntlen)
{
    ASSERT_ERR((((uint32_t)btrxprimidcntlen << 31) & ~((uint32_t)0x80000000)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x80000000)) | ((uint32_t)btrxprimidcntlen << 31));
}

__INLINE uint8_t ip_dfancntl_btrxprimantid_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x7F000000)) >> 24);
}

__INLINE void ip_dfancntl_btrxprimantid_setf(uint8_t btrxprimantid)
{
    ASSERT_ERR((((uint32_t)btrxprimantid << 24) & ~((uint32_t)0x7F000000)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x7F000000)) | ((uint32_t)btrxprimantid << 24));
}

__INLINE uint8_t ip_dfancntl_bttxprimidcntlen_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

__INLINE void ip_dfancntl_bttxprimidcntlen_setf(uint8_t bttxprimidcntlen)
{
    ASSERT_ERR((((uint32_t)bttxprimidcntlen << 23) & ~((uint32_t)0x00800000)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x00800000)) | ((uint32_t)bttxprimidcntlen << 23));
}

__INLINE uint8_t ip_dfancntl_bttxprimantid_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x007F0000)) >> 16);
}

__INLINE void ip_dfancntl_bttxprimantid_setf(uint8_t bttxprimantid)
{
    ASSERT_ERR((((uint32_t)bttxprimantid << 16) & ~((uint32_t)0x007F0000)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x007F0000)) | ((uint32_t)bttxprimantid << 16));
}

__INLINE uint8_t ip_dfancntl_lerxprimidcntlen_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

__INLINE void ip_dfancntl_lerxprimidcntlen_setf(uint8_t lerxprimidcntlen)
{
    ASSERT_ERR((((uint32_t)lerxprimidcntlen << 15) & ~((uint32_t)0x00008000)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x00008000)) | ((uint32_t)lerxprimidcntlen << 15));
}

__INLINE uint8_t ip_dfancntl_lerxprimantid_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00007F00)) >> 8);
}

__INLINE void ip_dfancntl_lerxprimantid_setf(uint8_t lerxprimantid)
{
    ASSERT_ERR((((uint32_t)lerxprimantid << 8) & ~((uint32_t)0x00007F00)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x00007F00)) | ((uint32_t)lerxprimantid << 8));
}

__INLINE uint8_t ip_dfancntl_letxprimidcntlen_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

__INLINE void ip_dfancntl_letxprimidcntlen_setf(uint8_t letxprimidcntlen)
{
    ASSERT_ERR((((uint32_t)letxprimidcntlen << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)letxprimidcntlen << 7));
}

__INLINE uint8_t ip_dfancntl_letxprimantid_getf(void)
{
    uint32_t localVal = REG_IP_RD(ip_DFANCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000007F)) >> 0);
}

__INLINE void ip_dfancntl_letxprimantid_setf(uint8_t letxprimantid)
{
    ASSERT_ERR((((uint32_t)letxprimantid << 0) & ~((uint32_t)0x0000007F)) == 0);
    REG_IP_WR(ip_DFANCNTL_ADDR, (REG_IP_RD(ip_DFANCNTL_ADDR) & ~((uint32_t)0x0000007F)) | ((uint32_t)letxprimantid << 0));
}


#endif // _REG_IPCORE_H_

