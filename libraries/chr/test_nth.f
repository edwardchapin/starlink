      SUBROUTINE TEST_NTH(STATUS)
*+
*  Name:
*     TEST_NTH

*  Purpose:
*     Test CHR_NTH.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL TEST_NTH(STATUS)

*  Description:
*     Test CHR_NTH.
*     If any failure occurs, return STATUS = SAI__ERROR.
*     Otherwise, STATUS is unchanged.

*  Arguments:
*     STATUS = INTEGER (Returned)
*        The status of the tests. 

*  Copyright:
*     Copyright (C) 1989, 1993, 1994 Science & Engineering Research Council.
*     All Rights Reserved.

*  Licence:
*     This program is free software; you can redistribute it and/or
*     modify it under the terms of the GNU General Public License as
*     published by the Free Software Foundation; either version 2 of
*     the License, or (at your option) any later version.
*     
*     This program is distributed in the hope that it will be
*     useful,but WITHOUT ANY WARRANTY; without even the implied
*     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
*     PURPOSE. See the GNU General Public License for more details.
*     
*     You should have received a copy of the GNU General Public License
*     along with this program; if not, write to the Free Software
*     Foundation, Inc., 59 Temple Place,Suite 330, Boston, MA
*     02111-1307, USA

*  Authors:
*     RLVAD::AJC: A J Chipperfield (STARLINK)
*     RLVAD::ACC: A C Charles (STARLINK)
*     {enter_new_authors_here}

*  History:
*     17-AUG-1989 (RLVAD::AJC):
*        Original version.
*     14-SEP-1993 (ACC)
*        Modularised version: broken into one routine for each of 5 main 
*        categories of tests.
*     01-MAR-1994 (ACC)
*        Second modularised version: broken further into one routine for 
*        each of subroutine tested.  This subroutine created.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*  Subprograms called:   
*     CHR_NTH

*-

*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Arguments Given:
*     None

*  Arguments Returned:
      INTEGER STATUS

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'CHR_ERR'

*  External References:
      CHARACTER*2 CHR_NTH

*  Local Variables:
      INTEGER ISTAT              ! Local status
      CHARACTER*120 STRING

*.

*    Test CHR_NTH -- Logical to string

      ISTAT = SAI__OK
      STRING = ' '
      STRING = CHR_NTH (1)
      IF (STRING .NE. 'st') THEN
         PRINT *, 'CHR_NTH FAILS- STRING is:',STRING
         ISTAT = SAI__ERROR
      ENDIF

      STRING = ' '
      STRING = CHR_NTH (2)
      IF (STRING .NE. 'nd') THEN
         PRINT *, 'CHR_NTH FAILS- STRING is:',STRING
         ISTAT = SAI__ERROR
      ENDIF

      STRING = ' '
      STRING = CHR_NTH (3)
      IF (STRING .NE. 'rd') THEN
         PRINT *, 'CHR_NTH FAILS- STRING is:',STRING
         ISTAT = SAI__ERROR
      ENDIF

      STRING = ' '
      STRING = CHR_NTH (4)
      IF (STRING .NE. 'th') THEN
         PRINT *, 'CHR_NTH FAILS- STRING is:',STRING
         ISTAT = SAI__ERROR
      ENDIF

      IF (ISTAT .EQ. SAI__OK) THEN
         PRINT *, 'CHR_NTH OK'
      ELSE
         STATUS = SAI__ERROR
      ENDIF

      END
