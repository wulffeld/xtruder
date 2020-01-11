/*
**	$VER: Xt_FilesRepair.c (24.8.97)
**
**	Xtruder repair
**
**	Copyright (C) 1995-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Repair a file. If filename is NULL request a file.
 */
VOID xRepairFile( STRPTR filename )
{
	UBYTE	filename_to_use[ DEFAULTBUFFERLEN ] = "";
	ULONG	filesize;
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
		
			/// Is it an executable?
		
		if( xIsExecutable( filename_to_use ) )
			{
				/// Read file into a buffer
			
			if( filesize = xFileSize( filename_to_use ) )
				{
				if( buffer = xStoreFile( filename_to_use, filesize ) )
					{
						/// Check file
					
					FilevirusNode->fv_Buffer = buffer;
					FilevirusNode->fv_BufferLen = filesize;
					
					if( !fvCheckFile( FilevirusNode, FVCF_OnlyOne | FVCF_NoIntegrity ) )
						{
						if( FilevirusNode->fv_FileInfection )
							{
							BPTR	file;
							
							
								/// VIRUS FOUND!
							
							switch( fvRepairFile( FilevirusNode, NULL, 0 ) )
								{
								case FVMSG_DELETE:
								case FVMSG_RENAME:
									xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_FILECANONLYBEDELETED ), GCS( MSG_GF_OKAY ), FilePart( filename_to_use ) );
									break;
								
								case FVMSG_SAVE:
									if( file = Open( filename_to_use, MODE_NEWFILE ) )
										{
										Write( file, buffer, FilevirusNode->fv_BufferLen );
										
										Close( file );
										
										xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_INFECTEDHUNKCODEREMOVEDFILE ), GCS( MSG_GF_OKAY ) );
										}
									else
										xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_COULDNOTSAVEREPAIREDFILE ), GCS( MSG_GF_OKAY ) );
									break;
								
								default:
									xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_NOACTIONTAKEN ), GCS( MSG_GF_OKAY ) );
									break;
								}
							}
						else
							xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_FILEISNOTINFECTED ), GCS( MSG_GF_OKAY ), FilePart( filename_to_use ) );
						}
					FreeVec( buffer );
					}
				}
			}
		else
			xReqMsg( Window, GCS( MSG_REQ_INFO ), "'%s' is not executable", GCS( MSG_GF_OKAY ), FilePart( filename_to_use ) );
		
		xEnableWindow( Window );
		}
}
