      SUBROUTINE RFCUT( UFILE, STATUS )
*+
*   Name:
*      SUBROUTINE RFCUT
*
*   Description:
*      The wavelength ranges are read from a file.
*      The default file is $IUEDR_DATA/<camera><aperture>.cut,
*      which can be replaced by a user supplied file.
*
*   History:
*      Jack Giddings      01-MAY-82     AT4 version
*      Paul Rees          06-OCT-88     IUEDR Vn. 2.0
*      Martin Clayton     25-NOV-94     IUEDR Vn. 3.2
*
*   Method:
*
*-

*   Implicit:
      IMPLICIT NONE

*   Starlink includes:
      INCLUDE 'SAE_PAR'

*   Global variables:
      INCLUDE 'CMHEAD'
      INCLUDE 'CMCUT'

*   Constants:
      INTEGER FILENAMESIZE       ! maximum length of file name string
      INTEGER HIRES              ! HIRES index
      PARAMETER (FILENAMESIZE = 81, HIRES = 2)

*   Import:
      BYTE UFILE(FILENAMESIZE)   ! user supplied file

*   Export:
      INTEGER STATUS             ! status return

*   External references:
      INTEGER STR_LEN            ! string length

*   Local variables:
      BYTE FILE(FILENAMESIZE)    ! file for default inputs

      INTEGER FD                 ! file descriptor
      INTEGER IRES               ! resolution index
*.

*   Check inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*   Get resolution index
      CALL IUE_RESN( RESOL, IRES, STATUS )
      IF ( STATUS .NE. SAI__OK ) THEN
         CALL ERROUT( 'Error: resolution invalid\\', STATUS )
         RETURN
      END IF

*   Wavelength Cutoff Data
      IF ( NOCUT .AND. IRES.EQ.HIRES ) THEN
         IF ( STR_LEN(UFILE) .GT. 0 ) THEN
            CALL STR_MOVE( UFILE, FILENAMESIZE, FILE )

         ELSE
            CALL STR_MOVE( 'IUEDR_DATA:\\', FILENAMESIZE, FILE )
            CALL STR_APPND( CAMERA, FILENAMESIZE, FILE )
            CALL STR_APPND( APER, FILENAMESIZE, FILE )
         END IF

         CALL RDFILE( FILE, '.cut\\', FD, STATUS )
         IF ( STATUS .NE. SAI__OK ) THEN
            CALL LINE_WCONT( '%p No Wavelength Range Data.\\' )
            STATUS = SAI__OK
            CALL PRTBUF( STATUS )

         ELSE
            CALL RDCUT( FD, STATUS )
            IF ( STATUS .NE. SAI__OK ) THEN
               CALL ERROUT( 'Error: in cut-off data\\', STATUS )
            END IF
            CALL CLFILE( FILE, FD, STATUS )
         END IF
      END IF
      END
