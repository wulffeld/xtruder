/*
**	$VER: Xtruder_Header.h (26.4.97)
**
**	All the includes needed in Xtruder
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/


#define		INTUI_V36_NAMES_ONLY
#define		_USEOLDEXEC_
#define		USE_BUILTIN_MATH

#include	<exec/types.h>
#include	<exec/memory.h>
#include	<exec/tasks.h>
#include	<exec/execbase.h>
#include	<exec/io.h>
#include	<datatypes/pictureclass.h>
#include	<devices/trackdisk.h>
#include	<devices/console.h>
#include	<devices/conunit.h>
#include	<dos/stdio.h>
#include	<dos/dostags.h>
#include	<dos/dosextens.h>
#include	<dos/doshunks.h>
#include	<dos/rdargs.h>
#include	<graphics/gfxbase.h>
#include	<graphics/text.h>
#include	<graphics/gfxmacros.h>
#include	<intuition/intuition.h>
#include	<intuition/gadgetclass.h>
#include	<intuition/intuitionbase.h>
#include	<intuition/classusr.h>
#include	<intuition/imageclass.h>
#include	<intuition/gadgetclass.h>
#include	<intuition/cghooks.h>
#include	<intuition/icclass.h>
#include	<intuition/classes.h>
#include	<intuition/sghooks.h>
#include	<libraries/amigaguide.h>
#include	<libraries/BBBF.h>
#include	<libraries/commodities.h>
#include	<libraries/FileID.h>
#include	<libraries/filevirus.h>
#include	<libraries/gadtools.h>
#include	<libraries/locale.h>
#include	<libraries/reqtools.h>
#include	<libraries/unpack.h>
#include	<libraries/xfdmaster.h>
#include	<libraries/xtruder.h>
#include	<rexx/storage.h>
#include	<rexx/rxslib.h>
#include	<utility/tagitem.h>
#include	<utility/hooks.h>
#include	<workbench/startup.h>
#include	<workbench/workbench.h>
#include	<own/mw.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<stdarg.h>
#include	<fctype.h>

#include	<pragmas/amigaguide_pragmas.h>
#include	<pragmas/BBBF_pragmas.h>
#include	<pragmas/commodities_pragmas.h>
#include	<pragmas/datatypes_pragmas.h>
#include	<pragmas/diskfont_pragmas.h>
#include	<pragmas/dos_pragmas.h>
#include	<pragmas/exec_pragmas.h>
#include	<pragmas/fileid_pragmas.h>
#include	<pragmas/filevirus_pragmas.h>
#include	<pragmas/gadtools_pragmas.h>
#include	<pragmas/graphics_pragmas.h>
#include	<pragmas/icon_pragmas.h>
#include	<pragmas/intuition_pragmas.h>
#include	<pragmas/keymap_pragmas.h>
#include	<pragmas/locale_pragmas.h>
#include	<pragmas/reqtools.h>
#include	<pragmas/unpack.h>
#include	<pragmas/utility_pragmas.h>
#include	<pragmas/wb_pragmas.h>
#include	<pragmas/xfdmaster_pragmas.h>
#include	<pragmas/xtruder_pragmas.h>

#include	<clib/alib_protos.h>
#include	<clib/amigaguide_protos.h>
#include	<clib/BBBF_protos.h>
#include	<clib/commodities_protos.h>
#include	<clib/datatypes_protos.h>
#include	<clib/diskfont_protos.h>
#include	<clib/dos_protos.h>
#include	<clib/exec_protos.h>
#include	<clib/fileid_protos.h>
#include	<clib/filevirus_protos.h>
#include	<clib/gadtools_protos.h>
#include	<clib/graphics_protos.h>
#include	<clib/icon_protos.h>
#include	<clib/intuition_protos.h>
#include	<clib/keymap_protos.h>
#include	<clib/locale_protos.h>
#include	<clib/macros.h>
#include	<clib/reqtools_protos.h>
#include	<clib/unpack_protos.h>
#include	<clib/utility_protos.h>
#include	<clib/wb_protos.h>
#include	<clib/xfdmaster_protos.h>
#include	<clib/xtruder_protos.h>

#include	".Headers/Backfill.h"
#include	".Headers/Boopsi.h"
#include	".Headers/Defines.h"
#include	".Headers/Data.h"
#include	".Headers/Prototypes.h"
#include	".Headers/Strings.h"
#include	".Headers/Structures.h"
