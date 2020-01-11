/*
**	$VER: XtruderCheckWindowUpdate.c (29.7.96)
**
**	Xtruder file check window update functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xCW_UpdatePathGadget( STRPTR pathname, BOOL use_static_contents )
{
	static UBYTE	pathnamebuf[ 250 ];
	
	
		/// If CheckWindow is valid the path gadget will certainly exist!
	
	if( CheckWindow )
		{
			/// In case of resize of check window it is necessary to use the static contents
		
		if( !use_static_contents )
			strncpy( pathnamebuf, pathname, 249 );
		
		GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_PATH ].xg_Gad, CheckWindow, NULL, GTTX_Text, pathnamebuf, TAG_END );
		}
}

VOID xCW_UpdateFileGadget( STRPTR filename, BOOL use_static_contents )
{
	static UBYTE	filenamebuf[ 50 ];
	
	
		/// If CheckWindow is valid the file gadget will certainly exist!
	
	if( CheckWindow )
		{
			/// In case of resize of check window it is necessary to use the static contents
		
		if( !use_static_contents )
			strncpy( filenamebuf, filename, 49 );
		
		GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_FILE ].xg_Gad, CheckWindow, NULL, GTTX_Text, filenamebuf, TAG_END );
		}
}

VOID xCW_UpdateMemoryInfo( VOID )
{
	if( CheckWindow && XSettings->xs_FC_CHW_MemoryInformation )
		{
		xWTxt( CheckWindow, 14, MemoryBox_Top + TextFont->tf_Baseline + 2, 1, GCS( MSG_CW_MEMORYINFO1 ), AvailMem( MEMF_CHIP ), AvailMem( MEMF_FAST ) );
		xWTxt( CheckWindow, 14, MemoryBox_Top + TextFont->tf_Baseline + TxtFontYSize + 2, 1, GCS( MSG_CW_MEMORYINFO2 ), AvailMem( MEMF_CHIP | MEMF_LARGEST ), AvailMem( MEMF_FAST | MEMF_LARGEST ) );
		}
}

VOID xCW_UpdateCheckInfo( struct CurrentCheck *cc )
{
	if( CheckWindow && XSettings->xs_FC_CHW_CheckInformation )
		{
		static	ULONG	oldhours,oldmins,oldsecs;
		ULONG	secs,mics;
		ULONG	hours,mins;
		
		
		if( cc->cc_Result != CCRES_CHECKEND )
			{
				/// Store end time of check					
			
			CurrentTime( &secs, &mics );
			
				/// Calculate how long the check took
			
			secs -= TimeSeconds;
			
			oldhours = hours = secs / 3600;
			secs -= hours * 3600;
			oldmins = mins = secs / 60;
			oldsecs = secs -= mins * 60;
			}
		else
			{
			hours = oldhours;
			mins = oldmins;
			secs = oldsecs;
			}
			
		xWTxt( CheckWindow, 14, CheckInfoBox_Top + TextFont->tf_Baseline + 2, 1, GCS( MSG_CW_CHECKINFO1 ), cc->cc_DirsEncountered, cc->cc_FilesEncountered, cc->cc_FilesChecked );
		xWTxt( CheckWindow, 14, CheckInfoBox_Top + TextFont->tf_Baseline + TxtFontYSize + 2, 1, GCS( MSG_CW_CHECKINFO2 ), cc->cc_FilesCrunched, cc->cc_FilesDecrunched );
		xWTxt( CheckWindow, 14, CheckInfoBox_Top + TextFont->tf_Baseline + TxtFontYSize * 2 + 2, 1, GCS( MSG_CW_CHECKINFO3 ), cc->cc_ArchivedFilesEncountered, cc->cc_ArchivedFilesExtracted );
		xWTxt( CheckWindow, 14, CheckInfoBox_Top + TextFont->tf_Baseline + TxtFontYSize * 3 + 2, 1, GCS( MSG_CW_CHECKINFO4 ), cc->cc_ViraFound, hours, mins, secs );
		}
}

UWORD xCheckWindowMsg( struct CurrentCheck *cc )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	ULONG					msgclass;
	APTR					ia;
	UWORD					msgcode;
	UWORD					qualifier;
	struct NewLVPos			nlv = {0,0};
	LONG					curvistop;
	
	
	while( msg = GT_GetIMsg( CheckWindow->UserPort ) )
		{
		msgclass = msg->Class;
		msgcode = msg->Code;
		qualifier = msg->Qualifier;
		ia = msg->IAddress;
		
		if( msgclass == IDCMP_GADGETUP )
			msgcode = ((struct Gadget *)msg->IAddress)->GadgetID;
		
		GT_ReplyIMsg( msg );
		
		switch( msgclass )
			{
			case IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( CheckWindow );
				GT_EndRefresh( CheckWindow, TRUE );
				break;
			
			case IDCMP_NEWSIZE:
				xCreateCheckWindowGadgets();
				xDrawCheckWindowBoxes();
				
				xCW_UpdatePathGadget( "", TRUE );
				xCW_UpdateFileGadget( "", TRUE );
				
				xCW_UpdateMemoryInfo();
				xCW_UpdateCheckInfo( cc );
				break;
			
			case IDCMP_RAWKEY:
				switch( msgcode )
					{
					case CURSORUP:
					case CURSORDOWN:
						if( !XSettings->xs_FC_CHW_AutoScroll || cc->cc_Result == CCRES_CHECKEND )
							{
							curvistop = xListViewTop( CheckWindowGadgets[ GAD_CW_CHECKHISTORY ].xg_Gad, CheckWindow );
							
							xNewListPos( &nlv, qualifier, msgcode, msgcode == CURSORUP ? curvistop : curvistop + HistoryLVVisible - 1, curvistop, HistoryLVVisible, CheckWindowHistoryList.cl_Entries );
							
							GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_CHECKHISTORY ].xg_Gad, CheckWindow, NULL,
								GTLV_Top, nlv.nl_NewTop,
								TAG_END );
							}
						break;
					
					case KEY_ESCAPE:
						if( cc->cc_Result == CCRES_CHECKEND )
							return( CCRES_CHECKEND );
						break;
					
					case KEY_HELP:
						xGuideDisplay( AGH_FILECHECK );
						break;
					
					default:
						if( tmpgad = xCheckUnderscore( CheckWindowGadgets, msgcode, qualifier, ia ) )
							{
							switch( tmpgad->xg_GadgetID )
								{
								case GTID_CWIN_STOP:
								case GTID_CWIN_PAUSE:
								case GTID_CWIN_HISTORY:
									if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
										{
										xPressButton( CheckWindow, tmpgad->xg_Gad );
										
										msgcode = tmpgad->xg_GadgetID;
										}
									break;
								}
							}
						else
							break;
					}
			
			case IDCMP_GADGETUP:
				switch( msgcode )
					{
					case GTID_CWIN_STOP:
						return( CW_STOP );
						break;
					
					case GTID_CWIN_PAUSE:
						xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_CW_OPERATIONPAUSED ), GCS( MSG_GF_CONTINUE ) );
						break;
					
					case GTID_CWIN_HISTORY:
						xDisableWindow( CheckWindow );
						
						xShowHistory();
						
						xEnableWindow( CheckWindow );
						break;
					}
				break;
			
			case IDCMP_CLOSEWINDOW:
				return( CCRES_CHECKEND );
				break;
			}
		}
	
	return( CW_NOTHING );
}

LONG xHandleCheckWindowIDCMP( struct CurrentCheck *cc )
{
	UWORD					res = CW_NOTHING;
	
	
	if( CheckWindow )
		{
		if( cc->cc_Result && cc->cc_Result != CCRES_CHECKEND )
			return( cc->cc_Result );
		
		if( cc->cc_Result == CCRES_CHECKEND )
			{
			StillChecking = FALSE;
			
				/// Clear gadgets
			
			xCW_UpdatePathGadget( "", FALSE );
			xCW_UpdateFileGadget( "", FALSE );
			
				/// (Un)Ghost gadgets
			
			xGhostObject( CheckWindow, CheckWindowGadgets[ 2 ].xg_Gad );
			xGhostObject( CheckWindow, CheckWindowGadgets[ 3 ].xg_Gad );
			
			if( !IsListEmpty( &XtruderHistory->his_List ) )
				xUnGhostObject( CheckWindow, CheckWindowGadgets[ 4 ].xg_Gad );
			
				/// Wait for the user to close the window
			
			do
				{
				WaitPort( CheckWindow->UserPort );
				}
			while( xCheckWindowMsg( cc ) != CCRES_CHECKEND );
			}
		else
			res = xCheckWindowMsg( cc );
		}
	
	return( res );
}
