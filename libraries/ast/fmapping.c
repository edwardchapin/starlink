/*
*+
*  Name:
*     fmapping.c

*  Purpose:
*     Define a FORTRAN 77 interface to the AST Mapping class.

*  Type of Module:
*     C source file.

*  Description:
*     This file defines FORTRAN 77-callable C functions which provide
*     a public FORTRAN 77 interface to the Mapping class.

*  Routines Defined:
*     AST_INVERT
*     AST_ISAMAPPING
*     AST_RESAMPLE<X>
*     AST_SIMPLIFY
*     AST_TRAN1
*     AST_TRAN2
*     AST_TRANN

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     RFWS: R.F. Warren-Smith (Starlink)

*  History:
*     11-JUL-1996 (RFWS):
*        Original version.
*     13-DEC-1996 (RFWS)
*        Added AST_SIMPLIFY.
*     28-MAY-1998 (RFWS):
*        Added AST_MAPBOX.
*     12-NOV-1998 (RFWS):
*        Added AST_RESAMPLE<X>.
*/

/* Define the astFORTRAN77 macro which prevents error messages from
   AST C functions from reporting the file and line number where the
   error occurred (since these would refer to this file, they would
   not be useful). */
#define astFORTRAN77

/* Header files. */
/* ============= */
#include "f77.h"                 /* FORTRAN <-> C interface macros (SUN/209) */
#include "c2f77.h"               /* F77 <-> C support functions/macros */
#include "error.h"               /* Error reporting facilities */
#include "mapping.h"             /* C interface to the Mapping class */

/* Module Variables. */
/* ================= */
/* Pointer to user-supplied (FORTRAN 77) interpolation function for
   use by AST_RESAMPLE<X>. */
static void (* ast_resample_FINTERP)();

/* Interpolation function interface. */
/* ================================= */
/* These functions are associated with allowing FORTRAN 77
   implementations of interpolation functions to be passed to
   AST_RESAMPLE<X> via the FORTRAN 77 interface and then to be invoked
   when necessary by the C code in the main implementation of
   astResample<X>. */

/* Define a macro which defines an interface function called
   ast_resample_interp<X> for a specific data type.

   The resulting function has a suitable interface to allow it to be
   passed as an interpolation function to the C interface
   (astResample<X>). In turn, it invokes the user-supplied FORTRAN 77
   interpolation function, a pointer to which should previously have
   been stored in the static variable "ast_resample_FINTERP". */
#define MAKE_AST_RESAMPLE_UINTERP(X,Xtype,Ftype) \
static void ast_resample_uinterp##X( int ndim, \
                                     const int lbnd[], const int ubnd[], \
                                     const Xtype in[], const Xtype in_var[], \
                                     int npoint, const int offset[], \
                                     const double *const coords[], \
                                     const double params[], int flags, \
                                     Xtype badval, \
                                     Xtype *out, Xtype *out_var, \
                                     int *nbad ) { \
   DECLARE_INTEGER(STATUS); \
\
/* Obtain the C status and then invoke the FORTRAN 77 interpolation \
   function via the stored pointer. Note that the "coords" array we \
   pass to FORTRAN has to be a contiguous 2-d array, so we must \
   de-reference one level of pointer compared to the C case. */ \
   STATUS = astStatus; \
   ( *ast_resample_FINTERP )( INTEGER_ARG(&ndim), \
                              INTEGER_ARRAY_ARG(lbnd), \
                              INTEGER_ARRAY_ARG(ubnd), \
                              Ftype##_ARRAY_ARG(in), \
                              Ftype##_ARRAY_ARG(in_var), \
                              INTEGER_ARG(&npoint), \
                              INTEGER_ARRAY_ARG(offset), \
                              DOUBLE_ARRAY_ARG(coords[ 0 ]), \
                              DOUBLE_ARRAY_ARG(params), \
                              INTEGER_ARG(flags), \
                              Ftype##_ARG(&badval), \
                              Ftype##_ARRAY_ARG(out), \
                              Ftype##_ARRAY_ARG(out_var), \
                              INTEGER_ARG(nbad), \
                              INTEGER_ARG(&STATUS) ); \
\
/* Set the C status to the returned FORTRAN 77 status. */ \
   astSetStatus( STATUS ); \
}

/* Invoke the above macro to define an interface function for each
   required data type. */
MAKE_AST_RESAMPLE_UINTERP(D,double,DOUBLE)
MAKE_AST_RESAMPLE_UINTERP(F,float,REAL)
MAKE_AST_RESAMPLE_UINTERP(I,int,INTEGER)
MAKE_AST_RESAMPLE_UINTERP(S,short int,WORD)
MAKE_AST_RESAMPLE_UINTERP(US,unsigned short int,UWORD)
MAKE_AST_RESAMPLE_UINTERP(B,signed char,BYTE)
MAKE_AST_RESAMPLE_UINTERP(UB,unsigned char,UBYTE)

/* Undefine the macro. */
#undef MAKE_AST_RESAMPLE_UINTERP

/* FORTRAN interface functions. */
/* ============================ */
/* These functions implement the remainder of the FORTRAN interface. */
F77_SUBROUTINE(ast_invert)( INTEGER(THIS),
                            INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)

   astAt( "AST_INVERT", NULL, 0 );
   astWatchSTATUS(
      astInvert( astI2P( *THIS ) );
   )
}

F77_LOGICAL_FUNCTION(ast_isamapping)( INTEGER(THIS),
                                      INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   F77_LOGICAL_TYPE(RESULT);

   astAt( "AST_ISAMAPPING", NULL, 0 );
   astWatchSTATUS(
      RESULT = astIsAMapping( astI2P( *THIS ) ) ? F77_TRUE : F77_FALSE;
   )
   return RESULT;
}

F77_SUBROUTINE(ast_mapbox)( INTEGER(THIS),
                            DOUBLE_ARRAY(LBND_IN),
                            DOUBLE_ARRAY(UBND_IN),
                            LOGICAL(FORWARD),
                            INTEGER(COORD_OUT),
                            DOUBLE(LBND_OUT),
                            DOUBLE(UBND_OUT),
                            DOUBLE_ARRAY(XL),
                            DOUBLE_ARRAY(XU),
                            INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   GENPTR_DOUBLE_ARRAY(LBND_IN)
   GENPTR_DOUBLE_ARRAY(UBND_IN)
   GENPTR_LOGICAL(FORWARD)
   GENPTR_INTEGER(COORD_OUT)
   GENPTR_DOUBLE(LBND_OUT)
   GENPTR_DOUBLE(UBND_OUT)
   GENPTR_DOUBLE_ARRAY(XL)
   GENPTR_DOUBLE_ARRAY(XU)
   double lbnd_out;
   double ubnd_out;

   astAt( "AST_MAPBOX", NULL, 0 );
   astWatchSTATUS(
      astMapBox( astI2P( *THIS ), LBND_IN, UBND_IN, F77_ISTRUE( *FORWARD ),
                 *COORD_OUT, &lbnd_out, &ubnd_out, XL, XU );
      *LBND_OUT = lbnd_out;
      *UBND_OUT = ubnd_out;
   )
}

/* AST_RESAMPLE<X> requires a function for each possible data type, so
   define it via a macro. */
#define MAKE_AST_RESAMPLE(f,F,Ftype,X,Xtype) \
F77_INTEGER_FUNCTION(ast_resample##f)( INTEGER(THIS), \
                                       INTEGER(NDIM_IN), \
                                       INTEGER_ARRAY(LBND_IN), \
                                       INTEGER_ARRAY(UBND_IN), \
                                       Ftype##_ARRAY(IN), \
                                       Ftype##_ARRAY(IN_VAR), \
                                       INTEGER(INTERP), \
                                       void (* FINTERP)(), \
                                       DOUBLE_ARRAY(PARAMS), \
                                       INTEGER(FLAGS), \
                                       DOUBLE(TOL), \
                                       INTEGER(MAXPIX), \
                                       Ftype(BADVAL), \
                                       INTEGER(NDIM_OUT), \
                                       INTEGER_ARRAY(LBND_OUT), \
                                       INTEGER_ARRAY(UBND_OUT), \
                                       INTEGER_ARRAY(LBND), \
                                       INTEGER_ARRAY(UBND), \
                                       Ftype##_ARRAY(OUT), \
                                       Ftype##_ARRAY(OUT_VAR), \
                                       INTEGER(STATUS) ) { \
   GENPTR_INTEGER(THIS) \
   GENPTR_INTEGER(NDIM_IN) \
   GENPTR_INTEGER_ARRAY(LBND_IN) \
   GENPTR_INTEGER_ARRAY(UBND_IN) \
   GENPTR_##Ftype##_ARRAY(IN) \
   GENPTR_##Ftype##_ARRAY(IN_VAR) \
   GENPTR_INTEGER(INTERP) \
   GENPTR_DOUBLE_ARRAY(PARAMS) \
   GENPTR_INTEGER(FLAGS) \
   GENPTR_DOUBLE(TOL) \
   GENPTR_INTEGER(MAXPIX) \
   GENPTR_##Ftype(BADVAL) \
   GENPTR_INTEGER(NDIM_OUT) \
   GENPTR_INTEGER_ARRAY(LBND_OUT) \
   GENPTR_INTEGER_ARRAY(UBND_OUT) \
   GENPTR_INTEGER_ARRAY(LBND) \
   GENPTR_INTEGER_ARRAY(UBND) \
   GENPTR_##Ftype##_ARRAY(OUT) \
   GENPTR_##Ftype##_ARRAY(OUT_VAR) \
   GENPTR_INTEGER(STATUS) \
\
   Xtype *out_var; \
   const Xtype *in_var; \
   void (* finterp)(); \
   F77_INTEGER_TYPE RESULT; \
\
   astAt( "AST_RESAMPLE"#F, NULL, 0 ); \
   astWatchSTATUS( \
\
/* If *INTERP is set to AST__UINTERP, store a pointer to the \
   user-supplied FORTRAN 77 interpolation function and use the \
   "ast_resample_uinterp<X>" C wrapper function to invoke it. */ \
      if ( *INTERP == AST__UINTERP ) { \
         ast_resample_FINTERP = FINTERP; \
         finterp = (void (*)()) ast_resample_uinterp##X; \
      } \
\
/* If the AST__USEVAR flag is set, use the input and output variance \
   arrays, otherwise pass NULL pointers. */ \
      in_var = out_var = NULL; \
      if ( AST__USEVAR & *FLAGS ) { \
         in_var = IN_VAR; \
         out_var = OUT_VAR; \
      } \
      RESULT = astResample##X( astI2P( *THIS ), *NDIM_IN, \
                               LBND_IN, UBND_IN, IN, in_var, \
                               *INTERP, finterp, PARAMS, *FLAGS, \
                               *TOL, *MAXPIX, *BADVAL, \
                               *NDIM_OUT, LBND_OUT, UBND_OUT, \
                               LBND, UBND, OUT, out_var ); \
   ) \
   return RESULT; \
}

/* Invoke the above macro to define a function for each data
   type. Include synonyms for some functions. */
MAKE_AST_RESAMPLE(d,D,DOUBLE,D,double)
MAKE_AST_RESAMPLE(r,R,REAL,F,float)
MAKE_AST_RESAMPLE(i,I,INTEGER,I,int)
MAKE_AST_RESAMPLE(s,S,WORD,S,short int)
MAKE_AST_RESAMPLE(us,US,UWORD,US,unsigned short int)
MAKE_AST_RESAMPLE(w,W,WORD,S,short int)
MAKE_AST_RESAMPLE(uw,UW,UWORD,US,unsigned short int)
MAKE_AST_RESAMPLE(b,B,BYTE,B,signed char)
MAKE_AST_RESAMPLE(ub,UB,UBYTE,UB,unsigned char)
#undef MAKE_AST_RESAMPLE

F77_INTEGER_FUNCTION(ast_simplify)( INTEGER(THIS),
                                    INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   F77_INTEGER_TYPE(RESULT);

   astAt( "AST_SIMPLIFY", NULL, 0 );
   astWatchSTATUS(
      RESULT = astP2I( astSimplify( astI2P( *THIS ) ) );
   )
   return RESULT;
}

F77_SUBROUTINE(ast_tran1)( INTEGER(THIS),
                           INTEGER(NPOINT),
                           DOUBLE(XIN),
                           LOGICAL(FORWARD),
                           DOUBLE(XOUT),
                           INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   GENPTR_INTEGER(NPOINT)
   GENPTR_DOUBLE(XIN)
   GENPTR_LOGICAL(FORWARD)
   GENPTR_DOUBLE(XOUT)

   astAt( "AST_TRAN1", NULL, 0 );
   astWatchSTATUS(
      astTran1( astI2P( *THIS ), *NPOINT, XIN, F77_ISTRUE( *FORWARD ), XOUT );
   )
}

F77_SUBROUTINE(ast_tran2)( INTEGER(THIS),
                           INTEGER(NPOINT),
                           DOUBLE(XIN),
                           DOUBLE(YIN),
                           LOGICAL(FORWARD),
                           DOUBLE(XOUT),
                           DOUBLE(YOUT),
                           INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   GENPTR_INTEGER(NPOINT)
   GENPTR_DOUBLE(XIN)
   GENPTR_DOUBLE(YIN)
   GENPTR_LOGICAL(FORWARD)
   GENPTR_DOUBLE(XOUT)
   GENPTR_DOUBLE(YOUT)

   astAt( "AST_TRAN2", NULL, 0 );
   astWatchSTATUS(
      astTran2( astI2P( *THIS ), *NPOINT, XIN, YIN,
                F77_ISTRUE( *FORWARD ), XOUT, YOUT );
   )
}

F77_SUBROUTINE(ast_trann)( INTEGER(THIS),
                           INTEGER(NPOINT),
                           INTEGER(NCOORD_IN),
                           INTEGER(INDIM),
                           DOUBLE_ARRAY(IN),
                           LOGICAL(FORWARD),
                           INTEGER(NCOORD_OUT),
                           INTEGER(OUTDIM),
                           DOUBLE_ARRAY(OUT),
                           INTEGER(STATUS) ) {
   GENPTR_INTEGER(THIS)
   GENPTR_INTEGER(NPOINT)
   GENPTR_INTEGER(NCOORD_IN)
   GENPTR_INTEGER(INDIM)
   GENPTR_DOUBLE_ARRAY(IN)
   GENPTR_LOGICAL(FORWARD)
   GENPTR_INTEGER(NCOORD_OUT)
   GENPTR_INTEGER(OUTDIM)
   GENPTR_DOUBLE_ARRAY(OUT)

   astAt( "AST_TRANN", NULL, 0 );
   astWatchSTATUS(
      astTranN( astI2P( *THIS ), *NPOINT, *NCOORD_IN, *INDIM,
                (const double (*)[])IN, F77_ISTRUE( *FORWARD ),
                *NCOORD_OUT, *OUTDIM, (double (*)[]) OUT );
   )
}
