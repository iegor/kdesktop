# ---- @OS_TYPE@/@ARCH_TYPE@ ---- 

# For cpu_accel compile we cannot have ansi
# (I dont have debugged why)

INCLUDES                =       $(all_includes)

noinst_LTLIBRARIES      =	libmmx.la
noinst_HEADERS		=	

kmpgincludedir          =       $(includedir)/$(THIS_LIB_NAME)/util/mmx

kmpginclude_HEADERS     =       mmx.h mmx_asm.h mm_accel.h

libmmx_la_SOURCES	=	cpu_accel.c mmx.c

AM_ASFLAGS		=	$(DEFS) $(DEFAULT_INCLUDES) $(all_includes)





























