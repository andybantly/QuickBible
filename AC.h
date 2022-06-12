// Copyright (C) 2007 Andrew S. Bantly
#pragma once

// CArithmeticEncoding (high level implementation)
#define BITMASK 0x80
#define MSB 0x8000
#define NSB 0x4000
#define USB 0x3FFF

class CArithmeticEncoder
{
public:
	CArithmeticEncoder();
	~CArithmeticEncoder();

	bool EncodeBuffer(char * pBufferIn,unsigned long ulnSrcCount,char ** ppBufferOut,unsigned long * pulnDestCount);
	bool DecodeBuffer(char * pBufferIn,unsigned long ulnSrcCount,char ** ppBufferOut,unsigned long * pulnDestCount);

protected:
	void InitModel();
	void ScaleCounts();
	unsigned int RangeCounts();
	void BuildMap();
	void OutputBit(unsigned short int usiBit,unsigned char & ucOutSymbol,unsigned char & ucBitMask,unsigned long & ulDestCount,unsigned char * pBuffer);
	void OutputUnderflowBits(unsigned short int usiBit,unsigned long & ulUnderflowBits,unsigned char & ucOutSymbol,unsigned char & ucBitMask,unsigned long & ulDestCount,unsigned char * pBuffer);
	void FlushBitMask(unsigned char & ucBitMask,unsigned char & ucOutSymbol,unsigned long & ulDestCount,unsigned char * pBuffer);

private:
	unsigned long m_ac[256];
	unsigned long m_ac2[256];
	unsigned short int m_ar[257];
	unsigned int m_aMap[16384];
};
