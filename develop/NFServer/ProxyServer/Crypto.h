#ifndef _CRYPTO_H
#define _CRYPTO_H

const unsigned int LICENSE_DATA0_VALUE = 0xE7990369;
const unsigned int LICENSE_DATA1_VALUE = 0x87930363;
const unsigned int LICENSE_DATA2_VALUE = 0x729C0606;
const unsigned int LICENSE_DATA3_VALUE = 0xF0990603;
const unsigned int LICENSE_DATA4_VALUE = 0x07A90306;
const unsigned int LICENSE_DATA5_VALUE = 0x37292304;
const unsigned int LICENSE_DATA6_VALUE = 0x879A2103;
const unsigned int LICENSE_DATA7_VALUE = 0xF3990304;
const unsigned int LICENSE_DATA8_VALUE = 0x373F0F03;
const unsigned int LICENSE_DATA9_VALUE = 0x8AA90322;
const unsigned int LICENSE_DATA10_VALUE = 0x85990303;
const unsigned int LICENSE_DATA11_VALUE = 0x379A0447;
const unsigned int LICENSE_DATA12_VALUE = 0x83990303;
const unsigned int LICENSE_DATA13_VALUE = 0xA7EE0557;
const unsigned int LICENSE_DATA14_VALUE = 0x8D9C53F3;
const unsigned int LICENSE_DATA15_VALUE = 0x579CC334;
const unsigned int LICENSE_DATA16_VALUE = 0x3A9EE303;
const unsigned int LICENSE_DATA17_VALUE = 0x97CC040D;
const unsigned int LICENSE_DATA18_VALUE = 0x0E9A0304;
const unsigned int LICENSE_DATA19_VALUE = 0x5399CC49;
const unsigned int LICENSE_DATA20_VALUE = 0x7FDD0303;
const unsigned int LICENSE_DATA21_VALUE = 0x8799530F;
const unsigned int LICENSE_DATA22_VALUE = 0x67CF0503;
const unsigned int LICENSE_DATA23_VALUE = 0x07D90F06;
const unsigned int LICENSE_DATA24_VALUE = 0x879C0307;
const unsigned int LICENSE_DATA25_VALUE = 0x57D9C306;
const unsigned int LICENSE_DATA26_VALUE = 0x07D90F05;
const unsigned int LICENSE_DATA27_VALUE = 0x8C9CF306;
const unsigned int LICENSE_DATA28_VALUE = 0x8B9903B3;
const unsigned int LICENSE_DATA29_VALUE = 0x0799F407;
const unsigned int LICENSE_DATA30_VALUE = 0x0E9C0803;
const unsigned int LICENSE_DATA31_VALUE = 0x87990309;

const unsigned char STATIC_KEY0_VALUE = 0x77;
const unsigned char STATIC_KEY1_VALUE = 0x56;
const unsigned char STATIC_KEY2_VALUE = 0xD3;
const unsigned char STATIC_KEY3_VALUE = 0x34;
const unsigned char STATIC_KEY4_VALUE = 0x2B;
const unsigned char STATIC_KEY5_VALUE = 0x8C;
const unsigned char STATIC_KEY6_VALUE = 0xE2;
const unsigned char STATIC_KEY7_VALUE = 0x2F;
const unsigned char STATIC_KEY8_VALUE = 0xED;
const unsigned char STATIC_KEY9_VALUE = 0xAC;
const unsigned char STATIC_KEY10_VALUE = 0xB8;
const unsigned char STATIC_KEY11_VALUE = 0x15;
const unsigned char STATIC_KEY12_VALUE = 0x45;
const unsigned char STATIC_KEY13_VALUE = 0x9E;
const unsigned char STATIC_KEY14_VALUE = 0x5E;
const unsigned char STATIC_KEY15_VALUE = 0xC4;

#pragma pack(push, 1)

union CRYPTO_INFO
{
	struct 
	{
		int	nLicenseData[32];
	};
	
	struct 
	{
		unsigned char data[252];
		unsigned int nCheckSum;
		
		union 
		{
			struct  
			{
				unsigned int nStartTime;
				unsigned int nEndTime;
			};

			unsigned char time[16];
		};
	};
};

#pragma pack(pop)

#endif // _CRYPTO_H

