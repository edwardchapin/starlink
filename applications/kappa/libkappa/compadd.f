      SUBROUTINE COMPADD( STATUS )
*+
*  Name:
*     COMPADD

*  Purpose:
*     Reduces the size of an NDF by adding values in rectangular boxes.

*  Language:
*     Starlink Fortran 77

*  Type of Module:
*     ADAM A-task

*  Invocation:
*     CALL COMPADD( STATUS )

*  Arguments:
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Description:
*     This application takes an NDF data structure and reduces it in
*     size by integer factors along each dimension.  The compression
*     is achieved by adding the values of the input NDF within
*     non-overlapping `rectangular' boxes whose dimensions are the
*     compression factors.  The additions may be normalised to correct
*     for any bad values present in the input NDF.

*  Usage:
*     compadd in out compress [wlim]

*  ADAM Parameters:
*     AXWEIGHT = _LOGICAL (Read)
*        When there is an AXIS variance array present in the NDF and
*        AXWEIGHT=TRUE the application forms weighted averages of the
*        axis centres using the variance.  For all other conditions
*        the non-bad axis centres are given equal weight during the
*        averaging to form the output axis centres. [FALSE]
*     COMPRESS( ) = _INTEGER (Read)
*        Linear compression factors to be used to create the output
*        NDF.  There should be one for each dimension of the NDF.  If
*        fewer are supplied the last value in the list of compression
*        factors is given to the remaining dimensions.  Thus if a
*        uniform compression is required in all dimensions, just one
*        value need be entered.  All values are constrained to be in
*        the range one to the size of its corresponding dimension.  The
*        suggested default is the current value. 
*     IN  = NDF (Read)
*        The NDF structure to be reduced in size.
*     NORMAL = _LOGICAL (Read)
*        When there are bad pixels present in the summation box these
*        are ignored.  Therefore a simple addition of the input-array
*        component's values will yield a result discordant with
*        neighbouring output pixels that were formed from summation of
*        all the pixels in the box.  When NORMAL=TRUE the output values
*        are normalised: the addition is multiplied by the ratio of the
*        number of pixels in the box to the number of good pixels
*        therein to arrive at the output value.  When NORMAL=FALSE the
*        output values are always just the sum of the good pixels.
*        [TRUE]
*     OUT = NDF (Write)
*        NDF structure to contain compressed version of the input NDF.
*     PRESERVE = _LOGICAL (Read)
*        If the input data type is to be preserved on output then this
*        parameter should be set true.   However, this may result in
*        overflows for integer types and hence additional bad values
*        written to the output NDF.  If this parameter is set false
*        then the output data type will be one of _REAL or _DOUBLE,
*        depending on the input type. [FALSE]
*     TITLE = LITERAL (Read)
*        Title for the output NDF structure.  A null value (!)
*        propagates the title from the input NDF to the output NDF. [!]
*     WLIM = _REAL (Read)
*        If the input NDF contains bad pixels, then this parameter
*        may be used to determine the number of good pixels which must
*        be present within the addition box before a valid output
*        pixel is generated.  It can be used, for example, to prevent
*        output pixels from being generated in regions where there are
*        relatively few good pixels to contribute to the smoothed
*        result.
*
*        WLIM specifies the minimum fraction of good pixels which must
*        be present in the summation box in order to generate a good
*        output pixel.  If this specified minimum fraction of good
*        input pixels is not present, then a bad output pixel will
*        result, otherwise the output value will be the sum of the
*        good values.  The value of this parameter should lie between
*        0.0 and 1.0 (the actual number used will be rounded up if
*        necessary to correspond to at least 1 pixel). [0.3]

*  Examples:
*     compadd cosmos galaxy 4
*        This compresses the NDF called cosmos summing four times in
*        each dimension, and stores the reduced data in the NDF called
*        galaxy.  Thus if cosmos is two-dimensional, this command
*        would result in a sixteen-fold reduction in the array
*        components.
*     compadd cosmos galaxy 4 wlim=1.0
*        This compresses the NDF called cosmos adding four times in
*        each dimension, and stores the reduced data in the NDF called
*        galaxy.  Thus if cosmos is two-dimensional, this command
*        would result in a sixteen-fold reduction in the array
*        components.  If a summation box contains any bad pixels, the
*        output pixel is set to bad.
*     compadd cosmos galaxy 4 0.0 preserve
*        As above except that a summation box need only contains a
*        single non-bad pixels for the output pixel to be good, and
*        galaxy's array components will have the same as those in
*        cosmos.
*     compadd cosmos galaxy [4,3] nonormal title="COSMOS compressed"
*        This compresses the NDF called cosmos adding four times in
*        the first dimension and three times in higher dimensions, and
*        stores the reduced data in the NDF called galaxy.  Thus if
*        cosmos is two-dimensional, this command would result in a
*        twelve-fold reduction in the array components.  Also, if there
*        are bad pixels there will be no normalistion correction for the
*        missing values.  The title of the output NDF is "COSMOS
*        compressed".
*     compadd in=arp244 compress=[1,1,3] out=arp244cs
*        Suppose arp244 is a huge NDF storing a spectral-line data
*        cube, with the third dimension being the spectral axis.
*        This command compresses arp244 in the spectral dimension,
*        adding every three pixels to form the NDF called arp244cs.

*  Notes:
*     -  The axis centres and variances are averaged, whilst the widths
*     are summed and always normalised for bad values.

*  Algorithm:
*     -  Obtain the input NDF. Inquire its bounds and dimensions. Set
*     lower limit of two dimensions.
*     -  Obtain the compression factors with acceptable ranges.  Abort
*     if there is no compression.  Obtain the minimum number of good
*     values in the input box to create a good output value.  Determine
*     whether or not variance is present.  Determine if normalisation
*     is required.  Obtain the data-type preservation flag.  Compute
*     the dimensions of the output NDF.
*     -  Create the output NDF via a section of the input array whose
*     bounds are those of the output array in order to propagate
*     character components, AXIS, HISTORY, and extensions.  Determine
*     the numeric data type for processing from the data and variance.
*     (It is also used for the axis arrays.)
*     -  Obtain workspace using the appropriate type.
*     -  Compress the data array, and if present the variance, axis
*     centre, axis width and variance.
*        o  The input data type is obtained and set in the output array
*        if the preserve flag is set.
*        o  Obtain the necessary workspace, mapping the input and
*        output arrays
*        o  Call the appropriate compression subroutine depending on
*        whether variance is present, and on the implementation type.
*        New workspace for counting is obtained for each axis array.
*        For axes determine whether or not weighted averages are to be
*        used to calculate output centres when variance is present.
*        For the AXIS arrays each axis array is processed
*        two-dimensionally, but with a dummy second dimension. They are
*        also averaged, not summed.  Axis width arrays are normalised. 
*     -  Tidy the NDF system.

*  Related Applications:
*     KAPPA: BLOCK, COMPAVE, COMPICK, PIXDUPE, SQORST, TRANSFORMER;
*     Figaro: ISTRETCH, YSTRACT.

*  Implementation Status:
*     -  This routine correctly processes the AXIS, DATA, VARIANCE,
*     LABEL, TITLE, UNITS, and HISTORY components of the input NDF and
*     propagates all extensions.  QUALITY is not processed since it is
*     a series of flags, not numerical values.
*     -  Processing of bad pixels and automatic quality masking are
*     supported.
*     -  All non-complex numeric data types can be handled.
*     -  Any number of NDF dimensions is supported.

*  Authors:
*     MJC: Malcolm J. Currie (STARLINK)
*     DSB: David S. Berry (STARLINK)
*     {enter_new_authors_here}

*  History:
*     1991 November 30 (MJC):
*        Original version.
*     1995 January 11 (MJC):
*        Made TITLE propagate from the input NDF.  Used PSX for
*        workspace.
*     27-FEB-1998 (DSB):
*        Type of local variable AXWT corrected from INTEGER to LOGICAL.
*     10-JUN-1998 (DSB):
*        Propagate WCS component. Ensure each output dimension is at least
*        one pixel long. 
*     {enter_further_changes_here}

*  Bugs:
*     {note_new_bugs_here}

*-

*  Type Definitions:
      IMPLICIT NONE            ! No default typing allowed

*  Global Constants:
      INCLUDE  'SAE_PAR'       ! Global SSE definitions
      INCLUDE  'PAR_ERR'       ! Parameter-system errors
      INCLUDE  'NDF_PAR'       ! NDF_ public constants

*  Status:
      INTEGER STATUS

*  Local Variables:
      CHARACTER
     :  COMP * ( 15 ),         ! List of components to process
     :  CTYPE * ( NDF__SZTYP ),! Numeric type for counting workspace
     :  DTYPE * ( NDF__SZFTP ),! Numeric type for output arrays
     :  ITYPE * ( NDF__SZTYP ),! Numeric type for processing
     :  TYPE * ( NDF__SZTYP )  ! Data type of an array component

      DOUBLE PRECISION
     :  MATRIX( NDF__MXDIM*NDF__MXDIM ),! Matrix component of linear mapping
     :  OFFSET( NDF__MXDIM )   ! Translation component of linear mapping

      INTEGER
     :  ACTVAL,                ! Actual number of compression factors
     :  ACOMPR( 2 ),           ! 1-d compression for axis (2nd factor
                               ! is dummy for subroutines)
     :  ADIMS( 2 ),            ! Axis dimension (2nd dimension is dummy
                               ! for subroutines).
     :  CMPMAX( NDF__MXDIM ),  ! Maximum compression factors
     :  CMPMIN( NDF__MXDIM ),  ! Minimum compression factors
     :  COMPRS( NDF__MXDIM ),  ! Compression factors
     :  EL,                    ! Number of elements in mapped array
     :  ELA,                   ! Number of elements in input axis array
     :  ELWS1,                 ! Number of elements in summation
                               ! workspace
     :  ELWS2,                 ! Number of elements in counting
                               ! workspace
     :  I, J,                  ! Loop counters for the dimensions
     :  IAXIS,                 ! Loop counter for the axis-array
                               ! components
     :  IDIMS( NDF__MXDIM ),   ! Dimensions of input NDF
     :  LBND( NDF__MXDIM ),    ! Lower bounds of input NDF
     :  LBNDO( NDF__MXDIM )    ! Lower bounds of output NDF

      INTEGER
     :  NDFI,                  ! Identifier to the input NDF
     :  NDFO,                  ! Identifier to the output NDF
     :  NDFS,                  ! Identifier to the section of the input
                               ! NDF
     :  NDIM,                  ! Padded dimensionality of the NDF
     :  NDIMI,                 ! Actual dimensionality of the NDF
     :  NLIM,                  ! Minimum number of elements in input
                               ! addition box to form good output value
     :  ODIMS( NDF__MXDIM ),   ! Dimensions of output array
     :  PNTRI( 2 ),            ! Pointer to input array component(s)
     :  PNTRO( 2 ),            ! Pointer to output array component(s)
     :  TOTCMP,                ! Total compression factor
     :  UBND( NDF__MXDIM ),    ! Upper bounds of input NDF
     :  UBNDO( NDF__MXDIM ),   ! Upper bounds of output NDF
     :  WPNTR1,                ! Pointer to summation workspace
     :  WPNTR2                 ! Pointer to counting workspace

      LOGICAL                  ! True if:
     :  AVAR,                  ! Axis variance is present
     :  AXIS,                  ! Axis structure is present
     :  AXWT,                  ! Axis weighted averages
     :  NORMAL,                ! Normalise the summations for bad values
     :  PRESTY,                ! Preserve the input array's data type in
                               ! the output arrays
     :  VAR,                   ! Variance is present
     :  WIDTH                  ! Axis width is present

      REAL
     :  WLIM                   ! Fraction of good pixels required

*.

*  Check the global status.
      IF ( STATUS .NE. SAI__OK ) RETURN

*  Start an NDF context.
      CALL NDF_BEGIN

*  Obtain the input NDF.
      CALL NDF_ASSOC( 'IN', 'READ', NDFI, STATUS )

*  Inquire the bounds and dimensions of the NDF.
      CALL NDF_BOUND( NDFI, NDF__MXDIM, LBND, UBND, NDIM, STATUS )
      CALL NDF_DIM( NDFI, NDF__MXDIM, IDIMS, NDIM, STATUS )

*  The subroutines require that there must be at least two dimensions
*  in the arrays, but higher ones may be dummies.
      NDIMI = NDIM
      NDIM = MAX( NDIM, 2 )

*  Obtain the compression factors.
*  ===============================
*
*  Set the acceptable range of values from no compression to compress
*  to a single element in a dimension.  Initialise values in case of an
*  error to prevent a possible divide-by-zero catastrophe.
      DO I = 1, NDIM
         CMPMIN( I ) = 1
         CMPMAX( I ) = IDIMS( I )
         COMPRS( I ) = 1
      END DO

*  Get the compression factors.
      CALL PAR_GRMVI( 'COMPRESS', NDIM, CMPMIN, CMPMAX, COMPRS, ACTVAL,
     :                STATUS )
      IF ( STATUS .NE. SAI__OK ) GOTO 999

*  Should less values be entered than is required copy the last value to
*  higher dimensions, limiting it to be no smaller than the corresponding
*  input NDF axis.
      IF ( ACTVAL .LT. NDIM ) THEN
         DO I = ACTVAL + 1, NDIM
            COMPRS( I ) = MIN( IDIMS( I ), COMPRS( ACTVAL ) )
         END DO
      END IF

*  Check there is going to be a compression.
*  =========================================

*  Find total compression.
      TOTCMP = 1
      DO I = 1, NDIM
         TOTCMP = TOTCMP * COMPRS( I )
      END DO

*  Report and abort if there is no compression.
      IF ( TOTCMP .EQ. 1 ) THEN
         STATUS = SAI__ERROR
         CALL ERR_REP( 'ERR_COMPADD_NOCMPR',
     :     'COMPADD: There is no compression to be made.', STATUS )
         GOTO 999
      END IF

*  Obtain the minimum fraction of good pixels which should be used to
*  calculate an output pixel value.
      CALL PAR_GDR0R( 'WLIM', 0.3, 0.0, 1.0, .FALSE., WLIM, STATUS )

*  Derive the minimum number of pixels, using at least one.
      NLIM = MAX( 1, NINT( REAL( TOTCMP ) * WLIM ) )

*  Determine if a variance component is present and derive a list of
*  the components to be processed together.  The variance component
*  cannot be processed alone, since it needs to know about bad pixels
*  in the data array, so only the variances of good data pixels are
*  used to find a resultant variance.  The bad-pixel flag could be
*  tested, but since we have to call the routine when there may be bad
*  pixels that would only serve to lengthen this routine.
      CALL NDF_STATE( NDFI, 'Variance', VAR, STATUS )
      IF ( VAR ) THEN
         COMP = 'Data,Variance'
      ELSE
         COMP = 'Data'
      END IF

*  Obtain the the flag to decide whether or not to normalise the
*  summations to allow for bad pixels.
      CALL PAR_GTD0L( 'NORMAL', .TRUE., .FALSE., NORMAL, STATUS )

*  Obtain the the flag to decide whether or not to preserve the input
*  type though this may lose precision.
      CALL PAR_GTD0L( 'PRESERVE', .FALSE., .FALSE., PRESTY, STATUS )
      IF ( STATUS .NE. SAI__OK ) GOTO 999

*  Compute the output NDF's dimensions.
*  ====================================

*  Work out the size of the output array from the input array
*  dimensions and the compression factor.  It relies on integer
*  arithmetic truncation.  Also derive bounds for the output array.
*  These are somewhat arbitrary.
      DO I = 1, NDIM
         ODIMS( I ) = MAX( 1, IDIMS( I ) / COMPRS( I ) )
         LBNDO( I ) = 1
         UBNDO( I ) = ODIMS( I )
      END DO

*  Create the output NDF.
*  ======================
*
*  Take a shortcut to propagate ancillary data from the input NDF.
*  Create a section from the input NDF of the size of the required NDF.
      CALL NDF_SECT( NDFI, NDIM, LBNDO, UBNDO, NDFS, STATUS )

*  Create the output NDF based on the sub-section.  The array components
*  and axes will be processed individually, but this enables the LABEL,
*  UNITS, HISTORY, AXIS character components, and extensions to be
*  propagated.
      CALL NDF_PROP( NDFS, 'Axis,Units', 'OUT', NDFO, STATUS )

*  Obtain a title and assign it to the output NDF.
*  ===============================================

*  A null results in the output title being the same as the input
*  title.
      CALL KPG1_CCPRO( 'TITLE', 'TITLE', NDFI, NDFO, STATUS )
      IF ( STATUS .NE. SAI__OK ) GOTO 999

*  Compress the data array.
*  ========================
*
*  Determine the processing type and the data type of the output data
*  array and variance.

*  Determine the numeric type to be used for processing the input
*  data and variance (if any) arrays.  Since the subroutines that
*  perform the addition need the data and variance arrays in the same
*  data type, the component list is used.  This application supports
*  single- and double-precision floating-point processing.
      CALL NDF_MTYPE( '_REAL,_DOUBLE', NDFI, NDFI, COMP, ITYPE, DTYPE,
     :                STATUS )

*  When the types of the input NDF's arrays are to be preserved, just
*  inquire what it is and set the output NDF type accordingly.  Notice
*  the data and variance arrays may be different, and so are handled
*  separately.  Otherwise the output arrays will have the mapped data
*  type upon unmapping.
      IF ( PRESTY ) THEN
         CALL NDF_TYPE( NDFI, 'Data', TYPE, STATUS )
         CALL NDF_STYPE( TYPE, NDFO, 'Data', STATUS )
         IF ( VAR ) THEN
            CALL NDF_TYPE( NDFI, 'Variance', TYPE, STATUS )
            CALL NDF_STYPE( TYPE, NDFO, 'Variance', STATUS )
         END IF
      END IF

*  Set the quantity of workspace required for addition.  In order to
*  save time by not annulling and then re-creating slightly
*  different-sized summation work arrays, the first is made
*  sufficiently large to to satisfy all requests.  Also set the type of
*  the space for counting required by the appropriate summing
*  subroutine.
      ELWS1 = 1
      DO I = 1, NDIM
         ELWS1 = MAX( ELWS1, IDIMS( I ) )
      END DO
      ELWS1 = ELWS1 * 2
      IF ( VAR ) THEN
         ELWS2 = IDIMS( 1 ) * 2
      ELSE
         ELWS2 = IDIMS( 1 )
      END IF

*  Obtain some workspace for the additions and map them.  First find
*  the quantity and the type of the counting space required.
      CALL PSX_CALLOC( ELWS1, ITYPE, WPNTR1, STATUS )
      CALL PSX_CALLOC( ELWS2, ITYPE, WPNTR2, STATUS )

*  Map the full input, and output data arrays.
      CALL NDF_MAP( NDFI, COMP, ITYPE, 'READ', PNTRI, EL, STATUS )
      CALL NDF_MAP( NDFO, COMP, ITYPE, 'WRITE', PNTRO, EL, STATUS )

*  Compress the input array to make the output array by addition in
*  non-overlapping boxes.  Call the appropriate routine depending on
*  whether there is variance or not, and the implementation type.
      IF ( VAR ) THEN
         IF ( ITYPE .EQ. '_REAL' ) THEN
            CALL KPG1_CMVDR( NDIM, IDIMS, %VAL( PNTRI( 1 ) ),
     :                       %VAL( PNTRI( 2 ) ), COMPRS, NLIM,
     :                       NORMAL, %VAL( PNTRO( 1 ) ),
     :                       %VAL( PNTRO( 2 ) ), %VAL( WPNTR1 ),
     :                       %VAL( WPNTR2 ), STATUS )

         ELSE IF ( ITYPE .EQ. '_DOUBLE' ) THEN
            CALL KPG1_CMVDD( NDIM, IDIMS, %VAL( PNTRI( 1 ) ),
     :                       %VAL( PNTRI( 2 ) ), COMPRS, NLIM,
     :                       NORMAL, %VAL( PNTRO( 1 ) ),
     :                       %VAL( PNTRO( 2 ) ), %VAL( WPNTR1 ),
     :                       %VAL( WPNTR2 ), STATUS )
         END IF
      ELSE
         IF ( ITYPE .EQ. '_REAL' ) THEN
            CALL KPG1_CMADR( NDIM, IDIMS, %VAL( PNTRI( 1 ) ),
     :                       COMPRS, NLIM, NORMAL, %VAL( PNTRO( 1 ) ),
     :                       %VAL( WPNTR1 ), %VAL( WPNTR2 ), STATUS )

         ELSE IF ( ITYPE .EQ. '_DOUBLE' ) THEN
            CALL KPG1_CMADD( NDIM, IDIMS, %VAL( PNTRI( 1 ) ),
     :                       COMPRS, NLIM, NORMAL, %VAL( PNTRO( 1 ) ),
     :                       %VAL( WPNTR1 ), %VAL( WPNTR2 ), STATUS )
         END IF
      END IF

*    Tidy the data and variance arrays.
      CALL NDF_UNMAP( NDFI, COMP, STATUS )
      CALL NDF_UNMAP( NDFO, COMP, STATUS )

*    Tidy the second workspace array, since it's type may not be
*    constant when processing the axis arrays---it could be the
*    implementation type or _INTEGER.
      CALL PSX_FREE( WPNTR2, STATUS )

*  Compress AXIS centre and variance arrays.
*  =========================================
*
*  First see whether or not there is an AXIS structure to compress.
      CALL NDF_STATE( NDFI, 'Axis', AXIS, STATUS )

      IF ( AXIS ) THEN

*  Obtain the the flag to decide whether or not the axis variance, if
*  present is used to weight the centres.
         CALL PAR_GTD0L( 'AXWEIGHT', .FALSE., .FALSE., AXWT, STATUS )

*  Define dummy compression factor and dimension.
         ACOMPR( 2 ) = 1
         ADIMS( 2 ) = 1

*  Loop for all axes.
         DO IAXIS = 1, NDIM

*  Inquire whether or not there is a variance array and derive a list
*  of the axis array components to be processed together.  The variance
*  component cannot be processed alone, since it needs to know about
*  bad values in the centre array, so only the variances of good centre
*  values are used to find a resultant variance.  Also define the size
*  and type of the counting workspace.  Note this assumes that the
*  processing type of the main data and variance is going to be used
*  for the axis centre and variance arrays (see below for the reason
*  why).
            AVAR = .FALSE.
            CALL NDF_ASTAT( NDFI, 'Variance', IAXIS, AVAR, STATUS )
            IF ( AVAR ) THEN
               COMP = 'Centre,Variance'
               CTYPE = ITYPE
               ELWS2 = IDIMS( IAXIS ) * 2
            ELSE
               COMP = 'Centre'
               CTYPE = '_INTEGER'
               ELWS2 = IDIMS( IAXIS )
            END IF

*  When the types of the input NDF's arrays are to be preserved, just
*  inquire what it is and set the output NDF type accordingly.  Notice
*  the data and variance arrays may be different, and so are handled
*  separately.  Otherwise the output axis arrays will have the mapped
*  data type upon unmapping.
            IF ( PRESTY ) THEN
               CALL NDF_ATYPE( NDFI, 'Centre', IAXIS, TYPE, STATUS )
               CALL NDF_ASTYP( TYPE, NDFO, 'Centre', IAXIS, STATUS )
               IF ( AVAR ) THEN
                  CALL NDF_ATYPE( NDFI, 'Varianc', IAXIS, TYPE, STATUS )
                  CALL NDF_ASTYP( TYPE, NDFO, 'Varianc', IAXIS, STATUS )
               END IF
            END IF

*  Obtain the mapped workspace for counting.
            CALL PSX_CALLOC( ELWS2, CTYPE, WPNTR2, STATUS )

*  Since there is no NDF_AMTYP for matching types, we'll have to assume
*  the implementation type of the data and/or main variance arrays
*  is satisfactory.  Map the input and output axis arrays.
            CALL NDF_AMAP( NDFI, COMP, IAXIS, ITYPE, 'READ',
     :                     PNTRI, ELA, STATUS )
            CALL NDF_AMAP( NDFO, 'Centre', IAXIS, ITYPE, 'WRITE',
     :                     PNTRO, EL, STATUS )

*  Fill in the actual compression factor and the dimension for the axis.
            ACOMPR( 1 ) = COMPRS( IAXIS )
            ADIMS( 1 ) = IDIMS( IAXIS )

*  Compress the input array to make the output array by averaging in
*  non-overlapping boxes.  Note that each axis array is 1-dimensional,
*  so pass just the required dimension, and compression factor.  Call
*  the appropriate routine depending on whether there is variance or
*  not, and the implementation type.  Since the original averaging
*  workspace is of sufficient length, is initialised each time it is
*  used, and also because we are using the same implementation type as
*  the data and main variance arrays we can re-cycle it.  If an
*  NDF_AMTYP is written then this will have to change.
            IF ( AVAR ) THEN
               IF ( ITYPE .EQ. '_REAL' ) THEN
                  CALL KPG1_CMVVR( 2, ADIMS, %VAL( PNTRI( 1 ) ),
     :                             %VAL( PNTRI( 2 ) ), ACOMPR,
     :                             NLIM, AXWT, %VAL( PNTRO( 1 ) ),
     :                             %VAL( PNTRO( 2 ) ), %VAL( WPNTR1 ),
     :                             %VAL( WPNTR2 ), STATUS )

               ELSE IF ( ITYPE .EQ. '_DOUBLE' ) THEN
                  CALL KPG1_CMVVD( 2, ADIMS, %VAL( PNTRI( 1 ) ),
     :                             %VAL( PNTRI( 2 ) ), ACOMPR,
     :                             NLIM, AXWT, %VAL( PNTRO( 1 ) ),
     :                             %VAL( PNTRO( 2 ) ), %VAL( WPNTR1 ),
     :                             %VAL( WPNTR2 ), STATUS )
               END IF
            ELSE
               IF ( ITYPE .EQ. '_REAL' ) THEN
                  CALL KPG1_CMAVR( 2, ADIMS, %VAL( PNTRI( 1 ) ),
     :                             ACOMPR, NLIM, %VAL( PNTRO( 1 ) ),
     :                             %VAL( WPNTR1 ), %VAL( WPNTR2 ),
     :                             STATUS )

               ELSE IF ( ITYPE .EQ. '_DOUBLE' ) THEN
                  CALL KPG1_CMAVD( 2, ADIMS, %VAL( PNTRI( 1 ) ),
     :                             ACOMPR, NLIM, %VAL( PNTRO( 1 ) ),
     :                             %VAL( WPNTR1 ), %VAL( WPNTR2 ),
     :                             STATUS )
               END IF
            END IF

*  Tidy the centre and variance arrays.
            CALL NDF_AUNMP( NDFI, COMP, IAXIS, STATUS )
            CALL NDF_AUNMP( NDFO, COMP, IAXIS, STATUS )

*  Tidy the counting workspace.
            CALL PSX_FREE( WPNTR2, STATUS )
         END DO

*  Extend the AXIS width arrays.
*  =============================

*  Loop for all axes.
         DO IAXIS = 1, NDIM

*  Inquire whether or not there is a width array.
            WIDTH = .FALSE.
            CALL NDF_ASTAT( NDFI, 'Width', IAXIS, WIDTH, STATUS )

            IF ( WIDTH ) THEN

*  When the type of the input NDF's axis width array is to be preserved,
*  just inquire what it is and set the output type accordingly.
               IF ( PRESTY ) THEN
                  CALL NDF_ATYPE( NDFI, 'Width', IAXIS, TYPE, STATUS )
                  CALL NDF_ASTYP( TYPE, NDFO, 'Width', IAXIS, STATUS )
               END IF

*  Obtain the workspace for counting.
               CALL PSX_CALLOC( IDIMS( IAXIS ), '_INTEGER', WPNTR2,
     :                          STATUS )

*  Since there is no NDF_AMTYP for matching types, we'll have to assume
*  the implementation type of the data and/or main variance arrays
*  is satisfactory.  Map the input and output width arrays.
               CALL NDF_AMAP( NDFI, 'Width', IAXIS, ITYPE, 'READ',
     :                        PNTRI, ELA, STATUS )
               CALL NDF_AMAP( NDFO, 'Width', IAXIS, ITYPE, 'WRITE',
     :                        PNTRO, EL, STATUS )

*  Fill in the actual compression factor and the dimension for the axis.
               ACOMPR( 1 ) = COMPRS( IAXIS )
               ADIMS( 1 ) = IDIMS( IAXIS )

*  Compress the input width array to make the output array by SUMMING
*  in non-overlapping boxes.  Note that each axis array is
*  1-dimensional, so pass just the required dimension, and compression
*  factor.  Call the appropriate routine depending on the
*  implementation type.  Since the original workspace is sufficiently
*  large, initialised each time it is used we can re-cycle just one
*  pair of arrays, and also because we are using the same
*  implementation type as the data and main variance arrays.  If an
*  NDF_AMTYP is written then this will have to change.  The widths are
*  normalised in case there are undefined widths present.
               IF ( ITYPE .EQ. '_REAL' ) THEN
                  CALL KPG1_CMADR( 2, ADIMS, %VAL( PNTRI( 1 ) ),
     :                             ACOMPR, NLIM, .TRUE.,
     :                             %VAL( PNTRO( 1 ) ), %VAL( WPNTR1 ),
     :                             %VAL( WPNTR2 ), STATUS )

               ELSE IF ( ITYPE .EQ. '_DOUBLE' ) THEN
                  CALL KPG1_CMADD( 2, ADIMS, %VAL( PNTRI( 1 ) ),
     :                             ACOMPR, NLIM, .TRUE.,
     :                             %VAL( PNTRO( 1 ) ), %VAL( WPNTR1 ),
     :                             %VAL( WPNTR2 ), STATUS )
               END IF

*  Tidy the axis-width arrays.
               CALL NDF_AUNMP( NDFI, 'Width', IAXIS, STATUS )
               CALL NDF_AUNMP( NDFO, 'Width', IAXIS, STATUS )

*  Tidy the counting workspace.
               CALL PSX_FREE( WPNTR2, STATUS )
            END IF
         END DO
      END IF

*  Propagate the WCS component, incorporating a linear mapping between
*  pixel coordinates. This mapping is described by a matrix and an offset
*  vector. Set these up. 
      DO I = 1, NDIMI*NDIMI
         MATRIX( I ) = 0.0
      END DO

      DO J = 1, NDIMI
         OFFSET( J ) = DBLE( 1 - LBND( J ) )/DBLE( COMPRS( J ) )
         MATRIX( NDIMI*( J - 1 ) + J ) = 1.0D0/DBLE( COMPRS( J ) )
      END DO

*  Propagate the WCS component.
      CALL KPG1_ASPRP( NDIMI, NDFI, NDFO, MATRIX, OFFSET, STATUS )

*  Tidy the counting workspace.
      CALL PSX_FREE( WPNTR1, STATUS )

*  Come here if something has gone wrong.
  999 CONTINUE

*  Tidy the NDF system.
      CALL NDF_END( STATUS )

      IF ( STATUS .NE. SAI__OK ) THEN
         CALL ERR_REP( 'COMPADD_ERR',
     :     'COMPADD: Unable to compress an NDF by summing '/
     :     /'neighbouring values.', STATUS )
      END IF

      END
