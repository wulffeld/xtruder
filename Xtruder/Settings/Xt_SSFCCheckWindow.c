/*
**	$VER: XtruderSettingsCheckWindow.c (30.7.96)
**
**	Xtruder check window settings module
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		CWGT_GADS							8

VOID xOpenFCCheckWindowSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	ULONG					t;
	UWORD					wh,ww;
	
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_FCCW_AUTOCLOSE, MSG_FCCW_WARNINGS );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * CWGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
			/// Auto close
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_FCCW_AUTOCLOSE );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_FC_CW_AUTOCLOSE;
		
		gads[0].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_CHW_AutoClose,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Memory information
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_FCCW_MEMORYINFORMATION );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_CW_MEMORYINFORMATION;
		
		gads[1].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_CHW_MemoryInformation,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Check information
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_FCCW_CHECKINFORMATION );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_FC_CW_CHECKINFORMATION;
		
		gads[2].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_CHW_CheckInformation,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// History
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_FCCW_HISTORY );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_FC_CW_HISTORY;
		
		gads[3].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_CHW_History,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Auto scroll
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_FCCW_AUTOSCROLL );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_FC_CW_AUTOSCROLL;
		
		gads[4].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[3].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_CHW_AutoScroll,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			GA_Disabled,	!XSettings->xs_FC_CHW_History,
			TAG_END );
		
			/// Warnings
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = GCS( MSG_FCCW_WARNINGS );
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_FC_CW_WARNINGS;
		
		gads[5].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[4].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_CHW_Warnings,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Okay and Cancel
		
		t = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[6].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[6].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			t,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[5].xg_Gad,
			BUT_Text,		gads[6].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[7].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[7].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + 26 - ConfirmGadWidth,
			GA_Top,			t,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[6].xg_Gad,
			BUT_Text,		gads[7].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh = t + GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + 26 + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCCW_SETTINGS ), ww, wh, glist, gads, CWGT_GADS - 1 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCCW_BAR_CHECKWINDOW ) );
			xBarTxt( win, gads[ CWGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleFCCheckWindowSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				XSettings->xs_FC_CHW_AutoClose = xIsGadSelected( gads[0].xg_Gad );
				XSettings->xs_FC_CHW_MemoryInformation = xIsGadSelected( gads[1].xg_Gad );
				XSettings->xs_FC_CHW_CheckInformation = xIsGadSelected( gads[2].xg_Gad );
				XSettings->xs_FC_CHW_History = xIsGadSelected( gads[3].xg_Gad );
				XSettings->xs_FC_CHW_AutoScroll = xIsGadSelected( gads[4].xg_Gad );
				XSettings->xs_FC_CHW_Warnings = xIsGadSelected( gads[5].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[7].xg_Gad );
		DisposeObject( gads[6].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCCheckWindowSettings( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					code,qualifier;
	
	
	while( 1 )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			code = msg->Code;
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
							xPressButton( win, gads[ CWGT_GADS - 1 ].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_FCHK_CHECKWINDOW );
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
									
									case GTID_FC_CW_AUTOCLOSE:
									case GTID_FC_CW_MEMORYINFORMATION:
									case GTID_FC_CW_CHECKINFORMATION:
									case GTID_FC_CW_HISTORY:
									case GTID_FC_CW_AUTOSCROLL:
									case GTID_FC_CW_WARNINGS:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
												TAG_END );
											
											code = tmpgad->xg_GadgetID;
											}
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
						case GTID_FC_CW_HISTORY:
							if( !xIsGadSelected( gads[ 3 ].xg_Gad ) )
								xGhostGadget( win, gads[ 4 ].xg_Gad );
							else
								xUnGhostGadget( win, gads[ 4 ].xg_Gad );
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
