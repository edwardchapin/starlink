#include <stdio.h>
#include <math.h>
#include <string.h>

#include "sae_par.h"
#include "ast.h"
#include "ndf.h"
#include "sc2ast.h"
#include "Ers.h"

/* -----------------------------------------------------------------------*/
/* Temporary kludge required for sc2ast_createwcs_compat   EC 05Dec2005   */
/*                       and for sc2ast_kludgemodjuldate                  */
#ifndef PI
#define PI 3.14159265358979            /* math constant */
#endif

#ifndef dmod
#define dmod(A,B) ((B)!=0.0?((A)*(B)>0.0?(A)-(B)*floor((A)/(B))\
                                        :(A)+(B)*floor(-(A)/(B))):(A))
#endif

#ifndef DS2R
#define DS2R 7.2722052166430399038487115353692196393452995355905e-5
#endif
/* -----------------------------------------------------------------------*/

static char errmess[132];              /* For DRAMA error messages */

#define MM2RAD 2.4945e-5               /* scale at array in radians */
#define PIBY2 1.57079632679
#define PIX2MM 1.135                   /* pixel interval in mm */
#define SPD 86400.0                    /* Seconds per day */
#define JCMT_LON "W155:28:37.20"       /* Longitude of JCMT */
#define JCMT_LAT "N19:49:22.11"        /* Geodetic latitude of JCMT */
const double RAD2DEG = 90.0 / PIBY2;   /* Convert Radians to degrees */

/*+ sc2ast_createwcs - create WCS description */

void sc2ast_createwcs
(
int subnum,             /* subarray number, 0-7 (given). If -1 is
                           supplied the cached AST objects will be freed. */
double az,              /* Boresight azimuth in radians (given) */
double el,              /* Boresight elevation in radians (given) */
double tai,             /* TAI (supplied as a Modified Julian Date) */
int extra_frames,       /* Ignored (i.e. no longer used) */
AstFrameSet **fset,     /* constructed frameset (returned) */
int *status             /* global status (given and returned) */
)
/* Method :
     Build an AST frameset containing mappings from the original GRID
     coordinates of the bolometers to celestial coordinates. This
     includes the rotations and reflections relevant to each subarray and
     the distortion imposed by the SCUBA-2 optics.
 
     This function allocates static resources (AST object pointers) which
     should be freed when no longer needed by calling this function with
     "subnum" set to -1. In this is done, the cached resources are freed
     and this function returns without further action, returning a NULL 
     pointer in "*fset".

   Authors :
     B.D.Kelly (bdk@roe.ac.uk)
     Tim Jenness (timj@jach.hawaii.edu)
     D.S. Berry (dsb@ast.man.ac.uk)

   History :
     01Apr2005 : original (bdk)
     21Apr2005 : update comments and add labels to some of the frames
                 (bdk)
     25Apr2005 : revise polynomials and plate scale (bdk)
     27Apr2005 : put in subarray orientations (bdk)
     10May2005 : remember xoff and yoff are in pixel units (bdk)
     10May2005 : fix syntax error in xoff and add more frame names (bdk)
     18May2005 : Check subnam range (timj)
     20Jun2005 : add zpixelframe (bdk)
     23Aug2005 : De-projection (Az,El) rather than (RA,Dec), and leave
                 final SkyFrame describing (Az,El). Also, replace supplied 
                 (ra,dec) parameters by (az,el,tai). (dsb)
     07Nov2005 : Remove duplicate elevation argument (timj)
                 Remove unused parallactic angle argument (timj)
     08Nov2005 : Use RAD2DEG global rather than 90*PIBY2 (timj)
     27Jan2006 : Use ErsRep rather than printf (timj)
     28Jan2006 : Annul all the mappings/frames to fix terrible leak (timj)
     07Feb2006 : Cache Mappings which do not depend on "az", "el" or "tai".
                 Do not create extra Frames unless specifically requested.
                 Use AST contexts for handling the annulling of AST objects.
                 Avoid use of FitsChans for extra speed. (dsb)
     17Feb2006 : Combine Nasmyth->Cassegrain rotation with the rotations
                    performed after the tangent plane->spherical projection.
                 Cache the SkyFrame used to represent AzEl coords.
                 Cache the FrameSets returned for each subnum
                 Do away with the facility for including intermediate
                    Frames (dsb)
     1Mar2006  : Check for (subnum==-1) before checking inherited status (dsb)
*/

{
   AstMapping *azelmap;
   AstMapping *mapping;
   AstMatrixMap *flipmap;
   AstMatrixMap *revmap;
   AstMatrixMap *rotmap;
   AstPolyMap *polymap;
   AstShiftMap *shiftmap;
   AstZoomMap *radmap;
   AstZoomMap *zoommap;
   AstShiftMap *zshiftmap;

   double a;                       /* subarray angle */
   double rotangle[8] =
      { 0.0, PIBY2, 2*PIBY2, 3*PIBY2, 0.0, PIBY2, 2*PIBY2, 3*PIBY2 };
   double rot[4];
   double reverse[8] =
      { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0 };
   double rev[4];
   double shift[2];
   double zshift[2];

/* xoff and yoff are the distance in pixel units from the tracking centre
   to the [0][0] pixel in a subarray */

   double xoff[8] =
      { -41.5,  33.5,  41.5, -33.5, -41.5,  33.5,  41.5, -33.5 };
   double yoff[8] =
      {  33.5,  41.5, -33.5, -41.5,  33.5,  41.5, -33.5, -41.5 };

   double flip[8] =
      { 1.0, 1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0 };


/* Distortion mappings. X and Y are in the distorted image, x and y are
   undistorted (Nasmyth). All units are mm.

   Least-squares fit to ray tracing provided by Tully gives

   x = 0.30037 + 0.99216 * X - 1.4428e-4 * X^2 - 3.612e-6 * X^3
       + 9.6e-4 * Y - 8.628e-6 * Y^2 - 2.986e-7 * Y^3

   y = 0.6518 + 1.0488e-3 * X - 6.58e-5 * X^2 -1.63e-7 * X^3
       + 0.98645 * Y - 2.218e-4 * Y^2 - 1.082e-6 * Y^3

   X = -0.30461 + 1.0079 * x + 1.502e-4 * x^2 + 3.757e-6 * x^3
       - 9.73e-4 * y + 9.79e-6 * y^2 + 3.135e-7 * y^3

   Y = -0.662328 - 1.082e-3 * x + 6.956e-5 * x^2 + 1.676e-7 * x^3
       + 1.0135 * y + 2.321e-4 * y^2 + 1.221e-6 * y^3

 */

/* Forward coefficients are from FRAME850 to Nasmyth */
   double coeff_f[] = 
               { 
/* x-coordinate */
               0.30037,     1.0, 0.0, 0.0,
               0.99216,     1.0, 1.0, 0.0,
               -1.4428e-4,  1.0, 2.0, 0.0,
               -3.612e-6,   1.0, 3.0, 0.0,
               9.6e-4,      1.0, 0.0, 1.0,
               -8.628e-6,   1.0, 0.0, 2.0,
               -2.986e-7,   1.0, 0.0, 3.0,
/* y-coordinate */
               0.6518,      2.0, 0.0, 0.0,
               1.0488e-3,   2.0, 1.0, 0.0,
               -6.58e-5,    2.0, 2.0, 0.0,
               -1.63e-7,    2.0, 3.0, 0.0,
               0.98645,     2.0, 0.0, 1.0,
               -2.218e-4,   2.0, 0.0, 2.0,
               -1.082e-6,   2.0, 0.0, 3.0
               };

/* Inverse coefficients are from Nasmyth to FRAME850 */

   double coeff_i[] = 
               { 
/* X-coordinate */
               -0.30461,    1.0, 0.0, 0.0,
               1.0079,      1.0, 1.0, 0.0,
               1.502e-4,    1.0, 2.0, 0.0,
               3.757e-6,    1.0, 3.0, 0.0,
               -9.73e-4,    1.0, 0.0, 1.0,
               9.79e-6,     1.0, 0.0, 2.0,
               3.135e-7,    1.0, 0.0, 3.0,
/* Y-coordinate */
               -0.662328,   2.0, 0.0, 0.0,
               -1.082e-3,   2.0, 1.0, 0.0,
               6.956e-5,    2.0, 2.0, 0.0,
               1.676e-7,    2.0, 3.0, 0.0,
               1.0135,      2.0, 0.0, 1.0,
               2.321e-4,    2.0, 0.0, 2.0,
               1.221e-6,    2.0, 0.0, 3.0
               };


/* A cache containing, for each sub-array, a FrameSet and a Mapping. The 
   FrameSet will contai a single Frame representing GRID coords in the 
   sub-array. The result of applying the Mapping to this Frame will be 
   Cartesian (i.e. in the tangent plane) Nasmyth coords in rads. The 
   AST pointers in this cache are exempted from AST context handling, and
   so need to be released explicitly using astAnnul. This is done by 
   calling this function with the sub-frame number set to -1. */
   static AstMapping *map_cache[ 8 ] = { NULL, NULL, NULL, NULL, NULL, NULL, 
                                         NULL, NULL };
   static AstFrameSet *frameset_cache[ 8 ] = { NULL, NULL, NULL, NULL, NULL, NULL, 
                                               NULL, NULL };

/* Cache the SkyFrame used to represent final spherical (Az,El) coords */
   static AstSkyFrame *skyframe = NULL;

/* Cache used to hold Mappings needed in the tangent plane to celestial
   longitude,latitude Mapping. */
   static AstMapping *azel_cache[ 2 ] = { NULL, NULL };





/* Check the sub-array number. If it is -1, free the cached AST objects and 
   return. Otherwise, report an error if the value is illegal. We do
   this before checking the inherited status so that the memory is freed
   even if an error has occurred. */
   if( subnum == -1 ) {

      for( subnum = 0; subnum < 8; subnum++ ) {
         if( map_cache[ subnum ] ) {
            map_cache[ subnum ] = astAnnul( map_cache[ subnum ] );
         }
         if( frameset_cache[ subnum ] ) {
            frameset_cache[ subnum ] = astAnnul( frameset_cache[ subnum ] );
         }
      }

      if( azel_cache[ 0 ] ) azel_cache[ 0 ] = astAnnul( azel_cache[ 0 ] );
      if( azel_cache[ 1 ] ) azel_cache[ 1 ] = astAnnul( azel_cache[ 1 ] );
      if( skyframe ) skyframe = astAnnul( skyframe );

      return;

   } else if ( subnum < 0 || subnum > 7 ) {
     *status = SAI__ERROR;
     sprintf( errmess, "Sub array number '%d' out of range\n", subnum );
     ErsRep( 0, status, errmess );
     return;

   }



/* Now initialise the returned pointer and check the inherited status */
   *fset = AST__NULL;
   if ( *status != SAI__OK ) return;

/* Start an AST context. This means we do not need to worry about
   annulling AST objects. Note, there should be no "return" statements
   before the matching call to astEnd. */
   astBegin;

/* The Mapping from GRID coords to AzEl coords can be thought of as divided 
   into two parts; the early part which goes from GRID to Cartesian Nasmyth 
   coords, and the later part which goes from Cartesian Nasmyth to spherical 
   AzEl coords. The nature of the early part is fixed for each subarray and 
   does not depend on any of the supplied parameters (other than sub-array 
   number). Therefore we can create the early part once for each sub-array 
   and cache them for later use. The later part depends on the "az", "el" 
   and "tai" parameters and so cannot be cached. Create the early part of 
   the required Mapping for the requested sub-array if it has not already 
   been created. The cached Mapping transforms positions within the Frame
   encapsulated within the cached FrameSet into Cartesian Nasmyth coords in
   radians. */
   if( !map_cache[ subnum ] ) {

/* Create an AST frame describing GRID coordinates within the subarray
   and put it into the cached FrameSet for this subarray. The centre of
   the first pixel has coords (1.0,1.0) in the GRID Frame. */
      frameset_cache[ subnum ] = astFrameSet( astFrame ( 2, "Domain=GRID" ), 
                                              "" );   

/* We add a dummy 2D Frame to the FrameSet so that there is a Frame to
   remove on the first call to astRemoveFrame below. */
      astAddFrame( frameset_cache[ subnum ], AST__BASE, 
                   astUnitMap( 2, "" ), astFrame( 2, "" ) );

/* The GRID domain locates the [0][0] pixel at coordinates (1,1). Shift
   these so that the [0][0] pixel is at the origin of a coordinate system */
      zshift[0] = -1.0;
      zshift[1] = -1.0;
      zshiftmap = astShiftMap ( 2, zshift, "" );
      map_cache[ subnum ] = (AstMapping *) zshiftmap;

/* The mapping from pixel numbers to millimetres is a simple scaling,
   because the pixel separation is the same in both coordinates and is
   accurately constant. A ZoomMap can be used for this. */
      zoommap = astZoomMap ( 2, PIX2MM, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      zoommap, 1, "" );

/* The mm coords now have to be rotated through an angle approximating
   a multiple of 90 degrees */
      a = rotangle[ subnum ];
      rot[ 0 ] = cos( a );
      rot[ 1 ] = -sin( a );
      rot[ 2 ] = sin( a );
      rot[ 3 ] = cos( a );
      rotmap = astMatrixMap ( 2, 2, 0, rot, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      rotmap, 1, "" );

/* The Y coordinate now has to be reversed */
      rev[ 0 ] = 1;
      rev[ 1 ] = 0;
      rev[ 2 ] = 0;
      rev[ 3 ] = reverse[ subnum ];
      revmap = astMatrixMap ( 2, 2, 0, rev, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      revmap, 1, "" );

/* For each 450/850 subarray, the next Mapping creates FRAME450/FRAME850
   coordinates, which are coordinates in millimetres with origin at the
   optical axis. For a 450 subarray the axes are chosen such that the
   first axis maps onto Frame850 North and the second onto the inverted
   Focus850 UP once the dichroic reflection is taken into account. */
      shift[ 0 ] = xoff[ subnum ] * PIX2MM;
      shift[ 1 ] = yoff[ subnum ] * PIX2MM;
      shiftmap = astShiftMap( 2, shift, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      shiftmap, 1, "" );

/* The final step into Frame850 coordinates is only needed for the 450
   subarrays. */
      rev[ 0 ] = 1;
      rev[ 1 ] = 0;
      rev[ 2 ] = 0;
      rev[ 3 ] = flip[ subnum ];
      flipmap = astMatrixMap( 2, 2, 0, rev, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      flipmap, 1, "" );

/* Correct for polynomial distortion */
      polymap = astPolyMap( 2, 2, 14, coeff_f, 14, coeff_i, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      polymap, 1, "" );

/* Convert from mm to radians (but these coords are still cartesian (x,y)
   (i.e. measured in the tangent plane) rather than spherical (lon,lat)
   measured on the sky. */
      radmap = astZoomMap ( 2, MM2RAD, "" );
      map_cache[ subnum ] = (AstMapping *) astCmpMap( map_cache[ subnum ], 
                                                      radmap, 1, "" );

/* Simplify the Cached Mapping. */
      map_cache[ subnum ] = astSimplify( map_cache[ subnum ] );

/* Exempt the cached AST objects from AST context handling. This means
   that the pointers will not be annulled as a result of calling astEnd. 
   Therefore the objects need to be annulled explicitly when no longer
   needed. this is done by calling this function with "subnum" set to -1. */
      astExempt( map_cache[ subnum ] );
      astExempt( frameset_cache[ subnum ] );
   }

/* Create a Mapping from these Cartesian Nasmyth coords (in rads) to spherical 
   AzEl coords (in rads). */
   azelmap = sc2ast_maketanmap( az, el, azel_cache, status );

/* Combine this with the cached Mapping (from GRID coords for the subarray to
   Cartesian Nasmyth coords in rads), to get the total Mapping from GRID 
   coords to spherical AzEl in rads. */
   mapping = (AstMapping *) astCmpMap( map_cache[ subnum ], azelmap, 1, "" );

/* If not already created, create a SkyFrame describing (Az,El). Hard-wire 
   the geodetic longitude and latitude of JCMT into this Frame. Note, the 
   Epoch value should be TDB, but we supply TT (=TAI+32.184 sec) instead 
   since the difference is only 1-2 milliseconds. We cache the created 
   SkyFrame to avoid the overhead of constantly re-creating it. The Epoch 
   is set every time though since this will vary from call to call. */
   if( !skyframe ) {
      skyframe = astSkyFrame ( "system=AzEl" );
      astSetC( skyframe, "ObsLon", JCMT_LON );
      astSetC( skyframe, "ObsLat", JCMT_LAT );   
      astExempt( skyframe );
   }
   astSet( skyframe, "Epoch=MJD %.*g", DBL_DIG, tai + 32.184/SPD );

/* Now modify the cached FrameSet to use the new Mapping and SkyFrame.
   First remove the existing current Frame and then add in the new one.
   Note, we add a copy of the SkyFrame rather than the cached SkyFrame 
   itself since the SkyFrame contained in the FrameSet will be modified 
   by later functions.  */
   astRemoveFrame( frameset_cache[ subnum ], AST__CURRENT );
   astAddFrame( frameset_cache[ subnum ], AST__BASE, mapping, 
                astCopy( skyframe ) );

/* Return the final FrameSet. */
   *fset = astClone( frameset_cache[ subnum ] );

/* Export the returned FrameSet pointer, and then end the AST context. This 
   will annul all AST objects created since the matching call to astBegin,
   except for those which have been exported using astExport or exempted
   using astExempt. The use of AST contexts requires that the function
   does not exit prematurely before reaching the astEnd call. Therefore 
   there should usually no "return" statements within the body of the AST
   context. */
   astExport( *fset );
   astEnd;
}





/*+ sc2ast_getdomain - select a domain within a frameset */

void sc2ast_getdomain
(
char *name,               /* AST domain name (given) */
AstFrameSet *fset,        /* modified frameset (given and returned) */
int *status               /* global status (given and returned) */
)
/* Method :
    Find the last frame within the frameset matching the given domain
    name, and make it the current frame.
   Authors :
     B.D.Kelly (bdk@roe.ac.uk)
   History :
     10May2005 : original (bdk)
*/

{
   AstFrame *frame;    /* individual frame */
   int frameno;        /* number of frame */
   int nframes;        /* number of frames in the frameset */
   int j;              /* loop counter */

   if ( *status != SAI__OK ) return;

   astBegin;

/* Search for the domain name */

   nframes = astGetI ( fset, "NFRAME" );

   frameno = 0;
   for ( j=nframes; j>0; j-- )
   {
      frame = astGetFrame ( fset, j );
      if ( strcmp ( name, astGetC ( frame, "DOMAIN" )) == 0 )
      {
         frameno = j;
         break;
      }
      astAnnul ( frame );
   }

   astEnd;

   if ( frameno != 0  )
   {
      astSetI ( fset, "CURRENT", frameno );      
   }
}


/*+ sc2ast_makefitschan - create a set of FITS headers in a FitsChan */

void sc2ast_makefitschan
(
double crpix1,            /* index of reference point (given) */
double crpix2,            /* index of reference point (given) */
double cd1_1,             /* data increment (given) */
double cd2_2,             /* data increment (given) */
double crval1,            /* reference coordinate (given) */
double crval2,            /* reference coordinate (given) */
char *ctype1,             /* coordinate mapping type (given) */
char *ctype2,             /* coordinate mapping type (given) */
AstFitsChan *fitschan,   /* FitsChan to be filled (given and returned) */
int *status               /* global status (given and returned) */
)
/* Method :
     Contruct a set of FITS headers and put them into an AST FitsChan.
   Authors :
     B.D.Kelly (bdk@roe.ac.uk)
     T.Jenness (timj@jach)
   History :
     13Apr2005 : original (bdk)
     08Nov2005 : Use astSetFits rather than intermediate strings (timj)
*/

{

   if ( *status != SAI__OK ) return;

   astSetFitsF ( fitschan, "CRPIX1", crpix1, " ", 0 );
   astSetFitsF ( fitschan, "CRPIX2", crpix2, " ", 0 );
   astSetFitsF ( fitschan, "CD1_1", cd1_1, " ", 0 );
   astSetFitsF ( fitschan, "CD2_2", cd2_2, " ", 0 );
   astSetFitsF ( fitschan, "CRVAL1", crval1, " ", 0 );
   astSetFitsF ( fitschan, "CRVAL2", crval2, " ", 0 );
   astSetFitsS ( fitschan, "CTYPE1", ctype1, " ", 0 );
   astSetFitsS ( fitschan, "CTYPE2", ctype2, " ", 0 );

   return;
}



/*+ sc2ast_moveframe - move the base frame within a frameset */

void sc2ast_moveframe
(
double x,                 /* X coordinate offset in pixels (given) */
double y,                 /* Y coordinate offset in pixels (given) */
AstFrameSet *fset,        /* modified frameset (given and returned) */
int *status               /* global status (given and returned) */
)
/* Method :
    Use astRemapFrame to shift the base frame of the frameset.
   Authors :
     B.D.Kelly (bdk@roe.ac.uk)
   History :
     26May2005 : original (bdk)
     07Jun2005 : apply shift in pixels (bdk)
*/

{
   AstShiftMap *shiftmap;
   double shift[2];

   if ( *status != SAI__OK ) return;

   astBegin;

/* Set up a shift mapping */

   shift[0] = x;
   shift[1] = y;
   shiftmap = astShiftMap ( 2, shift, "" );

/* Remap the base frame */

   astRemapFrame ( fset, AST__BASE, shiftmap );

   astEnd;

}


/*+ sc2ast_name2num - convert subarray name to id number */

void sc2ast_name2num
(
char *name,             /* subarray name s8a-d, s4a-d (given) */
int *subnum,            /* subarray number, 0-7 (returned) */
int *status             /* global status (given and returned) */
)
/* Method :
    Convert the SCUBA-2 standard name of a subarray into its
    corresponding index number.
   Authors :
     B.D.Kelly (bdk@roe.ac.uk)
     Tim Jenness (timj@jach.hawaii.edu)
   History :
     27Apr2005 : original (bdk)
     18May2005 : force initialisation of returned value (timj)
     27Jan2006 : use strncmp rather than strcmp (timj)
                 Associate error message with error condition (timj)
*/

{

   *subnum = 0; /* force initialisation */
 
   if ( *status != SAI__OK ) return;


   if ( strncmp ( name, "s8a", 3 ) == 0 )
   {
      *subnum = 0;
   }
   else if ( strncmp ( name, "s8b", 3 ) == 0 )
   {
      *subnum = 1;
   }
   else if ( strncmp ( name, "s8c", 3 ) == 0 )
   {
      *subnum = 2;
   }
   else if ( strncmp ( name, "s8d", 3 ) == 0 )
   {
      *subnum = 3;
   }
   else if ( strncmp ( name, "s4a", 3 ) == 0 )
   {
      *subnum = 4;
   }
   else if ( strncmp ( name, "s4b", 3 ) == 0 )
   {
      *subnum = 5;
   }
   else if ( strncmp ( name, "s4c", 3 ) == 0 )
   {
      *subnum = 6;
   }
   else if ( strncmp ( name, "s4d", 3 ) == 0 )
   {
      *subnum = 7;
   }
   else
   {
      *status = SAI__ERROR;
      sprintf( errmess,
	       "Error converting subarray name from '%s' to number",
	       name);
      ErsRep( 0, status, errmess);
   }
}



/*+ sc2ast_polframest - create a frameset for polarimetry */

void sc2ast_polframeset
(
AstFrameSet *frameset,  /* 2-D frameset (given) */
AstFrameSet **fset,     /* constructed 3-D frameset (returned) */
int *status             /* global status (given and returned) */
)
/* Method :
    Add a unit mapping onto an existing 2-D frameset to create a 3-D
    frameset.
   Authors :
     B.D.Kelly (bdk@roe.ac.uk)
   History :
     14Jun2005 : original (bdk)
*/

{
   AstCmpMap *cmpmap;
   AstFrame *extframe;
   AstFrame *gridframe;
   AstCmpFrame *polframe;
   AstSkyFrame *skyframe;
   AstUnitMap *unitmap;

 
   if ( *status != SAI__OK ) return;


/* The first step creates an AST frame corresponding to the subarray */

   gridframe = astFrame ( 3, "Domain=GRID" );
   extframe = astFrame ( 1, "Domain=GRID" );

/* An AST FrameSet is initialised and will be built up as the various frames
   and the mappings between them are created */

   *fset = astFrameSet ( gridframe, "" );

   unitmap = astUnitMap ( 1, " " );
   cmpmap = astCmpMap ( frameset, unitmap, 0, " " );
   skyframe = astSkyFrame ( " " );
   polframe = astCmpFrame ( skyframe, extframe, " " );
   astAddFrame ( *fset, AST__CURRENT, cmpmap, polframe );

}

/*+ sc2ast_kludgemodjuldate - kludge for the modified julian date */

double sc2ast_kludgemodjuldate
( 
double ra,           /* Right Ascension in radians (given) */
int *status          /* global status (given and returned) */
)

/* Description :
    Until the simulator runs on a single clock, this kludge will calculate
    a modified julian date that is consistent with the astronomical source
    being on the meridian (i.e. lst=ra as it is assumed everywhere in the
    simulator). This routine is needed to calculate the time fields
    for TAI/RTS.

   Authors :
    Ed Chapin (echapin@phas.ubc.ca)

   History :
    05Dec2005 : Original version (echapin)
*/

{
  double date;          /* ref. modified julian date */
  double delta_st;      /* How much does reflst have to change? radians */
  double lon;           /* JCMT west longitude in radians */
  double obsdate;       /* The modified julian date of the observation */
  double refgst;        /* ref. gst in radians */
  double reflst;        /* ref. lst in radians */
  double t;             /* Julian centuries since J2000 */
  double ut;            /* ref. universal time as day fraction */


  /*if ( !StatusOkP(status) ) return; */

  /* JCMT coordinates in radians */

  /*lat = ( 19.0 + (49.0/60.0) + (33.0/3600.0) ) / RAD2DEG;*/
  lon = ( 155.0 + (28.0/60.0) + (0.0/3600.0) ) / RAD2DEG;

  /* Define an arbitrary modified julian date + UT time */

  date = 51544;        /* Jan 1, 2000 */
  ut = 0.5;            /* noon */

  /* Use slalib slaGmsta algorithm to calculate greenwich LST at this date */

  t = ( ut + ( date - 51544.5 ) ) / 36525.0;

  refgst = DS2R * ( 24110.54841
                           + ( 8640184.812866
                           + ( 0.093104
                             - 6.2e-6 * t ) * t ) * t
                             + 86400.0 * ( dmod ( ut, 1.0 ) +
                                           dmod ( date, 1.0 ) ) ); 

  /* Calculate the reference LST from the GST using the telescope longitude */

  reflst = refgst - lon;

  /* If we want ra = lst, how far do we have to move in ST?
     i.e. what is the hour angle for the reference lst */
 
  delta_st = ra - reflst;

  /* Calculate the obsdate. Note that delta_st is multiplied by
     the ratio of a civil time interval to a sidereal time interval */

  obsdate = date + ut + delta_st/(2.*PI*1.002737909);

  return obsdate;
}

/*+ sc2ast_telpos - get telescope position and orientation */

void sc2ast_telpos
( 
double ra,           /* Right Ascension in radians (given) */
double dec,          /* Declination in radians (given) */
double lst,          /* local sidereal time in radians (given) */
double *az,          /* Azimuth in radians (returned) */
double *el,          /* Elevation in radians (returned) */
double *p            /* Parallactic angle in radians (returned) */
)

/* Description :
    Copy of dsim_telpos to avoid a libdsim dependency
    Use slalib algorithms to get from equatorial to horizontal coordinates.

   Authors :
    Ed Chapin (echapin@phas.ubc.ca)
    B.D.Kelly (bdk@roe.ac.uk)

   History :
    12May2005 : Original version (bdk)
    05Dec2005 : Copied into sc2ast.c (EC)
*/

{
   double cosp;      /* intermediate result */
   double phi;       /* latitude of telescope in radians */
   double ha;        /* hour angle in radians */
   double r;         /* intermediate result */
   double sinp;      /* intermediate result */
   double x;         /* cartesian coordinates */
   double y;         /* cartesian coordinates */
   double z;         /* cartesian coordinates */

/* JCMT is 19:49:33 N */

   phi = ( 19.0 + (49.0/60.0) + (33.0/3600.0) ) / RAD2DEG;
   ha = lst - ra;

/*
   Equivalent to slaDe2h ( ha, dec, phi, az, el );
*/

/* Az,El as x,y,z */

   x = - cos(ha) * cos(dec) * sin(phi) + sin(dec) * cos(phi);
   y = - sin(ha) * cos(dec);
   z = cos(ha) * cos(dec) * cos(phi) + sin(dec) * sin(phi);

/* To spherical */

   r = sqrt ( x*x + y*y );

   if ( r < 1.0e-20 )
   {
      *az = 0.0;
   }
   else
   {
      *az = atan2 ( y, x );
   }

   if ( *az < 0.0 )
   {
      *az += 2.0 * PI;
   }

   *el = atan2 ( z, r );

/*
   *p = slaPa ( ha, dec, phi ); 
*/
   sinp = cos ( phi ) * sin ( ha );
   cosp = sin ( phi ) * cos ( dec) - cos ( phi ) * sin ( dec) * cos ( ha );

   if ( sinp != 0.0 || cosp != 0.0 )
   {
      *p = atan2 ( sinp, cosp );
   }
   else
   {
      *p = 0.0;
   }

}

/*+ sc2ast_createwcs_compat - create WCS descriptionusing old parameters */

void sc2ast_createwcs_compat
(
int subnum,             /* subarray number, 0-7 (given) */
double ra,              /* Right Ascension of the telescope boresight */
double dec,             /* Declination of the telescope boresight */
double el,              /* Boresight elevation in radians (given) */
double p,               /* No longer used (pass any dummy value) */
AstFrameSet **fset,     /* constructed frameset (returned) */
int *status             /* global status (given and returned) */
)
/* Description : 
     This is a temporary routine meant to replace calls to
     sc2ast_createwcs that were written before TimJ changed the
     API. It is simply a wrapper for sc2ast_createwcs which generates
     an appropriate Modified Julian date so that LST=RA for the
     calculation of TAI using the old parameters for sc2ast_createwcs
     Authors : 
      Ed Chapin (echapin@phas.ubc.ca) 
     History : 
      05Dec2005 : original (EC)
*/

{
  double tai;
  double temp_az, temp_el, temp_lst, temp_p;

  temp_lst = ra;
  sc2ast_telpos ( ra, dec, temp_lst, &temp_az, &temp_el, &temp_p );

  tai = sc2ast_kludgemodjuldate( ra, status );
  
  sc2ast_createwcs( subnum, temp_az, temp_el, tai, 0, fset, status );
}



/*+ sc2ast_maketanmap - create a Mapping representing a tangent plane 
                        projection */

AstMapping *sc2ast_maketanmap
(
double lon,               /* Celestial longitude at ref point (rads) */
double lat,               /* Celestial latitude at ref point (rads) */
AstMapping *cache[ 2 ],   /* Cached Mappings (supply as NULL on 1st call) */
int *status               /* global status (given and returned) */
)
/* Method :
    The forward transformation of the returned Mapping transforms 
    cartesian tangent plane offsets in radians, into celestial longitude
    and latitude values, in radians. The reference point of the tangent
    plane is put at the supplied longitude and latitude position. It is
    assumed that the second cartesian input axis is parallel to celestial
    north.

    The "cache" array should be filled with NULL values before the first
    call to this function. It will be returned holding AST pointers to
    Mappings which will be needed on subsequent calls (these pointers are
    exempted from AST context handling).

   Authors :
     D.S.Berry (dsb@ast.man.ac.uk)

   History :
     10Feb2006 : original (dsb)
*/

{
   AstMapping *result;    
   AstMatrixMap *matmap;
   AstCmpMap *m1;
   AstWcsMap *wcsmap;
   double t1, t2, t3, ct, cn, st, sn, mat[ 9 ];

/* Check the inherited status. */
   if ( *status != SAI__OK ) return NULL;

/* If required, create a SphMap for converting spherical cartesian
   (x,y,z) positions to (lon,lat) positions. */
   if( !cache[ 0 ] ) {
      cache[ 0 ] = (AstMapping *) astSphMap( "" );
      astExempt( cache[ 0 ] );
   }

/* If required, create a CmpMap holding a WcsMap followed by an inverted
   SphMap. */
   if( !cache[ 1 ] ) {
      wcsmap = astWcsMap( 2, AST__TAN, 1, 2, "" );
      astInvert( wcsmap );
      astInvert( cache[ 0 ] );
      cache[ 1 ] = (AstMapping *) astCmpMap( wcsmap, cache[ 0 ], 1, "" );
      astInvert( cache[ 0 ] );
      wcsmap = astAnnul( wcsmap );
      astExempt( cache[ 1 ] );
   }

/* The required Mapping consists of the "cache[ 1 ]" Mapping, followed by
   a suitable MatrixMap which rotates the tangent point to the requested
   spherical (az,el) coordinates, and rotates the Nasmyth Y axis to 
   celestial north (elevation). followed by the "cache[ 0 ]" Mapping. The
   logic follows that of FITS-WCS paper II (which is what the WcsMap
   class assumes). The reference point of the TAN projection is at the 
   north pole of the "native" spherical coordinate system. The matrix map
   first rotates about the Z axis by -lat (to rotate the Nasmyth Y axis to
   north), then rotates about the Y axis by (pi/2-lat) (to rotate the
   reference point from the native north pole to the celestial north pole), 
   then rotates about the Z axis by -lon (to rotate the prime native meridian 
   to the prime celestial meridian). Here the Z axis points to the north
   pole, the X axis points to (lon,lat)=(0,0) and the Y axis points to 
   (lon,lat) = (90 degs,0) (the slalib convention). */

   ct = cos( lat );
   st = sin( lat );
   cn = cos( lon );
   sn = sin( lon );

   t1 = cn*st;
   t2 = sn*ct;
   t3 = ct*cn;

   mat[ 0 ] = t1*ct;
   mat[ 1 ] = -t1*st - t2;
   mat[ 2 ] = t3;
   mat[ 3 ] = st*t2 + t1;
   mat[ 4 ] = -sn*st*st + t3;
   mat[ 5 ] = t2;
   mat[ 6 ] = -ct*ct;
   mat[ 7 ] = st*ct;
   mat[ 8 ] = st;

   matmap = astMatrixMap( 3, 3, 0, mat, "" );

/* Create the required compound Mapping. */
   m1 = astCmpMap( cache[ 1 ], matmap, 1, "" );
   result = (AstMapping *) astCmpMap( m1, cache[ 0 ], 1, "" );
   matmap = astAnnul( matmap );
   m1 = astAnnul( m1 );

/* Return the required Mapping.*/
   return result;

}
