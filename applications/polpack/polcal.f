      SUBROUTINE POLCAL( STATUS )
*+
*  Name:
*     POLCAL

*  Purpose:
*     Converts a set of analysed intensity images into a cube holding Stokes 
*     vectors.

*  Language:
*     Starlink Fortran 77

*  Type of Module:
*     ADAM A-task

*  Invocation:
*     CALL POLCAL( STATUS )

*  Arguments:
*     STATUS = INTEGER (Given and Returned)
*        The global status.

*  Description:
*     This application converts a set of 2D intensity images into a
*     3D data cube holding a Stokes vector at every pixel in the area
*     covered by the supplied intensity images.
*
*     Either dual or single beam data can be processed, with an
*     appropriate algorithm being used in each case. There is also an
*     option to process dual-beam data using the single beam algorithm
*     (see parameter DUALBEAM).
*
*     All the input images should be aligned pixel-for-pixel, and should
*     have had the sky background removed. O and E ray images in
*     dual-beam data should have been extracted into separate images. 
*
*     The output consists of a 3D cube containing values of I, Q and U
*     in planes 1, 2 and 3. Currently, circular polarimetry can only be 
*     performed by the dual-beam algorithm, in which case the output cube 
*     contains only 2 planes holding I and V values (see parameter PMODE). 
*     If all the input images contain variances, and the VARIANCE parameter 
*     is not set to FALSE, then the output cube will also contain variances.

*  Usage:
*     polcal in out

*  ADAM Parameters:
*     DUALBEAM = _LOGICAL (Read)
*        If TRUE, then the input images are processed as dual-beam data.
*        If FALSE they are processed as single-beam data. If a null (!)
*        value is supplied, the dual-beam algorithm is used if all the input
*        images contain a legal value for the RAY item in the POLPACK
*        extension (i.e. either "E" or "O"). Otherwise, the single-beam
*        algorithm is used. It is possible to process dual-beam data using
*        the single-beam algorithm, so DUALBEAM may be set to FALSE even
*        if the input images hold dual-beam data. However, the opposite
*        is not true; the application will abort if DUALBEAM is set TRUE and 
*        any of the input images contain single-beam data. [!]
*     ETOL = _REAL (Read)
*        This parameter is only accessed by the dual-beam algorithm. The E 
*        factors are found using an iterative procedure in which the supplied 
*        intensity images are corrected using the current estimates of the E 
*        factors, and new estimates are then calculated on the basis of these 
*        corrected images. This procedure continues until the change in 
*        E-factor produced by an iteration is less than the value supplied 
*        for ETOL, or the maximum number of iterations specified by parameter 
*        MAXIT is reached. [0.01]
*     ILEVEL = _INTEGER (Read)
*        Specifies the amount of information to display on the screen.
*        Zero suppresses all output. A value of 1 produces minimal output
*        describing such things as warnings and the E and F factors
*        adopted (in dual-beam mode). A value of 2 produces more verbose 
*        output including details of each iteration in the iterative
*        processes used by both dual and single beam modes. [1]
*     IN = NDF (Read)
*        A group specifying the names of the input intensity images. This
*        may take the form of a comma separated list, or any of the other 
*        forms described in the help on "Group Expressions".
*     MAXIT = _INTEGER (Read)
*        This parameter is only accessed by the dual-beam algorithm. It
*        specifies the maximum number of iterations to be used when 
*        inter-comparing pairs of input images to determine their relative 
*        scale-factor and/or zero-point. If the specified number of 
*        iterations is exceeded without achieving the accuracy required by 
*        the settings of the TOLS and TOLZ parameters, then a warning message 
*        will be issued, but the results will still be used. The value given 
*        for MAXIT must be at least one. [30]
*     NITER = _INTEGER (Read)
*        This parameter is only accessed by the single-beam algorithm. It
*        specifies the maximum number of iterations to be used when 
*        rejecting aberrant input values. It may be set to zero to
*        prevent iterations and so speed up the processing. [1]
*     NSIGMA = _REAL (Read)
*        This parameter is only accessed by the single-beam algorithm. It
*        specifies the threshold at which to reject input data values. If
*        an input data value differs from the data value implied by the
*        current I, Q and U values by more than NSIGMA standard deviations, 
*        then it will not be included in the next calculation of I, Q and U.
*        If parameter VARIANCE is TRUE, then the standard deviation is 
*        obtained from the VARIANCE component of the input image. Otherwise,
*        a the standard deviation used is the RMS residual for the entire
*        image. [3.0]
*     OUT = NDF (Read)
*        The name of the output 3D cube holding the Stokes parameters.
*        The x-y plane of this cube covers the overlap region of the 
*        supplied intensity images. Plane 1 contains the total intensity. 
*        The other planes contain either Q and U, or V, depending on the 
*        value of parameter PMODE.
*     PMODE = LITERAL (Read)
*        This parameter is only accessed by the dual-beam algorithm. It
*        gives the mode of operation; CIRCULAR for measuring circular 
*        polarization, or LINEAR for measuring linear polarization. In 
*        circular mode, the only legal values for the POLPACK extension 
*        item "WPLATE" are 0.0 and 45.0. Currently, the single-beam algorithm 
*        can only perform linear polarimetry. [LINEAR]
*     SKYSUP = _REAL (Read)
*        This parameter is only accessed by the dual-beam algorithm. It is
*        a positive "sky noise suppression factor" used to control the
*        effects of sky noise when pairs of input images are
*        inter-compared to determine their relative scale-factor. It is
*        intended to prevent the resulting scale-factor estimate being
*        biased by the many similar values present in the "sky
*        background" of typical astronomical data.  SKYSUP controls an
*        algorithm which reduces the weight given to data where there
*        is a high density of points with the same value, in order to
*        suppress this effect. 
*
*        A SKYSUP value of unity can often be effective, but a value
*        set by the approximate ratio of sky pixels to useful object
*        pixels (i.e. those containing non-sky signal) in a "typical"
*        image will usually be better. The precise value
*        is not critical. A value of zero disables the sky noise
*        suppression algorithm completely. The default value for SKYSUP
*        is 10. This is normally reasonable for CCD frames of extended 
*        objects such as galaxies, but a larger value, say 100, may give 
*        slightly better results for star fields. [10]
*     TITLE = LITERAL (Read)
*        A title for the output cube. [Output from POLCAL]
*     TOLS = _REAL (Read)
*        This parameter is only accessed by the dual-beam algorithm. It
*        defines the accuracy tolerance to be achieved when inter-comparing 
*        pairs of input images to determine their relative scale-factor. The 
*        value given for TOLS specifies the tolerable fractional error in the 
*        estimation of the relative scale-factor between any pair of input 
*        NDFs. This value must be positive. [0.001]
*     TOLZ = _REAL (Read)
*        This parameter is only accessed by the dual-beam algorithm. It
*        defines the accuracy tolerance to be achieved when inter-comparing 
*        pairs of input images to determine their relative zero-points. The 
*        value given for TOLZ specifies the tolerable absolute error in the 
*        estimation of the relative zero-point between any pair of input 
*        images whose relative scale-factor is unity. The value used is 
*        multiplied by the relative scale-factor estimate (which reflects the 
*        fact that an image with a larger data range can tolerate a larger 
*        error in estimating its zero-point). The TOLS value supplied must 
*        be positive. [0.05]
*     VARIANCE = _LOGICAL (Read)
*        This parameter should be set to a TRUE value if variances are to
*        be included in the output cube. This can only be done if all the
*        input intensity images have associated variance values. A null (!)
*        value results in variance values being created if possible, but
*        not otherwise. [!]
 
*  The Single-beam Algorithm:
*        In single-bean mode, the I, Q and U values at each output pixel are 
*        chosen to minimise the sum of the squared residuals between the 
*        supplied intensity values and the intensity values implied by the I, 
*        Q and U values. Input intensity values are weighted by the reciprocal 
*        of the associated variance values when doing the fit if output 
*        variances are being created (see parameter VARIANCE). The algorithm 
*        is described by Sparks and Axon (submitted to P.A.S.P.). 
*
*        In order to provide some safe-guards against aberrant values in the 
*        input images, an iterative scheme is used which rejects input data 
*        values which are badly inconsistent with the calculated I, Q and U 
*        values. After rejecting such values, the algorithm goes on to 
*        re-calculate the I, Q and U values excluding the rejected input 
*        values. This process is repeated until no new pixels are rejected
*        or the maximum number of iterations specified by parameter NITER
*        is reached. Input values are rejected if they differ by more than
*        NSIGMA standard deviations from the value implied by the current
*        I, Q and U values. The standard deviation used is the square
*        root of the input variance value if output variances are being
*        created (see parameter VARIANCE), or the standard deviation of all
*        residuals in the current image otherwise.
*
*        Single beam mode can take account of imperfections in the analyser.
*        The transmission (i.e. the overall throughput) and efficiency
*        (i.e. the ability to reject light polarised across the axis) of the 
*        analyser are read from the POLPACK extension. If not found, values 
*        of 1.0 are used for both. These values are appropriate for a
*        perfect analyser. A pefectly bad analyser (a piece of high quality 
*        glass for instance) would have a transmission of 2.0 and an
*        efficiency of zero. The extension items named T and EPS hold the
*        tranmission and efficiency.
*
*        Single beam mode can handle data taken by polarimeters containing
*        a number of fixed analysers, or a single rotating analyser, in
*        addition to the normal combination of fixed analyser and rotating
*        half-wave plate. The POLPACK extension in each input image should 
*        contain either a WPLATE value (giving the angle between a fixed
*        analyser and the half-wave plate), or an ANLANG value (giving the
*        angle between the rotating or fixed analyser and the polarimeter 
*        reference direction). Only one of these two extension items
*        should be present. The WPLATE and ANLANG items are free to take
*        any value (i.e. they are not restricted to the values 0.0, 22.5,
*        45.0 and 67.5 degrees as in the dual-beam algorithm).
*
*        There are a couple of restrictions in single-beam mode. Firstly,
*        there is currently no facility for measuring circular
*        polarization. Secondly, no attempt is made to correct for
*        difference in the exposure times of the input images, which
*        should all have been normalised to the same exposure time.

*  The Dual-beam Algorithm:
*        In dual-beam mode, Q and U are estimated by taking the difference
*        between pairs of orthoganally polarized images (i.e. the O and E
*        ray images extracted from a single dual-beam image). The total
*        intensity I is estimated by summing such pairs of images. The
*        various estimates of these quantities provided by different pairs 
*        are stacked together using a median estimator to provide some
*        safe-guards against aberrant input values. Circular 
*
*        Only polarimeters with a fixed analyser and rotating half-wave plate
*        are supported, and the half-wave plate angle (given by item
*        WPLATE in the POLPACK extension of each input image) must be one
*        of 0, 22.5, 45 and 67.5 degrees. Other values cause the
*        application to abort.
*
*        If input images for all four half-wave plate positions are
*        provided (0, 22.5, 45 and 67.5 degrees) then a correction is
*        made for any difference in sensitivity of the two channels (such
*        as can be caused by the use of polarized flat-field for
*        instance). This correction is known as the "F-factor" and is
*        based on the redundancy provided by using four half-wave plate
*        positions. If images with the required half-wave plate positions
*        are not provided, then it is assumed that the two channels are
*        equally sensitive (that is, an F-factor of 1.0 is used).
*
*        Corrections are also made for any difference in exposure times
*        for the supplied intensity images. These are based on the fact
*        that the sum of the O and E ray intensities should always equal
*        the total intensity (after any F-factor correction), and should
*        therefore be the same for all pairs of corresponding O and E ray
*        images if they have equal exposure times.
*
*        The E and F factors are calculated by inter-comparing pairs of
*        intensity images to estimate their relative scale factor and
*        zero point. This estimation is an iterative process, and is
*        controlled by parameters TOLS, TOLZ, SKYSUP and MAXIT.

*  Examples:
*     polcal "*_O,*_E" stokes
*        This example uses all images in the current directory which have
*        file names ending with either "_O" or "_E", and stores the
*        corresponding I, Q and U values in the 3d cube "stokes". These
*        images contain dual-beam data (indicated by the presence of the
*        RAY item in the POLPACK extension), and so the dual-beam
*        algorithm is used.
*     polcal "*_O,*_E" stokes nodualbeam
*       As above, but the data is processed as if it were single beam data.

*  Notes:
*     -  An item named STOKES is added to the POLPACK extension. It is a
*     character string identifying the quantity stored in each plane of
*     the cube. For linear polarimetry, it is set to "IQU", and for
*     circular polarimetry it is set to "IV".
*     -  WCS and AXIS components are propagated from the first supplied
*     input image to the output cube. 

*  Copyright:
*     Copyright (C) 1998 Central Laboratory of the Research Councils
 
*  Authors:
*     TMG: Tim Gledhill (STARLINK)
*     DSB: David S. Berry (STARLINK)
*     {enter_new_authors_here}

*  History:
*     28-JUL-1997 (TMG):
*        Original version.
*     15-MAY-1998 (DSB):
*        Prologue re-written.
*     02-JUN-1998 (TMG):
*        Added extra workspace array IPID to hold character identifiers
*     4-JUN-1998 (DSB):
*        Modified allocation of IPID workspace to include length of each
*        character identifier. Extended image identifiers from max of 10
*        characters to 30 characters. Modified call to POL_CALE to include
*        trailing length specifier for IPID strings.
*     8-FEB-1998 (DSB):
*        Added single beam mode.
*     {enter_changes_here}

*  Bugs:
*     {note_any_bugs_here}

*-
      
*  Type Definitions:
      IMPLICIT NONE              ! No implicit typing

*  Global Constants:
      INCLUDE 'SAE_PAR'          ! Standard SAE constants
      INCLUDE 'PAR_ERR'          ! PAR error constants
      
*  Status:
      INTEGER STATUS             ! Global status

*  Local Variables:
      INTEGER DBEAM              ! Dual/single beam mode flag
      INTEGER IGRP               ! GRP identifier for input images group      
      INTEGER ILEVEL             ! Screen information level
      INTEGER NNDF               ! No. of input images to process      
      INTEGER VAR                ! Variances required flag
      LOGICAL LVAL               ! Logical parameter value
*.

*  Check inherited global status.
      IF( STATUS .NE. SAI__OK ) RETURN

*  Start an NDF context.
      CALL NDF_BEGIN

*  Get a group containing the names of the object frames to be used.
      CALL RDNDF( 'IN', 0, 1, '  Give more image names...', 
     :            IGRP, NNDF, STATUS )

*  Get the amount of information to be displayed on the screen.
      CALL PAR_GET0I( 'ILEVEL', ILEVEL, STATUS )

*  Report the number of NDFs found to the user.
      IF( ILEVEL .GT. 0 ) THEN
         CALL MSG_SETI( 'NNDF', NNDF )
         CALL MSG_OUT( 'POLCAL_MSG1', 'Processing ^NNDF images...',
     :                 STATUS )
      END IF

*  Abort if an error has occurred.
      IF( STATUS .NE. SAI__OK ) GO TO 999

*  See if variances are to be created. If a null parameter value is
*  supplied, annul the error and leave the choice to be made later
*  on the basis of the supplied images (i.e. create variances if all
*  the supplied images contain variances, and do not create variances
*  if any do not). VAR > 0 implies create variances; VAR < 0 implies 
*  do not create variances; and VAR == 0 implies "create variances if
*  possible".
      CALL PAR_GET0L( 'VARIANCE', LVAL, STATUS )

      IF( STATUS .EQ. PAR__NULL ) THEN
         CALL ERR_ANNUL( STATUS )
         VAR = 0

      ELSE IF( LVAL ) THEN
         VAR = 1

      ELSE
         VAR = -1
      END IF

*  Abort if an error has occurred.
      IF( STATUS .NE. SAI__OK ) GO TO 999

*  See if we should run in single neam or dual beam mode. If a null
*  parameter value is supplied, annul the error and leave the choice to
*  be made later on the basis of the supplied images (i.e. run in dual
*  beam mode if all the supplied images contain dual-beam data, and run 
*  in single beam mode if any do not). DBEAM > 0 implies dual-beam; 
*  DBEAM < 0 implies single-beam; and DBEAM == 0 implies "dual-beam if
*  possible otherwise single-beam".
      CALL PAR_GET0L( 'DUALBEAM', LVAL, STATUS )

      IF( STATUS .EQ. PAR__NULL ) THEN
         CALL ERR_ANNUL( STATUS )
         DBEAM = 0

      ELSE IF( LVAL ) THEN
         DBEAM = 1

      ELSE
         DBEAM = -1
      END IF

*  Unless the user has specifically requested single-beam mode, try
*  dual-beam mode first. The DBEAM value will be set > 0 on exit if
*  the data was succesfully processed as dual-beam data.
      IF( DBEAM .GE. 0 ) CALL POL1_DULBM( IGRP, VAR, DBEAM, ILEVEL, 
     :                                    STATUS )

*  If the data was not succesfully processed as dual-beam data, try
*  single beam mode.
      IF( DBEAM .LE. 0 ) CALL POL1_SNGBM( IGRP, VAR, ILEVEL, STATUS )

* Tidy up.
 999  CONTINUE
      
*  Delete the group holding the NDF names.
      CALL GRP_DELET( IGRP, STATUS )

*  End the NDF context.
      CALL NDF_END( STATUS )

*  If an error occurred, then report a contextual message.
      IF ( STATUS .NE. SAI__OK ) THEN
         CALL ERR_REP( 'POLCAL_ERROR', 'POLCAL: Error producing '//
     :                 'Stokes parameters.', STATUS )
      END IF

      END
