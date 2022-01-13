;/**
 ;****************************************************************************************
 ;*
 ;* @file boot_vectors.s
 ;*
 ;* @brief ARM Exception Vectors table.
 ;*
 ;* Copyright (C) RivieraWaves 2009-2015
 ;*
 ;* $Rev:  $
 ;*
 ;****************************************************************************************
 ;*/

 
	
	EXPORT vectors
	
	IMPORT sys_Reset
	
	IMPORT Undefined_Exception
	IMPORT SoftwareInterrupt_Exception
		
	IMPORT Reserved_Exception
	IMPORT PrefetchAbort_Exception
	IMPORT DataAbort_Exception		
	IMPORT Irq_Exception
		
	PRESERVE8

	CODE32
    AREA SYS_BOOT, CODE, READONLY
    ENTRY
		
vectors
    ; reset handler
    LDR     PC,       boot_reset
    ; undefined handler
    LDR     PC,       boot_undefined
    ; SWI handler
    LDR     PC,		  boot_swi
    ; Prefetch error handler
    LDR     PC,       boot_pabort
    ; abort handler
    LDR     PC,       boot_dabort
    ; reserved vector
	LDR     PC,		  boot_reserved
    ; irq
    LDR     PC,       intc_irq
    ; fiq
    LDR     PC,       intc_fiq
	
	

boot_reset          DCD     sys_Reset
boot_undefined      DCD     Undefined
boot_swi            DCD     SoftwareInterrupt
boot_pabort        	DCD     PrefetchAbort
boot_dabort       	DCD     DataAbort
boot_reserved       DCD     sys_reserved
intc_irq            DCD     IRQ_Handler
intc_fiq            DCD     FIQ_Handler

I_SEL               EQU     0x00400000

OFFSET_ndefined     EQU     0x40
OFFSET_SWI_Handler  EQU     0x60
OFFSET_pabort       EQU     0x80
OFFSET_dabort       EQU     0xa0
OFFSET_reserved     EQU     0xc0
OFFSET_IRQ_Handler  EQU     0xe0
OFFSET_FIQ_Handler  EQU     0x100



;*******************************************************************************
;* Function Name  : UndefinedHandler
;* Description    : This function called when undefined instruction exception
;*                  is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
Undefined
;		STMFD SP!,{R0,R1}
;		LDR R0,=I_SEL
;		LDR R1,[R0]
;		CMP R1,#0X00	
;		BEQ    Undefined_Exception

;        LDR R0,=OFFSET_ndefined	
    ;    ADD	 R1, R0
  ;      BX R1

  B Undefined_Exception


;*******************************************************************************
;* Function Name  : SWIHandler
;* Description    : This function called when SWI instruction executed.
;* Input          : none
;* Output         : none
;*******************************************************************************
SoftwareInterrupt
		STMFD SP!,{R0,R1}
		LDR R0,=I_SEL
		LDR R1,[R0]
		CMP R1,#0X00	
		BEQ    SoftwareInterrupt_Exception

        LDR R0,=OFFSET_SWI_Handler	
        ADD	 R1, R0
        BX R1

PrefetchAbort
		STMFD SP!,{R0,R1}
		LDR R0,=I_SEL
		LDR R1,[R0]
		CMP R1,#0X00		
		BEQ    PrefetchAbort_Exception

        LDR R0,=OFFSET_pabort	
        ADD	 R1, R0
        BX R1

;*******************************************************************************
;* Function Name  : DataAbortHandler
;* Description    : This function is called when Data Abort exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
DataAbort
		STMFD SP!,{R0,R1}
		LDR R0,=I_SEL
		LDR R1,[R0]
		CMP R1,#0X00	
		BEQ    DataAbort_Exception

        LDR R0,=OFFSET_dabort	
        ADD	 R1, R0
        BX R1

;*******************************************************************************
;* Function Name  : IRQHandler
;* Description    : This function called when IRQ exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
IRQ_Handler
	STMFD SP!,{R0,R1}
	;LDR R0,=I_SEL
	;LDR R1,[R0]
	;CMP R1,#0X01
	;LDMFD SP!,{R0,R1}		
	;BEQ    Irq_Exception
        LDR    R1, =0x400004
	LDR    r0, [R1]
	BX     r0		
        ;B      SYSirq_IRQ_Handler

;*******************************************************************************
;* Function Name  : FIQHandler
;* Description    : This function is called when FIQ exception is entered.
;* Input          : none
;* Output         : none
;*******************************************************************************
FIQ_Handler
        STMFD SP!,{R0,R1}
        LDR    R1, =0x400008
	LDR    r0, [R1]
	BX     r0
        ;B      SYSirq_FIQ_Handler
        
sys_reserved
		;B	   SYSirq_reserved
		;;B	   Reserved_Exception

	

	
	

	END