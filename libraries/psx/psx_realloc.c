/* Subroutine:  psx_realloc( size, pntr, status )
*+
*  Name:
*     PSX_REALLOC

*  Purpose:
*     Change the size of an allocated region of virtual memory

*  Language:
*     ANSI C

*  Invocation:
*     CALL PSX_REALLOC( SIZE, PNTR, STATUS )

*  Description:
*     The routine changes the size of the region of virtual memory
*     pointed to by PNTR. The new size may be larger or smaller than
*     the old size. The contents of the object pointed to by PNTR
*     shall be unchanged up to the lesser of the old and new sizes.

*  Arguments:
*     SIZE = INTEGER (Given)
*        The new amount of virtual memory required
*     PNTR = POINTER (Given and Returned)
*        A pointer to the allocated storage
*     STATUS = INTEGER (Given)
*        The global status

*  Examples:
*        CALL PSX_MALLOC( 20, PNTR, STATUS )
*           ...
*        CALL PSX_REALLOC( 40, PNTR, STATUS )
*        CALL SUB1( %VAL(PNTR), 10, STATUS )
*           ...
*        SUBROUTINE SUB1( ARRAY, N, STATUS )
*        INTEGER N
*        INTEGER ARRAY( N )
*          ...
*
*        Allocates 20 bytes of storage, then extends it to 40 bytes.
*
*        The call to PSX_MALLOC allocates twenty bytes of storage. The
*        subsequent call to PSX_REALLOC extends this area to forty
*        bytes.  The pointer to this storage is then passed to
*        subroutine SUB1, where it is accessed as an array of integers.
*        We assume SUB1 returns without action if STATUS is bad.
*
*        Note that in this case the program needs to know that an
*        INTEGER variable is stored in four bytes. THIS IS NOT
*        PORTABLE. In such a case it is better to use the symbolic
*        constants NUM_NB<T> defined in the file PRM_PAR to determine
*        the number of bytes per unit of storage. (See SUN/39 for a
*        description of these constants).

*  Notes:
*     -  Storage allocated by PSX_REALLOC should be returned by a call
*        to PSX_FREE when it is no longer needed.
*     -  PNTR is declared to be of type POINTER. This is usually
*        represented in FORTRAN as an INTEGER, although any type that
*        uses the same amount of storage would be just as good.
*        The pointer will have been registered for C and FORTRAN use,
*        according to the scheme described in SUN/209, allowing its use
*        where pointers are longer than INTEGERs. For portability, the
*        construct %VAL(CNF_PVAL(PNTR)), rather than simply %VAL(PNTR),
*        should be used to pass the pointer to the subroutine. Function
*        CNF_PVAL is described in SUN/209 Section `Pointers'.
*     -  If SIZE is zero, then the space pointed to by PNTR is freed.
*     -  If the space that PNTR pointed to has been deallocated by a
*        call to PSX_FREE (or to PSX_REALLOC with SIZE = 0), then it is
*        undefined whether the pointer can subsequently be used by
*        PSX_REALLOC.  Consequently this should not be attempted, even
*        though it will work on some machines.
*     -  The value of PNTR may be changed by this routine as it may be
*        necessary to allocate a new region of memory and copy the
*        contents of the old region into the new one.

*  External Routines Used:
*     cnf: cnfCptr, cnfFptr, cnfMalloc, cnfRegp, cnfUregp

*  References:
*     -  POSIX standard (1988), section 8.1
*     -  ANSI C standard (1989), section 4.10.3.4

*  Copyright:
*     Copyright (C) 1991 Science & Engineering Research Council

*  Authors:
*     PMA: Peter Allan (Starlink, RAL)
*     RFWS: R.F. Warren-Smith (Starlink, RAL)
*     AJC: A.J. Chipperfield (Starlink, RAL)
*     TIMJ: Tim Jenness (JAC, Hawaii)
*     {enter_new_authors_here}

*  History:
*     5-MAR-1991 (PMA):
*        Original version.
*     27-JUN-1991 (PMA):
*        Changed IMPORT and EXPORT macros to GENPTR.
*     30-JAN-1992 (PMA):
*        Fix bug on incorrect use of pntr and add error reporting.
*     14-APR-1993 (PMA):
*        Cast the temporary pointer to an F77 pointer.
*     14-APR-199 (RFWS):
*        Use CNF for memory allocation.
*     23-JUN-2000 (AJC):
*        Improve documentation re pointers
*        Tidy refs to CNF routines
*     6-DEC-2004 (TIMJ):
*        Report allocation failure as unsigned int
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
-----------------------------------------------------------------------------
*/

#include <config.h>

/* Global Constants:		.					    */

#include <stdio.h>
#include <stdlib.h>		 /* Standard C library			    */
#if STDC_HEADERS
#  include <string.h>
#endif
#include "f77.h"		 /* C - Fortran interface		    */
#include "psx_err.h"             /* PSX error values                        */
#include "sae_par.h"		 /* ADAM constants			    */
#include "psx1.h"                /* declares psx1_rep_c */

F77_SUBROUTINE(psx_realloc)( INTEGER(size), POINTER(pntr), INTEGER(status) )
{

/* Pointers to Arguments:						    */

   GENPTR_INTEGER(size)
   GENPTR_POINTER(pntr)
   GENPTR_INTEGER(status)

/* Local Variables:							    */

   char errbuf[100];		 /* Buffer for error message		    */
   int reg;                      /* Registration error status               */
   size_t csize;                 /* Required memory size                    */
   void *cpntr;                  /* C version of input pointer              */
   void *p;                      /* Temporary pointer value                 */
   void *temp;			 /* Temporary return value from malloc 	    */

/* Check inherited global status.					    */

   if( *status != SAI__OK ) return;

/* Convert the incoming Fortran pointer to a C pointer and obtain the new   */
/* size.                                                                    */

   cpntr = cnfCptr( *pntr );
   csize = (size_t) *size;

/* Re-allocate the space.		                                    */

   temp = realloc( cpntr, csize );

/* If a pointer to new memory was returned, then un-register the old        */
/* pointer (if not NULL).                                                   */

   if ( ( temp != cpntr ) && ( cpntr != NULL ) ) cnfUregp( cpntr );

/* If a pointer to new memory was returned, attempt to register the new     */
/* pointer (if not NULL).                                                   */
       
   if ( ( temp != cpntr ) && ( temp != NULL ) )
   {
      reg = cnfRegp( temp );

/* If it could not be registered, then attempt to allocate some new memory  */
/* with a registered pointer associated with it.                            */

      if ( !reg )
      {
         p = cnfMalloc( csize );

/* If successful, transfer the data to the new (registered) memory and free */
/* the memory which could not be registered.                                */

         if ( p )
         {
            (void) memcpy( p, temp, csize );
            free( temp );
            temp = p;
         }
         else

/* If no registered memory was available, free the unregistered memory and  */
/* set the returned pointer to NULL.                                        */
         {
            free( temp );
            temp = NULL;
         }
      }

/* If an error occurred during pointer registration, free the unregistered  */
/* memory and set the returned pointer to NULL.                             */

      else if ( reg < 0 )
      {
         free( temp );
         temp = NULL;
      }
   }

/* Check that the space was allocated.					    */

   if( temp != 0 )

/* Copy the pointer to the allocated storage space to the subroutine	    */
/* argument, converting to a Fortran pointer.				    */

   {
      *pntr = cnfFptr( temp );
   }
   else

/* Set STATUS to an error code and report the error.			    */

   {
      *pntr = (F77_POINTER_TYPE)0;
      *status = PSX__NOALL;
      sprintf( errbuf, 
         "Failed to allocate space with realloc. %u bytes requested",
         csize );
      psx1_rep_c( "PSX_REALLOC_NOALL", errbuf, status );
   }

}
