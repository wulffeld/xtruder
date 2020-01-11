/*
**	$VER: XtruderSSPBC.c (2.8.96)
**
**	Xtruder file check PBC settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/**
 ---> +---------------------------------------------------------+
      | Check                                                   |
      +---------------------------------------------------------+
      | WX_Protection Bits for Checked files                    |
      |                                                         |
      | PBC bits  R W E D A S P H                               |
      |                                                         |
      | Check files with PBC set                        GT_Chk  |
      | Set PBC on executables                          GT_Chk  |
      | Set PBC on archives                             GT_Chk  |
      +---------------------------------------------------------+
**/

#define		PBC_GADS							13

VOID xOpenFCPBCSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	ULONG					pbc_string;
	UWORD					wh,ww;
	
	
	pbc_string = 25 + xMaxStringLength( ScreenFont, MSG_FCPB_PBCBITS, MSG_FCPB_PBCBITS );
	txtlength  = 25 + xMaxStringLength( ScreenFont, MSG_FCPB_CHECKFILESWITHPBCSET, MSG_FCPB_SETPBCONARCHIVES );
	
	if( (pbc_string + 7 * 30) < txtlength )
		pbc_string = txtlength - 214;
	
	if( gads = AllocVec( sizeof( struct xGadget ) * PBC_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= pbc_string - 3;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4 + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_Flags			= PLACETEXT_ABOVE;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = "_R";
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_FC_PB_PBREAD;
		
		gads[0].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_READ) ? FALSE : TRUE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = "_W";
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_PB_PBWRITE;
		
		gads[1].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_WRITE) ? FALSE : TRUE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = "_E";
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_FC_PB_PBEXECUTABLE;
		
		gads[2].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_EXECUTE) ? FALSE : TRUE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = "_D";
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_FC_PB_PBDELETABLE;
		
		gads[3].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_DELETE) ? FALSE : TRUE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = "_A";
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_FC_PB_PBARCHIVE;
		
		gads[4].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[3].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_ARCHIVE) ? TRUE : FALSE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = "_S";
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_FC_PB_PBSCRIPT;
		
		gads[5].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[4].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_SCRIPT) ? TRUE : FALSE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[6].xg_GadgetText = "_P";
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_FC_PB_PBPURE;
		
		gads[6].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[5].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & FIBF_PURE) ? TRUE : FALSE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_LeftEdge		+= 30;
		ng.ng_GadgetText	= gads[7].xg_GadgetText = "_H";
		ng.ng_GadgetID		= gads[7].xg_GadgetID = GTID_FC_PB_PBHIDDEN;
		
		gads[7].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[6].xg_Gad, &ng,
			GTCB_Checked, (XSettings->xs_FC_PBC_PBCBits & (1 << 7)) ? TRUE : FALSE,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_GadgetText	= gads[8].xg_GadgetText = GCS( MSG_FCPB_CHECKFILESWITHPBCSET );
		ng.ng_GadgetID		= gads[8].xg_GadgetID = GTID_FC_PB_CHECKFILESWITHPBCSET;
		
		gads[8].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[7].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_PBC_CheckPBCFiles,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[9].xg_GadgetText = GCS( MSG_FCPB_SETPBCONEXECUTABLES );
		ng.ng_GadgetID		= gads[9].xg_GadgetID = GTID_FC_PB_SETPBCONEXECUTABLES;
		
		gads[9].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[8].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_PBC_SetPBCOnExe,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[10].xg_GadgetText = GCS( MSG_FCPB_SETPBCONARCHIVES );
		ng.ng_GadgetID		= gads[10].xg_GadgetID = GTID_FC_PB_SETPBCONARCHIVES;
		
		gads[10].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[9].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_PBC_SetPBCOnArchives,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[11].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[11].xg_GadgetID =  GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[10].xg_Gad,
			BUT_Text,		gads[11].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[12].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[12].xg_GadgetID = GTID_CANCEL,
			GA_Left,		ng.ng_LeftEdge + 26 - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[11].xg_Gad,
			BUT_Text,		gads[12].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = ng.ng_LeftEdge + 26 + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCPB_SETTINGS ), ww, wh, glist, gads, 12 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xWTxt( win, pbc_string - 11 - xMaxStringLength( ScreenFont, MSG_FCPB_PBCBITS, MSG_FCPB_PBCBITS ), gads[0].xg_Gad->TopEdge + ScreenFont->tf_Baseline + 2, 1, GCS( MSG_FCPB_PBCBITS ) );
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCPB_BAR_PROTECTIONBITSFOR ) );
			xBarTxt( win, gads[12].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleFCPBCSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				XSettings->xs_FC_PBC_PBCBits =
				( !xIsGadSelected( gads[0].xg_Gad ) ? FIBF_READ : 0 ) |
				( !xIsGadSelected( gads[1].xg_Gad ) ? FIBF_WRITE : 0 ) |
				( !xIsGadSelected( gads[2].xg_Gad ) ? FIBF_EXECUTE : 0 ) |
				( !xIsGadSelected( gads[3].xg_Gad ) ? FIBF_DELETE : 0 ) |
				( xIsGadSelected( gads[4].xg_Gad ) ? FIBF_ARCHIVE : 0 ) |
				( xIsGadSelected( gads[5].xg_Gad ) ? FIBF_SCRIPT : 0 ) |
				( xIsGadSelected( gads[6].xg_Gad ) ? FIBF_PURE : 0 ) |
				( xIsGadSelected( gads[7].xg_Gad ) ? (1 << 7) : 0 );
				
				XSettings->xs_FC_PBC_CheckPBCFiles = xIsGadSelected( gads[8].xg_Gad );
				XSettings->xs_FC_PBC_SetPBCOnExe = xIsGadSelected( gads[9].xg_Gad );
				XSettings->xs_FC_PBC_SetPBCOnArchives = xIsGadSelected( gads[10].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[12].xg_Gad );
		DisposeObject( gads[11].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCPBCSettings( struct Window *win, struct xGadget *gads )
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
							if( tmpgad = xFindGadget( gads, GTID_CANCEL ) )
								xPressButton( win, tmpgad->xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_FCHK_PBC );
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
									
									case GTID_FC_PB_PBREAD:
									case GTID_FC_PB_PBWRITE:
									case GTID_FC_PB_PBEXECUTABLE:
									case GTID_FC_PB_PBDELETABLE:
									case GTID_FC_PB_PBARCHIVE:
									case GTID_FC_PB_PBSCRIPT:
									case GTID_FC_PB_PBPURE:
									case GTID_FC_PB_PBHIDDEN:
									case GTID_FC_PB_CHECKFILESWITHPBCSET:
									case GTID_FC_PB_SETPBCONEXECUTABLES:
									case GTID_FC_PB_SETPBCONARCHIVES:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
												TAG_END );
											}
										else
											code = 0;
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
