/*
**	$VER: Backfill.h (22.8.96)
**
**	Custom backfill stuff used in Xtruder
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/


	// this seems to be a bug (or feature) in SAS/C: -6%5 gives -1 instead of 4

#define MOD(x,y) ((x)<0 ? (y)-((-(x))%(y)) : (x)%(y))

#define RECTSIZEX(r) ((r)->MaxX-(r)->MinX+1)
#define RECTSIZEY(r) ((r)->MaxY-(r)->MinY+1)

struct BackFillMsg
{
	struct Layer    *Layer;
	struct Rectangle Bounds;
	LONG             OffsetX;
	LONG             OffsetY;
};

struct BackFillOptions
{
	WORD MaxCopyWidth;  // maximum width for the copy
	WORD MaxCopyHeight; // maximum height for the copy
	BOOL CenterX;       // center the tiles horizontally?
	BOOL CenterY;       // center the tiles vertically?
	WORD OffsetX;       // offset to add
	WORD OffsetY;       // offset to add
	BOOL OffsetTitleY;  // add the screen titlebar height to the vertical offset?
};

struct BackFillInfo
{
	struct Hook            Hook;
	struct Screen         *Screen;
	Object                *PictureObject;
	struct BitMapHeader   *BitMapHeader;
	struct BitMap         *BitMap;
	WORD                   CopyWidth;
	WORD                   CopyHeight;
	struct BackFillOptions Options;
};
