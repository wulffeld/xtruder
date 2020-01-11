/*
**	$VER: XtruderSelectBootblock.c (24.10.95)
**
**	Xtruder select bootblock functions
**
**	Copyright (C) 1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Returns -1 for failure
 */
WORD xOpenBootblockSelectWindow( UWORD *coords, WORD unit )
{
	struct Window *		win;
	struct xGadget *	bbgads;
	ULONG				topedge = ScreenBarHeight + ScrFontYSize + 4;
	ULONG				txtlength;
	WORD				bbnr = -1;
	UWORD				id,g,msg;
	
	
	if( !Window )
		return( -1 );
	
	txtlength = 10 + xMaxStringLength( ScreenFont, MSG_BBT_SELECTBOOTBLOCK, MSG_BBT_CUSTOMSELECT );
	
	if( bbgads = AllocVec( 10 * sizeof( struct xGadget ), MEMF_CLEAR ) )
		{
		for( g = 0, id = GTID_BB_1XOFS, msg = MSG_BBT_1XOFS;
			 g < 10;
			 g++, id++, topedge += GadgetHeight + 2, msg++ )
			{
			if( bbgads[g].xg_Gad = NewObject( ButtonClass, NULL,
				GA_ID,			id,
				GA_Top,			topedge,
				GA_Left,		10,
				GA_Width,		txtlength + 20,
				GA_Height,		GadgetHeight,
				GA_RelVerify,	TRUE,
				(g > 0) ? GA_Previous : TAG_IGNORE, bbgads[g - 1].xg_Gad,
				BUT_Text,		GCS( msg ),
				BUT_TextFont,	ScreenFont,
				TAG_DONE ) )
				{
				bbgads[g].xg_GadgetText = GCS( msg );
				bbgads[g].xg_GadgetID = id;
				}
			else
				break;
			}
		
		if( bbgads[9].xg_Gad )
			{
			if( topedge + 4 <= Screen->Height )
				{
				for( g = 0; g < 9; g++ )
					bbgads[g].xg_NextGad = &bbgads[g+1];
				
				bbgads[g].xg_NextGad = NULL;
				
				if( win = OpenWindowTags( NULL,
					WA_Left,			coords[0],
					WA_Top,				coords[1],
					WA_Width,			10 + txtlength + 20 + 10,
					WA_Height,			topedge + 4,
					WA_IDCMP,			IDCMP_REFRESHWINDOW | IDCMP_RAWKEY | IDCMP_GADGETUP | IDCMP_CLOSEWINDOW,
					WA_Gadgets,			bbgads[0].xg_Gad,
					WA_CustomScreen,	Screen,
					WA_Title,			GCS( MSG_BBT_TL_BOOTBLOCK ),
					WA_DragBar,			TRUE,
					WA_Activate,		TRUE,
					WA_CloseGadget,		TRUE,
					WA_DepthGadget,		TRUE,
					WA_SmartRefresh,	TRUE,
					WA_RMBTrap,			TRUE,
					TAG_END ) )
					{
					GT_RefreshWindow( win, NULL );
					
					SetFont( win->RPort, ScreenFont );
					
					xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_BBT_BAR_SELECTBOOTBLOCKTYPE ) );
					
					bbnr = xHandleBootblockSelectIDCMP( win, bbgads );
					
						/// Remember coordinates
					
					coords[0] = win->LeftEdge;
					coords[1] = win->TopEdge;
					
					CloseWindow( win );
					}
				}
			else
				xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENWINDOW ), GCS( MSG_GF_OKAY ) );
			}
		
		for( g = 0; g < 10; g++ )
			DisposeObject( bbgads[g].xg_Gad );
		
		FreeVec( bbgads );
		}
	
	return( bbnr );
}

WORD xHandleBootblockSelectIDCMP( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					code,qualifier;
	BOOL					done = FALSE;
	
	
	while( !done )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			code = msg->Code;
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
						case KEY_ESCAPE:
							done = TRUE;
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( gads, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_BB_1XOFS:
									case GTID_BB_13FFS:
									case GTID_BB_20OFS:
									case GTID_BB_20FFS:
									case GTID_BB_20OFSINT:
									case GTID_BB_20FFSINT:
									case GTID_BB_30OFSCACHE:
									case GTID_BB_30FFSCACHE:
									case GTID_BB_CUSTOM:
									case GTID_BB_CUSTOMSELECT:
										xPressButton( win, tmpgad->xg_Gad );
										
										code = tmpgad->xg_GadgetID;
										break;
									}
								}
							break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_BB_1XOFS:
							return( BB_1XOFS );
							break;
						
						case GTID_BB_13FFS:
							return( BB_13FFS );
							break;
						
						case GTID_BB_20OFS:
							return( BB_20OFS );
							break;
						
						case GTID_BB_20FFS:
							return( BB_20FFS );
							break;
						
						case GTID_BB_20OFSINT:
							return( BB_20OFSINT );
							break;
						
						case GTID_BB_20FFSINT:
							return( BB_20FFSINT );
							break;
						
						case GTID_BB_30OFSCACHE:
							return( BB_30OFSCACHE );
							break;
						
						case GTID_BB_30FFSCACHE:
							return( BB_30FFSCACHE );
							break;
						
						case GTID_BB_CUSTOM:
							return( BB_CUSTOM );
							break;
						
						case GTID_BB_CUSTOMSELECT:
							return( BB_CUSTOMSELECT );
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					done = TRUE;
					break;
				}
			}
		}
	
	return( -1 );
}
