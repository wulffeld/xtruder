/*
**	$VER: XtruderMiscellaneous.c (7.8.96)
**
**	Xtruder miscellaneous functions
**
**	Copyright (C) 1994-1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xDisableWindow( struct Window *diswin )
{
	if( diswin )
		{
		xEnableWindow( diswin );
		
		InitRequester( &NullReq );
		
		if( Request( &NullReq, diswin ) )
			{
			if( IntuitionBase->lib_Version < 39 )
				SetPointer( diswin, WaitPtr, 16, 16, -6, 0 );
			else
				SetWindowPointer( diswin,
					WA_BusyPointer, TRUE,
					WA_PointerDelay, TRUE,
					TAG_DONE );
			}
		}
}

VOID xEnableWindow( struct Window *enawin )
{
	if( enawin )
		{
		EndRequest( &NullReq, enawin );
		
		ActivateWindow( enawin );
		
		if( IntuitionBase->lib_Version < 39 )
			ClearPointer( enawin );
		else
			SetWindowPointer( enawin,
				TAG_DONE );
		}
}

/*
 * Similar to EasyRequestArgs but now that we already use ReqTools let's
 * use the functions provided there.
 */
LONG __stdargs xReqMsg( struct Window *win, STRPTR title, STRPTR textformat, STRPTR gadgetformat, ... )
{
	UBYTE	strbuf[ DEFAULTBUFFERLEN ];
	
	
	RawDoFmt( textformat, (APTR)(&gadgetformat + 1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
	
	return( (LONG)rtEZRequestTags( strbuf, gadgetformat, NULL, NULL,
		RT_Window,		win,
		RT_Underscore,	'_',
		RT_TextAttr,	&RequesterFontAttr,
		RTEZ_ReqTitle,	title,
		TAG_END ) );
}

/*
 * Return either a string from the opened catalog file or default english
 */
STRPTR xGetCatalogStr( LONG stringnum )
{
	STRPTR	builtin = NULL;
	UWORD	num;
	
	
	for( num = 0; AS[ num ].as_ID; num++ )
		{
		if( AS[ num ].as_ID == stringnum )
			{
			builtin = AS[ num ].as_Str;
			
			break;
			}
		}
	
	return( builtin );
}

VOID __stdargs xSPrintF( STRPTR buffer, STRPTR format, ... )
{
	RawDoFmt( format, (APTR)(&format+1), (VOID (*)())"\x16\xc0\x4E\x75", buffer );
}

VOID xSendXtruderMessage( UWORD command, STRPTR string )
{
	struct MsgPort *	xport;
	struct MsgPort *	replyport;
	struct XtruderMsg *	xmsg;
	
	
	if( xmsg = AllocVec( sizeof( struct XtruderMsg ), MEMF_CLEAR ) )
		{
		if( replyport = CreateMsgPort() )
			{
			xmsg->xt_Msg.mn_ReplyPort = replyport;
			xmsg->xt_Command = XTCMD_KILL;
			
			Forbid();
			
			if( xport = FindPort( "XTRUDER.1" ) )
				PutMsg( xport, (struct Message *)xmsg );
			
			Permit();
			
			if( xport )
				{
				WaitPort( replyport );
				
				GetMsg( replyport );
				}
			
			DeleteMsgPort( replyport );
			}
		FreeVec( xmsg );
		}
}

/*
 * EditHook for cursor down actions
 */
ULONG __interrupt __asm __saveds xCursorDownPatchFunc( register __a0 struct Hook *hookptr, register __a2 struct SGWork *sgw, register __a1 ULONG *msg )
{
	if( *msg == SGH_KEY )
		{
		switch( sgw->IEvent->ie_Code )
			{
			case CURSORDOWN:
				sgw->Actions |= SGA_USE | SGA_END;
				sgw->Code = HOOK_CURSORDOWN;
				break;
			}
		}
	else
		return( 0 );

	return( (ULONG)~0 );
}
