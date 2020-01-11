/*
**	$VER: Xt_ReportsCheck.c (8.8.96)
**
**	Reports: Check functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * 'filename' is the name of the report file
 */
VOID xStartCheckReport( STRPTR filename )
{
	BPTR	lock,oldcd;
	
	
	xDisableWindow( MRIWindow );
	
		/// Make the report path the current path
	
	if( lock = Lock( XSettings->xs_REP_ReportsPath, ACCESS_READ ) )
		{
		oldcd = CurrentDir( lock );
		
		xCheckReport( filename );
		
		CurrentDir( oldcd );
		
		UnLock( lock );
		}
	
	xEnableWindow( MRIWindow );
}

VOID xCheckReport( STRPTR filename )
{
	struct CurrentReport *		cr;
	BOOL						couldnotcheckreport = FALSE;
	
	
		/// Allocate space for information on this check
	
	if( cr = AllocVec( sizeof( struct CurrentReport ), MEMF_CLEAR ) )
		{
		cr->cr_HandleMismatch = cr->cr_PreviousHandleMismatch = -1;
		
		strcpy( cr->cr_ReportFileName, filename );
		
			/// Check if the file is infact a report
		
		if( xIsFileReport( filename ) )
			{
				/// Start checking...
			
			if( cr->cr_ReportFile = Open( filename, MODE_OLDFILE ) )
				{
				if( xOpenCHIWindow() )
					{
					BOOL	finished = FALSE;
					
					
					xCHI_UpdateCheckInfo( cr );
					
						/// Get rid of the header - 10 bytes ahead
					
					Seek( cr->cr_ReportFile, 10, OFFSET_CURRENT );
					
						/// Loop like you've never looped before
					
					while( !finished )
						{
						struct FileReport	fr;
						UBYTE				pathname[ DEFAULTBUFFERLEN ] = "";
						STRPTR				fn,fn2;
						
						
						switch( xHandleCHIWindowIDCMP( cr ) )
							{
							case CW_NOTHING:
								if( sizeof( struct FileReport ) == Read( cr->cr_ReportFile, &fr, sizeof( struct FileReport ) ) )
									{
									fn = fn2 = FilePart( fr.fr_FileName );
									
									if( fn != fr.fr_FileName )
										if( *(fn - 1) == '/' )
											fn--;
									
									strncpy( pathname, fr.fr_FileName, fn - fr.fr_FileName );
									
									xCHI_UpdatePathGadget( pathname );
									xCHI_UpdateFileGadget( fn2 );
									
									xCheckReportObject( cr, &fr );
									}
								else
									finished = TRUE;
								break;
							
							case CW_STOP:
								cr->cr_Result = CCRES_USERBREAK;
			                    
								finished = TRUE;
								break;
							}
						
						xCHI_UpdateCheckInfo( cr );
						}
					
						/// Close CHI window
					
					cr->cr_Result = CCRES_CHECKEND;
					
					xHandleCHIWindowIDCMP( cr );
					
					xCloseCHIWindow();
					}
				else
					couldnotcheckreport = TRUE;
				
				Close( cr->cr_ReportFile );
				}
			else
				couldnotcheckreport = TRUE;
			}
		FreeVec( cr );
		}
	else
		couldnotcheckreport = TRUE;
	
	if( couldnotcheckreport )
		xReqMsg( CRIWindow, GCS( MSG_REQ_INFO ), GCS( MSG_CHI_COULDNOTCHECKREPORT ), GCS( MSG_GF_OKAY ) );
}

VOID xCheckReportObject( struct CurrentReport *cr, struct FileReport *fr )
{
	struct FileInfoBlock *	fib;
	BPTR					file;
	
	
	if( file = Open( fr->fr_FileName, MODE_OLDFILE ) )
		{
		if( fib = AllocDosObject( DOS_FIB, TAG_DONE ) )
			{
			if( ExamineFH( file, fib ) )
				{
				BOOL	mismatch = FALSE;
				
				
				cr->cr_FilesChecked++;
				
					/// Check if CRC is okay
				
				if( fr->fr_CRC )
					{
					UBYTE *	buf;
					
					
					if( buf = xStoreFile( fr->fr_FileName, fib->fib_Size ) )
						{
						if( fr->fr_CRC != xaDoCRC( buf, fib->fib_Size ) )
							{
							xAddReportCheckHistoryNode( 4, GCS( MSG_CHI_CRCISWRONG ), fr->fr_FileName );
							
							mismatch = TRUE;
							}
						
						FreeVec( buf );
						}
					else
						xAddReportCheckHistoryNode( 1, GCS( MSG_CHI_COULDNOTCHECKCRC ), fr->fr_FileName );
					}
				else
					xAddReportCheckHistoryNode( 1, GCS( MSG_CHI_COULDNOTCHECKCRC ), fr->fr_FileName );
				
					/// Check if size is right
				
				if( fr->fr_Size != fib->fib_Size )
					{
					xAddReportCheckHistoryNode( 4, GCS( MSG_CHI_SIZEISWRONG ), fr->fr_FileName );
					
					mismatch = TRUE;
					}
				
					/// Check if date is right
				
				if( fr->fr_Date.ds_Days != fib->fib_Date.ds_Days ||
					fr->fr_Date.ds_Minute != fib->fib_Date.ds_Minute ||
					fr->fr_Date.ds_Tick != fib->fib_Date.ds_Tick )
					{
					xAddReportCheckHistoryNode( 2, GCS( MSG_CHI_DATEISWRONG ), fr->fr_FileName );
					
					mismatch = TRUE;
					}
				
					/// Check if protection is right
				
				if( fr->fr_Protection != fib->fib_Protection )
					{
					xAddReportCheckHistoryNode( 2, GCS( MSG_CHI_PROTECTIONISWRONG ), fr->fr_FileName );
					
					mismatch = TRUE;
					}
				
					/// Check if comment is right
				
				if( strcmp( fr->fr_Comment, fib->fib_Comment ) )
					{
					xAddReportCheckHistoryNode( 2, GCS( MSG_CHI_COMMENTISWRONG ), fr->fr_FileName );
					
					mismatch = TRUE;
					}
				
					/// Handle mismatch
				
				if( mismatch )
					{
					BOOL	gotpath = TRUE;
					
					
					cr->cr_MismatchesFound++;
					
						/// Add a separator
					
					xAddReportCheckHistoryNode( 0, "" );
					
					if( cr->cr_HandleMismatch != RC_COPYALLTO && cr->cr_HandleMismatch != RC_IGNOREALL )
						{
							/// Request action to take on file
						
						cr->cr_HandleMismatch = xReqMsg( CHIWindow, GCS( MSG_REQ_INFO ), GCS( MSG_CHI_FILEDOESNOTMATCH ), GCS( MSG_CHI_GF_REPORTMISMATCH ), FilePart( fr->fr_FileName ) );
						}
					
					switch( cr->cr_HandleMismatch )
						{
						case RC_COPYTO:
							if( xGetCopyToPath( cr ) )
								{
								if( !xCHI_CopyFile( fr->fr_FileName, cr->cr_CopyToPath ) )
									xAddReportCheckHistoryNode( 1, GCS( MSG_FC_COULDNOTCOPYFILETO ), FilePart( fr->fr_FileName ), cr->cr_CopyToPath );
								}
							break;
						
						case RC_COPYALLTO:
							if( !cr->cr_CopyToPath[0] || cr->cr_PreviousHandleMismatch != RC_COPYALLTO )
								gotpath = xGetCopyToPath( cr );
							
							if( gotpath )
								{
								if( !xCHI_CopyFile( fr->fr_FileName, cr->cr_CopyToPath ) )
									xAddReportCheckHistoryNode( 1, GCS( MSG_FC_COULDNOTCOPYFILETO ), FilePart( fr->fr_FileName ), cr->cr_CopyToPath );
								else
									cr->cr_HandleMismatch = -1;
								}
							else
								cr->cr_HandleMismatch = -1;
							break;
						
						case RC_DELETE:
							break;
						
						case RC_IGNORE:
						case RC_IGNOREALL:
							break;
						}
					
					cr->cr_PreviousHandleMismatch = cr->cr_HandleMismatch;
					}
				}
			FreeDosObject( DOS_FIB, fib );
			}
		Close( file );
		}
	else
		{	
		xAddReportCheckHistoryNode( 1, GCS( MSG_CHI_FILENOTFOUND ), fr->fr_FileName );
		
			/// Add a separator
		
		xAddReportCheckHistoryNode( 0, "" );
		}
}

BOOL xGetCopyToPath( struct CurrentReport *cr )
{
	UBYTE	destdir[ DEFAULTBUFFERLEN ];
	
	
	if( xSelectPath( MRIWindow,
		SELFP_Dir,		cr->cr_CopyToPath,
		SELFP_DestDir,	destdir,
		TAG_END ) )
		{
		if( strcpy( cr->cr_CopyToPath, xGetFullName( destdir, "" ) ) )
			return( TRUE );
		}
	
	return( FALSE );
}

VOID __stdargs xAddReportCheckHistoryNode( UWORD color, STRPTR format, ... )
{
	struct HistoryNode *	hn;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ];
	
	
	RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
	
		/// Remove list from ListView
	
	GT_SetGadgetAttrs( CHIWindowGadgets[ 5 ].xg_Gad, CHIWindow, NULL,
		GTLV_Labels, ~0,
		TAG_END );
	
		/// Add information to list
	
	if( hn = AllocVec( sizeof( struct HistoryNode ), MEMF_CLEAR ) )
		{
		if( hn->hn_Name = AllocVec( strlen( strbuf ) + 1, MEMF_CLEAR ) )
			{
			strcpy( hn->hn_Name, strbuf );
			hn->hn_Node.ln_Name = hn->hn_Name;
			hn->hn_Color = color;
			CHIHistoryList.cl_Entries++;
			
			AddTail( &CHIHistoryList.cl_List, (struct Node *)hn );
			}
		}
	
		/// Reattach list to ListView
	
	GT_SetGadgetAttrs( CHIWindowGadgets[ 5 ].xg_Gad, CHIWindow, NULL,
		GTLV_Labels, &CHIHistoryList.cl_List,
		TAG_END );
}

BOOL xCHI_CopyFile( STRPTR filename, STRPTR destdir )
{
	BPTR	file,dfile;
	UBYTE *	buffer;
	LONG	s;
	BOOL	success = FALSE;
	UBYTE	strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	if( buffer = AllocVec( 100000, MEMF_ANY ) )
		{
		if( strncpy( strbuf, destdir, DEFAULTPATHLEN ) )
			{
			if( AddPart( strbuf, FilePart( filename ), DEFAULTBUFFERLEN ) )
				{
				if( file = Open( filename, MODE_OLDFILE ) )
					{
					if( dfile = Open( strbuf, MODE_NEWFILE ) )
						{
						BOOL	done = FALSE;
						
						
						while( !done )
							{
							switch( s = Read( file, buffer, 100000 ) )
								{
								case -1:
										/// Read() Error
									
									done = TRUE;
									break;
								
								case 0:
									done = success = TRUE;
									break;
								
								default:
									if( Write( dfile, buffer, s ) == -1 )
										done = TRUE;
									break;
								}
							}
						
						Close( dfile );
						}
					Close( file );
					}
				}
			}
		FreeVec( buffer );
		}
	
	return( success );
}
