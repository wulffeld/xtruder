/*
**	$VER: XtruderSSFCUnpacking.c (2.8.96)
**
**	Xtruder file check unpacking settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/**
 ---> +---------------------------------------------------------+
      | Check                                                   |
      +---------------------------------------------------------+
      | WX_Unpacking                                            |
      |                                                         |
      |                           When decrunching use GT_Cycle |
      |                               Extract archives  GT_Chk  |
      |                          Ask before extracting  GT_Chk  |
      |            Ask before deleting extracted files  GT_Chk  |
      |                           Decrunch executables  GT_Chk  |
      |                            Decrunch data files  GT_Chk  |
      |                   Ask before decrunching files  GT_Chk  |
      |           Ask before deleting decrunched files  GT_Chk  |
      |                       Decrunch encrypted files  GT_Chk  |
      |                            Check DMS bootblock  GT_Chk  |
      +---------------------------------------------------------+
**/

#define		UNPACK_GADS							23

VOID xOpenFCUnpackingSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	UBYTE *					wdu_labels[] = { "", "", "", "", NULL };
	ULONG					txtlength;
	UWORD					wh,ww;
	UWORD					cyclewidth;
	BOOL					use_arc = !(XSettings->xs_FC_UNP_ExtractArchives);
	BOOL					use_dec = !(XSettings->xs_FC_UNP_DecrunchExecutables || XSettings->xs_FC_UNP_DecrunchDataFiles);
	
	
	cyclewidth = 35 + xMaxStringLength( ScreenFont, MSG_FCUN_WHEN_UNPACKTHENXFD, MSG_FCUN_WHEN_ONLYXFD );
	txtlength  = 15 + xMaxStringLength( ScreenFont, MSG_FCUN_WHENDECRUNCHINGUSE, MSG_FCUN_CHECKDMSBOOTBLOCK );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * UNPACK_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		wdu_labels[0] = GCS( MSG_FCUN_WHEN_UNPACKTHENXFD );
		wdu_labels[1] = GCS( MSG_FCUN_WHEN_XFDTHENUNPACK );
		wdu_labels[2] = GCS( MSG_FCUN_WHEN_ONLYUNPACK );
		wdu_labels[3] = GCS( MSG_FCUN_WHEN_ONLYXFD );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= cyclewidth;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_FCUN_WHENDECRUNCHINGUSE );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_FC_UN_WHENDECRUNCHINGUSE;
		
		gads[0].xg_Gad = CreateGadget( CYCLE_KIND, gads[0].xg_Gad, &ng,
			GTCY_Active,	XSettings->xs_FC_UNP_WhenDecrunchingUse,
			GTCY_Labels,	wdu_labels,
			GT_Underscore,	'_',
			GA_Disabled,	use_dec,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_Width			= 26;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_FCUN_EXTRACTARCHIVES );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_UN_EXTRACTARCHIVES;
		
		gads[1].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_UNP_ExtractArchives,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_FCUN_ASKBEFOREEXTRACTING );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_FC_UN_ASKBEFOREEXTRACTING;
		
		gads[2].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_UNP_AskBeforeExtracting,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			GA_Disabled,	use_arc,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_FCUN_ASKBEFOREDELETINGEXT );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_FC_UN_ASKBEFOREDELETINGEXT;
		
		gads[3].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			GA_Disabled,	use_arc,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_FCUN_DECRUNCHEXECUTABLES );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_FC_UN_DECRUNCHEXECUTABLES;
		
		gads[4].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[3].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_UNP_DecrunchExecutables,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = GCS( MSG_FCUN_DECRUNCHDATAFILES );
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_FC_UN_DECRUNCHDATAFILES;
		
		gads[5].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[4].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_UNP_DecrunchDataFiles,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[6].xg_GadgetText = GCS( MSG_FCUN_ASKBEFOREDECRUNCHING );
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_FC_UN_ASKBEFOREDECRUNCHING;
		
		gads[6].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[5].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_UNP_AskBeforeDecrunching,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			GA_Disabled,	use_dec,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[7].xg_GadgetText = GCS( MSG_FCUN_ASKBEFOREDELETINGDEC );
		ng.ng_GadgetID		= gads[7].xg_GadgetID = GTID_FC_UN_ASKBEFOREDELETINGDEC;
		
		gads[7].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[6].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_UNP_AskBeforeDeletingDecFiles,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			GA_Disabled,	use_dec,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[8].xg_GadgetText = GCS( MSG_FCUN_DECRUNCHENCRYPTED );
		ng.ng_GadgetID		= gads[8].xg_GadgetID = GTID_FC_UN_DECRUNCHENCRYPTED;
		
		gads[8].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[7].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_UNP_DecrunchEncryptedFiles,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			GA_Disabled,	use_dec,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[9].xg_GadgetText = GCS( MSG_FCUN_CHECKDMSBOOTBLOCK );
		ng.ng_GadgetID		= gads[9].xg_GadgetID = GTID_FC_UN_CHECKDMSBOOTBLOCK;
		
		gads[9].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[8].xg_Gad, &ng,
			GTCB_Checked, XSettings->xs_FC_UNP_CheckDMSBootblock,
			GTCB_Scaled,	TRUE,
			GT_Underscore, '_',
			GA_Disabled,	use_arc,
			TAG_END );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[10].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[10].xg_GadgetID =  GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[9].xg_Gad,
			BUT_Text,		gads[10].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[11].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[11].xg_GadgetID = GTID_CANCEL,
			GA_Left,		ng.ng_LeftEdge + cyclewidth - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[10].xg_Gad,
			BUT_Text,		gads[11].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = ng.ng_LeftEdge + cyclewidth + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCUN_SETTINGS ), ww, wh, glist, gads, 11 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCUN_BAR_UNPACKING ) );
			xBarTxt( win, gads[11].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleFCUnpackingSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				XSettings->xs_FC_UNP_WhenDecrunchingUse = xCycleActive( gads[0].xg_Gad, win );
				XSettings->xs_FC_UNP_ExtractArchives = xIsGadSelected( gads[1].xg_Gad );
				XSettings->xs_FC_UNP_AskBeforeExtracting = xIsGadSelected( gads[2].xg_Gad );
				XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles = xIsGadSelected( gads[3].xg_Gad );
				XSettings->xs_FC_UNP_DecrunchExecutables = xIsGadSelected( gads[4].xg_Gad );
				XSettings->xs_FC_UNP_DecrunchDataFiles = xIsGadSelected( gads[5].xg_Gad );
				XSettings->xs_FC_UNP_AskBeforeDecrunching = xIsGadSelected( gads[6].xg_Gad );
				XSettings->xs_FC_UNP_AskBeforeDeletingDecFiles = xIsGadSelected( gads[7].xg_Gad );
				XSettings->xs_FC_UNP_DecrunchEncryptedFiles = xIsGadSelected( gads[8].xg_Gad );
				XSettings->xs_FC_UNP_CheckDMSBootblock = xIsGadSelected( gads[9].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[11].xg_Gad );
		DisposeObject( gads[10].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCUnpackingSettings( struct Window *win, struct xGadget *gads )
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
							xGuideDisplay( AGH_STGS_FCHK_UNPACKING );
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
									
									case GTID_FC_UN_WHENDECRUNCHINGUSE:
										xRefreshCycleGadget( win, tmpgad->xg_Gad, qualifier, 3 );
										break;
									
									case GTID_FC_UN_EXTRACTARCHIVES:
									case GTID_FC_UN_ASKBEFOREEXTRACTING:
									case GTID_FC_UN_ASKBEFOREDELETINGEXT:
									case GTID_FC_UN_DECRUNCHEXECUTABLES:
									case GTID_FC_UN_DECRUNCHDATAFILES:
									case GTID_FC_UN_ASKBEFOREDECRUNCHING:
									case GTID_FC_UN_ASKBEFOREDELETINGDEC:
									case GTID_FC_UN_DECRUNCHENCRYPTED:
									case GTID_FC_UN_CHECKDMSBOOTBLOCK:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
												TAG_END );
											
											code = tmpgad->xg_GadgetID;
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
						
						case GTID_FC_UN_EXTRACTARCHIVES:
							if( xIsGadSelected( gads[1].xg_Gad ) )
								{
								GT_SetGadgetAttrsA( gads[2].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[3].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[9].xg_Gad, win, NULL, DisabledFalseTags );
								}
							else
								{
								GT_SetGadgetAttrsA( gads[2].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[3].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[9].xg_Gad, win, NULL, DisabledTrueTags );
								}
							break;
						
						case GTID_FC_UN_DECRUNCHEXECUTABLES:
						case GTID_FC_UN_DECRUNCHDATAFILES:
							if( xIsGadSelected( gads[4].xg_Gad ) || xIsGadSelected( gads[5].xg_Gad ) )
								{
								GT_SetGadgetAttrsA( gads[11].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[6].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[7].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[8].xg_Gad, win, NULL, DisabledFalseTags );
								}
							else
								{
								GT_SetGadgetAttrsA( gads[11].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[6].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[7].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[8].xg_Gad, win, NULL, DisabledTrueTags );
								}
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
