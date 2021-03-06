      SUBROUTINE ARY1_ACCOK( IACB, ACCESS, OK, STATUS )
*+
*  Name:
*     ARY1_ACCOK

*  Purpose:
*     Determine whether a specified type of ACB access is available.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL ARY1_ACCOK( IACB, ACCESS, OK, STATUS )

*  Description:
*     The routine returns a logical value indicating whether the
*     specified mode of access to an array entry in the ACB is
*     permitted by the current setting of the ACB access control flags.

*  Arguments:
*     IACB = INTEGER (Given)
*        Index to the array entry in the ACB.
*     ACCESS = CHARACTER * ( * ) (Given)
*        The type of access required (case insensitive).
*     OK = LOGICAL (Returned)
*        Whether the specified type of access is available.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Notes:
*     -  BOUNDS and SHIFT access is always permitted if the array is not
*     a base array, regardless of the state of the corresponding access
*     control flags.

*  Algorithm:
*     -  Test the specified access type against each permitted value in
*     turn and obtain the value of the associated access control flag
*     (taking account of whether the array is a base array, if
*     appropriate).
*     -  If the access type was not recognised, then report an error.

*  Copyright:
*     Copyright (C) 1989 Science & Engineering Research Council.
*     All Rights Reserved.
*     Copyright (C) 2006 Particle Physics and Astronomy Research
*     Council. All Rights Reserved.

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
*     Foundation, Inc., 51 Franklin Street,Fifth Floor, Boston, MA
*     02110-1301, USA

*  Authors:
*     RFWS: R.F. Warren-Smith (STARLINK)
*     DSB:David S Berry (JAC)
*     {enter_new_authors_here}

*  History:
*     13-SEP-1989 (RFWS):
*        Original version.
*     9-OCT-1989 (RFWS):
*        Changed to allow BOUNDS and SHIFT access regardless of the
*        access control flag settings if the object being accessed is
*        not a base array.
*     8-MAY-2006 (DSB):
*        Prevent write access to any scaled array.
*     7-JUL-2006 (DSB):
*        Delegate responsibilty for preventing write access to scaled
*        array to ARY1_CHMOD.
*     1-NOV-2010 (DSB):
*        Remove references to DCB.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'DAT_PAR'          ! DAT_ public constants
      INCLUDE 'ARY_PAR'          ! ARY_ public constants
      INCLUDE 'ARY_CONST'        ! ARY_ private constants
      INCLUDE 'ARY_ERR'          ! ARY_ error codes

*  Global Variables:
      INCLUDE 'ARY_ACB'          ! ARY_ Access Control Block
*        ACB_ACC( ARY__MXACC, ARY_MXACB ) = LOGICAL (Read)
*           Access control flags.
*        ACB_CUT( ARY__MXACB ) = LOGICAL (Read )
*           Whether the array is a cut.

*  Arguments Given:
      INTEGER IACB
      CHARACTER * ( * ) ACCESS

*  Arguments Returned:
      LOGICAL OK

*  Status:
      INTEGER STATUS             ! Global status

*  External References:
      LOGICAL CHR_SIMLR          ! Case insensitive string comparison
*.

*  Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Test the requested access type against each permitted value in turn
*  and obtain the value of the associated access control flag. Take
*  account of whether the object is a base array, if appropriate.

*  ...BOUNDS access.
      IF ( CHR_SIMLR( ACCESS, 'BOUNDS' ) ) THEN
         OK = ACB_ACC( ARY__BOUND, IACB ) .OR. ACB_CUT( IACB )

*  ...DELETE access.
      ELSE IF ( CHR_SIMLR( ACCESS, 'DELETE' ) ) THEN
         OK = ACB_ACC( ARY__DELET, IACB )

*  ...SHIFT access.
      ELSE IF ( CHR_SIMLR( ACCESS, 'SHIFT' ) ) THEN
         OK = ACB_ACC( ARY__SHIFT, IACB ) .OR. ACB_CUT( IACB )

*  ...TYPE access.
      ELSE IF ( CHR_SIMLR( ACCESS, 'TYPE' ) ) THEN
         OK = ACB_ACC( ARY__TYPE, IACB )

*  ...SCALE access.
      ELSE IF ( CHR_SIMLR( ACCESS, 'SCALE' ) ) THEN
         OK = ACB_ACC( ARY__SCALE, IACB )

*  ...WRITE access.
      ELSE IF ( CHR_SIMLR( ACCESS, 'WRITE' ) ) THEN
         OK = ACB_ACC( ARY__WRITE, IACB )

*  If the access type was not recognised, then report an error.
      ELSE
         STATUS = ARY__ACCIN
         CALL MSG_SETC( 'BADACC', ACCESS )
         CALL ERR_REP( 'ARY1_ACCOK_BAD',
     :   'Invalid access type ''^BADACC'' specified (possible ' //
     :   'programming error).', STATUS )
      END IF

*  Call error tracing routine and exit.
      IF ( STATUS .NE. SAI__OK ) CALL ARY1_TRACE( 'ARY1_ACCOK', STATUS )

      END
