      SUBROUTINE COF_NDF2F( NDF, FILNAM, NOARR, ARRNAM, BITPIX, BLOCKF,
     :                      ORIGIN, PROFIT, PROEXT, STATUS )
*+
*  Name:
*     COF_NDF2F

*  Purpose:
*     Converts an NDF into a FITS file.

*  Language:
*     Starlink Fortran 77

*  Invocation:
*     CALL COF_NDF2F( NDF, FILNAM, NOARR, ARRNAM, BITPIX, BLOCKF,
*                     ORIGIN, PROFIT, PROEXT, STATUS )

*  Description:
*     This routine converts an NDF into a FITS file.  It uses as much
*     standard NDF information as possible to define the headers....

*  Arguments:
*     NDF = INTEGER (Given)
*        The identifier of the NDF to be converted to a FITS file.
*     FILNAM = CHARACTER * ( * ) (Given)
*        The name of the output FITS file.
*     NOARR = INTEGER (Given)
*        The number of NDF arrays to copy.
*     ARRNAM( NOARR ) = CHARACTER * ( * ) (Given)
*        The names of the NDF array components to write to the FITS
*        file.  These should be in the order to be written.  If the
*        Data component is present it should be first.
*     BITPIX = INTEGER (Given)
*        The number of bits per pixel (FITS BITPIX) required for the
*        output FITS file.  A value of 0 means use the BITPIX of the
*        input array.
*     BLOCKF = INTEGER (Given)
*        The blocking factor for the output file.  It must be a positive
*        integer between 1 and 10.
*     ORIGIN = CHARACTER * ( * ) (Given)
*        The name of the institution were the FITS file originates.
*        This is used to create the ORIGIN kcard in the FITS header.
*        A blank value gives a default of "Starlink Project, U.K.".
*     PROFIT = LOGICAL (Given)
*        If .TRUE., the contents of the FITS extension, if present, are
*        merged into the FITS header.  Certain cards in this extension
*        are not propagated ever and others may only be propagated when
*        certain standard items are not present in the NDF.  See routine
*        COF_WHEAD for details.
*     PROEXT = LOGICAL (Given)
*        If .TRUE., the NDF extensions (other than the FITS extension)
*        are propagated to the FITS files as FITS binary-table
*        extensions, one per structure of the hierarchy.
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Notes:
*     The rules for the conversion are as follows:
*     -  The NDF main data array becomes the primary data array of the
*     FITS file if it is in value of parameter COMP, otherwise the first
*     array defined by parameter COMP will become the primary data
*     array.  A conversion from floating point to integer or to a
*     shorter integer type will cause the output array to be scaled and
*     offset, the values being recorded in keywords BSCALE and BZERO.
*     There is an offset (keyword BZERO) applied to signed byte and
*     unsigned word types to make them unsigned-byte and signed-word
*     values respectively in the FITS array (this is because FITS does
*     not support these data types).
*     -  The FITS keyword BLANK records the bad values for integer
*     output types.  Bad values in floating-point output arrays are
*     denoted by IEEE not-a-number values. 
*     -  The NDF's quality and variance arrays appear in individual
*     FITS IMAGE extensions immediately following the primary header
*     and data unit, unless that component already appears as the
*     primary data array.  The quality array will always be written as
*     an unsigned-byte array in the FITS file, regardless of the value
*     of the parameter BITPIX.
*     -  Here are details of the processing of standard items from the
*     NDF into the FITS header, listed by FITS keyword.
*        SIMPLE, EXTEND, PCOUNT, GCOUNT --- all take their default
*          values.
*        BITPIX, NAXIS, NAXISn --- are derived directly from the NDF
*          data array;
*        CRVALn, CDELTn, CRPIXn, CTYPEn, CUNITn --- are derived from
*          the NDF axis structures if possible.  If no linear NDF axis
*          structures are present, the values in the NDF FITS extension
*          are copied (when parameter PROFITS is true).  If any axes
*          are non-linear, all FITS axis information is lost.
*        OBJECT, LABEL, BUNITS --- the values held in the NDF's title,
*          label, and units components respectively are used if
*          they are defined; otherwise any values found in the FITS
*          extension are used (provided parameter PROFITS is true).
*        ORIGIN and DATE --- are created automatically.  However the
*          former may be overriden by an ORIGIN card in the NDF
*          extension.
*        EXTNAME --- is the component name of the object from the COMP
*          argument.
*        HDUCLAS1, HDUCLASn --- "NDF" and the value of COMP
*          respectively.
*        LBOUNDn --- is the pixel origin for the nth dimension when
*          any of the pixel origins is not equal to 1.  (This is not a
*          standard FITS keyword.)
*        XTENSION, BSCALE, BZERO, BLANK and END --- are not propagated
*          from the NDF's FITS extension.  XTENSION will be set for
*          any extension.  BSCALE and BZERO will be defined based on
*          the chosen output data type in comparison with the NDF
*          array's type, but cards with values 1.0 and 0.0 respectively
*          are written to reserve places in the header section.  These
*          `reservation' cards are for efficiency and they can always
*          be deleted later.  BLANK is set to the Starlink standard bad
*          value corresponding to the type specified by BITPIX, but only
*          for integer types and not for the quality array.  It appears
*          regardless of whether or not there are bad values actually
*          present in the array; this is for the same efficiency
*          reasons as before.  The END card terminates the FITS header.
*          The END card is written by FITSIO automatically once the
*          header is closed.

*  Implementation Deficiencies:
*     - There is no propagation of NDF history records to the FITS file.
*     - There is no support for FITS World Co-ordinate Systems.
*     [routine_deficiencies]...

*  [optional_subroutine_items]...
*  Authors:
*     MJC: Malcolm J. Currie (STARLINK)
*     {enter_new_authors_here}

*  History:
*     1994 May 31 (MJC):
*        Original version.
*     1995 November 22 (MJC):
*        Some bug fixes and improvements.
*     1996 September 11 (MJC):
*        Simplified the error message when the FITS file exists.
*     {enter_further_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
      
*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'DAT_PAR'          ! Data-system constants
      INCLUDE 'NDF_PAR'          ! NDF constants
      INCLUDE 'PRM_PAR'          ! PRIMDAT public constants

*  Arguments Given:
      INTEGER NDF
      CHARACTER * ( * ) FILNAM
      INTEGER NOARR
      CHARACTER * ( * ) ARRNAM( NOARR )
      INTEGER BITPIX
      INTEGER BLOCKF
      CHARACTER * ( * ) ORIGIN
      LOGICAL PROFIT
      LOGICAL PROEXT

*  Status:
      INTEGER STATUS             ! Global status

*  External References:
      INTEGER CHR_LEN            ! Length of a string less trailing
                                 ! blanks

*  Local Constants:
      INTEGER   FITSOK           ! Value of good FITSIO status
      PARAMETER( FITSOK = 0 )

      INTEGER   GCOUNT           ! Value of FITS GCOUNT keyword
      PARAMETER( GCOUNT = 1 )

      INTEGER   PCOUNT           ! Value of FITS PCOUNT keyword
      PARAMETER( PCOUNT = 0 )

*  Local Variables:
      LOGICAL ANYF               ! True if bad values present in array
      LOGICAL BAD                ! True if bad values may be present in
                                 ! array
      INTEGER BLANK              ! Data blank for integer arrays
      INTEGER BPIN               ! Input array's BITPIX
      INTEGER BPINU              ! Input array's BITPIX unsigned version
      INTEGER BPOUT              ! Output array's BITPIX
      INTEGER BPOUTU             ! Output array's BITPIX unsigned
                                 ! version
      DOUBLE PRECISION BSCALE    ! Block-integer scale factor
      DOUBLE PRECISION BZERO     ! Block-integer offset
      INTEGER DIMS( NDF__MXDIM ) ! NDF dimensions (axis length)
      INTEGER EL                 ! Number of elements in array
      LOGICAL FEXIST             ! FITS already exists?
      LOGICAL FITPRE             ! FITS extension is present
      INTEGER FSTAT              ! FITSIO error status
      INTEGER FSTATC             ! FITSIO error status for file closure
      INTEGER FUNIT              ! Fortran I/O unit for the FITS file
      INTEGER I                  ! Loop counter
      INTEGER ICOMP              ! Loop counter
      INTEGER IPNTR              ! Pointer to input array
      DOUBLE PRECISION MAXV      ! Max. value to appear in scaled array
      DOUBLE PRECISION MINV      ! Min. value to appear in scaled array
      INTEGER NBYTES             ! Number of bytes per array value
      INTEGER NC                 ! Number of character in string
      INTEGER NDECIM             ! Number of decimal places in header
                                 ! value
      INTEGER NDIM               ! Number of dimensions
      INTEGER NEXTN              ! Number of extensions
      INTEGER NEX2PR             ! Number of extensions to process
      BYTE NULL8                 ! Null value for BITPIX=8
      INTEGER * 2 NULL16         ! Null value for BITPIX=16
      INTEGER NULL32             ! Null value for BITPIX=32
      REAL NUL_32                ! Null value for BITPIX=-32
      DOUBLE PRECISION NUL_64    ! Null value for BITPIX=-64
      LOGICAL OPEN               ! FITS file exists?
      LOGICAL PROPEX             ! True if the FITS extension is to be
                                 ! propagated for the current header
      LOGICAL SCALE              ! True if the array is to be scaled
      LOGICAL SHIFT              ! True if a BZERO offset is required
      CHARACTER * ( NDF__SZTYP ) TYPE ! NDF array's data type
      LOGICAL VALID              ! True if the NDF identifier is valid
      CHARACTER * ( DAT__SZLOC ) XLOC ! Locator to an NDF extension
      CHARACTER * ( NDF__SZXNM ) XNAME ! Name of NDF extension

*  Internal References:
      INCLUDE 'NUM_DEC_CVT'      ! NUM declarations for conversions
      INCLUDE 'NUM_DEF_CVT'      ! NUM definitions for conversions

*.

*  Check the inherited global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Open the FITS file.
*  ===================

*  Find a free logical-unit.
      CALL FIO_GUNIT( FUNIT, STATUS )

*  Open the FITS file.
      CALL FTINIT( FUNIT, FILNAM, BLOCKF, FSTAT )

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.  To simplify the error message, test to see if the file
*  exists, and if it does make a shorter report (note that the status
*  must be set bad too) and clear the FITSIO error-message stack.
*  Record whether the file was actually opened or not.
      IF ( FSTAT .GT. FITSOK ) THEN
         INQUIRE( FILE=FILNAM, EXIST=FEXIST )
         IF ( FEXIST ) THEN
            STATUS = SAI__ERROR
            CALL ERR_REP( 'COF_NDF2F_FILEEXIST',
     :        'Error creating the output FITS file '/
     :        /FILNAM( :CHR_LEN( FILNAM ) )//' because it already '/
     :        /'exists.', STATUS )
            CALL FTCMSG

         ELSE
            CALL COF_FIOER( FSTAT, 'COF_NDF2F_OPENERR', 'FTINIT',
     :        'Error creating the output FITS file '/
     :        /FILNAM( :CHR_LEN( FILNAM ) )//'.', STATUS )
         END IF
         OPEN = .FALSE.
         GOTO 999
      ELSE
         OPEN = .TRUE.
      END IF

*  Validate the NDF identifier.
*  ============================
      CALL NDF_VALID( NDF, VALID, STATUS )

*  Report an error if the identifier is not valid.
      IF ( .NOT. VALID ) THEN
         STATUS = SAI__ERROR
         CALL ERR_REP( 'COF_NDF2F_INVNDF',
     :     'COF_NDF2F: The identifier to the input NDF is invalid. '/
     :     /'(Probable programming error.)', STATUS )
         GOTO 999
      END IF

*  Loop for each array component.
*  ==============================
      DO ICOMP = 1, NOARR

*  Define the structure of the array.
*  ==================================

*  Inquire the shape of the NDF.
         CALL NDF_DIM( NDF, NDF__MXDIM, DIMS, NDIM, STATUS )

*  Obtain the NDF type.
         CALL NDF_TYPE( NDF, ARRNAM( ICOMP ), TYPE, STATUS )

*  Obtain the bad-pixel flag.
         CALL NDF_BAD( NDF, ARRNAM( ICOMP ), .FALSE., BAD, STATUS )

*  Find the input BITPIX.
*  ======================

*  Get the the number of bytes per data type.
         CALL COF_TYPSZ( TYPE, NBYTES, STATUS )

*  Convert this to a pseudo-BITPIX value, where floating point values
*  are designated by being greater than the integer types.  The unsigned
*  versions are needed to determine ascendency, and so whether or not
*  scaling is required.
         IF ( TYPE .EQ. '_REAL' .OR. TYPE .EQ. '_DOUBLE' ) THEN
            BPIN = -NBYTES * 8
            BPINU = 1 - BPIN
         ELSE
            BPIN = NBYTES * 8
            BPINU = BPIN
         END IF

*  Define the effective output data type (BITPIX).
*  ===============================================

*  Note the QUALITY will always be unsigned byte and therefore we
*  ignore the supplied BITPIX.
         IF ( ARRNAM( ICOMP ) .EQ. 'QUALITY' ) THEN
            BPOUT = 8
            BPOUTU = BPOUT
         ELSE IF ( BITPIX .EQ. 0 ) THEN
            BPOUT = BPIN
            BPOUTU = BPINU
         ELSE
            BPOUT = BITPIX

*  Allow for the sign of floating-point BITPIX values.
            IF ( BITPIX .EQ. -32 .OR. BITPIX .EQ. -64 ) THEN
               BPOUTU = 1 - BPOUT
            ELSE
               BPOUTU = BPOUT
            END IF
         END IF

*  Open a new header and data unit for extensions.
*  ===============================================

*  A new HDU is created when the file is opened so a new header need
*  only be opened done for extensions.
         IF ( ICOMP .GT. 1 ) THEN
            CALL FTCRHD( FUNIT, FSTAT )

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
            IF ( FSTAT .GT. FITSOK ) THEN
               CALL COF_FIOER( FSTAT, 'COF_NDF2F_NHDU', 'FTCRHD',
     :           'Error creating the header and data unit for an '/
     :           /'IMAGE extension.', STATUS )
               GOTO 999
            END IF
         END IF

*  Write the header.
*  =================
*
*  Decide whether or not to propagate the FITS extension.  It will
*  only appear in the primary array's header, if requested.
         PROPEX = PROFIT .AND. ICOMP .EQ. 1

*  First write the standard headers, and merge in the FITS extension
*  when requested to do so.
         CALL COF_WHEAD( NDF, ARRNAM( ICOMP ), FUNIT, BPOUT, PROPEX,
     :                   STATUS )
         IF ( STATUS .NE. SAI__OK ) GOTO 999

*  Determine the block-floating point conversion and blank value.
*  ==============================================================
*
*  This code is a little messy for efficiency.  We have to determine
*  certain properties of the transformation between FITS values and
*  input array value in order to set the BLANK, BSCALE, and BZERO
*  cards, so that the cards must be written before we write the
*  array.  This means a two-stage process.

*  Set the defaults.
         SCALE = .FALSE.
         SHIFT = .FALSE.
         BSCALE = 1.0D0
         BZERO = 0.0D0

*  Is format conversion required?
*  ------------------------------
*
*  Scaling is required when the requested BITPIX has lower precision
*  than the array type and BITPIX is an integer type.  It is also
*  needed when the input data type does not match the FITS data types,
*  namely _BYTE and _UWORD.  Deal with a special case first as it needs
*  to be in a separate IF block.
*
*  When there is scaling to perform we have to convert the input data
*  type to _INTEGER, as there is no FITSIO routine for writing a _UWORD
*  array to the FITS file.  Adjust the input BITPIX accordingly.
         IF ( TYPE .EQ. '_UWORD' .AND. BPOUT .EQ. 8 ) THEN
            TYPE = '_INTEGER'
            BPIN = 32
         END IF

*  Next tackle the non-standard input data types as they are short.
*  Record the fact that an offset is required, and reset the bad pixel
*  (BLANK) value.
         IF ( TYPE .EQ. '_BYTE' ) THEN
            SHIFT = .TRUE.
            BZERO = -128.0D0
            BLANK = VAL__BADB + 128

*  When there is scaling to perform a simple shift is not adequate
*  so use the scaling route.
         ELSE IF ( TYPE .EQ. '_UWORD' ) THEN
            SHIFT = .TRUE.
            BZERO = 32768.0D0
            BLANK = VAL__BADUW - 32768

*  Compare the component's BITPIX with that supplied.
         ELSE IF ( BPOUTU .LT. BPINU ) THEN

*  The data must be rescaled and the bad-pixel value altered to that of
*  the output type.
            SCALE = .TRUE.

*  Integer types will have a blank value.  Note that it is the output
*  type that is assigned.
            IF ( BPOUT .EQ. 32 ) THEN
               BLANK = VAL__BADI

            ELSE IF ( BPOUT .EQ. 16 ) THEN
               BLANK = NUM_WTOI( VAL__BADW )

            ELSE IF ( BPOUT .EQ. 8 ) THEN
               BLANK = NUM_UBTOI( VAL__BADUB )

            END IF

*  Set the null values.  Only one will be needed, depending on the
*  value of BPOUT, but it as efficient to assign them all.
            NULL32 = VAL__BADI
            NULL16 = VAL__BADW
            NULL8 = VAL__BADUB
            NUL_32 = VAL__BADR
            NUL_64 = VAL__BADD

*  Deal with the types where no scaling or offset is required.
         ELSE

*  Integer types will have a blank value.  Also set the null values.
*  These are based upon the type of the NDF array.  The FITSIO routine
*  will perform any type conversion required.
            IF ( TYPE .EQ. '_INTEGER' ) THEN
               BLANK = VAL__BADI
               NULL32 = VAL__BADI

            ELSE IF ( TYPE .EQ. '_WORD' ) THEN
               BLANK = NUM_WTOI( VAL__BADW )
               NULL16 = VAL__BADW

            ELSE IF ( TYPE .EQ. '_UBYTE' ) THEN
               BLANK = NUM_UBTOI( VAL__BADUB )
               NULL8 = VAL__BADUB

            ELSE IF ( TYPE .EQ. '_REAL' ) THEN
               NUL_32 = VAL__BADR

            ELSE IF ( TYPE .EQ. '_UBYTE' ) THEN
               NUL_64 = VAL__BADD

            END IF
         END IF

*  Set the blank value in the header.
*  ==================================

*  Only required for the integer types.  BLANK has no meaning for
*  floating-point in FITS.
         IF ( BPOUT .GT. 0 .AND.
     :        ARRNAM( ICOMP ) .NE. 'QUALITY' ) THEN

*  The header should already contain a BLANK keyword.
*  Reset the BLANK keyword in the header. Ampersand instructs the
*  routine not to modify the comment of the BLANK header card.
            CALL FTMKYJ( FUNIT, 'BLANK', BLANK, '&', FSTAT )

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
            IF ( FSTAT .GT. FITSOK ) THEN
               CALL COF_FIOER( FSTAT, 'COF_NDF2F_BLANK1', 'FTMKYJ',
     :           'Error modifying the BLANK header card.', STATUS )
               GOTO 999
            END IF

         END IF

*  Find the scaling.
*  =================

         IF ( SCALE ) THEN

*  To scale we have to map the input array, find the extreme values,
*  and hence derive the scale and offset.  These are then applied to
*  form a new work array of the same data type wherein the bad values
*  are replaced by the bad values for the output data type.  This array
*  is then passed to the FITSIO routine which performs a type
*  conversion to the required output type.

*  First map the input array component.
            CALL NDF_MAP( NDF, ARRNAM( ICOMP ), TYPE, 'READ', IPNTR,
     :                    EL, STATUS )

*  Set the scaling limits to double precision.  The _DOUBLE output
*  would not need scaling so it is omitted.  This is done so that the
*  scaling routine need only be generic for the input data type, and
*  not for the output too.
            IF ( BPOUT .EQ. 8 ) THEN
               MAXV = NUM_UBTOD( VAL__MAXUB )
               MINV = NUM_UBTOD( VAL__MINUB )

            ELSE IF ( BPOUT .EQ. 16 ) THEN
               MAXV = NUM_WTOD( VAL__MAXW )
               MINV = NUM_WTOD( VAL__MINW )

            ELSE IF ( BPOUT .EQ. 32 ) THEN
               MAXV = DBLE( VAL__MAXI )
               MINV = DBLE( VAL__MINI )

            ELSE IF ( BPOUT .EQ. -32 ) THEN
               MAXV = DBLE( VAL__MAXR )
               MINV = DBLE( VAL__MINR )

            END IF

*  Evaluate the scaling and offset.  Call the appropriate routine
*  dependent on the array-component's type to evaluate the scaling.
*  Note that _UBYTE and _BYTE will never need scaling; _BYTE and _UWORD
*  need a shift of BZERO.  The scaling itself is done by FITSIO (FTPSCL
*  sets the scale and offset).
            IF ( TYPE .EQ. '_WORD' ) THEN
               CALL COF_ESCOW( BAD, EL, %VAL( IPNTR ), MINV, MAXV,
     :                          BSCALE, BZERO, STATUS )

            ELSE IF ( TYPE .EQ. '_INTEGER' ) THEN
               CALL COF_ESCOI( BAD, EL, %VAL( IPNTR ), MINV, MAXV,
     :                          BSCALE, BZERO, STATUS )

            ELSE IF ( TYPE .EQ. '_REAL' ) THEN
               CALL COF_ESCOR( BAD, EL, %VAL( IPNTR ), MINV, MAXV,
     :                          BSCALE, BZERO, STATUS )

            ELSE IF ( TYPE .EQ. '_DOUBLE' ) THEN
               CALL COF_ESCOD( BAD, EL, %VAL( IPNTR ), MINV, MAXV,
     :                          BSCALE, BZERO, STATUS )

            END IF

         ELSE

*  No scaling required.
*  ====================
*
*  Any type conversion will be performed by the FITSIO array-writing
*  routine.
            CALL NDF_MAP( NDF, ARRNAM( ICOMP ), TYPE, 'READ', IPNTR,
     :                    EL, STATUS )
         END IF
         IF ( STATUS .NE. SAI__OK ) GOTO 999

*  Revise the scale and zero cards.
*  ================================
         IF ( SCALE .OR. SHIFT ) THEN

*  Decide the appropriate number of decimals needed to represent the
*  block floating point scale and offset.  The minus 7 excludes the
*  sign, leading zero, decimal point and the trailing exponent.
            IF ( TYPE .EQ. '_DOUBLE' ) THEN
               NDECIM = VAL__SZD - 7
            ELSE
               NDECIM = VAL__SZR - 7
            END IF

*     Reset the BSCALE keyword in the header. Ampersand instructs the
*     routine not to modify the comment of the BSCALE header card.
            CALL FTMKYD( FUNIT, 'BSCALE', BSCALE, NDECIM, '&', FSTAT )

*  Similarly for the BZERO card.
            CALL FTMKYD( FUNIT, 'BZERO', BZERO, NDECIM, '&', FSTAT )

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
            IF ( FSTAT .GT. FITSOK ) THEN
               CALL COF_FIOER( FSTAT, 'COF_NDF2F_HSCOF', 'FTMKYD',
     :           'Error modifying the BSCALE or BZERO header card.',
     :           STATUS )
               GOTO 999
            END IF
         END IF

*  Set the data scaling and offset.
*  ================================
         CALL FTPSCL( FUNIT, BSCALE, BZERO, FSTAT )

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
         IF ( FSTAT .GT. FITSOK ) THEN
            CALL COF_FIOER( FSTAT, 'COF_NDF2F_SCOF', 'FTPSCL',
     :        'Error defining the scale and offset.', STATUS )
            GOTO 999
         END IF

*  Set the blank data value.
*  =========================

*  Only required for the integer types.  BLANK has no meaning for
*  floating-point in FITS.  Note that this moust be done after the call
*  to FTPDEF, and so cannot be done when the header value is modified.
         IF ( BPOUT .GT. 0 ) THEN

*  Set the data blank value.
            CALL FTPNUL( FUNIT, BLANK, FSTAT )

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
            IF ( FSTAT .GT. FITSOK ) THEN
               CALL COF_FIOER( FSTAT, 'COF_NDF2F_BLANK2', 'FTPNUL',
     :           'Error modifying the BLANK value.', STATUS )
               GOTO 999
            END IF

         END IF

*  Write the output array to the FITS file.
*  ========================================

         IF ( BAD ) THEN

*  Call the appropriate routine for the data type of the supplied
*  array.  The group is 0, and we always start at the first element.
*  Remember that the input BITPIX values for floating point are one
*  minus the true BITPIX (the non-standard values were needed to
*  determine whether or not scaling was required).  The arrays may have
*  bad pixels.
            IF ( BPIN .EQ. 8 ) THEN
               CALL FTPPNB( FUNIT, 0, 1, EL, %VAL( IPNTR ), NULL8,
     :                      ANYF, FSTAT )

            ELSE IF ( BPIN .EQ. 16 ) THEN
               CALL FTPPNI( FUNIT, 0, 1, EL, %VAL( IPNTR ), NULL16,
     :                      ANYF, FSTAT )

            ELSE IF ( BPIN .EQ. 32 ) THEN
               CALL FTPPNJ( FUNIT, 0, 1, EL, %VAL( IPNTR ), NULL32,
     :                      ANYF, FSTAT )

            ELSE IF ( BPIN .EQ. -32 ) THEN
               CALL FTPPNE( FUNIT, 0, 1, EL, %VAL( IPNTR ), NUL_32,
     :                      ANYF, FSTAT )

            ELSE IF ( BPIN .EQ. -64) THEN
               CALL FTPPND( FUNIT, 0, 1, EL, %VAL( IPNTR ), NUL_64,
     :                      ANYF, FSTAT )
            END IF

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
            IF ( FSTAT .GT. FITSOK ) THEN
               NC = CHR_LEN( ARRNAM( ICOMP ) )
               CALL COF_FIOER( FSTAT, 'COF_NDF2F_WRDATAERR', 'FTPPNx',
     :           'Error writing '//ARRNAM( ICOMP )( :NC )//' array '/
     :           /'component to FITS file '/
     :           /FILNAM( :CHR_LEN( FILNAM ) )//'.', STATUS )
               GOTO 999
            END IF

         ELSE

*  Call faster routine when there are no bad pixels.  Call the
*  appropriate routine for the data type of the supplied array.  The
*  group is 0, and we always start at the first element.  Remember that
*  the input BITPIX values for floating point are one minus the true
*  BITPIX (the non-standard values were needed to determine whether or
*  not scaling was required).
            IF ( BPIN .EQ. 8 ) THEN
               CALL FTPPRB( FUNIT, 0, 1, EL, %VAL( IPNTR ), FSTAT )

            ELSE IF ( BPIN .EQ. 16 ) THEN
               CALL FTPPRI( FUNIT, 0, 1, EL, %VAL( IPNTR ), FSTAT )

            ELSE IF ( BPIN .EQ. 32 ) THEN
               CALL FTPPRJ( FUNIT, 0, 1, EL, %VAL( IPNTR ), FSTAT )

            ELSE IF ( BPIN .EQ. -32 ) THEN
               CALL FTPPRE( FUNIT, 0, 1, EL, %VAL( IPNTR ), FSTAT )

            ELSE IF ( BPIN .EQ. -64 ) THEN
               CALL FTPPRD( FUNIT, 0, 1, EL, %VAL( IPNTR ), FSTAT )

            END IF

*  Handle a bad status.  Negative values are reserved for non-fatal
*  warnings.
            IF ( FSTAT .GT. FITSOK ) THEN
               NC = CHR_LEN( ARRNAM( ICOMP ) )
               CALL COF_FIOER( FSTAT, 'COF_NDF2F_WRDATAERR', 'FTPPRx',
     :           'Error writing '//ARRNAM( ICOMP )( :NC )//' array '/
     :           /'component to FITS file '/
     :           /FILNAM( :CHR_LEN( FILNAM ) )//'.', STATUS )
               GOTO 999
            END IF

         END IF

*  Tidy the array.
*  ===============
*  Unmap the input array.
         CALL NDF_UNMAP( NDF, ARRNAM( ICOMP ), STATUS )

*  Map the input

      END DO

*  Process extensions.
*  ===================
      IF ( PROEXT ) THEN

*  Use binary tables for all extensions other than FITS.  Special
*  software for handling standard extensions will be provided as it
*  becomes available.  

*  Look for NDF extensions.  Check whether or not there are any present.
         CALL NDF_XNUMB( NDF, NEXTN, STATUS )

         IF ( NEXTN .GE. 1 ) THEN
      
*  See if one of these is the FITS extension.
            CALL NDF_XSTAT( NDF, 'FITS', FITPRE, STATUS )

*  Find the number of extensions to process, as the FITS extension
*  is handled elsewhere.
            IF ( FITPRE ) THEN
               NEX2PR = NEXTN - 1
            ELSE
               NEX2PR = NEXTN
            END IF

*  Are there any extensions to process?
            IF ( NEX2PR .GE. 1 ) THEN

*  Loop through the extensions.
               DO I = 1, NEXTN

*  Get the name of the next extension.
                  CALL NDF_XNAME( NDF, I, XNAME, STATUS )

*  Skip over the FITS extension.
                  IF ( XNAME .NE. 'FITS' ) THEN

*  Get a locator to the extension.
                     CALL NDF_XLOC( NDF, XNAME, 'READ', XLOC, STATUS )

*  Process the extension into a hierarchy.
                     CALL COF_THIER( XNAME, XLOC, FUNIT, STATUS )

*  Annul the locator so it may be reused.
                     CALL DAT_ANNUL( XLOC, STATUS )
                  END IF
               END DO
            END IF
         END IF

      END IF

  999 CONTINUE      

*  Close the FITS file.
      IF ( OPEN ) THEN
         CALL FTCLOS( FUNIT, FSTATC )
         IF ( FSTATC .GT. FITSOK ) THEN
            CALL COF_FIOER( FSTATC, 'COF_NDF2F_CLOSE', 'FTCLOS',
     :        'Error closing the FITS file '//FILNAM, STATUS )
         END IF
      END IF

      END
