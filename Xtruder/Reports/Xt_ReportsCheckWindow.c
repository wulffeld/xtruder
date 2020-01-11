/*
**	$VER: Xt_ReportsCheckWindow.c (8.8.96)
**
**	Reports: Check interface
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct Hook			CHIHistoryRenderHook;


#define		SPACE_BETWEEN_GADS				4

UWORD xCalc_CHIThreeGadgetsWidth( VOID )
{
	UWORD	max = 15 + xMaxStringLength( TextFont, MSG_CHI_GAD_PAUSE, MSG_CHI_GAD_SAVEHISTORY );
	
	
	return( (UWORD)(max * 3 + 2 * SPACE_BETWEEN_GADS) );
}

UWORD xCalc_CHICheckInfoBoxWidth( VOID )
{
	UBYTE	strbuf[ 150 ];
	
	
	xSPrintF( strbuf, GCS( MSG_CHI_CHECKINFO ), 888888, 888888, 888888 );
		
	return( (UWORD)(xTextLength( TextFont, strbuf ) + 10) );
}

UWORD xCalc_CHICheckInfoBoxHeight( VOID )
{
	return( (UWORD)(TxtFontYSize + 4) );
}

UWORD xCalc_CHIHistoryLVHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

UWORD xCalc_CHIMinWindowWidth( VOID )
{
	return( (UWORD)(__builtin_max( xCalc_CHIThreeGadgetsWidth(), xCalc_CheckInfoBoxWidth() ) ) );
}

UWORD xCalc_CHIMinWindowHeight( VOID )
{
	UWORD	minheight;
	
	
	minheight = ScreenBarHeight + 2 + ((TxtFontYSize + 6) + 2) * 3 + 10;
	
	minheight += xCalc_CHICheckInfoBoxHeight() + 2;
	
	minheight += xCalc_CHIHistoryLVHeight() + 2;
	
	return( minheight );
}

VOID xRemoveCHIWindowGadgets( VOID )
{
		/// Remove gadgets from window
	
	CHIBoxWidth = CHICheckInfoBox_Top = CHICheckInfoBox_Height = 0;
	
	if( CHIWindowGadgets )
		{
		if( CHIWindowGadList )
			{
			RemoveGList( CHIWindow, CHIWindowGadList, -1 );
			
			FreeGadgets( CHIWindowGadList );
			CHIWindowGadList = NULL;
			}
		
		DisposeObject( CHIWindowGadgets[ 4 ].xg_Gad );		/// Save history
		DisposeObject( CHIWindowGadgets[ 3 ].xg_Gad );		/// Pause
		DisposeObject( CHIWindowGadgets[ 2 ].xg_Gad );		/// Stop
		
		FreeVec( CHIWindowGadgets );
		CHIWindowGadgets = NULL;
		}
}

ULONG __asm __saveds xCHIHistoryRenderHook( REGA1 struct LVDrawMsg *msg, REGA2 struct HistoryNode *node )
{
	struct RastPort *	rp;
	struct TextExtent	extent;
	UBYTE				state;
	ULONG				fit;
	WORD				x,y;
	WORD				slack;
	ULONG				apen = node->hn_Color;
	ULONG				bpen;
	UWORD *				pens;
	STRPTR				name = node->hn_Node.ln_Name;
	
	
	if( msg->lvdm_MethodID != LV_DRAW )
		return( LVCB_UNKNOWN );
	
	rp    = msg->lvdm_RastPort;
	state = msg->lvdm_State;
	pens  = msg->lvdm_DrawInfo->dri_Pens;
	
	bpen = pens[FILLPEN];
	
	if( (state == LVR_NORMAL) || (state == LVR_NORMALDISABLED) )
		bpen = pens[BACKGROUNDPEN];
	
	if( node->hn_Color == 4 )
		{
		apen = 2;
		bpen = 3;
		}
	
	fit = TextFit( rp, name, strlen( name ), &extent, NULL, 1,
					msg->lvdm_Bounds.MaxX - msg->lvdm_Bounds.MinX - 3,
					msg->lvdm_Bounds.MaxY - msg->lvdm_Bounds.MinY + 1 );
	
	slack = (msg->lvdm_Bounds.MaxY - msg->lvdm_Bounds.MinY) - (extent.te_Extent.MaxY - extent.te_Extent.MinY);
	
	x = msg->lvdm_Bounds.MinX - extent.te_Extent.MinX + 2;
	y = msg->lvdm_Bounds.MinY - extent.te_Extent.MinY + ((slack+1) / 2);
	
	extent.te_Extent.MinX += x;
	extent.te_Extent.MaxX += x;
	extent.te_Extent.MinY += y;
	extent.te_Extent.MaxY += y;
	
	SetABPenDrMd( rp, apen, bpen, JAM2 );
	
	Move( rp, x, y );
	Text( rp, name, fit );
	
	SetAPen( rp, bpen );
	xFillOldExtent( rp, &msg->lvdm_Bounds, &extent.te_Extent );
	
	if( (state == LVR_NORMALDISABLED) || (state == LVR_SELECTEDDISABLED) )
		xGhost( rp, pens[BLOCKPEN], msg->lvdm_Bounds.MinX, msg->lvdm_Bounds.MinY, msg->lvdm_Bounds.MaxX, msg->lvdm_Bounds.MaxY );
	
	return( LVCB_OK );
}

BOOL xCreateCHIWindowGadgets( VOID )
{
	struct NewGadget	ng;
	UWORD				n,g;
	BOOL				success = TRUE;
	
	
	xRemoveCHIWindowGadgets();
	
	SetAPen( CHIWindow->RPort, 0 );
	RectFill( CHIWindow->RPort, CHIWindow->BorderLeft, CHIWindow->BorderTop, CHIWindow->Width - CHIWindow->BorderRight - 1,  CHIWindow->Height - CHIWindow->BorderBottom - 1 );
	SetAPen( CHIWindow->RPort, 1 );
	
	RefreshWindowFrame( CHIWindow );
	
		/// Calculate something
	
	CHICheckInfoBox_Height = xCalc_CHICheckInfoBoxHeight();
	CHIBoxWidth = __builtin_max( CHIWindow->Width - 20, xCalc_CHIMinWindowWidth() );
	
	if( CHIWindowGadgets = AllocVec( sizeof( struct xGadget ) * 8, MEMF_CLEAR ) )
		{
		CHIWindowGadgets[0].xg_Gad = CreateContext( &CHIWindowGadList );
		
			/// Path
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		= ScreenBarHeight + 2;
		ng.ng_Width			= CHIBoxWidth;
		ng.ng_Height		= (TxtFontYSize + 6);
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &TextFontAttr;
		ng.ng_GadgetText	= CHIWindowGadgets[0].xg_GadgetText = "";
		ng.ng_GadgetID		= CHIWindowGadgets[0].xg_GadgetID = GTID_CHI_PATH;
		
		CHIWindowGadgets[0].xg_Gad = CreateGadget( TEXT_KIND, CHIWindowGadgets[0].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// File
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= CHIWindowGadgets[1].xg_GadgetText = "";
		ng.ng_GadgetID		= CHIWindowGadgets[1].xg_GadgetID = GTID_CHI_FILE;
		
		CHIWindowGadgets[1].xg_Gad = CreateGadget( TEXT_KIND, CHIWindowGadgets[0].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Pause
		
		CHIWindowGadgets[2].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CHIWindowGadgets[2].xg_GadgetID = GTID_CHI_PAUSE,
			GA_Left,		10,
			GA_Top,			ng.ng_TopEdge += ng.ng_Height + 2,
			GA_Width,		n = ((CHIWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CHIWindowGadgets[1].xg_Gad,
			GA_Disabled,	!CHIStillChecking,
			BUT_Text,		CHIWindowGadgets[2].xg_GadgetText = GCS( MSG_CHI_GAD_PAUSE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Stop
		
		CHIWindowGadgets[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CHIWindowGadgets[3].xg_GadgetID = GTID_CHI_STOP,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CHIWindowGadgets[2].xg_Gad,
			GA_Disabled,	!CHIStillChecking,
			BUT_Text,		CHIWindowGadgets[3].xg_GadgetText = GCS( MSG_CHI_GAD_STOP ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Save history
		
		CHIWindowGadgets[4].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CHIWindowGadgets[4].xg_GadgetID = GTID_CHI_SAVEHISTORY,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS + n + SPACE_BETWEEN_GADS,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		CHIBoxWidth - n - SPACE_BETWEEN_GADS - n - SPACE_BETWEEN_GADS,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CHIWindowGadgets[3].xg_Gad,
			GA_Disabled,	TRUE,
			BUT_Text,		CHIWindowGadgets[4].xg_GadgetText = GCS( MSG_CHI_GAD_SAVEHISTORY ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
		CHICheckInfoBox_Top = ng.ng_TopEdge += (TxtFontYSize + 6) + 2;
		
		ng.ng_TopEdge += CHICheckInfoBox_Height + 2;
		
			/// Report check history
		
		CHIHistoryRenderHook.h_Entry = (HOOKFUNC)xCHIHistoryRenderHook;
		
		ng.ng_Height		= CHIWindow->Height - CHIWindow->BorderBottom - 2 - ng.ng_TopEdge;
		ng.ng_Height		= (((ng.ng_Height - 4) / TxtFontYSize) * TxtFontYSize) + 4;
		ng.ng_GadgetText	= CHIWindowGadgets[5].xg_GadgetText = "";
		ng.ng_GadgetID		= CHIWindowGadgets[5].xg_GadgetID = GTID_CHI_LISTVIEW;
		
		CHIWindowGadgets[5].xg_Gad = CreateGadget( LISTVIEW_KIND, CHIWindowGadgets[4].xg_Gad, &ng,
			GTLV_Labels,		&CHIHistoryList.cl_List,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			GTLV_ReadOnly,		TRUE,
			GTLV_CallBack,		&CHIHistoryRenderHook,
			TAG_END );
		
		CHIHistoryListViewVisible = (ng.ng_Height - 4) / TxtFontYSize;
		
		for( g = 0; g < 5; g++ )
			{
			if( !CHIWindowGadgets[g].xg_Gad || !CHIWindowGadgets[g + 1].xg_Gad )
				success = FALSE;
			
			CHIWindowGadgets[g].xg_NextGad = &CHIWindowGadgets[g + 1];
			}
		
		CHIWindowGadgets[g].xg_NextGad = NULL;
		
		if( success )
			{
			AddGList( CHIWindow, CHIWindowGadList, -1, -1, NULL );
			RefreshGList( CHIWindowGadList, CHIWindow, NULL, -1 );
			
			GT_RefreshWindow( CHIWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveCHIWindowGadgets();
	
	return( FALSE );
}

VOID xDrawCHIWindowBoxes( VOID )
{
		/// Report check information box
	
	DrawBevelBox( CHIWindow->RPort, 10, CHICheckInfoBox_Top, CHIBoxWidth, CHICheckInfoBox_Height,
		GTBB_Recessed, TRUE,
		GT_VisualInfo, VisualInfo,
		TAG_END );
}

/*
 * Report check window uses TextFont!
 */
BOOL xOpenCHIWindow( VOID )
{
	UWORD	minheight = xCalc_CHIMinWindowHeight();
	
	
	if( Screen )
		{
		NewList( &CHIHistoryList.cl_List );
		CHIHistoryList.cl_Entries = 0;
		
		if( CHIWindow = OpenWindowTags( NULL,
			WA_Left,			Coords->xc_ReportsCheckWindow[0],
			WA_Top,				Coords->xc_ReportsCheckWindow[1],
			WA_Width,			__builtin_max( xCalc_CHIMinWindowWidth() + 20, Coords->xc_ReportsCheckWindow[2] ),
			WA_Height,			__builtin_max( minheight, Coords->xc_ReportsCheckWindow[3] ),
			WA_IDCMP,			IDCMP_REFRESHWINDOW |
								IDCMP_RAWKEY |
								IDCMP_GADGETUP |
								IDCMP_GADGETDOWN |
								IDCMP_INTUITICKS |
								IDCMP_MOUSEBUTTONS |
								IDCMP_MOUSEMOVE |
								IDCMP_CLOSEWINDOW |
								IDCMP_NEWSIZE,
			WA_PubScreen,		Screen,
			WA_Title,			GCS( MSG_CHI_TL_REPORTCHECKING ),
			WA_DragBar,			TRUE,
			WA_CloseGadget,		TRUE,
			WA_Activate,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_RMBTrap,			TRUE,
			WA_MinWidth,		xCalc_CHIMinWindowWidth() + 20,
			WA_MinHeight,		minheight,
			WA_MaxWidth,		-1,
			WA_MaxHeight,		-1,
			TAG_END ) )
			{
			CHIStillChecking = TRUE;
			
			if( xCreateCHIWindowGadgets() )
				{
				SetFont( CHIWindow->RPort, TextFont );
				
				xDrawCHIWindowBoxes();
				
				return( TRUE );
				}
			}
		
		xCloseCHIWindow();
		}
	
	return( FALSE );
}

VOID xCloseCHIWindow( VOID )
{
	xRemoveCHIWindowGadgets();
	
	if( CHIWindow )
		{
			/// Store x,y position of window
		
		Coords->xc_ReportsCheckWindow[0] = CHIWindow->LeftEdge;
		Coords->xc_ReportsCheckWindow[1] = CHIWindow->TopEdge;
		Coords->xc_ReportsCheckWindow[2] = CHIWindow->Width;
		Coords->xc_ReportsCheckWindow[3] = CHIWindow->Height;
		
			/// Close check window
		
		CloseWindow( CHIWindow );
		CHIWindow = NULL;
		}
	
		/// Free list
	
	xFreeHistoryNodes( &CHIHistoryList.cl_List );
}

VOID xCHI_UpdatePathGadget( STRPTR pathname )
{
	static UBYTE	pathnamebuf[ 250 ];
	
	
	strncpy( pathnamebuf, pathname, 249 );
	
	GT_SetGadgetAttrs( CHIWindowGadgets[ 0 ].xg_Gad, CHIWindow, NULL, GTTX_Text, pathnamebuf, TAG_END );
}

VOID xCHI_UpdateFileGadget( STRPTR filename )
{
	static UBYTE	filenamebuf[ 50 ];
	
	
	strncpy( filenamebuf, filename, 49 );
	
	GT_SetGadgetAttrs( CHIWindowGadgets[ 1 ].xg_Gad, CHIWindow, NULL, GTTX_Text, filenamebuf, TAG_END );
}

VOID xCHI_UpdateCheckInfo( struct CurrentReport *cr )
{
	xWTxt( CHIWindow, 14, CHICheckInfoBox_Top + TextFont->tf_Baseline + 2, 1, GCS( MSG_CHI_CHECKINFO ), cr->cr_FilesChecked, cr->cr_MismatchesFound );
}

UWORD xCheckCHIWindowMsg( struct CurrentReport *cr )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	ULONG					msgclass;
	APTR					ia;
	UWORD					msgcode;
	UWORD					qualifier;
	struct NewLVPos			nlv = {0,0};
	LONG					curvistop;
	
	
	while( msg = GT_GetIMsg( CHIWindow->UserPort ) )
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
				GT_BeginRefresh( CHIWindow );
				GT_EndRefresh( CHIWindow, TRUE );
				break;
			
			case IDCMP_NEWSIZE:
				xCreateCHIWindowGadgets();
				xDrawCHIWindowBoxes();
				xCHI_UpdateCheckInfo( cr );
				break;
			
			case IDCMP_RAWKEY:
				switch( msgcode )
					{
					case CURSORUP:
					case CURSORDOWN:
						curvistop = xListViewTop( CHIWindowGadgets[ 5 ].xg_Gad, CHIWindow );
						
						xNewListPos( &nlv, qualifier, msgcode, msgcode == CURSORUP ? curvistop : curvistop + CHIHistoryListViewVisible - 1, curvistop, CHIHistoryListViewVisible, xCountListEntries( &CHIHistoryList.cl_List ) );
						
						GT_SetGadgetAttrs( CHIWindowGadgets[ 5 ].xg_Gad, CHIWindow, NULL,
							GTLV_Top,		nlv.nl_NewTop,
							TAG_END );
						break;
					
					case KEY_ESCAPE:
						if( cr->cr_Result == CCRES_CHECKEND )
							return( CCRES_CHECKEND );
						break;
					
					default:
						if( tmpgad = xCheckUnderscore( CHIWindowGadgets, msgcode, qualifier, ia ) )
							{
							switch( tmpgad->xg_GadgetID )
								{
								case GTID_CHI_PAUSE:
								case GTID_CHI_STOP:
								case GTID_CHI_SAVEHISTORY:
									if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
										{
										xPressButton( CHIWindow, tmpgad->xg_Gad );
										
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
					case GTID_CHI_PAUSE:
						xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_CW_OPERATIONPAUSED ), GCS( MSG_GF_CONTINUE ) );
						break;
					
					case GTID_CHI_STOP:
						return( CW_STOP );
						break;
					
					case GTID_CHI_SAVEHISTORY:
						xSaveReportCheckHistory();
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

LONG xHandleCHIWindowIDCMP( struct CurrentReport *cr )
{
	UWORD					res = CW_NOTHING;
	
	
	if( CHIWindow )
		{
		if( cr->cr_Result && cr->cr_Result != CCRES_CHECKEND )
			return( cr->cr_Result );
		
		if( cr->cr_Result == CCRES_CHECKEND )
			{
				/// It has finished creating the report
			
			CHIStillChecking = FALSE;
			
				/// Clear gadgets
			
			xCHI_UpdatePathGadget( "" );
			xCHI_UpdateFileGadget( "" );
			
				/// Ghost gadgets
			
			xGhostObject( CHIWindow, CHIWindowGadgets[ 2 ].xg_Gad );
			xGhostObject( CHIWindow, CHIWindowGadgets[ 3 ].xg_Gad );
			
			if( !IsListEmpty( &CHIHistoryList.cl_List ) )
				xUnGhostObject( CHIWindow, CHIWindowGadgets[ 4 ].xg_Gad );
			
				/// Wait for the user to close the window
			
			do
				{
				WaitPort( CHIWindow->UserPort );
				}
			while( xCheckCHIWindowMsg( cr ) != CCRES_CHECKEND );
			}
		else
			res = xCheckCHIWindowMsg( cr );
		}
	
	return( res );
}

VOID xSaveReportCheckHistory( VOID )
{
	struct HistoryNode *	worknode;
	BPTR					file;
	UBYTE					filename[ DEFAULTBUFFERLEN ];
	BOOL					overwrite = TRUE;
	
	
	if( xSelectFile( CHIWindow,
		SELFP_DestFile,	filename,
		TAG_END ) )
		{
		if( xExists( filename ) )
			overwrite = xReqMsg( Window, GCS( MSG_REQ_WARNING ), GCS( MSG_FC_FILEALREADYEXISTS ), GCS( MSG_GF_CONTINUEABORT ), FilePart( filename ) );
		
		if( overwrite )
			{
			if( file = Open( filename, MODE_NEWFILE ) )
				{
				for( worknode = (struct HistoryNode *)((struct List *)&CHIHistoryList.cl_List)->lh_Head;
					 worknode->hn_Node.ln_Succ;
					 worknode = (struct HistoryNode *)worknode->hn_Node.ln_Succ )
					{
					FPrintf( file, "%s\n", worknode->hn_Name );
					}
				
				Close( file );
				}
			}
		}
}
