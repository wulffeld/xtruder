BOOL xReadSectors( struct IOExtTD *disk, ULONG length, ULONG offset, UBYTE *secbuffer )
{
	disk->iotd_Req.io_Length	= (ULONG)length;
	disk->iotd_Req.io_Data		= (APTR)secbuffer;
	disk->iotd_Req.io_Command	= CMD_READ;
	disk->iotd_Req.io_Offset	= (ULONG)(offset);
	DoIO( (struct IORequest *)disk );
	
	if( disk->iotd_Req.io_Error )
		{
		xTDError( disk->iotd_Req.io_Error );
		
		return( FALSE );
		}
	
	return( TRUE );
}

BOOL xWriteSectors( struct IOExtTD *disk, ULONG length, ULONG offset, UBYTE *secbuffer )
{
	disk->iotd_Req.io_Length	= (ULONG)length;
	disk->iotd_Req.io_Data		= (APTR)secbuffer;
	disk->iotd_Req.io_Command	= CMD_WRITE;
	disk->iotd_Req.io_Offset	= (ULONG)(offset);
	DoIO( (struct IORequest *)disk );
	
	if( disk->iotd_Req.io_Error )
		{
		xTDError( disk->iotd_Req.io_Error );
		
		return( FALSE );
		}
	
	return( TRUE );
}

VOID xMotorOff( struct IOExtTD *disk )
{
	disk->iotd_Req.io_Length		= 0;
	disk->iotd_Req.io_Command		= TD_MOTOR;
	DoIO( (struct IORequest *)disk );
}

VOID xMotorOn( struct IOExtTD *disk )
{
	disk->iotd_Req.io_Length		= 1;
	disk->iotd_Req.io_Command		= TD_MOTOR;
	DoIO( (struct IORequest *)disk );
}

BOOL xDiskStatus( LONG unit )
{
	DiskUnit[unit]->iotd_Req.io_Flags   = IOF_QUICK;
	DiskUnit[unit]->iotd_Req.io_Command = TD_PROTSTATUS;
	BeginIO( (struct IORequest *)DiskUnit[unit] );
	
	if( DiskUnit[unit]->iotd_Req.io_Error )
		xTDError( DiskUnit[unit]->iotd_Req.io_Error );
	else
		if( !DiskUnit[unit]->iotd_Req.io_Actual )
			return( TRUE );
	
	return( FALSE );
}

