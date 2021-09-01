// =================================================================================================
// Copyright Adobe
// Copyright 2008 Adobe
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================
#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.
#include "public/include/XMP_Const.h"

#include "XMPFiles/source/FormatSupport/PNG_Support.hpp"

#include "source/XIO.hpp"

#include <string.h>

typedef std::basic_string<unsigned char> filebuffer;

namespace CRC
{
    /* Table of CRCs of all 8-bit messages. */
    static unsigned long crc_table[256];
    
    /* Flag: has the table been computed? Initially false. */
    static int crc_table_computed = 0;
    
    /* Make the table for a fast CRC. */
    static void make_crc_table(void)
    {
        unsigned long c;
        int n, k;
        
        for (n = 0; n < 256; n++)
        {
            c = (unsigned long) n;
            for (k = 0; k < 8; k++)
            {
                if (c & 1)
                {
                    c = 0xedb88320L ^ (c >> 1);
                }
                else
                {
                    c = c >> 1;
                }
            }
            crc_table[n] = c;
        }
        crc_table_computed = 1;
    }
    
    /* Update a running CRC with the bytes buf[0..len-1]--the CRC
     should be initialized to all 1's, and the transmitted value
     is the 1's complement of the final running CRC (see the
     crc() routine below). */
    
    static unsigned long update_crc(unsigned long crc, unsigned char *buf, int len)
    {
        unsigned long c = crc;
        int n;
        
        if (!crc_table_computed)
        {
            make_crc_table();
        }
        
        for (n = 0; n < len; n++)
        {
            c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
        }
        
        return c;
    }
    
    /* Return the CRC of the bytes buf[0..len-1]. */
    static unsigned long crc(unsigned char *buf, int len)
    {
        return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
    }
} // namespace CRC

namespace PNG_Support
{
    enum chunkType {
        // Critical chunks - (shall appear in this order, except PLTE is optional)
        IHDR = 'IHDR',
        PLTE = 'PLTE',
        IDAT = 'IDAT',
        IEND = 'IEND',
        // Ancillary chunks - (need not appear in this order)
        cHRM = 'cHRM',
        gAMA = 'gAMA',
        iCCP = 'iCCP',
        sBIT = 'sBIT',
        sRGB = 'sRGB',
        bKGD = 'bKGD',
        hIST = 'hIST',
        tRNS = 'tRNS',
        pHYs = 'pHYs',
        sPLT = 'sPLT',
        tIME = 'tIME',
        iTXt = 'iTXt',
        tEXt = 'tEXt',
        zTXt = 'zTXt'
        
    };
    
    // =============================================================================================
    
    long OpenPNG ( XMP_IO* fileRef, ChunkState & inOutChunkState )
    {
        XMP_Int64 pos = 0;
        long name;
        XMP_Uns32 len;
        
        pos = fileRef->Seek ( 8, kXMP_SeekFromStart );
        if (pos != 8) return 0;
        
        // read first and following chunks
        while ( ReadChunk ( fileRef, inOutChunkState, &name, &len, pos) ) {}
        
        return (long)inOutChunkState.chunks.size();
        
    }
    
    // =============================================================================================
    
    bool  ReadChunk ( XMP_IO* fileRef, ChunkState & inOutChunkState, long * chunkType, XMP_Uns32 * chunkLength, XMP_Int64 & inOutPosition )
    {
        try
        {
            XMP_Uns64 startPosition = inOutPosition;
            long bytesRead;
            char buffer[8];
            
            bytesRead = fileRef->Read ( buffer, 8 );
            if ( bytesRead != 8 ) return false;
            inOutPosition += 8;
            *chunkLength = GetUns32BE(buffer);
            *chunkType = GetUns32BE(buffer + 4);
            
            inOutPosition += *chunkLength;
            inOutPosition += 4; //for CRC

			if(inOutPosition > fileRef->Length())
				XMP_Throw("Invalid PNG chunk length", kXMPErr_BadPNG);
            
            ChunkData	newChunk;
            
            newChunk.pos = startPosition;
            newChunk.len = *chunkLength;
            newChunk.type = *chunkType;
            
            // check for XMP in iTXt-chunk
            if (newChunk.type == iTXt)
            {
                CheckiTXtChunkHeader(fileRef, inOutChunkState, newChunk);
            }
            
            inOutChunkState.chunks.push_back ( newChunk );
            
            fileRef->Seek ( inOutPosition, kXMP_SeekFromStart );
            
        } catch ( ... ) {
			throw;
        }
        
        return true;
        
    }
    
    // =============================================================================================
    
    /*PNG file is structured in a series of chunks, where each chunk consists of four parts:
     Length: 4 bytes
     Chunk type: 4 bytes
     Chunk data: The data bytes of “Length” length.
     CRC: 4 bytes.
     
     XMP metadata is present in chunk of type “iTXt”.FindAndReadXMPChunk api will continue to
     find chunks until it finds "iTXt" chunk. XMP metadata is extracted from "iTXt" chunk by
     ExtractXMPPacket api. FindAndReadXMPChunk terminates if it finds "iTXt" chunk or end of 
     file is reached.
     */
    
	bool  FindAndReadXMPChunk(XMP_IO* fileRef, std::string& outXMPPacket, XMP_Int64& outXmpOffset, bool isOpenForRead)
	{
		XMP_Int64 inOutPosition = 8;

		XMP_Int64 fileLength = fileRef->Length();
		XMP_Int64 remainingBytes = fileLength - inOutPosition;

		XMP_Uns32 chunkType = 0;
		XMP_Uns32 chunkLength = 0;
		bool processedXMP = false;
		bool iendChunkFound = false;
		XMP_Uns32 currentBufferSize = 0;

		const XMP_Uns8 CRC = 4; //No. of bytes stores CRC info. 
		const XMP_Uns32 MaxBufferSize = 1024 * 1024; //max buffer.
		XMP_Uns8* buffer = NULL;
		outXMPPacket.clear();

		try {
			while (remainingBytes >= 8)
			{
				XMP_Int64 seekedPosition = fileRef->Seek(inOutPosition, kXMP_SeekFromStart);
				if (seekedPosition == -1)
				{
					XMP_Throw ( "seek failed", kXMPErr_BadPNG );
				}

				if (remainingBytes > MaxBufferSize)
				{
					currentBufferSize = MaxBufferSize;
				}
				else
				{
					currentBufferSize = static_cast<XMP_Uns32>(remainingBytes);
				}

				buffer = new XMP_Uns8[currentBufferSize];

				fileRef->Read(buffer, currentBufferSize, true);
				
				//Process the buffer.
				XMP_Uns8* tempBuffer = buffer;
				XMP_Uns8* bufferLimit = buffer + currentBufferSize;
				while (true)
				{
					chunkLength = GetUns32BE(tempBuffer);
					chunkType = GetUns32BE(tempBuffer + 4);

					tempBuffer += 8;

					if (chunkType == iTXt)
					{
						ExtractXMPPacket(fileRef, chunkLength, tempBuffer, bufferLimit - tempBuffer, inOutPosition, outXMPPacket, outXmpOffset);
						processedXMP = true;
						break;
					}
					else if (chunkType == IEND && isOpenForRead) {
						/*signifies end of png file. No need to process further.*/
						iendChunkFound = true;
						break;
					}

					//skip the current chunk.
					inOutPosition += 8 + static_cast<XMP_Int64>(chunkLength) + CRC;
					
					if(inOutPosition > fileLength)
					{
						XMP_Throw ( "Invalid PNG chunk length", kXMPErr_BadPNG );
					}

					//if current buffer have enough data for current chunk and next chunk's 8 bytes
					if ((bufferLimit - tempBuffer) > static_cast<XMP_Int64>((chunkLength + CRC + 8)))
					{
						tempBuffer += chunkLength + CRC;
						continue;
					}

					break;
				}

				delete[] buffer;
				buffer = NULL;

				if (processedXMP || iendChunkFound)
				{
					//if xmp packet found or IEND chunk found, no need to process further.
					break;
				}

				remainingBytes = fileLength - inOutPosition;
			}
		}
		catch(XMP_Error&)
		{
			delete[] buffer;
			buffer = NULL;
			throw;
		}
		catch (...)
		{
			delete[] buffer;
			buffer = NULL;
			throw;
		}

		return (outXMPPacket.size() != 0);
	}
           
    // =============================================================================================
    
    bool ExtractXMPPacket(XMP_IO* fileRef, XMP_Uns32 chunkLength, XMP_Uns8* buffer, XMP_Int64 bufferLength,XMP_Int64 filePosition, std::string& outXMPPacket,XMP_Int64& xmpOffset)
    {
        if(chunkLength <= ITXT_HEADER_LEN)
            return false;

        std::string packetStr;
        
        if(bufferLength >= chunkLength)
        {
            packetStr.assign((char*)buffer , chunkLength);
        }
        else
        {
			XMP_Int64 fileLength = fileRef->Length();
			filePosition += 8;

			if (filePosition + chunkLength + 4 > fileLength) //not enough data in file.
			{
				XMP_Throw ( "Invalid PNG chunk length", kXMPErr_BadPNG );
			}

            packetStr.assign((char*)buffer , bufferLength);
            xmpOffset = filePosition + ITXT_HEADER_LEN;
            filePosition += bufferLength;
            
            XMP_Uns32 remainingLength = chunkLength - static_cast<XMP_Uns32>(bufferLength);
            std::string remainingPacket;
            remainingPacket.reserve(remainingLength);
            remainingPacket.assign(remainingLength, ' ');
            
            fileRef->Seek ( filePosition, kXMP_SeekFromStart );
            long bytesRead = fileRef->Read ( const_cast<char *>(remainingPacket.data()), remainingLength, true);
            packetStr += remainingPacket;
        }
        
        if (packetStr.compare(0, ITXT_HEADER_LEN, std::string(ITXT_HEADER_DATA, ITXT_HEADER_LEN)) == 0)
        {
            outXMPPacket.clear();
            outXMPPacket = packetStr.substr(ITXT_HEADER_LEN);
        }
        else
        {
            return false;
        }
        
        return true;
    }
    
    
    
    // =============================================================================================
    
    bool WriteXMPChunk ( XMP_IO* fileRef, XMP_Uns32 len, const char* inBuffer )
    {
        bool ret = false;
        XMP_Uns32 datalen = (4 + ITXT_HEADER_LEN + len);
        unsigned char* buffer = new unsigned char[datalen];
        
        try
        {
            size_t pos = 0;
            memcpy(&buffer[pos], ITXT_CHUNK_TYPE, 4);
            pos += 4;
            memcpy(&buffer[pos], ITXT_HEADER_DATA, ITXT_HEADER_LEN);
            pos += ITXT_HEADER_LEN;
            memcpy(&buffer[pos], inBuffer, len);
            
            XMP_Uns32 crc_value = MakeUns32BE( (XMP_Uns32)CalculateCRC( buffer, datalen ));
            datalen -= 4;
            XMP_Uns32 len_value = MakeUns32BE( datalen );
            datalen += 4;
            
            fileRef->Write ( &len_value, 4 );
            fileRef->Write ( buffer, datalen );
            fileRef->Write ( &crc_value, 4 );
            
            ret = true;
        }
        catch ( ... ) {}
        
        delete [] buffer;
        
        return ret;
    }
    
    // =============================================================================================
    
    bool CopyChunk ( XMP_IO* sourceRef, XMP_IO* destRef, ChunkData& chunk )
    {
        try
        {
            sourceRef->Seek ( chunk.pos, kXMP_SeekFromStart  );
            XIO::Copy (sourceRef, destRef, (chunk.len + 12));
            
        } catch ( ... ) {
            
            return false;
            
        }
        
        return true;
    }
    
    // =============================================================================================
    
    unsigned long UpdateChunkCRC( XMP_IO* fileRef, ChunkData& inOutChunkData )
    {
        unsigned long ret = 0;
        unsigned long datalen = (inOutChunkData.len + 4);
        unsigned char* buffer = new unsigned char[datalen];
        
        try
        {
            fileRef->Seek ( (inOutChunkData.pos + 4), kXMP_SeekFromStart );
            
            size_t pos = 0;
            long bytesRead = fileRef->Read ( &buffer[pos], (inOutChunkData.len + 4) );
            
            XMP_Uns32 crc = (XMP_Uns32)CalculateCRC( buffer, (inOutChunkData.len + 4) );
            XMP_Uns32 crc_value = MakeUns32BE( crc );
            
            fileRef->Seek ( (inOutChunkData.pos + 4 + 4 + inOutChunkData.len), kXMP_SeekFromStart );
            fileRef->Write ( &crc_value, 4 );
            
            ret = crc;
        }
        catch ( ... ) {}
        
        delete [] buffer;
        
        return ret;
    }
    
    // =============================================================================================
    
    bool CheckIHDRChunkHeader ( ChunkData& inOutChunkData )
    {
        return (inOutChunkData.type == IHDR);
    }
    
    // =============================================================================================
    
    unsigned long CheckiTXtChunkHeader ( XMP_IO* fileRef, ChunkState& inOutChunkState, ChunkData& inOutChunkData )
    {
        try
        {
            if(inOutChunkData.len <= ITXT_HEADER_LEN)
                return 0;
            
            fileRef->Seek ( (inOutChunkData.pos + 8), kXMP_SeekFromStart );
            
            char buffer[ITXT_HEADER_LEN];
            long bytesRead = fileRef->Read ( buffer, ITXT_HEADER_LEN );
            
            if (bytesRead == ITXT_HEADER_LEN)
            {
                if (memcmp(buffer, ITXT_HEADER_DATA, ITXT_HEADER_LEN) == 0)
                {
                    // return length of XMP
                    
                    inOutChunkState.xmpPos = inOutChunkData.pos + 8 + ITXT_HEADER_LEN;
                    inOutChunkState.xmpLen = inOutChunkData.len - ITXT_HEADER_LEN;
                    inOutChunkState.xmpChunk = inOutChunkData;
                    inOutChunkData.xmp = true;
                    
                    return inOutChunkState.xmpLen;
                    
                }
            }
        }
        catch ( ... ) {}
        
        return 0;
    }
    
    bool ReadBuffer ( XMP_IO* fileRef, XMP_Uns64 & pos, XMP_Uns32 len, char * outBuffer )
    {
        try
        {
            if ( (fileRef == 0) || (outBuffer == 0) ) return false;
            
            fileRef->Seek ( pos, kXMP_SeekFromStart  );
            long bytesRead = fileRef->Read ( outBuffer, len );
            if ( XMP_Uns32(bytesRead) != len ) return false;
            
            return true;
        }
        catch ( ... ) {}
        
        return false;
    }
    
    bool WriteBuffer ( XMP_IO* fileRef, XMP_Uns64 & pos, XMP_Uns32 len, const char * inBuffer )
    {
        try
        {
            if ( (fileRef == 0) || (inBuffer == 0) ) return false;
            
            fileRef->Seek ( pos, kXMP_SeekFromStart  );
            fileRef->Write ( inBuffer, len  );
            
            return true;
        }
        catch ( ... ) {}
        
        return false;
    }
    
    unsigned long CalculateCRC( unsigned char* inBuffer, XMP_Uns32 len )
    {
        return CRC::update_crc(0xffffffffL, inBuffer, len) ^ 0xffffffffL;
    }
    
} // namespace PNG_Support
