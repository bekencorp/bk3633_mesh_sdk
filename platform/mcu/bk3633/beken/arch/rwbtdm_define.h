#ifndef  RW_DM_TOP
#define  RW_DM_TOP

#include "uart2.h"


/*
//----MEM---MAP
0x00814000 + offset
//ET
	00-0F	ET_0
	10-1F	ET_1
    ...
	F0-FF	ET_15

//Freqency Table
	


//CS	
	100-1FF:	CS_0
	200-2FF:	CS_1
	...
	E00-EFF:	CS_13
	F00-F7F:	CS_14     //???
	F80-FFF:	CS_15	  //???
	
//0_descptor  (Tx_descptor & Sub_txdescptor ;  RX_descptor & Sub_rxdescptor ; )

	//BLE
	1000-11FF:	TX_descptor0 + txbuf[...] + Sub_descptor0 + txbuf[...] + Sub ....
	1200-13FF:	RX_descptor0 + rxbuf[...] + Sub_descptor0 + rxbuf[...] + Sub ....
		|
		|-------TX_descptor0:					(0x0-0x0F)	<---------
		|-------TX_descptor0_tdatabuf	(0x10-0x5F)			     |
		|-------TX_descptor1:					(0x60-0x6F)			     |
		|-------TX_descptor1_tdatabuf	(0x70-0xBF)			     |
		|-------TX_descptor2:					(0xC0-0xCF)			     |
		|-------TX_descptor2_tdatabuf	(0xD0-0x11F)         |
		|-------TX_descptor3:					(0x120-0x12F)        |
		|-------TX_descptor3_tdatabuf	(0x130-0x17F)    		 |
		|-------TX_descptor4:					(0x180-0x18F)		     |
		|-------TX_descptor4_tdatabuf	(0x190-0x1DF)-------->
		|
		|-------RX_descptor0:					(0x200-0x21F) <-------|
		|-------RX_descptor0_tdatabuf	(0x220-0x26F)		      |
		|-------RX_descptor1:					(0x270-0x28F)		      |
		|-------RX_descptor1_tdatabuf	(0x290-0x2DF)		      |
		|-------RX_descptor2:					(0x2E0-0x2FF)		      |
		|-------RX_descptor2_tdatabuf	(0x300-0x34F)		      |
		|-------RX_descptor3:					(0x350-0x39F)		      |
		|-------RX_descptor3_tdatabuf	(0x3A0-0x3EF)--------->

	
	
	1400-15FF:	TX_descptor1 + txbuf[...] + Sub_descptor1 + txbuf[...] + Sub ....
	1600-17FF:	RX_descptor1 + rxbuf[...] + Sub_descptor1 + rxbuf[...] + Sub ....




	.... (now, will be only use three ET-CS-Descptor)
	


//other	
  3000:
	
*/




//======================================
#define   EXC_MEM_START			0x00810000
#define   EXC_MEM_LEN			0x4000
//======================================



//======================================
//exchange table  param
#define	  ET_START			EXC_MEM_START+0
#define	  ET_LEN				0x10
#define	  ET_0					0
#define	  ET_1					1
#define	  ET_2					2
#define	  ET_3					3
#define	  ET_4					4
#define	  ET_5					5
#define	  ET_6					6
#define	  ET_7					7
#define	  ET_8					8
#define	  ET_9					9
#define	  ET_10					10
#define	  ET_11					11
#define	  ET_12					12
#define	  ET_13					13
#define	  ET_14					14
#define	  ET_15					15

#define	  BLE_MODE				2
#define   BR_EDR_MODE			1

#define   ET_ready4procee		0
#define   ET_wait4start			1
#define   ET_working			  2
#define   ET_normal_end			3
#define   ET_before_end			4
#define   ET_after_end			5
#define   ET_skipped			  6

#define	  ET_ISO_FLASE			0
#define	  ET_RSVD_FLASE     0
#define	  RSVD_SLOT				  1

#define	  ET_SCO				1
#define	  ET_ACL				0



typedef struct {
	uint8	mode ;
	uint8	status;
	uint8	iso;
	uint8	rsvd;
	uint8	ae_nps;	
	uint8	spa;	
	uint8	prio1;
	uint32	raw_stp;
	uint16	fine_stp;
	uint16	cs_ptr;
	uint16	prio1_time;
	uint8	prio1_time_uint;
	uint8	prio2;
	uint8	prio3;
	uint8	pti_prio;	
} ET_struct ;



typedef struct {
	uint8	mode ;
	uint8	status;
	uint8	esco;
	uint8	rsvd;
	uint8	sniff;
	uint8	csb;
	uint8	spa;
	uint8	prio1;
	uint32	raw_stp;
	uint16	fine_stp;
	uint16	cs_ptr;
	uint16	prio1_time;
	uint8	prio1_time_uint;
	uint8	prio2;
	uint8	prio3;
	uint8	pti_prio;
	uint8	vchan;
} BT_ET_struct ;



//======================================


#define 		MASTER_CONNECT			    2  
#define			SLAVE_CONNECT				3
#define			LD_ADVERTISER				4
#define			HD_ADVERTISER				5
#define			EXT_ADVERTISER		        6
#define			PASSIVE_SCANNER		        8
#define			ACTIVE_SCANNER		        9
#define			EXT_PASSIVE_SCANNER	        10
#define			EXT_ACTIVE_SCANNER	        11
#define			INITIATOR				    14
#define 		EXT_INITIATOR				15
#define			TX_TEST					    28
#define 		RX_TEST						29
#define 		TRX_TEST					30



#define			MASTER_CONNECT		      2
#define			SLAVE_CONNECT		      3
#define			PAGE				      4
#define			PAGE_SCAN			      5




#define			BD_ADDR_IS_PRIVACY	  1
#define			BD_ADDR_IS_PUBLIC	  0

#define			AES_CCM_WITH_MIC	  0
#define			AES_CCM_WITHOUT_MIC	  1

#define			RATE_1M		0
#define			RATE_2M		1
#define			RATE_125k	2
#define			RATE_500k	3

// MY_BD_ADDR = 0xDE0102030405
#define			MY_BD_ADDR_H	0xDE01
#define			MY_BD_ADDR_M	0x0203
#define			MY_BD_ADDR_L	0x0405


#define			NON_ISOTXDESCPTOR	0  
#define 		NON_ISORXDESCPTOR	0
#define 		NON_ACLTXDESCPTOR	0

#define			NON_NXT_TXDESCPTOR	0
#define			NON_NXT_RXDESCPTOR	0

#define			ADV_IND				0
#define			ADV_DIRECT_IND		1
#define			ADV_NONCONN_IND		2
#define			SCAN_REQ			3
#define			AUX_SCAN_REQ		3
#define			SCAN_RSP			4
#define			CONNECT_IND			5
#define			AUX_CONNECT_REQ		5
#define			ADV_SCAN_IND		6
#define			ADV_EXT_IND			7
#define			AUX_ADV_IND			7
#define			AUX_SCAN_RSP		7
#define			AUX_CHAIN_IND		7
#define			AUX_SYNC_IND		7
#define			AUX_CONNECT_RSP		8

#define			NON_TX_DATA_BUF		0


#define	  CS_START				EXC_MEM_START+0x100
#define	  CS_LEN				0x100

#define	  ET0_CS_STA_ADDR			0x100
#define	  ET1_CS_STA_ADDR			0x200
#define	  ET2_CS_STA_ADDR			0x300
#define	  ET3_CS_STA_ADDR			0x400
#define	  ET4_CS_STA_ADDR			0x500
#define	  ET5_CS_STA_ADDR			0x600

//==================================================

typedef struct {
	uint8   connected;
	uint32  rxtype; 
	uint16	lldata[11];
	uint8		packet_num;
	uint32  rxdatabuf0[20];
	uint32  rxdatabuf1[20];
	uint32  rxdatabuf2[20];
	uint32  rxdatabuf3[20];	
} BLE_connect_req_rx;  



typedef		struct {
	 uint8  is_connected;
	 uint8	Access_Addr[4] ;
	 uint8	Crc_Init[3];
	 uint8  WinSize[1];
	 uint8  WinOffset[2];
	 uint8  Interval[2];
	 uint8  Latency[2];
	 uint8  Timeout[2];
	 uint8  Chmap[5];
	 uint8  Hop_Sca[1];
	
	 uint32	connect_clkncnt;	
	 uint32	connect_finecnt;
	
}  RWBLE_CONNECT_IND_INFO ;




typedef	 struct {
			uint32		clkncnt ;
		  uint32    finecnt ;
	    uint16    Access_Addr_high16  ;
	    uint16    Access_Addr_low16  ;
	    uint16    Access_Crc_high8  ;
	    uint16    Access_Crc_low16  ;	
	    uint16    Interval     ;		
	    uint16    Lattency     ;		
	    uint16    Timeout      ;	
	    uint16    ChanMap_H8   ;	
	    uint16    ChanMap_M    ;	
	    uint16    ChanMap_L    ;	
	    uint16    HOP_SCA      ;		
	

}  RWBLE_NEW_ET_PARAM ;









typedef struct {

	struct {
		unsigned int  format		: 5;
		unsigned int  rfu_3			: 3;
		unsigned int  dnabort		: 1;
		unsigned int  rxbsyen		: 1;
		unsigned int  txbsyen		: 1;
		unsigned int  rfu_5			: 5;
	}	CS_00 ;

	struct {
		unsigned int  priv_npub		  : 1;
		unsigned int  rxcryten		  : 1;
		unsigned int  txcryten		  : 1;
		unsigned int  crytmode		  : 1;
		unsigned int  micmode		  : 1;
		unsigned int  nullrxllidflt	  : 1;
		unsigned int  sas         	  : 1;
		unsigned int  rfu_1			  : 1;
		unsigned int  linkble		  : 5;		
		unsigned int  rfu_3			  : 3;
	}	CS_02 ;
	
	
	struct {
		unsigned int  isotype			  : 2;
		unsigned int  isosyncen			  : 1;
		unsigned int  isosyncmode		  : 1;
		unsigned int  rfu4				  : 4;
		unsigned int  grouplab		      : 2;
		unsigned int  steamlab		      : 6;
	}	CS_04 ;
	
	
	struct {
		unsigned int  txrate			  : 2;
		unsigned int  rxrate			  : 2;
		unsigned int  auxrate			  : 2;
		unsigned int  rfu2				  : 2;
		unsigned int  txthreshold		  : 4;
		unsigned int  rxthreshold		  : 4;
	}	CS_06 ;

	//08
	uint16	bdaddr_l;
	//0A
	uint16	bdaddr_m;
	//0C
	uint16	bdaddr_h;
	//0E
	uint16	syncword_l;
	//10
	uint16	syncword_h;
	//12
	uint16	crc_initial_low16;
	//14
	struct {
		unsigned int  rxmaxctebuf			: 8;
		unsigned int  crc_initial_high8		: 8;
	}	CS_14 ;	
	

	struct {
		unsigned int  ral_en				: 1;
		unsigned int  ral_mode				: 1;
		unsigned int  local_rpa_sel			: 1;
		unsigned int  periodic_fliter_en 	: 1;
		unsigned int  rfu_4					: 4;
		unsigned int  filter_policy			: 8;
	}	CS_16 ;


	struct {
		unsigned int  ch_indx				: 6;
		unsigned int  rfu_2					: 2;
		unsigned int  hop_intval			: 5;
		unsigned int  hop_mode 				: 2;
		unsigned int  hop_enable			: 1;
	}	CS_18 ;

	struct {
		unsigned int  txpwr						: 8;
		unsigned int  ext_pa_en				    : 1;
		unsigned int  ends_on_scondary_chan		: 1;
		unsigned int  rx_mafs_err 				: 1;
		unsigned int  rx_mic_err				: 1;
		unsigned int  nesn						: 1;
		unsigned int  sn						: 1;
		unsigned int  lastempty					: 1;
		unsigned int  rxbuffull					: 1;
	}	CS_1A ;
	
	struct {
		unsigned int  dfen						: 1;
		unsigned int  dffilteren				: 1;
		unsigned int  dftype		            : 2;
		unsigned int  dfsampcnt 				: 2;
		unsigned int  dfswctrl				    : 2;
		unsigned int  dfrespen				    : 1;
		unsigned int  rfu_2						: 7;
	}	CS_1C ;	
	

	struct {
		unsigned int  rx_wind_size						: 15;
		unsigned int  rx_wind_wide						: 1;
	}	CS_1E ;

	//20
	uint16	isotxdesptr;
	//22
	uint16	isorxdesptr;	
	//24
	uint16	acltxdesptr;	
	
	struct {
		unsigned int  ana_pattern_lenth					: 7;
		unsigned int  rfu1						        : 1;
		unsigned int  maxsampcte					    : 5;
	    unsigned int  rfu2						        : 2;
		unsigned int  dfrsp       						: 1;		
	}	CS_26 ;	
	
	struct {
		unsigned int  ana_pattern_desptr			    : 14;
		unsigned int  rfu1						        : 2;	
	}	CS_28 ;		
	
	union	{
	uint16	windoffset;
	uint16	min_evnt_time;
	} CS_2A;

	//2C
	uint16	max_evnt_time;


	union	{
	uint16	conn_interval;
	uint16	llchmap_low16;
	} CS_2E;

	//30
	uint16	llchmap_middle16;

	struct {
		unsigned int  llchmap_high5				: 5;
		unsigned int  adv_chmap					: 3;
		unsigned int  rfu_2						: 2;
		unsigned int  chan_aux				    : 6;
	}	CS_32 ;

	//34
	struct {
		unsigned int  isorxmaxbuf				: 8;
		unsigned int  aclrxmaxbuf				: 8;
	}	CS_34 ;	

	//36
	uint16	rxmax_time;

	union	{
	uint16	sk0;
	uint16	adv_bdaddr0;
	uint16	peer_rpa_ptr;
	} CS_38;

	union	{
	uint16	sk1;
	uint16	adv_bdaddr1;
	} CS_3A;

	union	{
	uint16	sk2;
	uint16	adv_bdaddr2;
	} CS_3C;


	union	{
	uint16	sk3;
	uint16	adv_bdaddr3;
	} CS_3E;

	union	{
	uint16	sk4;
	uint16	auxdesptr;
	} CS_40;


	union	{
	uint16	sk5;
	uint16	windoffset_2M;
	} CS_42;

	union	{
	uint16	sk6;
	uint16	conn_interval_2M;
	} CS_44;

	union	{
	uint16	sk7;
	uint16	windoffset_LR;
	} CS_46;

	union	{
	uint16	iv0;
	uint16	conn_interval_LR;
	} CS_48;

	//4A
	uint16	iv1;

	//4C
	uint16	iv2;

	//4E
	uint16	iv3;

	union	{
	uint16	tx_windoffset;
	uint16	txccmpktcnt0;
	} CS_50;

	//52
	union	{
	uint16	txccmpktcnt1;
	uint16	ext_adv_state;
	} CS_52;

	//54
	uint16	txccmpktcnt2;

	//56
	uint16	rxccmpktcnt0;

	//58
	uint16	rxccmpktcnt1;

	//5A
	uint16	rxccmpktcnt2;

	//5c
	uint16	evnt_cnt;
	
	//5E
	uint16	evnt_cnt_offset_low16;	

	//60
	uint16	evnt_cnt_offset_middle16;
	
	//62
	uint16	evnt_cnt_offset_high16;	
	
	//64
	uint16	isoevnt_ctrl;

	//66
	uint16	isotxrxctrl;

	struct {
		unsigned int  acl_txdesp_cnt			: 8;
		unsigned int  acl_rxdesp_cnt			: 8;
	}	CS_68 ;

	struct {
		unsigned int  iso_txpkgcnt			: 8;
		unsigned int  iso_rxpkgcnt			: 8;
	}	CS_6A ;

} CS_struct;








/*
   used for BLE5.1

typedef struct {
	//00
	uint8	format	;
	uint8	dn_abort;		// when coexistence used
	uint8	txbsy_en;
	uint8	rxbsy_en;	
	
	//02	
	uint8	privacy_npublic	;	//BLE Device_Address type  
	uint8	rx_encpty_en ;
	uint8	tx_encpty_en ;
	uint8	crypt_mode;	
	uint8	mic_mode;
	uint8	null_rxllid_field ;  //!!!!!!!!!!!!!!!
	uint8	sas;				//stop rx after rx syncword,when syncword err????
	uint8	linklable;			//!!!!!!!!!!!!
	
	//04	
	uint8	iso_type;	//not used 
	uint8	iso_sync_en;	//not used 
	uint8	iso_sync_mode;	  //not used 
	uint8	group_label;	//not used
	uint8	stream_label;	//not used
	
	//06	
	uint8	txrate;
	uint8	rxrate;
	uint8	auxrate;
	uint8	txthreshold;
	uint8	rxthreshold; 

	//08
	uint16	bdaddr_l;
	//0A
	uint16	bdaddr_m;
	//0C
	uint16	bdaddr_h;
	//0E
	uint16	syncword_l;
	//10
	uint16	syncword_h;
	//12
	uint16	crc_initial_low16;
	
	//14
	uint8   crc_initial_high8;
	uint8	rxcte_maxbuf; //AOA\DOA will use
	
	//16
	uint8	ral_en;
	uint8	ral_mode;
	uint8	local_rpa_sel;
	uint8	periodic_fliter_en;
	uint8	filter_policy;
	
	//18
	uint8	ch_indx;
	uint8	hop_intval;
	uint8	hop_mode;
	uint8	hop_enable;
	
	//1A
	uint8	txpwr;
	uint8	ext_pa_open;
	uint8	ends_on_scondary_chan;
	uint8	rx_mafs_err;
	uint8	rx_mic_err;
	uint8	nesn;
	uint8	sn;
	uint8	lastempty;
	uint8	rxbuffull;

	//1C
	uint8	df_en;
	uint8	df_filteren;
	uint8   df_type;
	uint8	df_samp_ctrl;
	uint8	df_swch_ctrl;
	uint8	df_rsp_en;
	
	//1E
	uint16	rx_wind_size;
	uint8	rx_wind_wide;
	
	//20
	uint16	iso_txdesptr;
	//22
	uint16	iso_rxdesptr;
	//24	
	uint16	acl_txdesptr;	
	
	//26
	uint8	antenna_pattern_len;
	uint8	max_cte_samp;
	uint8	df_rsp;
	
	//28
	uint16	antenna_id_ptr;

	//2A  -- union
	union	{
	uint16	windoffset;
	uint16	min_evnt_time;
	} CS_2A;
	
	//2C
	uint16	max_evnt_time;
	
	//2E
	union	{
	uint16	conn_interval;
	uint16	llchmap_low16;
	} CS_2E;	
	
	//30
	uint16	llchmap_middle16;
	
	//32
	uint8	llchmap_high5;
	uint8	adv_chmap;
	uint8	chan_aux;
	
	//34
	uint8	acl_rxmaxbuf;
	uint8	iso_rxmaxbuf;	
	
	//36
	uint16	rxmax_time;	
	
	
	//38
	union	{
	uint16	sk0;
	uint16	adv_bdaddr0;
	uint16	peer_rpa_ptr;
	} CS_38;
	
	//3A
	union	{
	uint16	sk1;
	uint16	adv_bdaddr1;
	} CS_3A;

	//3C
	union	{
	uint16	sk2;
	uint16	adv_bdaddr2;
	} CS_3C;
	
	//3E
	union	{
	uint16	sk3;
	uint16	adv_bdaddr3;
	} CS_3E;	

	//40
	union	{
	uint16	sk4;
	uint16	auxdesptr;
	} CS_40;	

	//42
	union	{
	uint16	sk5;
	uint16	windoffset_2M;
	} CS_42;

	//44
	union	{
	uint16	sk6;
	uint16	conn_interval_2M;
	} CS_44;	
	
	//46
	union	{
	uint16	sk7;
	uint16	windoffset_LR;
	} CS_46;
	
	//48
	union	{
	uint16	iv0;
	uint16	conn_interval_LR;
	} CS_48;

	//4A
	uint16	iv1;

	//4C
	uint16	iv2;

	//4E
	uint16	iv3;	

	//50
	union	{
	uint16	tx_windoffset;
	uint16	txccmpktcnt0;
	} CS_50;

	//52
	union	{
	uint16	txccmpktcnt1;
	uint16	ext_adv_state;
	} CS_52;

	//54
	uint16	txccmpktcnt2;
	
	//56
	uint16	rxccmpktcnt0;	
	
	//58
	uint16	rxccmpktcnt1;	

	//5A
	uint16	rxccmpktcnt2;
	
	//5C
	uint16	evnt_cnt;
	
	//5E
	uint16	evnt_cnt_offset0;

	//60
	uint16	evnt_cnt_offset1;

	//62
	uint16	evnt_cnt_offset2;

	//64
	uint16	iso_evnt_ctrl;

	//66
	uint8	iso_last_sn;
	uint8	iso_last_nesn;
	uint8	iso_last_md;
	uint8	iso_wait_ack;
	uint8	iso_rsvd;
	uint8	iso_retx;
	uint8	iso_sn;
	uint8	iso_nesn;
	uint8	iso_md;
	uint8	iso_last_empty;
	uint8	iso_rxbuff_full;
	
	//68
	uint8	acl_txdesp_cnt;
	uint8	acl_rxdesp_cnt;
	
	//6A
	uint8	iso_txpkgcnt;
	uint8	iso_rxpkgcnt;	
	
	
} CS_struct ;

*/





//===================================

#define		TXDESCPTR_SIZE				0x10
#define		TXDESCPTR_BUF_SIZE			0x50
#define		RXDESCPTR_SIZE				0x20
#define		RXDESCPTR_BUF_SIZE			0x50


#define	  ANTENNA0_ID_PTR			(0x00A0)
#define	  ANTENNA1_ID_PTR			(0x00A8)

#define		ET0_CS_TXDESCPTR0_STA_ADDR		    (0x0000+0x1000)
#define		ET0_CS_TXDESCPTR0_TBUF_STA_ADDR		(0x0000+0x1010)
#define		ET0_CS_TXDESCPTR1_STA_ADDR			(0x0000+0x1060)
#define		ET0_CS_TXDESCPTR1_TBUF_STA_ADDR		(0x0000+0x1070)
#define		ET0_CS_TXDESCPTR2_STA_ADDR			(0x0000+0x10C0)
#define		ET0_CS_TXDESCPTR2_TBUF_STA_ADDR		(0x0000+0x10D0)
#define		ET0_CS_TXDESCPTR3_STA_ADDR			(0x0000+0x1120)
#define		ET0_CS_TXDESCPTR3_TBUF_STA_ADDR		(0x0000+0x1130)
#define		ET0_CS_TXDESCPTR4_STA_ADDR			(0x0000+0x1180)
#define		ET0_CS_TXDESCPTR4_TBUF_STA_ADDR		(0x0000+0x1190)

#define		ET0_CS_RXDESCPTR0_STA_ADDR			(0x0000+0x1200)
#define		ET0_CS_RXDESCPTR0_TBUF_STA_ADDR		(0x0000+0x1220)
#define		ET0_CS_RXDESCPTR1_STA_ADDR			(0x0000+0x1270)
#define		ET0_CS_RXDESCPTR1_TBUF_STA_ADDR		(0x0000+0x1290)
#define		ET0_CS_RXDESCPTR2_STA_ADDR			(0x0000+0x12E0)
#define		ET0_CS_RXDESCPTR2_TBUF_STA_ADDR		(0x0000+0x1300)
#define		ET0_CS_RXDESCPTR3_STA_ADDR			(0x0000+0x1350)
#define		ET0_CS_RXDESCPTR3_TBUF_STA_ADDR		(0x0000+0x1370)


#define		ET1_CS_TXDESCPTR0_STA_ADDR			(0x0000+0x1000+0x400)
#define		ET1_CS_TXDESCPTR0_TBUF_STA_ADDR		(0x0000+0x1010+0x400)
#define		ET1_CS_TXDESCPTR1_STA_ADDR			(0x0000+0x1060+0x400)
#define		ET1_CS_TXDESCPTR1_TBUF_STA_ADDR		(0x0000+0x1070+0x400)
#define		ET1_CS_TXDESCPTR2_STA_ADDR			(0x0000+0x10C0+0x400)
#define		ET1_CS_TXDESCPTR2_TBUF_STA_ADDR		(0x0000+0x10D0+0x400)
#define		ET1_CS_TXDESCPTR3_STA_ADDR			(0x0000+0x1120+0x400)
#define		ET1_CS_TXDESCPTR3_TBUF_STA_ADDR		(0x0000+0x1130+0x400)
#define		ET1_CS_TXDESCPTR4_STA_ADDR			(0x0000+0x1180+0x400)
#define		ET1_CS_TXDESCPTR4_TBUF_STA_ADDR		(0x0000+0x1190+0x400)

#define		ET1_CS_RXDESCPTR0_STA_ADDR			(0x0000+0x1200+0x400)
#define		ET1_CS_RXDESCPTR0_TBUF_STA_ADDR		(0x0000+0x1220+0x400)
#define		ET1_CS_RXDESCPTR1_STA_ADDR			(0x0000+0x1270+0x400)
#define		ET1_CS_RXDESCPTR1_TBUF_STA_ADDR		(0x0000+0x1290+0x400)
#define		ET1_CS_RXDESCPTR2_STA_ADDR			(0x0000+0x12E0+0x400)
#define		ET1_CS_RXDESCPTR2_TBUF_STA_ADDR		(0x0000+0x1300+0x400)
#define		ET1_CS_RXDESCPTR3_STA_ADDR			(0x0000+0x1350+0x400)
#define		ET1_CS_RXDESCPTR3_TBUF_STA_ADDR		(0x0000+0x1370+0x400)







//.....


//=================================================================



typedef	struct {
	//00
	uint8	tx_done;
	uint16	nxt_txdescptor_ptr;
	
	//02
	union	{
		struct {
			unsigned int  txllid	: 2;
			unsigned int  txnesn	: 1;
			unsigned int  txsn		: 1;
			unsigned int  txmd		: 1;
			unsigned int  txcp		: 1;	
			unsigned int  txaclrfu	: 2;
			unsigned int  txlen		: 8;				
		}	TX_DATA_header ;
		
		struct {
			unsigned int  txtype	: 4;
			unsigned int  txadvrfu	: 1;
			unsigned int  txchsel	: 1;
			unsigned int  txtxadd	: 1;
			unsigned int  txrxadd	: 1;	
			unsigned int  txlen		: 8;				
		}	TX_ADV_header ;		
		
	} TX_02;
	
	//04
	uint16	txdata_ptr;
	
	//06
	struct {
			unsigned int  txaelenth	: 6;
			unsigned int  txaemode	: 2;
			unsigned int  txadva	: 1;
			unsigned int  txtarga	: 1;
			unsigned int  txcte		: 1;	
			unsigned int  txadi		: 1;
			unsigned int  txauxptr	: 1;	
			unsigned int  txsync	: 1;
			unsigned int  txpow		: 1;	
			unsigned int  txrsvd	: 1;		
		}	TX_EXT_ADV_header ;

	
	//08
	uint8	auxoffset_low8;
	uint8	auxoffset_uint;
	uint8	aux_ca;
	uint8	tx_llchmap;
		
	//0A
	uint8	auxoffset_high5;	
	uint8	auxphy;		
	
	//0C
	uint16	ext_txdata_ptr;
	
	//0E
	uint8	txctetime;
	uint8	txcterfu;
	uint8	txctetype;

} TX_desptor ;





//===================================

typedef struct	{
	
	//00
	uint8	rx_done;
	uint16	nxt_rxdescptor_ptr;	
	
	//02
	union	{
		struct {
			unsigned int  sync_err		: 1;
			unsigned int  rxtime_err	: 1;
			unsigned int  len_err		: 1;
			unsigned int  crc_err		: 1;
			unsigned int  mic_err		: 1;	
			unsigned int  llid_err		: 1;
			unsigned int  sn_err		: 1;
			unsigned int  nesn_err		: 1;
			unsigned int  rfu			: 7;
			unsigned int  rxcte_err		: 1;			
		}	RX_state_CE ;
		
		struct {
			unsigned int  sync_err		: 1;
			unsigned int  rxtime_err	: 1;
			unsigned int  len_err		: 1;
			unsigned int  crc_err		: 1;
			unsigned int  priv_err		: 1;	
			unsigned int  type_err		: 1;
			unsigned int  bdaddr_match		: 1;
			unsigned int  peer_addr_match	: 1;	
			unsigned int  in_periodic_list	: 1;	
			unsigned int  in_white_list		: 1;
			unsigned int  device_filter_ok	: 1;
			unsigned int  advmode_err		: 1;
			unsigned int  follow_auxptr		: 1;
			unsigned int  rfu			: 2;
			unsigned int  rxcte_err		: 1;			
		}	RX_state_ADV ;		

		struct {
			unsigned int  sync_err		: 1;
			unsigned int  rxtime_err	: 1;
			unsigned int  len_err		: 1;
			unsigned int  crc_err		: 1;
			unsigned int  mic_err		: 1;	
			unsigned int  llid_err		: 1;
			unsigned int  sn_err		: 1;
			unsigned int  nesn_err		: 1;
			unsigned int  rxgrop_label	: 3;
			unsigned int  rxsteam_label	: 5;			
		}	RX_state_ISO ;
		
	} TX_02;	

	//04	
	union	{
		struct {
			unsigned int  rxllid	: 2;
			unsigned int  rxnesn	: 1;
			unsigned int  rxsn		: 1;
			unsigned int  rxmd		: 1;
			unsigned int  rxcp		: 1;	
			unsigned int  rxaclrfu	: 2;
			unsigned int  rxlen		: 8;				
		}	RX_DATA_header ;
		
		struct {
			unsigned int  rxtype	: 4;
			unsigned int  rxadvrfu	: 1;
			unsigned int  rxchsel	: 1;
			unsigned int  rxtxadd	: 1;
			unsigned int  rxrxadd	: 1;	
			unsigned int  rxlen		: 8;				
		}	RX_ADV_header ;				

		struct {
			unsigned int  rxllid	: 2;
			unsigned int  rxnesn	: 1;
			unsigned int  rxsn		: 1;
			unsigned int  rxmd		: 1;
			unsigned int  rxiso_rfu	: 3;
			unsigned int  rxlen		: 8;				
		}	RX_ISO ;

	} TX_04;
	
	//06
	uint8	rxrssi;
	uint8	used_ch_idx;
	uint8	rxrate;
	
	//08
	uint16	clknsync_low16;
	
	//0A
	uint16	clknsync_high12;

	//0C
	uint16	rxfinesync;
	uint8	rxlink_label;	
	
	//0E
	uint16	rx_white_list_ptr;

	//10
	uint16	rx_ral_ptr;
	
	//12
	struct {
			unsigned int  rxaelenth	: 6;
			unsigned int  rxaemode	: 2;
			unsigned int  rxadva	: 1;
			unsigned int  rxtarga	: 1;
			unsigned int  rxcte		: 1;	
			unsigned int  rxadi		: 1;
			unsigned int  rxauxptr	: 1;	
			unsigned int  rxsync	: 1;
			unsigned int  rxpow		: 1;	
			unsigned int  rxrsvd	: 1;		
		}	RX_EXT_ADV_header ;
	
	//14
	uint16	rxdata_ptr;

	//16
	uint8	rxcte_time;
	uint8	rxcte_type;
	uint8	num_rx_iq_samp;
		
	//18
	uint16	rxcte_ptr;	
		
} RX_desptor ;








//================================================================================================


#define		BT_ET_TXBUF_BASE						0x1000
#define		BT_ET_RXBUF_BASE						0x3000





#define		BT_ET0_TXDESCPRT0_START_ADDR				0x0A00
#define		BT_ET0_TXDESCPRT1_START_ADDR				0x0A10
#define		BT_ET0_TXDESCPRT2_START_ADDR				0x0A20
#define		BT_ET0_TXDESCPRT3_START_ADDR				0x0A30

#define		BT_ET1_TXDESCPRT0_START_ADDR				0x0A40
#define		BT_ET1_TXDESCPRT1_START_ADDR				0x0A50
#define		BT_ET1_TXDESCPRT2_START_ADDR				0x0A60
#define		BT_ET1_TXDESCPRT3_START_ADDR				0x0A70

#define		BT_ET2_TXDESCPRT0_START_ADDR				0x0A80
#define		BT_ET2_TXDESCPRT1_START_ADDR				0x0A90
#define		BT_ET2_TXDESCPRT2_START_ADDR				0x0AA0
#define		BT_ET2_TXDESCPRT3_START_ADDR				0x0AB0

#define		BT_ET3_TXDESCPRT0_START_ADDR				0x0AC0
#define		BT_ET3_TXDESCPRT1_START_ADDR				0x0AD0
#define		BT_ET3_TXDESCPRT2_START_ADDR				0x0AE0
#define		BT_ET3_TXDESCPRT3_START_ADDR				0x0AF0

#define		BT_ET4_TXDESCPRT0_START_ADDR				0x0B00
#define		BT_ET4_TXDESCPRT1_START_ADDR				0x0B10
#define		BT_ET4_TXDESCPRT2_START_ADDR				0x0B20
#define		BT_ET4_TXDESCPRT3_START_ADDR				0x0B30

#define		BT_ET5_TXDESCPRT0_START_ADDR				0x0B40
#define		BT_ET5_TXDESCPRT1_START_ADDR				0x0B50
#define		BT_ET5_TXDESCPRT2_START_ADDR				0x0B60
#define		BT_ET5_TXDESCPRT3_START_ADDR				0x0B70




#define		BT_ET0_ACL_TXBUF0_START_ADDR			0x0000+0x1000
#define		BT_ET0_ACL_TXBUF1_START_ADDR			0x0000+0x1000+0x100
#define		BT_ET0_LM_TXBUF0_START_ADDR				0x0000+0x1000+0x400
#define		BT_ET0_LM_TXBUF1_START_ADDR				0x0000+0x1000+0x500

#define		BT_ET1_ACL_TXBUF0_START_ADDR			0x0000+0x1800
#define		BT_ET1_ACL_TXBUF1_START_ADDR			0x0000+0x1800+0x100
#define		BT_ET1_LM_TXBUF0_START_ADDR				0x0000+0x1800+0x400
#define		BT_ET1_LM_TXBUF1_START_ADDR				0x0000+0x1800+0x500


#define		BT_ET2_ACL_TXBUF0_START_ADDR			0x0000+0x2000
#define		BT_ET2_ACL_TXBUF1_START_ADDR			0x0000+0x2000+0x100
#define		BT_ET2_LM_TXBUF0_START_ADDR				0x0000+0x2000+0x400
#define		BT_ET2_LM_TXBUF1_START_ADDR				0x0000+0x2000+0x500

#define		BT_ET3_ACL_TXBUF0_START_ADDR			0x0000+0x2800
#define		BT_ET3_ACL_TXBUF1_START_ADDR			0x0000+0x2800+0x100
#define		BT_ET3_LM_TXBUF0_START_ADDR				0x0000+0x2800+0x400
#define		BT_ET3_LM_TXBUF1_START_ADDR				0x0000+0x2800+0x500

#define		BT_ET4_ACL_TXBUF0_START_ADDR			0x0000+0x3000
#define		BT_ET4_ACL_TXBUF1_START_ADDR			0x0000+0x3000+0x100
#define		BT_ET4_LM_TXBUF0_START_ADDR				0x0000+0x3000+0x400
#define		BT_ET4_LM_TXBUF1_START_ADDR				0x0000+0x3000+0x500

#define		BT_ET5_ACL_TXBUF0_START_ADDR			0x0000+0x3800
#define		BT_ET5_ACL_TXBUF1_START_ADDR			0x0000+0x3800+0x100
#define		BT_ET5_LM_TXBUF0_START_ADDR				0x0000+0x3800+0x400
#define		BT_ET5_LM_TXBUF1_START_ADDR				0x0000+0x3800+0x500




#define		BT_SCO_PACKET							0
#define		BT_eSCO_PACKET						1
#define		BT_eSCO_1m								0
#define		BT_eSCO_23m								1
#define		BT_ET0_SCO_TXBUF0						0x1800
#define		BT_ET0_SCO_TXBUF1						0x1A00
#define		BT_ET0_SCO_RXBUF0						0x1C00
#define		BT_ET0_SCO_RXBUF1						0x1E00





typedef struct {

	struct {
		unsigned int  format		: 5;
		unsigned int  rfu2			: 2;
		unsigned int  sam_en		: 1;
		unsigned int  dnabort		: 1;
		unsigned int  rxbsyen		: 1;
		unsigned int  txbsyen		: 1;
		unsigned int  rfu1			: 1;
		unsigned int  fmwspatt	: 3;
		unsigned int  rfu_1			: 1;
	}	CS_00 ;


	//02
	uint16	clkoff_low16;
	//04
	uint16	clkoff_high12;

	struct {
		unsigned int  linkble		: 5;
		unsigned int  rfu3			: 3;
		unsigned int  aclltaddr	: 3;
		unsigned int  acledr		: 1;
		unsigned int  whdsb			: 1;
		unsigned int  laap			: 1;
		unsigned int  afh_ena		: 1;
		unsigned int  akn_ena		: 1;
	}	CS_06 ;


	//08
	uint16	bdaddr_l;
	//0A
	uint16	bdaddr_m;
	//0C
	uint16	bdaddr_h;
	//0E
	uint16	bch_low16;
	//10
	uint16	bch_mid16;

	struct {
		unsigned int  bch_high2		: 2;
		unsigned int  rfu4			  : 4;
		unsigned int  rxmaxbuf		: 10;
	}	CS_12 ;


	struct {
		unsigned int  txpwr		: 8;
		unsigned int  freq		: 7;
		unsigned int  fh_en		: 1;
	}	CS_14 ;

	struct {
		unsigned int  txcrypt		: 2;
		unsigned int  txbcry		: 1;
		unsigned int  txeir			: 1;
		unsigned int  rfu2			: 2;
		unsigned int  nullflt		: 1;
		unsigned int  stopmode	: 1;
		unsigned int  rxcrypt		: 2;
		unsigned int  rxbcry		: 1;
		unsigned int  rxeir			: 1;
		unsigned int  rfu1			: 1;
		unsigned int  rxthr			: 3;
	}	CS_16 ;


	struct {
		unsigned int  rx_wind_size						: 15;
		unsigned int  rx_wind_wide						: 1;
	}	CS_18 ;

	//1A
	uint16	txdesptr;

	//1C
	uint16	chanmap0;
	//1E
	uint16	chanmap1;
	//20
	uint16	chanmap2;
	//22
	uint16	chanmap3;
	//24
	uint16	chanmap4;

	struct {
		unsigned int  to_num						: 8;
		unsigned int  att_num						: 8;
	}	CS_26 ;

	//28
	uint16	maxfrm_time;

	//2A
	uint16	sam_ptr;

	struct {
		unsigned int  sam_lenth						: 8;
		unsigned int  sam_offset					: 8;
	}	CS_2C ;

	//2E
	uint16	sk0;
	uint16	sk1;
	uint16	sk2;
	uint16	sk3;
	uint16	sk4;
	uint16	sk5;
	uint16	sk6;
	uint16	sk7;

	//3E
	uint16	iv0;
	uint16	iv1;
	uint16	iv2;
	uint16	iv3;

	//46
	uint16	txccmpldcnt_low16;
	//48
	uint16	txccmpldcnt_mid16;
	//4A
	uint16	txccmpldcnt_high4;

	//4C
	uint16	rxccmpldcnt_low16;
	//4E
	uint16	rxccmpldcnt_mid16;
	//50
	uint16	rxccmpldcnt_high4;
	//52
	uint16	rxclkn_low16;
	//54
	uint16	rxclkn_high12;

	struct {
		unsigned int  rxbit						: 10;
		unsigned int  rfu5						: 5;
		unsigned int  frrxok					: 1;
	}	CS_56 ;

	struct {
		unsigned int  acltxdesc_cnt				: 5;
		unsigned int  rfu3						: 3;
		unsigned int  radiotxdesc_cnt			: 5;
		unsigned int  rfu_3						: 3;
	}	CS_58 ;


	struct {
		unsigned int  aclrxdesc_cnt				: 5;
		unsigned int  rfu3						: 3;
		unsigned int  radiorxdesc_cnt			: 5;
		unsigned int  rfu_3						: 3;
	}	CS_5A ;

//	struct {
//		unsigned int  escolast_seqn				: 1;
//		unsigned int  escolast_ack				: 1;
//		unsigned int  escolastwaitack			: 1;
//		unsigned int  rfu5						: 5;
//		unsigned int  escoretx					: 1;
//		unsigned int  escorsvd					: 1;
//		unsigned int  rfu6						: 6;
//	}	CS_5C ;

	struct {
		unsigned int  lasttx_seqn				: 1;
		unsigned int  lasttx_ack				: 1;
		unsigned int  rfu5						: 5;
		unsigned int  lasttx_null				: 1;
		unsigned int  rfu2						: 2;
		unsigned int  waitack					: 1;
		unsigned int  rswitch					: 1;
		unsigned int  fnak						: 1;
		unsigned int  fcrc						: 1;
		unsigned int  txflush					: 1;
		unsigned int  txtog						: 1;
	}	CS_5C ;

	struct {
		unsigned int  lastrx_seqn				: 1;
		unsigned int  lastrx_ack				: 1;
		unsigned int  lastrxflow				: 1;
		unsigned int  rfu7						: 7;
		unsigned int  rxlmp						: 1;
		unsigned int  rxaudio					: 1;
		unsigned int  rxacl						: 1;
		unsigned int  rxid						: 1;
		unsigned int  rxbfmicerr				: 1;
		unsigned int  rxbuf_full				: 1;

	}	CS_5E ;



} BT_CS_struct ;



typedef	struct {
	//00
	uint8	tx_done;
	uint16	nxt_txdescptor_ptr;

	//02
	struct {
			unsigned int  txltaddr	: 3;
			unsigned int  txtype	: 4;
			unsigned int  txflow	: 1;
			unsigned int  txarqn	: 1;
			unsigned int  txseqn	: 1;
			unsigned int  rfu6		: 6;
		}	TX_header ;
	//04
	struct {
			unsigned int  txllid	: 2;
			unsigned int  txpflow	: 1;
			unsigned int  txlength	: 10;
			unsigned int  txundef	: 3;
		}	TX_Pheader ;

	//06
	uint16	txaclbuf_ptr;

	//08
	uint16	txlmbuf_ptr;

} BT_TX_desptor ;





typedef	struct {
	
	uint16  bch_h	;
	uint16	bch_m ;
	uint16	bch_l ;
	uint8		lap_0;
	uint8   lap_1;
	uint8   lap_2;
	uint8   eir;
	uint8   rfu1;
	uint8   sr;
	uint8   sp;
	uint8   uap8;
	uint16  nap16;
	uint32  class_of_device;
	uint8   lt_addr;
	uint32  clkn;
	uint8   page_scan_mode;

	
} FHS_packet ;












#define		SCO_HV1		5
#define		SCO_HV2		6
#define		SCO_HV3		7
#define		SCO_DV		8

#define	    eSCO_EV3	7
#define	    eSCO_EV4	12
#define		eSCO_EV5	13

#define		eSCO_2EV3	6
#define		eSCO_3EV3	7
#define		sSCO_2EV5	12
#define		eSCO_3EV5	13











//============================================
//define  RW_IP INT position

#define  RW_BLE_TXINT			3    ////1
#define  RW_BLE_RXINT			4    ////2
#define  RW_BLE_ENDEVNT_INT		1    ////5
#define  RW_BLE_ERR_INT			16   ////8



#define	 RW_BT_START_INT		0    ////4
#define	 RW_BT_END_INT			1    ////5
#define  RW_BT_ERR_INT			16   ////8
#define	 RW_BT_RX_INT			4    ////2


////--INTCNTL0
#define	 RW_DM_ERR_INT			16   ////8
////--INTCNTL1
#define	 RW_DM_CLKN_INT			0
#define	 RW_DM_SLEEP_INT		1    ////3
////#define	 RW_DM_FIFO_INT     	15
#define	 FIFO_INT     	15





//============================================

extern   void ble_sys_initial(void);
extern   void RW_EM_total_clr(uint32_t start_point , uint32_t  len) ;
extern   void RW_ET_clr (uint8 ET_indx);
extern	 void RW_BLE_ET_inital(uint8  ET_indx  , ET_struct * my_cfg ) ;
extern	 void RW_BLE_CS_initial( uint8  ET_indx  , CS_struct * my_cs_cfg );
extern	 void RW_BLE_TX_descptor_initial( uint32_t TX_descptor_start_addr  , TX_desptor * my_txdesptor );
extern   void RW_BLE_ET0_RXDPTR_initial(void);
extern   void xvr_reg_initial(void);
extern   void RW_BLE_freqtable(uint32_t freqtab_startaddr);
extern   void RW_BLE_CTE_RXDPTR_initial(void);
extern	 void RW_GOTO_SLEEP(uint32 time);
extern	 void RW_DEBUG_TEST_CPORT(uint8 index);
extern	 void RW_DEBUG_TEST_DPORT(uint8 index);






//===========BT ======================
extern	 void RW_BT_ET_inital(uint8  ET_indx  , BT_ET_struct * my_cfg ) ;
extern	 void RW_BT_CS_initial( uint8  ET_indx  , BT_CS_struct * my_cs_cfg );
extern	 void RW_BT_TX_descptor_initial( uint32_t TX_descptor_start_addr  , BT_TX_desptor * my_txdesptor );
extern	 void RW_BT_RXDPTR_initial(void) ;

//=================================
extern void ble_adv_ind(void);
extern void ble_active_scan(void);




#endif
 

