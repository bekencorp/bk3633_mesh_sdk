#ifndef _REG_INTC_H_
#define _REG_INTC_H_

#include <stdint.h>
#include "_reg_intc.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#if 0

#define REG_INTC_COUNT 70

#define REG_INTC_DECODING_MASK 0x000001FF

/**
 * @brief IRQSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00              IRQSTAT   0x0
 * </pre>
 */
#define INT_IRQSTATUS_ADDR   0x00820400
#define INT_IRQSTATUS_OFFSET 0x00000000
#define INT_IRQSTATUS_INDEX  0x00000000
#define INT_IRQSTATUS_RESET  0x00000000

__INLINE uint32_t int_irqstatus_get(void)
{
    return REG_INT_RD(INT_IRQSTATUS_ADDR);
}

// field definitions
#define INT_IRQSTAT_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_IRQSTAT_LSB    0
#define INT_IRQSTAT_WIDTH  ((uint32_t)0x00000020)

#define INT_IRQSTAT_RST    0x0

__INLINE uint32_t int_irqstatus_irqstat_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_IRQSTATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief IRQRAWSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           IRQRAWSTAT   0x0
 * </pre>
 */
#define INT_IRQRAWSTATUS_ADDR   0x00820404
#define INT_IRQRAWSTATUS_OFFSET 0x00000004
#define INT_IRQRAWSTATUS_INDEX  0x00000001
#define INT_IRQRAWSTATUS_RESET  0x00000000

__INLINE uint32_t int_irqrawstatus_get(void)
{
    return REG_INT_RD(INT_IRQRAWSTATUS_ADDR);
}

// field definitions
#define INT_IRQRAWSTAT_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_IRQRAWSTAT_LSB    0
#define INT_IRQRAWSTAT_WIDTH  ((uint32_t)0x00000020)

#define INT_IRQRAWSTAT_RST    0x0

__INLINE uint32_t int_irqrawstatus_irqrawstat_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_IRQRAWSTATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief IRQENABLESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         IRQENABLESET   0x0
 * </pre>
 */
#define INT_IRQENABLESET_ADDR   0x00820408
#define INT_IRQENABLESET_OFFSET 0x00000008
#define INT_IRQENABLESET_INDEX  0x00000002
#define INT_IRQENABLESET_RESET  0x00000000

__INLINE uint32_t int_irqenableset_get(void)
{
    return REG_INT_RD(INT_IRQENABLESET_ADDR);
}

__INLINE void int_irqenableset_set(uint32_t value)
{
    REG_INT_WR(INT_IRQENABLESET_ADDR, value);
}

// field definitions
#define INT_IRQENABLESET_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_IRQENABLESET_LSB    0
#define INT_IRQENABLESET_WIDTH  ((uint32_t)0x00000020)

#define INT_IRQENABLESET_RST    0x0

__INLINE uint32_t int_irqenableset_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_IRQENABLESET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void int_irqenableset_setf(uint32_t irqenableset)
{
    ASSERT_ERR((((uint32_t)irqenableset << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_INT_WR(INT_IRQENABLESET_ADDR, (uint32_t)irqenableset << 0);
}

/**
 * @brief IRQENABLECLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       IRQENABLECLEAR   0x0
 * </pre>
 */
#define INT_IRQENABLECLEAR_ADDR   0x0082040C
#define INT_IRQENABLECLEAR_OFFSET 0x0000000C
#define INT_IRQENABLECLEAR_INDEX  0x00000003
#define INT_IRQENABLECLEAR_RESET  0x00000000

__INLINE void int_irqenableclear_set(uint32_t value)
{
    REG_INT_WR(INT_IRQENABLECLEAR_ADDR, value);
}

// field definitions
#define INT_IRQENABLECLEAR_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_IRQENABLECLEAR_LSB    0
#define INT_IRQENABLECLEAR_WIDTH  ((uint32_t)0x00000020)

#define INT_IRQENABLECLEAR_RST    0x0

__INLINE void int_irqenableclear_setf(uint32_t irqenableclear)
{
    ASSERT_ERR((((uint32_t)irqenableclear << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_INT_WR(INT_IRQENABLECLEAR_ADDR, (uint32_t)irqenableclear << 0);
}

/**
 * @brief IRQSOFT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     02               IRQSFT   0
 * </pre>
 */
#define INT_IRQSOFT_ADDR   0x00820410
#define INT_IRQSOFT_OFFSET 0x00000010
#define INT_IRQSOFT_INDEX  0x00000004
#define INT_IRQSOFT_RESET  0x00000000

__INLINE uint32_t int_irqsoft_get(void)
{
    return REG_INT_RD(INT_IRQSOFT_ADDR);
}

__INLINE void int_irqsoft_set(uint32_t value)
{
    REG_INT_WR(INT_IRQSOFT_ADDR, value);
}

// field definitions
#define INT_IRQSFT_BIT    ((uint32_t)0x00000004)
#define INT_IRQSFT_POS    2

#define INT_IRQSFT_RST    0x0

__INLINE void int_irqsoft_irqsft_setf(uint8_t irqsft)
{
    ASSERT_ERR((((uint32_t)irqsft << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_INT_WR(INT_IRQSOFT_ADDR, (uint32_t)irqsft << 2);
}

/**
 * @brief IRQACK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00               IRQACK   0x0
 * </pre>
 */
#define INT_IRQACK_ADDR   0x00820414
#define INT_IRQACK_OFFSET 0x00000014
#define INT_IRQACK_INDEX  0x00000005
#define INT_IRQACK_RESET  0x00000000

__INLINE void int_irqack_set(uint32_t value)
{
    REG_INT_WR(INT_IRQACK_ADDR, value);
}

// field definitions
#define INT_IRQACK_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_IRQACK_LSB    0
#define INT_IRQACK_WIDTH  ((uint32_t)0x00000020)

#define INT_IRQACK_RST    0x0

__INLINE void int_irqack_setf(uint32_t irqack)
{
    ASSERT_ERR((((uint32_t)irqack << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_INT_WR(INT_IRQACK_ADDR, (uint32_t)irqack << 0);
}

/**
 * @brief IRQINDEX register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  04:00             IRQINDEX   0x0
 * </pre>
 */
#define INT_IRQINDEX_ADDR   0x0082041C
#define INT_IRQINDEX_OFFSET 0x0000001C
#define INT_IRQINDEX_INDEX  0x00000007
#define INT_IRQINDEX_RESET  0x00000000

__INLINE uint32_t int_irqindex_get(void)
{
    return REG_INT_RD(INT_IRQINDEX_ADDR);
}

// field definitions
#define INT_IRQINDEX_MASK   ((uint32_t)0x0000001F)
#define INT_IRQINDEX_LSB    0
#define INT_IRQINDEX_WIDTH  ((uint32_t)0x00000005)

#define INT_IRQINDEX_RST    0x0

__INLINE uint8_t int_irqindex_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_IRQINDEX_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000001F)) == 0);
    return (localVal >> 0);
}

/**
 * @brief FIQSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00              FIQSTAT   0x0
 * </pre>
 */
#define INT_FIQSTATUS_ADDR   0x00820500
#define INT_FIQSTATUS_OFFSET 0x00000100
#define INT_FIQSTATUS_INDEX  0x00000040
#define INT_FIQSTATUS_RESET  0x00000000

__INLINE uint32_t int_fiqstatus_get(void)
{
    return REG_INT_RD(INT_FIQSTATUS_ADDR);
}

// field definitions
#define INT_FIQSTAT_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_FIQSTAT_LSB    0
#define INT_FIQSTAT_WIDTH  ((uint32_t)0x00000020)

#define INT_FIQSTAT_RST    0x0

__INLINE uint32_t int_fiqstatus_fiqstat_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_FIQSTATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief FIQRAWSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00           FIQRAWSTAT   0x0
 * </pre>
 */
#define INT_FIQRAWSTATUS_ADDR   0x00820504
#define INT_FIQRAWSTATUS_OFFSET 0x00000104
#define INT_FIQRAWSTATUS_INDEX  0x00000041
#define INT_FIQRAWSTATUS_RESET  0x00000000

__INLINE uint32_t int_fiqrawstatus_get(void)
{
    return REG_INT_RD(INT_FIQRAWSTATUS_ADDR);
}

// field definitions
#define INT_FIQRAWSTAT_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_FIQRAWSTAT_LSB    0
#define INT_FIQRAWSTAT_WIDTH  ((uint32_t)0x00000020)

#define INT_FIQRAWSTAT_RST    0x0

__INLINE uint32_t int_fiqrawstatus_fiqrawstat_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_FIQRAWSTATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief FIQENABLESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00         FIQENABLESET   0x0
 * </pre>
 */
#define INT_FIQENABLESET_ADDR   0x00820508
#define INT_FIQENABLESET_OFFSET 0x00000108
#define INT_FIQENABLESET_INDEX  0x00000042
#define INT_FIQENABLESET_RESET  0x00000000

__INLINE uint32_t int_fiqenableset_get(void)
{
    return REG_INT_RD(INT_FIQENABLESET_ADDR);
}

__INLINE void int_fiqenableset_set(uint32_t value)
{
    REG_INT_WR(INT_FIQENABLESET_ADDR, value);
}

// field definitions
#define INT_FIQENABLESET_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_FIQENABLESET_LSB    0
#define INT_FIQENABLESET_WIDTH  ((uint32_t)0x00000020)

#define INT_FIQENABLESET_RST    0x0

__INLINE uint32_t int_fiqenableset_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_FIQENABLESET_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0xFFFFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void int_fiqenableset_setf(uint32_t fiqenableset)
{
    ASSERT_ERR((((uint32_t)fiqenableset << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_INT_WR(INT_FIQENABLESET_ADDR, (uint32_t)fiqenableset << 0);
}

/**
 * @brief FIQENABLECLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00       FIQENABLECLEAR   0x0
 * </pre>
 */
#define INT_FIQENABLECLEAR_ADDR   0x0082050C
#define INT_FIQENABLECLEAR_OFFSET 0x0000010C
#define INT_FIQENABLECLEAR_INDEX  0x00000043
#define INT_FIQENABLECLEAR_RESET  0x00000000

__INLINE void int_fiqenableclear_set(uint32_t value)
{
    REG_INT_WR(INT_FIQENABLECLEAR_ADDR, value);
}

// field definitions
#define INT_FIQENABLECLEAR_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_FIQENABLECLEAR_LSB    0
#define INT_FIQENABLECLEAR_WIDTH  ((uint32_t)0x00000020)

#define INT_FIQENABLECLEAR_RST    0x0

__INLINE void int_fiqenableclear_setf(uint32_t fiqenableclear)
{
    ASSERT_ERR((((uint32_t)fiqenableclear << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_INT_WR(INT_FIQENABLECLEAR_ADDR, (uint32_t)fiqenableclear << 0);
}

/**
 * @brief FIQACK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00               FIQACK   0x0
 * </pre>
 */
#define INT_FIQACK_ADDR   0x00820510
#define INT_FIQACK_OFFSET 0x00000110
#define INT_FIQACK_INDEX  0x00000044
#define INT_FIQACK_RESET  0x00000000

__INLINE void int_fiqack_set(uint32_t value)
{
    REG_INT_WR(INT_FIQACK_ADDR, value);
}

// field definitions
#define INT_FIQACK_MASK   ((uint32_t)0xFFFFFFFF)
#define INT_FIQACK_LSB    0
#define INT_FIQACK_WIDTH  ((uint32_t)0x00000020)

#define INT_FIQACK_RST    0x0

__INLINE void int_fiqack_setf(uint32_t fiqack)
{
    ASSERT_ERR((((uint32_t)fiqack << 0) & ~((uint32_t)0xFFFFFFFF)) == 0);
    REG_INT_WR(INT_FIQACK_ADDR, (uint32_t)fiqack << 0);
}

/**
 * @brief FIQINDEX register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  04:00             FIQINDEX   0x0
 * </pre>
 */
#define INT_FIQINDEX_ADDR   0x00820514
#define INT_FIQINDEX_OFFSET 0x00000114
#define INT_FIQINDEX_INDEX  0x00000045
#define INT_FIQINDEX_RESET  0x00000000

__INLINE uint32_t int_fiqindex_get(void)
{
    return REG_INT_RD(INT_FIQINDEX_ADDR);
}

__INLINE void int_fiqindex_set(uint32_t value)
{
    REG_INT_WR(INT_FIQINDEX_ADDR, value);
}

// field definitions
#define INT_FIQINDEX_MASK   ((uint32_t)0x0000001F)
#define INT_FIQINDEX_LSB    0
#define INT_FIQINDEX_WIDTH  ((uint32_t)0x00000005)

#define INT_FIQINDEX_RST    0x0

__INLINE uint8_t int_fiqindex_getf(void)
{
    uint32_t localVal = REG_INT_RD(INT_FIQINDEX_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x0000001F)) == 0);
    return (localVal >> 0);
}
#endif

#if 1 //beken used
//#include <stdint.h>
//#include "_reg_intc.h"
//#include "compiler.h"
//#include "arch.h"
//#include "reg_access.h"

#define REG_INTC_COUNT 19


/**
 * @brief INTSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00              INTSTAT   0x0
 * </pre>
 */
#define INTC_INTSTATUS_ADDR   0x00800048

__INLINE uint32_t intc_status_get(void)
{
    return REG_PL_RD(INTC_INTSTATUS_ADDR);
}

__INLINE void intc_status_clear(uint32_t status)
{
    REG_PL_WR(INTC_INTSTATUS_ADDR,status);
}




/**
 * @brief IRQENABLESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00         								   0x0
 * </pre>
 */
#define INTC_FIQIRQENABLE_ADDR   0x00800044


__INLINE void intc_enable_set(uint32_t value)
{
    uint32_t reg;
    reg = REG_PL_RD(INTC_FIQIRQENABLE_ADDR);
    reg |= value;
    REG_PL_WR(INTC_FIQIRQENABLE_ADDR, reg);
}

__INLINE uint32_t intc_enable_get(void)
{
    return REG_PL_RD(INTC_FIQIRQENABLE_ADDR);
}



/**
 * @brief IRQENABLECLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00        									   0x0
 * </pre>
 */

__INLINE void intc_enable_clear(uint32_t value)
{  
    uint32_t reg;
    reg = REG_PL_RD(INTC_FIQIRQENABLE_ADDR);
    reg &= ~value  ;
    REG_PL_WR(INTC_FIQIRQENABLE_ADDR, reg);
}


/**
 * @brief IRQENABLESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00         							   0x0
 * </pre>
 */
#define INTC_MODULEENABLE_ADDR   0x00800040


__INLINE void intc_module_enable_set(uint32_t value)
{
    uint32_t reg;
    reg = REG_PL_RD(INTC_MODULEENABLE_ADDR);
    reg |= value;
    REG_PL_WR(INTC_MODULEENABLE_ADDR, reg);
}

__INLINE uint32_t intc_module_enable_get(void)
{
    return REG_PL_RD(INTC_MODULEENABLE_ADDR);
}



/**
 * @brief  register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00       				   0x0
 * </pre>
 */

__INLINE void intc_module_enable_clear(uint32_t value)
{
   
    uint32_t reg;
    reg = REG_PL_RD(INTC_MODULEENABLE_ADDR);
    reg &= ~value ;
    REG_PL_WR(INTC_MODULEENABLE_ADDR, reg);
}
#endif//#if 1

#endif // _REG_INTC_H_

