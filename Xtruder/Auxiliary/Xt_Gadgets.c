/*
**	$VER: XtruderGadgets.c (28.8.95)
**
**	Xtruder functions for gadgets
**
**	Copyright (C) 1994 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Returns a vanilla code converted from a msg rawkey code
 */
UWORD xConvertMsgRawToVan( UWORD code, UWORD qualifier, APTR iaddress )
{
	struct InputEvent	ie;
	APTR *				eventptr;
	UBYTE				buffer[8];
	WORD				i;
	
	
	ie.ie_Class = IECLASS_RAWKEY;
	ie.ie_Code = code;
	ie.ie_Qualifier = qualifier;
	
	eventptr = iaddress;
	ie.ie_EventAddress = *eventptr;
	
	i = MapRawKey( &ie, buffer, 8, NULL );
	
	if( i > 0 )
		return( (UWORD)ToLower( buffer[0] ) );
	
	return( 0 );
}

/*
 * Goes through a link list of (X)gadgets to see wether any of them has an
 * underscore which corresponds to the code given
 */
struct xGadget *xCheckUnderscore( struct xGadget *g, UWORD code, UWORD qualifier, APTR iaddress )
{
	UBYTE *	src;
	UWORD	newcode;
	
	
	if( newcode = xConvertMsgRawToVan( code, qualifier, iaddress ) )
		{
		while( g )
			{
			if( src = g->xg_GadgetText )
				{
				while( *src )
					{
					if( *src == '_' )
						{
						src++;
						
						if( tolower( *src ) == newcode )
							return( g );
						
						break;
						}
					
					src++;
					}
				}
			
			g = g->xg_NextGad;
			}
		}
	
	return( NULL );
}

/*
 * Converts a vanilla char to a rawkey code...
 */
UWORD xConvertVanToRaw( STRPTR str )
{
	UBYTE	iebuffer[6];
	UBYTE	vanchar[2] = {0};
	UBYTE *	src = str;
	
	
	while( *src )
		{
		if( *src == '_' )
			{
			*src++;
			vanchar[0] = *src;
			
			if( MapANSI( vanchar, 1, iebuffer, 3, NULL) > 0 )
				{
				xSPrintF( vanchar, "%ld", iebuffer[0] );
				
				return( (UWORD)atol( vanchar ) );
				}
			
			break;
			}
		*src++;
		}
}

UBYTE xIsGadSelected( struct Gadget *gad )
{
	if( gad->Flags & GFLG_SELECTED )
		return( 1 );
	
	return 0;
}

/*
 * Sets the string in a string gadget
 */
VOID xSetGadgetString( struct Window *win, struct Gadget *gad, STRPTR string )
{
	GT_SetGadgetAttrs( gad, win, NULL,
		GTST_String, string,
		TAG_END );
}

VOID xGhostGadget( struct Window *win, struct Gadget *gad )
{
	if( !(gad->Flags & GFLG_DISABLED) )
		{
		GT_SetGadgetAttrs( gad, win, NULL,
			GA_Disabled, TRUE,
			TAG_DONE );
		}
}

VOID xUnGhostGadget( struct Window *win, struct Gadget *gad )
{
	if( gad->Flags & GFLG_DISABLED )
		{
		GT_SetGadgetAttrs( gad, win, NULL,
			GA_Disabled, FALSE,
			TAG_DONE );
		}
}

VOID xGhostObject( struct Window *win, struct Gadget *gad )
{
	if( !(gad->Flags & GFLG_DISABLED) )
		{
		SetGadgetAttrs( gad, win, NULL,
			GA_Disabled, TRUE,
			TAG_DONE );
		}
}

VOID xUnGhostObject( struct Window *win, struct Gadget *gad )
{
	if( gad->Flags & GFLG_DISABLED )
		{
		SetGadgetAttrs( gad, win, NULL,
			GA_Disabled, FALSE,
			TAG_DONE );
		}
}

VOID xPressButton( struct Window *win, struct Gadget *gad )
{
	SetGadgetAttrs( gad, win, NULL,
		GA_Selected, TRUE,
		TAG_DONE );
	
	Delay( 4 );
	
	SetGadgetAttrs( gad, win, NULL,
		GA_Selected, FALSE,
		TAG_DONE );
}

/*
 * Tries to find a gadget starting from the given gadget
 */
struct xGadget *xFindGadget( struct xGadget *g, UWORD gadgetid )
{
	do
		{
		if( g->xg_GadgetID == gadgetid )
			break;
		}
	while( g = g->xg_NextGad );
	
	return( g );
}

LONG xListViewTop( struct Gadget *gad, struct Window *win )
{
	LONG top;
	
	
	if( GadToolsBase->lib_Version >= 39 )
		GT_GetGadgetAttrs( gad, win, NULL, GTLV_Top, &top, TAG_END );
	else
		top = (LONG)(*(short *)(((char *)gad)+sizeof(struct Gadget)+4));
	
	return( top );
}

VOID xNewListPos( struct NewLVPos *nlv, UWORD rawqual, UWORD code, LONG curselected, LONG curvistop, LONG visible, LONG listentries )
{
	WORD	maxposition = listentries - 1;
	WORD	newposition;
	UBYTE	shiftpressed = rawqual & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
	UWORD	altpressed = rawqual & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);
	UBYTE	ctrlpressed = rawqual & IEQUALIFIER_CONTROL;
	UBYTE	cursordown = code == CURSORDOWN ? 1 : 0;
	UBYTE	move = 1;
	
	
		/// If current is -1 there isn't any highlighted entries
	
	if( curselected == -1 )
		{
		nlv->nl_NewTop = nlv->nl_NewSelected = 0;
		
		return;
		}
	
		/// If Control pressed move to top or bottom
	
	if( ctrlpressed )
		{
		if( cursordown )
			{
			nlv->nl_NewSelected = nlv->nl_NewTop = listentries - 1;
			
			return;
			}
		else
			{
			nlv->nl_NewSelected = nlv->nl_NewTop = 0;
			
			return;
			}
		}
	
		/// If Alt pressed move 2 entries
	
	if( altpressed )
		move = 2;
	else
	if( shiftpressed )
		move = visible;
	
		/// Now adjust position
	
	if( cursordown )
		{
		newposition = curselected + move;
		
		if( newposition > maxposition )
			{
			nlv->nl_NewSelected = maxposition;
			
			if( maxposition - visible < 0 )
				nlv->nl_NewTop = 0;
			else
				nlv->nl_NewTop = maxposition - visible + 1;
			}
		else
		if( newposition >= curvistop + visible )
			{
				/// New selected entry's position is too large for it to
				/// be in the windows area. We must recalculate the new
				/// visible top position.
			
			nlv->nl_NewTop = newposition - visible + 1;
			nlv->nl_NewSelected = newposition;
			}
		else
			{
			nlv->nl_NewTop = curvistop;
			nlv->nl_NewSelected = newposition;
			}
		}
	else
		{
			/// Cursor up
		
		newposition = curselected - move;
		
		if( newposition < 0 )
			nlv->nl_NewTop = nlv->nl_NewSelected = 0;
		else
		if( newposition < curvistop )
			{
				/// New selected entry's position is higher up than current
				/// visible top so we must adjust visible top position.
			
			nlv->nl_NewTop = nlv->nl_NewSelected = newposition;
			}
		else
			{
			nlv->nl_NewTop = curvistop;
			nlv->nl_NewSelected = newposition;
			}
		}
}

LONG xSliderLevel( struct Gadget *gad, struct Window *win )
{
	LONG	level;
	
	
	if( GadToolsBase->lib_Version >= 39 )
		GT_GetGadgetAttrs( gad, win, NULL, GTSL_Level, &level, TAG_END );
	else
		level = (LONG)(*(short *)(((char *)gad)+sizeof(struct Gadget)+4) );
	
	return( level );
}

LONG xCycleActive( struct Gadget *gad, struct Window *win )
{
	LONG	active;
	
	
	if( GadToolsBase->lib_Version >= 39 )
		GT_GetGadgetAttrs( gad, win, NULL, GTCY_Active, &active, TAG_END );
	else
		active = (LONG)(*(short *)(((char *)gad)+sizeof(struct Gadget)+6));
	
	return( active );
}

VOID xRefreshCycleGadget( struct Window *win, struct Gadget *gad, UWORD qualifier, UWORD maxcycles )
{
	UWORD	active = xCycleActive( gad, win );
	
	
	qualifier &= IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
	
	if( active == maxcycles )
		{
		GT_SetGadgetAttrs( gad, win, NULL,
			GTCY_Active, qualifier ? maxcycles - 1 : 0,
			TAG_END );
		}
	else
		{
		GT_SetGadgetAttrs( gad, win, NULL,
			GTCY_Active, (active == 0) ? (qualifier ? maxcycles : 1) : (qualifier ? active - 1 : active + 1),
			TAG_END );
		}
}
