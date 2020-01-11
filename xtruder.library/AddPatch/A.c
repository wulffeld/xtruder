#include	<exec/types.h>
#include	<exec/memory.h>
#include	<libraries/xtruder.h>
#include	<own/mw.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<ctype.h>

#include	<clib/xtruder_protos.h>
#include	<clib/dos_protos.h>
#include	<clib/exec_protos.h>

#include	<pragmas/xtruder_pragmas.h>
#include	<pragmas/dos_pragmas.h>
#include	<pragmas/exec_pragmas.h>


extern struct ExecBase *	SysBase;
extern struct Library *		DOSBase;

struct Library *			GfxBase;
struct Library *			IntuitionBase;
struct Library *			GadToolsBase;
struct Library *			WorkbenchBase;
struct Library *			KeymapBase;
struct Library *			UtilityBase;
struct Library *			XtruderBase;


void main( int argc, char *argv[] )
{
	struct Library *		testlib = NULL;
	struct XtruderInfo *	xi;
	WORD					o;
	ULONG *					ad;
	ULONG					temp;
	UWORD					libdef;
	
	
	if( argc == 3 )
		{
		if( XtruderBase = OpenLibrary( "xtruder.library", 37 ) )
			{
			GadToolsBase = OpenLibrary( "gadtools.library", 0 );
			GfxBase = OpenLibrary( "graphics.library", 0 );
			IntuitionBase = OpenLibrary( "intuition.library", 0 );
			KeymapBase = OpenLibrary( "keymap.library", 0 );
			UtilityBase = OpenLibrary( "utility.library", 0 );
			WorkbenchBase = OpenLibrary( "workbench.library", 0 );
			
			switch( argv[1][0] )
				{
				case '1':
					testlib = (struct Library *)SysBase;
					libdef = XI_LIB_EXEC;
					break;
				
				case '2':
					testlib = DOSBase;
					libdef = XI_LIB_DOS;
					break;
				
				case '3':
					testlib = GfxBase;
					libdef = XI_LIB_GRAPHICS;
					break;
				
				case '4':
					testlib = IntuitionBase;
					libdef = XI_LIB_INTUITION;
					break;
				
				case '5':
					testlib = GadToolsBase;
					libdef = XI_LIB_GADTOOLS;
					break;
				
				case '6':
					testlib = KeymapBase;
					libdef = XI_LIB_KEYMAP;
					break;
				
				case '7':
					testlib = UtilityBase;
					libdef = XI_LIB_UTILITY;
					break;
				
				case '8':
					testlib = WorkbenchBase;
					libdef = XI_LIB_WORKBENCH;
					break;
				}
			
			if( testlib )
				{
				if( xi = xlAllocXtruderInfo() )
					{
					UWORD *	adr;
					
					
					xi->xi_LibBase = (ULONG *)testlib;
					xi->xi_Library = libdef;
					
					o = atol( argv[2] );
					
					Printf( "Checking offset in [1m%s[m\n", testlib->lib_Node.ln_Name );
					
					xi->xi_LVOOffset = o + 2;
					
					temp = (ULONG)testlib + xi->xi_LVOOffset;
					ad = (ULONG *)temp;
					adr = temp = (UWORD *)*ad;
					
					if( temp < 0xF80000 )
						{
						BPTR	file;
						UBYTE	buf[20];
						UBYTE	codes[500] = "";
						int		start,offsets,a;
						char	use = 0;
						
						
						if( file = Open( "RAM:NewPatch", MODE_NEWFILE ) )
							{
							Printf( "LVO $%lx\n", temp );
							
							printf( "Start at offset? " );
							scanf( "%d", &start );
							
							printf( "How many offsets? " );
							scanf( "%d", &offsets );
							
							FPrintf( file, "	{ _PID,		XI_RES_PATCH,	%ld,	%ld-1,", o, offsets );
							
							for( a = start; a < start + offsets; a++ )
								{
								Printf( "Offset %ld\n", a );
								
								if( use != 'A' && use != 'a' )
									{
									Printf( "Use this recog code $%04lx ? (Y/a/n/q) ", adr[a] );
									Flush( Output() );
									scanf( "%s", buf );
									
									use = buf[0];
									}
								else
									Printf( "$%0lx\n", adr[a] );
								
								switch( use )
									{
									case '\n':
									case 'Y':
									case 'y':
										FPrintf( file, "%ld,", a );
										
										sprintf( buf, "0x%04x", adr[a] );
										
										buf[2] = toupper( buf[2] );
										buf[3] = toupper( buf[3] );
										buf[4] = toupper( buf[4] );
										buf[5] = toupper( buf[5] );
										
										strcat( codes, buf );
										
										if( a == start + offsets - 1 )
											FPrintf( file, "	%s },	// %ld\n", codes, o );
										else
											strcat( codes, "," );
										break;
									
									case 'a':
									case 'A':
										FPrintf( file, "%ld,", a );
										
										sprintf( buf, "0x%04x", adr[a] );
										
										buf[2] = toupper( buf[2] );
										buf[3] = toupper( buf[3] );
										buf[4] = toupper( buf[4] );
										buf[5] = toupper( buf[5] );
										
										strcat( codes, buf );
										
										if( a == start + offsets - 1 )
											FPrintf( file, "	%s },	// %ld\n", codes, o );
										else
											strcat( codes, "," );
										break;
									
									case 'n':
									case 'N':
										offsets++;
										break;
									
									case 'q':
									case 'Q':
										break;
										break;
									}
								}
							Close( file );
							}
						}
					else
						Printf( "Code for offset is located in ROM\n" );
					
					xlFreeXtruderInfo( xi );
					}
				}
			
			CloseLibrary( WorkbenchBase );
			CloseLibrary( UtilityBase );
			CloseLibrary( KeymapBase );
			CloseLibrary( IntuitionBase );
			CloseLibrary( GfxBase );
			CloseLibrary( GadToolsBase );
			
			CloseLibrary( XtruderBase );
			}
		}
}
