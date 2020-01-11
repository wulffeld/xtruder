/*
**	$VER: XtruderPopUpMenuClass.c (18.11.94)
**
**	Xtruder boopsi gadget
**
**	Copyright (C) 1994 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
** Class STATIC Protos for real Method functions and for class dispatcher.
*/
ULONG __saveds ASM PUMG_Dispatcher( REGA0 Class *, REGA2 Object *, REGA1 Msg );

ULONG	PUMG_NEW( Class *cl, Object *o, struct opSet *ops );
ULONG	PUMG_DISPOSE( Class *cl, Object *o, Msg msg );
ULONG	PUMG_SET( Class *cl, Object *o, struct opSet *ops );
ULONG	PUMG_GET( Class *cl, Object *o, struct opGet *opg );
ULONG	PUMG_UPDATE( Class *cl, Object *o, struct opUpdate *opu );
ULONG	PUMG_NOTIFY( Class *cl, Object *o, struct opUpdate *opu );
ULONG	PUMG_RENDER( Class *cl, Object *o, struct gpRender *gpr );
ULONG	PUMG_GOACTIVE( Class *cl, Object *o, struct gpInput *gpi );
ULONG	PUMG_HANDLEINPUT( Class *cl, Object *o, struct gpInput *gpi );
ULONG	PUMG_GOINACTIVE( Class *cl, Object *o, struct gpGoInactive *gpgi );

/* Protos for help functions. */

VOID	PUMG_MakeCheckings( struct PUMGData *PD );
VOID	PUMG_GetGadgetRect( Class *cl, Object *o, struct GadgetInfo *gi, struct Rectangle *rect );
VOID	PUMG_DrawPopupWindow( struct PUMGData *PD, struct DrawInfo *dri, ULONG From, LONG Count );
VOID	PUMG_DrawFrame( struct Window *win, int order, struct DrawInfo *dri, UBYTE *name, struct TextFont *tf, BOOL Active, BOOL NewLook, ULONG ItemHeight );
VOID	PUMG_DrawPopupMenuImage( struct RastPort *RP, ULONG Pen1, ULONG Pen2, ULONG Left, ULONG Top, LONG Height );

/*
** Global Create PopUpMenuClass function
*/
Class *xInitPopUpMenuGadgetClass()
{
	Class	*cl;
	
	if( cl = MakeClass( NULL, GADGETCLASS, NULL, sizeof(struct PUMGData), 0 ) )
		cl->cl_Dispatcher.h_Entry = (HookFunction)PUMG_Dispatcher;
	
	return( cl );
}

ULONG __saveds ASM PUMG_Dispatcher( REGA0 Class *cl, REGA2 Object *o, REGA1 Msg msg )
{
	ULONG	retval;
	
	
	switch( msg->MethodID )
		{
		case OM_NEW:
			retval = PUMG_NEW(cl, o, (struct opSet *)msg );
			break;
		
		case OM_DISPOSE:
			retval = PUMG_DISPOSE( cl, o, msg );
			break;
		
		case OM_SET:
			retval = PUMG_SET(cl, o, (struct opSet *)msg );
			break;
		
		case OM_GET:
			retval = PUMG_GET( cl, o, (struct opGet *)msg );
			break;
		
		case OM_UPDATE:
			retval = PUMG_UPDATE( cl, o, (struct opUpdate *)msg );
			break;
		
		case OM_NOTIFY:
			retval = PUMG_NOTIFY( cl, o, (struct opUpdate *)msg );
			break;
		
		case GM_RENDER:
			retval = PUMG_RENDER( cl, o, (struct gpRender *)msg );
			break;
		
		case GM_GOACTIVE:
			retval = PUMG_GOACTIVE( cl, o, (struct gpInput *)msg );
			break;
		
		case GM_HANDLEINPUT:
			retval = PUMG_HANDLEINPUT( cl, o, (struct gpInput *)msg );
			break;
		
		case GM_GOINACTIVE:
			retval = PUMG_GOINACTIVE( cl, o, (struct gpGoInactive *)msg );
			break;
		
		default:
			retval = DoSuperMethodA(cl, o, msg);
			break;
		}
	
	return( retval );
}

ULONG PUMG_NEW( Class *cl, Object *o, struct opSet *ops )
{
	struct PUMGData *	PD;
	Object *			object;
	
	
	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	
	if( object )
		{
		PD = INST_DATA( cl, object );
		
		PD->pd_Labels    = (struct List *)GetTagData( PUMG_Labels, NULL, ops->ops_AttrList );
		PD->pd_Header    = (STRPTR)GetTagData( PUMG_Header, (ULONG)"", ops->ops_AttrList );
		PD->pd_Active    = GetTagData( PUMG_Active, 0, ops->ops_AttrList );
		PD->pd_NewLook   = (BOOL)GetTagData( PUMG_NewLook, FALSE, ops->ops_AttrList );
		PD->pd_MenuWidth = (UWORD)GetTagData( PUMG_MenuWidth, 100, ops->ops_AttrList );
		
		PUMG_MakeCheckings( PD );
		
		PD->Font = (struct TextFont *)GetTagData( PUMG_TextFont, NULL, ops->ops_AttrList);
		
		PD->FrameImage = (struct Image *)NewObject( NULL,"frameiclass",
												IA_Recessed,    FALSE,
												IA_EdgesOnly,   FALSE,
												IA_FrameType,   FRAME_BUTTON,
												TAG_END );
		
		if( PD->FrameImage == NULL )
			{
			CoerceMethod(cl, o, OM_DISPOSE);
			
			object = NULL;
			}
		}
	
	return( (ULONG)object );
}

ULONG PUMG_DISPOSE( Class *cl, Object *o, Msg msg )
{
	struct	PUMGData	*PD = INST_DATA( cl, o );
	
	
	if( PD->popup_window )
		CloseWindow( PD->popup_window );
	
	if( PD->FrameImage )
		DisposeObject( PD->FrameImage );
	
	return( DoSuperMethodA(cl, o, msg) );
}

ULONG PUMG_SET( Class *cl, Object *o, struct opSet *ops )
{
	struct	PUMGData	*PD = INST_DATA( cl, o );
	struct	TagItem		*tag, notify;
	ULONG				retval;
	UWORD				old_active;
	
	
	retval = DoSuperMethodA( cl, o, (Msg)ops );
	
	/* I decided that it would be best that the values which are
	** specific to this class, could bot be changed when we have
	** our PopUpMenu window opened.
	*/
	if( (ops->ops_AttrList != NULL) && (PD->popup_window == NULL) )
		{
		tag = FindTagItem( PUMG_Labels, ops->ops_AttrList );
		
		if( tag )
			{
			PD->pd_Labels = (struct List *)tag->ti_Data;
			
			retval = TRUE;
			}
		
		old_active = PD->pd_Active;
		
		tag = FindTagItem( PUMG_Active, ops->ops_AttrList );
		
		if( tag )
			{
			PD->pd_Active = tag->ti_Data;
			
			retval = TRUE;
			}
		
		PUMG_MakeCheckings( PD );
		
		tag = FindTagItem( PUMG_TextFont, ops->ops_AttrList );
		
		if( tag )
			{
			PD->Font = (struct TextFont *)tag->ti_Data;
			
			retval = TRUE;
			}
		
		if( old_active != PD->pd_Active )
			{
			/* We send ourselves a OM_NOTIFY message, which will
			** eventually be broadcasted as OM_UPDATE message
			** to the target object. Note that we don't send it
			** simply to our parent, but to ourselves, so if
			** we have a children which needs to add it's own
			** data it will be added.
			*/
			PUMG_SetTagArg( notify, TAG_END, NULL );
			
			(VOID)DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );
			}
		}
	
	return( retval );
}

ULONG PUMG_GET( Class *cl, Object *o, struct opGet *opg )
{
	ULONG retval;
	struct	PUMGData *PD = INST_DATA( cl, o );
	
	
	switch( opg->opg_AttrID )
		{
		case PUMG_Labels:
			*opg->opg_Storage = (ULONG)PD->pd_Labels;
			retval = TRUE;
			break;
		
		case PUMG_Active:
			*opg->opg_Storage = (ULONG)PD->pd_Active;
			retval = TRUE;
			break;
		
		default:
			retval = DoSuperMethodA(cl, o, (Msg)opg);
			break;
		}
	
	return( retval );
}

ULONG PUMG_UPDATE( Class *cl, Object *o, struct opUpdate *opu )
{
	struct	PUMGData	*PD = INST_DATA( cl, o );
	struct	TagItem		*tag, notify;
	struct	RastPort	*rp;
	ULONG				retval;
	
	
	retval = DoSuperMethodA( cl, o, (Msg)opu );
	
		/// Update only if gadget isn't currently manipulated
	
	if( PD->popup_window == NULL )
		{
		if( opu->opu_AttrList )
			{
			tag = FindTagItem( PUMG_Active, opu->opu_AttrList );
			
			if( tag )
				{
				if( tag->ti_Data != PD->pd_Active )
					{
					PD->pd_Active = tag->ti_Data;
					
					PUMG_MakeCheckings( PD );
					
					rp = ObtainGIRPort( opu->opu_GInfo );
					
					if( rp )
						{
						DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
						ReleaseGIRPort( rp );
						}
					
						/// Notify the change
					
					PUMG_SetTagArg( notify, TAG_END, NULL );
					
					(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
					}
				}
			}
		}
	
	return( retval );
}

ULONG PUMG_NOTIFY( Class *cl, Object *o, struct opUpdate *opu )
{
	struct	TagItem		tags[3];
	struct	PUMGData	*PD = INST_DATA( cl, o );
	
	
	PUMG_SetTagArg( tags[0], GA_ID, ((struct Gadget *)o)->GadgetID );
	PUMG_SetTagArg( tags[1], PUMG_Active, PD->pd_Active );
	
	/* If there are no previous tags in OM_NOTIFY message, we
	** add them there as only ones. Otherwise we tag previous
	** tags to the end of our tags. Got it? :')
	*/
	if( opu->opu_AttrList == NULL )
		{
		PUMG_SetTagArg( tags[2], TAG_END, NULL );
		}
	else
		PUMG_SetTagArg( tags[2], TAG_MORE, opu->opu_AttrList );
	
	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG PUMG_RENDER( Class *cl, Object *o, struct gpRender *gpr )
{
	struct PUMGData *	PD = INST_DATA( cl, o );
	struct Gadget *		gad = (struct Gadget *)o;
	struct Rectangle	rect;
	struct DrawInfo *	dri;
	struct IBox			container;
	struct TextExtent	temp_te;
	struct RastPort *	RP = gpr->gpr_RPort;
	ULONG				retval, State;
	UWORD				BorderWidth, BorderHeight, TextWidth;
	UWORD				patterndata[2] = { 0x2222, 0x8888 };
	ULONG				TextPen, ImagePen1, ImagePen2;
	
	
	retval = DoSuperMethodA( cl, o, (Msg)gpr );
	
		/// Get real Min and Max positions
	
	PUMG_GetGadgetRect( cl, o, gpr->gpr_GInfo, &rect );
	
		/// Calculate real dimensions
	
	container.Left = rect.MinX;
	container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;
	
	dri = gpr->gpr_GInfo->gi_DrInfo;
	
	if( gad->Flags & GFLG_DISABLED )
		State = IDS_DISABLED;
	else
	if( gad->Flags & GFLG_SELECTED )
		State = IDS_SELECTED;
	else
		State = IDS_NORMAL;
	
		/// Frame rendering goes here
	
	SetAttrs( PD->FrameImage,
			IA_Left,	container.Left,
			IA_Top,		container.Top,
			IA_Width,	container.Width,
			IA_Height,	container.Height,
			TAG_END);
	
	DrawImageState( RP, PD->FrameImage, 0, 0, State, dri);
	
	if( dri )
		{
		TextPen = dri->dri_Pens[TEXTPEN];
		ImagePen1 = dri->dri_Pens[SHINEPEN];
		ImagePen2 = dri->dri_Pens[SHADOWPEN];
		}
	else
		{
		/*
		** If for some unknown reason Drawinfo is NULL then we
		** Use these predefined values, which should work atleast
		** for current OS releases.
		*/
		TextPen = ImagePen2 = 1;
		ImagePen1 = 2;
		}
	
		/// Draw the PopupMenu Image
	
	PUMG_DrawPopupMenuImage( RP, ImagePen1, ImagePen2,
		5 + container.Left, 2 + container.Top, -5 + container.Height);
	
	/*******************************/
	/* Text rendering starts here. */
	/*******************************/
	
		/// Do we have a proper font
	
	if( PD->Font == NULL )
		{
			/// If not we use the font we have in RastPort
		
		PD->Font = RP->Font;
		}
	else
		SetFont( RP, PD->Font );
	
		/// Check if we have nothing to print
	
	if( PD->pd_Count > 0 )
		{
		UBYTE	tempchar = 0,tempchar2;
		ULONG	len;
		UBYTE *	strbuf;
		
		
			/// This is what is shown IN the popupmenu gadgets header
		
		if( strbuf = AllocVec( strlen( PD->pd_Header ) + 1, MEMF_CLEAR ) )
			{
			UBYTE	tempstr[80];
			UBYTE	tempstr2[80];
			UWORD	x,y;
			
			
			strncpy( tempstr, PD->pd_Header, 80 );
			
			while( (tempstr[tempchar] != '_') && (tempstr[tempchar] != '\0') )
				tempchar++;
			
			if( tempstr[tempchar] != '\0' )
				{
				tempchar2 = tempchar;
				
				do
					{
					tempstr[tempchar2] = tempstr[tempchar2 + 1];
					
					tempchar2++;
					}
				while( tempstr[tempchar2] );
				}
			
				/// Remove '_' character
			
			xStrUcpy( strbuf, PD->pd_Header );
			
			len = TextFit( RP, strbuf, strlen( strbuf ), &temp_te, NULL, 1, container.Width - 28, 1 + PD->Font->tf_YSize );
			
			TextWidth = 1 + temp_te.te_Extent.MaxX - temp_te.te_Extent.MinX;
			
			SetAPen( RP, TextPen );
			
			x = 10 + container.Left + (container.Width - TextWidth)/2 - temp_te.te_Extent.MinX;
			y = PD->Font->tf_Baseline + (1 + container.Top + rect.MaxY - PD->Font->tf_YSize)/2;
			
			Move( RP, x, y );
			Text( RP, strbuf, len );
			
				/// Draw underscore '_'
			
			if( tempchar < strlen( PD->pd_Header ) )
				{
				WORD	c = -1;
				
				
					/// Copy string until underscore
				
				do
					{
					c++;
					tempstr2[c] = PD->pd_Header[c];
					tempstr2[c + 1] = '\0';
					}
				while( (tempstr2[c] != '_') && (tempstr2[c] != '\0') );
				
					/// And simply draw on top of the header text
				
				SetDrMd( RP, JAM1 );
				Move( RP, x, y + 1 );
				Text( RP, tempstr2, strlen( tempstr2 ) );
				}
			
			FreeVec( strbuf );
			}
		}
	
		/// Disabled pattern rendering is here
	
	if( State == IDS_DISABLED )
		{
		BorderHeight = 1;
		BorderWidth = (IntuitionBase->lib_Version < 39) ? 1 : 2;
		
		container.Left += BorderWidth;
		container.Top += BorderHeight;
		container.Width = __builtin_max( 1, container.Width - 2 * BorderWidth );
		container.Height = __builtin_max( 1, container.Height - 2 * BorderHeight );
		
		SetDrMd( RP, JAM1 );
		SetAfPt( RP, patterndata, 1 );
		
		RectFill(RP,  (LONG)container.Left, (LONG)container.Top,
				-1 + container.Left + container.Width,
				-1 + container.Top + container.Height );
		
		SetAfPt(RP, NULL, 0 );
		}
	
		/// Copy current Rectangle
	
	PD->rect = rect;
	
	return( retval );
}

ULONG PUMG_GOACTIVE( Class *cl, Object *o, struct gpInput *gpi )
{
	struct RastPort *	rp;
	struct PUMGData *	PD = INST_DATA( cl, o );
	struct GadgetInfo *	gi = gpi->gpi_GInfo;
	struct Gadget *		gad = (struct Gadget *)o;
	ULONG				retval = GMR_MEACTIVE, Left, Top;
	
	
		/// Test if we are disabled
	
	if( gad->Flags & GFLG_DISABLED )
		return( GMR_NOREUSE );
	
		/// Call first our parent class
	
	DoSuperMethodA( cl, o, (Msg)gpi );
	
		/// Chech whether we were activated from mouse or keyboard
	
	PD->ActiveFromMouse = (gpi->gpi_IEvent != NULL);
	
		/// Select this gadget
	
	gad->Flags |= GFLG_SELECTED;
	
	/* We make sure that NewLook isn't active if DrawInfo version
	** isn't high enough. Since V39 DrawInfo version >= 2!!
	*/
	if( PD->pd_NewLook )
		if( gi->gi_DrInfo && (gi->gi_DrInfo->dri_Version < 2))
			PD->pd_NewLook = FALSE;
	
	rp = ObtainGIRPort( gi );
	
	if( rp )
		{
			/// Render ourselves as selected gadget
		
		DoMethod( o, GM_RENDER, gi, rp, GREDRAW_UPDATE );
		
		ReleaseGIRPort( rp );
		
			/// Get the domain top/left position
		
		Left = gi->gi_Domain.Left;
		Top = gi->gi_Domain.Top;
		
			/// If this is window, we have to add window Left/Top values too
		
		if( gi->gi_Window )
			{
			Left += gi->gi_Window->LeftEdge;
			Top += gi->gi_Window->TopEdge;
			}
		
			/// If NewLook is ON, then size of one item is smaller
		
		PD->ItemHeight = PD->Font->tf_YSize + (PD->pd_NewLook ? 1 : 5);
		
			/// Count how many items fits to menu
		
		PD->FitsItems = (gi->gi_Screen->Height - 4) / PD->ItemHeight;
		
		if( PD->FitsItems > PD->pd_Count )
			PD->FitsItems = PD->pd_Count;
		
		PD->popup_window = OpenWindowTags(NULL,
							WA_Left,            Left + PD->rect.MinX,
							WA_Top,             Top + PD->rect.MaxY,
							WA_Width,           PD->pd_MenuWidth,
							WA_Height,          4 + PD->FitsItems*PD->ItemHeight,
							WA_Activate,        FALSE,
							WA_CustomScreen,    gi->gi_Screen,
							WA_SizeGadget,      FALSE,
							WA_DragBar,         FALSE,
							WA_DepthGadget,     FALSE,
							WA_CloseGadget,     FALSE,
							WA_Borderless,      TRUE,
							WA_Flags,           0,
							WA_AutoAdjust,      TRUE,
							WA_RMBTrap,         TRUE,
							WA_SimpleRefresh,   TRUE,
							WA_NoCareRefresh,   TRUE,
							TAG_END );
		
		if( PD->popup_window == NULL )
			retval = GMR_NOREUSE;
		else
			{
				/// We make sure Active item isn't too large to display
			
			if( PD->FitsItems < PD->pd_Active )
				PD->pd_Active = PD->FitsItems-1;
			
			/*
			** If activated from keyboard we can set temporary value
			** to currently activated item. Otherwise we set it
			** to -1 which means that there is no active item.
			*/
			PD->Temp_Active = PD->ActiveFromMouse ? (ULONG)~0 : PD->pd_Active;
			
				/// Render all items
			
			PUMG_DrawPopupWindow( PD, gi->gi_DrInfo, 0, -1);
			}
		}
	else
		retval = GMR_NOREUSE;
	
	return( retval );
}

ULONG PUMG_HANDLEINPUT( Class *cl, Object *o, struct gpInput *gpi )
{
	struct	GadgetInfo	*gi = gpi->gpi_GInfo;
	struct	InputEvent	*ie = gpi->gpi_IEvent;
	struct	PUMGData	*PD = INST_DATA(cl, o);
	struct	PUMGNode	*pnode;
	ULONG				retval = GMR_MEACTIVE;
	WORD				X, Y;
	WORD				count, old_active, newactive;
	struct	TagItem		tags;	/* If our possible child class, doesn't know
								** how to handle NULL AttrList ptr, then
								** this can save lots of crashes.
								*/
	
	/* If there is anykind of AutoPoint program then our main window
	** might get inactive and we wouldn't get any more messages.
	** So we check out that we are active and deactivate ourselves
	** if our window isn't active anymore.
	*/
	if( gi->gi_Window )
		if( (gi->gi_Window->Flags & WFLG_WINDOWACTIVE) == 0 )
			return( GMR_NOREUSE );
	
	if( PD->ActiveFromMouse )
		{
		X = PD->popup_window->MouseX;
		Y = PD->popup_window->MouseY;
		
		count = ( (Y - 2) >= 0 ) ? (Y - 2) / (PD->ItemHeight) : ~0;
		
		old_active = PD->Temp_Active;
		
		if( (X > 2) && (X < (PD->popup_window->Width - 2)) && (count >= 0) && (count < PD->FitsItems) )
			PD->Temp_Active = (UWORD)count;
		else
			PD->Temp_Active = (UWORD)~0;
		
		if( old_active != (WORD)PD->Temp_Active )
			{
			PUMG_DrawPopupWindow( PD, gi->gi_DrInfo,(ULONG)PD->Temp_Active, 1 );
			PUMG_DrawPopupWindow( PD, gi->gi_DrInfo,(ULONG)old_active, 1 );
			}
		
		while( ie && (retval == GMR_MEACTIVE) )
			{
			if( ie->ie_Class == IECLASS_RAWMOUSE )
				{
				if( ie->ie_Code == SELECTUP )
					{
					retval = GMR_NOREUSE;
					
					if( (PD->Temp_Active != (UWORD)~0) )
						{
						PD->pd_Active = PD->Temp_Active;
						
						PUMG_MakeCheckings( PD );
						
						PUMG_SetTagArg(tags, TAG_END, NULL);
						
						(VOID)DoMethod( o, OM_NOTIFY, &tags, gi, 0);
						
						retval |= GMR_VERIFY;
						
						*gpi->gpi_Termination = (ULONG)PD->pd_Active;
						}
					}
				}
			
			ie = ie->ie_NextEvent;
			}
		}
	else
		{
		while( ie && (retval == GMR_MEACTIVE) )
			{
			switch( ie->ie_Class )
				{
				case IECLASS_RAWMOUSE:
					if( ie->ie_Code != IECODE_NOBUTTON )
						retval = GMR_REUSE; /* Reuse the InputEvent. */
					break;
				
				case IECLASS_RAWKEY:
					old_active = PD->Temp_Active;
					
					switch( ie->ie_Code )
						{
						case CURSORDOWN:
							if( ie->ie_Qualifier & (ALTLEFT | ALTRIGHT) )
								PD->Temp_Active = PD->FitsItems-1;  /* Jump to end. */
							else
							if( PD->Temp_Active < (PD->FitsItems-1) )
								PD->Temp_Active += 1;
							break;
						
						case CURSORUP:
							if( ie->ie_Qualifier & (ALTLEFT | ALTRIGHT) )
								PD->Temp_Active = 0;    /* Jump to start. */
							else
							if( PD->Temp_Active > 0 )
								PD->Temp_Active -= 1;
							break;
						
						case 0x45:  /* ESC key. */
							retval = GMR_NOREUSE;
							break;
						
						case 0x44:  /* RETURN key. */
							PD->pd_Active = PD->Temp_Active;
							PUMG_MakeCheckings( PD );
							
							PUMG_SetTagArg( tags, TAG_END, NULL );
							(VOID)DoMethod( o, OM_NOTIFY, &tags, gi, 0);
							
							retval = GMR_NOREUSE | GMR_VERIFY;
							*gpi->gpi_Termination = (ULONG)PD->pd_Active;
							break;
						
						default:
							for( pnode = (struct PUMGNode *)PD->pd_Labels->lh_Head, newactive = 0;
								 pnode->pum_Node.ln_Succ;
								 pnode = (struct PUMGNode *)pnode->pum_Node.ln_Succ )
								{
								if( ie->ie_Code == pnode->pum_RawkeyID )
									{
									PD->pd_Active = newactive;
									PUMG_MakeCheckings( PD );
									
									PUMG_SetTagArg( tags, TAG_END, NULL );
									(VOID)DoMethod( o, OM_NOTIFY, &tags, gi, 0);
									
									retval = GMR_NOREUSE | GMR_VERIFY;
									*gpi->gpi_Termination = (ULONG)PD->pd_Active;
									}
								
								newactive++;
								}
							break;
						}
					
						/// Update the popupwindow items, if changes were made
					
					if( old_active != PD->Temp_Active )
						{
						PUMG_DrawPopupWindow( PD, gi->gi_DrInfo, (ULONG)PD->Temp_Active, 1 );
						PUMG_DrawPopupWindow( PD, gi->gi_DrInfo, (ULONG)old_active, 1 );
						}
				}
			
			ie = ie->ie_NextEvent;
			}
		}
	
	return( retval );
}

ULONG PUMG_GOINACTIVE( Class *cl, Object *o, struct gpGoInactive *gpgi )
{
	struct	RastPort	*rp;
	struct	PUMGData	*PD = INST_DATA(cl, o);
	ULONG				retval;
	
	
	retval = DoSuperMethodA( cl, o, (Msg)gpgi );
	
	((struct Gadget *)o)->Flags &= ~GFLG_SELECTED;
	
	rp = ObtainGIRPort( gpgi->gpgi_GInfo );
	
	if( rp )
		{
		DoMethod( o, GM_RENDER, gpgi->gpgi_GInfo, rp, GREDRAW_UPDATE );
		
		ReleaseGIRPort( rp );
		}
	
	if( PD->popup_window )
		{
		CloseWindow(PD->popup_window);
		
		PD->popup_window = NULL;
		}
	
	return( retval );
}

/*
** Static functions for help with real Method functions
*/
VOID PUMG_MakeCheckings( struct	PUMGData *PD )
{
	struct Node *node;
	
	
	PD->pd_Count = 0;
	
	if( PD->pd_Labels == NULL )
		PD->pd_Active = 0;
	else
	if( PD->pd_Labels != (struct List *)~0 )
		{
		node = (struct Node *)PD->pd_Labels->lh_Head;
		
		while( node->ln_Succ )
			{
			PD->pd_Count += 1;
			
			node = node->ln_Succ;
			}
		
		if( PD->pd_Active >= PD->pd_Count )
			PD->pd_Active = PD->pd_Count + (PD->pd_Count == 0) - 1;
		}
}

VOID PUMG_GetGadgetRect( Class *cl, Object *o, struct GadgetInfo *gi, struct Rectangle *rect )
{
	struct Gadget *		gad = (struct Gadget *)o;
	LONG				W, H;
	
	
	rect->MinX = gad->LeftEdge;
	rect->MinY = gad->TopEdge;
	W = gad->Width;
	H = gad->Height;
	
	if( gi )
		{
		if( gad->Flags & GFLG_RELRIGHT )
			rect->MinX += gi->gi_Domain.Width - 1;
		
		if( gad->Flags & GFLG_RELBOTTOM )
			rect->MinY += gi->gi_Domain.Height - 1;
		
		if( gad->Flags & GFLG_RELWIDTH )
			W += gi->gi_Domain.Width;
		
		if( gad->Flags & GFLG_RELHEIGHT )
			H += gi->gi_Domain.Height;
		}
	
	rect->MaxX = rect->MinX + W - (W > 0);
	rect->MaxY = rect->MinY + H - (H > 0);
}

VOID PUMG_DrawPopupWindow( struct PUMGData *PD, struct DrawInfo *dri, ULONG From, LONG Count )
{
	struct	Node		*node;
	struct	Window		*win = PD->popup_window;
	struct	RastPort	*RP = win->RPort;
	WORD				i, End;
	
	
	if( PD->pd_Count && dri )
		{
			/// If we want to draw all entries then we draw
			/// window borders too.
		
		if( Count == -1)
			{
			Count = PD->FitsItems;
			
			if( PD->pd_NewLook )
				{
					/// Set background to MENU background color
				
				SetRast( RP, (ULONG)dri->dri_Pens[BARBLOCKPEN] );
				
				SetAPen( RP, (ULONG)dri->dri_Pens[BARDETAILPEN] );
				
				Move( RP, 0, -1 + win->Height );
				Draw( RP, 0, 0 );
				Draw( RP, -1 + win->Width, 0 );
				Draw( RP, -1 + win->Width, -1 + win->Height);
				Draw( RP, 1, -1 + win->Height);
				Draw( RP, 1, 1);
				Move( RP, -2 + win->Width, 1 );
				Draw( RP, -2 + win->Width, -2 + win->Height);
				}
			else
				{
				SetAPen( RP, (ULONG)dri->dri_Pens[SHINEPEN]);
				
				Move( RP, 0, -1L + win->Height);
				Draw( RP, 0, 0 );
				Draw( RP, -1L + win->Width, 0 );
				
				SetAPen( RP, (ULONG)dri->dri_Pens[SHADOWPEN]);
				
				Draw( RP, -1L + win->Width, -1L + win->Height);
				Draw( RP, 1, -1L + win->Height);
				}
			}
		
		SetFont( RP, PD->Font );
		SetDrMd( RP, JAM1);
		
		node = PD->pd_Labels->lh_Head;
		
		for( i = 0, End = From + Count; node->ln_Succ ; i++ )
			{
			if( i < PD->FitsItems )
				{
				if( (i >= From) && ( i < End ) )
					{
					PUMG_DrawFrame( PD->popup_window, i, dri, node->ln_Name,
						PD->Font, (BOOL)(i == PD->Temp_Active),
						PD->pd_NewLook, (ULONG)PD->ItemHeight );
					}
				else
					if( i >= End ) return;
				}
			
			node = node->ln_Succ;
			}
		}
}

VOID PUMG_DrawFrame( struct Window *win, int order, struct DrawInfo *dri, UBYTE *name, struct TextFont *tf, BOOL Active, BOOL NewLook, ULONG ItemHeight )
{
	struct RastPort *	RP = win->RPort;
	struct TextExtent	temp_te;
	ULONG				Pen1, Pen2, TextPen, BPen;
	ULONG				Top, Width, Bottom, MaxX, Len, TextWidth;
	ULONG				font_height = tf->tf_YSize;
	UBYTE				tempchar = 0,tempchar2;
	UBYTE				tempstr[80];
	UBYTE				tempstr2[80];
	
	
	strncpy( tempstr, name, 80 );
	
	while( (tempstr[tempchar] != '_') && (tempstr[tempchar] != '\0') )
		tempchar++;
	
	if( tempstr[tempchar] != '\0' )
		{
		tempchar2 = tempchar;
		
		do
			{
			tempstr[tempchar2] = tempstr[tempchar2 + 1];
			
			tempchar2++;
			}
		while( tempstr[tempchar2] );
		}
	
	TextPen = dri->dri_Pens[TEXTPEN];
	
	if( Active )
		{
		if( NewLook )
			{
			BPen = dri->dri_Pens[BARDETAILPEN];
			TextPen = dri->dri_Pens[BARBLOCKPEN]; /* Override previous value. */
			}
		else
			{
			Pen2 = dri->dri_Pens[SHINEPEN];
			Pen1 = dri->dri_Pens[SHADOWPEN];
			BPen = dri->dri_Pens[FILLPEN];
			}
		}
	else
		{
		if( NewLook )
			{
			BPen = dri->dri_Pens[BARBLOCKPEN];
			TextPen = dri->dri_Pens[BARDETAILPEN];  /* Override previous value. */
			}
		else
			{
			Pen2 = dri->dri_Pens[SHADOWPEN];
			Pen1 = dri->dri_Pens[SHINEPEN];
			BPen = dri->dri_Pens[BACKGROUNDPEN];
			}
		}
	
	Top = 2 + order * ItemHeight;
	Bottom = Top + ItemHeight - 1;
	MaxX = win->Width - 4;
	
	SetAPen( RP, BPen );
	
	RectFill( RP, 4, Top, MaxX - 1, Bottom );
	
		/// Draw Recessed Border
	
	if( NewLook == FALSE )
		{
		SetAPen( RP, Pen1 );
		
		Move( RP, 3, Bottom );
		Draw( RP, 3, Top );
		Draw( RP, MaxX, Top );
		
		SetAPen( RP, Pen2 );
		
		Draw( RP, MaxX, Bottom );
		Draw( RP, 4, Bottom );
		}
	
	SetAPen( RP, TextPen );
	
	Width = win->Width - 10;
	
	Len = TextFit( RP, tempstr, strlen(tempstr), &temp_te, NULL, 1, Width, 1 + font_height );
	
	TextWidth = temp_te.te_Extent.MaxX - temp_te.te_Extent.MinX;
	
	Move( RP, 5 + temp_te.te_Extent.MinX, (ItemHeight - font_height)/2 + 1 + Top + tf->tf_Baseline );
	Text( RP, tempstr, Len );
	
		/// Create underscore
	
	if( tempchar < strlen( name ) )
		{
		UBYTE	txt[2] = {0,0};
		UWORD	x,y;
		ULONG	underscorewidth;
		ULONG	tempstrwidth;
		
		
			/// Copy string until underscore
		
		strncpy( tempstr2, tempstr, tempchar + 1 );
		
		txt[0] = tempstr2[ tempchar ];
		
			/// Length in pixels for underscore
		
		TextFit( RP, txt, strlen(txt), &temp_te, NULL, 1, Width, 1 + font_height );
		
		underscorewidth = temp_te.te_Extent.MaxX - temp_te.te_Extent.MinX;
		
		tempstr[ tempchar ] = 0;
		
		TextFit( RP, tempstr, strlen( tempstr ), &temp_te, NULL, 1, Width, 1 + font_height );
		
		tempstrwidth = temp_te.te_Extent.MaxX - temp_te.te_Extent.MinX;
		
		x = 5 + temp_te.te_Extent.MinX + tempstrwidth;
		y = (ItemHeight - font_height)/2 + Top + tf->tf_YSize;
		
		Move( RP, x, y );
		Draw( RP, x + underscorewidth, y );
		}
}

VOID PUMG_DrawPopupMenuImage( struct RastPort *RP, ULONG Pen1, ULONG Pen2, ULONG Left, ULONG Top, LONG Height )
{
	ULONG count, i;
	
	
	if( Height > 0 )
		{
		SetAPen( RP, Pen1 );
		
		Move(RP, Left, Top + Height - 1 );
		Draw(RP, Left, Top );
		
		count = (Height-1) / 4;
		
		for( i = 0; i <= count; i++ )
			{
			Move(RP, Left, Top + i*4);
			Draw(RP, 10 + Left, Top + i*4);
			}
		
			/// Draw other horizontal bar
		
		Move( RP, 16 + Left, Top);
		Draw( RP, 16 + Left, Top + Height);
		
		SetAPen( RP, Pen2 );
		
		Move( RP, 1 + Left, Top + Height - 1);
		Draw( RP, 11 + Left, Top + Height - 1);
		Draw( RP, 11 + Left, Top);
		
		if( Height >= 4 )
			{
			count = (Height-4) / 4;
			
			for( i = 0; i <= count; i++ )
				{
				Move( RP, 1 + Left, 3 + Top + i*4);
				Draw( RP, 11 + Left, 3 + Top + i*4);
				}
			}
		
			/// Draw other horizontal bar
		
		Move( RP, 15 + Left, Top);
		Draw( RP, 15 + Left, Top + Height );
	}
}
