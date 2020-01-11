/*
**	$VER: XtruderSettingsARexx.c (2.3.96)
**
**	Xtruder settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		ARGTB_NAME							0x00000001
#define		ARGTB_REMOVE						0x00000002
#define		ARGTB_UPDOWN						0x00000004
#define		ARGTB_SORT							0x00000008
#define		ARGTB_SCRIPT						0x00000010
#define		ARGTB_GETSCRIPT						0x00000020

#define		ARGT_NAME							0
#define		ARGT_LISTVIEW						1
#define		ARGT_ADD							2
#define		ARGT_REMOVE							3
#define		ARGT_UP								4
#define		ARGT_DOWN							5
#define		ARGT_SORT							6
#define		ARGT_SCRIPT							7
#define		ARGT_GETSCRIPT						8

#define		ARGT_GADS							11

static UWORD GhostPattern[2] =
{
	0x4444,0x1111,
};

// 12481248
// *** ***
// * *** **
static UWORD BarPattern[2] =
{
	0x7777,0xDDDD,
};

/*
 * Apply a ghosting pattern to a given rectangle in a rastport
 */
VOID xGhost( struct RastPort *rp, UWORD pen, UWORD x0, UWORD y0, UWORD x1, UWORD y1 )
{
	SetABPenDrMd( rp, pen, 0, JAM1 );
	SetAfPt( rp, GhostPattern, 1 );
	RectFill( rp, x0, y0, x1, y1 );
	SetAfPt( rp, NULL, 0 );
}

/*
 * Erase any part of "oldExtent" which is not covered by "newExtent"
 */
VOID xFillOldExtent(struct RastPort *rp, struct Rectangle *oldExtent, struct Rectangle *newExtent )
{
	if( oldExtent->MinX < newExtent->MinX )
		RectFill( rp, oldExtent->MinX, oldExtent->MinY, newExtent->MinX-1, oldExtent->MaxY );
	
	if( oldExtent->MaxX > newExtent->MaxX )
		RectFill( rp, newExtent->MaxX+1, oldExtent->MinY, oldExtent->MaxX, oldExtent->MaxY );
	
	if( oldExtent->MaxY > newExtent->MaxY )
		RectFill( rp, oldExtent->MinX, newExtent->MaxY+1, oldExtent->MaxX, oldExtent->MaxY );
	
	if( oldExtent->MinY < newExtent->MinY )
		RectFill( rp, oldExtent->MinX, oldExtent->MinY, oldExtent->MaxX, newExtent->MinY-1 );
}

/* This function is called whenever an item of the listview needs to be drawn
 * by gadtools. The function must fill every pixel of the area described in
 * the LVDrawMsg structure. This function does the exact same rendering as
 * the built-in rendering function in gadtools, except that it render the
 * normal items using the highlight text pen instead of simply text pen.
 */
ULONG __asm __saveds xRenderHook( REGA1 struct LVDrawMsg *msg, REGA2 struct ARexxNode *node )
{
	struct RastPort *	rp;
	struct TextExtent	extent;
	UBYTE				state;
	ULONG				fit;
	WORD				x,y;
	WORD				slack;
	ULONG				apen;
	ULONG				bpen;
	UWORD *				pens;
	STRPTR				name;
	
	
	if( msg->lvdm_MethodID != LV_DRAW )
		return( LVCB_UNKNOWN );
	
	rp    = msg->lvdm_RastPort;
	state = msg->lvdm_State;
	pens  = msg->lvdm_DrawInfo->dri_Pens;
	
	apen = pens[FILLTEXTPEN];
	bpen = pens[FILLPEN];
	
	if( (state == LVR_NORMAL) || (state == LVR_NORMALDISABLED) )
		{
		apen = pens[TEXTPEN];
		bpen = pens[BACKGROUNDPEN];
		}
	
	if( node->an_Separator )
		name = "-";
	else
		name = node->an_Node.ln_Name;
	
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
	
	if( node->an_Separator )
		{
		SetAPen( rp, bpen );
		RectFill( rp, x, extent.te_Extent.MinY, msg->lvdm_Bounds.MaxX, extent.te_Extent.MaxY );
		
		SetAfPt( rp, BarPattern, 1 );
		SetABPenDrMd( rp, apen, bpen, JAM2 );
		
		y = extent.te_Extent.MinY + ((extent.te_Extent.MaxY-extent.te_Extent.MinY)/2);
		
		RectFill( rp, x, y, msg->lvdm_Bounds.MaxX - 1, y + 1 );
		
		SetAfPt( rp, NULL, 0 );
		}
	else
		{
		SetABPenDrMd( rp, apen, bpen, JAM2 );
		
		Move( rp, x, y );
		Text( rp, name, fit );
		
		SetAPen( rp, bpen );
		xFillOldExtent( rp, &msg->lvdm_Bounds, &extent.te_Extent );
		}
	
	if( (state == LVR_NORMALDISABLED) || (state == LVR_SELECTEDDISABLED) )
		xGhost( rp, pens[BLOCKPEN], msg->lvdm_Bounds.MinX, msg->lvdm_Bounds.MinY, msg->lvdm_Bounds.MaxX, msg->lvdm_Bounds.MaxY );
	
	return( LVCB_OK );
}

VOID xOpenARexxSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	struct ARexxNode *		anode = NULL;
	struct Hook				renderHook;
	struct List *			copylist;
	BOOL					goon = FALSE;
	ULONG					txtlength;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 30 );
	UWORD					ww,wh,lefttext,centerwidth;
	UWORD					lvh = ScrFontYSize * 8 + 2;
	UWORD					top,id,msg,g,width,left;
	
	
	if( !IsListEmpty( ARexxScriptsList ) )
		anode = (struct ARexxNode *)ARexxScriptsList->lh_Head;
	
	renderHook.h_Entry = (HOOKFUNC)xRenderHook;
	
	lefttext    = 15 + xMaxStringLength( ScreenFont, MSG_AR_ITEMS, MSG_AR_SCRIPT );
	centerwidth = 15 + xMaxStringLength( ScreenFont, MSG_AR_ADD, MSG_AR_DOWN );
	txtlength   = 15 + xMaxStringLength( ScreenFont, MSG_AR_SORTALPHABETICALLY, MSG_AR_SORTALPHABETICALLY );
	
	if( centerwidth * 2 > txtlength )
		txtlength = centerwidth * 2;
	else
		centerwidth = txtlength / 2;
	
	if( stringgadlength > txtlength )
		{
		txtlength = stringgadlength;
		centerwidth = txtlength / 2;
		}
	
	if( gads = AllocVec( sizeof( struct xGadget ) * ARGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
			/// First create the string gadget for the listview
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= lefttext;
		ng.ng_TopEdge		= ScreenBarHeight + lvh;
		ng.ng_Width			= txtlength;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_AR_NAME );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_AR_NAME;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN - 1,
			GT_Underscore,	'_',
			GA_Disabled,	anode ? anode->an_Separator : TRUE,
			TAG_END );
		
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Height		= lvh;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_AR_ITEMS );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_AR_LISTVIEW;
		
		gads[1].xg_Gad = CreateGadget( LISTVIEW_KIND, gads[0].xg_Gad, &ng,
			GTLV_ShowSelected,	gads[0].xg_Gad,
			GTLV_Selected,		anode ? 0 : ~0,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			GTLV_Labels,		ARexxScriptsList,
			GTLV_CallBack,		&renderHook,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Create Add, Remove, Up & Down gads
		
		top = gads[1].xg_Gad->TopEdge + gads[1].xg_Gad->Height + GadgetHeight;
		
		for( id = GTID_AR_ADD, msg = MSG_AR_ADD, g = 2;
			 id <= GTID_AR_SORTALPHABETICALLY;
			 id++, msg++, g++ )
			{
			if( g == 4 || g == 6 )
				top += GadgetHeight;
			
			if( g == 3 || g == 5 )
				{
				left = lefttext + centerwidth;
				width = txtlength - centerwidth;
				}
			else
				{
				left = lefttext;
				width = centerwidth;
				}
			
			if( g == 6 )
				width = txtlength;
			
			gads[g].xg_Gad	= NewObject( ButtonClass, NULL,
				GA_ID,			gads[g].xg_GadgetID = id,
				GA_Left,		left,
				GA_Top,			top,
				GA_Width,		width,
				GA_Height,		GadgetHeight,
				GA_RelVerify,	TRUE,
				GA_Previous,	gads[g-1].xg_Gad,
				g > 2 ? GA_Disabled : TAG_IGNORE,	anode ? FALSE : TRUE,
				BUT_Text,		gads[g].xg_GadgetText = GCS( msg ),
				BUT_TextFont,	ScreenFont,
				TAG_DONE );
			}
		
		ng.ng_TopEdge		= top + GadgetHeight;
		ng.ng_Width			-= 20;
		ng.ng_Height		= GadgetHeight;
		ng.ng_GadgetText	= gads[7].xg_GadgetText = GCS( MSG_AR_SCRIPT );
		ng.ng_GadgetID		= gads[7].xg_GadgetID = GTID_AR_SCRIPT;
		
		gads[7].xg_Gad = CreateGadget( STRING_KIND, gads[6].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN - 1,
			GTST_String,	anode ? anode->an_FileName : (STRPTR)"",
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			GA_Disabled,	anode ? anode->an_Separator : TRUE,
			TAG_END );
		
		gads[8].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[8].xg_GadgetID = GTID_AR_GETSCRIPT,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[7].xg_Gad,
			GA_Disabled,	anode ? anode->an_Separator : TRUE,
			BUT_Type,		BUTTYPE_FILE,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[9].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[9].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[8].xg_Gad,
			BUT_Text,		gads[9].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[10].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[10].xg_GadgetID = GTID_CANCEL,
			GA_Left,		lefttext + txtlength - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[9].xg_Gad,
			BUT_Text,		gads[10].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = lefttext + txtlength + 10;
		
		if( copylist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
			{
			goon = TRUE;
			
			NewList( copylist );
			
			if( !IsListEmpty( ARexxScriptsList ) )
				{
				for( anode = (struct ARexxNode *)ARexxScriptsList->lh_Head;
					 anode->an_Node.ln_Succ;
					 anode = (struct ARexxNode *)anode->an_Node.ln_Succ )
					{
					xAddARexxScriptNode( copylist, anode->an_MenuName, anode->an_FileName, anode->an_Separator );
					}
				}
			}
		
		if( goon )
			{
			goon = FALSE;
			
			if( settingsgui = xOpenSettingsGUI( GCS( MSG_AR_SETTINGS ), ww, wh, glist, gads, ARGT_GADS - 1 ) )
				{
				win = settingsgui->sg_Window;
				
				GT_RefreshWindow( win, NULL );
				
				SetFont( win->RPort, ScreenFont );
				
				xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_AR_BAR_AREXX ) );
				xBarTxt( win, gads[ ARGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
				
				if( goon = xHandleARexxSettings( win, gads ) )
					SettingsChanged = TRUE;
				
				xCloseSettingsGUI( settingsgui );
				}
			
				/// User canceled -> copy nodes from copylist to real list
			
			if( !goon )
				{
				xFreeARexxScriptNodes( ARexxScriptsList );
				
				NewList( ARexxScriptsList );
				
				if( !IsListEmpty( copylist ) )
					{
					for( anode = (struct ARexxNode *)copylist->lh_Head;
						 anode->an_Node.ln_Succ;
						 anode = (struct ARexxNode *)anode->an_Node.ln_Succ )
						{
						xAddARexxScriptNode( ARexxScriptsList, anode->an_MenuName, anode->an_FileName, anode->an_Separator );
						}
					}
				}
			
			xFreeARexxScriptNodes( copylist );
			
			FreeVec( copylist );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[10].xg_Gad );
		DisposeObject( gads[9].xg_Gad );
		DisposeObject( gads[8].xg_Gad );
		DisposeObject( gads[6].xg_Gad );
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[4].xg_Gad );
		DisposeObject( gads[3].xg_Gad );
		DisposeObject( gads[2].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleARexxSettings( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	struct NewLVPos			nlv = {0,0};
	struct ARexxNode *		anode;
	APTR					ia;
	ULONG					class;
	UWORD					altcode,code,qualifier;
	LONG					curselected = IsListEmpty( ARexxScriptsList ) ? -1 : 0;
	LONG					curvistop;
	LONG					visible = (gads[ARGT_LISTVIEW].xg_Gad->Height - 2) / ScrFontYSize;
	LONG					top;
	LONG					entries;
	UWORD					a;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	if( anode = xFindARexxNode( curselected ) )
		{
		if( anode->an_Separator )
			xHandleARexxGadgets( win, gads, ARGTB_NAME | ARGTB_SCRIPT, TRUE );
		}
	
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
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( win );
					GT_EndRefresh( win, TRUE );
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case CURSORUP:
						case CURSORDOWN:
							if( !IsListEmpty( ARexxScriptsList ) )
								{
								curvistop = xListViewTop( gads[ARGT_LISTVIEW].xg_Gad, win );
								
								xNewListPos( &nlv, qualifier, code, curselected, curvistop, visible, xCountListEntries( ARexxScriptsList ) );
								
								GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
									GTLV_Selected,	nlv.nl_NewSelected,
									GTLV_Top,		nlv.nl_NewTop,
									TAG_END );
								
								curselected = nlv.nl_NewSelected;
								
									/// Update strings
								
								xUpdateStringGads( win, gads, curselected );
								
									/// Check bar and enable gadgets
								
								if( anode = xFindARexxNode( curselected ) )
									{
									if( anode->an_Separator )
										xHandleARexxGadgets( win, gads, ARGTB_NAME | ARGTB_SCRIPT, TRUE );
									else
										xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_NAME | ARGTB_UPDOWN | ARGTB_SCRIPT | ARGTB_SORT, FALSE );
									}
								}
							break;
						
							/// Return
						
						case KEY_RETURN:
							if( curselected > -1 )
								{
								xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_NAME | ARGTB_UPDOWN | ARGTB_SCRIPT | ARGTB_SORT, FALSE );
								
								ActivateGadget( gads[ARGT_NAME].xg_Gad, win, NULL );
								}
							break;
						
						case KEY_ESCAPE:
							xPressButton( win, gads[ ARGT_GADS - 1 ].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_AREXX );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( gads, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_AR_ADD:
									case GTID_AR_REMOVE:
									case GTID_AR_UP:
									case GTID_AR_DOWN:
									case GTID_AR_SORTALPHABETICALLY:
									case GTID_OKAY:
									case GTID_CANCEL:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											xPressButton( win, tmpgad->xg_Gad );
											
											code = tmpgad->xg_GadgetID;
											}
										break;
									
									case GTID_AR_NAME:
									case GTID_AR_SCRIPT:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											ActivateGadget( tmpgad->xg_Gad, win, NULL );
										
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
						case GTID_AR_SORTALPHABETICALLY:
							GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels, ~0,
								TAG_END );
							
							xSortARexxScriptNodes();
							
							GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels, ARexxScriptsList,
								TAG_END );
							break;
						
						case GTID_AR_LISTVIEW:
								/// Enable the user to edit an entry
							
							curselected = altcode;
							
							xUpdateStringGads( win, gads, curselected );
							
							if( anode = xFindARexxNode( curselected ) )
								{
								if( anode->an_Separator )
									xHandleARexxGadgets( win, gads, ARGTB_NAME | ARGTB_SCRIPT, TRUE );
								else
									{
									xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_NAME | ARGTB_UPDOWN | ARGTB_SCRIPT | ARGTB_SORT, FALSE );
									
									ActivateGadget( gads[ARGT_NAME].xg_Gad, win, NULL );
									}
								}
							break;
						
						case GTID_AR_NAME:
							xUpdateARexxScriptNode( win, gads, GETSTRING( gads[ARGT_NAME].xg_Gad ), GETSTRING( gads[ARGT_SCRIPT].xg_Gad ), curselected );
							break;
						
							/// Add an item (menu item name/separator)
						
						case GTID_AR_ADD:
							strbuf[0] = 0;
							
							if( a = rtGetString( strbuf, DEFAULTBUFFERLEN - 1, GCS( MSG_AR_TL_ADD ), NULL,
								RT_Window,		Window,
								RT_Underscore,	'_',
								RT_TextAttr,	&RequesterFontAttr,
								RT_ReqPos,		REQPOS_CENTERSCR,
								RTGS_GadFmt,	GCS( MSG_AR_GF_ADD ),
								RTGS_TextFmt,	GCS( MSG_AR_ADDTEXT ),
								TAG_END ) )
								{
								GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
									GTLV_Labels, ~0,
									TAG_END );
								
									/// Add the name or separator
								
								if( xAddARexxScriptNode( ARexxScriptsList, a == 2 ? (GadToolsBase->lib_Version >= 39 ? (STRPTR)"" : (STRPTR)"---") : strbuf, "", a == 2 ? TRUE : FALSE ) )
									{
									if( curselected == -1 )
										{
											/// First node in list...
										
										curselected = 0;
										
										if( anode = xFindARexxNode( curselected ) )
											{
											if( anode->an_Separator )
												xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_UPDOWN | ARGTB_SORT, FALSE );
											else
												xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_NAME | ARGTB_UPDOWN | ARGTB_SCRIPT | ARGTB_SORT, FALSE );
											}
										}
									
									GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
										GTLV_Labels,	ARexxScriptsList,
										GTLV_Selected,	curselected,
										TAG_END );
									}
								else
									GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
										GTLV_Labels, ARexxScriptsList,
										TAG_END );
								}
							break;
						
						case GTID_AR_REMOVE:
							if( !IsListEmpty( ARexxScriptsList ) )
								{
								if( curselected > -1 )
									{
									BOOL	rem;
									
									
									GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
										GTLV_Labels, ~0,
										TAG_END );
									
									rem = xRemoveARexxScriptNode( curselected );
									
									GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
										GTLV_Labels, ARexxScriptsList,
										TAG_END );
									
									if( rem )
										{
										if( !IsListEmpty( ARexxScriptsList ) )
											{
											if( curselected > (xCountListEntries( ARexxScriptsList ) - 1) )
												curselected--;
											
												/// Set new current selected
											
											GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
												GTLV_Selected, curselected,
												TAG_END );
											
												/// If removed was a separator stuff needs updating
											
											if( anode = xFindARexxNode( curselected ) )
												{
												if( anode->an_Separator )
													xHandleARexxGadgets( win, gads, ARGTB_NAME | ARGTB_SCRIPT, TRUE );
												else
													{
													xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_NAME | ARGTB_UPDOWN | ARGTB_SCRIPT | ARGTB_SORT, FALSE );
													
													xUpdateStringGads( win, gads, curselected );
													}
												}
											}
										else
											{
												/// Disable the lot
											
											xHandleARexxGadgets( win, gads, ARGTB_REMOVE | ARGTB_NAME | ARGTB_UPDOWN | ARGTB_SCRIPT | ARGTB_SORT, TRUE );
											
											curselected = -1;
											}
										break;
										}
									}
								}
							break;
						
						case GTID_AR_UP:
							GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels, ~0,
								TAG_END );
							
							if( curselected > 0 && xCountListEntries( ARexxScriptsList ) > 1 )
								{
								xSwapARexxNodes( curselected, TRUE );
								
								curselected--;
								
								xUpdateStringGads( win, gads, curselected );
								}
							
							top = xListViewTop( gads[ARGT_LISTVIEW].xg_Gad, win );
							
							GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels,	ARexxScriptsList,
								GTLV_Selected,	curselected,
								GTLV_Top,		top < curselected ? top : curselected,
								TAG_END );
							break;
						
						case GTID_AR_DOWN:
							GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels, ~0,
								TAG_END );
							
							entries = xCountListEntries( ARexxScriptsList );
							
							if( curselected < (entries - 1) && entries > 1 )
								{
								xSwapARexxNodes( curselected, FALSE );
								
								curselected++;
								
								xUpdateStringGads( win, gads, curselected );
								}
							
							top = xListViewTop( gads[ARGT_LISTVIEW].xg_Gad, win );
							
							GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels,	ARexxScriptsList,
								GTLV_Selected,	curselected,
								GTLV_Top,		(top + visible - 1) < curselected ? __builtin_max( curselected + 1 - visible, 0 ) : top,
								TAG_END );
							break;
						
						case GTID_AR_SCRIPT:
							xUpdateARexxScriptNode( win, gads, GETSTRING( gads[ARGT_NAME].xg_Gad ), GETSTRING( gads[ARGT_SCRIPT].xg_Gad ), curselected );
							
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_AR_GETSCRIPT:
							if( xAddARexxScript( win, gads, curselected ) )
								xUpdateStringGads( win, gads, curselected );
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

VOID xUpdateStringGads( struct Window *win, struct xGadget *gads, LONG nr )
{
	struct ARexxNode *	anode;
	
	
	anode = (struct ARexxNode *)ARexxScriptsList->lh_Head;
	
	while( nr && anode )
		{
		nr--;
		
		anode = (struct ARexxNode *)anode->an_Node.ln_Succ;
		}
	
	if( anode )
		{
		if( !(gads[ARGT_SCRIPT].xg_Gad->Flags & GFLG_DISABLED) )
			{
			GT_SetGadgetAttrs( gads[ARGT_SCRIPT].xg_Gad, win, NULL,
				GTST_String, anode->an_FileName,
				TAG_END );
			}
		}
}

VOID xHandleARexxGadgets( struct Window *win, struct xGadget *gads, UWORD gad, BOOL disable )
{
	struct TagItem *	tags = disable ? DisabledTrueTags : DisabledFalseTags;
	BOOL				go = TRUE;
	
	
	if( gad & ARGTB_REMOVE )
		{
		if( !disable )
			{
			if( !(gads[ARGT_REMOVE].xg_Gad->Flags & GFLG_DISABLED) )
				go = FALSE;
			}
		
		if( go )
			SetGadgetAttrsA( gads[ARGT_REMOVE].xg_Gad, win, NULL, tags );
		else
			go = TRUE;
		}
	
	if( gad & ARGTB_UPDOWN )
		{
		if( !disable )
			{
			if( !(gads[ARGT_UP].xg_Gad->Flags & GFLG_DISABLED) )
				go = FALSE;
			}
		
		if( go )
			{
			SetGadgetAttrsA( gads[ARGT_UP].xg_Gad, win, NULL, tags );
			SetGadgetAttrsA( gads[ARGT_DOWN].xg_Gad, win, NULL, tags );
			}
		else
			go = TRUE;
		}
	
	if( gad & ARGTB_SORT )
		{
		if( !disable )
			{
			if( !(gads[ARGT_SORT].xg_Gad->Flags & GFLG_DISABLED) )
				go = FALSE;
			}
		
		if( go )
			SetGadgetAttrsA( gads[ARGT_SORT].xg_Gad, win, NULL, tags );
		else
			go = TRUE;
		}
	
	if( gad & ARGTB_NAME )
		{
		if( !disable )
			{
			if( !(gads[ARGT_NAME].xg_Gad->Flags & GFLG_DISABLED) )
				go = FALSE;
			}
		
		if( go )
			GT_SetGadgetAttrsA( gads[ARGT_NAME].xg_Gad, win, NULL, tags );
		else
			go = TRUE;
		}
	
	if( gad & ARGTB_SCRIPT )
		{
		if( !disable )
			{
			if( !(gads[ARGT_SCRIPT].xg_Gad->Flags & GFLG_DISABLED) )
				go = FALSE;
			}
		
		if( go )
			{
			GT_SetGadgetAttrsA( gads[ARGT_SCRIPT].xg_Gad, win, NULL, tags );
			SetGadgetAttrsA( gads[ARGT_GETSCRIPT].xg_Gad, win, NULL, tags );
			}
		}
}

/*
 * Add a script to an item ARexxScriptsList.. 
 */
BOOL xAddARexxScript( struct Window *win, struct xGadget *gads, LONG curselected )
{
	BPTR				lock;
	BOOL				added = FALSE;
	UBYTE				strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE				strbuf2[ DEFAULTBUFFERLEN ] = "";
	
	
	rtChangeReqAttr( FileReq, RTFI_Dir, REXXPATH, TAG_END );
	
	if( rtFileRequest( FileReq, REXXFILE, GCS( MSG_TL_SELECTFILE ),
		RT_Window,		win,
		RT_Underscore,	'_',
		RT_TextAttr,	&RequesterFontAttr,
		RTFI_OkText,	GCS( MSG_GF_SELECT ),
		TAG_END ) )
		{
		added = TRUE;
		
		GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
			GTLV_Labels, ~0,
			TAG_END );
		
		strncpy( strbuf1, FileReq->Dir, DEFAULTPATHLEN );
		AddPart( strbuf1, REXXFILE, DEFAULTBUFFERLEN );
		
		if( lock = Lock( strbuf1, ACCESS_READ ) )
			{
			if( NameFromLock( lock, strbuf2, DEFAULTBUFFERLEN ) )
				xUpdateARexxScriptNode( win, gads, NULL, strbuf2, curselected );
			
			UnLock( lock );
			}
		
			/// Reattach the modified list to the gadget and set the selected one
		
		GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
			GTLV_Labels, ARexxScriptsList,
			TAG_END );
		
		strncpy( REXXPATH, FileReq->Dir, DEFAULTPATHLEN );
		}
	
	return( added );
}

BOOL xAddARexxScriptNode( struct List *l, STRPTR menuname, STRPTR filename, BOOL separator )
{
	struct ARexxNode *	an;
	
	
	if( an = AllocVec( sizeof( struct ARexxNode ), MEMF_CLEAR ) )
		{
		an->an_Separator = separator;
		
		strncpy( an->an_MenuName, menuname, DEFAULTBUFFERLEN );
		strncpy( an->an_FileName, filename, DEFAULTBUFFERLEN );
		
		an->an_Node.ln_Name = an->an_MenuName;
		
		AddTail( l, (struct Node *)an );
		
		return( TRUE );
		}
	
	return( FALSE );
}

BOOL xRemoveARexxScriptNode( WORD number )
{
	struct Node *	worknode;
	BOOL			success = FALSE;
	
	
	worknode = ARexxScriptsList->lh_Head;
	
	while( worknode && number )
		{
		number--;
		
		worknode = worknode->ln_Succ;
		}
	
	if( worknode )
		{
		Remove( worknode );
		
		FreeVec( worknode );
		
		success = TRUE;
		}
	
	return( success );
}

VOID xFreeARexxScriptNodes( struct List *l )
{
	if( l && !IsListEmpty( l ) )
		{
		struct Node *	worknode;
		struct Node *	nextnode;
		
		
		worknode = l->lh_Head;
		
		while( nextnode = worknode->ln_Succ )
			{
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		}
}

VOID xSortARexxScriptNodes( VOID )
{
	struct ARexxNode *	pos;
	struct ARexxNode *	min;
	struct ARexxNode *	nn;
	UBYTE				temp[DEFAULTBUFFERLEN] = "";
	BOOL				sep;
	
	
	pos = (struct ARexxNode *)ARexxScriptsList->lh_Head;
	
	/*
	** Så længe pos ikke er i slutningen af filen
	*/
	while( pos->an_Node.ln_Succ )
		{
		nn = pos;
		min = pos;
		
		while( (struct ARexxNode *)nn->an_Node.ln_Succ )
			{
				// min > nn  -> min = nn
			
			if( stricmp( min->an_MenuName, nn->an_MenuName ) > 0 )
				min = nn;
			
			nn = (struct ARexxNode *)nn->an_Node.ln_Succ;
			}
		
			// min og pos bytter plads
		
		nn = pos;
		
			// I stedet for at bruge endnu en temp pointer!
		
		strcpy( temp, pos->an_MenuName );
		strcpy( nn->an_MenuName, min->an_MenuName );
		strcpy( min->an_MenuName, temp );
		
		strcpy( temp, pos->an_FileName );
		strcpy( nn->an_FileName, min->an_FileName );
		strcpy( min->an_FileName, temp );
		
		sep = pos->an_Separator;
		nn->an_Separator = min->an_Separator;
		min->an_Separator = sep;
		
		pos = (struct ARexxNode *)pos->an_Node.ln_Succ;
		}
}

VOID xUpdateARexxScriptNode( struct Window *win, struct xGadget *gads, STRPTR menuname, STRPTR filename, ULONG nr )
{
	struct ARexxNode *	pnode;
	UWORD				c = 0;
	
	
	GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
		GTLV_Labels, ~0,
		TAG_END );
	
	pnode = (struct ARexxNode *)ARexxScriptsList->lh_Head;
	
		/// Find the right buffer
	
	while( pnode )
		{
		if( c == nr )
			break;
		
		c++;
		pnode = (struct ARexxNode *)pnode->an_Node.ln_Succ;
		}
	
	/*
	** Copy the new names
	*/
	if( pnode )
		{
		if( menuname )
			strncpy( pnode->an_MenuName, menuname, 39 );
		
		if( filename )
			strncpy( pnode->an_FileName, filename, DEFAULTBUFFERLEN - 1 );
		}
	
	GT_SetGadgetAttrs( gads[ARGT_LISTVIEW].xg_Gad, win, NULL,
		GTLV_Labels,	ARexxScriptsList,
		GTLV_Selected,	nr,
		TAG_END );
}

VOID xSwapARexxNodes( LONG curselected, BOOL upwards )
{
	struct ARexxNode *	curnode;
	struct ARexxNode *	swapnode;
	LONG				n = curselected;
	
	
	curnode = (struct ARexxNode *)ARexxScriptsList->lh_Head;
	
	while( n && curnode )
		{
		n--;
		
		curnode = (struct ARexxNode *)curnode->an_Node.ln_Succ;
		}
	
	if( curnode )
		{
		if( upwards )
			{
			swapnode = (struct ARexxNode *)curnode->an_Node.ln_Pred;
			
			Remove( (struct Node *)curnode );
			
			Insert( ARexxScriptsList, (struct Node *)curnode, swapnode->an_Node.ln_Pred );
			}
		else
			{
			swapnode = (struct ARexxNode *)curnode->an_Node.ln_Succ;
			
			Remove( (struct Node *)curnode );
			
			Insert( ARexxScriptsList, (struct Node *)curnode, (struct Node *)swapnode );
			}
		}
}

struct ARexxNode *xFindARexxNode( LONG nr )
{
	struct ARexxNode *	anode = NULL;
	
	
	if( nr != -1 )
		{
		anode = (struct ARexxNode *)ARexxScriptsList->lh_Head;
		
		while( nr && anode )
			{
			nr--;
			
			anode = (struct ARexxNode *)anode->an_Node.ln_Succ;
			}
		}
	
	return( anode );
}
