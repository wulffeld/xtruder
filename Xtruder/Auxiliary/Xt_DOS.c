/*
**	$VER: XtruderDOS.c (4.8.96)
**
**	Xtruder functions for DOS use
**
**	Copyright (C) 1994-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * If xtruderport != NULL send the args to old Xtruder
 */
struct XtruderArgs *xGetCLIArguments( struct MsgPort *xtruderport )
{
	struct XtruderArgs *	xtargs;
	
	
	if( xtargs = AllocVec( sizeof( struct XtruderArgs ), MEMF_CLEAR ) )
		{
		if( xtargs->xa_RArgs = ReadArgs( USAGE, xtargs->xa_Args, NULL ) )
			{
				/// Open GUI on a public screen?
			
			if( xtargs->xa_Args[ ARG_PUBSCREEN ] )
				strncpy( XSettings->xs_GUI_PublicScreenName, (STRPTR)xtargs->xa_Args[ ARG_PUBSCREEN ], 79 );
			
			return( xtargs );
			}
		
		FreeVec( xtargs );
		}
	
	return( NULL );
}

VOID xHandleCLIArguments( struct XtruderArgs *xtargs, struct MsgPort *mp )
{
	UWORD	exitcode = 0;
	
	
		/// Check a file/directory
	
	if( xtargs->xa_Args[ ARG_FILE ] )
		{
		struct List				cliargslist;
		UBYTE **				files;
		
		
		NewList( &cliargslist );
		
		files = (UBYTE **)xtargs->xa_Args[ ARG_FILE ];
		
		while( *files )
			{
			xAddNameNode( &cliargslist, *files );
			
			files++;
			}
		
			/// Check files
		
		exitcode = xStartCheck( &cliargslist, CHECKTYPE_CLIARGS, TRUE );
		}
	
		/// Check prespecified
	
	if( xtargs->xa_Args[ ARG_PRESPECIFIED ] )
		{
		exitcode += xStartCheck( NULL, CHECKTYPE_PRESPECIFIED, TRUE );
		}
	
		/// Check vectors
	
	if( xtargs->xa_Args[ ARG_VECTORS ] )
		xCheckLVO( TRUE );
	
	if( xtargs->xa_Args[ ARG_EXIT ] )
		{
		xFreeCLIArguments( xtargs );
		
		xCloseSystem( 0, exitcode, 0 );
		}
}

VOID xFreeCLIArguments( struct XtruderArgs *xtargs )
{
	if( xtargs )
		{
		FreeArgs( xtargs->xa_RArgs );
		
		FreeVec( xtargs );
		}
}

/*
 * Select a single file.
 *
 * Window pointer might not be valid!
 */
BOOL __stdargs xSelectFile( struct Window *win, ... )
{
	struct Gadget *	gad;
	STRPTR			tempstr;
	UBYTE			strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE			strbuf2[ DEFAULTBUFFERLEN ] = "";
	UBYTE			filename[ 110 ] = "";
	UWORD			len;
	BOOL			success = FALSE;
	BPTR			lock;
	va_list 		tags;
	
	
	va_start( tags, win );
	
	strcpy( filename, (STRPTR)GetTagData( SELFP_File, (ULONG)"", (struct TagItem *)tags ) );
	
		/// Set starting directory
	
	if( tempstr = (STRPTR)GetTagData( SELFP_Dir, (ULONG)"", (struct TagItem *)tags ) )
		rtChangeReqAttr( FileReq, RTFI_Dir, tempstr, TAG_END );
	
		/// If a gadget is specified then override SELFP_Dir and SELFP_File
	
	if( gad = (struct Gadget *)GetTagData( SELFP_Gadget, NULL, (struct TagItem *)tags ) )
		{
		strcpy( strbuf1, GETSTRING( gad ) );
		
			/// Must not be empty
		
		if( strbuf1[0] )
			{
				/// Strip %s and "%s" in the end
			
			if( GetTagData( SELFP_StripFormats, FALSE, (struct TagItem *)tags ) )
				{
				UBYTE	str[10] = {'"','%','s','"','\0'};
				
				
				len = strlen( strbuf1 );
				
				if( len > 4 )
					{
					if( !strncmp( &strbuf1[ len - 4 ], str, 4 ) )
						strbuf1[ len - 5 ] = '\0';
					else
					if( !strncmp( &strbuf1[ len - 2 ], "%s", 2 ) )
						strbuf1[ len - 3 ] = '\0';
					}
				else
				if( len > 2 )
					{
					if( !strncmp( &strbuf1[ len - 2 ], "%s", 2 ) )
						strbuf1[ len - 3 ] = '\0';
					}
				}
			
				/// Copy filename
			
			strcpy( filename, FilePart( strbuf1 ) );
			
				/// Isolate pathname from filename and set it as default
			
			if( xPathName( strbuf1, strbuf2, DEFAULTPATHLEN ) )
				rtChangeReqAttr( FileReq, RTFI_Dir, strbuf2, TAG_END );
			}
		else
			{
				/// Fallback and use SELFP_Dir
			
			if( tempstr = (STRPTR)GetTagData( SELFP_Dir, (ULONG)"", (struct TagItem *)tags ) )
				rtChangeReqAttr( FileReq, RTFI_Dir, tempstr, TAG_END );
			}
		}
	
		/// Request file
	
	if( rtFileRequest( FileReq, filename, (STRPTR)GetTagData( SELFP_ReqTitle, (ULONG)GCS( MSG_TL_SELECTFILE ), (struct TagItem *)tags ),
		RT_Window,		win,
		RT_Underscore,	'_',
		RT_TextAttr,	&RequesterFontAttr,
		RTFI_OkText,	GetTagData( SELFP_OkText, (ULONG)GCS( MSG_GF_SELECT ), (struct TagItem *)tags ),
		TAG_END ) )
		{
		success = TRUE;
		
			/// Store directory
		
		if( tempstr = (STRPTR)GetTagData( SELFP_DestDir, (ULONG)NULL, (struct TagItem *)tags ) )
			strncpy( tempstr, FileReq->Dir, DEFAULTPATHLEN );
		
			/// Store directory and concantenate filename
		
		if( tempstr = (STRPTR)GetTagData( SELFP_DestFile, (ULONG)NULL, (struct TagItem *)tags ) )
			{
			strncpy( strbuf1, FileReq->Dir, DEFAULTBUFFERLEN );
			AddPart( strbuf1, filename, DEFAULTBUFFERLEN );
			strcpy( tempstr, strbuf1 );
			
				/// Try to get the full name
			
			if( lock = Lock( strbuf1, ACCESS_READ ) )
				{
				NameFromLock( lock, tempstr, DEFAULTPATHLEN );
				
				UnLock( lock );
				}
			else
			if( lock = Lock( FileReq->Dir, ACCESS_READ ) )
				{
				if( NameFromLock( lock, strbuf1, DEFAULTPATHLEN ) )
					{
					AddPart( strbuf1, filename, DEFAULTBUFFERLEN );
					
					strcpy( tempstr, strbuf1 );
					}
				
				UnLock( lock );
				}
			
			}
		}
	
	va_end( filearg );
	
	return( success );
}

/*
 * Select a single path...
 */
BOOL __stdargs xSelectPath( struct Window *win, ... )
{
	struct Gadget *	gad;
	STRPTR			tempstr;
	UBYTE			strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE			filename[ 110 ] = "hej";
	UWORD			len;
	BOOL			success = FALSE;
	BPTR			lock;
	va_list 		tags;
	
	
	va_start( tags, win );
	
		/// Set starting directory
	
	if( tempstr = (STRPTR)GetTagData( SELFP_Dir, (ULONG)"", (struct TagItem *)tags ) )
		rtChangeReqAttr( FileReq, RTFI_Dir, tempstr, TAG_END );
	
		/// If a gadget is specified then override SELFP_Dir and SELFP_File
	
	if( gad = (struct Gadget *)GetTagData( SELFP_Gadget, NULL, (struct TagItem *)tags ) )
		{
		strcpy( strbuf1, GETSTRING( gad ) );
		
			/// Must not be empty
		
		if( strbuf1[0] )
			{
				/// Strip %s and "%s"
			
				/// Strip %s and "%s" in the end
			
			if( GetTagData( SELFP_StripFormats, FALSE, (struct TagItem *)tags ) )
				{
				UBYTE	str[10] = {'"','%','s','"','\0'};
				
				
				len = strlen( strbuf1 );
				
				if( len > 4 )
					{
					if( !strncmp( &strbuf1[ len - 4 ], str, 4 ) )
						strbuf1[ len - 5 ] = '\0';
					else
					if( !strncmp( &strbuf1[ len - 2 ], "%s", 2 ) )
						strbuf1[ len - 3 ] = '\0';
					}
				else
				if( len > 2 )
					{
					if( !strncmp( &strbuf1[ len - 2 ], "%s", 2 ) )
						strbuf1[ len - 3 ] = '\0';
					}
				}
			
				/// Set directory
			
			rtChangeReqAttr( FileReq, RTFI_Dir, strbuf1, TAG_END );
			}
		else
			{
				/// Fallback and use SELFP_Dir
			
			if( tempstr = (STRPTR)GetTagData( SELFP_Dir, (ULONG)"", (struct TagItem *)tags ) )
				rtChangeReqAttr( FileReq, RTFI_Dir, tempstr, TAG_END );
			}
		}
	
		/// Request directory
	
	if( rtFileRequest( FileReq, filename, (STRPTR)GetTagData( SELFP_ReqTitle, (ULONG)GCS( MSG_TL_SELECTPATH ), (struct TagItem *)tags ),
		RT_Window,		win,
		RT_Underscore,	'_',
		RT_TextAttr,	&RequesterFontAttr,
		RTFI_Flags,		FREQF_NOFILES,
		RTFI_OkText,	GetTagData( SELFP_OkText, (ULONG)GCS( MSG_GF_SELECT ), (struct TagItem *)tags ),
		TAG_END ) )
		{
		success = TRUE;
		
			/// Store directory
		
		if( tempstr = (STRPTR)GetTagData( SELFP_DestDir, (ULONG)NULL, (struct TagItem *)tags ) )
			{
			strncpy( tempstr, FileReq->Dir, DEFAULTPATHLEN );
			strncpy( strbuf1, FileReq->Dir, DEFAULTPATHLEN );
			
			if( lock = Lock( strbuf1, ACCESS_READ ) )
				{
				NameFromLock( lock, tempstr, DEFAULTPATHLEN );
				
				UnLock( lock );
				}
			}
		}
	
	va_end( filearg );
	
	return( success );
}

/*
 * Copy a file to given destination
 */
BOOL xCopyFile( STRPTR filename, STRPTR destdir )
{
	BPTR	file,dfile;
	UBYTE *	buffer;
	LONG	s;
	BOOL	success = FALSE;
	UBYTE	strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	if( buffer = AllocVec( 32765, MEMF_ANY ) )
		{
		if( strncpy( strbuf, destdir, DEFAULTPATHLEN ) )
			{
			if( AddPart( strbuf, FilePart( filename ), DEFAULTBUFFERLEN ) )
				{
				if( file = Open( FilePart( filename ), MODE_OLDFILE ) )
					{
					if( dfile = Open( strbuf, MODE_NEWFILE ) )
						{
						while( 1 )
							{
							s = Read( file, buffer, 32765 );
							
								/// Read() Error
							
							if( s == -1 )
								{
								break;
								}
							else
							if( s )
								{
								if( Write( dfile, buffer, s ) == -1 )
									{
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
	
	return( success );
}

BOOL xExists( STRPTR filename )
{
	BPTR	lock;
	
	
	if( lock = Lock( filename, ACCESS_READ ) )
		{
		UnLock( lock );
		
		return( TRUE );
		}
	
	return( FALSE );
}

/*
 * Find the size of a given file and return it
 */
ULONG xFileSize( STRPTR filename )
{
	struct FileInfoBlock *	fib;
	BPTR					lock;
	ULONG					size = 0;
	
	
	if( fib = AllocDosObject( DOS_FIB, TAG_DONE ) )
		{
		if( lock = Lock( filename, ACCESS_READ ) )
			{
			if( Examine( lock, fib ) )
				{
				size = fib->fib_Size;
				}
			UnLock( lock );
			}
		FreeDosObject( DOS_FIB, fib );
		}
	
	return( size );
}

BOOL xPathName( STRPTR filename, STRPTR pathdest, ULONG pathlength )
{
	BPTR	lock,plock;
	BOOL	success = FALSE;
	
	
	if( lock = Lock( filename, ACCESS_READ ) )
		{
		plock = ParentDir( lock );
		
		if( NameFromLock( plock, pathdest, pathlength ) )
			success = TRUE;
		
		UnLock( lock );
		}
	
	return( success );
}

/*
 * Used in conjunction with string gadgets...
 */
STRPTR xGetFullName( STRPTR path, STRPTR filename )
{
	BPTR	lock,cd,file;
	
	
	strcpy( STRBUF0, path );
	AddPart( STRBUF0, filename, DEFAULTBUFFERLEN );
	
	if( lock = Lock( path, ACCESS_READ ) )
		{
		cd = CurrentDir( lock );
		
		if( file = Lock( filename, ACCESS_READ ) )
			{
			NameFromLock( file, STRBUF0, DEFAULTBUFFERLEN );
			
			UnLock( file );
			}
		
		CurrentDir( cd );
		
		UnLock( lock );
		}
	
	return( STRBUF0 );
}

/*
 * Workaround V37 bug...
 *
 * IsWild - 1 means there were wildcards in the pattern,
 *          0 means there were no wildcards in the pattern,
 *         -1 means there was a buffer overflow or other error
 */
LONG xParsePatternNoCase( STRPTR pattern, STRPTR destbuf )
{
	UBYTE	strbuf[ DEFAULTBUFFERLEN ];
	UBYTE	a;
	
	
	for( a = 0; a < strlen( pattern ); a++ )
		strbuf[a] = ToUpper( pattern[a] );
	
	strbuf[a] = 0;
	
	return( ParsePatternNoCase( strbuf, destbuf, DEFAULTBUFFERLEN ) );
}

UBYTE *xStoreFile( STRPTR filename, ULONG size )
{
	UBYTE *	buffer = NULL;
	BPTR	file;
	BOOL	okay = FALSE;
	
	
	if( size )
		{
		if( buffer = AllocVec( size, MEMF_CLEAR ) )
			{
			if( file = Open( filename, MODE_OLDFILE ) )
				{
				if( Read( file, buffer, size ) == size )
					okay = TRUE;
				
				Close( file );
				}
			
			if( !okay )
				{
				FreeVec( buffer );
				
				buffer = NULL;
				}
			}
		}
	
	return( buffer );
}

BOOL xIsExecutable( STRPTR filename )
{
	BPTR	file;
	ULONG	header;
	BOOL	exe = FALSE;
	
	
	if( file = Open( filename, MODE_OLDFILE ) )
		{
		if( Read( file, &header, 4 ) == 4 )
			{
			if( header == HUNK_HEADER )
				exe = TRUE;
			}
		Close( file );
		}
	
	return( exe );
}
