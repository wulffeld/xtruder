/*
**	$VER: Xt_ReportsUpdate.c (14.8.96)
**
**	Reports: Update functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xUpdateReport( STRPTR filename )
{
	struct CurrentReport *	cr;
	BOOL					couldnotcreatereport = FALSE;
	BPTR					old_report;
	
	
		/// Disable input
	
	xDisableWindow( Window );
			
		/// Allocate space for information on this check
	
	if( cr = AllocVec( sizeof( struct CurrentReport ), MEMF_CLEAR ) )
		{
		strcpy( cr->cr_ReportFileName, filename );
		
			/// Open old report
		
		if( old_report = Open( filename, MODE_OLDFILE ) )
			{
				/// Open temp report which will be renamed later
			
			if( cr->cr_ReportFile = Open( ".report.temp", MODE_NEWFILE ) )
				{
				if( xOpenCRIWindow() )
					{
						/// Header
					
					Write( cr->cr_ReportFile, "REPO3.000\n", strlen( "REPO3.000\n" ) );
					
						/// Update
					
					xUpdateObjects( cr, old_report );
					
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
			
			Close( old_report );
			
			if( couldnotcreatereport == FALSE )
				{
				DeleteFile( filename );
				
				Rename( ".report.temp", filename );
				}
			}
		else
			couldnotcreatereport = TRUE;
		
		FreeVec( cr );
		}
	else
		couldnotcreatereport = TRUE;
	
	if( couldnotcreatereport )
		xReqMsg( CRIWindow, GCS( MSG_REQ_INFO ), GCS( MSG_CRI_COULDNOTCREATEREPORT ), GCS( MSG_GF_OKAY ) );
	
		/// Enable input
	
	xEnableWindow( Window );
}

VOID xUpdateObjects( struct CurrentReport *cr, BPTR old_report )
{
	struct FileReport		fr;
	struct FileInfoBlock *	fib;
	BPTR					lock;
	UBYTE *					buffer;
	BOOL					finished = FALSE;
	UBYTE					pathname[ DEFAULTBUFFERLEN ] = "";
	STRPTR					fn,fn2;
	
	
		/// Get rid of the header - 10 bytes ahead
	
	Seek( old_report, 10, OFFSET_CURRENT );
	
		/// Read till end
	
	while( !finished )
		{
			/// Read old report
		
		if( sizeof( struct FileReport ) == Read( old_report, &fr, sizeof( struct FileReport ) ) )
			{
			switch( xHandleCRIWindowIDCMP( cr ) )
				{
				case CW_NOTHING:
					fn = fn2 = FilePart( fr.fr_FileName );
					
					if( fn != fr.fr_FileName )
						if( *(fn - 1) == '/' )
							fn--;
					
					strncpy( pathname, fr.fr_FileName, fn - fr.fr_FileName );
					pathname[ fn - fr.fr_FileName ] = 0;
					
					xCRI_UpdatePathGadget( pathname );
					xCRI_UpdateFileGadget( FilePart( fr.fr_FileName ) );
					
						/// Get new information
					
					if( lock = Lock( fr.fr_FileName, ACCESS_READ ) )
						{
						if( fib = AllocDosObject( DOS_FIB, TAG_DONE ) )
							{
							if( Examine( lock, fib ) )
								{
								strcpy( fr.fr_Comment, fib->fib_Comment );
								fr.fr_Size = fib->fib_Size;
								fr.fr_Date.ds_Days = fib->fib_Date.ds_Days;
								fr.fr_Date.ds_Minute = fib->fib_Date.ds_Minute;
								fr.fr_Date.ds_Tick = fib->fib_Date.ds_Tick;
								fr.fr_Protection = fib->fib_Protection;
								
								fr.fr_CRC = 0;
								
								if( buffer = xStoreFile( fr.fr_FileName, fib->fib_Size ) )
									{
									fr.fr_CRC = xaDoCRC( buffer, fib->fib_Size );
									
									FreeVec( buffer );
									}
								else
									xAddReportCreateErrorNode( 4, GCS( MSG_CRI_COULDNOTCREATECRC ), fr.fr_FileName );
								
									/// Add the file to the report
								
								Write( cr->cr_ReportFile, &fr, sizeof( struct FileReport ) );
								}
							FreeDosObject( DOS_FIB, fib );
							}
						UnLock( lock );
						}
					break;
				
				case CW_STOP:
					cr->cr_Result = CCRES_USERBREAK;
					
					return;
					break;
				}
			}
		else
			finished = TRUE;
		}
}
