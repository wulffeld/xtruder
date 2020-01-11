#ifndef XTRUDER_PROTOS_H
#define XTRUDER_PROTOS_H

struct XtruderInfo * ASM xlAllocXtruderInfo( VOID );
VOID ASM xlFreeXtruderInfo( REGA0 struct XtruderInfo * );
BOOL ASM xlCheckLVO( REGA0 struct XtruderInfo * );
STRPTR ASM xlPatchListFirst( REGA0 struct XtruderInfo * );
STRPTR ASM xlPatchListNext( REGA0 struct XtruderInfo * );
#endif
