/*
**	$VER: Xt_ReportsMainWindow.c (8.8.96)
**
**	Reports: Main reports interface
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		SPACE_BETWEEN_GADS				4


/*
 * Calculates the minimum width for the four gadgets 'Create', 'Update', 'Check' and 'Delete'.
 */
UWORD xCalc_MRITopGadgetsWidth( VOID )
{
	UWORD	max = 15 + xMaxStringLength( TextFont, MSG_MRI_GAD_CREATE, MSG_MRI_GAD_DELETE );
	
	
	return( (UWORD)(max * 4 + 3 * SPACE_BETWEEN_GADS) );
}

/*
 * Calculates the minimum height of the main report listview
 */
UWORD xCalc_MRIListViewHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

/*
 * Calculates the minimum width of the main report window
 */
UWORD xCalc_MRIMinWindowWidth( VOID )
{
	return( (UWORD)(20 + xCalc_MRITopGadgetsWidth()) );
}

/*
 * Calculates the minimum height of the main report window
 */
UWORD xCalc_MRIMinWindowHeight( VOID )
{
	UWORD	minheight;
	
	
	minheight = ScreenBarHeight + 2 + (TxtFontYSize + 6) + 2 + (TxtFontYSize + 6) + 2 + 10;
	minheight += xCalc_MRIListViewHeight() + 2;
	
	return( minheight );
}

VOID xRemoveMRIWindowGadgets( VOID )
{
		/// Remove gadgets from window
	
	MRIBoxWidth = 0;
	
	if( MRIWindowGadgets )
		{
		if( MRIWindowGadList )
			{
			RemoveGList( MRIWindow, MRIWindowGadList, -1 );
			
			FreeGadgets( MRIWindowGadList );
			MRIWindowGadList = NULL;
			}
		
		DisposeObject( MRIWindowGadgets[ 5 ].xg_Gad );			/// Get reports path
		DisposeObject( MRIWindowGadgets[ 3 ].xg_Gad );			/// Delete
		DisposeObject( MRIWindowGadgets[ 2 ].xg_Gad );			/// Check
		DisposeObject( MRIWindowGadgets[ 1 ].xg_Gad );			/// Update
		DisposeObject( MRIWindowGadgets[ 0 ].xg_Gad );			/// Create
		
		FreeVec( MRIWindowGadgets );
		MRIWindowGadgets = NULL;
		}
	
}

BOOL xCreateMRIWindowGadgets( VOID )
{
	struct NewGadget	ng;
	UWORD				n,g;
	BOOL				success = TRUE;
	
	
		/// Remove previous gadgets
	
	xRemoveMRIWindowGadgets();
	
		/// Refresh window so it's as neat as a baby's bottom
	
	SetAPen( MRIWindow->RPort, 0 );
	RectFill( MRIWindow->RPort, MRIWindow->BorderLeft, MRIWindow->BorderTop, MRIWindow->Width - MRIWindow->BorderRight - 1,  MRIWindow->Height - MRIWindow->BorderBottom - 1 );
	SetAPen( MRIWindow->RPort, 1 );
	
	RefreshWindowFrame( MRIWindow );
	
		/// Calculate something
	
	MRIBoxWidth = __builtin_max( MRIWindow->Width - 20, xCalc_MRIMinWindowWidth() );
	
		/// Start creating those gadgets
	
	if( MRIWindowGadgets = AllocVec( sizeof( struct xGadget ) * 7, MEMF_CLEAR ) )
		{
		MRIWindowGadgets[0].xg_Gad = CreateContext( &MRIWindowGadList );
		
			/// Create
		
		MRIWindowGadgets[0].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			MRIWindowGadgets[0].xg_GadgetID = GTID_MRI_CREATE,
			GA_Left,		10,
			GA_Top,			ScreenBarHeight + 2,
			GA_Width,		n = ((MRIWindow->Width - 20) - 3 * SPACE_BETWEEN_GADS) / 4,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	MRIWindowGadgets[0].xg_Gad,
			BUT_Text,		MRIWindowGadgets[0].xg_GadgetText = GCS( MSG_MRI_GAD_CREATE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Update
		
		MRIWindowGadgets[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			MRIWindowGadgets[1].xg_GadgetID = GTID_MRI_UPDATE,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			ScreenBarHeight + 2,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	MRIWindowGadgets[0].xg_Gad,
			GA_Disabled,	TRUE,
			BUT_Text,		MRIWindowGadgets[1].xg_GadgetText = GCS( MSG_MRI_GAD_UPDATE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check
		
		MRIWindowGadgets[2].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			MRIWindowGadgets[2].xg_GadgetID = GTID_MRI_CHECK,
			GA_Left,		10 + (n + SPACE_BETWEEN_GADS) * 2,
			GA_Top,			ScreenBarHeight + 2,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	MRIWindowGadgets[1].xg_Gad,
			GA_Disabled,	TRUE,
			BUT_Text,		MRIWindowGadgets[2].xg_GadgetText = GCS( MSG_MRI_GAD_CHECK ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Delete
		
		MRIWindowGadgets[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			MRIWindowGadgets[3].xg_GadgetID = GTID_MRI_DELETE,
			GA_Left,		10 + (n + SPACE_BETWEEN_GADS) * 3,
			GA_Top,			ScreenBarHeight + 2,
			GA_Width,		MRIBoxWidth - (n + SPACE_BETWEEN_GADS) * 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	MRIWindowGadgets[2].xg_Gad,
			GA_Disabled,	TRUE,
			BUT_Text,		MRIWindowGadgets[3].xg_GadgetText = GCS( MSG_MRI_GAD_DELETE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Reports path
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_TextAttr		= &TextFontAttr;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		= ScreenBarHeight + 2 + TxtFontYSize + 6 + 2;
		ng.ng_Width			= MRIBoxWidth - GETGADWIDTH;
		ng.ng_Height		= TxtFontYSize + 6;
		ng.ng_GadgetText	= MRIWindowGadgets[4].xg_GadgetText = "";
		ng.ng_GadgetID		= MRIWindowGadgets[4].xg_GadgetID = GTID_MRI_REPORTSPATH;
		
		MRIWindowGadgets[4].xg_Gad = CreateGadget( STRING_KIND, MRIWindowGadgets[3].xg_Gad, &ng,
			GTST_String,	XSettings->xs_REP_ReportsPath,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
        	GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Get reports path
		
		MRIWindowGadgets[5].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			MRIWindowGadgets[5].xg_GadgetID = GTID_MRI_GETREPORTSPATH,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		GETGADWIDTH,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	MRIWindowGadgets[4].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Reports listview
		
		ng.ng_TopEdge		+= TxtFontYSize + 6 + 2;
		ng.ng_Width			= MRIBoxWidth;
		ng.ng_Height		= MRIWindow->Height - MRIWindow->BorderBottom - 2 - ng.ng_TopEdge;
		ng.ng_Height		= (((ng.ng_Height - 4) / TxtFontYSize) * TxtFontYSize) + 4;
		ng.ng_GadgetText	= MRIWindowGadgets[6].xg_GadgetText = "";
		ng.ng_GadgetID		= MRIWindowGadgets[6].xg_GadgetID = GTID_MRI_REPORTSLISTVIEW;
		
		MRIWindowGadgets[6].xg_Gad = CreateGadget( LISTVIEW_KIND, MRIWindowGadgets[5].xg_Gad, &ng,
			GTLV_Labels,		&MRIReportsList,
			GTLV_ShowSelected,	NULL,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			TAG_END );
		
		MRIReportsListViewVisible = (ng.ng_Height - 4) / TxtFontYSize;
		
			/// Check if all gadgets were created
		
		for( g = 0; g < 6; g++ )
			{
			if( !MRIWindowGadgets[ g ].xg_Gad || !MRIWindowGadgets[ g + 1 ].xg_Gad )
				success = FALSE;
			
			MRIWindowGadgets[ g ].xg_NextGad = &MRIWindowGadgets[ g + 1 ];
			}
		
		MRIWindowGadgets[ g ].xg_NextGad = NULL;
		
		if( success )
			{
			AddGList( MRIWindow, MRIWindowGadList, -1, -1, NULL );
			RefreshGList( MRIWindowGadList, MRIWindow, NULL, -1 );
			
			GT_RefreshWindow( MRIWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveMRIWindowGadgets();
	
	return( FALSE );
}

/*
 * Report window uses TextFont!
 */
VOID xOpenMRIWindow( VOID )
{
	UWORD	minheight = xCalc_MRIMinWindowHeight();
	
	
	if( Screen )
		{
		xDisableWindow( Window );
		
		NewList( &MRIReportsList );
		
		if( MRIWindow = OpenWindowTags( NULL,
			WA_Left,			Coords->xc_ReportsMainWindow[0],
			WA_Top,				Coords->xc_ReportsMainWindow[1],
			WA_Width,			__builtin_max( xCalc_MRIMinWindowWidth() + 20, Coords->xc_ReportsMainWindow[2] ),
			WA_Height,			__builtin_max( minheight, Coords->xc_ReportsMainWindow[3] ),
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
			WA_Title,			GCS( MSG_MRI_TL_REPORTS ),
			WA_Activate,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_MinWidth,		xCalc_MRIMinWindowWidth() + 20,
			WA_MinHeight,		minheight,
			WA_MaxWidth,		-1,
			WA_MaxHeight,		-1,
			TAG_END ) )
			{
			if( xCreateMRIWindowGadgets() )
				{
				SetFont( MRIWindow->RPort, TextFont );
				
					/// Read list
				
				xRereadReportsList();
				
				xHandleMRIWindowIDCMP();
				
					/// Remember settings
				
				strcpy( XSettings->xs_REP_ReportsPath, GETSTRING( MRIWindowGadgets[ 4 ].xg_Gad ) );
				
					/// Flush reports list
				
				xFreeReportNodes();
				}
			
			xCloseMRIWindow();
			}
		
		xEnableWindow( Window );
		}
}

VOID xCloseMRIWindow( VOID )
{
	xRemoveMRIWindowGadgets();
	
	if( MRIWindow )
		{
			/// Store position of window
		
		Coords->xc_ReportsMainWindow[0] = MRIWindow->LeftEdge;
		Coords->xc_ReportsMainWindow[1] = MRIWindow->TopEdge;
		Coords->xc_ReportsMainWindow[2] = MRIWindow->Width;
		Coords->xc_ReportsMainWindow[3] = MRIWindow->Height;
		
			/// Close check window
		
		CloseWindow( MRIWindow );
		MRIWindow = NULL;
		}
}

VOID xHandleMRIWindowIDCMP( VOID )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					code,altcode,qualifier;
	LONG					curselected = -1,oldsel = -1;
	struct NewLVPos			nlv = {0,0};
	ULONG					seconds, micros,sec1 = 0,mic1 = 0,sec2,mic2;
	UWORD					entries;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ];
	struct ReportNode *		node;
	
	
	while( 1 )
		{
		WaitPort( MRIWindow->UserPort );
		
		while( msg = GT_GetIMsg( MRIWindow->UserPort ) )
			{
			class = msg->Class;
			code = altcode = msg->Code;
			qualifier = msg->Qualifier;
			ia = msg->IAddress;
			seconds = msg->Seconds;
			micros = msg->Micros;
			
			if( class == IDCMP_GADGETUP )
				code = ((struct Gadget *)msg->IAddress)->GadgetID;
			
			GT_ReplyIMsg( msg );
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( MRIWindow );
					GT_EndRefresh( MRIWindow, TRUE );
					break;
				
				case IDCMP_NEWSIZE:
						/// Remember settings
					
					strcpy( XSettings->xs_REP_ReportsPath, GETSTRING( MRIWindowGadgets[ 4 ].xg_Gad ) );
					
					if( !xCreateMRIWindowGadgets() )
						return;
					
					curselected = -1;
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case CURSORUP:
						case CURSORDOWN:
							if( entries = xCountListEntries( &MRIReportsList ) )
								{
								xNewListPos( &nlv, qualifier, code, curselected, xListViewTop( MRIWindowGadgets[ 6 ].xg_Gad, MRIWindow ), MRIReportsListViewVisible, entries );
								
								GT_SetGadgetAttrs( MRIWindowGadgets[ 6 ].xg_Gad, MRIWindow, NULL,
									GTLV_Selected,	curselected = nlv.nl_NewSelected,
									GTLV_Top,		nlv.nl_NewTop,
									TAG_END );
								
								xUpdateMRIGadgets( curselected );
								}
							break;
						
						case KEY_RETURN:
							if( node = (struct ReportNode *)xFindCurrentSelectedNode( &MRIReportsList, curselected ) )
								xStartCheckReport( node->rn_Filename );
							break;
						
						case KEY_ESCAPE:
							return;
							break;
						
						case KEY_TAB:
							ActivateGadget( MRIWindowGadgets[ 4 ].xg_Gad, MRIWindow, NULL );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( MRIWindowGadgets, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_MRI_CREATE:
									case GTID_MRI_UPDATE:
									case GTID_MRI_CHECK:
									case GTID_MRI_DELETE:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											xPressButton( MRIWindow, tmpgad->xg_Gad );
											
											code = tmpgad->xg_GadgetID;
											}
										break;
									}
								}
							else
								break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_MRI_CREATE:
							strbuf[0] = 0;
							
							if( rtGetString( strbuf, 32, GCS( MSG_MRI_TL_CREATEREPORT ), NULL,
								RT_Window,			MRIWindow,
								RT_Underscore,		'_',
								RT_TextAttr,		&RequesterFontAttr,
								RTGS_TextFmt,		GCS( MSG_MRI_BODY_CREATEREPORT ),
								RTGS_GadFmt,		GCS( MSG_GF_OKAYCANCEL ),
								TAG_END ) )
								{
								xStartCreateReport( strbuf, FALSE );
								
								xRereadReportsList();
								}
							break;
						
						case GTID_MRI_UPDATE:
							if( node = (struct ReportNode *)xFindCurrentSelectedNode( &MRIReportsList, curselected ) )
								{
								xStartCreateReport( node->rn_Filename, TRUE );
								
								xRereadReportsList();
								}
							break;
						
						case GTID_MRI_CHECK:
							if( node = (struct ReportNode *)xFindCurrentSelectedNode( &MRIReportsList, curselected ) )
								xStartCheckReport( node->rn_Filename );
							break;
						
						case GTID_MRI_DELETE:
							if( node = (struct ReportNode *)xFindCurrentSelectedNode( &MRIReportsList, curselected ) )
								{
								if( xReqMsg( MRIWindow, GCS( MSG_REQ_INFO ), GCS( MSG_MRI_DELETEREPORT ), GCS( MSG_GF_YESNO ), node->rn_Filename ) )
									{
										/// Construct filename
									
									strcpy( strbuf, GETSTRING( MRIWindowGadgets[ 4 ].xg_Gad ) );
									AddPart( strbuf, node->rn_Filename, DEFAULTBUFFERLEN );
									
										/// Commence delete
									
									DeleteFile( strbuf );
									
									xRereadReportsList();
									
									if( (curselected > 0) && (curselected == xCountListEntries( &MRIReportsList )) )
										{
										GT_SetGadgetAttrs( MRIWindowGadgets[ 6 ].xg_Gad, MRIWindow, NULL,
											GTLV_Selected,	--curselected,
											TAG_END );
										}
									}
								}
							break;
						
						case GTID_MRI_REPORTSPATH:
							strcpy( XSettings->xs_REP_ReportsPath, GETSTRING( MRIWindowGadgets[ 4 ].xg_Gad ) );
							
							if( altcode != HOOK_CURSORDOWN )
								{
								xRereadReportsList();
								
								break;
								}
						
						case GTID_MRI_GETREPORTSPATH:
							xDisableWindow( MRIWindow );
							
								/// Select a directory
							
							if( xSelectPath( MRIWindow,
								SELFP_DestDir,	XSettings->xs_REP_ReportsPath,
								SELFP_Gadget,	MRIWindowGadgets[ 4 ].xg_Gad,
								TAG_END ) )
								{
								xSetGadgetString( MRIWindow, MRIWindowGadgets[ 4 ].xg_Gad, XSettings->xs_REP_ReportsPath );
								
								xRereadReportsList();
								}
							
							xEnableWindow( MRIWindow );
							break;
						
						case GTID_MRI_REPORTSLISTVIEW:
							curselected = altcode;
							
								/// Save the old time
							
							sec2 = sec1;
							mic2 = mic1;
							
								/// Get the new time
							
							sec1 = seconds;
							mic1 = micros;
							
							if( curselected == oldsel && DoubleClick( sec2, mic2, sec1, mic1 ) )
								{
								if( node = (struct ReportNode *)xFindCurrentSelectedNode( &MRIReportsList, curselected ) )
									xStartCheckReport( node->rn_Filename );
								
									/// Reset the values
								
								sec1 = 0;
								mic1 = 0;
								}
							
							oldsel = curselected;
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					return;
					break;
				}
			}
		}
}

VOID xUpdateMRIGadgets( LONG curselected )
{
	UWORD	n;
	
	
	for( n = 1; n < 4; n++ )
		{
		if( curselected >= 0 )
			xUnGhostObject( MRIWindow, MRIWindowGadgets[ n ].xg_Gad );
		else
			xGhostObject( MRIWindow, MRIWindowGadgets[ n ].xg_Gad );
		}
}
