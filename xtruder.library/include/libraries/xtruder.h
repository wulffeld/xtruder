#ifndef XTRUDER_BASE_H
#define XTRUDER_BASE_H

#ifndef  EXEC_TYPES_H
#include  <exec/types.h>
#endif

#ifndef  EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif


struct XtruderBase
{
	struct Library	libnode;
	UBYTE			flags;
	UBYTE			pad;
	
	/* We are now longword aligned */
	ULONG			SysLib;
	ULONG			DosLib;
	ULONG			IntLib;
	ULONG			GfxLib;
	ULONG			UtilityLib;
	ULONG			SegList;
};


#define XTRUDERNAME   "xtruder.library"

struct XtruderInfo
{
	UBYTE *	xi_Buffer;					// E.g. File data
	ULONG	xi_BufferSize;				// Size of fucking buffer
	ULONG *	xi_MemAdrStart;
	ULONG *	xi_MemAdrEnd;
	ULONG *	xi_LibBase;					// Base of a library
	WORD	xi_LVOOffset;				// Function
	UWORD	xi_Library;					// Which library See below
	
	UWORD	xi_KnownVira;				// Number of known vira
	UWORD	xi_KnownPatches;			// Number of known patches
	
	UWORD	xi_Result;					// See below
	UWORD	xi_Error;					// See below
	UBYTE	xi_Name[50];				// Virus or patch name
};

/*
** xi_Library
*/
#define		XI_LIB_DOS						1
#define		XI_LIB_EXEC						2
#define		XI_LIB_GRAPHICS					3
#define		XI_LIB_INTUITION				4
#define		XI_LIB_UTILITY					5
#define		XI_LIB_ICON						6
#define		XI_LIB_WORKBENCH				7
#define		XI_LIB_GADTOOLS					8
#define		XI_LIB_KEYMAP					9

/*
** xi_Result...
*/
#define		XI_RES_VIRUS					1
#define		XI_RES_PATCH					2
#define		XI_RES_UNKNOWN					50

#endif
