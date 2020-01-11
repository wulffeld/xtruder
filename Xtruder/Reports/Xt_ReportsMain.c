/*
**	$VER: Xt_ReportsMain.c (8.8.96)
**
**	Reports: Main functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Flushes the reportslist and reads what is in the directory.
 */
VOID xRereadReportsList( VOID )
{
	struct AnchorPath *	ap;
	ULONG				r;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	BPTR				lock,oldrepcd;
	
	
		/// Freeze reports listview
	
	GT_SetGadgetAttrs( MRIWindowGadgets[ 6 ].xg_Gad, MRIWindow, NULL,
		GTLV_Labels, ~0,
		TAG_END );
	
		/// Flush reports list
	
	xFreeReportNodes();
	
		/// Find reports in directory
	
	if( lock = Lock( GETSTRING( MRIWindowGadgets[ 4 ].xg_Gad ), ACCESS_READ ) )
		{
		oldrepcd = CurrentDir( lock );
		
		if( ap = AllocVec( sizeof( struct AnchorPath ) + 256, MEMF_PUBLIC | MEMF_CLEAR ) )
			{
			ap->ap_Strlen = 256;
			
				/// Go through this path/file/pattern...
			
			for( r = MatchFirst( "#?", ap ); r == 0; r = MatchNext( ap ) )
				{
				if( ap->ap_Info.fib_DirEntryType < 0 )
					{
					BPTR	oldcd = CurrentDir( ap->ap_Current->an_Lock );
					
					
					if( NameFromLock( ap->ap_Current->an_Lock, strbuf, DEFAULTBUFFERLEN ) )
						{
							/// If file is a report add it to the list
						
						if( xIsFileReport( ap->ap_Info.fib_FileName ) )
							xAddReportNode( ap->ap_Info.fib_FileName, &ap->ap_Info.fib_Date );
						}
					
					CurrentDir( oldcd );
					}
				}
			
			MatchEnd( ap );
			
			FreeVec( ap );
			}
		
		CurrentDir( oldrepcd );
		
		UnLock( lock );
		}
	
		/// Sort list
	
	xSortReportNodes( &MRIReportsList );
	
		/// Unfreeze reports listview
	
	GT_SetGadgetAttrs( MRIWindowGadgets[ 6 ].xg_Gad, MRIWindow, NULL,
		GTLV_Labels, &MRIReportsList,
		TAG_END );
}

VOID xAddReportNode( STRPTR filename, struct DateStamp *ds )
{
	struct ReportNode *	rnode;
	
	
	if( rnode = AllocVec( sizeof( struct ReportNode ), MEMF_CLEAR ) )
		{
		struct DateTime	dt;
		UBYTE			datebuf[20];
		UBYTE			timebuf[20];
		
		
		dt.dat_Stamp = *ds;
		dt.dat_Format = FORMAT_DOS;
		dt.dat_Flags = 0;
		dt.dat_StrDay = NULL;
		dt.dat_StrDate = datebuf;
		dt.dat_StrTime = timebuf;
		
		if( DateToStr( &dt ) )
			{
			xSPrintF( rnode->rn_ListViewName, "%-34s%s %s", filename, datebuf, timebuf );
			strcpy( rnode->rn_Filename, filename );
			}
		else
			{
			strcpy( rnode->rn_ListViewName, filename );
			strcpy( rnode->rn_Filename, filename );
			}
		
		rnode->rn_Node.ln_Name = rnode->rn_ListViewName;
		
		AddTail( &MRIReportsList, (struct Node *)rnode );
		}
}

VOID xSortReportNodes( struct List *list )
{
	struct ReportNode *	a;
	struct ReportNode *	b;
	UBYTE				buf[ DEFAULTBUFFERLEN ];
	
	
    for( a = (struct ReportNode *)list->lh_Head;
		 a->rn_Node.ln_Succ;
		 a = (struct ReportNode *)a->rn_Node.ln_Succ )
		{
	    for( b = (struct ReportNode *)list->lh_Head;
			 b->rn_Node.ln_Succ;
			 b = (struct ReportNode *)b->rn_Node.ln_Succ )
			{
			if( strcmp( a->rn_Filename, b->rn_Filename ) < 0 )
				{
				strcpy( buf, a->rn_Filename );
				strcpy( a->rn_Filename, b->rn_Filename );
				strcpy( b->rn_Filename, buf );
				
				strcpy( buf, a->rn_ListViewName );
				strcpy( a->rn_ListViewName, b->rn_ListViewName );
				strcpy( b->rn_ListViewName, buf );
				}
			}
		}
}

VOID xFreeReportNodes( VOID )
{
	struct Node *	worknode;
	struct Node *	nextnode;
	
	
	if( !IsListEmpty( &MRIReportsList ) )
		{
		worknode = MRIReportsList.lh_Head;
		
		while( nextnode = worknode->ln_Succ )
			{
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		
		NewList( &MRIReportsList );
		}
}

/*
 * Returns TRUE if the file is a report file
 */
BOOL xIsFileReport( STRPTR filename )
{
	ULONG	buf[1];
	BPTR	file;
	BOOL	success = FALSE;
	
	
	if( file = Open( filename, MODE_OLDFILE ) )
		{
		if( 4 == Read( file, buf, 4 ) )
			{
			if( buf[0] == 0x5245504F )			/// REPO
				success = TRUE;
			}
		Close( file );
		}
	
	return( success );
}
