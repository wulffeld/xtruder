/*
**	$VER: XtruderBackfill.c (22.8.96)
**
**	Xtruder backfill routines
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/****** CopyTiledBitMap ****************************************************************************
*
*   NAME
*	CopyTiledBitMap() -- fill bitmap with bitmap pattern
*
*   SYNOPSIS
*	CopyTiledBitMap(Src,SrcOffsetX,SrcOffsetY,SrcSizeX,SrcSizeY,Dst,DstBounds)

*	void CopyTiledBitMap(struct BitMap *,WORD,WORD,WORD,WORD,struct BitMap *,struct Rectangle *)
*
*   FUNCTION
*	fills the destination bitmap with repeated (tiled) copies of the source bitmap
*
*   INPUTS
*	Src = source bitmap.
*	SrcOffsetX = offset (in x direction) to use in the blit.
*	SrcOffsetY = offset (in y direction) to use in the blit.
*	             The pixel at position (SrcOffsetX,SrcOffsetY) in the source bitmap
*	             will be the first visible one in the upper left corner of DstBounds.
*	SrcSizeX = width of the source bitmap Src.
*	SrcSizeY = height of the source bitmap Src.
*	Dst = destination bitmap.
*	DstBounds = this rectangle in the destination bitmap will be filled.
*
*   RESULT
*	None.
*
*   EXAMPLE
*	This function is most useful in backfill hooks.
*
*   NOTES
*	The following relations must hold:
*		0 <= SrcOffsetX < SrcSizeX
*		0 <= SrcOffsetY < SrcSizeY
*	I.e. if calculating the offset involves some kind of modulo-operations
*	they have to be done before calling this function
*
*   BUGS
*
*   AUTHOR
*	Copyright © 1995 by Walter Dörwald
*
*   SEE ALSO
*
****************************************************************************************************
*
*	This function uses an "exponential" blit to fill the dest rect.
*	Suppose we have a 64×64 source bitmap and want to use offset 0,0
*	(i.e. the upper left corner of the source bitmap is copied to the
*	upper left corner of the dest rect. Let the dest rect be 320×320
*	with upper left corner (0,0). Thus we'll blit 5×5 copies of the
*	source bitmap to the destination.
*	Doing this with two nested loops would thus require 25 blits, or
*	x·y blits for x horizontal tiles and y vertical tiles.
*	First one copy of the source bitmap is blitted to the upper left
*	corner of the dest rect:
*
*		#....
*		.....
*		.....
*		.....
*		.....
*
*	This copy is then blitted to the right:
*
*		##...
*		.....
*		.....
*		.....
*		.....
*
*	*Both* copies are once again blitted to the right
*
*		####.
*		.....
*		.....
*		.....
*		.....
*
*	Finally the first row is filled with one blit:
*
*		#####
*		.....
*		.....
*		.....
*		.....
*
*	Now the same is done vertically to fill the complete rect:
*
*		#####   #####   #####
*		#####   #####   #####
*		.....   #####   #####
*		.....   #####   #####
*		.....   .....   #####
*
*	This requires 7 blit, or generally 1+ceil(log_2(x))+ceil(log_2(y))
*
*	The blits are done first in x direction and then in y direction
*	to minimize potential masking effects at the left and right border.
*
*	If the offsets into the source bitmap are not 0, part of a second tile
*	is blitted to the destination bitmap to once again get a tile of the
*	required size.
*/

VOID CopyTiledBitMap( struct BitMap *Src, WORD SrcOffsetX, WORD SrcOffsetY, WORD SrcSizeX, WORD SrcSizeY, struct BitMap *Dst, struct Rectangle *DstBounds )
{
	WORD	FirstSizeX;  // the width of the rectangle to blit as the first column
	WORD	FirstSizeY;  // the height of the rectangle to blit as the first row
	WORD	SecondMinX;  // the left edge of the second column
	WORD	SecondMinY;  // the top edge of the second column
	WORD	SecondSizeX; // the width of the second column
	WORD	SecondSizeY; // the height of the second column
	WORD	Pos;         // used as starting position in the "exponential" blit
	WORD	Size;        // used as bitmap size in the "exponential" blit
	
	
		/// The width of the first tile, this is either the rest of the tile right to SrcOffsetX
		/// or the width of the dest rect, if the rect is narrow
	
	FirstSizeX = MIN( SrcSizeX - SrcOffsetX, RECTSIZEX( DstBounds ) );
	
		/// The start for the second tile (if used)
	
	SecondMinX = DstBounds->MinX + FirstSizeX;
	
		/// The width of the second tile (we want the whole tile to be SrcSizeX pixels wide, if we
		/// use SrcSizeX-SrcOffsetX pixels for the left part we'll use SrcOffsetX for the right part)
	
	SecondSizeX = MIN( SrcOffsetX,DstBounds->MaxX-SecondMinX+1);
	
		/// The same values are calculated for y direction
	
	FirstSizeY = MIN( SrcSizeY - SrcOffsetY, RECTSIZEY( DstBounds ) );
	
	SecondMinY = DstBounds->MinY + FirstSizeY;
	
	SecondSizeY = MIN( SrcOffsetY, DstBounds->MaxY - SecondMinY + 1 );
	
		/// Blit the first piece of the tile
	
	BltBitMap( Src, SrcOffsetX, SrcOffsetY, Dst, DstBounds->MinX, DstBounds->MinY, FirstSizeX, FirstSizeY, 0xC0, -1, NULL );
	
		/// If SrcOffset was 0 or the dest rect was to narrow, we won't need a second column
	
	if( SecondSizeX > 0 )
		BltBitMap( Src, 0, SrcOffsetY, Dst, SecondMinX, DstBounds->MinY, SecondSizeX, FirstSizeY, 0xC0, -1, NULL );
	
		/// Is a second row necessary?
	
	if( SecondSizeY > 0 )
		{
		BltBitMap( Src, SrcOffsetX, 0, Dst, DstBounds->MinX, SecondMinY, FirstSizeX, SecondSizeY, 0xC0, -1, NULL );
		
		if( SecondSizeX>0)
			BltBitMap(Src,0,0,Dst,SecondMinX,SecondMinY,SecondSizeX,SecondSizeY,0xC0,-1,NULL);
		}
	
		/// This loop generates the first row of the tiles
	
	for( Pos = DstBounds->MinX + SrcSizeX, Size = MIN( SrcSizeX, DstBounds->MaxX - Pos + 1); Pos <= DstBounds->MaxX; )
		{
		BltBitMap( Dst, DstBounds->MinX, DstBounds->MinY, Dst, Pos, DstBounds->MinY, Size, MIN( SrcSizeY, RECTSIZEY( DstBounds ) ), 0xC0, -1, NULL );
		Pos += Size;
		Size = MIN( Size << 1, DstBounds->MaxX - Pos + 1 );
		}
	
		/// This loop blit the first row down several times to fill the whole dest rect
	
	for (Pos = DstBounds->MinY+SrcSizeY,Size = MIN( SrcSizeY,DstBounds->MaxY-Pos+1);Pos<=DstBounds->MaxY;)
		{
		BltBitMap(Dst,DstBounds->MinX,DstBounds->MinY,Dst,DstBounds->MinX,Pos,RECTSIZEX(DstBounds),Size,0xC0,-1,NULL);
		Pos += Size;
		Size = MIN( Size<<1,DstBounds->MaxY-Pos+1);
		}
}

/*
 * This function must be compiled without stack checking
 */
VOID __saveds __asm WindowPatternBackFillFunc( register __a0 struct Hook *Hook,register __a2 struct RastPort *RP, register __a1 struct BackFillMsg *BFM )
{
	struct BackFillInfo *	BFI = (struct BackFillInfo *)Hook;	// get the data for our backfillhook (but __saveds is nonetheless required because this function needs GfxBase)
	struct RastPort			cRP = {0};
	WORD					OffsetX;							// the offset within the tile in x direction
	WORD					OffsetY;							// the offset within the tile in y direction
	
	
	if( BFI->PictureObject )
		{
		cRP = *RP;					// copy the rastport
//		cRP.Layer = NULL;			// eliminate bogus clipping from our copy
	
		OffsetX = BFM->Bounds.MinX - BFI->Options.OffsetX;		// The first tile normally isn't totally visible => calculate the offset (offset 0 would mean that the left edge of the damage rectangle coincides with the left edge of a tile)
		
		if( BFI->Options.CenterX) // horizontal centering?
			OffsetX -= (BFI->Screen->Width-BFI->BitMapHeader->bmh_Width)/2;
	
		OffsetY = BFM->Bounds.MinY-BFI->Options.OffsetY; // The same values are calculated for y direction
		
		if( BFI->Options.OffsetTitleY ) // shift the tiles down?
			OffsetY -= BFI->Screen->BarHeight+1;
		
		if( BFI->Options.CenterY) // horizontal centering?
			OffsetY -= (BFI->Screen->Height-BFI->BitMapHeader->bmh_Height)/2;
	
		CopyTiledBitMap(BFI->BitMap,MOD(OffsetX,BFI->BitMapHeader->bmh_Width),MOD(OffsetY,BFI->BitMapHeader->bmh_Height),BFI->CopyWidth,BFI->CopyHeight,cRP.BitMap,&BFM->Bounds);
		}
}

/*
 * This function calculates the sizes to be used for the copy of the bitmap
 */
VOID CalculateCopySizes( struct BackFillInfo *BFI )
{
	BFI->CopyWidth = (BFI->BitMapHeader->bmh_Width>BFI->Options.MaxCopyWidth) ? BFI->BitMapHeader->bmh_Width : BFI->Options.MaxCopyWidth-BFI->Options.MaxCopyWidth%BFI->BitMapHeader->bmh_Width;
	BFI->CopyHeight = (BFI->BitMapHeader->bmh_Height>BFI->Options.MaxCopyHeight) ? BFI->BitMapHeader->bmh_Height : BFI->Options.MaxCopyHeight-BFI->Options.MaxCopyHeight%BFI->BitMapHeader->bmh_Height;
}

BOOL LoadBackgroundImage(struct BackFillInfo *BFI,STRPTR PicName,struct Screen *Scr,struct BackFillOptions *BFO)
{
	if( DataTypesBase )
		{
		memset( BFI, 0, sizeof( *BFI ) );
		
		BFI->Hook.h_Entry = (ULONG (*)())WindowPatternBackFillFunc;
		BFI->Screen = Scr;
		BFI->Options = *BFO;
		
		if( BFI->PictureObject = NewDTObject( PicName,
			DTA_SourceType,			DTST_FILE,
			DTA_GroupID,			GID_PICTURE,
			PDTA_Remap,				TRUE,
			PDTA_Screen,			Scr,
			PDTA_FreeSourceBitMap,	TRUE,
			OBP_Precision,			PRECISION_IMAGE,
			TAG_DONE ) )
			{
			if( DoMethod( BFI->PictureObject, DTM_PROCLAYOUT, NULL, 1 ) )
				{
				struct BitMap *	BitMap;
				
				GetDTAttrs( BFI->PictureObject, PDTA_BitMapHeader, &BFI->BitMapHeader, TAG_DONE );
				GetDTAttrs( BFI->PictureObject, PDTA_DestBitMap, &BitMap, TAG_DONE );
				
				if( !BitMap )
					GetDTAttrs( BFI->PictureObject, PDTA_BitMap, &BitMap, TAG_DONE );
				
				if( BitMap )
					{
					LONG	Depth = GetBitMapAttr( BitMap, BMA_DEPTH );
					
					
					CalculateCopySizes( BFI );
					
						/// The bitmap is copied to a friend bitmap of the screen to speed up
						/// rendering with gfxboards (no c2p/p2c!); this additionally allows to
						/// use this copy as a "picture cache", i.e. if the picture is 2x2 the
						/// copy can be made 16x16 (with 64 tiles) or whatever to speed up rendering
					
					if( BFI->BitMap = AllocBitMap( BFI->CopyWidth, BFI->CopyHeight, Depth, Depth == 8 ? BMF_MINPLANES : 0L, Scr->RastPort.BitMap ) )
						{
						struct Rectangle CopyBounds;
						CopyBounds.MinX = 0;
						CopyBounds.MinY = 0;
						CopyBounds.MaxX = BFI->CopyWidth - 1;
						CopyBounds.MaxY = BFI->CopyHeight - 1;
						
							/// Blit the tiles to the bitmap
						
						CopyTiledBitMap( BitMap, 0, 0, BFI->BitMapHeader->bmh_Width, BFI->BitMapHeader->bmh_Height, BFI->BitMap, &CopyBounds );
						
						return( TRUE );
						}
					}
				}
			}
		UnloadBackgroundImage( BFI );
		}
	
	return( FALSE );
}

VOID UnloadBackgroundImage( struct BackFillInfo *BFI )
{
	if( DataTypesBase )
		{
		WaitBlit();
		
			/// both frees work with NULL
		
		FreeBitMap( BFI->BitMap );
		
			/// datatype object is freed here, because - although a different bitmap is used for
			/// blitting to the screen - the pens have to stay allocated
		
		DisposeDTObject( BFI->PictureObject );
		
		memset( BFI, 0, sizeof(*BFI) );
		}
}
