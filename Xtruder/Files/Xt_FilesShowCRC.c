/*
**	$VER: XtruderFilesShowCRC.c (24.8.97)
**
**	Xtruder CRC check
**
**	Copyright (C) 1995-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Show the CRC value of a file. If filename is NULL request a file.
 */
VOID xShowCRC( STRPTR filename )
{
	UBYTE	filename_to_use[ DEFAULTBUFFERLEN ] = "";
	ULONG	filesize,crc;
	UBYTE *	buffer;
	
	
		/// GUI must be present
	
	if( Window )
		{
		xDisableWindow( Window );
		
		if( filename == NULL )
			{
			if( !xSelectFile( Window,
				SELFP_Dir,		RPATH,
				SELFP_DestFile,	filename_to_use,
				TAG_END ) )
				{
				xEnableWindow( Window );
	
				return;
				}
			
				/// Remember path
			
			strncpy( RPATH, FileReq->Dir, DEFAULTPATHLEN );
			}
		else
			strcpy( filename_to_use, filename );
		
			/// Read file into a buffer
		
		if( filesize = xFileSize( filename_to_use ) )
			{
			if( buffer = xStoreFile( filename_to_use, filesize ) )
				{
					/// Get CRC of buffer
				
				crc = xaDoCRC( buffer, filesize );
				
				xReqMsg( Window, GCS( MSG_REQ_INFO ), "%s - CRC %ld $%lx", GCS( MSG_GF_OKAY ), FilePart( filename_to_use ), crc, crc );
				
				FreeVec( buffer );
				}
			}
		
		xEnableWindow( Window );
		}
}
