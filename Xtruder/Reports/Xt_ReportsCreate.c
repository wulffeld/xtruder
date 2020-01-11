/*
**	$VER: Xt_ReportsCreate.c (8.8.96)
**
**	Reports: Create functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * 'filename' is the name of the report file
 */
VOID xStartCreateReport( STRPTR filename, BOOL update )
{
	BPTR	lock,oldcd;
	
	
	xDisableWindow( MRIWindow );
	
		/// Make the report path the current path
	
	if( lock = Lock( XSettings->xs_REP_ReportsPath, ACCESS_READ ) )
		{
		oldcd = CurrentDir( lock );
		
		if( update )
			xUpdateReport( filename );
		else
			xCreateReport( filename );
		
		CurrentDir( oldcd );
		
		UnLock( lock );
		}
	
	xEnableWindow( MRIWindow );
}

VOID xCreateReport( STRPTR filename )
{
	struct rtFileList *			fl,*tl;
	struct CurrentReport *		cr;
	UBYTE						strbuf[ DEFAULTBUFFERLEN ];
	BOOL						overwrite = TRUE;
	BOOL						couldnotcreatereport = FALSE;
	
	
		/// Disable input
	
	xDisableWindow( Window );
			
		/// Allocate space for information on this check
	
	if( cr = AllocVec( sizeof( struct CurrentReport ), MEMF_CLEAR ) )
		{
		strcpy( cr->cr_ReportFileName, filename );
		
			/// Check if a report with that name already exists
		
		if( xExists( filename ) )
			{
			if( !xReqMsg( CRIWindow, GCS( MSG_REQ_INFO ), GCS( MSG_CRI_REPORTEXISTSOVERWRITE ), GCS( MSG_GF_YESNO ) ) )
				overwrite = FALSE;
			}
		
			/// Start creating...
		
		if( overwrite )
			{
				/// Request the user to select the drawers and files he wants checked...
			
			rtChangeReqAttr( FileReq, RTFI_Dir, RPATH, TAG_END );
			
			if( fl = rtFileRequest( FileReq, RFILE, GCS( MSG_TL_SELECTFILESDRAWERS ),
				RT_Window,		Window,
				RT_Underscore,	'_',
				RT_TextAttr,	&RequesterFontAttr,
				RTFI_Flags,		FREQF_MULTISELECT | FREQF_SELECTDIRS,
				RTFI_OkText,	GCS( MSG_GF_SELECT ),
				TAG_END ) )
				{
				strncpy( RPATH, FileReq->Dir, DEFAULTPATHLEN );
				
				tl = fl;
				
				if( cr->cr_ReportFile = Open( filename, MODE_NEWFILE ) )
					{
					if( xOpenCRIWindow() )
						{
						BPTR	lock;
						
						
							/// Header
						
						Write( cr->cr_ReportFile, "REPO3.000\n", strlen( "REPO3.000\n" ) );
						
							/// Path
						
						if( lock = Lock( FileReq->Dir, ACCESS_READ ) )
							{
							if( NameFromLock( lock, cr->cr_ReportPath, DEFAULTBUFFERLEN ) )
								{
									/// Go through the linked list
								
								while( tl )
									{
									strcpy( strbuf, FileReq->Dir );
									AddPart( strbuf, tl->Name, DEFAULTBUFFERLEN );
									
									xCreateObject( cr, strbuf );
									
										/// User pressed Stop?
									
									if( cr->cr_Result == CCRES_USERBREAK )
										break;
									
									cr->cr_Result = 0;
									
										/// Get next object
									
									tl = tl->Next;
									}
								}
							else
								couldnotcreatereport = TRUE;
							
							UnLock( lock );
							}
						else
							couldnotcreatereport = TRUE;
						
							/// Close CRI window
						
						cr->cr_Result = CCRES_CHECKEND;
						
						xHandleCRIWindowIDCMP( cr );
						
						xCloseCRIWindow();
						}
					else
						couldnotcreatereport = TRUE;
					
					Close( cr->cr_ReportFile );
					}
				else
					couldnotcreatereport = TRUE;
				
				rtFreeFileList( fl );
				}
			}
		
		FreeVec( cr );
		}
	else
		couldnotcreatereport = TRUE;
	
	if( couldnotcreatereport )
		xReqMsg( CRIWindow, GCS( MSG_REQ_INFO ), GCS( MSG_CRI_COULDNOTCREATEREPORT ), GCS( MSG_GF_OKAY ) );
	
		/// Enable input
	
	xEnableWindow( Window );
}

/*
 * Add a report on a file (files in directory)
 */
VOID xCreateObject( struct CurrentReport *cr, STRPTR object )
{
	struct FileReport		fr;
	struct AnchorPath *		ap;
	BPTR					oldcd;
	ULONG					r;
	UBYTE *					buffer;
	UBYTE					strbuf1[ DEFAULTBUFFERLEN ];
	
	
	if( ap = AllocVec( sizeof( struct AnchorPath ) + 256, MEMF_PUBLIC | MEMF_CLEAR ) )
		{
		ap->ap_Strlen = 256;
		
			/// Go through this path/file/pattern...
		
		for( r = MatchFirst( object, ap ); r == 0 && cr->cr_Result != CCRES_USERBREAK; r = MatchNext( ap ) )
			{
			switch( xHandleCRIWindowIDCMP( cr ) )
				{
				case CW_NOTHING:
					if( ap->ap_Info.fib_DirEntryType < 0 )
						{
						oldcd = CurrentDir( ap->ap_Current->an_Lock );
						
						if( NameFromLock( ap->ap_Current->an_Lock, strbuf1, DEFAULTBUFFERLEN ) )
							{
								/// New name in filename gadget
							
							xCRI_UpdateFileGadget( ap->ap_Info.fib_FileName );
							
							strcpy( fr.fr_FileName, strbuf1 );
							AddPart( fr.fr_FileName, ap->ap_Info.fib_FileName, DEFAULTBUFFERLEN );
							strcpy( fr.fr_Comment, ap->ap_Info.fib_Comment );
							fr.fr_Size = ap->ap_Info.fib_Size;
							fr.fr_Date.ds_Days = ap->ap_Info.fib_Date.ds_Days;
							fr.fr_Date.ds_Minute = ap->ap_Info.fib_Date.ds_Minute;
							fr.fr_Date.ds_Tick = ap->ap_Info.fib_Date.ds_Tick;
							fr.fr_Protection = ap->ap_Info.fib_Protection;
							
							fr.fr_CRC = 0;
							
							if( ap->ap_Info.fib_Size )
								{
								if( buffer = xStoreFile( ap->ap_Info.fib_FileName, ap->ap_Info.fib_Size ) )
									{
									fr.fr_CRC = xaDoCRC( buffer, ap->ap_Info.fib_Size );
									
									FreeVec( buffer );
									}
								else
									xAddReportCreateErrorNode( 4, GCS( MSG_CRI_COULDNOTCREATECRC ), fr.fr_FileName );
								}
							
								/// Add the file to the report
							
							Write( cr->cr_ReportFile, &fr, sizeof( struct FileReport ) );
							}
						CurrentDir( oldcd );
						}
					else
						{
							/// Enter sub directory
						
						if( !(ap->ap_Flags & APF_DIDDIR) )
							{
								/// New path in pathname gadget
							
							xCRI_UpdatePathGadget( ap->ap_Buf );
							
							ap->ap_Flags |= APF_DODIR;
							}
						
						ap->ap_Flags &= ~APF_DIDDIR;
						}
					break;
				
				case CW_STOP:
					MatchEnd( ap );

					FreeVec( ap );

					cr->cr_Result = CCRES_USERBREAK;
                    
					return;
					break;
				}
			}
		
		MatchEnd( ap );
		
		FreeVec( ap );
		}
}

VOID __stdargs xAddReportCreateErrorNode( UWORD color, STRPTR format, ... )
{
	struct HistoryNode *	hn;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ];
	
	
	RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
	
		/// Remove list from ListView
	
	GT_SetGadgetAttrs( CRIWindowGadgets[ 4 ].xg_Gad, CRIWindow, NULL,
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
			
			AddTail( &CRIErrorsList, (struct Node *)hn );
			}
		}
	
		/// Reattach list to ListView
	
	GT_SetGadgetAttrs( CRIWindowGadgets[ 4 ].xg_Gad, CRIWindow, NULL,
		GTLV_Labels, &CRIErrorsList,
		TAG_END );
}

VOID xFreeErrorNodes( VOID )
{
	struct Node *	worknode;
	struct Node *	nextnode;
	
	
	if( !IsListEmpty( &CRIErrorsList ) )
		{
		worknode = CRIErrorsList.lh_Head;
		
		while( nextnode = worknode->ln_Succ )
			{
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		
		NewList( &CRIErrorsList );
		}
}
