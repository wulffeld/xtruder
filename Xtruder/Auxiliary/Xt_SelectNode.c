/*
**	$VER: XtruderSelectNode.c (21.8.96)
**
**	Xtruder select a node from a list window functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct Gadget *		SelNodeGadList;
struct Gadget *		ListViewGad;

/*
 * This functions opens a sizable window with a listview and returns a structure to a header
 * for this window's important vars.
 */
struct Node *xOpenSelectNodeWindow( struct List *slist, STRPTR title, STRPTR bartitle, UWORD guidecontext, UWORD *coords )
{
	struct Window *	win;
	struct Node *	node = NULL;
	UWORD			width;
	
	
		/// Figure out max and min bounds for window
	
	width = 100 + TextLength( RastPort, bartitle, strlen( bartitle ) );
	
	if( width > coords[2] )
		coords[2] = width;
	
	if( 8 * ScrFontYSize > coords[3] )
		coords[3] = 8 * ScrFontYSize;
	
		/// Open the window and attach listview
	
	if( win = OpenWindowTags( NULL,
		WA_CustomScreen,	Screen,
		WA_Title,			title,
		WA_IDCMP,			IDCMP_REFRESHWINDOW |
							IDCMP_MOUSEBUTTONS |
							LISTVIEWIDCMP |
							IDCMP_CLOSEWINDOW |
							IDCMP_NEWSIZE |
							IDCMP_RAWKEY,
		WA_Left,			coords[0],
		WA_Top,				coords[1],
		WA_Width,			coords[2],
		WA_Height,			coords[3],
		WA_DragBar,			TRUE,
		WA_CloseGadget,		TRUE,
		WA_DepthGadget,		TRUE,
		WA_Activate,		TRUE,
		WA_SmartRefresh,	TRUE,
		WA_RMBTrap,			TRUE,
		WA_SizeGadget,		TRUE,
		WA_SizeBBottom,		TRUE,
		WA_MinWidth,		width,
		WA_MinHeight,		8 * ScrFontYSize,
		WA_MaxWidth,		Screen->Width,
		WA_MaxHeight,		Screen->Height,
		TAG_END ) )
		{
		GT_RefreshWindow( win, NULL );
		
		SetFont( win->RPort, ScreenFont );
		SetAPen( win->RPort, 1 );
		
		if( xCreateSelectNodeGadgets( win, slist, FALSE, -1 ) )
			{
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, bartitle );
			
			node = xHandleSelectNodeIDCMP( win, slist, bartitle, guidecontext );
			
			xCreateSelectNodeGadgets( win, slist, TRUE, 0 );
			}
		
		coords[0] = win->LeftEdge;
		coords[1] = win->TopEdge;
		coords[2] = win->Width;
		coords[3] = win->Height;
		
		CloseWindow( win );
		}
	
	return( node );
}

struct Node *xHandleSelectNodeIDCMP( struct Window *win, struct List *slist, STRPTR bartitle, UWORD guidecontext )
{
	struct NewLVPos			nlv = {0,0};
	struct IntuiMessage *	msg;
	ULONG					class;
	UWORD					qual,code,altcode;
	WORD					visible,curselected = -1, oldsel = -1;
	ULONG					seconds, micros;
	ULONG					sec1 = 0,mic1 = 0,sec2,mic2;
	BOOL					done = FALSE;


	visible = (ListViewGad->Height - 2) / ScrFontYSize;
	
	while( !done )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			altcode = code = msg->Code;
			qual = msg->Qualifier;
			seconds = msg->Seconds;
			micros = msg->Micros;
			
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
							xNewListPos( &nlv, qual, code, curselected, xListViewTop( ListViewGad, win ), visible, xCountListEntries( slist ) );
							
							GT_SetGadgetAttrs( ListViewGad, win, NULL,
								GTLV_Selected,	curselected = nlv.nl_NewSelected,
								GTLV_Top,		nlv.nl_NewTop,
								TAG_END );
							break;
						
						case KEY_RETURN:
							done = TRUE;
							break;
						
						case KEY_ESCAPE:
							done = TRUE;
							
							curselected = -1;
							break;
						}
					break;
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case 1000:
							curselected = altcode;
							
								/// Save the old time
							
							sec2 = sec1;
							mic2 = mic1;
							
								/// Get the new time
							
							sec1 = seconds;
							mic1 = micros;
							
							if( curselected == oldsel && DoubleClick( sec2, mic2, sec1, mic1 ) )
								{
								done = TRUE;
								
								sec1 = 0;
								mic1 = 0;
								}
							
							oldsel = curselected;
							break;
						}
					break;
				
				case IDCMP_NEWSIZE:
					if( !(xCreateSelectNodeGadgets( win, slist, FALSE, curselected ) ) )
						done = TRUE;
					else
						visible = (ListViewGad->Height - 2) / ScrFontYSize;
					
					xBarTxt( win, ScreenBarHeight + ScrFontYSize, bartitle );
					break;
				
				case IDCMP_CLOSEWINDOW:
					done = TRUE;
					
					curselected = -1;
					break;
				}
			}
		}
	
		/// Find the filenode of the selected number
	
	if( curselected != -1 )
		return( xFindCurrentSelectedNode( slist, curselected ) );
	
	return( NULL );
}

struct Gadget *xCreateSelectNodeGadgets( struct Window *win, struct List *slist, BOOL quit, WORD curselected )
{
	struct NewGadget	ng;
	struct Gadget *		gad;
	
	
	SetAPen( win->RPort, 0 );
	RectFill( win->RPort, 4, ScreenBarHeight, win->Width - 5,  win->Height - 8 );
	
	if( SelNodeGadList )
		{
		RemoveGList( win, SelNodeGadList, -1 );
		
		FreeGadgets( SelNodeGadList );
		SelNodeGadList = NULL;
		}
	
	if( quit )
		return( FALSE );
	
	RefreshWindowFrame( win );
	
	gad = CreateContext( &SelNodeGadList );
	
	ng.ng_VisualInfo	= VisualInfo;
	ng.ng_TextAttr		= &ScreenFontAttr;
	ng.ng_LeftEdge		= 10;
	ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
	ng.ng_Width			= win->Width - 20;
	ng.ng_Height		= win->Height - (win->BorderTop + win->BorderBottom) - ScrFontYSize - 4;
	ng.ng_GadgetText	= "";
	ng.ng_GadgetID		= 1000;
	ng.ng_Flags			= 0;
	
	if( ListViewGad = CreateGadget( LISTVIEW_KIND, gad, &ng,
		GTLV_ShowSelected,	NULL,
		GTLV_Labels,		slist,
		GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
		curselected != -1 ? GTLV_Selected : TAG_IGNORE, curselected,
		TAG_END ) )
		{
		AddGList( win, SelNodeGadList, -1, -1, NULL );
		RefreshGList( SelNodeGadList, win, NULL, -1 );
		
		GT_RefreshWindow( win, NULL );
		
		return( gad );
		}
	
	FreeGadgets( SelNodeGadList );
	SelNodeGadList = NULL;
	
	return( NULL );
}
