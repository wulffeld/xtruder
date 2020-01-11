/*
**	$VER: XtruderBoopsi.c (23.11.94)
**
**	Xtruder boopsi related stuff
**
**	Copyright (C) 1994 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
** Create a button class
*/
Class *xInitButtonGadgetClass( void )
{
	Class *cl;
	
	
	if( cl = MakeClass( NULL, "gadgetclass", NULL, sizeof(struct ButtonData), 0) )
		cl->cl_Dispatcher.h_Entry = (ULONG(*)())xDispatchButtonGadget;
	
	return( cl );
}

/*
** Take care of actions happening to button gadget
*/
ULONG ASM __saveds xDispatchButtonGadget( REGA0 Class *cl, REGA2 Object *o, REGA1 Msg msg )
{
	struct	Gadget		*g = (struct Gadget *)o;
	struct	gpInput		*gpi = (struct gpInput *)msg;
	struct	InputEvent	*ie;
	struct	RastPort	*rp;
	struct	ButtonData	*inst;
	struct	TagItem		*ti;
	struct	opGet		*opget;
	ULONG				retval;
	Object				*object;
	
	
	switch( msg->MethodID )
		{
		case OM_NEW:
			if( object = (Object *)DoSuperMethodA(cl, o, msg) )
				{
				ti = ((struct opSet *)msg)->ops_AttrList;
				
				inst = INST_DATA(cl, object);
				inst->bd_LastState = 0;
				inst->bd_Otherunderscore	= (BOOL)GetTagData( BUT_OtherUnderscore, FALSE, ti );
				inst->bd_Text				= (STRPTR)GetTagData( BUT_Text, NULL, ti );
				inst->bd_Textcolor			= (UWORD)GetTagData( BUT_TextColor, 1, ti );
				inst->bd_Backcolor			= (UWORD)GetTagData( BUT_BackColor, 0, ti );
				inst->bd_Selbackcolor		= (UWORD)GetTagData( BUT_SelectedBackColor, 3, ti );
				inst->bd_Image				= (struct Image *)GetTagData( BUT_Image, NULL, ti );
				inst->bd_SImage				= (struct Image *)GetTagData( BUT_SelectImage, NULL, ti );
				inst->bd_Type				= GetTagData( BUT_Type, 0, ti );
				inst->bd_TextFont			= (struct TextFont *)GetTagData( BUT_TextFont, 0, ti );
				}
			
			retval = (ULONG)object;
			break;
		
		case GM_HITTEST:
			retval = GMR_GADGETHIT;
			break;
		
		case GM_GOACTIVE:
			inst = INST_DATA(cl, o);
			
			if( gpi->gpi_IEvent && !(g->Flags & GFLG_DISABLED) )
				{
				g->Flags |= GFLG_SELECTED;
				
				xRenderButton( cl, g, (struct gpRender *)msg, inst, g->Flags & GFLG_SELECTED);
				
				retval =  GMR_MEACTIVE;
				}
			else
				retval = GMR_NOREUSE;
			break;
		
		case GM_RENDER:
			inst = INST_DATA(cl, o);
			
			retval = xRenderButton( cl, g, (struct gpRender *)msg, inst, g->Flags & GFLG_SELECTED);
			break;
		
		case GM_HANDLEINPUT:
			retval = GMR_MEACTIVE;
			
			ie = gpi->gpi_IEvent;
			
			while( ie )
			{
			switch( ie->ie_Class )
				{
				case IECLASS_RAWMOUSE:
					switch( ie->ie_Code )
						{
						case SELECTUP:
							if( (gpi->gpi_Mouse.X < 0 ) ||
								(gpi->gpi_Mouse.X > g->Width) ||
								(gpi->gpi_Mouse.Y < 0 ) ||
								(gpi->gpi_Mouse.Y > g->Height) )
								{
								retval = GMR_REUSE;
								}
							else
								{
								retval = GMR_NOREUSE | GMR_VERIFY;
								}
							break;
						
						case MENUDOWN:
							retval = GMR_NOREUSE;
							break;
						
						case IECODE_NOBUTTON:
							inst = INST_DATA(cl, o);
							
							if( (gpi->gpi_Mouse.X < 0 ) ||
								(gpi->gpi_Mouse.X > g->Width) ||
								(gpi->gpi_Mouse.Y < 0 ) ||
								(gpi->gpi_Mouse.Y > g->Height) )
								{
								if( inst->bd_LastState )
									{
									g->Flags &= ~GFLG_SELECTED;
									
									xRenderButton( cl, g, (struct gpRender *)msg, inst, FALSE );
									
									inst->bd_LastState = FALSE;
									}
								}
							else
								{
								if( !inst->bd_LastState )
									{
									g->Flags |= GFLG_SELECTED;
									
									xRenderButton( cl, g, (struct gpRender *)msg, inst, TRUE );
									
									inst->bd_LastState = TRUE;
									}
								}
							break;
						}
					break;
				}
			
			ie = ie->ie_NextEvent;
			}
			break;
		
		case GM_GOINACTIVE:
			g->Flags &= ~GFLG_SELECTED;
			
			inst = INST_DATA(cl, o);
			
			retval = xRenderButton( cl, g, (struct gpRender *)msg, inst, g->Flags & GFLG_SELECTED );
			break;
		
		case OM_SET:
			ti = ((struct opSet *)msg)->ops_AttrList;
			
			if( FindTagItem( GA_Width, ti ) ||
				FindTagItem( GA_Height, ti ) ||
				FindTagItem( GA_Top, ti ) ||
				FindTagItem( GA_Left, ti ) ||
				FindTagItem( BUT_OtherUnderscore, ti ) ||
				FindTagItem( BUT_Text,  ti ) ||
				FindTagItem( BUT_TextColor, ti ) ||
				FindTagItem( BUT_BackColor, ti ) ||
				FindTagItem( BUT_SelectedBackColor, ti ) ||
				FindTagItem( BUT_Image, ti ) ||
				FindTagItem( BUT_SelectImage, ti ) ||
				FindTagItem( BUT_Type, ti ) )
				{
				WORD	x,y,w,h;
				
				
				x = g->LeftEdge;
				y = g->TopEdge;
				w = g->Width;
				h = g->Height;
				
				retval = DoSuperMethodA(cl, o, msg);
				
				if( rp = ObtainGIRPort( ((struct opSet *)msg)->ops_GInfo) )
					{
					UWORD			*pens = ((struct opSet *)msg)->ops_GInfo->gi_DrInfo->dri_Pens;
					
					
					if( x != g->LeftEdge || y != g->TopEdge || w != g->Width || h != g->Height )
						{
						SetAPen(rp, pens[BACKGROUNDPEN]);
						
						SetDrMd(rp, JAM1);
						
						RectFill(rp, x, y, x+w, y+h);
						}
					
					inst = INST_DATA( cl, o );
					inst->bd_Otherunderscore	= (BOOL)GetTagData( BUT_OtherUnderscore, (BOOL)inst->bd_Otherunderscore, ti );
					inst->bd_Text				= (STRPTR)GetTagData( BUT_Text, (ULONG)inst->bd_Text, ti );
					inst->bd_Textcolor			= (UWORD)GetTagData( BUT_TextColor, inst->bd_Textcolor, ti );
					inst->bd_Backcolor			= (UWORD)GetTagData( BUT_BackColor, inst->bd_Backcolor, ti );
					inst->bd_Selbackcolor		= (UWORD)GetTagData( BUT_SelectedBackColor, inst->bd_Selbackcolor, ti );
					inst->bd_TextFont			= (struct TextFont *)GetTagData( BUT_TextFont, (ULONG)inst->bd_TextFont, ti );
					inst->bd_Image				= (struct Image *)GetTagData( BUT_Image, (ULONG)inst->bd_Image, ti );
					inst->bd_SImage				= (struct Image *)GetTagData( BUT_SelectImage, (ULONG)inst->bd_SImage, ti );
					inst->bd_Type				= (UWORD)GetTagData( BUT_Type, inst->bd_Type, ti );
					
					DoMethod( o, GM_RENDER, ((struct opSet *)msg)->ops_GInfo, rp, GREDRAW_REDRAW );
					
					ReleaseGIRPort( rp );
					}
				}
			else
			if( FindTagItem( GA_Selected, ti ) ||
				FindTagItem( GA_Disabled, ti ) )
				{
				/*
				 * GA_Selected and GA_Disabled need a refresh of the gadget.
				 * The parent class will set the selected or disabled bits
				 * of the gadget->Flags.
				 */
				retval = DoSuperMethodA(cl, o, msg);
				
				if( rp = ObtainGIRPort( ((struct opSet *)msg)->ops_GInfo) )
					{
					DoMethod(o, GM_RENDER, ((struct opSet *)msg)->ops_GInfo, rp, GREDRAW_REDRAW);
					
					ReleaseGIRPort(rp);
					}
				}
			else
				retval = DoSuperMethodA(cl, o, msg);
			break;
		
		case OM_GET:
			opget = (struct opGet *)msg;
			
			inst = INST_DATA(cl, o);
			
			retval = TRUE;
			
			switch( opget->opg_AttrID )
				{
				case BUT_OtherUnderscore:
					*(opget->opg_Storage) = (ULONG)inst->bd_Otherunderscore;
					break;
				
				case BUT_Text:
					*(opget->opg_Storage) = (ULONG)inst->bd_Text;
					break;
				
				case BUT_TextColor:
					*(opget->opg_Storage) = (ULONG)inst->bd_Textcolor;
					break;
				
				case BUT_BackColor:
					*(opget->opg_Storage) = (ULONG)inst->bd_Backcolor;
					break;
				
				case BUT_SelectedBackColor:
					*(opget->opg_Storage) = (ULONG)inst->bd_Selbackcolor;
					break;
				
				case BUT_Image:
					*(opget->opg_Storage) = (ULONG)inst->bd_Image;
					break;
				
				case BUT_SelectImage:
					*(opget->opg_Storage) = (ULONG)inst->bd_SImage;
					break;
				
				case BUT_Type:
					*(opget->opg_Storage) = (ULONG)inst->bd_Type;
					break;
				
				case BUT_TextFont:
					*(opget->opg_Storage) = (ULONG)inst->bd_TextFont;
					break;
				
				default:
					retval = DoSuperMethodA( cl, o, msg );
				}
			break;
		
		default:
			retval = DoSuperMethodA( cl, o, msg );
			break;
		}
	
	return( retval );
}

static WORD drawer_points[] =
{
	11,0,
	11,-6,	// [3]
	9,-8,	// [5] - 2
	6,-8,	// [7] 0
	4,-6,	// [9] + 2
	0,-6,	// [11] 0
	0,-1,
	1,-1,
	1,-5,	// [17] + 1
	5,-5,	// [19] 0
	6,-4,	// [21] + 1
	10,-4	// [23] 0  
};

static WORD file_points[] =
{
	11, 0,
	11,-4,	// [3]
	 7,-8,	// [5] - 4
	 7,-4,	// [7] + 4
	10,-4,	// [9]
	10, 0,
	 0, 0,
	 0,-8,	// [15]
	 1, 0,
	 1,-8,	// [19]
	 7,-8,	// [21]
};

static WORD arrow_points[] =
{
	 0, 0,
	-1, 0,
	-2,-1,
	 1,-1,
	 2,-2,
	-3,-2,
	-4,-3,
	 3,-3,
	 4,-4,
	-5,-4,
	 0,-4,
	 1,-4,
	 1,-7,	// [25]
	 0,-4,
	 0,-7,	// [29]
	-1,-4,
	-1,-7,	// [33]
	-2,-4,
	-2,-7,	// [37]
};

static WORD arrowdisk_points[] =
{
	 1, 0,
	11, 0,
};

VOID xDrawPoints( struct RastPort *rp, WORD b_x, WORD b_y, WORD n, WORD *pt)
{
	while( n-- )
		{
		Draw(rp, b_x + *pt, b_y + *(pt+1));
		
		pt += 2;
		}
}

/*
** Removes the underscore from a string and returns the position of it
*/
UWORD xStrUcpy( STRPTR dest, STRPTR src )
{
	UBYTE *start = src;
	UWORD uline = 0;
	
	
	while( *src )
		{
		if( *src == '_' )
			{
			uline = src - start + 1;
			
			src++;
			}
		else
			*dest++ = *src++;
		}
	
	return( uline );
}

ULONG xRenderButton( Class *cl, struct Gadget *g, struct gpRender *msg, struct ButtonData *inst, BOOL sel )
{
	struct	RastPort	*rp;
	struct	TextExtent	te = {0};
	ULONG				retval = TRUE;
	UWORD				*pens = msg->gpr_GInfo->gi_DrInfo->dri_Pens;
	UWORD				slen = 0;
	
	
	if( msg->MethodID == GM_RENDER )
		rp = msg->gpr_RPort;
	else
		rp = ObtainGIRPort(msg->gpr_GInfo);
	
	if( rp )
		{
		struct	TextFont *oldTextFont;
		UBYTE	back, shine, shadow, text;
		UWORD	x, y, w, h, text_x = 0, text_y = 0;
		UBYTE	*str = NULL;
		UWORD	underline = 0;
		UWORD	depth = 1 << rp->BitMap->Depth;
		UBYTE	oldAPen, oldDrMd;
		ULONG	state;
		
		
		oldAPen = rp->FgPen;
		oldDrMd = rp->DrawMode;
		oldTextFont = rp->Font;
		
		if( sel )
			{
				/// Defaults for selected gadget
			
			back	= pens[FILLPEN];
			shine	= pens[SHADOWPEN];
			shadow	= pens[SHINEPEN];
			text	= pens[FILLTEXTPEN];
			
			if( inst->bd_Selbackcolor != (UWORD)~0 )
				back = inst->bd_Selbackcolor;
			}
		else
			{
				/// Defaults
			
			back	= pens[FILLPEN];
			shine	= pens[SHINEPEN];
			shadow	= pens[SHADOWPEN];
			text	= pens[TEXTPEN];
			
			if( inst->bd_Backcolor != (UWORD)~0 )
				back = inst->bd_Backcolor;
			}
		
		if( inst->bd_Textcolor != (UWORD)~0 )
			text = inst->bd_Textcolor;
		
		if( (back % depth) == (text % depth) )
			{
			if( (back % depth) == 0 )
				text = 1;
			else
				text = 0;
			}
		
		x = g->LeftEdge;
		y = g->TopEdge;
		w = g->Width-1;
		h = g->Height-1;
		
		if( inst->bd_Text )
			{
			if( inst->bd_TextFont )
				SetFont(rp, inst->bd_TextFont);
			
			slen = strlen(inst->bd_Text);
			
			if( str = AllocVec(slen, MEMF_ANY) )
				{
					/// Remove possible underscore
				
				if( underline = xStrUcpy(str, inst->bd_Text) )
					slen--;
				
				TextExtent(rp, str, slen, &te);
				text_x = x + (w+1)/2 - te.te_Width/2;
				text_y = y + (h+1)/2 - (te.te_Extent.MaxY - te.te_Extent.MinY + 1)/2 - te.te_Extent.MinY;
				}
			}
		
			/// Draw full box
		
		SetDrMd(rp, JAM1);
		SetAPen(rp, back);
		RectFill( rp, x+2, y+1, x+w-2, y+h-1 );
		
		SetAPen(rp, shadow);
		RectFill(rp, x+1, y+h, x+w, y+h);
		RectFill(rp, x+w-1, y+1, x+w-1, y+h);
		RectFill(rp, x+w, y, x+w, y+h);
		
		if( str )
			{
			SetAPen( rp, text );
			Move( rp, text_x, text_y );
			Text( rp, str, slen );
			
			if( underline )
				{
				slen = TextLength(rp, str, underline - 1);
				
				text_x += slen;
				
				slen = TextLength(rp, str + underline - 1, 1);
				
					/// Draw underscore
				
				if( inst->bd_Otherunderscore )
					RectFill( rp, text_x, text_y + te.te_Extent.MaxY + 1,
								text_x + slen - 1, text_y + te.te_Extent.MaxY + 1 );
				else
					RectFill( rp, text_x, text_y + te.te_Extent.MaxY,
								text_x + slen - 1, text_y + te.te_Extent.MaxY );
				}
			}
		
		if( inst->bd_Image || inst->bd_SImage )
			{
			struct	Image *image;
			UWORD	image_x, image_y;
			
			
			if( g->Flags & GFLG_SELECTED )
				{
				if( inst->bd_SImage )
					{
					state = IDS_NORMAL;
					image = inst->bd_SImage;
					}
				else
					{
					state = IDS_SELECTED;
					image = inst->bd_Image;
					}
				}
			else
				{
				state = IDS_NORMAL;
				image = inst->bd_Image;
				}
			
			image_x = x + (w+1)/2 - image->Width/2;
			image_y = y + (h+1)/2 - image->Height/2;
			
			DrawImageState(rp, image, image_x, image_y, state, msg->gpr_GInfo->gi_DrInfo);
			}
		
		if( inst->bd_Type == BUTTYPE_DRAWER )
			{
			WORD	b_x, b_y;
			
			
			SetAPen( rp, text );
			
			b_x = x + (w+1)/2 - 6;
			b_y = y + (h+1) - 3;
			
			Move( rp, b_x, b_y );
			
				/// Adapt Y values to height of gadget
			
			drawer_points[3] = -(h - 6);
			drawer_points[5] = drawer_points[7] = drawer_points[3] - 2;
			drawer_points[9] = drawer_points[11] = drawer_points[7] + 2;
			drawer_points[17] = drawer_points[19] = drawer_points[11] + 1;
			drawer_points[21] = drawer_points[23] = drawer_points[19] + 1;
			
			xDrawPoints( rp, b_x, b_y, 12, drawer_points );
			}
		else
		if( inst->bd_Type == BUTTYPE_FILE )
			{
			WORD	b_x, b_y;
			
			
			SetAPen( rp, text );
			
			b_x = x + (w+1)/2 - 6;
			b_y = y + (h+1) - 3;
			
			Move( rp, b_x, b_y );
			
			/*
			** Adapt Y values to height of gadget
			*/
			file_points[9] = file_points[7] = file_points[3] = -(h - 8);
			file_points[21] = file_points[19] = file_points[15] = file_points[5] = file_points[3] - 4;
			
			xDrawPoints( rp, b_x, b_y, 11, file_points );
			}
		else
		if( inst->bd_Type == BUTTYPE_ARROW )
			{
			WORD	b_x, b_y;
			
			
			SetAPen( rp, text );
			
			b_x = x + (w+1)/2;
			b_y = y + (h+1) - 5;
			
			Move( rp, b_x, b_y );
			
			/*
			** Adapt Y values to height of gadget
			*/
			arrow_points[25] = arrow_points[29] = arrow_points[33] = arrow_points[37] = -(h - 6);
			
			xDrawPoints( rp, b_x, b_y, 19, arrow_points );
			
			b_x = x + (w+1)/2 - 6;
			b_y = y + (h+1) - 3;
			
			Move( rp, b_x, b_y );
			
			xDrawPoints( rp, b_x, b_y, 2, arrowdisk_points );
			}
		
		SetAPen(rp, shine);
		RectFill(rp, x, y, x+w-1, y);
		RectFill(rp, x, y+1, x, y+h);
		RectFill(rp, x+1, y+1, x+1, y+h-1);
		
		if( g->Flags & GFLG_DISABLED )
			{
			UWORD	area_pattern[] = {0x1111, 0x4444};
			UWORD	*oldAfPt;
			UBYTE	oldAfSize;
			
			
			oldAfPt = rp->AreaPtrn;
			oldAfSize = rp->AreaPtSz;
			
			SetAPen( rp, text );
			SetAfPt( rp, area_pattern, 1 );
			RectFill( rp, x, y, x+w, y+h );
			
			SetAfPt( rp, oldAfPt, oldAfSize );
			}
		
		SetAPen( rp, oldAPen );
		SetDrMd( rp, oldDrMd );
		SetFont( rp, oldTextFont );
		
		FreeVec( str );
		
		if( msg->MethodID != GM_RENDER )
			ReleaseGIRPort(rp);
		}
	else
		retval = FALSE;
	
	return( retval );
}
