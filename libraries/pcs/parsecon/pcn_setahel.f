*+  PARSECON_SETAHEL - Sets-up action help text
      SUBROUTINE PARSECON_SETAHEL ( ENTRY, STATUS )
*    Description :
*     Loads the provided string into the help store for the most 
*     recently declared program action.
*    Invocation :
*     CALL PARSECON_SETAHEL ( ENTRY, STATUS )
*    Parameters :
*     ENTRY=CHARACTER*(*) (given)
*           help string
*     STATUS=INTEGER
*    Method :
*     Superfluous quotes are removed from the given string, and the 
*     result is put into the array holding help text.
*    Deficiencies :
*     <description of any deficiencies>
*    Bugs :
*     <description of any "bugs" which have not been fixed>
*    Authors :
*     B.D.Kelly (REVAD::BDK)
*     A J Chipperfield (STARLINK)
*    History :
*     13.05.1986:  Original (REVAD::BDK)
*     16.10.1990:  define QUOTE portably (RLVAD::AJC)
*     24.03.1993:  Add DAT_PAR for SUBPAR_CMN
*    endhistory
*    Type Definitions :
      IMPLICIT NONE

*    Global constants :
      INCLUDE 'SAE_PAR'
      INCLUDE 'DAT_PAR'

*    Import :
      CHARACTER*(*) ENTRY             ! the help string

*    Status :
      INTEGER STATUS

*    Global variables :
      INCLUDE 'SUBPAR_CMN'

*    Local Constants :
      CHARACTER*(*) QUOTE
      PARAMETER ( QUOTE = '''' )

*-

      IF ( STATUS .NE. SAI__OK ) RETURN

*   If the help text is a quoted string, process the quotes
      IF ( ENTRY(1:1) .EQ. QUOTE ) THEN
         CALL STRING_STRIPQUOT ( ENTRY, ACTHELP(ACTPTR), STATUS )

      ELSE
         ACTHELP(ACTPTR) = ENTRY

      ENDIF

      END
