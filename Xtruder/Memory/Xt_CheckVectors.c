/*
**	$VER: XtruderCheckVectors.c (27.8.96)
**
**	Xtruder vector functions
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xCheckVectors( VOID )
{
	if( SysBase->CoolCapture || SysBase->ColdCapture || SysBase->WarmCapture || SysBase->KickTagPtr || SysBase->KickMemPtr )
		{
		if( xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_MEM_VECTORSNOTOKAY ), GCS( MSG_GF_YESNO ) ) )
			xClearAllVectors();
		}
	else
		xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_MEM_VECTORSOKAY ), GCS( MSG_GF_OKAY ) );
}

VOID xClearAllVectors( VOID )
{
	SysBase->CoolCapture = 0;
	SysBase->ColdCapture = 0;
	SysBase->WarmCapture = 0;
	SysBase->KickTagPtr = 0;
	SysBase->KickMemPtr = 0;
}
