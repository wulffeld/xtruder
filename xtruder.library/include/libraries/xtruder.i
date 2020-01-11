	IFND	XTRUDER_BASE_I
XTRUDER_BASE_I SET 1


	IFND	EXEC_TYPES_I
	INCLUDE	"exec/types.i"
	ENDC

	IFND	EXEC_LIBRARIES_I
	INCLUDE	"exec/libraries.i"
	ENDC


STRUCTURE XtruderBase,LIB_SIZE
	UBYTE   xl_Flags
	UBYTE   xl_pad

	;We are now longword aligned
	ULONG   xl_SysLib
	ULONG   xl_DosLib
	ULONG   xl_IntLib
	ULONG   xl_GfxLib
	ULONG   xl_UtilityLib
	ULONG   xl_SegList
	LABEL   XtruderBase_SIZEOF

AOPNAME	MACRO
	dc.b	'xtruder.library',0
	ENDM

	ENDC	;XTRUDER_BASE_I

