/*
 * bk24.h
 *
 *  Created on: 2019-4-1
 *      Author: guodongWang
 */

#ifndef BK24_H_
#define BK24_H_

//************************************************************//
//BK24
//************************************************************//
#define  AHB_BK24_BASE                   0x00805000
#define  bk2401_config                  *((volatile unsigned long *)   (AHB_BK24_BASE +  0*4))
#define  bk2401_enaa                    *((volatile unsigned long *)   (AHB_BK24_BASE +  1*4))
#define  bk2401_enrx                    *((volatile unsigned long *)   (AHB_BK24_BASE +  2*4))
#define  bk2401_aw                      *((volatile unsigned long *)   (AHB_BK24_BASE +  3*4))
#define  bk2401_retr                    *((volatile unsigned long *)   (AHB_BK24_BASE +  4*4))
#define  bk2401_rfch                    *((volatile unsigned long *)   (AHB_BK24_BASE +  5*4))
#define  bk2401_setup                   *((volatile unsigned long *)   (AHB_BK24_BASE +  6*4))
#define  bk2401_r0_addr_0               *((volatile unsigned long *)   (AHB_BK24_BASE +  7*4))
#define  bk2401_r0_addr_1               *((volatile unsigned long *)   (AHB_BK24_BASE +  8*4))
#define  bk2401_r0_addr_2               *((volatile unsigned long *)   (AHB_BK24_BASE +  9*4))
#define  bk2401_r0_addr_3               *((volatile unsigned long *)   (AHB_BK24_BASE + 10*4))
#define  bk2401_r0_addr_4               *((volatile unsigned long *)   (AHB_BK24_BASE + 11*4))
#define  bk2401_r1_addr_0               *((volatile unsigned long *)   (AHB_BK24_BASE + 12*4))
#define  bk2401_r1_addr_1               *((volatile unsigned long *)   (AHB_BK24_BASE + 13*4))
#define  bk2401_r1_addr_2               *((volatile unsigned long *)   (AHB_BK24_BASE + 14*4))
#define  bk2401_r1_addr_3               *((volatile unsigned long *)   (AHB_BK24_BASE + 15*4))
#define  bk2401_r1_addr_4               *((volatile unsigned long *)   (AHB_BK24_BASE + 16*4))
#define  bk2401_r2_addr                 *((volatile unsigned long *)   (AHB_BK24_BASE + 17*4))
#define  bk2401_r3_addr                 *((volatile unsigned long *)   (AHB_BK24_BASE + 18*4))
#define  bk2401_r4_addr                 *((volatile unsigned long *)   (AHB_BK24_BASE + 19*4))
#define  bk2401_r5_addr                 *((volatile unsigned long *)   (AHB_BK24_BASE + 20*4))
#define  bk2401_tx_addr_0               *((volatile unsigned long *)   (AHB_BK24_BASE + 21*4))
#define  bk2401_tx_addr_1               *((volatile unsigned long *)   (AHB_BK24_BASE + 22*4))
#define  bk2401_tx_addr_2               *((volatile unsigned long *)   (AHB_BK24_BASE + 23*4))
#define  bk2401_tx_addr_3               *((volatile unsigned long *)   (AHB_BK24_BASE + 24*4))
#define  bk2401_tx_addr_4               *((volatile unsigned long *)   (AHB_BK24_BASE + 25*4))
#define  bk2401_r0_pw                   *((volatile unsigned long *)   (AHB_BK24_BASE + 26*4))
#define  bk2401_r1_pw                   *((volatile unsigned long *)   (AHB_BK24_BASE + 27*4))
#define  bk2401_r2_pw                   *((volatile unsigned long *)   (AHB_BK24_BASE + 28*4))
#define  bk2401_r3_pw                   *((volatile unsigned long *)   (AHB_BK24_BASE + 29*4))
#define  bk2401_r4_pw                   *((volatile unsigned long *)   (AHB_BK24_BASE + 30*4))
#define  bk2401_r5_pw                   *((volatile unsigned long *)   (AHB_BK24_BASE + 31*4))
#define  bk2401_dynpd                   *((volatile unsigned long *)   (AHB_BK24_BASE + 32*4))
#define  bk2401_feature                 *((volatile unsigned long *)   (AHB_BK24_BASE + 33*4))
#define  bk2401_cfg0c_0                 *((volatile unsigned long *)   (AHB_BK24_BASE + 34*4))
#define  bk2401_cfg0c_1                 *((volatile unsigned long *)   (AHB_BK24_BASE + 35*4))
#define  bk2401_cfg0c_2                 *((volatile unsigned long *)   (AHB_BK24_BASE + 36*4))
#define  bk2401_cfg0c_3                 *((volatile unsigned long *)   (AHB_BK24_BASE + 37*4))
#define  bk2401_cfg0d_0                 *((volatile unsigned long *)   (AHB_BK24_BASE + 38*4))
#define  bk2401_cfg0d_1                 *((volatile unsigned long *)   (AHB_BK24_BASE + 39*4))
#define  bk2401_cfg0d_2                 *((volatile unsigned long *)   (AHB_BK24_BASE + 40*4))
#define  bk2401_cfg0d_3                 *((volatile unsigned long *)   (AHB_BK24_BASE + 41*4))
#define  bk2401_table_0                 *((volatile unsigned long *)   (AHB_BK24_BASE + 42*4))
#define  bk2401_table_1                 *((volatile unsigned long *)   (AHB_BK24_BASE + 43*4))
#define  bk2401_table_2                 *((volatile unsigned long *)   (AHB_BK24_BASE + 44*4))
#define  bk2401_table_3                 *((volatile unsigned long *)   (AHB_BK24_BASE + 45*4))
#define  bk2401_table_4                 *((volatile unsigned long *)   (AHB_BK24_BASE + 46*4))
#define  bk2401_table_5                 *((volatile unsigned long *)   (AHB_BK24_BASE + 47*4))
#define  bk2401_table_6                 *((volatile unsigned long *)   (AHB_BK24_BASE + 48*4))
#define  bk2401_table_7                 *((volatile unsigned long *)   (AHB_BK24_BASE + 49*4))
#define  bk2401_table_8                 *((volatile unsigned long *)   (AHB_BK24_BASE + 50*4))
#define  bk2401_table_9                 *((volatile unsigned long *)   (AHB_BK24_BASE + 51*4))
#define  bk2401_table_A                 *((volatile unsigned long *)   (AHB_BK24_BASE + 52*4))
#define  bk2401_ce                      *((volatile unsigned long *)   (AHB_BK24_BASE + 53*4))
#define  bk2401_cmd                     *((volatile unsigned long *)   (AHB_BK24_BASE + 54*4))
#define  bk2401_fifo                    *((volatile unsigned long *)   (AHB_BK24_BASE + 55*4))
#define  bk2401_sdata_0                 *((volatile unsigned long *)   (AHB_BK24_BASE + 56*4))
#define  bk2401_sdata_1                 *((volatile unsigned long *)   (AHB_BK24_BASE + 57*4))
#define  bk2401_sdata_2                 *((volatile unsigned long *)   (AHB_BK24_BASE + 58*4))
#define  bk2401_sdata_3                 *((volatile unsigned long *)   (AHB_BK24_BASE + 59*4))
#define  bk2401_sctrl                   *((volatile unsigned long *)   (AHB_BK24_BASE + 60*4))
#define  bk2401_status                  *((volatile unsigned long *)   (AHB_BK24_BASE + 64*4))
#define  bk2401_observetx               *((volatile unsigned long *)   (AHB_BK24_BASE + 65*4))
#define  bk2401_cdstatus                *((volatile unsigned long *)   (AHB_BK24_BASE + 66*4))
#define  bk2401_fifostatus              *((volatile unsigned long *)   (AHB_BK24_BASE + 67*4))
#define  bk2401_rpl_width               *((volatile unsigned long *)   (AHB_BK24_BASE + 68*4))
#define  bk2401_bt_st                   *((volatile unsigned long *)   (AHB_BK24_BASE + 69*4))
#define  bk2401_bit_cnt_0               *((volatile unsigned long *)   (AHB_BK24_BASE + 72*4))
#define  bk2401_bit_cnt_1               *((volatile unsigned long *)   (AHB_BK24_BASE + 73*4))
#define  bk2401_bit_cnt_2               *((volatile unsigned long *)   (AHB_BK24_BASE + 74*4))
#define  bk2401_bit_cnt_3               *((volatile unsigned long *)   (AHB_BK24_BASE + 75*4))
#define  bk2401_err_cnt_0               *((volatile unsigned long *)   (AHB_BK24_BASE + 76*4))
#define  bk2401_err_cnt_1               *((volatile unsigned long *)   (AHB_BK24_BASE + 77*4))
#define  bk2401_err_cnt_2               *((volatile unsigned long *)   (AHB_BK24_BASE + 78*4))
#define  bk2401_err_cnt_3               *((volatile unsigned long *)   (AHB_BK24_BASE + 79*4))


void R_RX_PAYLOAD(uint8 numb);
void W_TX_PAYLOAD(uint8 numb);
void FLUSH_TX(void);
void FLUSH_RX(void);
void REUSE_TX_PL(void);
void ACTIVE(void);
unsigned char R_RX_PL_WD(void);
void W_ACK_PAYLOAD(uint8 numb, uint8 pipe);
void W_TX_PAYLOAD_NOACK(uint8 numb);
void NOP(void);
void W_ANALOG_REG(uint8 addr, uint32 dreg);
void bk2401_initial(void);
void test_bk24(uint8* para);
void goto_sleep();


#endif /* BK24_H_ */
