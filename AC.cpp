// Copyright (C) 2007 Andrew S. Bantly
#include "stdafx.h"
#include "AC.h"

#include <memory>
using namespace std;

CArithmeticEncoder::CArithmeticEncoder()
{
	// Initialize the modeling data arrays
	InitModel();
}

CArithmeticEncoder::~CArithmeticEncoder()
{
}

// Initialze the modeling data
void CArithmeticEncoder::InitModel()
{
	memset(&m_ac,0,sizeof(m_ac));
	memset(&m_ac2,0,sizeof(m_ac2));
	memset(&m_ar,0,sizeof(m_ar));
	memset(&m_aMap,0,sizeof(m_aMap));
}

// Scale the counts for AC
__inline void CArithmeticEncoder::ScaleCounts()
{
	// Make a copy for 0 count checking
	for (int i = 0;i < 256;++i)
		m_ac2[i] = m_ac[i];

	// Scale to restrict to 14 bits for precision
	for (;;)
	{
		unsigned int uiTotal = 0;
		for (int i = 0;i < 256;++i)
		{
			uiTotal += m_ac2[i];
			if (uiTotal > 16383)
			{
				for (int j = 0;j < 256;++j)
				{
					m_ac2[j] >>= 1;
					if (m_ac2[j] == 0 && m_ac[j] != 0)
						m_ac2[j] = 1;
				}
				break;
			}
		}
		if (uiTotal > 16383)
			continue;
		break;
	}
}

// Build the scaled range values
__inline unsigned int CArithmeticEncoder::RangeCounts()
{
	unsigned int uiScale = 0;
	for (int i = 0;i < 256;++i)
	{
		m_ar[i] = uiScale;
		uiScale += m_ac2[i];
	}
	m_ar[256] = uiScale;

	// Return the scale
	return uiScale;
}

// Build the map that maps the range values back to a symbol for fast decoding
__inline void CArithmeticEncoder::BuildMap()
{
	for (unsigned int ui = 0;ui < 256;++ui)
		if (m_ac2[ui])
			for (unsigned short int uiRange = m_ar[ui];uiRange < m_ar[ui + 1];++uiRange)
				m_aMap[uiRange] = ui;
}

// Output a bit
__inline void CArithmeticEncoder::OutputBit(unsigned short int usiBit,unsigned char & ucOutSymbol,unsigned char & ucBitMask,unsigned long & ulDestCount,unsigned char * pBuffer)
{
	// Output the most significant bit
	if (usiBit)
		ucOutSymbol |= ucBitMask;
	ucBitMask >>= 1;

	// Test for output
	if (ucBitMask == 0)
	{
		// Output encoded byte
		pBuffer[ulDestCount++] = ucOutSymbol;
		ucOutSymbol = 0;
		ucBitMask = BITMASK;
	}
}

// Output the underflow bits
__inline void CArithmeticEncoder::OutputUnderflowBits(unsigned short int usiBit,unsigned long & ulUnderflowBits,unsigned char & ucOutSymbol,unsigned char & ucBitMask,unsigned long & ulDestCount,unsigned char * pBuffer)
{
	// Account for all underflow bits
	while (ulUnderflowBits)
	{
		// Output the most significant bit
		OutputBit(usiBit,ucOutSymbol,ucBitMask,ulDestCount,pBuffer);

		// Decrement the count
		ulUnderflowBits--;
	}
}

// Flush the bitmask, simulating shifting in bits
__inline void CArithmeticEncoder::FlushBitMask(unsigned char & ucBitMask,unsigned char & ucOutSymbol,unsigned long & ulDestCount,unsigned char * pBuffer)
{
	while (ucBitMask)
	{
		ucOutSymbol |= ucBitMask;
		ucBitMask >>= 1;
	}

	// Output remaining encoded byte
	pBuffer[ulDestCount++] = ucOutSymbol;
	ucOutSymbol = 0;
	ucBitMask = BITMASK;
}

// Implementation of Encode
bool CArithmeticEncoder::EncodeBuffer(char * pBufferIn,unsigned long ulnSrcCount,char ** ppBufferOut,unsigned long * pulnDestCount)
{
	// Initialize the modeling data arrays
	InitModel();

	// Input buffer
	unsigned char * pBuffer = (unsigned char *)pBufferIn;

	// The symbol and count
	unsigned char ucSymbol = 0;
	unsigned int uinSymbol = 0;

	// Counts and scaled counts
	unsigned long ulByte;
	for (ulByte = 0;ulByte < ulnSrcCount;++ulByte)
		uinSymbol += (m_ac[pBuffer[ulByte]]++ ? 0 : 1);
	if (!uinSymbol)
		return false;

	// Scale the counts
	ScaleCounts();

	// Get the count ranges
	unsigned int uiScale = RangeCounts();

	// Allocate the maximum output buffer (Total bytes encoded + Total symbol count + Symbols and counts + Max Output data)
	unsigned long ulnDestCount = 4 + 1 + 1280 + ulnSrcCount;
	*ppBufferOut = new char[ulnDestCount];
	unsigned char * pBufferOut = (unsigned char *)*ppBufferOut;

	// Starting buffer position is at the total symbol count
	unsigned long ulDestCount = 4;

	// Write the count of symbols modeling data
	pBufferOut[ulDestCount++] = (unsigned char)(uinSymbol - 1);

	// Write the symbol counts modeling data
	ucSymbol = 0; 
	do
	{
		if (m_ac[ucSymbol])
		{
			pBufferOut[ulDestCount++] = ucSymbol;
			pBufferOut[ulDestCount++] = (unsigned char)((m_ac[ucSymbol] & 0xFF000000) >> 24);
			pBufferOut[ulDestCount++] = (unsigned char)((m_ac[ucSymbol] & 0x00FF0000) >> 16);
			pBufferOut[ulDestCount++] = (unsigned char)((m_ac[ucSymbol] & 0x0000FF00) >> 8);
			pBufferOut[ulDestCount++] = (unsigned char)(m_ac[ucSymbol] & 0x000000FF);
		}
	} while (++ucSymbol);

	// Encode the data
	unsigned short int usiLow = 0,usiHigh = 0xFFFF;
	unsigned int uiRange = 0;
	unsigned long ulUnderflowBits = 0;

	// Output tracking
	unsigned char ucOutSymbol = 0;
	unsigned char ucBitMask = BITMASK;

	// Output bit
	unsigned short int usiBit = 0;

	// Main loop
	for (ulByte = 0;ulByte < ulnSrcCount;++ulByte)
	{
		// Get the symbol
		ucSymbol = pBuffer[ulByte];

		// Calculate the range, high value, and low value
		uiRange = (unsigned int)(usiHigh - usiLow + 1);
		usiHigh = usiLow + (unsigned short int)((uiRange * m_ar[ucSymbol + 1]) / uiScale - 1);
		usiLow = usiLow + (unsigned short int)((uiRange * m_ar[ucSymbol]) / uiScale);

		// Build onto the output
		for (;;)
		{
			// Check for output
			usiBit = usiHigh & MSB;
			if (usiBit == (usiLow & MSB))
			{
				// Output the most significant bit
				OutputBit(usiBit,ucOutSymbol,ucBitMask,ulDestCount,pBufferOut);

				// Output previous underflow bits first
				if (ulUnderflowBits > 0)
				{
					// Get the underflow bit
					usiBit = ~usiHigh & MSB;

					// Output the underflow bits
					OutputUnderflowBits(usiBit,ulUnderflowBits,ucOutSymbol,ucBitMask,ulDestCount,pBufferOut);
				}
			}
			else if ((usiLow & NSB) && !(usiHigh & NSB))
			{
				// Underflow prevention
				ulUnderflowBits++;
				usiHigh |= NSB;
				usiLow &= USB;
			}
			else
				break;

			// Shift the inputs
			usiHigh = usiHigh << 1;
			usiHigh |= 1;
			usiLow = usiLow << 1;
		}

		// Update the symbol count
		if (!(--m_ac[ucSymbol]))
		{
			// Scale the counts for the removed symbol
			ScaleCounts();

			// Get the range for the counts
			uiScale = RangeCounts();
		}
	}

	// Flush the encoder of the 2 low MSB's and any remaing underflow
	usiBit = usiLow & NSB;
	OutputBit(usiBit,ucOutSymbol,ucBitMask,ulDestCount,pBufferOut);
	usiBit = ~usiLow & NSB;
	OutputBit(usiBit,ucOutSymbol,ucBitMask,ulDestCount,pBufferOut);

	// Output the remaining underflow bits
	if (ulUnderflowBits > 0)
		OutputUnderflowBits(usiBit,ulUnderflowBits,ucOutSymbol,ucBitMask,ulDestCount,pBufferOut);

	// Flush out the bitmask
	if (ucBitMask)
		FlushBitMask(ucBitMask,ucOutSymbol,ulDestCount,pBufferOut);

	// Output the total bytes encoded
	pBufferOut[0] = (unsigned char)((ulnSrcCount & 0xFF000000) >> 24);
	pBufferOut[1] = (unsigned char)((ulnSrcCount & 0x00FF0000) >> 16);
	pBufferOut[2] = (unsigned char)((ulnSrcCount & 0x0000FF00) >> 8);
	pBufferOut[3] = (unsigned char)(ulnSrcCount & 0x000000FF);

	// Update the destination count
	*pulnDestCount = ulDestCount;

	return true;
}

// Implementation of decode
bool CArithmeticEncoder::DecodeBuffer(char * pBufferIn,unsigned long ulnSrcCount,char ** ppBufferOut,unsigned long * pulnDestCount)
{
	// Initialize the modeling data arrays
	InitModel();

	// Input buffer
	unsigned char * pBuffer = (unsigned char *)pBufferIn;
	unsigned long ulSrcCount = 0;

	// Read the total bytes encoded
	unsigned char uc4 = pBuffer[ulSrcCount++];
	unsigned char uc3 = pBuffer[ulSrcCount++];
	unsigned char uc2 = pBuffer[ulSrcCount++];
	unsigned char uc1 = pBuffer[ulSrcCount++];

	// Calculate the total bytes
	unsigned long ulTotal = (uc4 << 24) + (uc3 << 16) + (uc2 << 8) + uc1;
	*ppBufferOut = new char[ulTotal];
	unsigned char * pBufferOut = (unsigned char *)*ppBufferOut;
	*pulnDestCount = ulTotal;

	// The symbol
	unsigned char ucSymbol = 0;

	// Read the count of modeling data
	unsigned int uinSymbol = pBuffer[ulSrcCount++] + 1;

	// Read the modeling data
	for (unsigned int uin = 0;uin < uinSymbol;++uin)
	{
		ucSymbol = pBuffer[ulSrcCount++];
		uc4 = pBuffer[ulSrcCount++];
		uc3 = pBuffer[ulSrcCount++];
		uc2 = pBuffer[ulSrcCount++];
		uc1 = pBuffer[ulSrcCount++];
		unsigned long ulSymbolTotal = (uc4 << 24) + (uc3 << 16) + (uc2 << 8) + uc1;
		m_ac[ucSymbol] = ulSymbolTotal;
	}

	// Scale the counts
	ScaleCounts();

	// Get the range of counts
	unsigned int uiScale = RangeCounts();

	// Build the decode map of range to symbol for fast lookup
	BuildMap();

	// Initialize the code variables
	unsigned short int usiCode = 0;
	unsigned char ucCode = 0;

	// Read the first code word
	ucCode = pBuffer[ulSrcCount++];
	usiCode |= ucCode;
	usiCode <<= 8;
	ucCode = pBuffer[ulSrcCount++];
	usiCode |= ucCode;

	// Initialize the count of decoded characters and code
	unsigned long ulDestCount = 0;

	// Initialize the range
	unsigned short int usiLow = 0,usiHigh = 0xFFFF;
	unsigned int uiRange = 0;
	unsigned long ulUnderflowBits = 0;
	unsigned short int usiCount = 0;

	// The bit mask tracks the remaining bits in the input code buffer
	unsigned char ucBitMask = 0;

	// The main decoding loop
	do
	{
		// Get the range and count for the current arithmetic code
		uiRange = (unsigned int)(usiHigh - usiLow + 1);
		usiCount = (unsigned short int)((((usiCode - usiLow) + 1) * uiScale - 1) / uiRange);

		// Look up the symbol in the map
		ucSymbol = m_aMap[usiCount];

		// Output the symbol
		pBufferOut[ulDestCount++] = ucSymbol;
		if (ulDestCount >= ulTotal)
			break;

		// Calculate the high and low value of the symbol
		usiHigh = usiLow + (unsigned short int)((uiRange * m_ar[ucSymbol + 1]) / uiScale - 1);
		usiLow = usiLow + (unsigned short int)((uiRange * m_ar[ucSymbol]) / uiScale);

		// Remove the symbol from the stream
		for (;;)
		{
			if ((usiHigh & MSB) == (usiLow & MSB))
			{
				// Fall through to shifting
			}
			else if ((usiLow & NSB) == NSB && (usiHigh & NSB) == 0)
			{
				// Account for underflow
				usiCode ^= NSB;
				usiHigh |= NSB;
				usiLow &= USB;
			}
			else
				break;

			// Shift out a bit from the low and high values
			usiHigh <<= 1;
			usiHigh |= 1;
			usiLow <<= 1;

			// Shift out a bit from the code
			usiCode <<= 1;

			// Test for a needing an input symbol
			if (ucBitMask)
			{
shift:

				// Test for shifting in bits
				if (ucCode & ucBitMask)
					usiCode |= 1;
				ucBitMask >>= 1;
			}
			else
			{
				// Load up a new code
				if (ulSrcCount < ulnSrcCount)
				{
					ucCode = pBuffer[ulSrcCount++];
					ucBitMask = BITMASK;
					goto shift;
				}
				else if (ulDestCount < ulTotal)
				{
					// Helper with the last remaining symbols
					ucBitMask = BITMASK;
					goto shift;
				}
			}
		}

		// Update the symbol count
		if (!(--m_ac[ucSymbol]))
		{
			// Scale the counts for the removed symbol
			ScaleCounts();

			// Get the range of counts
			uiScale = RangeCounts();

			// Build the lookup map
			BuildMap();
		}
	} while (ulDestCount < ulTotal);

	return true;
}
