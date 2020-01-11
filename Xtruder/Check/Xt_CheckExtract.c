/*
**	$VER: XtruderCheckExtract.c (8.2.96)
**
**	Xtruder check extract module
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		DELETE_ALL	1
#define		DELETE_ONE	2
#define		SKIP_ONE	3
#define		SKIP_ALL	4

/*
 * Used ONLY in conjunction with xDoFile() and xDoExtractedFile()
 *
 * Extract an archive...
 */
BOOL xCHK_ExtractArchive( struct CurrentCheck *cc, struct CurrentFile *cf, struct CurrentFile *oldarchivecf )
{
	struct CurrentFile *	archivecf;
	struct List *			list;
	struct FileInfoBlock *	fib;
	struct ArchiveNode *	node;
	BOOL					success = FALSE;
	UBYTE					strbuf1[ DEFAULTBUFFERLEN ];
	UBYTE					strbuf2[ DEFAULTBUFFERLEN ];
	UBYTE					strbuf3[ DEFAULTBUFFERLEN ];
	UBYTE					oldpath[ DEFAULTBUFFERLEN ] = "";
	UBYTE					newpath[ DEFAULTBUFFERLEN ] = "";
	UBYTE					indention[ 100 ];
	UWORD					pathslashes,pathlength,n;
	STRPTR					pathptr;
	BPTR					file,out,outnil,lock,cd;
	LONG					controlmsg;
	UWORD					delete = DELETE_ONE;
	
	
		/// Should we ask the user first?
	
	if( XSettings->xs_FC_UNP_AskBeforeExtracting )
		{
		if( !xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_EXTRACTTHEARCHIVE ), GCS( MSG_GF_YESNO ), FilePart( oldarchivecf->cf_FileName ), oldarchivecf->cf_Size ) )
			return( FALSE );
		}
	
		/// Write out that we are beginning to extract archive...
	
	xAddFileCheckHistoryNodes( 1, GCS( MSG_EXTRACTINGARCHIVE ), FilePart( oldarchivecf->cf_FileName ), oldarchivecf->cf_Size );
	
		/// Finally extract with one of the internal routines...
	
	if( (XSettings->xs_FC_ARC_LhAExtractRoutine == XP_INTERNAL1EXTRACT && oldarchivecf->cf_ArchiveType == AT_LHA)
		||
		(XSettings->xs_FC_ARC_LZXExtractRoutine == XP_INTERNAL1EXTRACT && oldarchivecf->cf_ArchiveType == AT_LZX) )
		{
			/// Internal routine 1
		
		if( list = xSetUpInternal_1_ArchiveList( oldarchivecf->cf_FileName, oldarchivecf->cf_ArchiveType, cc ) )
			{
				/// Now that we have made a list of the valid files which we may
				/// check let's extract the valid files.
			
			switch( oldarchivecf->cf_ArchiveType )
				{
				case AT_LHA:
					xSPrintF( strbuf1, "LhA x -mM \"%s\" #? \"%s%s\"",
						oldarchivecf->cf_FileName,
						XSettings->xs_PAT_ArchiveExtractPath,
						XSettings->xs_PAT_ArchiveExtractPath[ strlen( XSettings->xs_PAT_ArchiveExtractPath )-1 ] == ':' ? "\0" : "/" );
						break;
				
				case AT_LZX:
					xSPrintF( strbuf1, "LZX x -m \"%s\" \"%s%s\"",
						oldarchivecf->cf_FileName,
						XSettings->xs_PAT_ArchiveExtractPath,
						XSettings->xs_PAT_ArchiveExtractPath[ strlen( XSettings->xs_PAT_ArchiveExtractPath )-1 ] == ':' ? "\0" : "/" );
					break;
				}
			
				/// Extract a file from the archive
			
			if( outnil = Open( "NIL:", MODE_NEWFILE ) )
				{
					/// Extract as much as possible
				
	 			SystemTags( strbuf1, SYS_Output, outnil, TAG_DONE );
				
				if( fib = AllocDosObject( DOS_FIB, TAG_DONE ) )
					{
						/// Go through the list of files in the archive...
					
					if( archivecf = AllocVec( sizeof( struct CurrentFile ), MEMF_CLEAR ) )
						{
						success = TRUE;
						
						for( node = (struct ArchiveNode *)list->lh_Head;
							 node->an_Node.ln_Succ;
							 node = (struct ArchiveNode *)node->an_Node.ln_Succ )
							{
							cc->cc_ArchivedFilesExtracted++;
							
							controlmsg = xHandleCheckWindowIDCMP( cc );
							
							if( controlmsg == CW_STOP )
								{
								cc->cc_Result = CCRES_USERBREAK;
								
								break;
								}
							else
								{
								if( lock = Lock( XSettings->xs_PAT_ArchiveExtractPath, ACCESS_READ ) )
									{
									cd = CurrentDir( lock );
									
										/// Clear protection flags on the extracted file
									
									SetProtection( node->an_Name, 0 );
									
										/// Get the path of the file
									
									for( pathslashes = 0, pathptr = node->an_Name; *pathptr != 0; pathptr++ )
										{
										if( *pathptr == '/' )
											pathslashes++;
										}
									
									if( pathslashes )
										{
											/// Search back to the last slash
										
										while( *pathptr != '/' )
											pathptr--;
										
										pathlength = pathptr - node->an_Name;
										strncpy( newpath, node->an_Name, pathlength );
										newpath[ pathlength ] = 0;
										
											/// Compare new isolated path with old path
										
										if( stricmp( oldpath, newpath ) )
											{
												/// Add indention
											
											for( n = 0, indention[0] = 0; n < pathslashes; n++ )
												strcat( indention, "  " );
											
												/// Search from end of line for the last '/'
											
											for( pathptr = node->an_Name + strlen( node->an_Name ) - 1; *pathptr != 0; pathptr-- )
												{
												if( *pathptr == '/' )
													{
													while( pathptr > node->an_Name )
														{
														pathptr--;
														
														if( *pathptr == '/' )
															{
															pathptr++;			/// Lose the slash
															
															break;
															}
														}
													
														/// Finally copy..
													
													n = 0;
													
													while( *pathptr != '/' )
														strbuf3[ n++ ] = *pathptr++;
													
													strbuf3[ n ] = 0;
													break;
													}
												}
											
											xAddFileCheckHistoryNodes( 2, "%s>> %s", indention, strbuf3 );
											
											strcpy( oldpath, newpath );
											}
										}
									
										/// Lock the file and check it
									
									if( file = Lock( node->an_Name, ACCESS_READ ) )
										{
										if( Examine( file, fib ) )
											{
											strcpy( archivecf->cf_FileName, node->an_Name );
											strcpy( archivecf->cf_Comment, fib->fib_Comment );
											archivecf->cf_Size = fib->fib_Size;
											archivecf->cf_Result = 0;
											
												/// Write the filename
											
											xCHK_ATxt( XSettings->xs_HIS_RecFileCheck, TRUE, fib->fib_FileName,
												FI_ExtractedFullName,	node->an_Name,
												FI_Size,				fib->fib_Size,
												FI_Protection,			fib->fib_Protection,
												XSettings->xs_FC_INF_WrtInfo_Date ? FI_Date : TAG_IGNORE, XSettings->xs_FC_INF_WrtInfo_Date ? xCHK_FormatFileDate( fib, strbuf1 ) : 0,
												XSettings->xs_FC_INF_WrtInfo_Filetype ? FI_Filetype : TAG_IGNORE, XSettings->xs_FC_INF_WrtInfo_Filetype ? xCHK_FileType( fib->fib_FileName, strbuf2 ) : 0,
												TAG_END );
											
												/// Check the file
											
											xCHK_DoExtractedFile( cc, cf, archivecf );
											}
										UnLock( file );
										}
									
										/// Delete temp file ?
									
									if( XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles )
										{
										switch( delete )
											{
											case DELETE_ALL:
												DeleteFile( node->an_Name );
												break;
											
											case DELETE_ONE:
											case SKIP_ONE:
												if( 2 <= (delete = xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_DELETEEXTRACTEDFILE ), GCS( MSG_FC_GF_DELETEEXTRACTED ), node->an_Name ) ) )
													DeleteFile( node->an_Name );
												break;
											}
										}
									
									CurrentDir( cd );
									
									UnLock( lock );
									}
								}
							}
						
							/// Only delete all in temp if this is the first call
							/// from the main check routine. This could be a
							/// recursive call where would then wrongly delete
							/// some files that haven't been checked.
						
						if( cf == oldarchivecf )
							xCHK_DeleteAll( XSettings->xs_PAT_ArchiveExtractPath, XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles );
						
						FreeVec( archivecf );
						}
					FreeDosObject( DOS_FIB, fib );
					}
				
				Close( outnil );
				}
			
			xFreeArchiveNodes( list );
			
			FreeVec( list );
			}
		}
	else
		{
			/// Internal routine 2
		
		if( list = xSetUpInternal_2_ArchiveList( oldarchivecf->cf_FileName, oldarchivecf->cf_ArchiveType, cc ) )
			{
			if( fib = AllocDosObject( DOS_FIB, TAG_DONE ) )
				{
					/// CON title & CON
				
				if( strstr( XSettings->xs_FC_ARC_Output, "%s" ) )
					{
					xSPrintF( strbuf2, GCS( MSG_CONTL_EXTRACTING ), FilePart( oldarchivecf->cf_FileName ), oldarchivecf->cf_Size );
					xSPrintF( strbuf3, XSettings->xs_FC_ARC_Output, strbuf2 );
					}
				else
					strcpy( strbuf3, XSettings->xs_FC_ARC_Output );
				
				if( out = Open( strbuf3, MODE_NEWFILE ) )
					{
					if( archivecf = AllocVec( sizeof( struct CurrentFile ), MEMF_CLEAR ) )
						{
						BOOL	filesmall;
						
						
						success = TRUE;
						
							/// Go through the list of files in the archive...
						
						for( node = (struct ArchiveNode *)list->lh_Head;
							 node->an_Node.ln_Succ;
							 node = (struct ArchiveNode *)node->an_Node.ln_Succ )
							{
							controlmsg = xHandleCheckWindowIDCMP( cc );
							
							if( controlmsg == CW_STOP )
								{
								cc->cc_Result = CCRES_USERBREAK;
								
								break;
								}
							else
								{
									/// Is it small enough ?
								
								filesmall = TRUE;
								
								if( XSettings->xs_FC_ARC_LhAMaxSize || XSettings->xs_FC_ARC_LZXMaxSize )
									{
									switch( oldarchivecf->cf_ArchiveType )
										{
										case AT_LHA:
											if( XSettings->xs_FC_ARC_LhAMaxSize != 0 && XSettings->xs_FC_ARC_LhAMaxSize < node->an_ExtractedSize )
												filesmall = FALSE;
											break;
										
										case AT_LZX:
											if( XSettings->xs_FC_ARC_LZXMaxSize != 0 && XSettings->xs_FC_ARC_LZXMaxSize < node->an_ExtractedSize )
												filesmall = FALSE;
											break;
										}
									}
								
								if( filesmall )
									{
									switch( oldarchivecf->cf_ArchiveType )
										{
										case AT_LHA:
											xSPrintF( strbuf2, "LhA x -mM \"%s\" \"%s\" \"%s%s\"",
												oldarchivecf->cf_FileName,
												node->an_Name,
												XSettings->xs_PAT_ArchiveExtractPath,
												XSettings->xs_PAT_ArchiveExtractPath[ strlen( XSettings->xs_PAT_ArchiveExtractPath )-1 ] == ':' ? "\0" : "/" );
												break;
										
										case AT_LZX:
											xSPrintF( strbuf2, "LZX x -m \"%s\" \"%s\" \"%s%s\"",
												oldarchivecf->cf_FileName,
												node->an_Name,
												XSettings->xs_PAT_ArchiveExtractPath,
												XSettings->xs_PAT_ArchiveExtractPath[ strlen( XSettings->xs_PAT_ArchiveExtractPath )-1 ] == ':' ? "\0" : "/" );
											break;
										}
									
										/// Extract a file from the archive
									
									if( !SystemTags( strbuf2, 
										SYS_Output, out,
										TAG_DONE ) )
										{
										cc->cc_ArchivedFilesExtracted++;
										
										if( lock = Lock( XSettings->xs_PAT_ArchiveExtractPath, ACCESS_READ ) )
											{
											cd = CurrentDir( lock );
											
												/// Clear protection on extracted file
											
											SetProtection( node->an_Name, 0 );
											
											if( file = Lock( node->an_Name, ACCESS_READ ) )
												{
												if( Examine( file, fib ) )
													{
													strcpy( archivecf->cf_FileName, node->an_Name );
													strcpy( archivecf->cf_Comment, fib->fib_Comment );
													archivecf->cf_Size = fib->fib_Size;
													archivecf->cf_Result = 0;
													
													xCHK_DoExtractedFile( cc, cf, archivecf );
													}
												UnLock( file );
												}
											
												/// Delete temp file ?
											
											if( XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles )
												{
												switch( delete )
													{
													case DELETE_ALL:
														DeleteFile( node->an_Name );
														break;
													
													case DELETE_ONE:
													case SKIP_ONE:
														if( 2 <= (delete = xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_DELETEEXTRACTEDFILE ), GCS( MSG_FC_GF_DELETEEXTRACTED ), node->an_Name ) ) )
															DeleteFile( node->an_Name );
														break;
													}
												}
											
											CurrentDir( cd );
											
											UnLock( lock );
											}
										}
									}
								}
							}
						
						xCHK_DeleteAll( XSettings->xs_PAT_ArchiveExtractPath, XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles );
						
						FreeVec( archivecf );
						}
					Close( out );
					}
				FreeDosObject( DOS_FIB, fib );
				}
			xFreeArchiveNodes( list );
			
			FreeVec( list );
			}
		}
	
	return( success );
}

/*
 * Checks an extracted file...
 *
 * Pass two CurrentFiles because 'cf' is the archive from which 'archivecf' was
 * extracted.
 */
VOID xCHK_DoExtractedFile( struct CurrentCheck *cc, struct CurrentFile *cf, struct CurrentFile *archivecf )
{
	BPTR	file = NULL;
	UBYTE *	filebuffer;
	ULONG	filesize = archivecf->cf_Size;
	STRPTR	filename = archivecf->cf_FileName;
	UBYTE	library_determine = 5;
	BOOL	extractarcs = XSettings->xs_FC_UNP_ExtractArchives;
	BOOL	check = cf->cf_ArchiveType == AT_LHA ? (RegName[0] == '\n' ? FALSE : TRUE) : TRUE;
	
	
		/// First read the file into a buffer
	
	if( filebuffer = AllocVec( filesize, MEMF_CLEAR ) )
		{
		if( file = Open( filename, MODE_OLDFILE ) )
			{
			if( filesize == Read( file, filebuffer, filesize ) )
				{
					/// File has now been read. Let's see if it's an executable
				
				if( ((ULONG *)filebuffer)[0] == 0x000003F3 && check )
					{
						/// It sure was! Let's check it then.
					
					if( xCHK_CheckBuffer( filebuffer, filesize ) )
						{
							/// VIRUS FOUND! Fill in virus name etc.
						
						cc->cc_ViraFound++;
						strncpy( archivecf->cf_VirusName, FilevirusNode->fv_FileInfection->fi_VirusName, DEFAULTBUFFERLEN );
			
						if( XSettings->xs_HIS_RecFileCheck )
							XtruderHistory->his_ViraFound++;
						
							/// Deal with virus according to preferences
						
						xCHK_HandleVirusInArchive( cc, cf, archivecf );
						
							/// Return from this function!
						
						FreeVec( filebuffer );
						
						Close( file );
						
						return;
						}
					}
				
					/// File was not executable OR it was not a virus.
					/// Now check if we should decrunch files.
				
				if( XSettings->xs_FC_UNP_DecrunchExecutables && check )
					{
					if( library_determine = xCHK_IsFileCrunched( archivecf, filename, filebuffer, filesize ) )
						{
						cc->cc_FilesCrunched++;
						
							/// Decrunch and check file
						
						archivecf->cf_Buffer = filebuffer;
						archivecf->cf_BufferSize = filesize;
						
						xCHK_DecrunchExeOrDataAndCheck( cc, cf, archivecf, library_determine );
						
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
						if( UInfo->ui_CrunchNum == CN_Dms )
							{
							archivecf->cf_ArchiveType = AT_DMS;
							
								/// Check the bootblock
							
							if( xReadDMSBootblock( archivecf->cf_FileName, FALSE ) )
								{
								struct Bootblock *	bb;
								LONG				ret;
								
								
								bb = (struct Bootblock *)CheckBoot( (ULONG *)XStrings->xs_Bootblock, &ret );
								
								switch( ret )
									{
									case BOOT_VIRUS:
										strcpy( archivecf->cf_VirusName, bb->bootname );
										
										xCHK_HandleVirusInArchive( cc, cf, archivecf );
										break;
									}
								}
							else
								xAddFileCheckHistoryNodes( 3, GCS( MSG_COULDNOTCHECKBOOTBLOCK ), FilePart( archivecf->cf_FileName ) );
							}
						}
					break;
				
				case CRU_Archive:
						/// Free some memory
					
					FreeVec( filebuffer );
					filebuffer = NULL;
					
					switch( UInfo->ui_CrunchNum )
						{
						case CN_Lha:
							archivecf->cf_ArchiveType = AT_LHA;
							
							if( XSettings->xs_FC_ARC_ExtractLhA )
								{
									/// 'cf' is the original archive from which this archive we are
									/// processing right now was extracted from.
								
								xCHK_ExtractArchive( cc, cf, archivecf );
								}
							break;
						
						case CN_Lzx:
							archivecf->cf_ArchiveType = AT_LZX;
							
							if( XSettings->xs_FC_ARC_ExtractLZX )
								{
									/// 'cf' is the original archive from which this archive we are
									/// processing right now was extracted from.
								
								xCHK_ExtractArchive( cc, cf, archivecf );
								}
							break;
						}
					break;
				}
			}
		}
	
	if( file )
		Close( file );
	
	FreeVec( filebuffer );
}

/*
 * Outputs an archive list to TempPath and puts the files and dirs within
 * the archive into a specified list.
 *
 * Returns the list with the files within the archive
 */
struct List *xSetUpInternal_1_ArchiveList( STRPTR archivename, UWORD archivetype, struct CurrentCheck *cc )
{
	struct List *	arclist;
	UBYTE *			s;
	BPTR			lock,file,olddir,out,par;
	UWORD			i = 0;
	UBYTE			strbuf1[ DEFAULTBUFFERLEN + 1 ];
	UBYTE			strbuf2[ DEFAULTBUFFERLEN * 2 ];
	UBYTE			strbuf3[ DEFAULTBUFFERLEN * 2 ];
	
	
		/// Create filter for later parsing of listing
	
	xParsePatternNoCase( XSettings->xs_FC_MIS_ArchiveFilter, strbuf3 );
	
		/// Allocate a list for holding list of "good" names
	
	if( arclist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( arclist );
		
			/// Lock on archive
		
		if( lock = Lock( archivename, ACCESS_READ ) )
			{
			par = ParentDir( lock );
			olddir = CurrentDir( par );
			
			switch( archivetype )
				{
				case AT_LHA:
					xSPrintF( strbuf1, "LhA v -mM \"%s\" #? >\"", FilePart( archivename ) );
					break;
				
				case AT_LZX:
					xSPrintF( strbuf1, "LZX l \"%s\" >\"", FilePart( archivename ) );
					break;
				}
			
				/// Add the unpack path
			
			if( strncpy( strbuf2, XSettings->xs_PAT_ArchiveExtractPath, DEFAULTBUFFERLEN * 2 ) )
				{
				if( AddPart( strbuf2, "Xtruder.arclist", DEFAULTBUFFERLEN * 2 ) )
					{
					strcat( strbuf1, strbuf2 );
					strcat( strbuf1, "\"" );
					
					out = Open( "NIL:", MODE_NEWFILE );
					
						/// Execute the listing
					
					if( !SystemTags( strbuf1,
						SYS_Output, out,
						TAG_DONE ) )
						{
							/// Okay we've got a list... let's read it
						
						if( file = Open( strbuf2, MODE_OLDFILE ) )
							{
							UBYTE	sizebuf[20];
							UWORD	n,t;
							STRPTR	arcfilename;
							
							
							switch( archivetype )
								{
								case AT_LHA:
									while( i++ < 6 )
										FGets( file, strbuf1, DEFAULTBUFFERLEN );
									
									while( s = FGets( file, strbuf1, DEFAULTBUFFERLEN ) )
										{
										if( strbuf1[0] == '-' )
											break;
										
											/// First get the size...
										
										if( strlen( strbuf1 ) > 20 )
											{
											t = n = 0;
											
											while( (strbuf1[n] == ' ') && (n < 20) )
												n++;
											
											if( n != 20 )
												{
												for( n; strbuf1[n] != ' '; n++, t++ )
													sizebuf[t] = strbuf1[n];
												
												sizebuf[t] = 0;
												
													/// Replace EOL with EOF
												
												strbuf1[ strlen( strbuf1 )-1 ] = '\0';
												
													/// Must be more than 43 chars in a "real" line
												
												if( strlen( strbuf1 ) > 43 )
													{
													UWORD	len;
													
													
													arcfilename = &strbuf1[43];
													len = strlen( arcfilename );
													
													cc->cc_ArchivedFilesEncountered++;
													
														/// Filename must not be empty
													
														/// First character must not be a '-'

														/// Last character must not be
														/// a '/' or else it will extract
														/// a directory
														
													if( arcfilename[0] &&
														strcmp( arcfilename, "*" ) &&
														(arcfilename[0] != '-') &&
														(arcfilename[ len - 1 ] != '/') )
														{
														BOOL	filesmall = TRUE;
														LONG	size = atol( sizebuf );
														
														
															/// Is size small enough ?
														
														if( XSettings->xs_FC_ARC_LZXMaxSize )
															{
															if( XSettings->xs_FC_ARC_LZXMaxSize < atol( sizebuf ) )
																filesmall = FALSE;
															}
														
														if( (filesmall) && (size > 0) )
															{
																/// Does it match Archive Filter
	
															if( MatchPatternNoCase( strbuf3, arcfilename ) )
																xAddArchiveNode( arclist, arcfilename, atol( sizebuf ) );
															}
														}
													}
												}
											}
										}
									break;
								
								case AT_LZX:
									while( i++ < 9 )
										FGets( file, strbuf1, DEFAULTBUFFERLEN );
									
									while( s = FGets( file, strbuf1, DEFAULTBUFFERLEN ) )
										{
										if( strbuf1[0] == '-' )
											break;
										
											/// First get the size...
										
										if( strlen( strbuf1 ) > 20 )
											{
											t = n = 0;
											
											while( (strbuf1[n] == ' ') && (n < 20) )
												n++;
											
											if( n != 20 )
												{
												for( n; strbuf1[n] != ' '; n++, t++ )
													sizebuf[t] = strbuf1[n];
												
												sizebuf[t] = 0;
												
													/// Replace EOL with EOF
												
												strbuf1[ strlen( strbuf1 )-1 ] = '\0';
												
													/// Must be more than 43 chars in a "real" line
													/// Could be "CRC header error"
												
												if( strlen( strbuf1 ) > 43 )
													{
													BOOL	filesmall = TRUE;
													LONG	size = atol( sizebuf );
													
													
													cc->cc_ArchivedFilesEncountered++;
													
														/// Is size small enough ?
													
													if( XSettings->xs_FC_ARC_LZXMaxSize )
														{
														if( XSettings->xs_FC_ARC_LZXMaxSize < size )
															filesmall = FALSE;
														}
													
													if( (filesmall) && (size > 0) )
														{
															/// Does it match Archive Filter ?
														
														if( MatchPatternNoCase( strbuf3, &strbuf1[43] ) )
															xAddArchiveNode( arclist, &strbuf1[43], atol( sizebuf ) );
														}
													}
												}
											}
										else
											break;
										}
									break;
								}
							
							Close( file );
							}
						}
					
					if( out )
						Close( out );
					}
				}
			
			CurrentDir( olddir );
			
			UnLock( par );
			UnLock( lock );
			}
		
			/// If no entries were found or stored we will free the list here
		
		if( IsListEmpty( arclist ) )
			{
			FreeVec( arclist );
			
			arclist = NULL;
			}
		else
			xSortArchiveNodes( arclist );
		}
	
	return( arclist );
}

/*
 * Outputs an archive list to TempPath and puts the files and dirs within
 * the archive into a specified list.
 *
 * Returns the list with the files within the archive
 */
struct List *xSetUpInternal_2_ArchiveList( STRPTR archivename, UWORD archivetype, struct CurrentCheck *cc )
{
	struct List *	arclist;
	UBYTE *			s;
	BPTR			lock,file;
	BPTR			olddir;
	UWORD			i = 0;
	UBYTE			strbuf1[ DEFAULTBUFFERLEN + 1 ];
	UBYTE			strbuf2[ DEFAULTBUFFERLEN * 2 ];
	UBYTE			strbuf3[ DEFAULTBUFFERLEN * 2 ];
	
	
	xParsePatternNoCase( XSettings->xs_FC_MIS_ArchiveFilter, strbuf3 );
	
	if( arclist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( arclist );
		
		if( lock = Lock( archivename, ACCESS_READ ) )
			{
			BPTR	par = ParentDir( lock );
			
			
			olddir = CurrentDir( par );
			
			switch( archivetype )
				{
				case AT_LHA:
					xSPrintF( strbuf1, "LhA v -M \"%s\" >\"", FilePart( archivename ) );
					break;
				
				case AT_LZX:
					xSPrintF( strbuf1, "LZX l \"%s\" >\"", FilePart( archivename ) );
					break;
				}
			
			if( strncpy( strbuf2, XSettings->xs_PAT_ArchiveExtractPath, DEFAULTBUFFERLEN * 2 ) )
				{
				if( AddPart( strbuf2, "Xtruder.arclist", DEFAULTBUFFERLEN * 2 ) )
					{
					strcat( strbuf1, strbuf2 );
					strcat( strbuf1, "\"" );
					
					if( !SystemTagList( strbuf1, NULL ) )
						{
							/// Okay we've got a list... let's read it
						
						if( file = Open( strbuf2, MODE_OLDFILE ) )
							{
							UBYTE	sizebuf[20];
							UWORD	n,t;
							STRPTR	arcfilename;
							
							
							switch( archivetype )
								{
								case AT_LHA:
									while( i++ < 6 )
										FGets( file, strbuf1, DEFAULTBUFFERLEN );
									
									while( s = FGets( file, strbuf1, DEFAULTBUFFERLEN ) )
										{
										if( strbuf1[0] == '-' )
											break;
										
											/// First get the size...
										
										if( strlen( strbuf1 ) > 20 )
											{
											t = n = 0;
											
											while( (strbuf1[n] == ' ') && (n < 20) )
												n++;
											
											if( n != 20 )
												{
												for( n; strbuf1[n] != ' '; n++, t++ )
													sizebuf[t] = strbuf1[n];
												
												sizebuf[t] = 0;
												
													/// Replace EOL with EOF
												
												strbuf1[ strlen( strbuf1 )-1 ] = '\0';
												
													/// Must be more than 43 chars in a "real" line
												
												if( strlen( strbuf1 ) > 43 )
													{
													UWORD	len;
													
													
													arcfilename = &strbuf1[43];
													len = strlen( arcfilename );
													
													cc->cc_ArchivedFilesEncountered++;
													
														/// Filename must not be empty
													
														/// First character must not be a '-'

														/// Last character must not be
														/// a '/' or else it will extract
														/// a directory
														
														/// Name must also not contain a '"'
														
														/// Must not be a '*'
													
													if( arcfilename[0] &&
														(arcfilename[0] != '-') &&
														(arcfilename[ len - 1 ] != '/') &&
														!stpchr( arcfilename, '"' ) &&
														strcmp( arcfilename, "*" ) )
														{
															/// Does it match Archive Filter 

														if( MatchPatternNoCase( strbuf3, arcfilename ) )
															xAddArchiveNode( arclist, arcfilename, atol( sizebuf ) );
														}
													}
												}
											}
										}
									break;
								
								case AT_LZX:
									while( i++ < 9 )
										FGets( file, strbuf1, DEFAULTBUFFERLEN );
									
									while( s = FGets( file, strbuf1, DEFAULTBUFFERLEN ) )
										{
										if( strbuf1[0] == '-' )
											break;
										
											/// First get the size...
										
										if( strlen( strbuf1 ) > 20 )
											{
											t = n = 0;
											
											while( (strbuf1[n] == ' ') && (n < 20) )
												n++;
											
											if( n != 20 )
												{
												for( n; strbuf1[n] != ' '; n++, t++ )
													sizebuf[t] = strbuf1[n];
												
												sizebuf[t] = 0;
												
													/// Replace EOL with EOF
												
												strbuf1[ strlen( strbuf1 )-1 ] = '\0';
												
												cc->cc_ArchivedFilesEncountered++;
												
													/// Must be more than 43 chars in a "real" line
													/// Could be "CRC header error"
												
												if( strlen( strbuf1 ) > 43 )
													{
														/// Does it match Archive Filter ?
													
													if( MatchPatternNoCase( strbuf3, &strbuf1[43] ) )
														xAddArchiveNode( arclist, &strbuf1[43], atol( sizebuf ) );
													}
												}
											}
										else
											break;
										}
									break;
								}
							
							Close( file );
							}
						}
					}
				}
			
			CurrentDir( olddir );
			
			UnLock( par );
			UnLock( lock );
			}
		
			/// If no entries were found or stored we will free the list here
		
		if( IsListEmpty( arclist ) )
			{
			FreeVec( arclist );
			
			arclist = NULL;
			}
		else
			xSortArchiveNodes( arclist );
		}
	
	return( arclist );
}

/*
 * Sort the names of the files to extract
 */
VOID xSortArchiveNodes( struct List *arclist )
{
	struct ArchiveNode *	a;
	struct ArchiveNode *	b;
	UBYTE					temp[ DEFAULTBUFFERLEN ] = "";
	ULONG					tempsize;
	
	
	for( a = (struct ArchiveNode *)arclist->lh_Head; a->an_Node.ln_Succ; a = (struct ArchiveNode *)a->an_Node.ln_Succ )
		{
		for( b = a; b->an_Node.ln_Succ; b = (struct ArchiveNode *)b->an_Node.ln_Succ )
			{
			if( stricmp( a->an_Name, b->an_Name ) > 0 )
				{
				strcpy( temp, a->an_Name );
				strcpy( a->an_Name, b->an_Name );
				strcpy( b->an_Name, temp );
				
				tempsize = a->an_ExtractedSize;
				a->an_ExtractedSize = b->an_ExtractedSize;
				b->an_ExtractedSize = tempsize;
				}
			}
		}
}

VOID xAddArchiveNode( struct List *l, STRPTR filename, ULONG size )
{
	struct ArchiveNode *	nn;
	
	
		/// No need to add a file if it is 0 bytes big when extracted
		/// If name is longer than 255 it doesn't fit in buffer.
	
	if( (size > 0) && (strlen( filename ) < 256) )
		{
		if( nn = AllocVec( sizeof( struct ArchiveNode ), MEMF_CLEAR ) )
			{
			strcpy( nn->an_Name, filename );
			
			nn->an_Node.ln_Name = nn->an_Name;
			nn->an_ExtractedSize = size;
		
			AddTail( l, (struct Node *)nn );
			}
		}
}

VOID xFreeArchiveNodes( struct List *l )
{
	struct ArchiveNode *	worknode;
	struct ArchiveNode *	nextnode;
	
	
	worknode = (struct ArchiveNode *)l->lh_Head;
	
	while( nextnode = (struct ArchiveNode *)worknode->an_Node.ln_Succ )
		{
		FreeVec( worknode );
		
		worknode = nextnode;
		}
}

// Lha x ram:tet.lha ~(#?.guide|#?.doc|#?[*]#?|#?[|]#?|#?["]#?)

