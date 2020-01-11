#include	<exec/types.h>
#include	<exec/execbase.h>
#include	<exec/memory.h>
#include	<exec/libraries.h>
#include	<exec/resident.h>
#include	<libraries/xtruder.h>
#include	<own/mw.h>
#include	<string.h>

#include	<clib/alib_protos.h>
#include	<clib/exec_protos.h>
#include	<clib/dos_protos.h>
#include	<clib/xtruder_protos.h>

#include	<pragmas/exec_pragmas.h>
#include	<pragmas/dos_pragmas.h>


#define	VERSION		37
#define	REVISION	2
#define	VSTRING		"xtruder.library 37.2 (9.11.95)"


BOOL									xLIB_Init( VOID );
VOID									xLIB_Exit( VOID );
static struct Library * ASM __saveds	LibInit( REGA0 BPTR Segment, REGD0 struct Library * );
extern struct Library * ASM __saveds	LibOpen( REGA6 struct Library * );
extern BPTR ASM __saveds				LibExpunge( REGA6 struct Library * );
extern BPTR ASM __saveds				LibClose( REGA6 struct Library * );
static LONG ASM __saveds				LibNull( REGA6 struct Library * );

extern __far struct Resident RomTag;

static APTR LibVectors[] =
{
	LibOpen,
	LibClose,
	LibExpunge,
	LibNull,

	xlAllocXtruderInfo,
	xlFreeXtruderInfo,
	xlCheckLVO,
	xlPatchListFirst,
	xlPatchListNext,
	(APTR)-1
};

static struct ExecBase *			SysBase;
struct Library *					DOSBase;
struct Library *					IntuitionBase;

UBYTE								LibName[] = "xtruder.library";
UBYTE								LibID[] = VSTRING;
LONG								LibVersion = VERSION;
LONG								LibRevision = REVISION;

struct
{
	ULONG DataSize;
	APTR Table;
	APTR Data;
	struct Library * (*Init)();
} __aligned LibInitTab =
{
	sizeof(struct Library),
	LibVectors,
	NULL,
	LibInit
};

static BPTR LibSegment;

BOOL xLIB_Init( VOID )
{
	if( SysBase )
		return( TRUE );
	else
		{
		SysBase = *(struct ExecBase **)4;
		
		if( SysBase->LibNode.lib_Version < 37 )
			{
			SysBase = NULL;
			
			return( FALSE );
			}
		else
			{
			IntuitionBase = OpenLibrary( "intuition.library", 36 );
			
			if( DOSBase = OpenLibrary( "dos.library",37 ) )
				return( TRUE );
			
			xLIB_Exit();
			
			return( FALSE );
			}
		}
}

VOID xLIB_Exit()
{
	if( SysBase )
		{
		if( DOSBase )
			{
			if( IntuitionBase )
				{
				CloseLibrary( IntuitionBase );
				IntuitionBase = NULL;
				}
			
			CloseLibrary( DOSBase );
			
			DOSBase = NULL;
			}
		
		SysBase = NULL;
		}
}

static struct Library * ASM __saveds LibInit( REGA0 BPTR Segment, REGD0 struct Library *Base )
{
	Base->lib_Node.ln_Type	= NT_LIBRARY;
	Base->lib_Node.ln_Name	= LibName;
	Base->lib_Flags			= LIBF_CHANGED | LIBF_SUMUSED;
	Base->lib_Version		= LibVersion;
	Base->lib_Revision		= LibRevision;
	Base->lib_IdString		= (APTR)LibID;
	
	LibSegment = Segment;
	
	SysBase = *(struct ExecBase **)4;
	
	if( SysBase->LibNode.lib_Version < 37 )
		return( NULL );
	else
		return( Base );
}

extern struct Library * ASM __saveds LibOpen( REGA6 struct Library *Base )
{
	Base->lib_Flags &= ~LIBF_DELEXP;
	Base->lib_OpenCnt++;
	
	if( Base->lib_OpenCnt == 1 )
		{
		if( xLIB_Init() )
			return( Base );
		
		xLIB_Exit();
		
		return( NULL );
		}
	else
		return( Base );
}

extern BPTR ASM __saveds LibExpunge( REGA6 struct Library *Base )
{
	if( !Base->lib_OpenCnt && LibSegment )
		{
		BPTR	TempSegment = LibSegment;
		
		
		Remove( (struct Node *)Base );
		
		FreeMem( (BYTE *)Base - Base->lib_NegSize, Base->lib_NegSize + Base->lib_PosSize );
		
		LibSegment = NULL;
		
		return( TempSegment );
		}
	else
		{
		Base->lib_Flags &= ~LIBF_DELEXP;
		
		return( NULL );
		}
}

extern BPTR ASM __saveds LibClose( REGA6 struct Library *Base )
{
	Base->lib_OpenCnt--;
	
	if( !Base->lib_OpenCnt )
		{
		xLIB_Exit();
		
		if( Base->lib_Flags & LIBF_DELEXP )
			return( LibExpunge( Base ) );
		}
	
	return( NULL );
}

static LONG ASM __saveds LibNull( REGA6 struct Library *Base )
{
	return( NULL );
}

enum
{
	ALERTPATCH_PID,
	AUTOCENTRE_PID,
	CEDPATCH_PID,
	COPYMEMQUICKER_PID,
	CPUBLIT_PID,
	DIRECTORYOPUS_PID,
	FASTERBLIT_PID,
	IPREFS_PID,
	LOCALELIBRARY_PID,
	LOCKPATCH_PID,
	MAGICMENU_PID,
	NEWMENU_PID,
	ONEFONT_PID,
	PPLIB_PID,
	POWERSNAP_PID,
	PROP3D_PID,
	QMOUSE_PID,
	RTPATCH_PID,
	SETPATCH_PID,
	SETPATCHMRGCOP_PID,
	SETSCR_PID,
	TOOLSDAEMON_PID
};

	/// Known patches LVO names

#define		KNOWNPATCHES	22

UBYTE *LVOPatches[ KNOWNPATCHES ] =
{
	"AlertPatch          - David Swasbrook",
	"AutoCentre          - Colin Bell",
	"CEDPatch            - Preben Nielsen",
	"CopyMemQuicker      - Arthur Hagen",
	"CPUBlit             - Eddy Carroll",
	"Directory Opus      - Jonathan Potter",
	"FasterBlit          - Arthur Hagen",
	"IPrefs              - Commodore",
	"Locale Library      - Commodore",
	"LockPatch           - ?",
	"MagicMenu           - Martin Korndörfer",
	"NewMenu             - David Kinder",
	"OneFont             - Zdenek Kabelac",
	"PowerPacker library - Nico François",
	"PowerSnap           - Nico François",
	"Prop3D              - Massimo Tantignone",
	"QMouse              - Dan Babcock",
	"RTPatch             - Nico François",
	"SetPatch            - Commodore",
	"SetPatchMrgCop      - Digital Creations",
	"SetScr              - Norman Iscove",
	"ToolsDaemon         - Nico François",
};

#define		KNOWNVIRA		1

	/// Known vira LVO names

UBYTE *LVOVira[ KNOWNVIRA ] =
{
	"",
};

	/// LVO structure

struct LVORecog
{
	UWORD	lr_LVOName;					// Virus/patch name ID
	UWORD	lr_Type;					// Virus or patch or program
	WORD	lr_LVOOffset;				// It's offset in library
	UWORD *	lr_Recog;					// Recog code, including offsets
};

#define		KNOWN_DOS_PATCHES					5

UWORD dat_dos_lockpatch_84[]			= { 5, 0x0, 0x48E7,0x210C,0x2E02,0x2A41,0x49F9,     8,  0xE, 0x70FE,0xBE80,0x6710,0x2007,0x5280,0x670A,0x42A7,0x4EB9,    3, 0x22, 0x584F,0x220D,0x2407, NULL};
UWORD dat_dos_setpatch_432[]			= {16, 0x0, 0x203C,0x0000,0x0409,0x48E7,0x7F38,0x2400,0x2645,0x49FA,0x007E,0x2017,0xE588,0x2440,0x4EAE,0xFDF6,0x220A,0x6704, NULL};
UWORD dat_dos_localelibrary_744[]		= { 7, 0x0, 0x2F0E,0x2C7A,0xFF26,0x6100,0x0530,0x2C5F,0x4E75, NULL};
UWORD dat_dos_localelibrary_750[]		= { 7, 0x0, 0x2F0E,0x2C7A,0xFF18,0x6100,0x06CC,0x2C5F,0x4E75, NULL};
UWORD dat_dos_localelibrary_978[]		= {16, 0x0, 0x2F01,0x207A,0xFFF8,0x2068,0x007C,0x4E90,0x2240,0x201F,0x2C7A,0xFFEA,0x206E,0x0078,0x4EAE,0xFFB8,0x2C6E,0x0058, NULL};

struct LVORecog LVO_Dos_Recog[ KNOWN_DOS_PATCHES ] =
{
	{ LOCKPATCH_PID,		XI_RES_PATCH,	-84,	dat_dos_lockpatch_84 },	// Lock -84
	{ SETPATCH_PID,			XI_RES_PATCH,	-432,	dat_dos_setpatch_432 },	// ExAll -432
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-744,	dat_dos_localelibrary_744 },	// DateToStr -744
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-750,	dat_dos_localelibrary_750 },	// StrToDate -750
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-978,	dat_dos_localelibrary_978 },	// dosPrivate5 -978
};

#define		KNOWN_EXEC_PATCHES					8

UWORD dat_exec_alertpatch_108[]			= { 2, 0x0, 0x0839,0x0001,     5,  0x8, 0x6706,0x2F3A,0xFFF0,0x4E75,0x4EF9, NULL};
UWORD dat_exec_pplib_210[]				= {13, 0x0, 0x0CA9,0x5050,0x4D4D,0xFFF8,0x660A,0xB3E9,0xFFFC,0x6604,0x5189,0x5080,0x2F3A,0xFFE6,0x4E75, NULL};
UWORD dat_exec_rtpatch_408[]			= {16, 0x0, 0x7000,0x487A,0x0008,0x2F3A,0xFEB6,0x4E75,0x4A80,0x6602,0x4E75,0x48E7,0xFFFE,0x2640,0x206B,0x000A,0x1018,0xE188, NULL};
UWORD dat_exec_localelibrary_522[]		= {16, 0x0, 0x48E7,0x0030,0x9FFC,0x0000,0x0014,0x2F4A,0x000C,0x2F4B,0x0010,0x4DFA,0x0026,0x2F4E,0x0008,0x2C7A,0xFF6A,0x2449, NULL};
UWORD dat_exec_rtpatch_552[]			= { 2, 0x0, 0x487A,0x0008,     4,  0x8, 0x4E75,0x4A80,0x6602,0x4E75,     8, 0x2A, 0xB0BC,0x7265,0x712E,0x6714,0x7201,0xB0BC,0x6172,0x702E, NULL};
UWORD dat_exec_copymemquicker_624[]		= {13, 0x0, 0x7210,0xB081,0x653C,0x3208,0xE499,0x3209,0xE599,0x0241,0x000F,0x674C,0x323B,0x120A,0x4EFB, NULL};
UWORD dat_exec_copymemquicker_630[]		= {16, 0x6, 0xE448,0x0C40,0x0060,0x6524,0x720C,0x9041,0x80C1,0x48E7,0x3F3E,0x4CD8,0x7CFE,0x48D1,0x7CFE,0x43E9,0x0030,0x51C8, NULL};
UWORD dat_exec_setpatch_714[]			= {16, 0x0, 0x2208,0x675E,0x2209,0x675A,0xB0A8,0x0014,0x6456,0x2F0B,0x2210,0x2641,0x2213,0x6762,0x4AAB,0x0008,0x675C,0xB3EB, NULL};

struct LVORecog LVO_Exec_Recog[ KNOWN_EXEC_PATCHES ] =
{
	{ ALERTPATCH_PID,		XI_RES_PATCH,	-108,	dat_exec_alertpatch_108 },		// Alert -108
	{ PPLIB_PID,			XI_RES_PATCH,	-210,	dat_exec_pplib_210 },			// FreeMem -210
	{ RTPATCH_PID,			XI_RES_PATCH,	-408,	dat_exec_rtpatch_408 },			// OldOpenLibrary -408
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-522,	dat_exec_localelibrary_522 },	// RawDoFmt-522
	{ RTPATCH_PID,			XI_RES_PATCH,	-552,	dat_exec_rtpatch_552 },			// OpenLibrary -552
	{ COPYMEMQUICKER_PID,	XI_RES_PATCH,	-624,	dat_exec_copymemquicker_624 },	// CopyMem -624
	{ COPYMEMQUICKER_PID,	XI_RES_PATCH,	-630,	dat_exec_copymemquicker_630 },	// CopyMemQuick -630
	{ SETPATCH_PID,			XI_RES_PATCH,	-714,	dat_exec_setpatch_714 },		// FreePooled -714
};

#define		KNOWN_GADTOOLS_PATCHES				7

UWORD dat_gadt_prop3d_30[]				= { 8, 0x0, 0x4EF9,0x9EFC,0x0024,0x48E7,0x0F3E,0x2E00,0x48EF,0x4600,0x0030,     6, 0x16, 0x2A48,0x7C00,0x7A00,0x42AF,0x003C,0x7802, NULL};
UWORD dat_gadt_prop3d_36[]				= { 6, 0x0, 0x4EF9,0x514F,0x48E7,0x003E,0x2A48,0x2F4E,0x0018,     8, 0x12, 0x2F2C,0x009E,0x6100,0x09D4,0x584F,0x264D,0x2F40,0x0014, NULL};
UWORD dat_gadt_newmenu_48[]				= { 4, 0x0, 0x6100,0xFEF6,0x670C,0x23C9,     1,  0xC, 0x43F9,     1, 0x12, 0x2F39,     1, 0x18, 0x4E75, NULL};
UWORD dat_gadt_newmenu_60[]				= { 4, 0x0, 0x6100,0xFEC2,0x670C,0x23CA,     1,  0xC, 0x45F9,     1, 0x12, 0x2F39,     1, 0x18, 0x4E75, NULL};
UWORD dat_gadt_newmenu_66[]				= { 4, 0x0, 0x6100,0xFEDC,0x670C,0x23CA,     1,  0xC, 0x45F9,     1, 0x12, 0x2F39,     1, 0x18, 0x4E75, NULL};
UWORD dat_gadt_prop3d_126[]				= { 5, 0x0, 0x4EF9,0x9EFC,0x0010,0x48E7,0x003E,0x2A48,     9, 0x10, 0x2449,0x2F2C,0x009A,0x2F4E,0x0024,0x6100,0x0EC6,0x204D,0x224A, NULL};
UWORD dat_gadt_prop3d_132[]				= { 6, 0x0, 0x4EF9,0x514F,0x48E7,0x013E,0x2A48,0x2F4E,0x001C,    8,  0x12, 0x7E00,0x2F2C,0x00A2,0x6100,0x0E00,0x584F,0x266C,0x0088, NULL};

struct LVORecog LVO_GadTools_Recog[ KNOWN_GADTOOLS_PATCHES ] =
{
	{ PROP3D_PID,			XI_RES_PATCH,	-30,	dat_gadt_prop3d_30 },	// CreateGadgetA -30
	{ PROP3D_PID,			XI_RES_PATCH,	-36,	dat_gadt_prop3d_36 },	// FreeGadgets -36
	{ NEWMENU_PID,			XI_RES_PATCH,	-48,	dat_gadt_newmenu_48 },	// CreateMenusA -48
	{ NEWMENU_PID,			XI_RES_PATCH,	-60,	dat_gadt_newmenu_60 },	// LayoutMenuItemsA -60
	{ NEWMENU_PID,			XI_RES_PATCH,	-66,	dat_gadt_newmenu_66 },	// LayoutMenusA -66
	{ PROP3D_PID,			XI_RES_PATCH,	-126,	dat_gadt_prop3d_126 },	// GetVisualInfoA -126
	{ PROP3D_PID,			XI_RES_PATCH,	-132,	dat_gadt_prop3d_132 },	// FreeVisualInfo -132
};

#define		KNOWN_GRAPHICS_PATCHES				30

UWORD dat_gfx_cpublit_30[]				= { 8, 0x0, 0x4A44,0x67FA,0xB3C8,0x6606,0xB641,0x67EC,0x6008,0x4AB9,    8, 0x14, 0x66E2,0x2F06,0x0206,0x00F0,0x0C06,0x00C0,0x66D4,0x2C00, NULL};
UWORD dat_gfx_setpatch_30[]				= { 2, 0x0, 0x2F07,0x4EB9,     2,  0x4, 0x2E1F,0x4E75, NULL};
UWORD dat_gfx_fasterblit_42[]			= {16, 0x0, 0x2F03,0x2F02,0x3029,0x0024,0x48C0,0x3229,0x0026,0x9269,0x003E,0x48C1,0x2069,0x0004,0x7400,0x3410,0x2601,0x2069, NULL};
UWORD dat_gfx_fasterblit_48[]			= {16, 0x0, 0x2F03,0x2F02,0x2409,0x61C8,0x2242,0x7000,0x3229,0x0026,0x9269,0x003E,0x2069,0x0034,0xD268,0x0014,0x48C1,0x2069, NULL};
UWORD dat_gfx_fasterblit_54[]			= {16, 0x0, 0x48C0,0x67F8,0x48E7,0x3E3C,0x1829,0x0038,0x4844,0x3829,0x0040,0x2269,0x0034,0x2669,0x000E,0x6608,0x082B,0x0001, NULL};
UWORD dat_gfx_fasterblit_66[]			= {16, 0x0, 0x2009,0x672A,0x2008,0x6728,0x2F09,0x93C9,0x2F08,0x4EAE,0xFCD0,0x205F,0x225F,0x4A80,0x6714,0x2348,0x0034,0x4229, NULL};
UWORD dat_gfx_onefont_66[]				= {16, 0x0, 0x2F3A,0x006E,0xB1FA,0x006E,0x661A,0x48E7,0xC0E2,0x6116,0x4CDF,0x4703,0x207A,0x0066,0x6604,0x207A,0x005C,0x117C, NULL};
UWORD dat_gfx_fasterblit_198[]			= {16, 0x0, 0x2049,0x7000,0x7212,0x20C0,0x51C9,0xFFFC,0x3080,0x3341,0x0018,0x1341,0x001B,0x5229,0x001C,0x3341,0x0022,0x223C, NULL};
UWORD dat_gfx_setpatchmrgcop_210[]		= {16, 0x0, 0x2F09,0x207A,0xFFF4,0x4E90,0x48E7,0xBE00,0x226F,0x0018,0x2009,0x6728,0x206E,0x0026,0x343A,0x00B8,0x3430,0x2000, NULL};
UWORD dat_gfx_fasterblit_270[]			= {16, 0x0, 0x2F0E,0x486E,0x00C8,0x2C6E,0x01A4,0x7000,0x7210,0x4EAE,0xFECE,0x2057,0x2EAE,0x0114,0x3F3C,0x0004,0x33FC,0x4000, NULL};
UWORD dat_gfx_fasterblit_300[]			= {16, 0x0, 0x2F06,0x41F9,0x00DF,0xF040,0x4246,0x526E,0x00AA,0x66E0,0x4A28,0xFFC2,0x0828,0x0006,0xFFC2,0x66DC,0x30FC,0x01F0, NULL};
UWORD dat_gfx_fasterblit_324[]			= {16, 0x0, 0x48E7,0x381E,0x2411,0x6700,0x009C,0x2A42,0x41ED,0x0048,0x284E,0x2C6E,0x01A4,0x4EAE,0xFDCC,0x2C4C,0x242D,0x0008, NULL};
UWORD dat_gfx_fasterblit_306[]			= {16, 0x0, 0x2F04,0x7800,0x2044,0x0829,0x0003,0x0021,0x6608,0x4EAE,0xFEC8,0x281F,0x4E75,0x48E7,0xC420,0x2449,0x082A,0x0001, NULL};
UWORD dat_gfx_fasterblit_342[]			= {16, 0x0, 0x1340,0x0019,0x137C,0x000F,0x001E,0x0029,0x0001,0x0021,0x1229,0x001C,0x0241,0x0007,0x41E9,0x0030,0x6736,0xE249, NULL};
UWORD dat_gfx_fasterblit_348[]			= {16, 0x0, 0x1340,0x001A,0x6004,0x1340,0x0019,0x137C,0x000F,0x001E,0x0029,0x0001,0x0021,0x1229,0x001C,0x0241,0x0007,0x41E9, NULL};
UWORD dat_gfx_fasterblit_354[]			= {16, 0x0, 0x1340,0x001C,0x600A,0x1340,0x001A,0x6004,0x1340,0x0019,0x137C,0x000F,0x001E,0x0029,0x0001,0x0021,0x1229,0x001C, NULL};
UWORD dat_gfx_fasterblit_432[]			= {11, 0x0, 0x2F08,0x41ED,0x0048,0x2F0E,0x2C6E,0x01A4,0x4EAE,0xFDCC,0x2C5F,0x205F,0x4E75, NULL};
UWORD dat_gfx_fasterblit_438[]			= {11, 0x0, 0x2F08,0x41ED,0x0048,0x2F0E,0x2C6E,0x01A4,0x4EAE,0xFDC6,0x2C5F,0x205F,0x4E75, NULL};
UWORD dat_gfx_fasterblit_456[]			= { 4, 0x0, 0x526E,0x00AA,0x66A4,0x4E75, NULL};
UWORD dat_gfx_fasterblit_462[]			= {16, 0x0, 0x536E,0x00AA,0x6D46,0x2F0D,0x2F09,0x43F9,0x00DF,0xF09A,0x2A6E,0x01A4,0x32BC,0x4000,0x522D,0x0126,0x4AAE,0x003A, NULL};
UWORD dat_gfx_setpatch_588[]			= {14, 0x0, 0x3028,0x0020,0x0240,0x0800,0x6710,0x2268,0x0024,0x2269,0x0004,0x0C29,0x0008,0x0005,0x6706,0x4EF9,     2, 0x10, 0x2F0A,0x2008, NULL};
UWORD dat_gfx_setpatch_636[]			= { 5, 0x0, 0x0C68,0x805C,0x0006,0x6706,0x4EF9,    11,  0x7, 0x41E8,0x0028,0x2F20,0x2F20,0x2F20,0x2F20,0x2F20,0x2F20,0x2F20,0x2F20,0x2F20, NULL};
UWORD dat_gfx_setpatch_660[]			= {16, 0x0, 0x2F00,0x207A,0xFFF8,0x4E90,0x221F,0x0C41,0x0004,0x660A,0x2040,0x43FA,0xFFA4,0x2149,0x0094,0x4E75,0x0022,0x76C8, NULL};
UWORD dat_gfx_setpatch_666[]			= {16, 0x0, 0x48E7,0x0112,0x2648,0x7000,0x102B,0x0008,0x0440,0x0012,0x664A,0x7000,0x102B,0x000E,0x5540,0x6640,0x7000,0x102B, NULL};
UWORD dat_gfx_setpatch_672[]			= {16, 0x0, 0x4E55,0xFFF4,0x48E7,0x0032,0x2648,0x2449,0x200B,0x614C,0x206E,0x015A,0x254B,0x0018,0x2F40,0x0010,0x2F48,0x0014, NULL};
UWORD dat_gfx_setpatch_702[]			= {16, 0x0, 0x4E55,0xFFF8,0x48E7,0x0032,0x2648,0x200B,0x618A,0x206E,0x015A,0x2F40,0x000C,0x2F48,0x0010,0x2078,0x0004,0x5228, NULL};
UWORD dat_gfx_fasterblit_774[]			= {16, 0x0, 0x48E7,0xFFFC,0x2669,0x0004,0x7200,0x122B,0x0005,0xE549,0x41EB,0x0008,0x48C2,0x0642,0x000F,0x0242,0xFFF0,0x2802, NULL};
UWORD dat_gfx_fasterblit_786[]			= {16, 0x0, 0x48E7,0x3F38,0x9440,0x48C2,0x5282,0x2802,0x0644,0x000F,0x0244,0xFFF0,0x6F2A,0x2E00,0x2A01,0x9641,0x6D22,0x2648, NULL};
UWORD dat_gfx_setpatch_942[]			= {16, 0x0, 0x48E7,0x3F3E,0x2648,0x206E,0x019C,0x2A4E,0x2C6E,0x01A4,0x4EAE,0xFDCC,0x2C4D,0x202E,0x01D8,0xB0AA,0x0040,0x6700, NULL};
UWORD dat_gfx_setpatch_1050[]			= {16, 0x0, 0x48E7,0x0720,0x7E00,0x2A08,0x2C0E,0x2C6E,0x01A0,0x203C,0x8000,0x0009,0x4EAE,0xFFE2,0x2C46,0x2045,0x4A80,0x6718, NULL};

struct LVORecog LVO_Graphics_Recog[ KNOWN_GRAPHICS_PATCHES ] =
{
	{ CPUBLIT_PID,			XI_RES_PATCH,	-30,	dat_gfx_cpublit_30 },	// BltBitMap -30
	{ SETPATCH_PID,			XI_RES_PATCH,	-30,	dat_gfx_setpatch_30 },	// BltBitMap -30
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-42,	dat_gfx_fasterblit_42 },	// ClearEOL -42
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-48,	dat_gfx_fasterblit_48 },	// ClearScreen -48
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-54,	dat_gfx_fasterblit_54 },	// TextLength -54
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-66,	dat_gfx_fasterblit_66 },	// SetFont -66
	{ ONEFONT_PID,			XI_RES_PATCH,	-66,	dat_gfx_onefont_66 },	// SetFont -66
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-198,	dat_gfx_fasterblit_198 },	// InitRastport -198
	{ SETPATCHMRGCOP_PID,	XI_RES_PATCH,	-210,	dat_gfx_setpatchmrgcop_210 },	// MrgCop -210
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-270,	dat_gfx_fasterblit_270 },	// WaitTOF -270
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-300,	dat_gfx_fasterblit_300 },	// BltClear -300
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-324,	dat_gfx_fasterblit_324 },	// WritePixel -324
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-306,	dat_gfx_fasterblit_306 },	// RectFill -306
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-342,	dat_gfx_fasterblit_342 },	// SetAPen -342
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-348,	dat_gfx_fasterblit_348 },	// SetBPen -348
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-354,	dat_gfx_fasterblit_354 },	// SetDrMd -354
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-432,	dat_gfx_fasterblit_432 },	// LockLayerRom -432
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-438,	dat_gfx_fasterblit_438 },	// UnlockLayerRom -438
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-456,	dat_gfx_fasterblit_456 },	// OwnBlitter -456
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-462,	dat_gfx_fasterblit_462 },	// DisownBlitter -462
	{ SETPATCH_PID,			XI_RES_PATCH,	-588,	dat_gfx_setpatch_588 },	// ScrollVPort -588
	{ SETPATCH_PID,			XI_RES_PATCH,	-636,	dat_gfx_setpatch_636 },	// BltMaskBitMapRastPort -636
	{ SETPATCH_PID,			XI_RES_PATCH,	-660,	dat_gfx_setpatch_660 },	// GfxNew -660
	{ SETPATCH_PID,			XI_RES_PATCH,	-666,	dat_gfx_setpatch_666 },	// GfxFree -666
	{ SETPATCH_PID,			XI_RES_PATCH,	-672,	dat_gfx_setpatch_672 },	// GfxAssociate -672
	{ SETPATCH_PID,			XI_RES_PATCH,	-702,	dat_gfx_setpatch_702 },	// GfxLookUp -702
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-774,	dat_gfx_fasterblit_774 },	// WritePixelLine8 -774
	{ FASTERBLIT_PID,		XI_RES_PATCH,	-786,	dat_gfx_fasterblit_786 },	// WritePixelArray8 -786
	{ SETPATCH_PID,			XI_RES_PATCH,	-942,	dat_gfx_setpatch_942 },	// ChangeVPBitMap -942
	{ SETPATCH_PID,			XI_RES_PATCH,  -1050,	dat_gfx_setpatch_1050 },	// BestModeIDA -1050
};

#define		KNOWN_INTUITION_PATCHES				35

UWORD dat_int_cedpatch_54[]				= {10, 0x2, 0x0074,0x670A,0x6100,0x009E,0x6704,0x6100,0x01CC,0x2F3A,0x0008,0x4E75, NULL};
UWORD dat_int_magicmenu_54[]			= { 6, 0xC, 0x6100,0xFF12,0x4A40,0x6718,0x204D,0x4EBA,     4, 0x1C, 0x206C,0x0568,0x2C78,0x0004, NULL};
UWORD dat_int_toolsdaemon_54[]			= { 4, 0x0, 0x48E7,0x001E,0x2A48,0x49F9,    12,  0xC, 0x2C78,0x0004,0x4EAE,0xFF7C,0x266D,0x001C,0x204D,0x2C6C,0x0078,0x226C,0x0064,0x4E91, NULL};
UWORD dat_int_qmouse_60[]				= {13, 0x0, 0x6100,0xFDB8,0x227A,0x0710,0x43E9,0x01F4,0x701F,0xB1E9,0x0010,0x670A,0x43E9,0x001A,0x51C8, NULL};
UWORD dat_int_qmouse_66[]				= {16, 0x0, 0x6100,0xFDB2,0x227A,0x06BA,0x2229,0x05B8,0x487A,0x0004,0x6004,0x6000,0xFDAE,0x6100,0xFD4C,0x43E9,0x00F4,0x701F, NULL};
UWORD dat_int_powersnap_66[]			= { 4, 0xC, 0x673A,0x48E7,0xC0CA,0x49F9,     8, 0x1A, 0x0004,0x226C,0x00EC,0x202C,0x0382,0x4EAE,0xFEBC,0x2C6C, NULL};
UWORD dat_int_magicmenu_72[]			= { 4, 0x0, 0x48E7,0x010E,0x2A48,0x49F9,     8, 0x22, 0x206C,0x0568,0x2C78,0x0004,0x4EAE,0xFDC6,0x2007,0x600C, NULL};
UWORD dat_int_qmouse_72[]				= {16, 0x0, 0x227A,0x0664,0x6100,0xFD38,0x6100,0xFD00,0x487A,0x0046,0x2F08,0x4EAE,0xFFC4,0x205F,0x227A,0x064C,0xB1E9,0x1602, NULL};
UWORD dat_int_iprefs_78[]				= {10, 0x2, 0xE5E6,0x4A40,0x6604,0x7000,0x4E75,0x6100,0x03AC,0x2079,0x0023,0x9C96, NULL};
UWORD dat_int_qmouse_78[]				= {14, 0x0, 0x6100,0xFDF0,0x4EAE,0xFF2E,0x4A80,0x6700,0xFDF4,0x2040,0x227A,0x06D0,0x2229,0x05BC,0x487A,0x0004, NULL};
UWORD dat_int_iprefs_96[]				= { 3, 0x0, 0x48E7,0x0082,0x2C79,     1,  0xA, 0x2279,     6, 0x10, 0x203C,0x0000,0x8000,0x4EAE,0xFEBC,0x4A79,     5, 0x20, 0x4CDF,0x4100,0x6602,0x4E75,0x2279, NULL};
UWORD dat_int_qmouse_96[]				= { 7, 0x0, 0x227A,0x085A,0x4AA9,0x168A,0x6604,0x2F29,0x05D0, NULL};
UWORD dat_int_autocentre_198[]			= { 7, 0xA, 0x205F,0x3028,0x0000,0x4A40,0x6634,0x3428,0x0004,     3, 0x2E, 0x660E,0x3001,0x0240, NULL};
UWORD dat_int_rtpatch_198[]				= { 1, 0x0, 0x4A79,     8,  0x6, 0x6632,0x3028,0x000E,0x0240,0x1000,0x6708,0x2268,0x0020,     6, 0x18, 0xFD9C,0x7001,0xB068,0x0008,0x6418,0x42A7, NULL};
UWORD dat_int_magicmenu_204[]			= { 7, 0x6, 0x2A4F,0x200F,0x0200,0x00FC,0x2E40,0x2648,0x49F9,     6, 0x18, 0x6100,0xFC2A,0x4A40,0x6718,0x204B,0x4EBA, NULL};
UWORD dat_int_newmenu_204[]				= { 8, 0x0, 0x6100,0xFF26,0x6708,0x00A8,0x0020,0x0000,0x000E,0x2F39,     1, 0x14, 0x4E75, NULL};
UWORD dat_int_cedpatch_264[]			= {10, 0x2, 0x0092,0x670A,0x6100,0x00D8,0x6704,0x6100,0x0150,0x2F3A,0x0008,0x4E75, NULL};
UWORD dat_int_magicmenu_264[]			= { 5, 0x0, 0x48E7,0x011E,0x2649,0x2A48,0x49F9,     5,  0xE, 0x6100,0xFED8,0x4A40,0x671E,0x204D, NULL};
UWORD dat_int_toolsdaemon_264[]			= { 4, 0x2, 0x003E,0x2649,0x2A48,0x49F9,    12,  0xE, 0x2C78,0x0004,0x4EAE,0xFF7C,0x204D,0x6100,0xF9CA,0x4A40,0x661E,0x204D,0x224B,0x2C6C, NULL};
UWORD dat_int_qmouse_270[]				= { 2, 0x0, 0x6100,0xFE1A, NULL};
UWORD dat_int_magicmenu_312[]			= { 9, 0x2, 0x48E7,0x011E,0x2A4F,0x200F,0x0200,0x00FC,0x2E40,0x2648,0x49F9, NULL};
UWORD dat_int_rtpatch_348[]				= {10, 0x0, 0x48E7,0xC040,0x7200,0x2029,0x0006,0xB280,0x650C,0x4CDF,0x0203,0x2F39, NULL};
UWORD dat_int_magicmenu_450[]			= { 9, 0x2, 0x48E7,0x011E,0x2A4F,0x200F,0x0200,0x00FC,0x2E40,0x2648,0x49F9, NULL};
UWORD dat_int_prop3d_468[]				= { 8, 0x0, 0x4EF9,0x9EFC,0x0028,0x48E7,0x013E,0x2E01,0x2A48,0x48EF,0x4620,     4, 0x18, 0x2200,0x2F40,0x001C,0x2F42, NULL};
UWORD dat_int_rtpatch_588[]				= { 1, 0x0, 0x4A79,     3,  0x6, 0x6704,0x7000,0x4E75, NULL};
UWORD dat_int_magicmenu_606[]			= { 9, 0x2, 0x48E7,0x013E,0x2A4F,0x200F,0x0200,0x00FC,0x2E40,0x2449,0x2648, NULL};
UWORD dat_int_newmenu_606[]				= { 4, 0x0, 0x6100,0xFF10,0x670C,0x23C9,     1,  0xC, 0x43F9,     1, 0x12, 0x2F39,     1, 0x18, 0x4E75, NULL};
UWORD dat_int_qmouse_606[]				= {12, 0x0, 0x2F09,0x487A,0x0018,0x518F,0x2E88,0x207A,0x0840,0x2F68,0x05D4,0x0004,0x205F,0x4E75, NULL};
UWORD dat_int_autocentre_612[]			= {12,0x14, 0x3428,0x0004,0xB47C,0xFFFF,0x672A,0x45FA,0x0078,0x3612,0x3028,0x000C,0x3200,0x0240, NULL};
UWORD dat_int_cedpatch_612[]			= {10, 0x2, 0x0056,0x6700,0x0044,0x203A,0xFE30,0x80BA,0xFE80,0x6738,0x2F0A,0x2F09, NULL};
UWORD dat_int_iprefs_612[]				= { 3, 0x8, 0x8000,0x002F,0x7200,     7, 0x14, 0x4EAE,0xFFDC,0x4A80,0x660C,0x4CDF,0x4300,0x2F39, NULL};
UWORD dat_int_rtpatch_612[]				= {16, 0x0, 0x2009,0x6732,0x48E7,0x00C2,0x207A,0xFDDA,0x2C68,0x003A,0x2049,0x203C,0x8000,0x003A,0x2F08,0x4EAE,0xFFE2,0x205F, NULL};
UWORD dat_int_magicmenu_702[]			= { 5, 0x0, 0x48E7,0x011E,0x2649,0x2A48,0x49F9,     5,  0xE, 0x6100,0xFE8C,0x4A40,0x671A,0x204D, NULL};
UWORD dat_int_toolsdaemon_702[]			= { 5, 0x0, 0x48E7,0x003E,0x2649,0x2A48,0x49F9,     9,  0xE, 0x2C78,0x0004,0x4EAE,0xFF7C,0x204D,0x6100,0xF968,0x4A40,0x661E, NULL};
UWORD dat_int_qmouse_816[]				= {16, 0x0, 0x6100,0xFEC4,0x48E7,0x0CFE,0x2A7A,0x0818,0x45ED,0x01F4,0x781F,0x97CB,0x2A2A,0x0010,0x6602,0x264A,0xB1C5,0x670E, NULL};

struct LVORecog LVO_Intuition_Recog[ KNOWN_INTUITION_PATCHES ] =
{
	{ CEDPATCH_PID,			XI_RES_PATCH,	-54,	dat_int_cedpatch_54 },		// ClearMenuStrip -54
	{ MAGICMENU_PID,		XI_RES_PATCH,	-54,	dat_int_magicmenu_54 },		// ClearMenuStrip -54
	{ TOOLSDAEMON_PID,		XI_RES_PATCH,	-54,	dat_int_toolsdaemon_54 },	// ClearMenuStrip -54
	{ QMOUSE_PID,			XI_RES_PATCH,	-60,	dat_int_qmouse_60 },		// ClearPointer -60
	{ QMOUSE_PID,			XI_RES_PATCH,	-66,	dat_int_qmouse_66 },		// CloseScreen -66
	{ POWERSNAP_PID,		XI_RES_PATCH,	-66,	dat_int_powersnap_66 },		// CloseScreen -66
	{ MAGICMENU_PID,		XI_RES_PATCH,	-72,	dat_int_magicmenu_72 },		// CloseWindow -72
	{ QMOUSE_PID,			XI_RES_PATCH,	-72,	dat_int_qmouse_72 },		// CloseWindow -72
	{ IPREFS_PID,			XI_RES_PATCH,	-78,	dat_int_iprefs_78 },		// CloseWorkbench -78
	{ QMOUSE_PID,			XI_RES_PATCH,	-78,	dat_int_qmouse_78 },		// CloseWorkbench -78
	{ IPREFS_PID,			XI_RES_PATCH,	-96,	dat_int_iprefs_96 },		// DisplayBeep -96
	{ QMOUSE_PID,			XI_RES_PATCH,	-96,	dat_int_qmouse_96 },		// DisplayBeep -96
	{ AUTOCENTRE_PID,		XI_RES_PATCH,	-198,	dat_int_autocentre_198 },	// OpenScreen -198
	{ RTPATCH_PID,			XI_RES_PATCH,	-198,	dat_int_rtpatch_198 },		// OpenScreen -198
	{ MAGICMENU_PID,		XI_RES_PATCH,	-204,	dat_int_magicmenu_204 },	// OpenWindow -204
	{ NEWMENU_PID,			XI_RES_PATCH,	-204,	dat_int_newmenu_204 },		// OpenWindow -204
	{ CEDPATCH_PID,			XI_RES_PATCH,	-264,	dat_int_cedpatch_264 },		// SetMenuStrip -264
	{ MAGICMENU_PID,		XI_RES_PATCH,	-264,	dat_int_magicmenu_264 },	// SetMenuStrip -264
	{ TOOLSDAEMON_PID,		XI_RES_PATCH,	-264,	dat_int_toolsdaemon_264 },	// SetMenuStrip -264
	{ QMOUSE_PID,			XI_RES_PATCH,	-270,	dat_int_qmouse_270 },		// SetPointer -270
	{ MAGICMENU_PID,		XI_RES_PATCH,	-312,	dat_int_magicmenu_312 },	// WindowToFront -312
	{ RTPATCH_PID,			XI_RES_PATCH,	-348,	dat_int_rtpatch_348 },		// AutoRequest -348
	{ MAGICMENU_PID,		XI_RES_PATCH,	-450,	dat_int_magicmenu_450 },	// ActivateWindow -450
	{ PROP3D_PID,			XI_RES_PATCH,	-468,	dat_int_prop3d_468 },		// NewModifyProp -468
	{ RTPATCH_PID,			XI_RES_PATCH,	-588,	dat_int_rtpatch_588 },		// EasyRequestArgs -588
	{ MAGICMENU_PID,		XI_RES_PATCH,	-606,	dat_int_magicmenu_606 },	// OpenWindowTagList -606
	{ NEWMENU_PID,			XI_RES_PATCH,	-606,	dat_int_newmenu_606 },		// OpenWindowTagList -606
	{ QMOUSE_PID,			XI_RES_PATCH,	-606,	dat_int_qmouse_606 },		// OpenWindowTagList -606
	{ AUTOCENTRE_PID,		XI_RES_PATCH,	-612,	dat_int_autocentre_612 },	// OpenScreenTagList -612
	{ CEDPATCH_PID,			XI_RES_PATCH,	-612,	dat_int_cedpatch_612 },		// OpenScreenTagList -612
	{ IPREFS_PID,			XI_RES_PATCH,	-612,	dat_int_iprefs_612 },		// OpenScreenTagList -612
	{ RTPATCH_PID,			XI_RES_PATCH,	-612,	dat_int_rtpatch_612 },		// OpenScreenTagList -612
	{ MAGICMENU_PID,		XI_RES_PATCH,	-702,	dat_int_magicmenu_702 },	// ResetMenuStrip -702
	{ TOOLSDAEMON_PID,		XI_RES_PATCH,	-702,	dat_int_toolsdaemon_702 },	// ResetMenuStrip -702
	{ QMOUSE_PID,			XI_RES_PATCH,	-816,	dat_int_qmouse_816 },		// SetWindowPointerA -816
};


#define		KNOWN_UTILITY_PATCHES				4

UWORD dat_uti_localelibrary_162[]			= {16, 0x0, 0x70FF,0x2F0A,0x2C7A,0xFFA0,0x7200,0x2449,0x2248,0x206E,0x008C,0x2068,0x00F4,0x4E90,0x2C6E,0x005C,0x245F,0x4E75, NULL};
UWORD dat_uti_localelibrary_168[]			= {15, 0x0, 0x2F0A,0x2C7A,0xFFA0,0x7200,0x2449,0x2248,0x206E,0x008C,0x2068,0x00F4,0x4E90,0x2C6E,0x005C,0x245F,0x4E75, NULL};
UWORD dat_uti_localelibrary_174[]			= {13, 0x0, 0x2C7A,0xFFD8,0x1200,0x7000,0x1001,0x206E,0x008C,0x2068,0x00B4,0x4E90,0x2C6E,0x005C,0x4E75, NULL};
UWORD dat_uti_localelibrary_180[]			= {13, 0x0, 0x2C7A,0xFFBE,0x1200,0x7000,0x1001,0x206E,0x008C,0x2068,0x00B0,0x4E90,0x2C6E,0x005C,0x4E75, NULL};

struct LVORecog LVO_Utility_Recog[ KNOWN_UTILITY_PATCHES ] =
{
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-162,	dat_uti_localelibrary_162 },	// Stricmp -162
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-168,	dat_uti_localelibrary_168 },	// Strnicmp -168
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-174,	dat_uti_localelibrary_174 },	// ToUpper -174
	{ LOCALELIBRARY_PID,	XI_RES_PATCH,	-180,	dat_uti_localelibrary_180 },	// ToLower -180
};

#define		KNOWN_WORKBENCH_PATCHES				10

UWORD dat_wb_directoryopus_48[]			= {16, 0x0, 0x9EFC,0x0014,0x48E7,0x213E,0x2401,0x2E00,0x2649,0x2A48,0x2F4A,0x0028,0x49EE,0x0034,0x2F4E,0x002C,0x200D,0x6704, NULL};
UWORD dat_wb_directoryopus_54[]			= { 9, 0x0, 0x594F,0x48E7,0x003E,0x2A48,0x49EE,0x0034,0x2F4E,0x0014,0x200D, NULL};
UWORD dat_wb_directoryopus_60[]			= {16, 0x0, 0x9EFC,0x001C,0x48E7,0x392E,0x2401,0x2600,0x2E0A,0x2F4C,0x0030,0x49EE,0x0034,0x9BCD,0x7800,0x42AF,0x0038,0x2F48, NULL};
UWORD dat_wb_directoryopus_60_2[]		= {16, 0x0, 0x9EFC,0x0018,0x48E7,0x232E,0x2401,0x2C00,0x2E0A,0x2F4C,0x002C,0x49EE,0x0034,0x2F48,0x0024,0x2F49,0x0028,0x2F4E, NULL};
UWORD dat_wb_directoryopus_66[]			= {16, 0x0, 0x9EFC,0x000C,0x48E7,0x303E,0x2A48,0x49EE,0x0034,0x2F4E,0x0020,0x200D,0x6774,0x43FA,0xFA90,0x7000,0x2C78,0x0004, NULL};
UWORD dat_wb_directoryopus_66_2[]		= {10, 0xC, 0x2F4E,0x0018,0x200D,0x674E,0x43FA,0xFB9A,0x7000,0x2C78,0x0004,0x4EAE, NULL};
UWORD dat_wb_directoryopus_72[]			= {16, 0x0, 0x9EFC,0x0018,0x48E7,0x343E,0x2401,0x2600,0x2A49,0x2F4A,0x002C,0x49EE,0x0034,0x97CB,0x7A00,0x42AF,0x0034,0x2F48, NULL};
UWORD dat_wb_directoryopus_72_2[]		= {10, 0x8, 0x2401,0x2E00,0x2A49,0x2F4A,0x0028,0x49EE,0x0034,0x2F48,0x0024,0x2F4E, NULL};
UWORD dat_wb_directoryopus_78[]			= {16, 0x0, 0x9EFC,0x000C,0x48E7,0x303E,0x2A48,0x49EE,0x0034,0x2F4E,0x0020,0x200D,0x6774,0x43FA,0xFA0A,0x7000,0x2C78,0x0004, NULL};
UWORD dat_wb_directoryopus_78_2[]		= {10,0x24, 0x4A80,0x2C6F,0x0018,0x6738,0x246B,0x002E,0x2F0A,0x2F0D,0x6100,0x016A, NULL};

struct LVORecog LVO_Workbench_Recog[ KNOWN_WORKBENCH_PATCHES ] =
{
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-48,	dat_wb_directoryopus_48 },		// AddAppWindowA -48
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-54,	dat_wb_directoryopus_54 },		// RemoveAppWindow -54
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-60,	dat_wb_directoryopus_60 },		// AddAppIconA -60
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-60,	dat_wb_directoryopus_60_2 },	// AddAppIconA -60
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-66,	dat_wb_directoryopus_66 },		// RemoveAppIcon -66
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-66,	dat_wb_directoryopus_66_2 },	// RemoveAppIcon -66
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-72,	dat_wb_directoryopus_72 },		// AddAppMenuItemA -72
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-72,	dat_wb_directoryopus_72_2 },	// AddAppMenuItemA -72
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-78,	dat_wb_directoryopus_78 },		// RemoveAppMenuItem -78
	{ DIRECTORYOPUS_PID,	XI_RES_PATCH,	-78,	dat_wb_directoryopus_78_2 },	// RemoveAppMenuItem -78
};


struct Library *	DOSBase;

/*
 * Allocate the main structure used
 */
struct XtruderInfo * ASM xlAllocXtruderInfo( VOID )
{
	struct XtruderInfo *	xi;
	
	
	if( xi = AllocVec( sizeof( struct XtruderInfo ), MEMF_CLEAR ) )
		xi->xi_KnownPatches = KNOWNPATCHES;
	
	return( xi );
}

/*
 * Free the above
 */
VOID ASM xlFreeXtruderInfo( REGA0 struct XtruderInfo *xi )
{
	FreeVec( xi );
}

/*
 * Check an offset in a library to see if it's a patch,virus or unknown
 */
BOOL __asm xlCheckLVO( REGA0 struct XtruderInfo *xi )
{
	struct LVORecog *	lvor = NULL;
	BOOL				success = FALSE;
	ULONG *				ad;
	ULONG				temp;
	UWORD				n,endlvos;
	
	
	if( xi->xi_LibBase )
		{
		xi->xi_Result = 0;
		
		switch( xi->xi_Library )
			{
			case XI_LIB_DOS:
				lvor = LVO_Dos_Recog;
				
				endlvos = KNOWN_DOS_PATCHES;
				break;
			
			case XI_LIB_EXEC:
				lvor = LVO_Exec_Recog;
				
				endlvos = KNOWN_EXEC_PATCHES;
				break;
			
			case XI_LIB_GADTOOLS:
				lvor = LVO_GadTools_Recog;
				
				endlvos = KNOWN_GADTOOLS_PATCHES;
				break;
			
			case XI_LIB_GRAPHICS:
				lvor = LVO_Graphics_Recog;
				
				endlvos = KNOWN_GRAPHICS_PATCHES;
				break;
			
			case XI_LIB_INTUITION:
				lvor = LVO_Intuition_Recog;
				
				endlvos = KNOWN_INTUITION_PATCHES;
				break;
			
			case XI_LIB_UTILITY:
				lvor = LVO_Utility_Recog;
				
				endlvos = KNOWN_UTILITY_PATCHES;
				break;
			
			case XI_LIB_WORKBENCH:
				lvor = LVO_Workbench_Recog;
				
				endlvos = KNOWN_WORKBENCH_PATCHES;
				break;
			}
		
		if( lvor )
			{
			for( n = 0; n < endlvos; n++ )
				{
				if( lvor[n].lr_LVOOffset + 2 == xi->xi_LVOOffset )
					{
					UWORD *	adr;
					
					
					temp = (ULONG)xi->xi_LibBase + xi->xi_LVOOffset;
					ad = (ULONG *)temp;
					
					adr = (UWORD *)*ad;
					
						/// Is this a "jump" patch ?
					
					if( (lvor[ n ].lr_Recog[ 2 ] == 0x4EF9) && (adr[0] == 0x4EF9) )
						{
						UWORD	numrecogs = lvor[ n ].lr_Recog[ 0 ];
						UWORD	recogoffset = lvor[ n ].lr_Recog[ 1 ];
						UWORD *	origadr;
						UBYTE *	temp;
						UWORD	recog;
						UWORD	endrecogs;
						BOOL	donewithrecog;
						ULONG	newa;
						
							/// Redirect address to the code which the
							/// patch would jmp to
						
						adr++;
						newa = *(ULONG *)adr;
						adr = (UWORD *)newa;
						
							/// Adjust check address to offset
						
						temp = (UBYTE *)adr;
						
						origadr = adr;
						
						adr = (UWORD *)(temp + recogoffset);
						
						donewithrecog = FALSE;
						
							/// Skip jump instruction
						
						recog = 3;
						
						while( !donewithrecog )
							{
							endrecogs = recog + numrecogs;
							
							for( ; recog < endrecogs; recog++, adr++ )
								{
								if( *adr != lvor[ n ].lr_Recog[ recog ] )
									{
										/// Mismatch - Abort
									
									donewithrecog = TRUE;
									
									break;
									}
								else
									{
									if( (recog + 1 == endrecogs) &&
										(lvor[ n ].lr_Recog[ endrecogs ] == NULL) )
										{
										xi->xi_Result = lvor[ n ].lr_Type;
										
										if( lvor[ n ].lr_Type == XI_RES_PATCH )
											strcpy( xi->xi_Name, LVOPatches[ lvor[ n ].lr_LVOName ] );
										else
											strcpy( xi->xi_Name, LVOVira[ lvor[ n ].lr_LVOName ] );
										
											/// This section of recog matches
										
										donewithrecog = success = TRUE;
										
										break;
										}
									}
								}
							
							if( !donewithrecog )
								{
								recog     += 2;
								numrecogs = lvor[ n ].lr_Recog[ endrecogs ];
								
								adr = (UWORD *)((UBYTE *)origadr + lvor[ n ].lr_Recog[ endrecogs + 1 ]);
								}
							}
						}
					else
						{
						UWORD	numrecogs = lvor[ n ].lr_Recog[ 0 ];
						UWORD	recogoffset = lvor[ n ].lr_Recog[ 1 ];
						UWORD *	origadr = adr;
						UBYTE *	temp;
						UWORD	recog;
						UWORD	endrecogs;
						BOOL	donewithrecog;
						
						
							/// Adjust check address to offset
						
						temp = (UBYTE *)adr;
						
						adr = (UWORD *)(temp + recogoffset);
						
						donewithrecog = FALSE;
						recog = 2;
						
						while( !donewithrecog )
							{
							endrecogs = recog + numrecogs;
							
							for( ; recog < endrecogs; recog++, adr++ )
								{
								if( *adr != lvor[ n ].lr_Recog[ recog ] )
									{
										/// Mismatch - Abort
									
									donewithrecog = TRUE;
									
									break;
									}
								else
									{
									if( (recog + 1 == endrecogs) &&
										(lvor[ n ].lr_Recog[ endrecogs ] == NULL) )
										{
										xi->xi_Result = lvor[ n ].lr_Type;
										
										if( lvor[ n ].lr_Type == XI_RES_PATCH )
											strcpy( xi->xi_Name, LVOPatches[ lvor[ n ].lr_LVOName ] );
										else
											strcpy( xi->xi_Name, LVOVira[ lvor[ n ].lr_LVOName ] );
										
											/// This section of recog matches
										
										donewithrecog = success = TRUE;
										
										break;
										}
									}
								}
							
							if( !donewithrecog )
								{
								recog     += 2;
								numrecogs = lvor[ n ].lr_Recog[ endrecogs ];
								
								adr = (UWORD *)((UBYTE *)origadr + lvor[ n ].lr_Recog[ endrecogs + 1 ]);
								}
							}
						}
					}
				
				if( xi->xi_Result != 0 )
					break;
				}
			}
		}
	
	return( success );
}

STRPTR __asm xlPatchListFirst( REGA0 struct XtruderInfo *xi )
{
	xi->xi_Result = 0;
	
	return( LVOPatches[0] );
}

STRPTR __asm xlPatchListNext( REGA0 struct XtruderInfo *xi )
{
	STRPTR	next = NULL;
	
	
	if( (xi->xi_Result + 1) < KNOWNPATCHES )
		{
		xi->xi_Result++;
		
		next = LVOPatches[xi->xi_Result];
		}
	
	return( next );
}
