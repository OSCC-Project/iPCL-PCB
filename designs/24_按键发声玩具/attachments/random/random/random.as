opt subtitle "HI-TECH Software Omniscient Code Generator (PRO mode) build 10920"

opt pagewidth 120

	opt pm

	processor	16F684
clrc	macro
	bcf	3,0
	endm
clrz	macro
	bcf	3,2
	endm
setc	macro
	bsf	3,0
	endm
setz	macro
	bsf	3,2
	endm
skipc	macro
	btfss	3,0
	endm
skipz	macro
	btfss	3,2
	endm
skipnc	macro
	btfsc	3,0
	endm
skipnz	macro
	btfsc	3,2
	endm
indf	equ	0
indf0	equ	0
pc	equ	2
pcl	equ	2
status	equ	3
fsr	equ	4
fsr0	equ	4
c	equ	1
z	equ	0
pclath	equ	10
	FNCALL	_main,_POWER_INITIAL
	FNCALL	_main,_DelayMs
	FNCALL	_main,_PA4_Level_Change_INITIAL
	FNCALL	_main,_address
	FNCALL	_main,_WByte
	FNCALL	_WByte,_DelayMs
	FNCALL	_WByte,_DelayUs
	FNCALL	_address,_rand
	FNCALL	_address,___awmod
	FNCALL	_DelayMs,_DelayUs
	FNCALL	_rand,_srand
	FNCALL	_rand,___lmul
	FNROOT	_main
	FNCALL	intlevel1,_ISR
	global	intlevel1
	FNROOT	intlevel1
	global	_randx
	global	_num
	global	_randf
	global	_FSR
psect	text113,local,class=CODE,delta=2
global __ptext113
__ptext113:
_FSR	set	4
	DABS	1,4,1	;_FSR

	global	_INDF
_INDF	set	0
	DABS	1,0,1	;_INDF

	global	_PCL
_PCL	set	2
	DABS	1,2,1	;_PCL

	global	_PCLATH
_PCLATH	set	10
	DABS	1,10,1	;_PCLATH

	global	_PORTA
_PORTA	set	5
	DABS	1,5,1	;_PORTA

	global	_SFR06
_SFR06	set	6
	DABS	1,6,1	;_SFR06

	global	_SFR07
_SFR07	set	7
	DABS	1,7,1	;_SFR07

	global	_SFR08
_SFR08	set	8
	DABS	1,8,1	;_SFR08

	global	_SFR09
_SFR09	set	9
	DABS	1,9,1	;_SFR09

	global	_STATUS
_STATUS	set	3
	DABS	1,3,1	;_STATUS

	global	_TMR0
_TMR0	set	1
	DABS	1,1,1	;_TMR0

	global	_CCPR1
_CCPR1	set	19
	DABS	1,19,2	;_CCPR1

	global	_TMR1
_TMR1	set	14
	DABS	1,14,2	;_TMR1

	global	_ADCON0
_ADCON0	set	31
	DABS	1,31,1	;_ADCON0

	global	_ADRESH
_ADRESH	set	30
	DABS	1,30,1	;_ADRESH

	global	_CCP1CON
_CCP1CON	set	21
	DABS	1,21,1	;_CCP1CON

	global	_CCPR1H
_CCPR1H	set	20
	DABS	1,20,1	;_CCPR1H

	global	_CCPR1L
_CCPR1L	set	19
	DABS	1,19,1	;_CCPR1L

	global	_CMCON0
_CMCON0	set	25
	DABS	1,25,1	;_CMCON0

	global	_CMCON1
_CMCON1	set	26
	DABS	1,26,1	;_CMCON1

	global	_ECCPAS
_ECCPAS	set	23
	DABS	1,23,1	;_ECCPAS

	global	_INTCONbits
_INTCONbits	set	11
	DABS	1,11,1	;_INTCONbits

	global	_MSCKCONbits
_MSCKCONbits	set	27
	DABS	1,27,1	;_MSCKCONbits

	global	_PCLATHbits
_PCLATHbits	set	10
	DABS	1,10,1	;_PCLATHbits

	global	_PIR1bits
_PIR1bits	set	12
	DABS	1,12,1	;_PIR1bits

	global	_PORTAbits
_PORTAbits	set	5
	DABS	1,5,1	;_PORTAbits

	global	_PORTC
_PORTC	set	7
	DABS	1,7,1	;_PORTC

	global	_PWM1CON
_PWM1CON	set	22
	DABS	1,22,1	;_PWM1CON

	global	_SFR00
_SFR00	set	0
	DABS	1,0,1	;_SFR00

	global	_SFR01
_SFR01	set	1
	DABS	1,1,1	;_SFR01

	global	_SFR02
_SFR02	set	2
	DABS	1,2,1	;_SFR02

	global	_SFR03
_SFR03	set	3
	DABS	1,3,1	;_SFR03

	global	_SFR04
_SFR04	set	4
	DABS	1,4,1	;_SFR04

	global	_SFR05
_SFR05	set	5
	DABS	1,5,1	;_SFR05

	global	_SFR0A
_SFR0A	set	10
	DABS	1,10,1	;_SFR0A

	global	_SFR0B
_SFR0B	set	11
	DABS	1,11,1	;_SFR0B

	global	_SFR0C
_SFR0C	set	12
	DABS	1,12,1	;_SFR0C

	global	_SFR11
_SFR11	set	17
	DABS	1,17,1	;_SFR11

	global	_SFR12
_SFR12	set	18
	DABS	1,18,1	;_SFR12

	global	_SFR18
_SFR18	set	24
	DABS	1,24,1	;_SFR18

	global	_SFR1B
_SFR1B	set	27
	DABS	1,27,1	;_SFR1B

	global	_SFR1C
_SFR1C	set	28
	DABS	1,28,1	;_SFR1C

	global	_SFR1D
_SFR1D	set	29
	DABS	1,29,1	;_SFR1D

	global	_SOSCPRHbits
_SOSCPRHbits	set	29
	DABS	1,29,1	;_SOSCPRHbits

	global	_STATUSbits
_STATUSbits	set	3
	DABS	1,3,1	;_STATUSbits

	global	_T1CON
_T1CON	set	16
	DABS	1,16,1	;_T1CON

	global	_T2CONbits
_T2CONbits	set	18
	DABS	1,18,1	;_T2CONbits

	global	_TMR1H
_TMR1H	set	15
	DABS	1,15,1	;_TMR1H

	global	_TMR1L
_TMR1L	set	14
	DABS	1,14,1	;_TMR1L

	global	_WDTCONbits
_WDTCONbits	set	24
	DABS	1,24,1	;_WDTCONbits

	global	_ADFM
_ADFM	set	255
	DABS	1,31,1	;_ADFM

	global	_ADIF
_ADIF	set	102
	DABS	1,12,1	;_ADIF

	global	_ADON
_ADON	set	248
	DABS	1,31,1	;_ADON

	global	_C
_C	set	24
	DABS	1,3,1	;_C

	global	_C1IF
_C1IF	set	99
	DABS	1,12,1	;_C1IF

	global	_C1INV
_C1INV	set	204
	DABS	1,25,1	;_C1INV

	global	_C1OUT
_C1OUT	set	206
	DABS	1,25,1	;_C1OUT

	global	_C2IF
_C2IF	set	100
	DABS	1,12,1	;_C2IF

	global	_C2INV
_C2INV	set	205
	DABS	1,25,1	;_C2INV

	global	_C2OUT
_C2OUT	set	207
	DABS	1,25,1	;_C2OUT

	global	_C2SYNC
_C2SYNC	set	208
	DABS	1,26,1	;_C2SYNC

	global	_CARRY
_CARRY	set	24
	DABS	1,3,1	;_CARRY

	global	_CCP1IF
_CCP1IF	set	101
	DABS	1,12,1	;_CCP1IF

	global	_CCP1M0
_CCP1M0	set	168
	DABS	1,21,1	;_CCP1M0

	global	_CCP1M1
_CCP1M1	set	169
	DABS	1,21,1	;_CCP1M1

	global	_CCP1M2
_CCP1M2	set	170
	DABS	1,21,1	;_CCP1M2

	global	_CCP1M3
_CCP1M3	set	171
	DABS	1,21,1	;_CCP1M3

	global	_CHS0
_CHS0	set	250
	DABS	1,31,1	;_CHS0

	global	_CHS1
_CHS1	set	251
	DABS	1,31,1	;_CHS1

	global	_CHS2
_CHS2	set	252
	DABS	1,31,1	;_CHS2

	global	_CIS
_CIS	set	203
	DABS	1,25,1	;_CIS

	global	_CKCNTI
_CKCNTI	set	217
	DABS	1,27,1	;_CKCNTI

	global	_CKMAVG
_CKMAVG	set	218
	DABS	1,27,1	;_CKMAVG

	global	_CKMEAIF
_CKMEAIF	set	102
	DABS	1,12,1	;_CKMEAIF

	global	_CM0
_CM0	set	200
	DABS	1,25,1	;_CM0

	global	_CM1
_CM1	set	201
	DABS	1,25,1	;_CM1

	global	_CM2
_CM2	set	202
	DABS	1,25,1	;_CM2

	global	_DC
_DC	set	25
	DABS	1,3,1	;_DC

	global	_DC1B0
_DC1B0	set	172
	DABS	1,21,1	;_DC1B0

	global	_DC1B1
_DC1B1	set	173
	DABS	1,21,1	;_DC1B1

	global	_ECCPAS0
_ECCPAS0	set	188
	DABS	1,23,1	;_ECCPAS0

	global	_ECCPAS1
_ECCPAS1	set	189
	DABS	1,23,1	;_ECCPAS1

	global	_ECCPAS2
_ECCPAS2	set	190
	DABS	1,23,1	;_ECCPAS2

	global	_ECCPASE
_ECCPASE	set	191
	DABS	1,23,1	;_ECCPASE

	global	_ECCPIF
_ECCPIF	set	101
	DABS	1,12,1	;_ECCPIF

	global	_EEIF
_EEIF	set	103
	DABS	1,12,1	;_EEIF

	global	_GIE
_GIE	set	95
	DABS	1,11,1	;_GIE

	global	_GO
_GO	set	249
	DABS	1,31,1	;_GO

	global	_GO_DONE
_GO_DONE	set	249
	DABS	1,31,1	;_GO_DONE

	global	_GO_nDONE
_GO_nDONE	set	249
	DABS	1,31,1	;_GO_nDONE

	global	_HC
_HC	set	25
	DABS	1,3,1	;_HC

	global	_INTE
_INTE	set	92
	DABS	1,11,1	;_INTE

	global	_INTF
_INTF	set	89
	DABS	1,11,1	;_INTF

	global	_IRP
_IRP	set	31
	DABS	1,3,1	;_IRP

	global	_OSFIF
_OSFIF	set	98
	DABS	1,12,1	;_OSFIF

	global	_P1M0
_P1M0	set	174
	DABS	1,21,1	;_P1M0

	global	_P1M1
_P1M1	set	175
	DABS	1,21,1	;_P1M1

	global	_PA0
_PA0	set	40
	DABS	1,5,1	;_PA0

	global	_PA1
_PA1	set	41
	DABS	1,5,1	;_PA1

	global	_PA2
_PA2	set	42
	DABS	1,5,1	;_PA2

	global	_PA3
_PA3	set	43
	DABS	1,5,1	;_PA3

	global	_PA4
_PA4	set	44
	DABS	1,5,1	;_PA4

	global	_PA5
_PA5	set	45
	DABS	1,5,1	;_PA5

	global	_PAGE
_PAGE	set	29
	DABS	1,3,1	;_PAGE

	global	_PAIE
_PAIE	set	91
	DABS	1,11,1	;_PAIE

	global	_PAIF
_PAIF	set	88
	DABS	1,11,1	;_PAIF

	global	_PDC0
_PDC0	set	176
	DABS	1,22,1	;_PDC0

	global	_PDC1
_PDC1	set	177
	DABS	1,22,1	;_PDC1

	global	_PDC2
_PDC2	set	178
	DABS	1,22,1	;_PDC2

	global	_PDC3
_PDC3	set	179
	DABS	1,22,1	;_PDC3

	global	_PDC4
_PDC4	set	180
	DABS	1,22,1	;_PDC4

	global	_PDC5
_PDC5	set	181
	DABS	1,22,1	;_PDC5

	global	_PDC6
_PDC6	set	182
	DABS	1,22,1	;_PDC6

	global	_PEIE
_PEIE	set	94
	DABS	1,11,1	;_PEIE

	global	_PRSEN
_PRSEN	set	183
	DABS	1,22,1	;_PRSEN

	global	_PSSAC0
_PSSAC0	set	186
	DABS	1,23,1	;_PSSAC0

	global	_PSSAC1
_PSSAC1	set	187
	DABS	1,23,1	;_PSSAC1

	global	_PSSBD0
_PSSBD0	set	184
	DABS	1,23,1	;_PSSBD0

	global	_PSSBD1
_PSSBD1	set	185
	DABS	1,23,1	;_PSSBD1

	global	_RA0
_RA0	set	40
	DABS	1,5,1	;_RA0

	global	_RA1
_RA1	set	41
	DABS	1,5,1	;_RA1

	global	_RA2
_RA2	set	42
	DABS	1,5,1	;_RA2

	global	_RA3
_RA3	set	43
	DABS	1,5,1	;_RA3

	global	_RA4
_RA4	set	44
	DABS	1,5,1	;_RA4

	global	_RA5
_RA5	set	45
	DABS	1,5,1	;_RA5

	global	_RAIE
_RAIE	set	91
	DABS	1,11,1	;_RAIE

	global	_RAIF
_RAIF	set	88
	DABS	1,11,1	;_RAIF

	global	_RC0
_RC0	set	56
	DABS	1,7,1	;_RC0

	global	_RC1
_RC1	set	57
	DABS	1,7,1	;_RC1

	global	_RC2
_RC2	set	58
	DABS	1,7,1	;_RC2

	global	_RC3
_RC3	set	59
	DABS	1,7,1	;_RC3

	global	_RC4
_RC4	set	60
	DABS	1,7,1	;_RC4

	global	_RC5
_RC5	set	61
	DABS	1,7,1	;_RC5

	global	_RP
_RP	set	29
	DABS	1,3,1	;_RP

	global	_RP0
_RP0	set	29
	DABS	1,3,1	;_RP0

	global	_RP1
_RP1	set	30
	DABS	1,3,1	;_RP1

	global	_SLVREN
_SLVREN	set	220
	DABS	1,27,1	;_SLVREN

	global	_SWDTEN
_SWDTEN	set	192
	DABS	1,24,1	;_SWDTEN

	global	_T0IE
_T0IE	set	93
	DABS	1,11,1	;_T0IE

	global	_T0IF
_T0IF	set	90
	DABS	1,11,1	;_T0IF

	global	_T1CKPS0
_T1CKPS0	set	132
	DABS	1,16,1	;_T1CKPS0

	global	_T1CKPS1
_T1CKPS1	set	133
	DABS	1,16,1	;_T1CKPS1

	global	_T1GINV
_T1GINV	set	135
	DABS	1,16,1	;_T1GINV

	global	_T1GSS
_T1GSS	set	209
	DABS	1,26,1	;_T1GSS

	global	_T1IF
_T1IF	set	96
	DABS	1,12,1	;_T1IF

	global	_T1OSCEN
_T1OSCEN	set	131
	DABS	1,16,1	;_T1OSCEN

	global	_T2CKPS0
_T2CKPS0	set	144
	DABS	1,18,1	;_T2CKPS0

	global	_T2CKPS1
_T2CKPS1	set	145
	DABS	1,18,1	;_T2CKPS1

	global	_T2IF
_T2IF	set	97
	DABS	1,12,1	;_T2IF

	global	_TMR0IE
_TMR0IE	set	93
	DABS	1,11,1	;_TMR0IE

	global	_TMR0IF
_TMR0IF	set	90
	DABS	1,11,1	;_TMR0IF

	global	_TMR1CS
_TMR1CS	set	129
	DABS	1,16,1	;_TMR1CS

	global	_TMR1GE
_TMR1GE	set	134
	DABS	1,16,1	;_TMR1GE

	global	_TMR1IF
_TMR1IF	set	96
	DABS	1,12,1	;_TMR1IF

	global	_TMR1ON
_TMR1ON	set	128
	DABS	1,16,1	;_TMR1ON

	global	_TMR2IF
_TMR2IF	set	97
	DABS	1,12,1	;_TMR2IF

	global	_TMR2ON
_TMR2ON	set	146
	DABS	1,18,1	;_TMR2ON

	global	_TOUTPS0
_TOUTPS0	set	147
	DABS	1,18,1	;_TOUTPS0

	global	_TOUTPS1
_TOUTPS1	set	148
	DABS	1,18,1	;_TOUTPS1

	global	_TOUTPS2
_TOUTPS2	set	149
	DABS	1,18,1	;_TOUTPS2

	global	_TOUTPS3
_TOUTPS3	set	150
	DABS	1,18,1	;_TOUTPS3

	global	_VCFG
_VCFG	set	254
	DABS	1,31,1	;_VCFG

	global	_WDTPS0
_WDTPS0	set	193
	DABS	1,24,1	;_WDTPS0

	global	_WDTPS1
_WDTPS1	set	194
	DABS	1,24,1	;_WDTPS1

	global	_WDTPS2
_WDTPS2	set	195
	DABS	1,24,1	;_WDTPS2

	global	_WDTPS3
_WDTPS3	set	196
	DABS	1,24,1	;_WDTPS3

	global	_Z
_Z	set	26
	DABS	1,3,1	;_Z

	global	_ZERO
_ZERO	set	26
	DABS	1,3,1	;_ZERO

	global	_nDONE
_nDONE	set	249
	DABS	1,31,1	;_nDONE

	global	_nPD
_nPD	set	27
	DABS	1,3,1	;_nPD

	global	_nPF
_nPF	set	27
	DABS	1,3,1	;_nPF

	global	_nT1SYNC
_nT1SYNC	set	130
	DABS	1,16,1	;_nT1SYNC

	global	_nTF
_nTF	set	28
	DABS	1,3,1	;_nTF

	global	_nTO
_nTO	set	28
	DABS	1,3,1	;_nTO

	global	_Key_Old_Data
psect	nvCOMMON,class=COMMON,space=1
global __pnvCOMMON
__pnvCOMMON:
_Key_Old_Data:
       ds      1

	global	_OPTION
_OPTION	set	129
	DABS	1,129,1	;_OPTION

	global	_SFR80
_SFR80	set	128
	DABS	1,128,1	;_SFR80

	global	_ADCON1
_ADCON1	set	159
	DABS	1,159,1	;_ADCON1

	global	_ADRESL
_ADRESL	set	158
	DABS	1,158,1	;_ADRESL

	global	_ANSEL
_ANSEL	set	145
	DABS	1,145,1	;_ANSEL

	global	_EECON1bits
_EECON1bits	set	156
	DABS	1,156,1	;_EECON1bits

	global	_EECON2bits
_EECON2bits	set	157
	DABS	1,157,1	;_EECON2bits

	global	_EEDATA
_EEDATA	set	154
	DABS	1,154,1	;_EEDATA

	global	_IOC
_IOC	set	150
	DABS	1,150,1	;_IOC

	global	_IOCAbits
_IOCAbits	set	150
	DABS	1,150,1	;_IOCAbits

	global	_OPTION_REG
_OPTION_REG	set	129
	DABS	1,129,1	;_OPTION_REG

	global	_OPTIONbits
_OPTIONbits	set	129
	DABS	1,129,1	;_OPTIONbits

	global	_OSCCONbits
_OSCCONbits	set	143
	DABS	1,143,1	;_OSCCONbits

	global	_OSCTUNE
_OSCTUNE	set	144
	DABS	1,144,1	;_OSCTUNE

	global	_PCONbits
_PCONbits	set	142
	DABS	1,142,1	;_PCONbits

	global	_PIE1bits
_PIE1bits	set	140
	DABS	1,140,1	;_PIE1bits

	global	_SFR81
_SFR81	set	129
	DABS	1,129,1	;_SFR81

	global	_SFR85
_SFR85	set	133
	DABS	1,133,1	;_SFR85

	global	_SFR8C
_SFR8C	set	140
	DABS	1,140,1	;_SFR8C

	global	_SFR8E
_SFR8E	set	142
	DABS	1,142,1	;_SFR8E

	global	_SFR8F
_SFR8F	set	143
	DABS	1,143,1	;_SFR8F

	global	_SFR92
_SFR92	set	146
	DABS	1,146,1	;_SFR92

	global	_SFR95
_SFR95	set	149
	DABS	1,149,1	;_SFR95

	global	_SFR96
_SFR96	set	150
	DABS	1,150,1	;_SFR96

	global	_SFR9A
_SFR9A	set	154
	DABS	1,154,1	;_SFR9A

	global	_SFR9B
_SFR9B	set	155
	DABS	1,155,1	;_SFR9B

	global	_SFR9C
_SFR9C	set	156
	DABS	1,156,1	;_SFR9C

	global	_SFR9D
_SFR9D	set	157
	DABS	1,157,1	;_SFR9D

	global	_TRISAbits
_TRISAbits	set	133
	DABS	1,133,1	;_TRISAbits

	global	_TRISC
_TRISC	set	135
	DABS	1,135,1	;_TRISC

	global	_VRCON
_VRCON	set	153
	DABS	1,153,1	;_VRCON

	global	_WPU
_WPU	set	149
	DABS	1,149,1	;_WPU

	global	_WPUAbits
_WPUAbits	set	149
	DABS	1,149,1	;_WPUAbits

	global	_ADCS0
_ADCS0	set	1276
	DABS	1,159,1	;_ADCS0

	global	_ADCS1
_ADCS1	set	1277
	DABS	1,159,1	;_ADCS1

	global	_ADCS2
_ADCS2	set	1278
	DABS	1,159,1	;_ADCS2

	global	_ADIE
_ADIE	set	1126
	DABS	1,140,1	;_ADIE

	global	_ANS0
_ANS0	set	1160
	DABS	1,145,1	;_ANS0

	global	_ANS1
_ANS1	set	1161
	DABS	1,145,1	;_ANS1

	global	_ANS2
_ANS2	set	1162
	DABS	1,145,1	;_ANS2

	global	_ANS3
_ANS3	set	1163
	DABS	1,145,1	;_ANS3

	global	_ANS4
_ANS4	set	1164
	DABS	1,145,1	;_ANS4

	global	_ANS5
_ANS5	set	1165
	DABS	1,145,1	;_ANS5

	global	_ANS6
_ANS6	set	1166
	DABS	1,145,1	;_ANS6

	global	_ANS7
_ANS7	set	1167
	DABS	1,145,1	;_ANS7

	global	_C1IE
_C1IE	set	1123
	DABS	1,140,1	;_C1IE

	global	_C2IE
_C2IE	set	1124
	DABS	1,140,1	;_C2IE

	global	_CCP1IE
_CCP1IE	set	1125
	DABS	1,140,1	;_CCP1IE

	global	_CKMEAIE
_CKMEAIE	set	1126
	DABS	1,140,1	;_CKMEAIE

	global	_ECCPIE
_ECCPIE	set	1125
	DABS	1,140,1	;_ECCPIE

	global	_EEIE
_EEIE	set	1127
	DABS	1,140,1	;_EEIE

	global	_HFIOFR
_HFIOFR	set	1146
	DABS	1,143,1	;_HFIOFR

	global	_HTS
_HTS	set	1146
	DABS	1,143,1	;_HTS

	global	_INTEDG
_INTEDG	set	1038
	DABS	1,129,1	;_INTEDG

	global	_IOC0
_IOC0	set	1200
	DABS	1,150,1	;_IOC0

	global	_IOC1
_IOC1	set	1201
	DABS	1,150,1	;_IOC1

	global	_IOC2
_IOC2	set	1202
	DABS	1,150,1	;_IOC2

	global	_IOC3
_IOC3	set	1203
	DABS	1,150,1	;_IOC3

	global	_IOC4
_IOC4	set	1204
	DABS	1,150,1	;_IOC4

	global	_IOC5
_IOC5	set	1205
	DABS	1,150,1	;_IOC5

	global	_IOCA0
_IOCA0	set	1200
	DABS	1,150,1	;_IOCA0

	global	_IOCA1
_IOCA1	set	1201
	DABS	1,150,1	;_IOCA1

	global	_IOCA2
_IOCA2	set	1202
	DABS	1,150,1	;_IOCA2

	global	_IOCA3
_IOCA3	set	1203
	DABS	1,150,1	;_IOCA3

	global	_IOCA4
_IOCA4	set	1204
	DABS	1,150,1	;_IOCA4

	global	_IOCA5
_IOCA5	set	1205
	DABS	1,150,1	;_IOCA5

	global	_IRCF0
_IRCF0	set	1148
	DABS	1,143,1	;_IRCF0

	global	_IRCF1
_IRCF1	set	1149
	DABS	1,143,1	;_IRCF1

	global	_IRCF2
_IRCF2	set	1150
	DABS	1,143,1	;_IRCF2

	global	_LFIOFR
_LFIOFR	set	1145
	DABS	1,143,1	;_LFIOFR

	global	_LFMOD
_LFMOD	set	1151
	DABS	1,143,1	;_LFMOD

	global	_LTS
_LTS	set	1145
	DABS	1,143,1	;_LTS

	global	_OSFIE
_OSFIE	set	1122
	DABS	1,140,1	;_OSFIE

	global	_OSTS
_OSTS	set	1147
	DABS	1,143,1	;_OSTS

	global	_PS0
_PS0	set	1032
	DABS	1,129,1	;_PS0

	global	_PS1
_PS1	set	1033
	DABS	1,129,1	;_PS1

	global	_PS2
_PS2	set	1034
	DABS	1,129,1	;_PS2

	global	_PSA
_PSA	set	1035
	DABS	1,129,1	;_PSA

	global	_RD
_RD	set	1248
	DABS	1,156,1	;_RD

	global	_SBODEN
_SBODEN	set	1140
	DABS	1,142,1	;_SBODEN

	global	_SBOREN
_SBOREN	set	1140
	DABS	1,142,1	;_SBOREN

	global	_SCS
_SCS	set	1144
	DABS	1,143,1	;_SCS

	global	_T0CS
_T0CS	set	1037
	DABS	1,129,1	;_T0CS

	global	_T0SE
_T0SE	set	1036
	DABS	1,129,1	;_T0SE

	global	_T1IE
_T1IE	set	1120
	DABS	1,140,1	;_T1IE

	global	_T2IE
_T2IE	set	1121
	DABS	1,140,1	;_T2IE

	global	_TMR1IE
_TMR1IE	set	1120
	DABS	1,140,1	;_TMR1IE

	global	_TMR2IE
_TMR2IE	set	1121
	DABS	1,140,1	;_TMR2IE

	global	_TRISA0
_TRISA0	set	1064
	DABS	1,133,1	;_TRISA0

	global	_TRISA1
_TRISA1	set	1065
	DABS	1,133,1	;_TRISA1

	global	_TRISA2
_TRISA2	set	1066
	DABS	1,133,1	;_TRISA2

	global	_TRISA3
_TRISA3	set	1067
	DABS	1,133,1	;_TRISA3

	global	_TRISA4
_TRISA4	set	1068
	DABS	1,133,1	;_TRISA4

	global	_TRISA5
_TRISA5	set	1069
	DABS	1,133,1	;_TRISA5

	global	_TRISC0
_TRISC0	set	1080
	DABS	1,135,1	;_TRISC0

	global	_TRISC1
_TRISC1	set	1081
	DABS	1,135,1	;_TRISC1

	global	_TRISC2
_TRISC2	set	1082
	DABS	1,135,1	;_TRISC2

	global	_TRISC3
_TRISC3	set	1083
	DABS	1,135,1	;_TRISC3

	global	_TRISC4
_TRISC4	set	1084
	DABS	1,135,1	;_TRISC4

	global	_TRISC5
_TRISC5	set	1085
	DABS	1,135,1	;_TRISC5

	global	_TUN0
_TUN0	set	1152
	DABS	1,144,1	;_TUN0

	global	_TUN1
_TUN1	set	1153
	DABS	1,144,1	;_TUN1

	global	_TUN2
_TUN2	set	1154
	DABS	1,144,1	;_TUN2

	global	_TUN3
_TUN3	set	1155
	DABS	1,144,1	;_TUN3

	global	_TUN4
_TUN4	set	1156
	DABS	1,144,1	;_TUN4

	global	_ULPWUE
_ULPWUE	set	1141
	DABS	1,142,1	;_ULPWUE

	global	_VR0
_VR0	set	1224
	DABS	1,153,1	;_VR0

	global	_VR1
_VR1	set	1225
	DABS	1,153,1	;_VR1

	global	_VR2
_VR2	set	1226
	DABS	1,153,1	;_VR2

	global	_VR3
_VR3	set	1227
	DABS	1,153,1	;_VR3

	global	_VREN
_VREN	set	1231
	DABS	1,153,1	;_VREN

	global	_VRR
_VRR	set	1229
	DABS	1,153,1	;_VRR

	global	_WPU0
_WPU0	set	1192
	DABS	1,149,1	;_WPU0

	global	_WPU1
_WPU1	set	1193
	DABS	1,149,1	;_WPU1

	global	_WPU2
_WPU2	set	1194
	DABS	1,149,1	;_WPU2

	global	_WPU4
_WPU4	set	1196
	DABS	1,149,1	;_WPU4

	global	_WPU5
_WPU5	set	1197
	DABS	1,149,1	;_WPU5

	global	_WPUA0
_WPUA0	set	1192
	DABS	1,149,1	;_WPUA0

	global	_WPUA1
_WPUA1	set	1193
	DABS	1,149,1	;_WPUA1

	global	_WPUA2
_WPUA2	set	1194
	DABS	1,149,1	;_WPUA2

	global	_WPUA4
_WPUA4	set	1196
	DABS	1,149,1	;_WPUA4

	global	_WPUA5
_WPUA5	set	1197
	DABS	1,149,1	;_WPUA5

	global	_WR
_WR	set	1256
	DABS	1,157,1	;_WR

	global	_WREN
_WREN	set	1250
	DABS	1,156,1	;_WREN

	global	_WREN1
_WREN1	set	1250
	DABS	1,156,1	;_WREN1

	global	_WREN2
_WREN2	set	1252
	DABS	1,156,1	;_WREN2

	global	_WREN3
_WREN3	set	1253
	DABS	1,156,1	;_WREN3

	global	_WRERR
_WRERR	set	1251
	DABS	1,156,1	;_WRERR

	global	_nBOD
_nBOD	set	1136
	DABS	1,142,1	;_nBOD

	global	_nBOR
_nBOR	set	1136
	DABS	1,142,1	;_nBOR

	global	_nPAPU
_nPAPU	set	1039
	DABS	1,129,1	;_nPAPU

	global	_nPOR
_nPOR	set	1137
	DABS	1,142,1	;_nPOR

	global	_nRAPU
_nRAPU	set	1039
	DABS	1,129,1	;_nRAPU

	global	_INTCON
_INTCON	set	11
	DABS	1,11,1	;_INTCON

	global	_PIR1
_PIR1	set	12
	DABS	1,12,1	;_PIR1

	global	_SFR0D
_SFR0D	set	13
	DABS	1,13,1	;_SFR0D

	global	_SFR0E
_SFR0E	set	14
	DABS	1,14,1	;_SFR0E

	global	_SFR0F
_SFR0F	set	15
	DABS	1,15,1	;_SFR0F

	global	_SFR10
_SFR10	set	16
	DABS	1,16,1	;_SFR10

	global	_TMR2
_TMR2	set	17
	DABS	1,17,1	;_TMR2

	global	_SFR13
_SFR13	set	19
	DABS	1,19,1	;_SFR13

	global	_SFR14
_SFR14	set	20
	DABS	1,20,1	;_SFR14

	global	_SFR15
_SFR15	set	21
	DABS	1,21,1	;_SFR15

	global	_SFR16
_SFR16	set	22
	DABS	1,22,1	;_SFR16

	global	_SFR17
_SFR17	set	23
	DABS	1,23,1	;_SFR17

	global	_T2CON
_T2CON	set	18
	DABS	1,18,1	;_T2CON

	global	_WDTCON
_WDTCON	set	24
	DABS	1,24,1	;_WDTCON

	global	_MSCKCON
_MSCKCON	set	27
	DABS	1,27,1	;_MSCKCON

	global	_SFR19
_SFR19	set	25
	DABS	1,25,1	;_SFR19

	global	_SFR1A
_SFR1A	set	26
	DABS	1,26,1	;_SFR1A

	global	_SFR1E
_SFR1E	set	30
	DABS	1,30,1	;_SFR1E

	global	_SFR1F
_SFR1F	set	31
	DABS	1,31,1	;_SFR1F

	global	_SOSCPRH
_SOSCPRH	set	29
	DABS	1,29,1	;_SOSCPRH

	global	_SOSCPRL
_SOSCPRL	set	28
	DABS	1,28,1	;_SOSCPRL

	global	_SFR82
_SFR82	set	130
	DABS	1,130,1	;_SFR82

	global	_SFR83
_SFR83	set	131
	DABS	1,131,1	;_SFR83

	global	_SFR84
_SFR84	set	132
	DABS	1,132,1	;_SFR84

	global	_TRISA
_TRISA	set	133
	DABS	1,133,1	;_TRISA

	global	_PIE1
_PIE1	set	140
	DABS	1,140,1	;_PIE1

	global	_SFR86
_SFR86	set	134
	DABS	1,134,1	;_SFR86

	global	_SFR87
_SFR87	set	135
	DABS	1,135,1	;_SFR87

	global	_SFR88
_SFR88	set	136
	DABS	1,136,1	;_SFR88

	global	_SFR89
_SFR89	set	137
	DABS	1,137,1	;_SFR89

	global	_SFR8A
_SFR8A	set	138
	DABS	1,138,1	;_SFR8A

	global	_SFR8B
_SFR8B	set	139
	DABS	1,139,1	;_SFR8B

	global	_PCON
_PCON	set	142
	DABS	1,142,1	;_PCON

	global	_SFR8D
_SFR8D	set	141
	DABS	1,141,1	;_SFR8D

	global	_OSCCON
_OSCCON	set	143
	DABS	1,143,1	;_OSCCON

	global	_PR2
_PR2	set	146
	DABS	1,146,1	;_PR2

	global	_SFR90
_SFR90	set	144
	DABS	1,144,1	;_SFR90

	global	_SFR91
_SFR91	set	145
	DABS	1,145,1	;_SFR91

	global	_SFR93
_SFR93	set	147
	DABS	1,147,1	;_SFR93

	global	_SFR94
_SFR94	set	148
	DABS	1,148,1	;_SFR94

	global	_WPUA
_WPUA	set	149
	DABS	1,149,1	;_WPUA

	global	_EEDAT
_EEDAT	set	154
	DABS	1,154,1	;_EEDAT

	global	_IOCA
_IOCA	set	150
	DABS	1,150,1	;_IOCA

	global	_SFR97
_SFR97	set	151
	DABS	1,151,1	;_SFR97

	global	_SFR98
_SFR98	set	152
	DABS	1,152,1	;_SFR98

	global	_SFR99
_SFR99	set	153
	DABS	1,153,1	;_SFR99

	global	_EEADR
_EEADR	set	155
	DABS	1,155,1	;_EEADR

	global	_EECON1
_EECON1	set	156
	DABS	1,156,1	;_EECON1

	global	_EECON2
_EECON2	set	157
	DABS	1,157,1	;_EECON2

	global	_SFR9E
_SFR9E	set	158
	DABS	1,158,1	;_SFR9E

	global	_SFR9F
_SFR9F	set	159
	DABS	1,159,1	;_SFR9F

	file	"random.as"
	line	#
psect cinit,class=CODE,delta=2
global start_initialization
start_initialization:

psect	bssCOMMON,class=COMMON,space=1
global __pbssCOMMON
__pbssCOMMON:
_randx:
       ds      4

_num:
       ds      1

_randf:
       ds      1

; Clear objects allocated to COMMON
psect cinit,class=CODE,delta=2
	clrf	((__pbssCOMMON)+0)&07Fh
	clrf	((__pbssCOMMON)+1)&07Fh
	clrf	((__pbssCOMMON)+2)&07Fh
	clrf	((__pbssCOMMON)+3)&07Fh
	clrf	((__pbssCOMMON)+4)&07Fh
	clrf	((__pbssCOMMON)+5)&07Fh
psect cinit,class=CODE,delta=2
global end_of_initialization

;End of C runtime variable initialization code

end_of_initialization:
clrf status
ljmp _main	;jump to C main() function
psect	cstackCOMMON,class=COMMON,space=1
global __pcstackCOMMON
__pcstackCOMMON:
	global	?_POWER_INITIAL
?_POWER_INITIAL:	; 0 bytes @ 0x0
	global	?_ISR
?_ISR:	; 0 bytes @ 0x0
	global	??_ISR
??_ISR:	; 0 bytes @ 0x0
	global	?_DelayUs
?_DelayUs:	; 0 bytes @ 0x0
	global	?_DelayMs
?_DelayMs:	; 0 bytes @ 0x0
	global	?_PA4_Level_Change_INITIAL
?_PA4_Level_Change_INITIAL:	; 0 bytes @ 0x0
	global	?_WByte
?_WByte:	; 0 bytes @ 0x0
	global	?_main
?_main:	; 2 bytes @ 0x0
	ds	2
	global	??_POWER_INITIAL
??_POWER_INITIAL:	; 0 bytes @ 0x2
	global	??_DelayUs
??_DelayUs:	; 0 bytes @ 0x2
	global	??_PA4_Level_Change_INITIAL
??_PA4_Level_Change_INITIAL:	; 0 bytes @ 0x2
	global	?_srand
?_srand:	; 0 bytes @ 0x2
	global	?___lmul
?___lmul:	; 4 bytes @ 0x2
	global	DelayUs@Time
DelayUs@Time:	; 1 bytes @ 0x2
	global	srand@x
srand@x:	; 2 bytes @ 0x2
	global	___lmul@multiplier
___lmul@multiplier:	; 4 bytes @ 0x2
	ds	1
	global	DelayUs@a
DelayUs@a:	; 1 bytes @ 0x3
	ds	1
	global	??_DelayMs
??_DelayMs:	; 0 bytes @ 0x4
	global	??_srand
??_srand:	; 0 bytes @ 0x4
	global	DelayMs@Time
DelayMs@Time:	; 1 bytes @ 0x4
	ds	1
	global	DelayMs@a
DelayMs@a:	; 1 bytes @ 0x5
	ds	1
	global	??_WByte
??_WByte:	; 0 bytes @ 0x6
	global	WByte@input
WByte@input:	; 1 bytes @ 0x6
	global	___lmul@multiplicand
___lmul@multiplicand:	; 4 bytes @ 0x6
	ds	1
	global	WByte@i
WByte@i:	; 1 bytes @ 0x7
	ds	1
	global	WByte@a
WByte@a:	; 1 bytes @ 0x8
	ds	2
	global	??___lmul
??___lmul:	; 0 bytes @ 0xA
	global	___lmul@product
___lmul@product:	; 4 bytes @ 0xA
	ds	4
	global	?___awmod
?___awmod:	; 2 bytes @ 0xE
	global	___awmod@divisor
___awmod@divisor:	; 2 bytes @ 0xE
	ds	2
	global	___awmod@dividend
___awmod@dividend:	; 2 bytes @ 0x10
	ds	2
	global	??___awmod
??___awmod:	; 0 bytes @ 0x12
	global	___awmod@counter
___awmod@counter:	; 1 bytes @ 0x12
	ds	1
	global	___awmod@sign
___awmod@sign:	; 1 bytes @ 0x13
	ds	1
	global	?_rand
?_rand:	; 2 bytes @ 0x14
	ds	2
	global	??_rand
??_rand:	; 0 bytes @ 0x16
	ds	4
	global	?_address
?_address:	; 2 bytes @ 0x1A
	ds	2
	global	??_address
??_address:	; 0 bytes @ 0x1C
	global	address@b
address@b:	; 2 bytes @ 0x1C
	ds	2
	global	??_main
??_main:	; 0 bytes @ 0x1E
	global	main@sleepflag
main@sleepflag:	; 2 bytes @ 0x1E
	ds	2
	global	main@i
main@i:	; 2 bytes @ 0x20
	ds	2
;;Data sizes: Strings 0, constant 0, data 0, bss 6, persistent 1 stack 0
;;Auto spaces:   Size  Autos    Used
;; COMMON          62     34      41

;;
;; Pointer list with targets:

;; ?_rand	int  size(1) Largest target is 0
;;
;; ?___lmul	unsigned long  size(1) Largest target is 0
;;
;; ?_address	unsigned int  size(1) Largest target is 0
;;
;; ?___awmod	int  size(1) Largest target is 0
;;


;;
;; Critical Paths under _main in COMMON
;;
;;   _main->_address
;;   _WByte->_DelayMs
;;   _address->_rand
;;   _DelayMs->_DelayUs
;;   _rand->___awmod
;;   ___awmod->___lmul
;;
;; Critical Paths under _ISR in COMMON
;;
;;   None.

;;
;;Main: autosize = 0, tempsize = 0, incstack = 0, save=0
;;

;;
;;Call Graph Tables:
;;
;; ---------------------------------------------------------------------------------
;; (Depth) Function   	        Calls       Base Space   Used Autos Params    Refs
;; ---------------------------------------------------------------------------------
;; (0) _main                                                 4     4      0     645
;;                                             30 COMMON     4     4      0
;;                      _POWER_INITIAL
;;                            _DelayMs
;;           _PA4_Level_Change_INITIAL
;;                            _address
;;                              _WByte
;; ---------------------------------------------------------------------------------
;; (1) _WByte                                                3     3      0     240
;;                                              6 COMMON     3     3      0
;;                            _DelayMs
;;                            _DelayUs
;; ---------------------------------------------------------------------------------
;; (1) _address                                              4     2      2     285
;;                                             26 COMMON     4     2      2
;;                               _rand
;;                            ___awmod
;; ---------------------------------------------------------------------------------
;; (2) _DelayMs                                              2     2      0      60
;;                                              4 COMMON     2     2      0
;;                            _DelayUs
;; ---------------------------------------------------------------------------------
;; (2) _rand                                                 6     4      2      75
;;                                             20 COMMON     6     4      2
;;                              _srand
;;                             ___lmul
;;                            ___awmod (ARG)
;; ---------------------------------------------------------------------------------
;; (2) _DelayUs                                              2     2      0      30
;;                                              2 COMMON     2     2      0
;; ---------------------------------------------------------------------------------
;; (2) ___awmod                                              6     2      4     195
;;                                             14 COMMON     6     2      4
;;                              _srand (ARG)
;;                             ___lmul (ARG)
;; ---------------------------------------------------------------------------------
;; (3) ___lmul                                              12     4      8      60
;;                                              2 COMMON    12     4      8
;; ---------------------------------------------------------------------------------
;; (3) _srand                                                2     0      2      15
;;                                              2 COMMON     2     0      2
;; ---------------------------------------------------------------------------------
;; (1) _PA4_Level_Change_INITIAL                             0     0      0       0
;; ---------------------------------------------------------------------------------
;; (1) _POWER_INITIAL                                        0     0      0       0
;; ---------------------------------------------------------------------------------
;; Estimated maximum stack depth 3
;; ---------------------------------------------------------------------------------
;; (Depth) Function   	        Calls       Base Space   Used Autos Params    Refs
;; ---------------------------------------------------------------------------------
;; (4) _ISR                                                  2     2      0       0
;;                                              0 COMMON     2     2      0
;; ---------------------------------------------------------------------------------
;; Estimated maximum stack depth 4
;; ---------------------------------------------------------------------------------

;; Call Graph Graphs:

;; _main (ROOT)
;;   _POWER_INITIAL
;;   _DelayMs
;;     _DelayUs
;;   _PA4_Level_Change_INITIAL
;;   _address
;;     _rand
;;       _srand
;;       ___lmul
;;       ___awmod (ARG)
;;         _srand (ARG)
;;         ___lmul (ARG)
;;     ___awmod
;;       _srand (ARG)
;;       ___lmul (ARG)
;;   _WByte
;;     _DelayMs
;;       _DelayUs
;;     _DelayUs
;;
;; _ISR (ROOT)
;;

;; Address spaces:

;;Name               Size   Autos  Total    Cost      Usage
;;BITCOMMON           3E      0       0       0        0.0%
;;EEDATA             100      0       0       0        0.0%
;;NULL                 0      0       0       0        0.0%
;;CODE                 0      0       0       0        0.0%
;;COMMON              3E     22      29       1       66.1%
;;BITSFR0              0      0       0       1        0.0%
;;SFR0                 0      0       0       1        0.0%
;;BITSFR1              0      0       0       2        0.0%
;;SFR1                 0      0       0       2        0.0%
;;STACK                0      0       3       2        0.0%
;;ABS                  0      0      29       3        0.0%
;;DATA                 0      0      2C       4        0.0%

	global	_main
psect	maintext,global,class=CODE,delta=2
global __pmaintext
__pmaintext:

;; *************** function _main *****************
;; Defined at:
;;		line 151 in file "random.C"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;  i               2   32[COMMON] unsigned int 
;;  sleepflag       2   30[COMMON] unsigned int 
;; Return value:  Size  Location     Type
;;                  2  491[COMMON] int 
;; Registers used:
;;		wreg, status,2, status,0, pclath, cstack
;; Tracked objects:
;;		On entry : 17F/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         4
;;      Temps:          0
;;      Totals:         4
;;Total ram usage:        4 bytes
;; Hardware stack levels required when called:    4
;; This function calls:
;;		_POWER_INITIAL
;;		_DelayMs
;;		_PA4_Level_Change_INITIAL
;;		_address
;;		_WByte
;; This function is called by:
;;		Startup code after reset
;; This function uses a non-reentrant model
;;
psect	maintext
	file	"random.C"
	line	151
	global	__size_of_main
	__size_of_main	equ	__end_of_main-_main
	
_main:	
	opt	stack 4
; Regs used in _main: [wreg+status,2+status,0+pclath+cstack]
	line	152
	
l1640:	
;random.C: 152: POWER_INITIAL();
	fcall	_POWER_INITIAL
	line	153
	
l1642:	
;random.C: 153: Key_Old_Data = 1;
	clrf	(_Key_Old_Data)
	incf	(_Key_Old_Data),f
	line	156
	
l1644:	
;random.C: 154: unsigned int i;
;random.C: 155: unsigned int sleepflag;
;random.C: 156: sleepflag = 0;
	clrf	(main@sleepflag)
	clrf	(main@sleepflag+1)
	line	159
	
l1646:	
;random.C: 158: {
;random.C: 159: PA2 =1;
	bcf	status, 5	;RP0=0, select bank0
	bsf	(42/8),(42)&7
	line	160
	
l1648:	
;random.C: 160: if(PA4==1)
	btfss	(44/8),(44)&7
	goto	u231
	goto	u230
u231:
	goto	l1680
u230:
	line	162
	
l1650:	
;random.C: 161: {
;random.C: 162: if(Key_Old_Data==0)
	movf	(_Key_Old_Data),f
	skipz
	goto	u241
	goto	u240
u241:
	goto	l1678
u240:
	line	164
	
l1652:	
;random.C: 163: {
;random.C: 164: for(i=0;i<150;i++)
	clrf	(main@i)
	clrf	(main@i+1)
	line	166
	
l1658:	
;random.C: 165: {
;random.C: 166: DelayMs(100);
	movlw	(064h)
	fcall	_DelayMs
	line	167
	
l1660:	
;random.C: 167: if(PA4==0)
	bcf	status, 5	;RP0=0, select bank0
	btfsc	(44/8),(44)&7
	goto	u251
	goto	u250
u251:
	goto	l1664
u250:
	goto	l1668
	line	164
	
l1664:	
	incf	(main@i),f
	skipnz
	incf	(main@i+1),f
	
l1666:	
	movlw	high(096h)
	subwf	(main@i+1),w
	movlw	low(096h)
	skipnz
	subwf	(main@i),w
	skipc
	goto	u261
	goto	u260
u261:
	goto	l1658
u260:
	line	172
	
l1668:	
;random.C: 170: }
;random.C: 171: }
;random.C: 172: if(i>=100)
	movlw	high(064h)
	subwf	(main@i+1),w
	movlw	low(064h)
	skipnz
	subwf	(main@i),w
	skipc
	goto	u271
	goto	u270
u271:
	goto	l1678
u270:
	line	174
	
l1670:	
;random.C: 173: {
;random.C: 174: sleepflag = 1;
	clrf	(main@sleepflag)
	incf	(main@sleepflag),f
	clrf	(main@sleepflag+1)
	line	175
	
l1672:	
;random.C: 175: PA4_Level_Change_INITIAL();
	fcall	_PA4_Level_Change_INITIAL
	line	176
	
l1674:	
;random.C: 176: GIE = 1;
	bsf	(95/8),(95)&7
	line	177
	
l1676:	
# 177 "random.C"
sleep ;#
psect	maintext
	line	180
	
l1678:	
;random.C: 178: }
;random.C: 179: }
;random.C: 180: Key_Old_Data=1;
	clrf	(_Key_Old_Data)
	incf	(_Key_Old_Data),f
	line	182
	
l1680:	
;random.C: 181: }
;random.C: 182: if(PA4==0)
	bcf	status, 5	;RP0=0, select bank0
	btfsc	(44/8),(44)&7
	goto	u281
	goto	u280
u281:
	goto	l1646
u280:
	line	184
	
l1682:	
;random.C: 183: {
;random.C: 184: if(Key_Old_Data==1)
	decf	(_Key_Old_Data),w
	skipz
	goto	u291
	goto	u290
u291:
	goto	l1646
u290:
	line	186
	
l1684:	
;random.C: 185: {
;random.C: 186: if(sleepflag==1)
	decf	(main@sleepflag),w
	iorwf	(main@sleepflag+1),w

	skipz
	goto	u301
	goto	u300
u301:
	goto	l1694
u300:
	line	188
	
l1686:	
;random.C: 187: {
;random.C: 188: PA2=0;
	bcf	(42/8),(42)&7
	line	189
	
l1688:	
;random.C: 189: DelayMs(45);
	movlw	(02Dh)
	fcall	_DelayMs
	line	190
	
l1690:	
;random.C: 190: PA2=1;
	bcf	status, 5	;RP0=0, select bank0
	bsf	(42/8),(42)&7
	line	191
;random.C: 191: DelayMs(15);
	movlw	(0Fh)
	fcall	_DelayMs
	line	192
	
l1692:	
;random.C: 192: sleepflag=0;
	clrf	(main@sleepflag)
	clrf	(main@sleepflag+1)
	line	194
	
l1694:	
;random.C: 193: }
;random.C: 194: num = address();
	fcall	_address
	movf	(0+(?_address)),w
	movwf	(_num)
	line	195
	
l1696:	
;random.C: 195: WByte(num);
	movf	(_num),w
	fcall	_WByte
	line	196
	
l1698:	
;random.C: 196: Key_Old_Data=0;
	clrf	(_Key_Old_Data)
	goto	l1646
	global	start
	ljmp	start
	opt stack 0
psect	maintext
	line	201
GLOBAL	__end_of_main
	__end_of_main:
;; =============== function _main ends ============

	signat	_main,90
	global	_WByte
psect	text114,local,class=CODE,delta=2
global __ptext114
__ptext114:

;; *************** function _WByte *****************
;; Defined at:
;;		line 101 in file "random.C"
;; Parameters:    Size  Location     Type
;;  input           1    wreg     unsigned char 
;; Auto vars:     Size  Location     Type
;;  input           1    6[COMMON] unsigned char 
;;  a               1    8[COMMON] unsigned char 
;;  i               1    7[COMMON] unsigned char 
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2, status,0, pclath, cstack
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         3
;;      Temps:          0
;;      Totals:         3
;;Total ram usage:        3 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    3
;; This function calls:
;;		_DelayMs
;;		_DelayUs
;; This function is called by:
;;		_main
;; This function uses a non-reentrant model
;;
psect	text114
	file	"random.C"
	line	101
	global	__size_of_WByte
	__size_of_WByte	equ	__end_of_WByte-_WByte
	
_WByte:	
	opt	stack 4
; Regs used in _WByte: [wreg+status,2+status,0+pclath+cstack]
;WByte@input stored from wreg
	line	103
	movwf	(WByte@input)
	
l1582:	
;random.C: 103: unsigned char i=8;
	movlw	(08h)
	movwf	(WByte@i)
	line	104
	
l1584:	
;random.C: 104: PA2 = 0;
	bcf	status, 5	;RP0=0, select bank0
	bcf	(42/8),(42)&7
	line	105
	
l1586:	
;random.C: 105: DelayMs(7);
	movlw	(07h)
	fcall	_DelayMs
	line	107
;random.C: 106: unsigned char a;
;random.C: 107: while(i--)
	goto	l1638
	
l474:	
	line	109
;random.C: 108: {
;random.C: 109: if(input&0x01)
	btfss	(WByte@input),(0)&7
	goto	u171
	goto	u170
u171:
	goto	l475
u170:
	line	111
	
l1588:	
;random.C: 110: {
;random.C: 111: PA2=1;
	bcf	status, 5	;RP0=0, select bank0
	bsf	(42/8),(42)&7
	line	112
	
l1590:	
;random.C: 112: for(a=0;a<3;a++)
	clrf	(WByte@a)
	line	114
	
l1596:	
;random.C: 113: {
;random.C: 114: DelayUs(4);
	movlw	(04h)
	fcall	_DelayUs
	line	112
	
l1598:	
	incf	(WByte@a),f
	
l1600:	
	movlw	(03h)
	subwf	(WByte@a),w
	skipc
	goto	u181
	goto	u180
u181:
	goto	l1596
u180:
	
l477:	
	line	116
;random.C: 115: }
;random.C: 116: PA2=0;
	bcf	status, 5	;RP0=0, select bank0
	bcf	(42/8),(42)&7
	line	117
	
l1602:	
;random.C: 117: for(a=0;a<1;a++)
	clrf	(WByte@a)
	line	119
	
l1608:	
;random.C: 118: {
;random.C: 119: DelayUs(4);
	movlw	(04h)
	fcall	_DelayUs
	line	117
	
l1610:	
	incf	(WByte@a),f
	
l1612:	
	movf	(WByte@a),w
	skipz
	goto	u190
	goto	l1608
u190:
	goto	l480
	line	122
	
l475:	
	line	124
;random.C: 122: else
;random.C: 123: {
;random.C: 124: PA2=1;
	bcf	status, 5	;RP0=0, select bank0
	bsf	(42/8),(42)&7
	line	125
	
l1614:	
;random.C: 125: for(a=0;a<1;a++)
	clrf	(WByte@a)
	line	127
	
l1620:	
;random.C: 126: {
;random.C: 127: DelayUs(4);
	movlw	(04h)
	fcall	_DelayUs
	line	125
	
l1622:	
	incf	(WByte@a),f
	
l1624:	
	movf	(WByte@a),w
	skipz
	goto	u200
	goto	l1620
u200:
	
l482:	
	line	129
;random.C: 128: }
;random.C: 129: PA2=0;
	bcf	status, 5	;RP0=0, select bank0
	bcf	(42/8),(42)&7
	line	130
	
l1626:	
;random.C: 130: for(a=0;a<3;a++)
	clrf	(WByte@a)
	line	132
	
l1632:	
;random.C: 131: {
;random.C: 132: DelayUs(4);
	movlw	(04h)
	fcall	_DelayUs
	line	130
	
l1634:	
	incf	(WByte@a),f
	
l1636:	
	movlw	(03h)
	subwf	(WByte@a),w
	skipc
	goto	u211
	goto	u210
u211:
	goto	l1632
u210:
	line	134
	
l480:	
	line	135
;random.C: 133: }
;random.C: 134: }
;random.C: 135: input=input>>1;
	clrc
	rrf	(WByte@input),f
	line	107
	
l1638:	
	decf	(WByte@i),f
	movf	((WByte@i)),w
	xorlw	0FFh
	skipz
	goto	u221
	goto	u220
u221:
	goto	l474
u220:
	
l485:	
	line	139
;random.C: 136: }
;random.C: 139: T0IE=0;
	bcf	(93/8),(93)&7
	line	140
	
l486:	
	return
	opt stack 0
GLOBAL	__end_of_WByte
	__end_of_WByte:
;; =============== function _WByte ends ============

	signat	_WByte,4216
	global	_address
psect	text115,local,class=CODE,delta=2
global __ptext115
__ptext115:

;; *************** function _address *****************
;; Defined at:
;;		line 143 in file "random.C"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;  b               2   28[COMMON] unsigned int 
;; Return value:  Size  Location     Type
;;                  2   26[COMMON] unsigned int 
;; Registers used:
;;		wreg, status,2, status,0, pclath, cstack
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: FFFFF/0
;; Data sizes:     COMMON
;;      Params:         2
;;      Locals:         2
;;      Temps:          0
;;      Totals:         4
;;Total ram usage:        4 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    3
;; This function calls:
;;		_rand
;;		___awmod
;; This function is called by:
;;		_main
;; This function uses a non-reentrant model
;;
psect	text115
	file	"random.C"
	line	143
	global	__size_of_address
	__size_of_address	equ	__end_of_address-_address
	
_address:	
	opt	stack 4
; Regs used in _address: [wreg+status,2+status,0+pclath+cstack]
	line	145
	
l1574:	
;random.C: 144: unsigned int b;
;random.C: 145: b = rand()%10;
	movlw	0Ah
	movwf	(?___awmod)
	clrf	(?___awmod+1)
	fcall	_rand
	movf	(1+(?_rand)),w
	movwf	1+(?___awmod)+02h
	movf	(0+(?_rand)),w
	movwf	0+(?___awmod)+02h
	fcall	___awmod
	movf	(1+(?___awmod)),w
	movwf	(address@b+1)
	movf	(0+(?___awmod)),w
	movwf	(address@b)
	line	146
	
l1576:	
;random.C: 146: b = b+1;
	incf	(address@b),f
	skipnz
	incf	(address@b+1),f
	line	147
	
l1578:	
;random.C: 147: return b;
	movf	(address@b+1),w
	movwf	(?_address+1)
	movf	(address@b),w
	movwf	(?_address)
	line	148
	
l489:	
	return
	opt stack 0
GLOBAL	__end_of_address
	__end_of_address:
;; =============== function _address ends ============

	signat	_address,90
	global	_DelayMs
psect	text116,local,class=CODE,delta=2
global __ptext116
__ptext116:

;; *************** function _DelayMs *****************
;; Defined at:
;;		line 66 in file "random.C"
;; Parameters:    Size  Location     Type
;;  Time            1    wreg     unsigned char 
;; Auto vars:     Size  Location     Type
;;  Time            1    4[COMMON] unsigned char 
;;  a               1    5[COMMON] unsigned char 
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2, status,0, pclath, cstack
;; Tracked objects:
;;		On entry : 20/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         2
;;      Temps:          0
;;      Totals:         2
;;Total ram usage:        2 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    2
;; This function calls:
;;		_DelayUs
;; This function is called by:
;;		_WByte
;;		_main
;; This function uses a non-reentrant model
;;
psect	text116
	file	"random.C"
	line	66
	global	__size_of_DelayMs
	__size_of_DelayMs	equ	__end_of_DelayMs-_DelayMs
	
_DelayMs:	
	opt	stack 4
; Regs used in _DelayMs: [wreg+status,2+status,0+pclath+cstack]
;DelayMs@Time stored from wreg
	line	68
	movwf	(DelayMs@Time)
	
l1566:	
;random.C: 67: unsigned char a;
;random.C: 68: for(a=0;a<Time;a++)
	clrf	(DelayMs@a)
	goto	l1572
	line	70
	
l1568:	
;random.C: 69: {
;random.C: 70: DelayUs(11);
	movlw	(0Bh)
	fcall	_DelayUs
	line	68
	
l1570:	
	incf	(DelayMs@a),f
	
l1572:	
	movf	(DelayMs@Time),w
	subwf	(DelayMs@a),w
	skipc
	goto	u161
	goto	u160
u161:
	goto	l1568
u160:
	line	72
	
l459:	
	return
	opt stack 0
GLOBAL	__end_of_DelayMs
	__end_of_DelayMs:
;; =============== function _DelayMs ends ============

	signat	_DelayMs,4216
	global	_rand
psect	text117,local,class=CODE,delta=2
global __ptext117
__ptext117:

;; *************** function _rand *****************
;; Defined at:
;;		line 14 in file "d:\sw\cabide\data\sources\rand.c"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;		None
;; Return value:  Size  Location     Type
;;                  2   20[COMMON] int 
;; Registers used:
;;		wreg, status,2, status,0, pclath, cstack
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: FFFFF/0
;; Data sizes:     COMMON
;;      Params:         2
;;      Locals:         0
;;      Temps:          4
;;      Totals:         6
;;Total ram usage:        6 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    2
;; This function calls:
;;		_srand
;;		___lmul
;; This function is called by:
;;		_address
;; This function uses a non-reentrant model
;;
psect	text117
	file	"d:\sw\cabide\data\sources\rand.c"
	line	14
	global	__size_of_rand
	__size_of_rand	equ	__end_of_rand-_rand
	
_rand:	
	opt	stack 4
; Regs used in _rand: [wreg+status,2+status,0+pclath+cstack]
	line	15
	
l1558:	
	movf	(_randf),f
	skipz
	goto	u141
	goto	u140
u141:
	goto	l1066
u140:
	line	16
	
l1560:	
	clrf	(?_srand)
	incf	(?_srand),f
	clrf	(?_srand+1)
	fcall	_srand
	
l1066:	
	line	17
	movlw	039h
	movwf	((??_rand+0)+0)
	movlw	030h
	movwf	((??_rand+0)+0+1)
	movlw	0
	movwf	((??_rand+0)+0+2)
	movlw	0
	movwf	((??_rand+0)+0+3)
	movf	(_randx+3),w
	movwf	(?___lmul+3)
	movf	(_randx+2),w
	movwf	(?___lmul+2)
	movf	(_randx+1),w
	movwf	(?___lmul+1)
	movf	(_randx),w
	movwf	(?___lmul)

	movlw	041h
	movwf	3+(?___lmul)+04h
	movlw	0C6h
	movwf	2+(?___lmul)+04h
	movlw	04Eh
	movwf	1+(?___lmul)+04h
	movlw	06Dh
	movwf	0+(?___lmul)+04h

	fcall	___lmul
	movf	(0+(?___lmul)),w
	addwf	(??_rand+0)+0,f
	movf	(1+(?___lmul)),w
	skipnc
	incfsz	(1+(?___lmul)),w
	goto	u150
	goto	u151
u150:
	addwf	(??_rand+0)+1,f
u151:
	movf	(2+(?___lmul)),w
	skipnc
	incfsz	(2+(?___lmul)),w
	goto	u152
	goto	u153
u152:
	addwf	(??_rand+0)+2,f
u153:
	movf	(3+(?___lmul)),w
	skipnc
	incf	(3+(?___lmul)),w
	addwf	(??_rand+0)+3,f
	movf	3+(??_rand+0)+0,w
	movwf	(_randx+3)
	movf	2+(??_rand+0)+0,w
	movwf	(_randx+2)
	movf	1+(??_rand+0)+0,w
	movwf	(_randx+1)
	movf	0+(??_rand+0)+0,w
	movwf	(_randx)

	movf	1+((((_randx)))+2),w
	movwf	(?_rand+1)
	movf	0+((((_randx)))+2),w
	movwf	(?_rand)
	
l1562:	
	bcf	(?_rand)+(15/8),(15)&7
	line	18
	
l1067:	
	return
	opt stack 0
GLOBAL	__end_of_rand
	__end_of_rand:
;; =============== function _rand ends ============

	signat	_rand,90
	global	_DelayUs
psect	text118,local,class=CODE,delta=2
global __ptext118
__ptext118:

;; *************** function _DelayUs *****************
;; Defined at:
;;		line 57 in file "random.C"
;; Parameters:    Size  Location     Type
;;  Time            1    wreg     unsigned char 
;; Auto vars:     Size  Location     Type
;;  Time            1    2[COMMON] unsigned char 
;;  a               1    3[COMMON] unsigned char 
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2, status,0
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: 0/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         2
;;      Temps:          0
;;      Totals:         2
;;Total ram usage:        2 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_DelayMs
;;		_WByte
;; This function uses a non-reentrant model
;;
psect	text118
	file	"random.C"
	line	57
	global	__size_of_DelayUs
	__size_of_DelayUs	equ	__end_of_DelayUs-_DelayUs
	
_DelayUs:	
	opt	stack 5
; Regs used in _DelayUs: [wreg+status,2+status,0]
;DelayUs@Time stored from wreg
	line	59
	movwf	(DelayUs@Time)
	
l1552:	
;random.C: 58: unsigned char a;
;random.C: 59: for(a=0;a<Time;a++)
	clrf	(DelayUs@a)
	goto	l1556
	line	60
	
l451:	
	line	61
;random.C: 60: {
;random.C: 61: _nop();
	nop
	line	59
	
l1554:	
	incf	(DelayUs@a),f
	
l1556:	
	movf	(DelayUs@Time),w
	subwf	(DelayUs@a),w
	skipc
	goto	u131
	goto	u130
u131:
	goto	l451
u130:
	line	63
	
l453:	
	return
	opt stack 0
GLOBAL	__end_of_DelayUs
	__end_of_DelayUs:
;; =============== function _DelayUs ends ============

	signat	_DelayUs,4216
	global	___awmod
psect	text119,local,class=CODE,delta=2
global __ptext119
__ptext119:

;; *************** function ___awmod *****************
;; Defined at:
;;		line 5 in file "d:\sw\cabide\data\sources\awmod.c"
;; Parameters:    Size  Location     Type
;;  divisor         2   14[COMMON] int 
;;  dividend        2   16[COMMON] int 
;; Auto vars:     Size  Location     Type
;;  sign            1   19[COMMON] unsigned char 
;;  counter         1   18[COMMON] unsigned char 
;; Return value:  Size  Location     Type
;;                  2   14[COMMON] int 
;; Registers used:
;;		wreg, status,2, status,0
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: FFFFF/0
;; Data sizes:     COMMON
;;      Params:         4
;;      Locals:         2
;;      Temps:          0
;;      Totals:         6
;;Total ram usage:        6 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_address
;; This function uses a non-reentrant model
;;
psect	text119
	file	"d:\sw\cabide\data\sources\awmod.c"
	line	5
	global	__size_of___awmod
	__size_of___awmod	equ	__end_of___awmod-___awmod
	
___awmod:	
	opt	stack 5
; Regs used in ___awmod: [wreg+status,2+status,0]
	line	8
	
l1516:	
	clrf	(___awmod@sign)
	line	9
	
l1518:	
	btfss	(___awmod@dividend+1),7
	goto	u61
	goto	u60
u61:
	goto	l1524
u60:
	line	10
	
l1520:	
	comf	(___awmod@dividend),f
	comf	(___awmod@dividend+1),f
	incf	(___awmod@dividend),f
	skipnz
	incf	(___awmod@dividend+1),f
	line	11
	
l1522:	
	clrf	(___awmod@sign)
	incf	(___awmod@sign),f
	line	13
	
l1524:	
	btfss	(___awmod@divisor+1),7
	goto	u71
	goto	u70
u71:
	goto	l1528
u70:
	line	14
	
l1526:	
	comf	(___awmod@divisor),f
	comf	(___awmod@divisor+1),f
	incf	(___awmod@divisor),f
	skipnz
	incf	(___awmod@divisor+1),f
	line	15
	
l1528:	
	movf	(___awmod@divisor+1),w
	iorwf	(___awmod@divisor),w
	skipnz
	goto	u81
	goto	u80
u81:
	goto	l1544
u80:
	line	16
	
l1530:	
	clrf	(___awmod@counter)
	incf	(___awmod@counter),f
	line	17
	goto	l1534
	line	18
	
l1532:	
	clrc
	rlf	(___awmod@divisor),f
	rlf	(___awmod@divisor+1),f
	line	19
	incf	(___awmod@counter),f
	line	17
	
l1534:	
	btfss	(___awmod@divisor+1),(15)&7
	goto	u91
	goto	u90
u91:
	goto	l1532
u90:
	line	22
	
l1536:	
	movf	(___awmod@divisor+1),w
	subwf	(___awmod@dividend+1),w
	skipz
	goto	u105
	movf	(___awmod@divisor),w
	subwf	(___awmod@dividend),w
u105:
	skipc
	goto	u101
	goto	u100
u101:
	goto	l1540
u100:
	line	23
	
l1538:	
	movf	(___awmod@divisor),w
	subwf	(___awmod@dividend),f
	movf	(___awmod@divisor+1),w
	skipc
	decf	(___awmod@dividend+1),f
	subwf	(___awmod@dividend+1),f
	line	24
	
l1540:	
	clrc
	rrf	(___awmod@divisor+1),f
	rrf	(___awmod@divisor),f
	line	25
	
l1542:	
	decfsz	(___awmod@counter),f
	goto	u111
	goto	u110
u111:
	goto	l1536
u110:
	line	27
	
l1544:	
	movf	(___awmod@sign),w
	skipz
	goto	u120
	goto	l1548
u120:
	line	28
	
l1546:	
	comf	(___awmod@dividend),f
	comf	(___awmod@dividend+1),f
	incf	(___awmod@dividend),f
	skipnz
	incf	(___awmod@dividend+1),f
	line	29
	
l1548:	
	movf	(___awmod@dividend+1),w
	movwf	(?___awmod+1)
	movf	(___awmod@dividend),w
	movwf	(?___awmod)
	line	30
	
l1294:	
	return
	opt stack 0
GLOBAL	__end_of___awmod
	__end_of___awmod:
;; =============== function ___awmod ends ============

	signat	___awmod,8314
	global	___lmul
psect	text120,local,class=CODE,delta=2
global __ptext120
__ptext120:

;; *************** function ___lmul *****************
;; Defined at:
;;		line 3 in file "d:\sw\cabide\data\sources\lmul.c"
;; Parameters:    Size  Location     Type
;;  multiplier      4    2[COMMON] unsigned long 
;;  multiplicand    4    6[COMMON] unsigned long 
;; Auto vars:     Size  Location     Type
;;  product         4   10[COMMON] unsigned long 
;; Return value:  Size  Location     Type
;;                  4    2[COMMON] unsigned long 
;; Registers used:
;;		wreg, status,2, status,0
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: FFFFF/0
;; Data sizes:     COMMON
;;      Params:         8
;;      Locals:         4
;;      Temps:          0
;;      Totals:        12
;;Total ram usage:       12 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_rand
;; This function uses a non-reentrant model
;;
psect	text120
	file	"d:\sw\cabide\data\sources\lmul.c"
	line	3
	global	__size_of___lmul
	__size_of___lmul	equ	__end_of___lmul-___lmul
	
___lmul:	
	opt	stack 4
; Regs used in ___lmul: [wreg+status,2+status,0]
	line	4
	
l1504:	
	clrf	(___lmul@product)
	clrf	(___lmul@product+1)
	clrf	(___lmul@product+2)
	clrf	(___lmul@product+3)
	line	6
	
l1151:	
	line	7
	btfss	(___lmul@multiplier),(0)&7
	goto	u31
	goto	u30
u31:
	goto	l1508
u30:
	line	8
	
l1506:	
	movf	(___lmul@multiplicand),w
	addwf	(___lmul@product),f
	movf	(___lmul@multiplicand+1),w
	clrz
	skipnc
	addlw	1
	skipnz
	goto	u41
	addwf	(___lmul@product+1),f
u41:
	movf	(___lmul@multiplicand+2),w
	clrz
	skipnc
	addlw	1
	skipnz
	goto	u42
	addwf	(___lmul@product+2),f
u42:
	movf	(___lmul@multiplicand+3),w
	clrz
	skipnc
	addlw	1
	skipnz
	goto	u43
	addwf	(___lmul@product+3),f
u43:

	line	9
	
l1508:	
	clrc
	rlf	(___lmul@multiplicand),f
	rlf	(___lmul@multiplicand+1),f
	rlf	(___lmul@multiplicand+2),f
	rlf	(___lmul@multiplicand+3),f
	line	10
	
l1510:	
	clrc
	rrf	(___lmul@multiplier+3),f
	rrf	(___lmul@multiplier+2),f
	rrf	(___lmul@multiplier+1),f
	rrf	(___lmul@multiplier),f
	line	11
	movf	(___lmul@multiplier+3),w
	iorwf	(___lmul@multiplier+2),w
	iorwf	(___lmul@multiplier+1),w
	iorwf	(___lmul@multiplier),w
	skipz
	goto	u51
	goto	u50
u51:
	goto	l1151
u50:
	line	12
	
l1512:	
	movf	(___lmul@product+3),w
	movwf	(?___lmul+3)
	movf	(___lmul@product+2),w
	movwf	(?___lmul+2)
	movf	(___lmul@product+1),w
	movwf	(?___lmul+1)
	movf	(___lmul@product),w
	movwf	(?___lmul)

	line	13
	
l1154:	
	return
	opt stack 0
GLOBAL	__end_of___lmul
	__end_of___lmul:
;; =============== function ___lmul ends ============

	signat	___lmul,8316
	global	_srand
psect	text121,local,class=CODE,delta=2
global __ptext121
__ptext121:

;; *************** function _srand *****************
;; Defined at:
;;		line 8 in file "d:\sw\cabide\data\sources\rand.c"
;; Parameters:    Size  Location     Type
;;  x               2    2[COMMON] unsigned int 
;; Auto vars:     Size  Location     Type
;;		None
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: FFFFF/0
;; Data sizes:     COMMON
;;      Params:         2
;;      Locals:         0
;;      Temps:          0
;;      Totals:         2
;;Total ram usage:        2 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_rand
;; This function uses a non-reentrant model
;;
psect	text121
	file	"d:\sw\cabide\data\sources\rand.c"
	line	8
	global	__size_of_srand
	__size_of_srand	equ	__end_of_srand-_srand
	
_srand:	
	opt	stack 4
; Regs used in _srand: [wreg]
	line	9
	
l1500:	
	movf	(srand@x),w
	movwf	(_randx)
	movf	(srand@x+1),w
	movwf	((_randx))+1
	clrf	2+((_randx))
	clrf	3+((_randx))
	line	10
	
l1502:	
	clrf	(_randf)
	incf	(_randf),f
	line	11
	
l1063:	
	return
	opt stack 0
GLOBAL	__end_of_srand
	__end_of_srand:
;; =============== function _srand ends ============

	signat	_srand,4216
	global	_PA4_Level_Change_INITIAL
psect	text122,local,class=CODE,delta=2
global __ptext122
__ptext122:

;; *************** function _PA4_Level_Change_INITIAL *****************
;; Defined at:
;;		line 87 in file "random.C"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;		None
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg
;; Tracked objects:
;;		On entry : 20/0
;;		On exit  : 20/20
;;		Unchanged: FFFDF/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         0
;;      Temps:          0
;;      Totals:         0
;;Total ram usage:        0 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_main
;; This function uses a non-reentrant model
;;
psect	text122
	file	"random.C"
	line	87
	global	__size_of_PA4_Level_Change_INITIAL
	__size_of_PA4_Level_Change_INITIAL	equ	__end_of_PA4_Level_Change_INITIAL-_PA4_Level_Change_INITIAL
	
_PA4_Level_Change_INITIAL:	
	opt	stack 6
; Regs used in _PA4_Level_Change_INITIAL: [wreg]
	line	88
	
l1490:	
;random.C: 88: TRISA4 =1;
	bsf	status, 5	;RP0=1, select bank1
	bsf	(1068/8)^080h,(1068)&7
	line	89
	
l1492:	
;random.C: 89: ReadAPin = PORTA;
	bcf	status, 5	;RP0=0, select bank0
	movf	(5),w	;volatile
	line	90
	
l1494:	
;random.C: 90: PAIF =0;
	bcf	(88/8),(88)&7
	line	91
	
l1496:	
;random.C: 91: IOCA4 =1;
	bsf	status, 5	;RP0=1, select bank1
	bsf	(1204/8)^080h,(1204)&7
	line	92
	
l1498:	
;random.C: 92: PAIE =1;
	bsf	(91/8),(91)&7
	line	94
	
l470:	
	return
	opt stack 0
GLOBAL	__end_of_PA4_Level_Change_INITIAL
	__end_of_PA4_Level_Change_INITIAL:
;; =============== function _PA4_Level_Change_INITIAL ends ============

	signat	_PA4_Level_Change_INITIAL,88
	global	_POWER_INITIAL
psect	text123,local,class=CODE,delta=2
global __ptext123
__ptext123:

;; *************** function _POWER_INITIAL *****************
;; Defined at:
;;		line 33 in file "random.C"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;		None
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg, status,2
;; Tracked objects:
;;		On entry : 17F/0
;;		On exit  : 17F/0
;;		Unchanged: FFE80/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         0
;;      Temps:          0
;;      Totals:         0
;;Total ram usage:        0 bytes
;; Hardware stack levels used:    1
;; Hardware stack levels required when called:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		_main
;; This function uses a non-reentrant model
;;
psect	text123
	file	"random.C"
	line	33
	global	__size_of_POWER_INITIAL
	__size_of_POWER_INITIAL	equ	__end_of_POWER_INITIAL-_POWER_INITIAL
	
_POWER_INITIAL:	
	opt	stack 6
; Regs used in _POWER_INITIAL: [wreg+status,2]
	
l1468:	
	movlw	(021h)
	bsf	status, 5	;RP0=1, select bank1
	movwf	(143)^080h	;volatile
	line	34
	
l1470:	
;random.C: 34: INTCON = 0;
	clrf	(11)	;volatile
	line	35
	
l1472:	
;random.C: 35: PORTA = 0B00000000;
	bcf	status, 5	;RP0=0, select bank0
	clrf	(5)	;volatile
	line	36
;random.C: 36: TRISA = 0B00010000;
	movlw	(010h)
	bsf	status, 5	;RP0=1, select bank1
	movwf	(133)^080h	;volatile
	line	37
;random.C: 37: OPTION = 0B10001000;
	movlw	(088h)
	movwf	(129)^080h	;volatile
	line	38
	
l1474:	
;random.C: 38: WPUA = 0B00000000;
	clrf	(149)^080h	;volatile
	line	39
	
l1476:	
;random.C: 39: MSCKCON = 0B00010000;
	movlw	(010h)
	bcf	status, 5	;RP0=0, select bank0
	movwf	(27)	;volatile
	line	40
	
l443:	
	return
	opt stack 0
GLOBAL	__end_of_POWER_INITIAL
	__end_of_POWER_INITIAL:
;; =============== function _POWER_INITIAL ends ============

	signat	_POWER_INITIAL,88
	global	_ISR
psect	text124,local,class=CODE,delta=2
global __ptext124
__ptext124:

;; *************** function _ISR *****************
;; Defined at:
;;		line 44 in file "random.C"
;; Parameters:    Size  Location     Type
;;		None
;; Auto vars:     Size  Location     Type
;;		None
;; Return value:  Size  Location     Type
;;		None               void
;; Registers used:
;;		wreg
;; Tracked objects:
;;		On entry : 0/0
;;		On exit  : 0/0
;;		Unchanged: FFFDF/0
;; Data sizes:     COMMON
;;      Params:         0
;;      Locals:         0
;;      Temps:          2
;;      Totals:         2
;;Total ram usage:        2 bytes
;; Hardware stack levels used:    1
;; This function calls:
;;		Nothing
;; This function is called by:
;;		Interrupt level 1
;; This function uses a non-reentrant model
;;
psect	text124
	file	"random.C"
	line	44
	global	__size_of_ISR
	__size_of_ISR	equ	__end_of_ISR-_ISR
	
_ISR:	
	opt	stack 4
; Regs used in _ISR: [wreg]
psect	intentry,class=CODE,delta=2
global __pintentry
__pintentry:
global interrupt_function
interrupt_function:
	global saved_w
	saved_w	set	btemp+0
	movwf	saved_w
	swapf	status,w
	movwf	(??_ISR+0)
	movf	pclath,w
	movwf	(??_ISR+1)
	ljmp	_ISR
psect	text124
	line	46
	
i1l1478:	
;random.C: 46: if(PAIE && PAIF)
	btfss	(91/8),(91)&7
	goto	u1_21
	goto	u1_20
u1_21:
	goto	i1l447
u1_20:
	
i1l1480:	
	btfss	(88/8),(88)&7
	goto	u2_21
	goto	u2_20
u2_21:
	goto	i1l447
u2_20:
	line	48
	
i1l1482:	
;random.C: 47: {
;random.C: 48: ReadAPin = PORTA;
	bcf	status, 5	;RP0=0, select bank0
	movf	(5),w	;volatile
	line	49
	
i1l1484:	
;random.C: 49: PAIF = 0;
	bcf	(88/8),(88)&7
	line	50
	
i1l1486:	
;random.C: 50: PAIE = 0;
	bcf	(91/8),(91)&7
	line	51
	
i1l1488:	
;random.C: 51: IOCA4 =0;
	bsf	status, 5	;RP0=1, select bank1
	bcf	(1204/8)^080h,(1204)&7
	line	55
	
i1l447:	
	movf	(??_ISR+1),w
	movwf	pclath
	swapf	(??_ISR+0)^0FFFFFF80h,w
	movwf	status
	swapf	saved_w,f
	swapf	saved_w,w
	retfie
	opt stack 0
GLOBAL	__end_of_ISR
	__end_of_ISR:
;; =============== function _ISR ends ============

	signat	_ISR,88
psect	text125,local,class=CODE,delta=2
global __ptext125
__ptext125:
	global	btemp
	btemp set 07Eh

	DABS	1,126,2	;btemp
	global	wtemp0
	wtemp0 set btemp
	end
