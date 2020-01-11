/*
**	$VER: XtruderSettingsStartup.c (2.3.95)
**
**	Xtruder settings module
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		STGT_GADS							8

VOID xOpenStartupSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	UWORD					wh,ww;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_ST_STARTUPSCRIPT, MSG_ST_SHOWSYSTEMINFORMATION );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * STGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= stringgadlength - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_ST_STARTUPSCRIPT );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_ST_STARTUPSCRIPT;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
			GTST_String,	XSettings->xs_STA_StartupScript,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			GA_Disabled,	!XSettings->xs_STA_ExecuteScript,
			TAG_END );
		
		gads[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[1].xg_GadgetID = GTID_ST_GETSTARTUPSCRIPT,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[0].xg_Gad,
			GA_Disabled,	!XSettings->xs_STA_ExecuteScript,
			BUT_Type,		BUTTYPE_FILE,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_ST_EXECUTESTARTUPSCRIPT );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_ST_EXECUTESTARTUPSCRIPT;
		
		gads[2].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_STA_ExecuteScript,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2 + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_ST_CHECKPRESPECIFIED );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_ST_CHECKPRESPECIFIED;
		
		gads[3].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_STA_CheckPrespecified,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_ST_CHECKVECTORS );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_ST_CHECKVECTORS;
		
		gads[4].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[3].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_STA_CheckVectors,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = GCS( MSG_ST_SHOWSYSTEMINFORMATION );
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_ST_SHOWSYSTEMINFORMATION;
		
		gads[5].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[4].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_STA_ShowSystemInformation,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[6].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[6].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[5].xg_Gad,
			BUT_Text,		gads[6].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[7].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[7].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + stringgadlength - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[6].xg_Gad,
			BUT_Text,		gads[7].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + stringgadlength + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_ST_SETTINGS ), ww, wh, glist, gads, STGT_GADS - 1 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_ST_BAR_AREXX ) );
			xBarTxt( win, gads[3].xg_Gad->TopEdge - 4, GCS( MSG_ST_BAR_STARTUP ) );
			xBarTxt( win, gads[ STGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleStartupSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				strcpy( XSettings->xs_STA_StartupScript, GETSTRING( gads[0].xg_Gad ) );
				XSettings->xs_STA_ExecuteScript = xIsGadSelected( gads[2].xg_Gad );
				XSettings->xs_STA_CheckPrespecified = xIsGadSelected( gads[3].xg_Gad );
				XSettings->xs_STA_CheckVectors = xIsGadSelected( gads[4].xg_Gad );
				XSettings->xs_STA_ShowSystemInformation = xIsGadSelected( gads[5].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[7].xg_Gad );
		DisposeObject( gads[6].xg_Gad );
		DisposeObject( gads[1].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleStartupSettings( struct Window *win, struct xGadget *gads )
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
			
			if( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN) )
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
							xPressButton( win, gads[ STGT_GADS - 1 ].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_STARTUP );
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
									
									case GTID_ST_STARTUPSCRIPT:
										ActivateGadget( tmpgad->xg_Gad, win, NULL );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_ST_EXECUTESTARTUPSCRIPT:
									case GTID_ST_CHECKPRESPECIFIED:
									case GTID_ST_CHECKVECTORS:
									case GTID_ST_SHOWSYSTEMINFORMATION:
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
						case GTID_ST_STARTUPSCRIPT:
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_ST_GETSTARTUPSCRIPT:
							xDisableWindow( win );
							
							if( xSelectFile( win,
								SELFP_ReqTitle,	GCS( MSG_TL_SELECTSCRIPT ),
								SELFP_DestFile,	strbuf,
								SELFP_Gadget,	gads[ 0 ].xg_Gad,
								TAG_END ) )
								{
								xSetGadgetString( win, gads[ 0 ].xg_Gad, strbuf );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_ST_EXECUTESTARTUPSCRIPT:
							if( xIsGadSelected( gads[2].xg_Gad ) )
								{
								xUnGhostGadget( win, gads[0].xg_Gad );
								xUnGhostObject( win, gads[1].xg_Gad );
								}
							else
								{
								xGhostGadget( win, gads[0].xg_Gad );
								xGhostObject( win, gads[1].xg_Gad );
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
