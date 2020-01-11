/*
**	$VER: XtruderCheckWindow.c (27.7.96)
**
**	Xtruder file check window functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct Hook			CheckHistoryRenderHook;


#define		SPACE_BETWEEN_GADS				4

UWORD xCalc_ThreeGadgetsWidth( VOID )
{
	UWORD	max = 15 + xMaxStringLength( TextFont, MSG_CW_GAD_HISTORY, MSG_CW_GAD_STOP );
	
	
	return( (UWORD)(max * 3 + 2 * SPACE_BETWEEN_GADS) );
}

UWORD xCalc_MemoryBoxWidth( VOID )
{
	UBYTE	strbuf[ 150 ];
	
	
	xSPrintF( strbuf, GCS( MSG_CW_MEMORYINFO1 ), 888888888, 888888888 );
	
	return( (UWORD)(xTextLength( TextFont, strbuf ) + 10) );
}

UWORD xCalc_MemoryBoxHeight( VOID )
{
	return( (UWORD)((2 * TxtFontYSize) + 4) );
}

UWORD xCalc_CheckInfoBoxWidth( VOID )
{
	UBYTE	strbuf[ 150 ];
	
	
	xSPrintF( strbuf, GCS( MSG_CW_CHECKINFO1 ), 888888, 888888, 888888 );
		
	return( (UWORD)(xTextLength( TextFont, strbuf ) + 10) );
}

UWORD xCalc_CheckInfoBoxHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

UWORD xCalc_WarningsLVHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

UWORD xCalc_MinWindowWidth( VOID )
{
	return( (UWORD)(__builtin_max(
              __builtin_max( xCalc_ThreeGadgetsWidth(), XSettings->xs_FC_CHW_MemoryInformation ? xCalc_MemoryBoxWidth() : 0 ),
              XSettings->xs_FC_CHW_CheckInformation ? xCalc_CheckInfoBoxWidth() : 0 ))
		  );
}

UWORD xCalc_MinWindowHeight( VOID )
{
	UWORD	minheight;
	
	
	minheight = ScreenBarHeight + 2 + (TxtFontYSize + 6) + 2 + (TxtFontYSize + 6) + 2 + (TxtFontYSize + 6) + 2 + 10;
	
	if( XSettings->xs_FC_CHW_MemoryInformation )
		minheight += xCalc_MemoryBoxHeight() + 2;
	
	if( XSettings->xs_FC_CHW_CheckInformation )
		minheight += xCalc_CheckInfoBoxHeight() + 2;
	
	if( XSettings->xs_FC_CHW_Warnings )
		minheight += xCalc_WarningsLVHeight() + 2;
	
	if( XSettings->xs_FC_CHW_History )
		minheight += xCalc_WarningsLVHeight() + 2;
	
	return( minheight );
}

VOID xRemoveCheckWindowGadgets( VOID )
{
		/// Remove gadgets from window
	
	Box_Width = MemoryBox_Top = MemoryBox_Height = CheckInfoBox_Top = CheckInfoBox_Height = 0;
	
	if( CheckWindowGadgets )
		{
		if( CheckWindowGadList )
			{
			RemoveGList( CheckWindow, CheckWindowGadList, -1 );
			
			FreeGadgets( CheckWindowGadList );
			CheckWindowGadList = NULL;
			}
		
		DisposeObject( CheckWindowGadgets[ GAD_CW_PAUSE ].xg_Gad );
		DisposeObject( CheckWindowGadgets[ GAD_CW_STOP ].xg_Gad );
		DisposeObject( CheckWindowGadgets[ GAD_CW_HISTORY ].xg_Gad );
		
		FreeVec( CheckWindowGadgets );
		CheckWindowGadgets = NULL;
		}
	
}

ULONG __asm __saveds xCheckHistoryRenderHook( REGA1 struct LVDrawMsg *msg, REGA2 struct HistoryNode *node )
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

BOOL xCreateCheckWindowGadgets( VOID )
{
	struct NewGadget	ng;
	UWORD				warningslv_height = 0;
	UWORD				n,g;
	BOOL				success = TRUE;
	
	
	xRemoveCheckWindowGadgets();
	
	SetAPen( CheckWindow->RPort, 0 );
	RectFill( CheckWindow->RPort, CheckWindow->BorderLeft, CheckWindow->BorderTop, CheckWindow->Width - CheckWindow->BorderRight - 1,  CheckWindow->Height - CheckWindow->BorderBottom - 1 );
	SetAPen( CheckWindow->RPort, 1 );
	
	RefreshWindowFrame( CheckWindow );
	
		/// Calculate something
	
	if( XSettings->xs_FC_CHW_MemoryInformation )
		MemoryBox_Height = xCalc_MemoryBoxHeight();
	
	if( XSettings->xs_FC_CHW_CheckInformation )
		CheckInfoBox_Height = xCalc_CheckInfoBoxHeight();
	
	if( XSettings->xs_FC_CHW_Warnings )
		warningslv_height = xCalc_WarningsLVHeight();
	
	Box_Width = __builtin_max( CheckWindow->Width - 20, xCalc_MinWindowWidth() );
	
	if( CheckWindowGadgets = AllocVec( sizeof( struct xGadget ) * 8, MEMF_CLEAR ) )
		{
		CheckWindowGadgets[0].xg_Gad = CreateContext( &CheckWindowGadList );
		
			/// Path
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		= ScreenBarHeight + 2;
		ng.ng_Width			= Box_Width;
		ng.ng_Height		= (TxtFontYSize + 6);
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &TextFontAttr;
		ng.ng_GadgetText	= CheckWindowGadgets[0].xg_GadgetText = "";
		ng.ng_GadgetID		= CheckWindowGadgets[0].xg_GadgetID = GTID_CWIN_PATH;
		
		CheckWindowGadgets[0].xg_Gad = CreateGadget( TEXT_KIND, CheckWindowGadgets[0].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// File
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= CheckWindowGadgets[1].xg_GadgetText = "";
		ng.ng_GadgetID		= CheckWindowGadgets[1].xg_GadgetID = GTID_CWIN_FILE;
		
		CheckWindowGadgets[1].xg_Gad = CreateGadget( TEXT_KIND, CheckWindowGadgets[0].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Pause
		
		CheckWindowGadgets[2].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CheckWindowGadgets[2].xg_GadgetID = GTID_CWIN_PAUSE,
			GA_Left,		10,
			GA_Top,			ng.ng_TopEdge += ng.ng_Height + 2,
			GA_Width,		n = ((CheckWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CheckWindowGadgets[1].xg_Gad,
			GA_Disabled,	!StillChecking,
			BUT_Text,		CheckWindowGadgets[2].xg_GadgetText = GCS( MSG_CW_GAD_PAUSE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Stop
		
		CheckWindowGadgets[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CheckWindowGadgets[3].xg_GadgetID = GTID_CWIN_STOP,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CheckWindowGadgets[2].xg_Gad,
			GA_Disabled,	!StillChecking,
			BUT_Text,		CheckWindowGadgets[3].xg_GadgetText = GCS( MSG_CW_GAD_STOP ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// History
		
		CheckWindowGadgets[4].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CheckWindowGadgets[4].xg_GadgetID = GTID_CWIN_HISTORY,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS + n + SPACE_BETWEEN_GADS,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		Box_Width - n - SPACE_BETWEEN_GADS - n - SPACE_BETWEEN_GADS,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CheckWindowGadgets[3].xg_Gad,
			GA_Disabled,	StillChecking | IsListEmpty( &XtruderHistory->his_List ),
			BUT_Text,		CheckWindowGadgets[4].xg_GadgetText = GCS( MSG_CW_GAD_HISTORY ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
		ng.ng_TopEdge += (TxtFontYSize + 6) + 2;
		
		if( XSettings->xs_FC_CHW_MemoryInformation )
			{
			MemoryBox_Top = ng.ng_TopEdge;
			
			ng.ng_TopEdge += MemoryBox_Height + 2;
			}
		
		if( XSettings->xs_FC_CHW_CheckInformation )
			{
			if( MemoryBox_Top )
				CheckInfoBox_Top = MemoryBox_Top + MemoryBox_Height + 2;
			else
				CheckInfoBox_Top = ng.ng_TopEdge;
			
			ng.ng_TopEdge += CheckInfoBox_Height + 2;
			}
		
		n = 4;
		
		GAD_CW_WARNINGS = GAD_CW_HISTORY + 1;
		
			/// Check history
		
		if( XSettings->xs_FC_CHW_History )
			{
			CheckHistoryRenderHook.h_Entry = (HOOKFUNC)xCheckHistoryRenderHook;
			
			ng.ng_Height		= CheckWindow->Height - CheckWindow->BorderBottom - 2 - ng.ng_TopEdge - warningslv_height - (XSettings->xs_FC_CHW_Warnings ? 2 : 0);
			ng.ng_Height		= (((ng.ng_Height - 4) / TxtFontYSize) * TxtFontYSize) + 4;
			ng.ng_GadgetText	= CheckWindowGadgets[5].xg_GadgetText = "";
			ng.ng_GadgetID		= CheckWindowGadgets[5].xg_GadgetID = GTID_CWIN_CHECKHISTORY;
			
			CheckWindowGadgets[5].xg_Gad = CreateGadget( LISTVIEW_KIND, CheckWindowGadgets[4].xg_Gad, &ng,
				GTLV_Labels,		&CheckWindowHistoryList.cl_List,
				GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
				GTLV_ReadOnly,		TRUE,
				GTLV_CallBack,		&CheckHistoryRenderHook,
				TAG_END );
			
			HistoryLVVisible = (ng.ng_Height - 4) / TxtFontYSize;
			
			GAD_CW_WARNINGS++;
			
			n++;
			}
		
			/// Warnings
		
		if( XSettings->xs_FC_CHW_Warnings )
			{
			if( XSettings->xs_FC_CHW_History )
				ng.ng_TopEdge += ng.ng_Height + 2;
			
			ng.ng_Height		= warningslv_height;
			ng.ng_GadgetText	= CheckWindowGadgets[ GAD_CW_WARNINGS ].xg_GadgetText = "";
			ng.ng_GadgetID		= CheckWindowGadgets[ GAD_CW_WARNINGS ].xg_GadgetID = GTID_CWIN_WARNINGS;
			
			CheckWindowGadgets[ GAD_CW_WARNINGS ].xg_Gad = CreateGadget( LISTVIEW_KIND, CheckWindowGadgets[ GAD_CW_WARNINGS - 1 ].xg_Gad, &ng,
				GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
				GTLV_ReadOnly,		TRUE,
				TAG_END );
			
			n++;
			}
		
		for( g = 0; g < n; g++ )
			{
			if( !CheckWindowGadgets[g].xg_Gad || !CheckWindowGadgets[g + 1].xg_Gad )
				success = FALSE;
			
			CheckWindowGadgets[g].xg_NextGad = &CheckWindowGadgets[g + 1];
			}
		
		CheckWindowGadgets[g].xg_NextGad = NULL;
		
		if( success )
			{
			AddGList( CheckWindow, CheckWindowGadList, -1, -1, NULL );
			RefreshGList( CheckWindowGadList, CheckWindow, NULL, -1 );
			
			GT_RefreshWindow( CheckWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveCheckWindowGadgets();
	
	return( FALSE );
}

VOID xDrawCheckWindowBoxes( VOID )
{
		/// Memory box
	
	if( XSettings->xs_FC_CHW_MemoryInformation )
		DrawBevelBox( CheckWindow->RPort, 10, MemoryBox_Top, Box_Width, MemoryBox_Height,
			GTBB_Recessed, TRUE,
			GT_VisualInfo, VisualInfo,
			TAG_END );
	
		/// Check information box
	
	if( XSettings->xs_FC_CHW_CheckInformation )
		DrawBevelBox( CheckWindow->RPort, 10, CheckInfoBox_Top, Box_Width, CheckInfoBox_Height,
			GTBB_Recessed, TRUE,
			GT_VisualInfo, VisualInfo,
			TAG_END );			
}

/*
 * Check window uses TextFont!
 */
BOOL xOpenCheckWindow( VOID )
{
	UWORD	minheight = xCalc_MinWindowHeight();
	WORD	maxheight = XSettings->xs_FC_CHW_History ? -1 : xCalc_MinWindowHeight();
	
	
	if( Screen )
		{
		if( CheckWindow = OpenWindowTags( NULL,
			WA_Left,			Coords->xc_CheckWindow[0],
			WA_Top,				Coords->xc_CheckWindow[1],
			WA_Width,			__builtin_max( xCalc_MinWindowWidth() + 20, Coords->xc_CheckWindow[2] ),
			WA_Height,			XSettings->xs_FC_CHW_History ? __builtin_max( minheight, Coords->xc_CheckWindow[3] ) : minheight,
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
			WA_Title,			GCS( MSG_CW_TL_FILECHECK ),
			WA_DragBar,			TRUE,
			WA_CloseGadget,		TRUE,
			WA_Activate,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_RMBTrap,			TRUE,
			WA_MinWidth,		xCalc_MinWindowWidth() + 20,
			WA_MinHeight,		minheight,
			WA_MaxWidth,		-1,
			WA_MaxHeight,		maxheight,
			TAG_END ) )
			{
			StillChecking = TRUE;
			
			NewList( &CheckWindowHistoryList.cl_List );
			CheckWindowHistoryList.cl_Entries = 0;
			
			NewList( &CheckWindowWarningsList );
			
			if( xCreateCheckWindowGadgets() )
				{
				SetFont( CheckWindow->RPort, TextFont );
				
				xDrawCheckWindowBoxes();
				
				return( TRUE );
				}
			}
		
		xCloseCheckWindow();
		}
	
	return( FALSE );
}

VOID xCloseCheckWindow( VOID )
{
	if( CheckWindow )
		{
			/// Remove gadgets
		
		xRemoveCheckWindowGadgets();
		
			/// Store x,y position of window
		
		Coords->xc_CheckWindow[0] = CheckWindow->LeftEdge;
		Coords->xc_CheckWindow[1] = CheckWindow->TopEdge;
		Coords->xc_CheckWindow[2] = CheckWindow->Width;
		Coords->xc_CheckWindow[3] = CheckWindow->Height;
		
			/// Close check window
		
		CloseWindow( CheckWindow );
		CheckWindow = NULL;
		
			/// Free lists
		
		xFreeHistoryNodes( &CheckWindowHistoryList.cl_List );
		xFreeNameNodes( &CheckWindowWarningsList );
		}
}
