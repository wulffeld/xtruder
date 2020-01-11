/*
**	$VER: Xt_Check.c (26.4.97)
**
**	Xtruder main check functions
**
**	Copyright (C) 1993-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Starts check on a file/path/pattern...
 */
VOID xDoObject( struct CurrentCheck *cc, STRPTR object )
{
	struct CurrentFile *	cf;
	struct AnchorPath *		ap;
	UBYTE					strbuf1[ DEFAULTBUFFERLEN ];
	UBYTE					strbuf2[ DEFAULTBUFFERLEN ];
	UBYTE					patternbuf[ DEFAULTBUFFERLEN ];
	UWORD					a;
	UWORD					pathindention = XSettings->xs_FC_MIS_PathIndention;
	BOOL					enterdirs = XSettings->xs_FC_MIS_EnterRecursively;
	BOOL					pathhistory = XSettings->xs_HIS_RecFileCheck;
	BOOL					writedate = XSettings->xs_FC_INF_WrtInfo_Date;
	BOOL					writefiletype = XSettings->xs_FC_INF_WrtInfo_Filetype;
	LONG					iswild;
	
	
	UInfo->ui_Flag = UFN_NoLoad;
	
	INDENT[ 0 ] = '\0';
	Indent = 0;
	
	if( ap = AllocVec( sizeof( struct AnchorPath ) + 256, MEMF_PUBLIC | MEMF_CLEAR ) )
		{
		ap->ap_Strlen = 256;
		
		iswild = xParsePatternNoCase( XSettings->xs_FC_MIS_MatchFilter, patternbuf );
		
			/// Go through this path/file/pattern...
		
		if( !MatchFirst( object, ap ) )
			{
			do
				{
					/// Check if the user fiddled with the control window...
				
				switch( xHandleCheckWindowIDCMP( cc ) )
					{
					case CW_NOTHING:
						xCW_UpdateCheckInfo( cc );
						
							/// If it's a file...

						if( ap->ap_Info.fib_DirEntryType < 0 )
							{
							BPTR	oldcd = CurrentDir( ap->ap_Current->an_Lock );
							
							
							cc->cc_FilesEncountered++;
							
							if( NameFromLock( ap->ap_Current->an_Lock, strbuf1, DEFAULTBUFFERLEN ) )
								{
								if( !iswild || MatchPatternNoCase( patternbuf, ap->ap_Buf ) )
									{
									if( cf = AllocVec( sizeof( struct CurrentFile ), MEMF_ANY | MEMF_CLEAR ) )
										{
											/// Store information for later use...
										
										strcpy( cf->cf_PathName, strbuf1 );
										strcpy( cf->cf_FileName, ap->ap_Info.fib_FileName );
										strcpy( cf->cf_Comment, ap->ap_Info.fib_Comment );
										cf->cf_Size = ap->ap_Info.fib_Size;
										cf->cf_Protection = ap->ap_Info.fib_Protection;
										
											/// New name in filename gadget
										
										xCW_UpdateFileGadget( cf->cf_FileName, FALSE );
										
										xCHK_ATxt( pathhistory, FALSE, cf->cf_FileName,
											FI_Size,		cf->cf_Size,
											FI_Protection,	cf->cf_Protection,
											writedate ? FI_Date : TAG_IGNORE, writedate ? xCHK_FormatFileDate( &ap->ap_Info, strbuf1 ) : 0,
											writefiletype ? FI_Filetype : TAG_IGNORE, writefiletype ? xCHK_FileType( ap->ap_Info.fib_FileName, strbuf2 ) : 0,
											TAG_END );
										
											/// Take care of protection bits
										
										switch( xCHK_HandleFileProtection( ap->ap_Buf, cf->cf_Protection ) )
											{
												/// Read protection removed or not present...
											
											case FILEPROT_HANDLED:
												xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_READPROTECTIONREMOVED ), FilePart( ap->ap_Buf ) );
											case FILEPROT_NOTPROTECTED:
												switch( xCHK_HandleFilePBC( ap->ap_Buf, cf->cf_Protection ) )
													{
													case FILEPBC_CHECKIT:
														xCHK_DoFile( cc, cf );
														break;
													
													case FILEPBC_SKIPIT:
														xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEHASPBCBITS ), FilePart( ap->ap_Buf ) );
														break;
													}
												break;
											
												/// File couldn't be deprotected due
												/// to preferences settings...
											
											case FILEPROT_PROTECTED:
												xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEPROTECTED ), FilePart( ap->ap_Buf ) );
												break;
											
												/// Failed to remove read protection...
											
											case FILEPROT_FAIL:
												xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COULDNOTREMOVEPROTECTION ) );
												break;
											}
										
										FreeVec( cf );
										}
									}
								}
							CurrentDir( oldcd );
							}
						else
							{
								/// Enter subdirectory?
							
							if( enterdirs )
								{
								if( ap->ap_Flags & APF_DIDDIR )
									{
										/// We are finished with that dir...
									
									if( XSettings->xs_FC_CHW_History || pathhistory )
										{
										Indent -= pathindention;
										
										INDENT[ Indent ] = '\0';
										}
									}
								else
									{
									cc->cc_DirsEncountered++;
									
										/// New path in pathname gadget
									
									xCW_UpdatePathGadget( ap->ap_Buf, FALSE );
									
									if( XSettings->xs_FC_CHW_History || pathhistory )
										{
										strcpy( strbuf1, (PathPart( ap->ap_Buf ))[0] ? PathPart( ap->ap_Buf ) : ap->ap_Buf );
										strcpy( strbuf2, strbuf1 );
										
										if( strbuf1[0] == '/' )
											{
											for( a = 0; a < strlen( strbuf1 ); a++ )
												strbuf2[a] = strbuf1[a+1];
											
											strbuf2[a] = 0;
											}
										
											/// History
										
										xAddFileCheckHistoryNodes( 2, "%s%s", INDENT, strbuf2 );
										
										Indent += pathindention;
										
										for( a = 0; a < Indent; a++ )
											INDENT[ a ] = ' ';
										
										INDENT[ a ] = '\0';
										}
										
									ap->ap_Flags |= APF_DODIR;
									}
								
								ap->ap_Flags &= ~APF_DIDDIR;
								}
							}
						break;
					
					case CW_STOP:
						MatchEnd( ap );
						
						FreeVec( ap );
						
						cc->cc_Result = CCRES_USERBREAK;
						
						UInfo->ui_Flag = 0;
                        
						return;
						break;
					}
				}
			while( !MatchNext( ap ) && cc->cc_Result != CCRES_USERBREAK );
			}
		
		MatchEnd( ap );
		
		FreeVec( ap );
		}
	
	UInfo->ui_Flag = 0;
}

/*
 * Checks a file to determine if it's protected from reading and if so
 * sets appropreate bits...
 *
 * Returns FILEPROT_FAIL for failing to deprotect
 * Returns FILEPROT_HANDLED for deprotected file
 * Returns FILEPROT_NOTPROTECTED for deprotected file
 * Returns FILEPROT_PROTECTED for protected file (but preferences not set to deprotect)
 */
ULONG xCHK_HandleFileProtection( STRPTR filename, ULONG protbits )
{
	if( protbits & FIBF_READ )
		{
		if( XSettings->xs_FC_MIS_AutoDeprotect )
			{
			if( SetProtection( filename, 0 ) )
				return( FILEPROT_HANDLED );
			else
				return( FILEPROT_FAIL );
			}
		return( FILEPROT_PROTECTED );
		}
	
	return( FILEPROT_NOTPROTECTED );
}

/*
 * Checks if an ordinary file has the same protection bits as specified
 * in preferences/PBCBits...
 */
ULONG xCHK_HandleFilePBC( STRPTR filename, ULONG protbits )
{
	if( XSettings->xs_FC_PBC_PBCBits == protbits )
		{
		if( XSettings->xs_FC_PBC_CheckPBCFiles )
			return( FILEPBC_CHECKIT );
		else
			return( FILEPBC_SKIPIT );
		}
	
	return( FILEPBC_CHECKIT );
}

/*
 * Handle a file. This is the main function in the check process.
 */
VOID xCHK_DoFile( struct CurrentCheck *cc, struct CurrentFile *cf )
{
	BPTR	file = NULL;
	UBYTE *	filebuffer;
	ULONG	filesize = cf->cf_Size;
	STRPTR	filename = cf->cf_FileName;
	UBYTE	library_determine = 5;
	BOOL	extractarcs = XSettings->xs_FC_UNP_ExtractArchives;
	
	
	xCW_UpdateMemoryInfo();
	
		/// First read the file into a buffer (if we can)
	
	if( filebuffer = AllocVec( filesize, MEMF_CLEAR ) )
		{
		if( file = Open( filename, MODE_OLDFILE ) )
			{
			if( filesize == Read( file, filebuffer, filesize ) )
				{
				xCW_UpdateMemoryInfo();
				
					/// File has now been read. Let's see if it's an executable
				
				if( ((ULONG *)filebuffer)[0] == 0x000003F3 )
					{
						/// It sure was! Let's check it then.
					
					cc->cc_FilesChecked++;
					
					if( xCHK_CheckBuffer( filebuffer, filesize ) )
						{
							/// VIRUS FOUND! Fill in virus name etc.
						
						cc->cc_ViraFound++;
						strncpy( cf->cf_VirusName, FilevirusNode->fv_FileInfection->fi_VirusName, DEFAULTBUFFERLEN );
			
						if( XSettings->xs_HIS_RecFileCheck )
							XtruderHistory->his_ViraFound++;
						
							/// Deal with virus according to preferences
						
						xCHK_HandleVirus( cc, cf );
						
							/// Return from this function!
						
						FreeVec( filebuffer );
						
						Close( file );
						
						return;
						}
					
						/// Executable file was not a virus so let's mark it as checked! We do
						/// not want to set PBC bits on a virus in case the user forgets about
						/// it!
					
					if( XSettings->xs_FC_PBC_SetPBCOnExe )
						SetProtection( filename, XSettings->xs_FC_PBC_PBCBits );
					}
				
					/// File was not executable OR it was not a virus.
					/// Now check if we should decrunch files.
				
				if( XSettings->xs_FC_UNP_DecrunchExecutables )
					{
					if( library_determine = xCHK_IsFileCrunched( cf, filename, filebuffer, filesize ) )
						{
						cc->cc_FilesCrunched++;
						
							/// Decrunch and check file
						
						cf->cf_Buffer = filebuffer;
						cf->cf_BufferSize = filesize;
						
						xCHK_DecrunchExeOrDataAndCheck( cc, cf, NULL, library_determine );
						
							/// Since we have just found a file which was crunched we
							/// need not check if it also is an archive do we now?
						
						extractarcs = FALSE;
						}
					}
				}
			else
				{
				Close( file );
				file = NULL;
				
				FreeVec( filebuffer );
				filebuffer = NULL;
				}
			}
		else
			{
			FreeVec( filebuffer );
			filebuffer = NULL;
			}
		}
	
		/// Should we check archives (DMS,LZX,LhA)? Only possible with unpack.library
		/// so we simply upDetermine() once more.
	
	if( extractarcs )
		{
		BOOL	detsuc = TRUE;
		
		
			/// If the file was not recognized as crunched or recognized
			/// as crunched but by xfdmaster we must determine the type
			/// with the unpack library since we use that with archives.
		
		if( library_determine != LCRU_UNPACK ) 
			{
			if( filebuffer )
				{
					/// Because UFN_NoLoad flag is set (faster)
				
				UInfo->ui_CrunchAdr = filebuffer;
				UInfo->ui_CrunchLen = filesize;
				
				detsuc = upDetermineFile( UInfo, NULL );
				}
			else
				{
					/// File was not read into a buffer so try the slow way
				
				UInfo->ui_Flag &= ~UFN_NoLoad;
				
				detsuc = upDetermineFile( UInfo, filename );
				
				UInfo->ui_Flag |= UFN_NoLoad;
				}
			}
		
		if( detsuc )
			{
				/// Mask out possible encryption bit
			
			UInfo->ui_CrunchType &= 0x7F;
			
			switch( UInfo->ui_CrunchType )
				{
				case CRU_Track:
					if( XSettings->xs_FC_UNP_CheckDMSBootblock )
						{
							/// Must be DMS
						
						if( UInfo->ui_CrunchNum == CN_Dms )
							{
							cf->cf_ArchiveType = AT_DMS;
							
								/// Check the bootblock
							
							if( xReadDMSBootblock( cf->cf_FileName, FALSE ) )
								{
								struct Bootblock *	bb;
								LONG				ret;
								
								
								bb = (struct Bootblock *)CheckBoot( (ULONG *)XStrings->xs_Bootblock, &ret );
								
								switch( ret )
									{
									case BOOT_VIRUS:
										strcpy( cf->cf_VirusName, bb->bootname );
										
										xCHK_HandleVirus( cc, cf );
										break;
									}
								}
							else
								xAddFileCheckHistoryNodes( 3, GCS( MSG_COULDNOTCHECKBOOTBLOCK ), FilePart( cf->cf_FileName ) );
							}
						}
					break;
				
				case CRU_Archive:
					
						/// Free some memory
					
					FreeVec( filebuffer );
					filebuffer = NULL;
					
					xCW_UpdateMemoryInfo();
					
						/// Check which archiver that were used
					
					switch( UInfo->ui_CrunchNum )
						{
						case CN_Lha:
							if( XSettings->xs_FC_ARC_ExtractLhA && KeyWord )
								{
								cc->cc_FilesChecked++;
								cf->cf_ArchiveType = AT_LHA;
								
								if( xCHK_ExtractArchive( cc, cf, cf ) )
									{
										/// Mark archive as checked ?
									
									if( XSettings->xs_FC_PBC_SetPBCOnArchives )
										SetProtection( cf->cf_FileName, XSettings->xs_FC_PBC_PBCBits );
									}
								}
							break;
						
						case CN_Lzx:
							if( XSettings->xs_FC_ARC_ExtractLZX )
								{
								cc->cc_FilesChecked++;
								cf->cf_ArchiveType = AT_LZX;
								
								if( xCHK_ExtractArchive( cc, cf, cf ) )
									{
										/// Mark archive as checked ?
									
									if( XSettings->xs_FC_PBC_SetPBCOnArchives )
										SetProtection( cf->cf_FileName, XSettings->xs_FC_PBC_PBCBits );
									}
								}
							break;
						}
					break;
				}				/// switch( UInfo->ui_CrunchType )
			}					/// if( detsuc )
		}						/// if( extractarcs )
	
	if( file )
		Close( file );
	
	FreeVec( filebuffer );
}

/*
 * Find out according to preferences if a file is crunched.
 *
 * Returns:
 *   2 (LCRU_XFDMASTER) - File is crunched (determined by xfdmaster.library)
 *   1 (LCRU_UNPACK)    - File is crunched (determined by unpack.library)
 *   0                  - File is not crunched
 */
UBYTE xCHK_IsFileCrunched( struct CurrentFile *cf, STRPTR filename, UBYTE *filebuffer, ULONG filesize )
{
	switch( XSettings->xs_FC_UNP_WhenDecrunchingUse )
		{
		case WDU_UNPACKTHENXFD:
			UInfo->ui_CrunchAdr = filebuffer;
			UInfo->ui_CrunchLen = filesize;
			
			if( !upDetermineFile( UInfo, NULL ) )
				{
					/// Unpack.library could not determine the cruncher type. Try
					/// xfdmaster.library.
				
				XFDInfo->xfdbi_SourceBuffer = filebuffer;
				XFDInfo->xfdbi_SourceBufLen = filesize;
				
				if( xfdRecogBuffer( XFDInfo ) )
					{
					strncpy( cf->cf_CruncherName, XFDInfo->xfdbi_PackerName, 32 );
					
					return( LCRU_XFDMASTER );
					}
				}
			else
				{
				if( (UInfo->ui_CrunchType & 0x7F) == CRU_Object  || 
					(UInfo->ui_CrunchType & 0x7F) == CRU_Object2 ||
					(XSettings->xs_FC_UNP_DecrunchDataFiles && (UInfo->ui_CrunchType & 0x7F) == CRU_Data) )
					{
					strncpy( cf->cf_CruncherName, UInfo->ui_CruncherName, 32 );
					
					return( LCRU_UNPACK );
					}
				}
			break;
		
		case WDU_XFDTHENUNPACK:
			XFDInfo->xfdbi_SourceBuffer = filebuffer;
			XFDInfo->xfdbi_SourceBufLen = filesize;
			
			if( !xfdRecogBuffer( XFDInfo ) )
				{
					/// xfdmaster.library could not determine the cruncher type. Try
					/// Unpack.library.
				
				UInfo->ui_CrunchAdr = filebuffer;
				UInfo->ui_CrunchLen = filesize;
				
				if( upDetermineFile( UInfo, NULL ) )
					{
					if( (UInfo->ui_CrunchType & 0x7F) == CRU_Object  || 
						(UInfo->ui_CrunchType & 0x7F) == CRU_Object2 ||
						(XSettings->xs_FC_UNP_DecrunchDataFiles && (UInfo->ui_CrunchType & 0x7F) == CRU_Data) )
						{
						strncpy( cf->cf_CruncherName, UInfo->ui_CruncherName, 32 );
						
						return( LCRU_UNPACK );
						}
					}
				}
			else
				{
				strncpy( cf->cf_CruncherName, XFDInfo->xfdbi_PackerName, 32 );
				
				return( LCRU_XFDMASTER );
				}
			break;
		
		case WDU_UNPACK:
			UInfo->ui_CrunchAdr = filebuffer;
			UInfo->ui_CrunchLen = filesize;
			
			if( upDetermineFile( UInfo, NULL ) )
				{
				if( (UInfo->ui_CrunchType & 0x7F) == CRU_Object  || 
					(UInfo->ui_CrunchType & 0x7F) == CRU_Object2 ||
					(XSettings->xs_FC_UNP_DecrunchDataFiles && (UInfo->ui_CrunchType & 0x7F) == CRU_Data) )
					{
					strncpy( cf->cf_CruncherName, UInfo->ui_CruncherName, 32 );
					
					return( LCRU_UNPACK );
					}
				}
			break;
		
		case WDU_XFD:
			XFDInfo->xfdbi_SourceBuffer = filebuffer;
			XFDInfo->xfdbi_SourceBufLen = filesize;
			
			if( xfdRecogBuffer( XFDInfo ) )
				{
				strncpy( cf->cf_CruncherName, XFDInfo->xfdbi_PackerName, 32 );
				
				return( LCRU_XFDMASTER );
				}
			break;
		}
	
	return( 0 );
}

/*
 * Checks a file for vira using filevirus.library.
 *
 * Returns:
 *   1 for vira found
 *   0 for no vira or error while checking
 */
UBYTE xCHK_CheckBuffer( STRPTR buffer, ULONG size )
{
	ULONG	flags = FVCF_OnlyOne;
	
	
	FilevirusNode->fv_Buffer = buffer;
	FilevirusNode->fv_BufferLen = size;
	
	if( !fvTestHunks( FilevirusNode ) )
		{
			/// File MUST be executable if NoIntegrity is specified
		
		if( ((ULONG *)buffer)[0] == 0x000003F3 )
			flags |= FVCF_NoIntegrity;
		else
			return( 0 );
		}
	
	if( !fvCheckFile( FilevirusNode, flags ) )
		{
			/// VIRUS FOUND!
		
		if( FilevirusNode->fv_FileInfection )
			return( 1 );
		}
	
	return( 0 );
}

/*
 * Checks if a file is executable. If 'filename' is NON-NULL the function
 * will examine the buffer contents.
 */
UBYTE xCHK_IsFileExecutable( UBYTE *buffer )
{
		/// Examine buffer
	
	if( ((ULONG *)buffer)[0] == 0x000003F3 )
		return( 1 );
	
	return( 0 );
}
