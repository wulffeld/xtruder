/*
**	$VER: XtruderHistory.c (12.2.95)
**
**	Xtruder history functions
**
**	Copyright (C) 1994-1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"



VOID __stdargs xAddHistoryNode( UWORD color, STRPTR format, ... )
{
	struct HistoryNode *	hn;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	if( hn = AllocVec( sizeof( struct HistoryNode ), MEMF_CLEAR ) )
		{
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
		
		if( hn->hn_Name = AllocVec( strlen( strbuf ) + 1, MEMF_CLEAR ) )
			{
			strcpy( hn->hn_Name, strbuf );
			hn->hn_Node.ln_Name = hn->hn_Name;
			hn->hn_Color = color;
			XtruderHistory->his_Entries++;
			
			AddTail( &XtruderHistory->his_List, (struct Node *)hn );
			
				/// Check if we exceed the limit number of entries for this list
			
			if( XSettings->xs_HIS_MaxEntries < XtruderHistory->his_Entries && XSettings->xs_HIS_MaxEntries )
				{
				if( hn = (struct HistoryNode *)RemHead( &XtruderHistory->his_List ) )
					{
					FreeVec( hn->hn_Name );
					FreeVec( hn );
					
					XtruderHistory->his_Entries--;
					}
				}
			
			return;
			}
		FreeVec( hn );
		}
}

VOID xClearHistory( BOOL ask )
{
	if( !IsListEmpty( &XtruderHistory->his_List ) )
		{
		xFreeHistoryNodes( &XtruderHistory->his_List );
		
		NewList( &XtruderHistory->his_List );
		
		XtruderHistory->his_Entries = 0;
		XtruderHistory->his_ViraFound = 0;
		}
}

VOID xFreeHistoryNodes( struct List *list )
{
	if( list )
		{
		if( !IsListEmpty( list ) )
			{
			struct HistoryNode *	worknode;
			struct HistoryNode *	nextnode;
			
			
			worknode = (struct HistoryNode *)list->lh_Head;
			
			while( nextnode = (struct HistoryNode *)worknode->hn_Node.ln_Succ )
				{
				FreeVec( worknode->hn_Name );
				
				FreeVec( worknode );
				
				worknode = nextnode;
				}
			
			NewList( list );
			}
		}
}

VOID xShowHistory( VOID )
{
	UBYTE	bartitle[100];
	
	
	xSPrintF( bartitle, GCS( MSG_ENTRIES ), xCountListEntries( &XtruderHistory->his_List ) );
	
	xOpenListShowWindow( &XtruderHistory->his_List, GCS( MSG_TL_HISTORY ), bartitle, Coords->xc_HistoryWindow, 0, TRUE );
}

/*
 * Flush and save history after a file check
 */
VOID xPerformHistorySave( VOID )
{
	BOOL	hissaved;
	BOOL	flush = XSettings->xs_HIS_SaveMethod == HISSM_AFTEREACHCHECKFLUSH ? TRUE : FALSE;
	
	
	if( hissaved = xSaveHistory( FALSE ) )
		{
			/// Should we flush history?
		
		if( flush )
			xClearHistory( FALSE );
		}
	
		/// Write out that we have saved and possibly flushed history...
	
	if( hissaved && flush )
		xAddFileCheckHistoryNodes( 2, GCS( MSG_HISTORYSAVEDANDFLUSHED ) );
	else
	if( hissaved )
		xAddFileCheckHistoryNodes( 2, GCS( MSG_HISTORYSAVED ) );
	else
		xAddFileCheckHistoryNodes( 4, GCS( MSG_COULDNOTSAVEHISTORY ) );
}

/*
 * If TRUE it will show a req with the calculated filename in it...
 */
BOOL xSaveHistory( BOOL askforfilename )
{
	struct HistoryNode *	worknode;
	struct DateTime		dt;
	struct DateStamp	ds;
	UBYTE				datebuf[100];
	UBYTE				timebuf[100];
	BPTR				file,lock;
	UWORD				a = 0;
	UBYTE				strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE				strbuf2[ DEFAULTBUFFERLEN ] = "";
	
	
		/// Calculate the date and time for the filename
	
	DateStamp( &ds );
	
	dt.dat_Stamp = ds;
	dt.dat_Format = FORMAT_DOS;
	dt.dat_Flags = 0;
	dt.dat_StrDay = NULL;
	dt.dat_StrDate = datebuf;
	dt.dat_StrTime = timebuf;
	
	if( DateToStr( &dt ) )
		{
			/// Convert conflicting characters
		
		for( a = 0; a < strlen( datebuf ); a++ )
			if( (datebuf[a] == '/') || (datebuf[a] == ':') )
				datebuf[a] = '.';
		
		for( a = 0; a < strlen( timebuf ); a++ )
			if( (timebuf[a] == '/') || (timebuf[a] == ':') )
				timebuf[a] = '.';
		
		xSPrintF( strbuf2, "%s %s", datebuf, timebuf );
		}
	
	if( XSettings->xs_HIS_UseDateAndTime )
		{
			/// Create the filename
		
		strcpy( strbuf1, XSettings->xs_HIS_HistoryPath );
		AddPart( strbuf1, strbuf2, DEFAULTBUFFERLEN );
		}
	else
	if( XSettings->xs_HIS_RequestFilename )
		{
		askforfilename = FALSE;
		
		rtChangeReqAttr( FileReq,
			RTFI_Dir, RPATH,
			TAG_END );
		
		if( rtFileRequest( FileReq, strbuf2, GCS( MSG_TL_SAVEHISTORY ),
			RT_Window,		Window,
			RT_Underscore,	'_',
			RT_TextAttr,	&RequesterFontAttr,
			RTFI_Flags,		FREQF_SAVE,
			RTFI_OkText,	GCS( MSG_GF_SAVE ),
			TAG_END ) )
			{
			strncpy( strbuf1, FileReq->Dir, DEFAULTBUFFERLEN );
			AddPart( strbuf1, strbuf2, DEFAULTBUFFERLEN );
			}
		else
			return( FALSE );
		}
	else
	if( XSettings->xs_HIS_Append )
		{
		strcpy( strbuf1, XSettings->xs_HIS_HistoryPath );
		AddPart( strbuf1, XSettings->xs_HIS_HistoryFile, DEFAULTBUFFERLEN );
		
			/// First check if there is a file to append to
		
		if( !(file = Open( strbuf1, MODE_OLDFILE ) ) )
			file = Open( strbuf1, MODE_NEWFILE );
		
		if( file )
			{
			Seek( file, 0, OFFSET_END );
			
				/// Write date and time of check
			
			FPrintf( file, "%s\n\n", strbuf2 );
			
				/// Write nodes
			
			worknode = (struct HistoryNode *)XtruderHistory->his_List.lh_Head;
			
			while( worknode )
				{
				FPrintf( file, "%s\n", worknode->hn_Name );
				
				worknode = (struct HistoryNode *)worknode->hn_Node.ln_Succ;
				}
			
			Close( file );
			
			return( TRUE );
			}
		else
			return( FALSE );
		}
	else
	if( XSettings->xs_HIS_HistoryFile[0] )
		{
		struct Window *	oldwin;
		
		
		strcpy( strbuf1, XSettings->xs_HIS_HistoryPath );
		AddPart( strbuf1, XSettings->xs_HIS_HistoryFile, DEFAULTBUFFERLEN );
		
		strcpy( strbuf2, strbuf1 );
		
		oldwin = Process->pr_WindowPtr;
		Process->pr_WindowPtr = (struct Window *)-1;
		
		while( lock = Lock( strbuf1, ACCESS_READ ) )
			{
			UnLock( lock );
			
			a++;
			
			if( a == 1000 )
				{
				Process->pr_WindowPtr = oldwin;
				
				return( FALSE );
				}
			
			xSPrintF( strbuf1, "%s.%-ld", strbuf2, a );
			}
		
		Process->pr_WindowPtr = oldwin;
		}
	else
		return( FALSE );
	
		/// Show the "calculated" name to the user...
	
	if( askforfilename )
		{
		strcpy( strbuf2, FilePart( strbuf2 ) );
		
		if( rtFileRequest( FileReq, strbuf2, GCS( MSG_TL_SAVEHISTORY ),
			RT_Window,		Window,
			RT_Underscore,	'_',
			RT_TextAttr,	&RequesterFontAttr,
			RTFI_Flags,		FREQF_SAVE,
			RTFI_OkText,	GCS( MSG_GF_SAVE ),
			TAG_END ) )
			{
			strncpy( strbuf1, FileReq->Dir, DEFAULTBUFFERLEN );
			AddPart( strbuf1, strbuf2, DEFAULTBUFFERLEN );
			}
		else
			return( FALSE );
		}
	
		/// Then we write all the entries to the file
	
	if( file = Open( strbuf1, MODE_NEWFILE ) )
		{
		worknode = (struct HistoryNode *)XtruderHistory->his_List.lh_Head;
		
		while( worknode )
			{
			FPrintf( file, "%s\n", worknode->hn_Name );
			
			worknode = (struct HistoryNode *)worknode->hn_Node.ln_Succ;
			}
		
		Close( file );
		}
	else
		return( FALSE );
	
	return( TRUE );
}
