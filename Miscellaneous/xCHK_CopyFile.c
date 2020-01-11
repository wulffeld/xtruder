/*
 * Copy a file bit by bit to decrunch path...
 */
BOOL xCHK_CopyFile( struct CurrentFile *cf )
{
	BPTR	file,dfile;
	UBYTE *	buffer;
	LONG	s;
	BOOL	success = FALSE;
	UBYTE	strbuf[ xMAXSTRLEN ];
	
	
	if( buffer = AllocVec( 65535, MEMF_ANY ) )
		{
		if( strncpy( strbuf, XPrefs->xp_PAT_ExeDecrunchPath, xMAXPATHLEN ) )
			{
			if( AddPart( strbuf, FilePart( cf->cf_FileName ), xMAXSTRLEN ) )
				{
				if( file = Open( cf->cf_FileName, MODE_OLDFILE ) )
					{
					if( dfile = Open( strbuf, MODE_NEWFILE ) )
						{
						while( 1 )
							{
							s = Read( file, buffer, 65535 );
							
								/// Read() Error
							
							if( s == -1 )
								{
								xATxt( XPrefs->xp_HIS_RecPathCheck, 3, GCS( MSG_COULDNTCOPYFILETO ), FilePart( cf->cf_FileName ), XPrefs->xp_PAT_ExeDecrunchPath );
								
								break;
								}
							else
							if( s )
								{
								if( Write( dfile, buffer, s ) == -1 )
									{
									xATxt( XPrefs->xp_HIS_RecPathCheck, 3, GCS( MSG_COULDNTCOPYFILETO ), FilePart( cf->cf_FileName ), XPrefs->xp_PAT_ExeDecrunchPath );
									
									break;
									}
								}
							else
								{
								success = TRUE;
								
								break;
								}
							}
						Close( dfile );
						}
					Close( file );
					}
				}
			}
		FreeVec( buffer );
		}
	else
		xATxt( XPrefs->xp_HIS_RecPathCheck, 3, GCS( MSG_NOTENOUGHMEMORY ) );
	
	return( success );
}
