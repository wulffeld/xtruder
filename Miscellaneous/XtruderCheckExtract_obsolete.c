
/*
 * Starts check on an extracted archive. The archive has been extracted with
 * the use of internal 1 routine.
 *
 * strbuf1+2 = miscellaneous
 * strbuf3 = ParsePatternNoCase()
 */
VOID xCHK_CheckExtractedArchive( struct CurrentCheck *cc, struct CurrentFile *cf )
{
	struct CurrentFile *	archivecf;
	struct AnchorPath *		ap;
	UBYTE					strbuf1[ xMAXSTRLEN ];
	UBYTE					strbuf2[ xMAXSTRLEN ];
	UBYTE					strbuf3[ xMAXSTRLEN ];
	UWORD					a;
	UWORD					oldindent = Indent;
	
	
	if( ap = AllocVec( sizeof( struct AnchorPath ) + 256, MEMF_PUBLIC | MEMF_CLEAR ) )
		{
		ap->ap_Strlen = 256;
		
		xParsePatternNoCase( XSettings->xs_CHE_ArchiveFilter, strbuf3 );
		
			/// Go through this path/file/pattern...
		
		if( !MatchFirst( XSettings->xs_PAT_ArchiveExtractPath, ap ) )
			{
				/// Put while() here cuz else we'll write Unpack which
				/// isn't our intention...
			
			ap->ap_Flags |= APF_DODIR;
			ap->ap_Flags &= ~APF_DIDDIR;
			
			while( !MatchNext( ap ) && cc->cc_Result != CCRES_USERBREAK )
				{
				switch( xCheckControlWinMsg( cc ) )
					{
					case CONTROL_STOP:
						MatchEnd( ap );
						
						FreeVec( ap );
						
						cc->cc_Result = CCRES_USERBREAK;
						
						return;
						break;
					
					case CONTROL_NOTHING:
							/// If it's a file...
						
						if( ap->ap_Info.fib_DirEntryType < 0 )
							{
							BPTR	oldcd = CurrentDir( ap->ap_Current->an_Lock );
							
							
							cc->cc_FilesEncountered++;
							
							if( NameFromLock( ap->ap_Current->an_Lock, strbuf1, xMAXSTRLEN ) )
								{
								if( MatchPatternNoCase( strbuf3, ap->ap_Buf ) )
									{
										/// Check if filename is valid
									
									if( strcmp( FilePart( ap->ap_Buf ), "*" ) )
										{
											/// First character must not be a '-'
										
										if( (FilePart( ap->ap_Buf ))[0] != '-' )
											{
												/// Name must also not contain a '"'
											
											if( !stpchr( FilePart( ap->ap_Buf ), '"' ) )
												{
												if( archivecf = AllocVec( sizeof( struct CurrentFile ), MEMF_ANY | MEMF_CLEAR ) )
													{
													strcpy( archivecf->cf_PathName, strbuf1 );
													
													xCHK_ATxt( XSettings->xs_HIS_RecPathCheck, TRUE, ap->ap_Info.fib_FileName,
														FI_Size,		ap->ap_Info.fib_Size,
														FI_Protection,	ap->ap_Info.fib_Protection,
														XSettings->xs_CHE_WrtInfo_Date ? FI_Date : TAG_IGNORE, XSettings->xs_CHE_WrtInfo_Date ? xFormatFileDate( &ap->ap_Info, strbuf1 ) : 0,
														XSettings->xs_CHE_WrtInfo_Filetype ? FI_Filetype : TAG_IGNORE, XSettings->xs_CHE_WrtInfo_Filetype ? xFileType( ap->ap_Info.fib_FileName, strbuf2 ) : 0,
														TAG_END );
													
														/// "Reset" protection bits
													
													SetProtection( ap->ap_Buf, 0 );
													
														/// Read protection removed or not
														/// present...
													
													if( strncpy( archivecf->cf_FileName, ap->ap_Info.fib_FileName, xMAXSTRLEN ) )
														{
														strncpy( archivecf->cf_Comment, ap->ap_Info.fib_Comment, 80 );
														archivecf->cf_Size = ap->ap_Info.fib_Size;
														archivecf->cf_Result = 0;
														
														xCHK_DoExtractedFile( cc, cf, archivecf );
														}
													
													FreeVec( archivecf );
													}
												}
											}
										}
									}
								}
							CurrentDir( oldcd );
							}
						else
							{
								/// Enter subdirectory?
							
							if( XSettings->xs_CHE_EnterRecursively == TRUE )
								{
								if( ap->ap_Flags & APF_DIDDIR )
									{
										/// We are finished with that dir...
									
									Indent -= XSettings->xs_GEN_Indention;
									
									if( Indent > 65000 )
										Indent = 0;
									
									INDENT[ Indent ] = 0;
									}
								else
									{
									strcpy( strbuf1, PathPart( ap->ap_Buf ) );
									strcpy( strbuf2, strbuf1 );
									
									if( strbuf1[0] == '/' )
										{
										for( a = 0; a < strlen( strbuf1 ); a++ )
											strbuf2[a] = strbuf1[a+1];
										
										strbuf2[a] = 0;
										}
									
									xATxt( XSettings->xs_HIS_RecPathCheck, 2, "%s>> %s", INDENT, strbuf2 );
									
									Indent += XSettings->xs_GEN_Indention;
									
									for( a = 0; a < Indent; a++ )
										INDENT[ a ] = ' ';
									
									INDENT[ a ] = '\0';
									
									cc->cc_DirsEncountered++;
									ap->ap_Flags |= APF_DODIR;
									}
								
								ap->ap_Flags &= ~APF_DIDDIR;
								}
							}
						break;
					}
				}
			}
		
		MatchEnd( ap );
		
		Indent = oldindent;
		
		for( a = 0; a < Indent; a++ )
			INDENT[ a ] = ' ';
		
		INDENT[ a ] = '\0';
		
		FreeVec( ap );
		}
}
