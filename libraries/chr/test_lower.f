      SUBROUTINE TEST_LOWER(STATUS)
*+
*  Name:
*     TEST_LOWER

*  Purpose:
*     Test CHR_LOWER.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL TEST_LOWER(STATUS)

*  Description:
*     Test CHR_LOWER.
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
*     02-MAR-1994 (ACC)
*        Second modularised version: broken further into one routine for 
*        each of subroutine tested.  This subroutine created.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*  Subprograms called:   
*     CHR_LOWER

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
      CHARACTER*1 CHR_LOWER

*.

*    Test CHR_LOWER

      IF ((CHR_LOWER('a') .EQ. 'a') .AND.
     :    (CHR_LOWER('A') .EQ. 'a') .AND.
     :    (CHR_LOWER('z') .EQ. 'z') .AND.
     :    (CHR_LOWER('Z') .EQ. 'z') .AND.
     :    (CHR_LOWER('!') .EQ. '!') .AND.
     :    (CHR_LOWER('@') .EQ. '@') .AND.
     :    (CHR_LOWER('[') .EQ. '[') .AND.
     :    (CHR_LOWER('`') .EQ. '`') .AND.
     :    (CHR_LOWER('{') .EQ. '{') .AND.
     :    (CHR_LOWER('~') .EQ. '~')) THEN
         PRINT *, 'CHR_LOWER OK'
      ELSE
         PRINT *, 'CHR_LOWER FAILS'
         STATUS = SAI__ERROR
      ENDIF

      END
