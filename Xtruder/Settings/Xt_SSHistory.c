/*
**	$VER: XtruderSettingsHistory.c (2.3.96)
**
**	Xtruder settings module
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		PRGT_HISFILENAME					2
#define		PRGT_GETHISFILENAME					3
#define		PRGT_USEDATEANDTIME					4
#define		PRGT_APPEND							5
#define		PRGT_REQFILENAME					6

#define		HIGT_GADS							12

VOID xOpenHistorySettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	UBYTE *					sm_labels[] = { "", "", "", NULL };
	ULONG					txtlength;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	ULONG					k,s = stringgadlength;
	UWORD					b1;
	UWORD					wh,ww;
	
	
	sm_labels[0] = GCS( MSG_HI_AFTERACTION );
	sm_labels[1] = GCS( MSG_HI_AFTERACTIONTHENFLUSH );
	sm_labels[2] = GCS( MSG_HI_DONOTSAVE );
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_HI_HISTORYPATH, MSG_HI_FILECHECK );
	k         = 35 + xMaxStringLength( ScreenFont, MSG_HI_AFTERACTION, MSG_HI_DONOTSAVE );
	
	if( k > s )
		s = k;
	
	if( gads = AllocVec( sizeof( struct xGadget ) * HIGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
			/// History path
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= s - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_HI_HISTORYPATH );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_HI_HISTORYPATH;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTPATHLEN,
			GTST_String,	XSettings->xs_HIS_HistoryPath,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Get history path
		
		gads[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[1].xg_GadgetID = GTID_HI_GETHISTORYPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[0].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
			/// History file
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_HI_HISTORYFILE );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_HI_HISTORYFILE;
		
		gads[2].xg_Gad = CreateGadget( STRING_KIND, gads[1].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
			GTST_String,	XSettings->xs_HIS_HistoryFile,
			GTIN_EditHook,	&CursorDownHook,
			GA_Disabled,	XSettings->xs_HIS_UseDateAndTime,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[3].xg_GadgetID = GTID_HI_GETHISTORYFILE,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[2].xg_Gad,
			GA_Disabled,	XSettings->xs_HIS_UseDateAndTime,
			BUT_Type,		BUTTYPE_FILE,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		if( XSettings->xs_HIS_RequestFilename )
			XSettings->xs_HIS_Append = XSettings->xs_HIS_UseDateAndTime = FALSE;
		else
		if( XSettings->xs_HIS_Append )
			XSettings->xs_HIS_UseDateAndTime = FALSE;
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_HI_USEDATEANDTIME );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_HI_USEDATEANDTIME;
		
		gads[4].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[3].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_HIS_UseDateAndTime,
			GTCB_Scaled,	TRUE,
			GA_Disabled,	XSettings->xs_HIS_RequestFilename | XSettings->xs_HIS_Append,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = GCS( MSG_HI_APPEND );
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_HI_APPEND;
		
		gads[5].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[4].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_HIS_Append,
			GTCB_Scaled,	TRUE,
			GA_Disabled,	XSettings->xs_HIS_RequestFilename | XSettings->xs_HIS_UseDateAndTime,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[6].xg_GadgetText = GCS( MSG_HI_REQUESTFILENAME );
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_HI_REQUESTFILENAME;
		
		gads[6].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[5].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_HIS_RequestFilename,
			GTCB_Scaled,	TRUE,
			GA_Disabled,	XSettings->xs_HIS_UseDateAndTime | XSettings->xs_HIS_Append,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Max entries
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= s - xProportionalLength( ScreenFont, 6 );
		ng.ng_Height		= ScrFontYSize + 4;
		ng.ng_GadgetText	= gads[7].xg_GadgetText = GCS( MSG_HI_MAXENTRIES );
		ng.ng_GadgetID		= gads[7].xg_GadgetID = GTID_HI_MAXENTRIES;
		
		gads[7].xg_Gad = CreateGadget( SLIDER_KIND, gads[6].xg_Gad, &ng,
			GA_Immediate,		TRUE,
			GA_RelVerify,		TRUE,
			GTSL_Min,			0,
			GTSL_Max,			32765,
			GTSL_MaxLevelLen,	5,
			GTSL_Level,			XSettings->xs_HIS_MaxEntries,
			GTSL_LevelFormat,	"%ld",
			GTSL_LevelPlace,	PLACETEXT_RIGHT,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Save method
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= s;
		ng.ng_Height		= GadgetHeight;
		ng.ng_GadgetText	= gads[8].xg_GadgetText = GCS( MSG_HI_SAVEMETHOD );
		ng.ng_GadgetID		= gads[8].xg_GadgetID = GTID_HI_SAVEMETHOD;
		
		gads[8].xg_Gad = CreateGadget( CYCLE_KIND, gads[7].xg_Gad, &ng,
			GTCY_Active,	XSettings->xs_HIS_SaveMethod,
			GTCY_Labels,	sm_labels,
			GT_Underscore,	'_',
			TAG_END );
		
		b1 = ng.ng_TopEdge	+= ng.ng_Height + 2 + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[9].xg_GadgetText = GCS( MSG_HI_FILECHECK );
		ng.ng_GadgetID		= gads[9].xg_GadgetID = GTID_HI_FILECHECK;
		
		gads[9].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[8].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_HIS_RecFileCheck,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Okay and Cancel
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[10].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[10].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[9].xg_Gad,
			BUT_Text,		gads[10].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[11].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[11].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + s - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[10].xg_Gad,
			BUT_Text,		gads[11].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + s + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_HI_SETTINGS ), ww, wh, glist, gads, HIGT_GADS - 1 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_HI_BAR_HISTORY ) );
			xBarTxt( win, b1 - 4, GCS( MSG_HI_BAR_ACTIONSTORECORD ) );
			xBarTxt( win, gads[ HIGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleHistorySettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				strcpy( XSettings->xs_HIS_HistoryPath, GETSTRING( gads[0].xg_Gad ) );
				strcpy( XSettings->xs_HIS_HistoryFile, GETSTRING( gads[2].xg_Gad ) );
				
				XSettings->xs_HIS_UseDateAndTime = xIsGadSelected( gads[4].xg_Gad );
				XSettings->xs_HIS_Append = xIsGadSelected( gads[5].xg_Gad );
				XSettings->xs_HIS_RequestFilename = xIsGadSelected( gads[6].xg_Gad );
				
				XSettings->xs_HIS_MaxEntries = xSliderLevel( gads[7].xg_Gad, win );
				XSettings->xs_HIS_SaveMethod = xCycleActive( gads[8].xg_Gad, win );
				
				XSettings->xs_HIS_RecFileCheck = xIsGadSelected( gads[9].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[11].xg_Gad );
		DisposeObject( gads[10].xg_Gad );
		DisposeObject( gads[3].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleHistorySettings( struct Window *win, struct xGadget gads[] )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					altcode,code,qualifier;
	BOOL					fromraw;
	UWORD					xp_maxentries = XSettings->xs_HIS_MaxEntries;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ];
	
	
	while( 1 )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			altcode = code = msg->Code;
			qualifier = msg->Qualifier;
			ia = msg->IAddress;
			
			if( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN) )
				code = ((struct Gadget *)msg->IAddress)->GadgetID;
			
			GT_ReplyIMsg( msg );
			
			tmpgad = NULL;
			
			fromraw = FALSE;
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( win );
					GT_EndRefresh( win, TRUE );
					break;
				
				case IDCMP_RAWKEY:
					fromraw = TRUE;
					
					switch( code )
						{
						case KEY_ESCAPE:
							xPressButton( win, gads[ HIGT_GADS - 1 ].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_HISTORY );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( gads, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_OKAY:
									case GTID_CANCEL:
										xPressButton( win, tmpgad->xg_Gad );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_HI_SAVEMETHOD:
										xRefreshCycleGadget( win, tmpgad->xg_Gad, qualifier, 2 );
										
										code = 0;
										break;
									
									case GTID_HI_HISTORYPATH:
									case GTID_HI_HISTORYFILE:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											ActivateGadget( tmpgad->xg_Gad, win, NULL );
									
									case GTID_HI_MAXENTRIES:
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_HI_USEDATEANDTIME:
									case GTID_HI_APPEND:
									case GTID_HI_REQUESTFILENAME:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
												TAG_END );
											
											altcode = xIsGadSelected( tmpgad->xg_Gad );
											code = tmpgad->xg_GadgetID;
											}
										break;
									
									case GTID_HI_FILECHECK:
										GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
											GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
											TAG_END );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									default:
										code = 0;
										break;
									}
								}
							else
								break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_HI_HISTORYPATH:
							if( (altcode != HOOK_CURSORDOWN) )
								break;
						
						case GTID_HI_GETHISTORYPATH:
							xDisableWindow( win );
							
							if( xSelectPath( win,
								SELFP_ReqTitle,	GCS( MSG_TL_SELECTPATH ),
								SELFP_DestDir,	strbuf,
								SELFP_Gadget,	gads[ 0 ].xg_Gad,
								TAG_END ) )
								{
								xSetGadgetString( win, gads[ 0 ].xg_Gad, strbuf );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_HI_HISTORYFILE:
							if( (altcode != HOOK_CURSORDOWN) || (tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
								break;
						
						case GTID_HI_GETHISTORYFILE:
							xDisableWindow( win );
							
							if( xSelectFile( win,
								SELFP_ReqTitle,	GCS( MSG_TL_SELECTFILE ),
								SELFP_Dir,		GETSTRING( gads[ 0 ].xg_Gad ),
								SELFP_DestFile,	strbuf,
								TAG_END ) )
								{
								xSetGadgetString( win, gads[ 2 ].xg_Gad, FilePart( strbuf ) );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_HI_MAXENTRIES:
							if( fromraw )
								{
								qualifier &= IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
								
								if( (qualifier && xp_maxentries > 0) || (!qualifier && xp_maxentries < 32764) )
									{
									if( qualifier )
										xp_maxentries--;
									else
										xp_maxentries++;
									
									GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
										GTSL_Level, xp_maxentries,
										TAG_END );
									}
								}
							else
								xp_maxentries = altcode;
							break;
						
						case GTID_HI_USEDATEANDTIME:
							if( altcode )
								{
								xGhostObject( win, gads[ PRGT_GETHISFILENAME ].xg_Gad );
								xGhostGadget( win, gads[ PRGT_HISFILENAME    ].xg_Gad );
								xGhostGadget( win, gads[ PRGT_APPEND         ].xg_Gad );
								xGhostGadget( win, gads[ PRGT_REQFILENAME    ].xg_Gad );
								}
							else
								{
								xUnGhostObject( win, gads[ PRGT_GETHISFILENAME ].xg_Gad );
								xUnGhostGadget( win, gads[ PRGT_HISFILENAME    ].xg_Gad );
								xUnGhostGadget( win, gads[ PRGT_APPEND         ].xg_Gad );
								xUnGhostGadget( win, gads[ PRGT_REQFILENAME    ].xg_Gad );
								}
							break;
						
						case GTID_HI_APPEND:
							if( altcode )
								{
								xGhostGadget( win, gads[ PRGT_USEDATEANDTIME ].xg_Gad );
								xGhostGadget( win, gads[ PRGT_REQFILENAME    ].xg_Gad );
								}
							else
								{
								xUnGhostGadget( win, gads[ PRGT_USEDATEANDTIME ].xg_Gad );
								xUnGhostGadget( win, gads[ PRGT_REQFILENAME    ].xg_Gad );
								}
							break;
						
						case GTID_HI_REQUESTFILENAME:
							if( altcode )
								{
								xGhostGadget( win, gads[ PRGT_USEDATEANDTIME ].xg_Gad );
								xGhostGadget( win, gads[ PRGT_APPEND         ].xg_Gad );
								}
							else
								{
								xUnGhostGadget( win, gads[ PRGT_USEDATEANDTIME ].xg_Gad );
								xUnGhostGadget( win, gads[ PRGT_APPEND         ].xg_Gad );
								}
							break;
						
						case GTID_OKAY:
							return( TRUE );
							break;
						
						case GTID_CANCEL:
							return( FALSE );
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					return( FALSE );
					break;
				}
			}
		}
}
