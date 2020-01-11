/*
**	$VER: XtruderGraphics.c (11.8.95)
**
**	Xtruder graphic (drawing, etc.) functions
**
**	Copyright (C) 1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Simple write of text to a window using current font...
 */
VOID __stdargs xWTxt( struct Window *w, ULONG x, ULONG y, UBYTE pen, STRPTR format, ... )
{
	UBYTE	strbuf[ DEFAULTBUFFERLEN ];
	
	
	if( w )
		{
		SetAPen( w->RPort, pen );
		
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
		
		Move( w->RPort, x, y );
		Text( w->RPort, strbuf, strlen( strbuf ) );
		}
}

/*
 * Draw a nice 3D line bar
 */
VOID __stdargs xBarTxt( struct Window *win, UWORD y, STRPTR format, ... )
{
	struct RastPort *	rp = win->RPort;
	UWORD				left = 10;
	UWORD				right = win->Width - 11;
	UWORD				by = y + 1 - (rp->Font->tf_YSize / 2);
	UWORD				pixellen;
	WORD				x;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
	
	pixellen = TextLength( rp, strbuf, strlen( strbuf ) );
	x = 2 + (win->Width / 2) - (pixellen / 2);
	
	if( strbuf[0] )
		{
		SetAPen( rp, 2 );
		
		Move( rp, x, y - (rp->Font->tf_YSize - rp->Font->tf_Baseline) + 2 );
		Text( rp, strbuf, strlen( strbuf ) );
		}
	
		/// Draw backwards black
	
	SetAPen( rp, 1 );
	
	if( !strbuf[0] )
		{
		x += 5;
		
		by = y - 3;
		
		left = 4;
		right = win->Width - 5;
		}
	
	Move( rp, x - 5, by );
	Draw( rp, left, by );
	
	Move( rp, left, by + 1 );
	
		/// Draw onwards white
	
	SetAPen( rp, 2 );
	
	Draw( rp, x - 5, by + 1 );
	
		/// Beyond txt
	
	if( !strbuf[0] )
		x -= 10;
	
	Move( rp, x + 5 + pixellen, by + 1 );
	Draw( rp, right, by + 1 );
	
	Move( rp, right, by );
	
		/// Draw onwards black
	
	SetAPen( rp, 1 );
	
	Draw( rp, x + 5 + pixellen, by );
}

/*
 * Goes through the strings and returns the pixel length of the longest
 * one found
 */
ULONG xMaxStringLength( struct TextFont *tf, ULONG msgidstart, ULONG msgidend )
{
	struct TextExtent	extent;
	struct RastPort		rp;
	ULONG				length = 0,l;
	
	
	InitRastPort( &rp );
	
	SetFont( &rp, tf );
	
	do
		{
		TextFit( &rp, GCS( msgidstart ), strlen( GCS( msgidstart ) ), &extent, NULL, 1, 32765, 32765 );
		
		l = 1 + extent.te_Extent.MaxX - extent.te_Extent.MinX;
		
		if( l > length )
			length = l;
		}
	while( msgidstart++ < msgidend );
	
	return( length );
}

ULONG xTextLength( struct TextFont *tf, STRPTR str )
{
	struct TextExtent	extent;
	struct RastPort		rp;
	
	
	InitRastPort( &rp );
	
	SetFont( &rp, tf );
	
	TextFit( &rp, str, strlen( str ), &extent, NULL, 1, 32765, 32765 );
	
	return( (ULONG)(1 + extent.te_Extent.MaxX - extent.te_Extent.MinX) );
}

/*
 * Calculate the length of a gadget which possibly is using a proportional
 * font. 
 */
ULONG xProportionalLength( struct TextFont *tf, UWORD chars )
{
	struct TextExtent	extent;
	struct RastPort		rp;
	ULONG				length;
	UWORD				a;
	UBYTE				tempstr[ 100 ];
	
	
	InitRastPort( &rp );
	
	SetFont( &rp, tf );
	
	for( a = 0; a < chars; a++ )
		tempstr[a] = 'A' + a;
	
	tempstr[a] = '\0';
	
	TextFit( &rp, tempstr, strlen( tempstr ), &extent, NULL, 1, 32765, 32765 );
	
	length = 1 + extent.te_Extent.MaxX - extent.te_Extent.MinX;
	
	return( length );
}
