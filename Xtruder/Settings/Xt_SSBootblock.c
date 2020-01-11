/*
**	$VER: XtruderSettingsMODBootblock.c (2.3.96)
**
**	Xtruder settings module
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		BBGT_GADS							6

VOID xOpenBootblockSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	ULONG					t;
	UWORD					wh,ww;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_BB_CUSTOMBOOTBLOCK, MSG_BB_CHECKINSERTEDDISKS );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * BBGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= stringgadlength - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_BB_CUSTOMBOOTBLOCK );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_BB_CUSTOMBOOTBLOCK;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	39,
			GTST_String,	XSettings->xs_BOO_CustomBBFile,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[1].xg_GadgetID = GTID_BB_GETCUSTOMBOOTBLOCK,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[0].xg_Gad,
			BUT_Type,		BUTTYPE_FILE,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_Width			= 26;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_BB_CHECKINSERTEDDISKS );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_BB_CHECKINSERTEDDISKS;
		
		gads[2].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_BOO_CheckInsertedDisks,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_BB_NOINTERFACE );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_BB_NOINTERFACE;
		
		gads[3].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_BOO_NoInterface,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		t = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[4].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[4].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			t,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[3].xg_Gad,
			BUT_Text,		gads[4].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[5].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[5].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + stringgadlength - ConfirmGadWidth,
			GA_Top,			t,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[4].xg_Gad,
			BUT_Text,		gads[5].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh = t + GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + stringgadlength + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_BB_SETTINGS ), ww, wh, glist, gads, BBGT_GADS - 1 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_BB_BAR_BOOTBLOCK ) );
			xBarTxt( win, gads[ BBGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleBootblockSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				strcpy( XSettings->xs_BOO_CustomBBFile, GETSTRING( gads[0].xg_Gad ) );
				XSettings->xs_BOO_CheckInsertedDisks = xIsGadSelected( gads[2].xg_Gad );
				XSettings->xs_BOO_NoInterface = xIsGadSelected( gads[3].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[4].xg_Gad );
		DisposeObject( gads[1].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleBootblockSettings( struct Window *win, struct xGadget gads[] )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					code,altcode,qualifier;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
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
							xPressButton( win, gads[ BBGT_GADS - 1 ].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_BOOTBLOCK );
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
									
									case GTID_BB_CUSTOMBOOTBLOCK:
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_BB_CHECKINSERTEDDISKS:
									case GTID_BB_NOINTERFACE:
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
						case GTID_BB_CUSTOMBOOTBLOCK:
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_BB_GETCUSTOMBOOTBLOCK:
							xDisableWindow( win );
							
							if( xSelectFile( win,
								SELFP_ReqTitle,	GCS( MSG_BB_SELECTCUSTOM ),
								SELFP_DestFile,	strbuf,
								SELFP_Gadget,	gads[ 0 ].xg_Gad,
								TAG_END ) )
								{
								xSetGadgetString( win, gads[ 0 ].xg_Gad, strbuf );
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
