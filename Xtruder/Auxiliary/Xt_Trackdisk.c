/*
**	$VER: XtruderTrack.c (17.8.96)
**
**	Xtruder functions for track use
**
**	Copyright (C) 1994-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Get available units (drives)
 */
VOID xGetTrackUnits( VOID )
{
	UBYTE	a;
	
	
	for( a = 0; a < 4; a++ )
		{
		if( DiskPort[a] = CreateMsgPort() )
			{
			if( DiskUnit[a] = (struct IOExtTD *)CreateIORequest( DiskPort[a], sizeof(struct IOExtTD)) )
				{
				if( !OpenDevice( "trackdisk.device", a, (struct IORequest *)DiskUnit[a], 0 ) )
					DrvUnit[a] = TRUE;
				else
					{
					DeleteMsgPort( DiskPort[a] );
					DeleteIORequest( (struct IORequest *)DiskUnit[a] );
					}
				}
			else
				DeleteMsgPort( DiskPort[a] );
			}
		}
}

VOID xFreeTrackUnits( VOID )
{
	UBYTE	a;
	
	
	for( a = 0; a < 4; a++ )
		{
		if( DrvUnit[a] )
			{
			CloseDevice( (struct IORequest *)DiskUnit[a] );
			DeleteIORequest( (struct IORequest *)DiskUnit[a] );
			DeleteMsgPort( DiskPort[a] );
			}
		}
}

BOOL xIsDiskInDrive( LONG unit )
{
	DiskUnit[unit]->iotd_Req.io_Flags   = IOF_QUICK;
	DiskUnit[unit]->iotd_Req.io_Command = TD_CHANGESTATE;
	BeginIO( (struct IORequest *)DiskUnit[unit] );
	
	if( DiskUnit[unit]->iotd_Req.io_Error )
		xTDError( DiskUnit[unit]->iotd_Req.io_Error );
	else
		if( !DiskUnit[unit]->iotd_Req.io_Actual )
			return( TRUE );
	
	return( FALSE );
}

VOID xFindPresentDisks( VOID )
{
	UBYTE	h;
	
	
	for( h = 0; h < 4; h++ )
		if( DrvUnit[h] )
			DiskPre[h] = xIsDiskInDrive( (LONG)h );
}

VOID xTDError( LONG error )
{
	switch( error )
		{
		case TDERR_WriteProt:
			xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_DISKWRITEPROTECTED ), GCS( MSG_GF_OKAY ) );
			break;
		
		case TDERR_DiskChanged:
			xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_NODISKINDRIVE ), GCS( MSG_GF_OKAY ) );
			break;
		
		case TDERR_BadDriveType:
			xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_INVALIDDRIVE ), GCS( MSG_GF_OKAY ) );
			break;
		
		case TDERR_BadUnitNum:
			xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_DRIVEDOESNOTEXIST ), GCS( MSG_GF_OKAY ) );
			break;
		
		case TDERR_DriveInUse:
			xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_DRIVEBUSY ), GCS( MSG_GF_OKAY ) );
			break;
		}
}
