/*
**	$VER: XtruderSettingsPaths.c (2.3.96)
**
**	Xtruder settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		PAGT_GADS							14

VOID xOpenPathsSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	UWORD					wh,ww;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_PA_ARCHIVEEXTRACTPATH, MSG_PA_GUIDEPATH );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * PAGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= stringgadlength - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_PA_ARCHIVEEXTRACTPATH );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_PA_ARCHIVEEXTRACTPATH;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTPATHLEN,
			GTST_String,	XSettings->xs_PAT_ArchiveExtractPath,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[1].xg_GadgetID = GTID_PA_GET_ARCHIVEEXTRACTPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[0].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_PA_FILEDECRUNCHPATH );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_PA_FILEDECRUNCHPATH;
		
		gads[2].xg_Gad = CreateGadget( STRING_KIND, gads[1].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTPATHLEN,
			GTST_String,	XSettings->xs_PAT_FileDecrunchPath,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[3].xg_GadgetID = GTID_PA_GET_FILEDECRUNCHPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[2].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_PA_VIRUSPATH );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_PA_VIRUSPATH;
		
		gads[4].xg_Gad		= CreateGadget( STRING_KIND, gads[3].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTPATHLEN,
			GTST_String,	XSettings->xs_PAT_VirusPath,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[5].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[5].xg_GadgetID = GTID_PA_GET_VIRUSPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[4].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[6].xg_GadgetText = GCS( MSG_PA_RECHECKPATH );
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_PA_RECHECKPATH;
		
		gads[6].xg_Gad		= CreateGadget( STRING_KIND, gads[5].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTPATHLEN,
			GTST_String,	XSettings->xs_PAT_RecheckPath,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[7].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[7].xg_GadgetID = GTID_PA_GET_RECHECKPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[6].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[8].xg_GadgetText = GCS( MSG_PA_AUTOCREATEABOVEPATHS );
		ng.ng_GadgetID		= gads[8].xg_GadgetID = GTID_PA_AUTOCREATEABOVEPATHS;
		
		gads[8].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[7].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_PAT_AutoCreateDrawers,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[9].xg_GadgetText = GCS( MSG_PA_AUTODELETEABOVEPATHS );
		ng.ng_GadgetID		= gads[9].xg_GadgetID = GTID_PA_AUTODELETEABOVEPATHS;
		
		gads[9].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[8].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_PAT_AutoDeleteDrawers,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= stringgadlength - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_GadgetText	= gads[10].xg_GadgetText = GCS( MSG_PA_GUIDEPATH );
		ng.ng_GadgetID		= gads[10].xg_GadgetID = GTID_PA_GUIDEPATH;
		
		gads[10].xg_Gad		= CreateGadget( STRING_KIND, gads[9].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTPATHLEN,
			GTST_String,	XSettings->xs_PAT_GuidePath,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[11].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[11].xg_GadgetID = GTID_PA_GET_GUIDEPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[10].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[12].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[12].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[11].xg_Gad,
			BUT_Text,		gads[12].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[13].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[13].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + stringgadlength - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[12].xg_Gad,
			BUT_Text,		gads[13].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + stringgadlength + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_PA_SETTINGS ), ww, wh, glist, gads, PAGT_GADS - 1 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_PA_BAR_PATHS ) );
			xBarTxt( win, gads[ PAGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandlePathsSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				strcpy( XSettings->xs_PAT_ArchiveExtractPath, GETSTRING( gads[0].xg_Gad ) );
				strcpy( XSettings->xs_PAT_FileDecrunchPath, GETSTRING( gads[2].xg_Gad ) );
				strcpy( XSettings->xs_PAT_VirusPath, GETSTRING( gads[4].xg_Gad ) );
				strcpy( XSettings->xs_PAT_RecheckPath, GETSTRING( gads[6].xg_Gad ) );
				XSettings->xs_PAT_AutoCreateDrawers = xIsGadSelected( gads[8].xg_Gad );
				XSettings->xs_PAT_AutoDeleteDrawers = xIsGadSelected( gads[9].xg_Gad );
				strcpy( XSettings->xs_PAT_GuidePath, GETSTRING( gads[10].xg_Gad ) );
				
				xCreatePaths();
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[13].xg_Gad );
		DisposeObject( gads[12].xg_Gad );
		DisposeObject( gads[11].xg_Gad );
		DisposeObject( gads[7].xg_Gad );
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[3].xg_Gad );
		DisposeObject( gads[1].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandlePathsSettings( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					code,altcode,qualifier;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ];
	
	
	while( 1 )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			code = altcode = msg->Code;
			qualifier = msg->Qualifier;
			ia = msg->IAddress;
			
			if( class == IDCMP_GADGETUP )
				code = ((struct Gadget *)msg->IAddress)->GadgetID;
			
			GT_ReplyIMsg( msg );
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( win );
					GT_EndRefresh( win, TRUE );
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case KEY_ESCAPE:
							xPressButton( win, gads[ PAGT_GADS - 1 ].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_PATHS );
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
									
									case GTID_PA_ARCHIVEEXTRACTPATH:
									case GTID_PA_FILEDECRUNCHPATH:
									case GTID_PA_VIRUSPATH:
									case GTID_PA_RECHECKPATH:
									case GTID_PA_GUIDEPATH:
										ActivateGadget( tmpgad->xg_Gad, win, NULL );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_PA_AUTOCREATEABOVEPATHS:
									case GTID_PA_AUTODELETEABOVEPATHS:
										GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
											GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
											TAG_END );
									
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
						case GTID_PA_ARCHIVEEXTRACTPATH:
						case GTID_PA_FILEDECRUNCHPATH:
						case GTID_PA_VIRUSPATH:
						case GTID_PA_RECHECKPATH:
						case GTID_PA_GUIDEPATH:
							if( altcode != HOOK_CURSORDOWN )
								break;
							else
								code++;
						
						case GTID_PA_GET_ARCHIVEEXTRACTPATH:
						case GTID_PA_GET_FILEDECRUNCHPATH:
						case GTID_PA_GET_VIRUSPATH:
						case GTID_PA_GET_RECHECKPATH:
						case GTID_PA_GET_GUIDEPATH:
							code--;
							
							xDisableWindow( win );
							
								/// Find string gadget
							
							if( tmpgad = xFindGadget( gads, code ) )
								{
								if( xSelectPath( win,
									SELFP_ReqTitle,	GCS( MSG_TL_SELECTPATH ),
									SELFP_DestDir,	strbuf,
									SELFP_Gadget,	tmpgad->xg_Gad,
									TAG_END ) )
									{
									xSetGadgetString( win, tmpgad->xg_Gad, strbuf );
									}
								}
							
							xEnableWindow( win );
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

VOID xCreatePaths( VOID )
{
	struct FileLock *	fl;
	BPTR				lock;
	UBYTE *				paths[4];
	UBYTE				p;
	
	
	paths[0] = XSettings->xs_PAT_ArchiveExtractPath;
	paths[1] = XSettings->xs_PAT_FileDecrunchPath;
	paths[2] = XSettings->xs_PAT_VirusPath;
	paths[3] = XSettings->xs_PAT_RecheckPath;
	
	for( p = 0; p < 4; p++ )
		{
		if( paths[ p ][ 0 ] )
			{
			if( lock = Lock( paths[ p ], ACCESS_READ ) )
				UnLock( lock );
			else
				{
				BOOL	create = TRUE;
				
				
				if( XSettings->xs_PAT_AutoCreateDrawers == FALSE )
					create = xReqMsg( Window, GCS( MSG_REQ_WARNING ), GCS( MSG_PATHDOESNOTEXIST ), GCS( MSG_GF_YESNO ), paths[ p ] );
				
				if( create )
					{
					if( fl = (struct FileLock *)CreateDir( paths[ p ] ) )
						UnLock( (BPTR)fl );
					else
						xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTCREATEPATH ), GCS( MSG_GF_OKAY ), paths[ p ] );
					}
				}
			}
		}
}
