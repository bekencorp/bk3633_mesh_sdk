#ifndef	RWBTDM_REGLIST
#define RWBTDM_REGLIST


#define		RWDM_BASEPTR			0x00820000
#define		RW_COMMON_REG_BASE		RWDM_BASEPTR+0x000
#define		RW_BT_REG_BASE			RWDM_BASEPTR+0x400
#define		RW_BLE_REG_BASE			RWDM_BASEPTR+0x800



//======================================================================================================
#define RW_COMMON_INT0_MSK			(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x000C)))
#define RW_COMMON_INT0_STATE		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0010)))
#define RW_COMMON_INT0_CLR			(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0014)))
	
#define RW_COMMON_INT1_MSK			(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0018)))
#define RW_COMMON_INT1_STATE		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x001C)))
#define RW_COMMON_INT1_CLR			(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0020)))	
#define RW_COMMON_ACTFIFO_STATE		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0024)))

#define RW_COMMON_DEEPSLCTRL		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0030)))
#define RW_COMMON_DEEPSLTIME		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0034)))	
#define RW_COMMON_DEEPSLDUR			(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0038)))
#define RW_COMMON_DEEPSLENPRESET	(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x003C)))	
#define RW_COMMON_FINECNTORR		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0040)))
#define RW_COMMON_CLKNCNTORR		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0044)))
#define RW_COMMON_RADIOCTRL1		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0074)))
#define RW_COMMON_TIMGENCTRL		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x00e0)))
#define	RW_COMMON_ACT_SCH_CTRL		(*((volatile unsigned long *)  (RW_COMMON_REG_BASE+ 0x0100)))


//=======================================================================================================
#define RW_BLE_CTRL					(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0000)))
#define RW_BLE_INT_MSK				(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x000C)))
#define RW_BLE_INT_STATE			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0010)))
#define RW_BLE_INT_RAW_STATE		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0014)))
#define RW_BLE_INT_CLR				(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0018)))

#define RW_BLE_CUR_RX_DESCPTOR_PTR	(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0028)))

#define RW_BLE_DIAGCTRL				(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0050)))


#define RW_BLE_RADIO_CTRL0			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0070)))
#define RW_BLE_RADIO_CTRL1			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0074)))
#define RW_BLE_RADIO_CTRL2			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0078)))
#define RW_BLE_RADIO_CTRL3			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x007C)))
#define RW_BLE_RADIO_PWRUPDN0		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0080)))
#define RW_BLE_RADIO_PWRUPDN1		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0084)))
#define RW_BLE_RADIO_PWRUPDN2		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0088)))
#define RW_BLE_RADIO_PWRUPDN3		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x008C)))
#define RW_BLE_RADIO_TXRXTIM0		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0090)))
#define RW_BLE_RADIO_TXRXTIM1		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0094)))
#define RW_BLE_RADIO_TXRXTIM2		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0098)))
#define RW_BLE_RADIO_TXRXTIM3		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x009C)))
#define RW_BLE_RADIO_SPIPTRCTRL0	(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x00A0)))
#define RW_BLE_RADIO_SPIPTRCTRL1	(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x00A4)))
#define RW_BLE_RADIO_SPIPTRCTRL2	(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x00A8)))
#define RW_BLE_RADIO_SPIPTRCTRL3	(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x00AC)))
	

#define RW_BLE_TIMEGENCTRL			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x00E0)))


#define	RW_BLE_ACT_SCH_CTRL			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0100)))

#define	RW_BLE_ENDEVT_CLKNCNT		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x010C)))
#define	RW_BLE_ENDEVT_FINECNT		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0110)))

#define RW_BLE_ADV_INTERVAL			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0120)))


#define RW_BLE_CTE_DFCTRL0_1US		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0170)))
#define RW_BLE_CTE_DFCTRL0_2US		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0174)))
#define RW_BLE_CTE_DFCTRL1_1US		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0178)))
#define RW_BLE_CTE_DFCTRL1_2US		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x017C)))	
#define RW_BLE_CTE_CURRENTPTR		(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0180)))
#define RW_BLE_CTE_ANTCTRL			(*((volatile unsigned long *)  (RW_BLE_REG_BASE+ 0x0184)))


//=======================================================================================================
//BT_REG

#define RW_BT_CTRL					(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0000)))
#define RW_BT_INT_MSK				(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x000C)))
#define RW_BT_INT_STATE				(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0010)))
#define RW_BT_INT_RAW_STATE			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0014)))
#define RW_BT_INT_CLR				(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0018)))
#define RW_BT_CUR_RX_DESCPTOR_PTR	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0028)))



#define RW_BT_RADIOCTRL2			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0078)))
#define RW_BT_RADIOCTRL3			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x007C)))

#define RW_BT_RADIOPWRPD			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x008C)))
#define RW_BT_RADIOTXRXTIME			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0090)))
#define RW_BT_TIMEGENCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x00E0)))

#define	RW_BT_SCH_CTRL				(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0100)))


#define	RW_BT_ESCO_0_CHANCTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01D0)))
#define	RW_BT_ESCO_0_MUTECTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01D4)))
#define	RW_BT_ESCO_0_CURTXPTRCTRL	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01D8)))
#define	RW_BT_ESCO_0_CURRXPTRCTRL	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01DC)))
#define	RW_BT_ESCO_0_LTCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01E0)))
#define	RW_BT_ESCO_0_TRCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01E4)))
#define	RW_BT_ESCO_0_DAYCTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01E8)))


#define	RW_BT_ESCO_1_CHANCTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01F0)))
#define	RW_BT_ESCO_1_MUTECTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01F4)))
#define	RW_BT_ESCO_1_CURTXPTRCTRL	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01F8)))
#define	RW_BT_ESCO_1_CURRXPTRCTRL	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x01FC)))
#define	RW_BT_ESCO_1_LTCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0200)))
#define	RW_BT_ESCO_1_TRCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0204)))
#define	RW_BT_ESCO_1_DAYCTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0208)))


#define	RW_BT_ESCO_2_CHANCTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0210)))
#define	RW_BT_ESCO_2_MUTECTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0214)))
#define	RW_BT_ESCO_2_CURTXPTRCTRL	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0218)))
#define	RW_BT_ESCO_2_CURRXPTRCTRL	(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x021C)))
#define	RW_BT_ESCO_2_LTCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0220)))
#define	RW_BT_ESCO_2_TRCTRL			(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0224)))
#define	RW_BT_ESCO_2_DAYCTRL		(*((volatile unsigned long *)  (RW_BT_REG_BASE+ 0x0228)))


#endif
