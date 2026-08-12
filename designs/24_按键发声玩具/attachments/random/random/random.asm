//Deviec:C10S01X
//-----------------------Variable---------------------------------
		_randx		EQU		62H
		_randf		EQU		67H
		_Key_Old_Data		EQU		68H
		_num		EQU		66H
//-----------------------Variable END---------------------------------
		ORG		0000H
		LJUMP 	0AH 			//0000 	380A
		ORG		0004H
		STR 	7EH 			//0004 	01FE
		SWAPR 	STATUS,0 		//0005 	0703
		STR 	40H 			//0006 	01C0
		LDR 	PCLATH,0 		//0007 	080A
		STR 	41H 			//0008 	01C1
		LJUMP 	157H 			//0009 	3957
		LJUMP 	18CH 			//000A 	398C

		//;random.C: 152: POWER_INITIAL();
		LCALL 	147H 			//000B 	3147
		ORG		000CH

		//;random.C: 153: Key_Old_Data = 1;
		CLRR 	68H 			//000C 	0168
		INCR	68H,1 			//000D 	09E8

		//;random.C: 154: unsigned int i;
		//;random.C: 155: unsigned int sleepflag;
		//;random.C: 156: sleepflag = 0;
		CLRR 	5EH 			//000E 	015E
		CLRR 	5FH 			//000F 	015F

		//;random.C: 158: {
		//;random.C: 159: PA2 =1;
		BCR 	STATUS,5 		//0010 	1283
		BSR 	5H,2 			//0011 	1905

		//;random.C: 160: if(PA4==1)
		BTSS 	5H,4 			//0012 	1E05
		LJUMP 	37H 			//0013 	3837
		ORG		0014H

		//;random.C: 161: {
		//;random.C: 162: if(Key_Old_Data==0)
		LDR 	68H,1 			//0014 	08E8
		BTSS 	STATUS,2 		//0015 	1D03
		LJUMP 	35H 			//0016 	3835

		//;random.C: 163: {
		//;random.C: 164: for(i=0;i<150;i++)
		CLRR 	60H 			//0017 	0160
		CLRR 	61H 			//0018 	0161

		//;random.C: 165: {
		//;random.C: 166: DelayMs(100);
		LDWI 	64H 			//0019 	2A64
		LCALL 	167H 			//001A 	3167

		//;random.C: 167: if(PA4==0)
		BCR 	STATUS,5 		//001B 	1283
		ORG		001CH
		BTSS 	5H,4 			//001C 	1E05
		LJUMP 	28H 			//001D 	3828
		INCR	60H,1 			//001E 	09E0
		BTSC 	STATUS,2 		//001F 	1503
		INCR	61H,1 			//0020 	09E1
		LDWI 	0H 			//0021 	2A00
		SUBWR 	61H,0 			//0022 	0C61
		LDWI 	96H 			//0023 	2A96
		ORG		0024H
		BTSC 	STATUS,2 		//0024 	1503
		SUBWR 	60H,0 			//0025 	0C60
		BTSS 	STATUS,0 		//0026 	1C03
		LJUMP 	19H 			//0027 	3819

		//;random.C: 170: }
		//;random.C: 171: }
		//;random.C: 172: if(i>=100)
		LDWI 	0H 			//0028 	2A00
		SUBWR 	61H,0 			//0029 	0C61
		LDWI 	64H 			//002A 	2A64
		BTSC 	STATUS,2 		//002B 	1503
		ORG		002CH
		SUBWR 	60H,0 			//002C 	0C60
		BTSS 	STATUS,0 		//002D 	1C03
		LJUMP 	35H 			//002E 	3835

		//;random.C: 173: {
		//;random.C: 174: sleepflag = 1;
		CLRR 	5EH 			//002F 	015E
		INCR	5EH,1 			//0030 	09DE
		CLRR 	5FH 			//0031 	015F

		//;random.C: 175: PA4_Level_Change_INITIAL();
		LCALL 	183H 			//0032 	3183

		//;random.C: 176: GIE = 1;
		BSR 	INTCON,7 		//0033 	1B8B
		ORG		0034H
		SLEEP	 			//0034 	0002

		//;random.C: 178: }
		//;random.C: 179: }
		//;random.C: 180: Key_Old_Data=1;
		CLRR 	68H 			//0035 	0168
		INCR	68H,1 			//0036 	09E8

		//;random.C: 181: }
		//;random.C: 182: if(PA4==0)
		BCR 	STATUS,5 		//0037 	1283
		BTSC 	5H,4 			//0038 	1605
		LJUMP 	10H 			//0039 	3810

		//;random.C: 183: {
		//;random.C: 184: if(Key_Old_Data==1)
		DECRSZ 	68H,0 		//003A 	0E68
		LJUMP 	10H 			//003B 	3810
		ORG		003CH

		//;random.C: 185: {
		//;random.C: 186: if(sleepflag==1)
		DECR 	5EH,0 			//003C 	0D5E
		IORWR 	5FH,0 			//003D 	035F
		BTSS 	STATUS,2 		//003E 	1D03
		LJUMP 	49H 			//003F 	3849
		LDWI 	2DH 			//0040 	2A2D

		//;random.C: 187: {
		//;random.C: 188: PA2=0;
		BCR 	5H,2 			//0041 	1105

		//;random.C: 189: DelayMs(45);
		LCALL 	167H 			//0042 	3167
		LDWI 	FH 			//0043 	2A0F
		ORG		0044H

		//;random.C: 190: PA2=1;
		BCR 	STATUS,5 		//0044 	1283
		BSR 	5H,2 			//0045 	1905

		//;random.C: 191: DelayMs(15);
		LCALL 	167H 			//0046 	3167

		//;random.C: 192: sleepflag=0;
		CLRR 	5EH 			//0047 	015E
		CLRR 	5FH 			//0048 	015F

		//;random.C: 193: }
		//;random.C: 194: num = address();
		LCALL 	132H 			//0049 	3132
		LDR 	5AH,0 			//004A 	085A
		STR 	66H 			//004B 	01E6
		ORG		004CH

		//;random.C: 195: WByte(num);
		LCALL 	C6H 			//004C 	30C6

		//;random.C: 196: Key_Old_Data=0;
		CLRR 	68H 			//004D 	0168
		LJUMP 	10H 			//004E 	3810
		CLRR 	53H 			//004F 	0153
		BTSS 	51H,7 			//0050 	1FD1
		LJUMP 	59H 			//0051 	3859
		COMR 	50H,1 			//0052 	0FD0
		COMR 	51H,1 			//0053 	0FD1
		ORG		0054H
		INCR	50H,1 			//0054 	09D0
		BTSC 	STATUS,2 		//0055 	1503
		INCR	51H,1 			//0056 	09D1
		CLRR 	53H 			//0057 	0153
		INCR	53H,1 			//0058 	09D3
		BTSS 	4FH,7 			//0059 	1FCF
		LJUMP 	60H 			//005A 	3860
		COMR 	4EH,1 			//005B 	0FCE
		ORG		005CH
		COMR 	4FH,1 			//005C 	0FCF
		INCR	4EH,1 			//005D 	09CE
		BTSC 	STATUS,2 		//005E 	1503
		INCR	4FH,1 			//005F 	09CF
		LDR 	4FH,0 			//0060 	084F
		IORWR 	4EH,0 			//0061 	034E
		BTSC 	STATUS,2 		//0062 	1503
		LJUMP 	7FH 			//0063 	387F
		ORG		0064H
		CLRR 	52H 			//0064 	0152
		INCR	52H,1 			//0065 	09D2
		BTSC 	4FH,7 			//0066 	17CF
		LJUMP 	6CH 			//0067 	386C
		BCR 	STATUS,0 		//0068 	1003
		RLR 	4EH,1 			//0069 	05CE
		RLR 	4FH,1 			//006A 	05CF
		LJUMP 	65H 			//006B 	3865
		ORG		006CH
		LDR 	4FH,0 			//006C 	084F
		SUBWR 	51H,0 			//006D 	0C51
		BTSS 	STATUS,2 		//006E 	1D03
		LJUMP 	72H 			//006F 	3872
		LDR 	4EH,0 			//0070 	084E
		SUBWR 	50H,0 			//0071 	0C50
		BTSS 	STATUS,0 		//0072 	1C03
		LJUMP 	7BH 			//0073 	387B
		ORG		0074H
		LDR 	4EH,0 			//0074 	084E
		SUBWR 	50H,1 			//0075 	0CD0
		LDR 	4FH,0 			//0076 	084F
		BTSS 	STATUS,0 		//0077 	1C03
		DECR 	51H,1 			//0078 	0DD1
		SUBWR 	51H,1 			//0079 	0CD1
		BCR 	STATUS,0 		//007A 	1003
		RRR	4FH,1 			//007B 	06CF
		ORG		007CH
		RRR	4EH,1 			//007C 	06CE
		DECRSZ 	52H,1 		//007D 	0ED2
		LJUMP 	6CH 			//007E 	386C
		LDR 	53H,0 			//007F 	0853
		BTSC 	STATUS,2 		//0080 	1503
		LJUMP 	87H 			//0081 	3887
		COMR 	50H,1 			//0082 	0FD0
		COMR 	51H,1 			//0083 	0FD1
		ORG		0084H
		INCR	50H,1 			//0084 	09D0
		BTSC 	STATUS,2 		//0085 	1503
		INCR	51H,1 			//0086 	09D1
		LDR 	51H,0 			//0087 	0851
		STR 	4FH 			//0088 	01CF
		LDR 	50H,0 			//0089 	0850
		STR 	4EH 			//008A 	01CE
		RET		 					//008B 	0004
		ORG		008CH
		LDR 	67H,1 			//008C 	08E7
		BTSS 	STATUS,2 		//008D 	1D03
		LJUMP 	93H 			//008E 	3893
		CLRR 	42H 			//008F 	0142
		INCR	42H,1 			//0090 	09C2
		CLRR 	43H 			//0091 	0143
		LCALL 	17AH 			//0092 	317A
		LDWI 	39H 			//0093 	2A39
		ORG		0094H
		STR 	56H 			//0094 	01D6
		LDWI 	30H 			//0095 	2A30
		STR 	57H 			//0096 	01D7
		CLRR 	58H 			//0097 	0158
		CLRR 	59H 			//0098 	0159
		LDR 	65H,0 			//0099 	0865
		STR 	45H 			//009A 	01C5
		LDR 	64H,0 			//009B 	0864
		ORG		009CH
		STR 	44H 			//009C 	01C4
		LDR 	63H,0 			//009D 	0863
		STR 	43H 			//009E 	01C3
		LDR 	62H,0 			//009F 	0862
		STR 	42H 			//00A0 	01C2
		LDWI 	41H 			//00A1 	2A41
		STR 	49H 			//00A2 	01C9
		LDWI 	C6H 			//00A3 	2AC6
		ORG		00A4H
		STR 	48H 			//00A4 	01C8
		LDWI 	4EH 			//00A5 	2A4E
		STR 	47H 			//00A6 	01C7
		LDWI 	6DH 			//00A7 	2A6D
		STR 	46H 			//00A8 	01C6
		LCALL 	FFH 			//00A9 	30FF
		LDR 	42H,0 			//00AA 	0842
		ADDWR 	56H,1 			//00AB 	0BD6
		ORG		00ACH
		LDR 	43H,0 			//00AC 	0843
		BTSC 	STATUS,0 		//00AD 	1403
		INCRSZ 	43H,0 		//00AE 	0A43
		ADDWR 	57H,1 			//00AF 	0BD7
		LDR 	44H,0 			//00B0 	0844
		BTSC 	STATUS,0 		//00B1 	1403
		INCRSZ 	44H,0 		//00B2 	0A44
		ADDWR 	58H,1 			//00B3 	0BD8
		ORG		00B4H
		LDR 	45H,0 			//00B4 	0845
		BTSC 	STATUS,0 		//00B5 	1403
		INCR	45H,0 			//00B6 	0945
		ADDWR 	59H,1 			//00B7 	0BD9
		LDR 	59H,0 			//00B8 	0859
		STR 	65H 			//00B9 	01E5
		LDR 	58H,0 			//00BA 	0858
		STR 	64H 			//00BB 	01E4
		ORG		00BCH
		LDR 	57H,0 			//00BC 	0857
		STR 	63H 			//00BD 	01E3
		LDR 	56H,0 			//00BE 	0856
		STR 	62H 			//00BF 	01E2
		LDR 	65H,0 			//00C0 	0865
		STR 	55H 			//00C1 	01D5
		LDR 	64H,0 			//00C2 	0864
		STR 	54H 			//00C3 	01D4
		ORG		00C4H
		BCR 	55H,7 			//00C4 	13D5
		RET		 					//00C5 	0004
		STR 	46H 			//00C6 	01C6

		//;random.C: 103: unsigned char i=8;
		LDWI 	8H 			//00C7 	2A08
		STR 	47H 			//00C8 	01C7
		LDWI 	7H 			//00C9 	2A07

		//;random.C: 104: PA2 = 0;
		BCR 	STATUS,5 		//00CA 	1283
		BCR 	5H,2 			//00CB 	1105
		ORG		00CCH

		//;random.C: 105: DelayMs(7);
		LCALL 	167H 			//00CC 	3167

		//;random.C: 106: unsigned char a;
		//;random.C: 107: while(i--)
		DECR 	47H,1 			//00CD 	0DC7
		LDR 	47H,0 			//00CE 	0847
		XORWI 	FFH 			//00CF 	26FF
		BTSC 	STATUS,2 		//00D0 	1503
		LJUMP 	FDH 			//00D1 	38FD

		//;random.C: 108: {
		//;random.C: 109: if(input&0x01)
		BTSS 	46H,0 			//00D2 	1C46
		LJUMP 	E8H 			//00D3 	38E8
		ORG		00D4H

		//;random.C: 110: {
		//;random.C: 111: PA2=1;
		BCR 	STATUS,5 		//00D4 	1283
		BSR 	5H,2 			//00D5 	1905

		//;random.C: 112: for(a=0;a<3;a++)
		CLRR 	48H 			//00D6 	0148

		//;random.C: 113: {
		//;random.C: 114: DelayUs(4);
		LDWI 	4H 			//00D7 	2A04
		LCALL 	171H 			//00D8 	3171
		LDWI 	3H 			//00D9 	2A03
		INCR	48H,1 			//00DA 	09C8
		SUBWR 	48H,0 			//00DB 	0C48
		ORG		00DCH
		BTSS 	STATUS,0 		//00DC 	1C03
		LJUMP 	D7H 			//00DD 	38D7

		//;random.C: 115: }
		//;random.C: 116: PA2=0;
		BCR 	STATUS,5 		//00DE 	1283
		BCR 	5H,2 			//00DF 	1105

		//;random.C: 117: for(a=0;a<1;a++)
		CLRR 	48H 			//00E0 	0148

		//;random.C: 118: {
		//;random.C: 119: DelayUs(4);
		LDWI 	4H 			//00E1 	2A04
		LCALL 	171H 			//00E2 	3171
		INCR	48H,1 			//00E3 	09C8
		ORG		00E4H
		LDR 	48H,0 			//00E4 	0848
		BTSC 	STATUS,2 		//00E5 	1503
		LJUMP 	E1H 			//00E6 	38E1
		LJUMP 	FAH 			//00E7 	38FA

		//;random.C: 122: else
		//;random.C: 123: {
		//;random.C: 124: PA2=1;
		BCR 	STATUS,5 		//00E8 	1283
		BSR 	5H,2 			//00E9 	1905

		//;random.C: 125: for(a=0;a<1;a++)
		CLRR 	48H 			//00EA 	0148

		//;random.C: 126: {
		//;random.C: 127: DelayUs(4);
		LDWI 	4H 			//00EB 	2A04
		ORG		00ECH
		LCALL 	171H 			//00EC 	3171
		INCR	48H,1 			//00ED 	09C8
		BTSC 	STATUS,2 		//00EE 	1503
		LJUMP 	EBH 			//00EF 	38EB

		//;random.C: 128: }
		//;random.C: 129: PA2=0;
		BCR 	STATUS,5 		//00F0 	1283
		BCR 	5H,2 			//00F1 	1105

		//;random.C: 130: for(a=0;a<3;a++)
		CLRR 	48H 			//00F2 	0148

		//;random.C: 131: {
		//;random.C: 132: DelayUs(4);
		LDWI 	4H 			//00F3 	2A04
		ORG		00F4H
		LCALL 	171H 			//00F4 	3171
		LDWI 	3H 			//00F5 	2A03
		INCR	48H,1 			//00F6 	09C8
		SUBWR 	48H,0 			//00F7 	0C48
		BTSS 	STATUS,0 		//00F8 	1C03
		LJUMP 	F3H 			//00F9 	38F3

		//;random.C: 133: }
		//;random.C: 134: }
		//;random.C: 135: input=input>>1;
		BCR 	STATUS,0 		//00FA 	1003
		RRR	46H,1 			//00FB 	06C6
		ORG		00FCH
		LJUMP 	CDH 			//00FC 	38CD

		//;random.C: 136: }
		//;random.C: 139: T0IE=0;
		BCR 	INTCON,5 		//00FD 	128B
		RET		 					//00FE 	0004
		CLRR 	4AH 			//00FF 	014A
		CLRR 	4BH 			//0100 	014B
		CLRR 	4CH 			//0101 	014C
		CLRR 	4DH 			//0102 	014D
		BTSS 	42H,0 			//0103 	1C42
		ORG		0104H
		LJUMP 	119H 			//0104 	3919
		LDR 	46H,0 			//0105 	0846
		ADDWR 	4AH,1 			//0106 	0BCA
		LDR 	47H,0 			//0107 	0847
		BCR 	STATUS,2 		//0108 	1103
		BTSC 	STATUS,0 		//0109 	1403
		ADDWI 	1H 			//010A 	2701
		BTSS 	STATUS,2 		//010B 	1D03
		ORG		010CH
		ADDWR 	4BH,1 			//010C 	0BCB
		LDR 	48H,0 			//010D 	0848
		BCR 	STATUS,2 		//010E 	1103
		BTSC 	STATUS,0 		//010F 	1403
		ADDWI 	1H 			//0110 	2701
		BTSS 	STATUS,2 		//0111 	1D03
		ADDWR 	4CH,1 			//0112 	0BCC
		LDR 	49H,0 			//0113 	0849
		ORG		0114H
		BCR 	STATUS,2 		//0114 	1103
		BTSC 	STATUS,0 		//0115 	1403
		ADDWI 	1H 			//0116 	2701
		BTSS 	STATUS,2 		//0117 	1D03
		ADDWR 	4DH,1 			//0118 	0BCD
		BCR 	STATUS,0 		//0119 	1003
		RLR 	46H,1 			//011A 	05C6
		RLR 	47H,1 			//011B 	05C7
		ORG		011CH
		RLR 	48H,1 			//011C 	05C8
		RLR 	49H,1 			//011D 	05C9
		BCR 	STATUS,0 		//011E 	1003
		RRR	45H,1 			//011F 	06C5
		RRR	44H,1 			//0120 	06C4
		RRR	43H,1 			//0121 	06C3
		RRR	42H,1 			//0122 	06C2
		LDR 	45H,0 			//0123 	0845
		ORG		0124H
		IORWR 	44H,0 			//0124 	0344
		IORWR 	43H,0 			//0125 	0343
		IORWR 	42H,0 			//0126 	0342
		BTSS 	STATUS,2 		//0127 	1D03
		LJUMP 	103H 			//0128 	3903
		LDR 	4DH,0 			//0129 	084D
		STR 	45H 			//012A 	01C5
		LDR 	4CH,0 			//012B 	084C
		ORG		012CH
		STR 	44H 			//012C 	01C4
		LDR 	4BH,0 			//012D 	084B
		STR 	43H 			//012E 	01C3
		LDR 	4AH,0 			//012F 	084A
		STR 	42H 			//0130 	01C2
		RET		 					//0131 	0004

		//;random.C: 144: unsigned int b;
		//;random.C: 145: b = rand()%10;
		LDWI 	AH 			//0132 	2A0A
		STR 	4EH 			//0133 	01CE
		ORG		0134H
		CLRR 	4FH 			//0134 	014F
		LCALL 	8CH 			//0135 	308C
		LDR 	55H,0 			//0136 	0855
		STR 	51H 			//0137 	01D1
		LDR 	54H,0 			//0138 	0854
		STR 	50H 			//0139 	01D0
		LCALL 	4FH 			//013A 	304F
		LDR 	4FH,0 			//013B 	084F
		ORG		013CH
		STR 	5DH 			//013C 	01DD
		LDR 	4EH,0 			//013D 	084E
		STR 	5CH 			//013E 	01DC

		//;random.C: 146: b = b+1;
		INCR	5CH,1 			//013F 	09DC
		BTSC 	STATUS,2 		//0140 	1503
		INCR	5DH,1 			//0141 	09DD

		//;random.C: 147: return b;
		LDR 	5DH,0 			//0142 	085D
		STR 	5BH 			//0143 	01DB
		ORG		0144H
		LDR 	5CH,0 			//0144 	085C
		STR 	5AH 			//0145 	01DA
		RET		 					//0146 	0004
		LDWI 	21H 			//0147 	2A21
		BSR 	STATUS,5 		//0148 	1A83
		STR 	FH 			//0149 	018F

		//;random.C: 34: INTCON = 0;
		CLRR 	INTCON 			//014A 	010B

		//;random.C: 35: PORTA = 0B00000000;
		BCR 	STATUS,5 		//014B 	1283
		ORG		014CH
		CLRR 	5H 			//014C 	0105

		//;random.C: 36: TRISA = 0B00010000;
		LDWI 	10H 			//014D 	2A10
		BSR 	STATUS,5 		//014E 	1A83
		STR 	5H 			//014F 	0185

		//;random.C: 37: OPTION = 0B10001000;
		LDWI 	88H 			//0150 	2A88
		STR 	1H 			//0151 	0181

		//;random.C: 38: WPUA = 0B00000000;
		CLRR 	15H 			//0152 	0115

		//;random.C: 39: MSCKCON = 0B00010000;
		LDWI 	10H 			//0153 	2A10
		ORG		0154H
		BCR 	STATUS,5 		//0154 	1283
		STR 	1BH 			//0155 	019B
		RET		 					//0156 	0004

		//;random.C: 46: if(PAIE && PAIF)
		BTSC 	INTCON,3 		//0157 	158B
		BTSS 	INTCON,0 		//0158 	1C0B
		LJUMP 	160H 			//0159 	3960

		//;random.C: 47: {
		//;random.C: 48: ReadAPin = PORTA;
		BCR 	STATUS,5 		//015A 	1283
		LDR 	5H,0 			//015B 	0805
		ORG		015CH

		//;random.C: 49: PAIF = 0;
		BCR 	INTCON,0 		//015C 	100B

		//;random.C: 50: PAIE = 0;
		BCR 	INTCON,3 		//015D 	118B

		//;random.C: 51: IOCA4 =0;
		BSR 	STATUS,5 		//015E 	1A83
		BCR 	16H,4 			//015F 	1216
		LDR 	41H,0 			//0160 	0841
		STR 	PCLATH 			//0161 	018A
		SWAPR 	40H,0 			//0162 	0740
		STR 	STATUS 			//0163 	0183
		ORG		0164H
		SWAPR 	7EH,1 			//0164 	07FE
		SWAPR 	7EH,0 			//0165 	077E
		RETI		 			//0166 	0009
		STR 	44H 			//0167 	01C4

		//;random.C: 67: unsigned char a;
		//;random.C: 68: for(a=0;a<Time;a++)
		CLRR 	45H 			//0168 	0145
		LDR 	44H,0 			//0169 	0844
		SUBWR 	45H,0 			//016A 	0C45
		BTSC 	STATUS,0 		//016B 	1403
		ORG		016CH
		RET		 					//016C 	0004

		//;random.C: 69: {
		//;random.C: 70: DelayUs(11);
		LDWI 	BH 			//016D 	2A0B
		LCALL 	171H 			//016E 	3171
		INCR	45H,1 			//016F 	09C5
		LJUMP 	169H 			//0170 	3969
		STR 	42H 			//0171 	01C2

		//;random.C: 58: unsigned char a;
		//;random.C: 59: for(a=0;a<Time;a++)
		CLRR 	43H 			//0172 	0143
		LDR 	42H,0 			//0173 	0842
		ORG		0174H
		SUBWR 	43H,0 			//0174 	0C43
		BTSC 	STATUS,0 		//0175 	1403
		RET		 					//0176 	0004

		//;random.C: 60: {
		//;random.C: 61: _nop();
		NOP		 					//0177 	0000
		INCR	43H,1 			//0178 	09C3
		LJUMP 	173H 			//0179 	3973
		LDR 	42H,0 			//017A 	0842
		STR 	62H 			//017B 	01E2
		ORG		017CH
		LDR 	43H,0 			//017C 	0843
		STR 	63H 			//017D 	01E3
		CLRR 	64H 			//017E 	0164
		CLRR 	65H 			//017F 	0165
		CLRR 	67H 			//0180 	0167
		INCR	67H,1 			//0181 	09E7
		RET		 					//0182 	0004

		//;random.C: 88: TRISA4 =1;
		BSR 	STATUS,5 		//0183 	1A83
		ORG		0184H
		BSR 	5H,4 			//0184 	1A05

		//;random.C: 89: ReadAPin = PORTA;
		BCR 	STATUS,5 		//0185 	1283
		LDR 	5H,0 			//0186 	0805

		//;random.C: 90: PAIF =0;
		BCR 	INTCON,0 		//0187 	100B

		//;random.C: 91: IOCA4 =1;
		BSR 	STATUS,5 		//0188 	1A83
		BSR 	16H,4 			//0189 	1A16

		//;random.C: 92: PAIE =1;
		BSR 	INTCON,3 		//018A 	198B
		RET		 					//018B 	0004
		ORG		018CH
		CLRR 	62H 			//018C 	0162
		CLRR 	63H 			//018D 	0163
		CLRR 	64H 			//018E 	0164
		CLRR 	65H 			//018F 	0165
		CLRR 	66H 			//0190 	0166
		CLRR 	67H 			//0191 	0167
		CLRR 	STATUS 			//0192 	0103
		LJUMP 	0BH 			//0193 	380B
			END
