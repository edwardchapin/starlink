/*
*class++
*  Name:
*     DSBSpecFrame

*  Purpose:
*     Dual sideband spectral coordinate system description.

*  Constructor Function:
c     astDSBSpecFrame
f     AST_DSBSPECFRAME

*  Description:
*     A DSBSpecFrame is a specialised form of SpecFrame which represents 
*     positions in a spectrum obtained using a dual sideband instrument.
*     Such an instrument produces a spectrum in which each point contains 
*     contributions from two distinctly different frequencies, one from
*     the "lower side band" (LSB) and one from the "upper side band" (USB). 
*     Corresponding LSB and USB frequencies are connected by the fact
*     that they are an equal distance on either side of a fixed central
*     frequency known as the "Local Oscillator" (LO) frequency.
*
*     When quoting a position within such a spectrum, it is necessary to
*     indicate whether the quoted position is the USB position or the 
*     corresponding LSB position. The SideBand attribute provides this
*     indication.
*
*     In practice, the LO frequency is specified by giving the distance
*     from the LO frequency to some "central" spectral position. Typically 
*     this central position is that of some interesting spectral feature. 
*     The distance from this central position to the LO frequency is known 
*     as the "intermediate frequency" (IF). The value supplied for IF can
*     be a signed value in order to indicate whether the LO frequency is
*     above of below the central position.

*  Inheritance:
*     The DSBSpecFrame class inherits from the SpecFrame class.

*  Attributes:
*     In addition to those attributes common to all SpecFrames, every
*     DSBSpecFrame also has the following attributes:
*
*     - DSBCentre: The central position of interest.
*     - IF: The intermediate frequency used to define the LO frequency.
*     - SideBand: Indicates which sideband the DSBSpecFrame represents.
*     - ImagFreq: The image sideband equivalent of the rest frequency.

*  Functions:
c     The DSBSpecFrame class does not define any new functions beyond those
f     The DSBSpecFrame class does not define any new routines beyond those
*     which are applicable to all SpecFrames.

*  Copyright:
*     <COPYRIGHT_STATEMENT>

*  Authors:
*     DSB: David Berry (Starlink)

*  History:
*     5-AUG-2004 (DSB):
*        Original version.
*     7-OCT-2004 (DSB):
*        Fixed SetAttrib code which assigns values to SideBand. Previously 
*        all supplied values were ignored, leaving SideBand unchanged.

*class--

*  Implementation Deficiencies:
*     - The default values for System and StdOfRest inherited from the
*     SpecFrame class are "Wave" and "Heliocentric". These are not
*     usually what is wanted for a DSB instrument. Defaults such as
*     "Freq" and "Topo" may be more appropriate. However, changing the
*     defaults inherited from SpecFrame may cause problems in the
*     astConvert algorithm. The astConvertX function in frame.c includes
*     the following implementation deficiency warning: "One likely
*     problem is with attributes which default in both the source and
*     destination Frames. This means they also default in the common
*     coordinate system. If these default values were to differ when
*     matching different target Frames, however, we would be in trouble,
*     because the common coordinate system would not then be remaining
*     constant. The longer-term solution to this is probably to provide
*     some mechanism to "fix" all attribute values for a Frame, by taking
*     any attributes that are un-set and explicitly setting a firm value
*     (equal to the default) so they cannot then change". So the defaults
*     should probably be left unchanged until this fix is made.

*/

/* Module Macros. */
/* ============== */
/* Set the name of the class we are implementing. This indicates to
   the header files that define class interfaces that they should make
   "protected" symbols available. */
#define astCLASS DSBSpecFrame

#define USB 1
#define LSB 0

/* Include files. */
/* ============== */
/* Interface definitions. */
/* ---------------------- */
#include "error.h"               /* Error reporting facilities */
#include "memory.h"              /* Memory management facilities */
#include "object.h"              /* Base Object class */
#include "channel.h"             /* I/O channels */
#include "specframe.h"           /* Spectral frames (parent class) */
#include "unit.h"                /* Unit handling */
#include "cmpmap.h"              /* Compound Mappings */
#include "unitmap.h"             /* Unit Mappings */
#include "winmap.h"              /* Window Mappings */
#include "dsbspecframe.h"        /* Interface definition for this class */

/* Error code definitions. */
/* ----------------------- */
#include "ast_err.h"             /* AST error codes */

/* C header files. */
/* --------------- */
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Module Variables. */
/* ================= */
/* Define the class virtual function table and its initialisation flag
   as static variables. */
static AstDSBSpecFrameVtab class_vtab; /* Virtual function table */
static int class_init = 0;             /* Virtual function table initialised? */

/* Pointers to parent class methods which are extended by this class. */
static const char *(* parent_getattrib)( AstObject *, const char * );
static const char *(* parent_getlabel)( AstFrame *, int );
static int (* parent_match)( AstFrame *, AstFrame *, int **, int **, AstMapping **, AstFrame ** );
static int (* parent_subframe)( AstFrame *, AstFrame *, int, const int *, const int *, AstMapping **, AstFrame ** );
static int (* parent_testattrib)( AstObject *, const char * );
static void (* parent_clearattrib)( AstObject *, const char * );
static void (* parent_setattrib)( AstObject *, const char * );
static void (* parent_overlay)( AstFrame *, const int *, AstFrame * );

/* External Interface Function Prototypes. */
/* ======================================= */
/* The following functions have public prototypes only (i.e. no
   protected prototypes), so we must provide local prototypes for use
   within this module. */
AstDSBSpecFrame *astDSBSpecFrameId_( const char *, ... );

/* Prototypes for Private Member Functions. */
/* ======================================== */

static AstMapping *TopoMap( AstDSBSpecFrame *, int, const char * );
static AstMapping *ToLSBMapping( AstDSBSpecFrame *, const char * );
static AstMapping *ToUSBMapping( AstDSBSpecFrame *, const char * );
static const char *GetAttrib( AstObject *, const char * );
static const char *GetLabel( AstFrame *, int );
static double GetImagFreq( AstDSBSpecFrame * );
static int Match( AstFrame *, AstFrame *, int **, int **, AstMapping **, AstFrame ** );
static int SubFrame( AstFrame *, AstFrame *, int, const int *, const int *, AstMapping **, AstFrame ** );
static int TestAttrib( AstObject *, const char * );
static void ClearAttrib( AstObject *, const char * );
static void Dump( AstObject *, AstChannel * );
static void Overlay( AstFrame *, const int *, AstFrame * );
static void SetAttrib( AstObject *, const char * );

static double GetIF( AstDSBSpecFrame * );
static int TestIF( AstDSBSpecFrame * );
static void ClearIF( AstDSBSpecFrame * );
static void SetIF( AstDSBSpecFrame *, double );

static double GetDSBCentre( AstDSBSpecFrame * );
static int TestDSBCentre( AstDSBSpecFrame * );
static void ClearDSBCentre( AstDSBSpecFrame * );
static void SetDSBCentre( AstDSBSpecFrame *, double );

static int GetSideBand( AstDSBSpecFrame * );
static int TestSideBand( AstDSBSpecFrame * );
static void ClearSideBand( AstDSBSpecFrame * );
static void SetSideBand( AstDSBSpecFrame *, int );


/* Member functions. */
/* ================= */
static void ClearAttrib( AstObject *this_object, const char *attrib ) {
/*
*  Name:
*     ClearAttrib

*  Purpose:
*     Clear an attribute value for a DSBSpecFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     void ClearAttrib( AstObject *this, const char *attrib )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the astClearAttrib protected
*     method inherited from the SpecFrame class).

*  Description:
*     This function clears the value of a specified attribute for a
*     DSBSpecFrame, so that the default value will subsequently be used.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     attrib
*        Pointer to a null-terminated string specifying the attribute
*        name.  This should be in lower case with no surrounding white
*        space.
*/

/* Local Variables: */
   AstDSBSpecFrame *this;        /* Pointer to the DSBSpecFrame structure */
   
/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the DSBSpecFrame structure. */
   this = (AstDSBSpecFrame *) this_object;

/* Check the attribute name and clear the appropriate attribute. */

/* DSBCentre. */
/* ---------- */
   if ( !strcmp( attrib, "dsbcentre" ) ) {
      astClearDSBCentre( this );

/* IF */
/* -- */
   } else if ( !strcmp( attrib, "if" ) ) {
      astClearIF( this );

/* SideBand */
/* -------- */
   } else if ( !strcmp( attrib, "sideband" ) ) {
      astClearSideBand( this );

/* Read-only attributes. */
/* --------------------- */
/* Test if the attribute name matches any of the read-only attributes
   of this class. If it does, then report an error. */
   } else if ( !strcmp( attrib, "imagfreq" ) ) {
      astError( AST__NOWRT, "astClear: Invalid attempt to clear the \"%s\" "
                "value for a %s.", attrib, astGetClass( this ) );
      astError( AST__NOWRT, "This is a read-only attribute." );

/* If the attribute is not recognised, pass it on to the parent method
   for further interpretation. */
   } else {
      (*parent_clearattrib)( this_object, attrib );
   }
}


static const char *GetAttrib( AstObject *this_object, const char *attrib ) {
/*
*  Name:
*     GetAttrib

*  Purpose:
*     Get the value of a specified attribute for a DSBSpecFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     const char *GetAttrib( AstObject *this, const char *attrib )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the protected astGetAttrib
*     method inherited from the SpecFrame class).

*  Description:
*     This function returns a pointer to the value of a specified
*     attribute for a DSBSpecFrame, formatted as a character string.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     attrib
*        Pointer to a null-terminated string containing the name of
*        the attribute whose value is required. This name should be in
*        lower case, with all white space removed.

*  Returned Value:
*     - Pointer to a null-terminated string containing the attribute
*     value.

*  Notes:
*     - The returned string pointer may point at memory allocated
*     within the DSBSpecFrame, or at static memory. The contents of the
*     string may be over-written or the pointer may become invalid
*     following a further invocation of the same function or any
*     modification of the DSBSpecFrame. A copy of the string should
*     therefore be made if necessary.
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Constants: */
#define BUFF_LEN 50              /* Max. characters in result buffer */

/* Local Variables: */
   AstDSBSpecFrame *this;        /* Pointer to the DSBSpecFrame structure */
   AstMapping *tmap;             /* Ptr to Mapping from topofreq to this */
   const char *result;           /* Pointer value to return */
   double dval;                  /* Attribute value */
   double dtemp;                 /* Attribute value */
   int ival;                     /* Attribute value */
   static char buff[ BUFF_LEN + 1 ]; /* Buffer for string result */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the SpecFrame structure. */
   this = (AstDSBSpecFrame *) this_object;

/* Compare "attrib" with each recognised attribute name in turn,
   obtaining the value of the required attribute. If necessary, write
   the value into "buff" as a null-terminated string in an appropriate
   format.  Set "result" to point at the result string. */

/* DSBCentre */
/* --------- */
   if ( !strcmp( attrib, "dsbcentre" ) ) {

/* Get the value as topocentric frequency in Hz. */
      dval = astGetDSBCentre( this );

/* Find the Mapping from topocentric frequency in Hz to the spectral system 
   described by this SpecFrame. */
      tmap = TopoMap( this, 0, "astGetAttrib" );         
      if ( astOK ) {

/* Transform the internal value from topocentric frequency into the required 
   system. */
         astTran1( tmap, 1, &dval, 1, &dtemp );
         if( dtemp == AST__BAD ) {
            astError( AST__INTER, "astGetAttrib(%s): Cannot convert DSBCentre "
                      "value from topocentric frequency to the required "
                      "system.", astGetClass( this ) );
         } else {

/* Format it. */
            (void) sprintf( buff, "%.*g", DBL_DIG, dtemp );
            result = buff;
         }
         tmap = astAnnul( tmap );
      }

/* IF */
/* -- */
   } else if ( !strcmp( attrib, "if" ) ) {
      dval = astGetIF( this );
      if ( astOK ) {
         (void) sprintf( buff, "%.*g", DBL_DIG, dval*1.0E-9 );
         result = buff;
      }

/* ImagFreq */
/* -------- */
   } else if ( !strcmp( attrib, "imagfreq" ) ) {
      dval = astGetImagFreq( this );
      if ( astOK ) {
         (void) sprintf( buff, "%.*g", DBL_DIG, dval*1.0E-9 );
         result = buff;
      }

/* SideBand */
/* -------- */
   } else if ( !strcmp( attrib, "sideband" ) ) {
      ival = astGetSideBand( this );
      if ( astOK ) {
         result = ( ival == USB ) ? "USB" : "LSB";
      }

/* If the attribute name was not recognised, pass it on to the parent
   method for further interpretation. */
   } else {
      result = (*parent_getattrib)( this_object, attrib );
   }

/* Return the result. */
   return result;

/* Undefine macros local to this function. */
#undef BUFF_LEN
}

static double GetImagFreq( AstDSBSpecFrame *this ) {
/*
*+
*  Name:
*     astGetImagFreq

*  Purpose:
*     Get the value of the ImagFreq attribute.

*  Type:
*     Protected virtual function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     double GetImagFreq( AstDSBSpecFrame *this )

*  Class Membership:
*     DSBSpecFrame method.

*  Description:
*     This function returns the image sideband frequency corresponding to
*     the rest frequency.

*  Parameters:
*     this
*        Pointer to the Frame.

*  Returned Value:
*     The required frequency, in Hz.

*  Notes:
*     - A value of AST__BAD will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Variables: */
   AstMapping *map;         /* Pointer to "Observed to Image" mapping */
   double result;           /* The returned frequency */
   double rf;               /* Rest frequency in observed sideband */
   int oldsb;               /* Original value of the SideBand Attribute */
   int oldsor;              /* Original value of the StdOfRest Attribute */
   int sbset;               /* Was the SideBand attribute set?*/
   int sorset;              /* Was the StdOfRest attribute set?*/

/* Check the global error status. */
   if ( !astOK ) return AST__BAD;

/* Initialise */
   oldsb = 0;
   oldsor = 0;

/* Save the original value of the StdOfRest attribute and set it
   temporarily to SOURCE. */
   sorset = astTestStdOfRest( this );
   if( sorset ) oldsor = astGetStdOfRest( this );
   astSetStdOfRest( this, AST__SCSOR );

/* Save the original value of the SideBand attribute and temporarily 
   set it to "observed". */
   sbset = astTestSideBand( this );
   if( sbset ) oldsb = astGetSideBand( this );
   astSetC( this, "SideBand", "observed" );

/* Create a Mapping which transforms positions from the observed to the
   image sideband. */
   if( astGetSideBand( this ) == USB ) {
      map = ToLSBMapping( this, "astGetImagFreq" );
   } else {
      map = ToUSBMapping( this, "astGetImagFreq" );
   }

/* Re-instate the original attribute values. */
   if( sorset ) {
      astSetStdOfRest( this, oldsor );
   } else {
      astClearStdOfRest( this );
   }

   if( sbset ) {
      astSetSideBand( this, oldsb );
   } else {
      astClearSideBand( this );
   }

/* Get the rest frequency in Hz, and transform it using the above Mapping. */
   rf = astGetRestFreq( this );
   astTran1( map, 1, &rf, 1, &result );

/* Annul the Mapping */
   map = astAnnul( map );

/* If an error has occurrred, return AST__BAD. */
   if( !astOK ) result = AST__BAD;

/* Return the result. */
   return result;

}

static const char *GetLabel( AstFrame *this, int axis ) {
/*
*  Name:
*     GetLabel

*  Purpose:
*     Access the Label string for a DSBSpecFrame axis.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     const char *GetLabel( AstFrame *this, int axis )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the astGetLabel method inherited
*     from the SpecFrame class).

*  Description:
*     This function returns a pointer to the Label string for a specified axis
*     of a DSBSpecFrame.

*  Parameters:
*     this
*        Pointer to the SpecFrame.
*     axis
*        Axis index (zero-based) identifying the axis for which information is
*        required.

*  Returned Value:
*     Pointer to a constant null-terminated character string containing the
*     requested information.

*  Notes:
*     -  A NULL pointer will be returned if this function is invoked with the
*     global error status set, or if it should fail for any reason.
*/

/* Local Constants: */
#define BUFF_LEN 200             /* Max characters in result string */

/* Local Variables: */
   const char *result;           /* Pointer to label string */
   static char buff[ BUFF_LEN + 1 ]; /* Buffer for result string */

/* Check the global error status. */
   if ( !astOK ) return NULL;

/* Initialise. */
   result = NULL;

/* Validate the axis index. */
   astValidateAxis( this, axis, "astGetLabel" );

/* Invoke the parent astGetLabel method to obtain a pointer to it. */
   result = (*parent_getlabel)( this, axis );

/* Check if this is a default value. If so, append a string indicating
   the sideband. */
   if ( !astTestLabel( this, axis ) ) {

/* If OK, supply a pointer to a suitable default label string. */
      sprintf( buff, "%s (%s)", result, astGetAttrib( this, "sideband" ) );
      result = buff;
   }

/* Return the result. */
   return result;

/* Undefine macros local to this function. */
#undef BUFF_LEN

}

void astInitDSBSpecFrameVtab_(  AstDSBSpecFrameVtab *vtab, const char *name ) {
/*
*+
*  Name:
*     astInitDSBSpecFrameVtab

*  Purpose:
*     Initialise a virtual function table for a DSBSpecFrame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     void astInitDSBSpecFrameVtab( AstDSBSpecFrameVtab *vtab, const char *name )

*  Class Membership:
*     DSBSpecFrame vtab initialiser.

*  Description:
*     This function initialises the component of a virtual function
*     table which is used by the DSBSpecFrame class.

*  Parameters:
*     vtab
*        Pointer to the virtual function table. The components used by
*        all ancestral classes will be initialised if they have not already
*        been initialised.
*     name
*        Pointer to a constant null-terminated character string which contains
*        the name of the class to which the virtual function table belongs (it 
*        is this pointer value that will subsequently be returned by the Object
*        astClass function).
*-
*/

/* Local Variables: */
   AstObjectVtab *object;        /* Pointer to Object component of Vtab */
   AstFrameVtab *frame;          /* Pointer to Frame component of Vtab */

/* Check the local error status. */
   if ( !astOK ) return;

/* Initialize the component of the virtual function table used by the
   parent class. */
   astInitSpecFrameVtab( (AstSpecFrameVtab *) vtab, name );

/* Store a unique "magic" value in the virtual function table. This
   will be used (by astIsADSBSpecFrame) to determine if an object belongs
   to this class.  We can conveniently use the address of the (static)
   class_init variable to generate this unique value. */
   vtab->check = &class_init;

/* Initialise member function pointers. */
/* ------------------------------------ */
/* Store pointers to the member functions (implemented here) that provide
   virtual methods for this class. */
   vtab->ClearDSBCentre = ClearDSBCentre;
   vtab->TestDSBCentre = TestDSBCentre;
   vtab->GetDSBCentre = GetDSBCentre;
   vtab->SetDSBCentre = SetDSBCentre;

   vtab->ClearIF = ClearIF;
   vtab->TestIF = TestIF;
   vtab->GetIF = GetIF;
   vtab->SetIF = SetIF;

   vtab->ClearSideBand = ClearSideBand;
   vtab->TestSideBand = TestSideBand;
   vtab->GetSideBand = GetSideBand;
   vtab->SetSideBand = SetSideBand;

   vtab->GetImagFreq = GetImagFreq;

/* Save the inherited pointers to methods that will be extended, and
   replace them with pointers to the new member functions. */
   object = (AstObjectVtab *) vtab;
   frame = (AstFrameVtab *) vtab;

   parent_clearattrib = object->ClearAttrib;
   object->ClearAttrib = ClearAttrib;

   parent_getattrib = object->GetAttrib;
   object->GetAttrib = GetAttrib;

   parent_setattrib = object->SetAttrib;
   object->SetAttrib = SetAttrib;

   parent_testattrib = object->TestAttrib;
   object->TestAttrib = TestAttrib;

   parent_overlay = frame->Overlay;
   frame->Overlay = Overlay; 

   parent_match = frame->Match;
   frame->Match = Match;

   parent_subframe = frame->SubFrame;
   frame->SubFrame = SubFrame;

   parent_getlabel = frame->GetLabel;
   frame->GetLabel = GetLabel;

/* Declare the class delete function.*/
   astSetDump( vtab, Dump, "DSBSpecFrame", "Dual sideband spectral axis" );

}

static AstMapping *ToLSBMapping( AstDSBSpecFrame *this, const char *method ){
/*
*  Name:
*     ToLSBMapping

*  Purpose:
*     Create a Mapping which transforms a DSBSpecFrame to the lower
*     sideband.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     AstMapping *ToLSBMapping( AstDSBSpecFrame *this, const char *method )

*  Class Membership:
*     DSBSpecFrame member function 

*  Description:
*     This function returns a pointer to a new Mapping which transforms
*     positions in the supplied DSBSpecFrame to the lower sideband. This
*     will be a UnitMap if the DSBSpecFrame already represents the lower
*     sideband.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     method
*        Pointer to a null-terminated string containing the name of the
*        public invoking method. This is only used in the construction of
*        error messages. 

*  Returned Value:
*     Pointer to a new Mapping.

*  Notes:
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstMapping *fmap;         /* LSB to USB (topo freq) */
   AstMapping *map1;         /* This to USB (topo freq) */
   AstMapping *map2;         /* This (LSB) to This (USB) */
   AstMapping *result;       /* Pointer to the returned Mapping */
   AstMapping *tmap;         /* This to topocentric freq */
   double f_lo;              /* Local oscillator freq (topo Hz) */
   double f_lsb_a;           /* First LSB freq */
   double f_lsb_b;           /* Second LSB freq */
   double f_usb_a;           /* First USB freq */
   double f_usb_b;           /* Second USB freq */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* If the DSBSpecFrame already represents the LSB, return a UnitMap.*/
   if( astGetSideBand( this ) == LSB ) {
      result = (AstMapping *) astUnitMap( 1, "" );

/* If the DSBSpecFrame represents the USB, create a suitable WinMap. */
   } else {

/* Find the Mapping from the spectral system described by this SpecFrame to 
   topocentric frequency in Hz. */
      tmap = TopoMap( this, 1, method );         

/* Calculate the local oscillator frequency (topocentric in Hertz). */
      f_lo = astGetDSBCentre( this ) + astGetIF( this );

/* Create a 1D WinMap which converts f_usb to f_lsb. */
      f_usb_a = 0.0;
      f_usb_b = 2*f_lo;
      f_lsb_a = 2*f_lo;
      f_lsb_b = 0.0;
      fmap = (AstMapping *) astWinMap( 1, &f_usb_a, &f_usb_b, &f_lsb_a, &f_lsb_b, "" );
      
/* Construct the Mapping: input to f_usb, f_usb to f_lsb, f_lsb to input */
      map1 = (AstMapping *) astCmpMap( tmap, fmap, 1, "" );
      astInvert( tmap );
      map2 = (AstMapping *) astCmpMap( map1, tmap, 1, "" );

/* Simplify */
      result = astSimplify( map2 );

/* Free resources */
      tmap = astAnnul( tmap );
      fmap = astAnnul( fmap );
      map1 = astAnnul( map1 );
      map2 = astAnnul( map2 );
   }

/* Return NULL if an error has occurred. */
   if( !astOK ) result = astAnnul( result );

/* Return the result. */
   return result;

}

static int Match( AstFrame *template_frame, AstFrame *target,
                  int **template_axes, int **target_axes, AstMapping **map,
                  AstFrame **result ) {
/*
*  Name:
*     Match

*  Purpose:
*     Determine if conversion is possible between two coordinate systems.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     int Match( AstFrame *template, AstFrame *target,
*                int **template_axes, int **target_axes,
*                AstMapping **map, AstFrame **result )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the protected astMatch method
*     inherited from the SpecFrame class).

*  Description:
*     This function matches a "template" DSBSpecFrame to a "target" Frame and
*     determines whether it is possible to convert coordinates between them.
*     If it is, a mapping that performs the transformation is returned along
*     with a new Frame that describes the coordinate system that results when
*     this mapping is applied to the "target" coordinate system. In addition,
*     information is returned to allow the axes in this "result" Frame to be
*     associated with the corresponding axes in the "target" and "template"
*     Frames from which they are derived.

*  Parameters:
*     template
*        Pointer to the template DSBSpecFrame. This describes the coordinate 
*        system (or set of possible coordinate systems) into which we wish to 
*        convert our coordinates.
*     target
*        Pointer to the target Frame. This describes the coordinate system in
*        which we already have coordinates.
*     template_axes
*        Address of a location where a pointer to int will be returned if the
*        requested coordinate conversion is possible. This pointer will point
*        at a dynamically allocated array of integers with one element for each
*        axis of the "result" Frame (see below). It must be freed by the caller
*        (using astFree) when no longer required.
*
*        For each axis in the result Frame, the corresponding element of this
*        array will return the index of the template DSBSpecFrame axis from 
*        which it is derived. If it is not derived from any template 
*        DSBSpecFrame axis, a value of -1 will be returned instead.
*     target_axes
*        Address of a location where a pointer to int will be returned if the
*        requested coordinate conversion is possible. This pointer will point
*        at a dynamically allocated array of integers with one element for each
*        axis of the "result" Frame (see below). It must be freed by the caller
*        (using astFree) when no longer required.
*
*        For each axis in the result Frame, the corresponding element of this
*        array will return the index of the target Frame axis from which it
*        is derived. If it is not derived from any target Frame axis, a value
*        of -1 will be returned instead.
*     map
*        Address of a location where a pointer to a new Mapping will be
*        returned if the requested coordinate conversion is possible. If
*        returned, the forward transformation of this Mapping may be used to
*        convert coordinates between the "target" Frame and the "result"
*        Frame (see below) and the inverse transformation will convert in the
*        opposite direction.
*     result
*        Address of a location where a pointer to a new Frame will be returned
*        if the requested coordinate conversion is possible. If returned, this
*        Frame describes the coordinate system that results from applying the
*        returned Mapping (above) to the "target" coordinate system. In
*        general, this Frame will combine attributes from (and will therefore
*        be more specific than) both the target and the template Frames. In
*        particular, when the template allows the possibility of transformaing
*        to any one of a set of alternative coordinate systems, the "result"
*        Frame will indicate which of the alternatives was used.

*  Returned Value:
*     A non-zero value is returned if the requested coordinate conversion is
*     possible. Otherwise zero is returned (this will not in itself result in
*     an error condition).

*  Notes:
*     -  A value of zero will be returned if this function is invoked with the
*     global error status set, or if it should fail for any reason.

*  Implementation Notes:
*     This implementation addresses the matching of a DSBSpecFrame class 
*     object to any other class of Frame. A DSBSpecFrame will match any class 
*     of DSBSpecFrame (i.e. possibly from a derived class) but will not match 
*     a less specialised class of Frame (except for a SpecFrame).
*/

/* Local Variables: */
   AstDSBSpecFrame *template;    /* Pointer to template DSBSpecFrame structure */
   int match;                    /* Coordinate conversion possible? */

/* Initialise the returned values. */
   *template_axes = NULL;
   *target_axes = NULL;
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;

/* Obtain a pointer to the template DSBSpecFrame structure. */
   template = (AstDSBSpecFrame *) template_frame;

/* The first criterion for a match is that the template matches as a
   SpecFrame class object. This ensures that the number of axes (1) and
   domain, class, etc. of the target Frame are suitable. Invoke the parent
   "astMatch" method to verify this. */
   match = (*parent_match)( template_frame, target,
                            template_axes, target_axes, map, result );

/* If a match was found, the target Frame must be a SpecFrame but may not
   be a DSBSpecFrame. We use the returned objects directly if the target 
   is a SpecFrame but not a DSBSpecFrame. So if a DSBSpecFrame and a
   base SpecFrame are aligned, this will result in the DSBSpecFrame
   behaving as a normal SpecFrame. */
   if ( astOK && match ) {
      if( astIsADSBSpecFrame( target ) ) {      

/* Annul the returned objects, which are not needed, but keep the memory 
   allocated for the axis association arrays, which we will re-use. */
         *map = astAnnul( *map );
         *result = astAnnul( *result );

/* We next set up the axis association arrays. */
         (*template_axes)[ 0 ] = 0;
         (*target_axes)[ 0 ] = 0;

/* Use the target's "astSubFrame" method to create a new Frame (the
   result Frame) with a copy of of the target axis. This process also 
   overlays the template attributes on to the target Frame and returns a 
   Mapping between the target and result Frames which effects the required 
   coordinate conversion. */
         match = astSubFrame( target, template, 1, *target_axes, *template_axes,
                              map, result );

/* If an error occurred, or conversion to the result Frame's coordinate 
   system was not possible, then free all memory, annul the returned 
   objects, and reset the returned value. */
         if ( !astOK || !match ) {
            *template_axes = astFree( *template_axes );
            *target_axes = astFree( *target_axes );
            if( *map ) *map = astAnnul( *map );
            if( *result ) *result = astAnnul( *result );
            match = 0;
         }
      }
   }

/* Return the result. */
   return match;
}

static void Overlay( AstFrame *template, const int *template_axes,
                     AstFrame *result ) {
/*
*  Name:
*     Overlay

*  Purpose:
*     Overlay the attributes of a template DSBSpecFrame on to another Frame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "specframe.h"
*     void Overlay( AstFrame *template, const int *template_axes,
*                   AstFrame *result )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the protected astOverlay method
*     inherited from the SpecFrame class).

*  Description:
*     This function overlays attributes of a DSBSpecFrame (the "template") on to
*     another Frame, so as to over-ride selected attributes of that second
*     Frame. Normally only those attributes which have been specifically set
*     in the template will be transferred. This implements a form of
*     defaulting, in which a Frame acquires attributes from the template, but
*     retains its original attributes (as the default) if new values have not
*     previously been explicitly set in the template.
*
*     Note that if the result Frame is a DSBSpecFrame and a change of spectral
*     coordinate system occurs as a result of overlaying its System
*     attribute, then some of its original attribute values may no
*     longer be appropriate (e.g. the Title, or attributes describing
*     its axes). In this case, these will be cleared before overlaying
*     any new values.

*  Parameters:
*     template
*        Pointer to the template DSBSpecFrame, for which values should have been
*        explicitly set for any attribute which is to be transferred.
*     template_axes
*        Pointer to an array of int, with one element for each axis of the
*        "result" Frame (see below). For each axis in the result frame, the
*        corresponding element of this array should contain the (zero-based)
*        index of the template axis to which it corresponds. This array is used
*        to establish from which template axis any axis-dependent attributes
*        should be obtained.
*
*        If any axis in the result Frame is not associated with a template
*        axis, the corresponding element of this array should be set to -1.
*     result
*        Pointer to the Frame which is to receive the new attribute values.

*  Returned Value:
*     void

*  Notes:
*     -  In general, if the result Frame is not from the same class as the
*     template DSBSpecFrame, or from a class derived from it, then attributes may
*     exist in the template DSBSpecFrame which do not exist in the result Frame. 
*     In this case, these attributes will not be transferred.
*/

/* Check the global error status. */
   if ( !astOK ) return;

/* Invoke the parent class astOverlay method to transfer attributes inherited
   from the parent class. */
   (*parent_overlay)( template, template_axes, result );

/* Check if the result Frame is a DSBSpecFrame or from a class derived from
   DSBSpecFrame. If not, we cannot transfer DSBSpecFrame attributes to it as it is
   insufficiently specialised. In this case simply omit these attributes. */
   if( astIsADSBSpecFrame( result ) && astOK ) {

/* Define macros that test whether an attribute is set in the template and,
   if so, transfers its value to the result. */
#define OVERLAY(attribute) \
   if ( astTest##attribute( template ) ) { \
      astSet##attribute( result, astGet##attribute( template ) ); \
   }

/* Use the macro to transfer each DSBSpecFrame attribute in turn. */
      OVERLAY(DSBCentre)
      OVERLAY(IF)
      OVERLAY(SideBand)
   }

/* Undefine macros local to this function. */
#undef OVERLAY
}

static void SetAttrib( AstObject *this_object, const char *setting ) {
/*
*  Name:
*     astSetAttrib

*  Purpose:
*     Set an attribute value for a DSBSpecFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     void SetAttrib( AstObject *this, const char *setting )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the astSetAttrib protected
*     method inherited from the SpecFrame class).

*  Description:
*     This function assigns an attribute value for a DSBSpecFrame, the
*     attribute and its value being specified by means of a string of
*     the form:
*
*        "attribute= value "
*
*     Here, "attribute" specifies the attribute name and should be in
*     lower case with no white space present. The value to the right
*     of the "=" should be a suitable textual representation of the
*     value to be assigned and this will be interpreted according to
*     the attribute's data type.  White space surrounding the value is
*     only significant for string attributes.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     setting
*        Pointer to a null-terminated string specifying the new attribute
*        value.
*/

/* Local Variables: */
   AstDSBSpecFrame *this;        /* Pointer to the DSBSpecFrame structure */
   AstMapping *tmap;             /* Ptr to Mapping from this to topofreq */
   AstMapping *umap;             /* Ptr to Mapping between units */
   double dtemp;                 /* Attribute value */
   double dval;                  /* Attribute value */
   int ival;                     /* Attribute value */
   int len;                      /* Length of setting string */
   int nc;                       /* Used length */
   int off;                      /* Offset to start of string */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the DSBSpecFrame structure. */
   this = (AstDSBSpecFrame *) this_object;

/* Obtain the length of the setting string. */
   len = (int) strlen( setting );

/* Test for each recognised attribute in turn, using "astSscanf" to parse the
   setting string and extract the attribute value (or an offset to it in the
   case of string values). In each case, use the value set in "nc" to check
   that the entire string was matched. Once a value has been obtained, use the
   appropriate method to set it. */

/* DSBCentre */
/* --------- */
   if ( strstr( setting, "dsbcentre=" ) ) {

/* Without any units indication - assume it is supplied in the system of
   the DSBSpecFrame. */
      int ok = 0;
      if( nc = 0,
           ( 1 == astSscanf( setting, "dsbcentre= %lg %n", &dval, &nc ) )
           && ( nc >= len ) ) {
         ok = 1;

/* With units indication. Is there a Mapping from the supplied units to the 
   units used by the DSBSpecFrame? If so, use the Mapping to convert the 
   supplied value to the required units. */
      } else if ( nc = 0,
           ( 1 == astSscanf( setting, "dsbcentre= %lg %n%*s %n", &dval, &off, &nc ) )
           && ( nc >= len ) ) {

         if( ( umap = astUnitMapper( setting + off, astGetUnit( this, 0 ), NULL, NULL ) ) ) {
            astTran1( umap, 1, &dval, 1, &dtemp );
            dval = dtemp;
            umap = astAnnul( umap );
            if( astOK && dval != AST__BAD ) ok = 1;

/* Otherwise report an error. */
         } else if( astOK ) {
            astError( AST__ATTIN, "astSetAttrib(%s): Value supplied for "
                      "attribute \"DSBCentre\" (%s) uses units which are "
                      "inappropriate for the current spectral system (%s).", 
                       astGetClass( this ), setting + 10, 
                       astGetTitle( this ) );
         }
      }

/* Convert the value from the supplied system to topocentric frequency in
   Hx, and store. */
      if( ok ) {

/* Find the Mapping from the spectral system described by this SpecFrame to 
   topocentric frequency in Hz. */
         tmap = TopoMap( this, 1, "astSetAttrib" );         
         if ( astOK ) {

/* Transform the supplied value to topocentric frequency. */
            astTran1( tmap, 1, &dval, 1, &dtemp );
            if( dtemp == AST__BAD ) {
               astError( AST__ATTIN, "astSetAttrib(%s): The setting \"%s\" is "
                         "invalid for a %s.", astGetClass( this ), setting,
                         astGetClass( this ) );
            } else {

/* Store it. */
               astSetDSBCentre( this, dtemp );
   
            }
            tmap = astAnnul( tmap );
         }

      } else if( astOK ) {
         astError( AST__ATTIN, "astSetAttrib(%s): The setting \"%s\" is "
                   "invalid for a %s.", astGetClass( this ), setting,
                   astGetClass( this ) );
      }

/* IF */
/* -- */
/* Without any units indication - assume GHz. Convert to Hz for storage. */
   } else if ( nc = 0,
        ( 1 == astSscanf( setting, "if= %lg %n", &dval, &nc ) )
        && ( nc >= len ) ) {
      astSetIF( this, dval*1.0E9 );

/* With units indication. */
   } else if ( nc = 0,
        ( 1 == astSscanf( setting, "if= %lg %n%*s %n", &dval, &off, &nc ) )
        && ( nc >= len ) ) {

/* Is there a Mapping from the supplied units to Hz? If so, use the
   Mapping to convert the supplied value to Hz. */
      if( ( umap = astUnitMapper( setting + off, "Hz", NULL, NULL ) ) ) {
         astTran1( umap, 1, &dval, 1, &dtemp );
         umap = astAnnul( umap );

/* Otherwise report an error. */
      } else if( astOK ) {
         astError( AST__ATTIN, "astSetAttrib(%s): Intermediate frequency given "
                   "in an inappropriate system of units \"%g %s\".", 
                   astGetClass( this ), dval, setting + off );
      }

/* Set the intermediate frequency. */
      astSetIF( this, dtemp );

/* SideBand */
/* -------- */
   } else if ( nc = 0,
               ( 0 == astSscanf( setting, "sideband= %n%*s %n", &ival, &nc ) )
               && ( nc >= len ) ) {

      if( astChrMatch( "usb", setting+ival ) ) {
         astSetSideBand( this, USB );

      } else if( astChrMatch( "lsb", setting+ival ) ) {
         astSetSideBand( this, LSB );

      } else if( astChrMatch( "observed", setting+ival ) ) {
         astSetSideBand( this, ( astGetIF( this ) > 0 ) ? LSB : USB );

      } else if( astChrMatch( "image", setting+ival ) ) {
         astSetSideBand( this, ( astGetIF( this ) <= 0 ) ? LSB : USB );

      } else {
         astError( AST__ATTIN, "astSetAttrib(%s): The setting \"%s\" is "
                   "invalid for a %s.", astGetClass( this ), setting,
                   astGetClass( this ) );
      }

/* Read-only attributes. */
/* --------------------- */
/* Define a macro to see if the setting string matches any of the
   read-only attributes of this class. */
#define MATCH(attrib) \
        ( nc = 0, ( 0 == astSscanf( setting, attrib "=%*[^\n]%n", &nc ) ) && \
                  ( nc >= len ) )

/* Use this macro to report an error if a read-only attribute has been
   specified. */
   } else if ( MATCH( "imagfreq" ) ) {
      astError( AST__NOWRT, "astSet: The setting \"%s\" is invalid for a %s.",
                setting, astGetClass( this ) );
      astError( AST__NOWRT, "This is a read-only attribute." );

/* Pass any unrecognised setting to the parent method for further
   interpretation. */
   } else {
      (*parent_setattrib)( this_object, setting );
   }
}

static int SubFrame( AstFrame *target_frame, AstFrame *template,
                     int result_naxes, const int *target_axes,
                     const int *template_axes, AstMapping **map,
                     AstFrame **result ) {
/*
*  Name:
*     SubFrame

*  Purpose:
*     Select axes from a DSBSpecFrame and convert to the new coordinate 
*     system.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     int SubFrame( AstFrame *target, AstFrame *template,
*                   int result_naxes, const int *target_axes,
*                   const int *template_axes, AstMapping **map,
*                   AstFrame **result )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the protected astSubFrame 
*     method inherited from the SpecFrame class).

*  Description:
*     This function selects a requested sub-set (or super-set) of the axes
*     from a "target" DSBSpecFrame and creates a new Frame with copies of
*     the selected axes assembled in the requested order. It then
*     optionally overlays the attributes of a "template" Frame on to the
*     result. It returns both the resulting Frame and a Mapping that
*     describes how to convert between the coordinate systems described by
*     the target and result Frames. If necessary, this Mapping takes
*     account of any differences in the Frames' attributes due to the
*     influence of the template.

*  Parameters:
*     target
*        Pointer to the target DSBSpecFrame, from which axes are to be 
*        selected.
*     template
*        Pointer to the template Frame, from which new attributes for the
*        result Frame are to be obtained. Optionally, this may be NULL, in
*        which case no overlaying of template attributes will be performed.
*     result_naxes
*        Number of axes to be selected from the target Frame. This number may
*        be greater than or less than the number of axes in this Frame (or
*        equal).
*     target_axes
*        Pointer to an array of int with result_naxes elements, giving a list
*        of the (zero-based) axis indices of the axes to be selected from the
*        target DSBSpecFrame. The order in which these are given determines
*        the order in which the axes appear in the result Frame. If any of the
*        values in this array is set to -1, the corresponding result axis will
*        not be derived from the target Frame, but will be assigned default
*        attributes instead.
*     template_axes
*        Pointer to an array of int with result_naxes elements. This should
*        contain a list of the template axes (given as zero-based axis indices)
*        with which the axes of the result Frame are to be associated. This
*        array determines which axes are used when overlaying axis-dependent
*        attributes of the template on to the result. If any element of this
*        array is set to -1, the corresponding result axis will not receive any
*        template attributes.
*
*        If the template argument is given as NULL, this array is not used and
*        a NULL pointer may also be supplied here.
*     map
*        Address of a location to receive a pointer to the returned Mapping.
*        The forward transformation of this Mapping will describe how to
*        convert coordinates from the coordinate system described by the target
*        DSBSpecFrame to that described by the result Frame. The inverse
*        transformation will convert in the opposite direction.
*     result
*        Address of a location to receive a pointer to the result Frame.

*  Returned Value:
*     A non-zero value is returned if coordinate conversion is possible
*     between the target and the result Frame. Otherwise zero is returned and
*     *map and *result are returned as NULL (but this will not in itself
*     result in an error condition). In general, coordinate conversion should
*     always be possible if no template Frame is supplied but may not always
*     be possible otherwise.

*  Notes:
*     -  A value of zero will be returned if this function is invoked with the
*     global error status set, or if it should fail for any reason.

*  Implementation Notes:
*     -  This implementation addresses the selection of axes from a 
*     DSBSpecFrame object. This results in another object of the same class 
*     only if the single DSBSpecFrame axis is selected exactly once. 
*     Otherwise, the result is a Frame class object which inherits the 
*     DSBSpecFrame's axis information (if appropriate) but none of the other 
*     properties of a DSBSpecFrame.
*     -  In the event that a DSBSpecFrame results, the returned Mapping will 
*     take proper account of the relationship between the target and result 
*     coordinate systems.
*     -  In the event that a Frame class object results, the returned Mapping
*     will only represent a selection/permutation of axes.

*  Implementation Deficiencies:
*     -  Any axis selection is currently permitted. Probably this should be
*     restricted so that each axis can only be selected once. The
*     astValidateAxisSelection method will do this but currently there are bugs
*     in the CmpFrame class that cause axis selections which will not pass this
*     test. Install the validation when these are fixed.
*/

/* Local Variables: */
   AstDSBSpecFrame *dsbresult;/* Pointer to the DSBSpecFrame result Frame */
   AstDSBSpecFrame *dsbtarget;/* Pointer to the DSBSpecFrame target Frame */
   AstMapping *map1;          /* Intermediate Mapping */
   AstMapping *map2;          /* Intermediate Mapping */
   AstMapping *map3;          /* Intermediate Mapping */
   int match;                 /* Coordinate conversion is possible? */

/* Initialise the returned values. */
   *map = NULL;
   *result = NULL;
   match = 0;

/* Check the global error status. */
   if ( !astOK ) return match;

/* Invoke the astSubFrame method inherited from the parent SpecFrame
   class. This will (if possible) create a result Frame which is a
   DSBSpecFrame (since the supplied target Frame is a DSBSpecFrame).
   However, the Mapping from target to result Frame will take no account
   of any differences in the values of the attributes specific to the 
   DSBSpecFrame class. */
   match = (*parent_subframe)( target_frame, template, result_naxes, 
                               target_axes, template_axes, map, result );

/* If a match occurred, and the result Frame is a DSBSpecFrame, we now 
   modify the Mapping to take account of DSBSpecFrame-specific attributes. */
   if( match && astIsADSBSpecFrame( *result ) ) {

/* Get pointers to the two DSBSpecFrames */
      dsbtarget = (AstDSBSpecFrame *) target_frame;      
      dsbresult = (AstDSBSpecFrame *) *result;

/* Create a Mapping which transforms positions from the target to an exact 
   copy of the target in which the SideBand attribute is set to USB. This
   will be a UnitMap if the target already represents the USB. */
      map1 = ToUSBMapping( dsbtarget, "astSubFrame" );

/* Create a Mapping which transforms positions from the result to an exact 
   copy of the target in which the SideBand attribute is set to USB. This
   will be a UnitMap if the target already represents the USB. Invert
   this Mapping to get a Mapping which goes from USB to the SideBand
   represented by the result. */
      map2 = ToUSBMapping( dsbresult, "astSubFrame" );
      astInvert( map2 );

/* Form a Mapping which first maps target values to USB, then applies the
   Mapping returned by the parent SubFrame method in order to convert
   between spectral systems, and then converts from USB to the SideBand of
   the result. */
      map3 = (AstMapping *) astCmpMap( map1, *map, 1, "" );
      map1 = astAnnul( map1 );
      *map = astAnnul( *map );
      map1 = (AstMapping *) astCmpMap( map3, map2, 1, "" );
      map3 = astAnnul( map3 );
      map2 = astAnnul( map2 );

/* Returned the simplified Mapping. */
      *map = astSimplify( map1 );
      map1 = astAnnul( map1 );

   }

/* If an error occurred or no match was found, annul the returned
   objects and reset the returned result. */
   if ( !astOK || !match ) {
      *map = astAnnul( *map );
      *result = astAnnul( *result );
      match = 0;
   }

/* Return the result. */
   return match;

}

static int TestAttrib( AstObject *this_object, const char *attrib ) {
/*
*  Name:
*     TestAttrib

*  Purpose:
*     Test if a specified attribute value is set for a DSBSpecFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     int TestAttrib( AstObject *this, const char *attrib )

*  Class Membership:
*     DSBSpecFrame member function (over-rides the astTestAttrib protected
*     method inherited from the SpecFrame class).

*  Description:
*     This function returns a boolean result (0 or 1) to indicate whether
*     a value has been set for one of a DSBSpecFrame's attributes.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     attrib
*        Pointer to a null-terminated string specifying the attribute
*        name.  This should be in lower case with no surrounding white
*        space.

*  Returned Value:
*     One if a value has been set, otherwise zero.

*  Notes:
*     - A value of zero will be returned if this function is invoked
*     with the global status set, or if it should fail for any reason.
*/

/* Local Variables: */
   AstDSBSpecFrame *this;        /* Pointer to the DSBSpecFrame structure */
   int result;                   /* Result value to return */

/* Initialise. */
   result = 0;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Obtain a pointer to the DSBSpecFrame structure. */
   this = (AstDSBSpecFrame *) this_object;

/* Check the attribute name and test the appropriate attribute. */

/* DSBCentre */
/* --------- */
   if ( !strcmp( attrib, "dsbcentre" ) ) {
      result = astTestDSBCentre( this );

/* IF */
/* -- */
   } else if ( !strcmp( attrib, "if" ) ) {
      result = astTestIF( this );

/* SideBand */
/* -------- */
   } else if ( !strcmp( attrib, "sideband" ) ) {
      result = astTestSideBand( this );

/* Read-only attributes. */
/* --------------------- */
/* Test if the attribute name matches any of the read-only attributes
   of this class. If it does, then return zero. */
   } else if ( !strcmp( attrib, "imagfreq" ) ) {
      result = 0;

/* If the attribute is not recognised, pass it on to the parent method
   for further interpretation. */
   } else {
      result = (*parent_testattrib)( this_object, attrib );
   }

/* Return the result, */
   return result;
}

static AstMapping *TopoMap( AstDSBSpecFrame *this, int forward, 
                            const char *method ){
/*
*  Name:
*     TopoMap

*  Purpose:
*     Create a Mapping which transforms a DSBSpecFrame to topocentric
*     frequency.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     AstMapping *TopoMap( AstDSBSpecFrame *this, int forward, 
*                          const char *method )

*  Class Membership:
*     DSBSpecFrame member function 

*  Description:
*     This function returns a pointer to a new Mapping which transforms
*     positions in the supplied DSBSpecFrame to the corresponding
*     topocentric frequency values in Hz (or the inverse of this).

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     forward
*        If zero, the calcuated Mapping is inverted before being returned.
*     method
*        Pointer to a null-terminated string containing the name of the
*        public invoking method. This is only used in the construction of
*        error messages. 

*  Returned Value:
*     Pointer to a new Mapping.

*  Notes:
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstMapping *result;       /* The returned Mapping */
   AstFrameSet *fs;          /* FrameSet connecting tf1 and tf2 */
   AstSpecFrame *tf1;        /* SpecFrame corresponding to this DSBSpecFrame */
   AstSpecFrame *tf2;        /* Topocentric frequency SpecFrame */
   int template_axis;        /* The axis to overlay */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* Make a SpecFrame and then overlay the SpecFrame attributes of this 
   DSBSpecFrame onto the new SpecFrame. This means it inherits the current 
   values of things like GeoLon and GeoLat. */
   tf1 = astSpecFrame( "" );
   template_axis = 0;
   (*parent_overlay)( (AstFrame *) this, &template_axis, (AstFrame *) tf1 );

/* Copy this new SpecFrame and set its attributes to describe topocentric
   frequency in Hz. Ensure that alignment occurs in the topocentric Frame. */
   astSetAlignStdOfRest( tf1, AST__TPSOR);
   tf2 = astCopy( tf1 );
   astSetSystem( tf2, AST__FREQ );
   astSetStdOfRest( tf2, AST__TPSOR );
   astSetUnit( tf2, 0, "Hz" );

/* Find the Mapping from the spectral system described by this SpecFrame to 
   topocentric frequency in Hz. */
   fs = astConvert( tf1, tf2, "SPECTRUM" );
   tf1 = astAnnul( tf1 );
   tf2 = astAnnul( tf2 );
   if ( astOK ) {
      if( !fs ) {
         astError( AST__INTER, "%s(%s): Cannot convert DSBCentre "
                   "value from the supplied system to topocentric frequency "
                   "(internal AST programming error).", method, 
                   astGetClass( this ) );
      } else {
         result = astGetMapping( fs, AST__BASE, AST__CURRENT );
         if( !forward ) astInvert( result );
      }
      fs = astAnnul( fs );
   }

/* Annul the result if an error has occurred. */
   if( !astOK ) result = astAnnul( result );

/* Return the result. */
   return result;

}

static AstMapping *ToUSBMapping( AstDSBSpecFrame *this, const char *method ){
/*
*  Name:
*     ToUSBMapping

*  Purpose:
*     Create a Mapping which transforms a DSBSpecFrame to the upper
*     sideband.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     AstMapping *ToUSBMapping( AstDSBSpecFrame *this, const char *method )

*  Class Membership:
*     DSBSpecFrame member function 

*  Description:
*     This function returns a pointer to a new Mapping which transforms
*     positions in the supplied DSBSpecFrame to the upper sideband. This
*     will be a UnitMap if the DSBSpecFrame already represents the upper
*     sideband.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame.
*     method
*        Pointer to a null-terminated string containing the name of the
*        public invoking method. This is only used in the construction of
*        error messages. 

*  Returned Value:
*     Pointer to a new Mapping.

*  Notes:
*     - A NULL pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*/

/* Local Variables: */
   AstMapping *fmap;         /* LSB to USB (topo freq) */
   AstMapping *map1;         /* This to USB (topo freq) */
   AstMapping *map2;         /* This (LSB) to This (USB) */
   AstMapping *result;       /* Pointer to the returned Mapping */
   AstMapping *tmap;         /* This to topocentric freq */
   double f_lo;              /* Local oscillator freq (topo Hz) */
   double f_lsb_a;           /* First LSB freq */
   double f_lsb_b;           /* Second LSB freq */
   double f_usb_a;           /* First USB freq */
   double f_usb_b;           /* Second USB freq */

/* Initialise. */
   result = NULL;

/* Check the global error status. */
   if ( !astOK ) return result;

/* If the DSBSpecFrame already represents the USB, return a UnitMap.*/
   if( astGetSideBand( this ) == USB ) {
      result = (AstMapping *) astUnitMap( 1, "" );

/* If the DSBSpecFrame represents the LSB, create a suitable WinMap. */
   } else {

/* Find the Mapping from the spectral system described by this SpecFrame to 
   topocentric frequency in Hz. */
      tmap = TopoMap( this, 1, method );         

/* Calculate the local oscillator frequency (topocentric in Hertz). */
      f_lo = astGetDSBCentre( this ) + astGetIF( this );

/* Create a 1D WinMap which converts f_lsb to f_usb. */
      f_lsb_a = 0.0;
      f_lsb_b = 2*f_lo;
      f_usb_a = 2*f_lo;
      f_usb_b = 0.0;
      fmap = (AstMapping *) astWinMap( 1, &f_lsb_a, &f_lsb_b, &f_usb_a, &f_usb_b, "" );
      
/* Construct the Mapping: input to f_lsb, f_lsb to f_usb, f_usb to input */
      map1 = (AstMapping *) astCmpMap( tmap, fmap, 1, "" );
      astInvert( tmap );
      map2 = (AstMapping *) astCmpMap( map1, tmap, 1, "" );

/* Simplify */
      result = astSimplify( map2 );

/* Free resources */
      tmap = astAnnul( tmap );
      fmap = astAnnul( fmap );
      map1 = astAnnul( map1 );
      map2 = astAnnul( map2 );
   }

/* Return NULL if an error has occurred. */
   if( !astOK ) result = astAnnul( result );

/* Return the result. */
   return result;

}

/* Functions which access class attributes. */
/* ---------------------------------------- */
/* Implement member functions to access the attributes associated with
   this class using the macros defined for this purpose in the
   "object.h" file. For a description of each attribute, see the class
   interface (in the associated .h file). */

/*
*att++
*  Name:
*     ImagFreq

*  Purpose:
*     The image sideband equivalent of the rest frequency.

*  Type:
*     Public attribute.

*  Synopsis:
*     Floating point, read-only.

*  Description:
*     This is a read-only attribute giving the frequency which
*     corresponds to the rest frequency but is in the opposite sideband.

*     The value is calculated by first transforming the rest frequency
*     (given by the RestFreq attribute) from the standard of rest of the
*     source (given by the SourceVel and SourceVRF attributes) to the
*     standard of rest of the observer (i.e. the topocentric standard of
*     rest). The resulting topocentric frequency is assumed to be in the
*     same sideband as the value given for the DSBCentre attribute (the
*     "observed" sideband), and is transformed to the other sideband (the
*     "image" sideband). The new frequency is converted back to the standard
*     of rest of the source, and the resulting value is returned as the
*     attribute value, in units of GHz.

*  Applicability:
*     DSBSpecFrame
*        All DSBSpecFrames have this attribute.

*att--
*/


/*
*att++
*  Name:
*     DSBCentre

*  Purpose:
*     The central position of interest in a dual sideband spectrum.

*  Type:
*     Public attribute.

*  Synopsis:
*     Floating point.

*  Description:
*     This attribute specifies the central position of interest in a dual 
*     sideband spectrum. Its sole use is to determine the local oscillator 
*     frequency (the frequency which marks the boundary between the lower 
*     and upper sidebands). See the description of the IF (intermediate 
*     frequency) attribute for details of how the local oscillator frequency
*     is calculated. The sideband containing this central position is
*     referred to as the "observed" sideband, and the other sideband as 
*     the "image" sideband. 
*
*     The value is accessed as a position in the spectral system
*     represented by the SpecFrame attributes inherited by this class, but
*     is stored internally as topocentric frequency. Thus, if the System
*     attribute of the DSBSpecFrame is set to "VRAD", the Unit attribute
*     set to "m/s" and the StdOfRest attribute set to "LSRK", then values
*     for the DSBCentre attribute should be supplied as radio velocity in
*     units of "m/s" relative to the kinematic LSR (alternative units may
*     be used by appending a suitable units string to the end of the value). 
*     This value is then converted to topocentric frequency and stored. If 
*     (say) the Unit attribute is subsequently changed to "km/s" before 
*     retrieving the current value of the DSBCentre attribute, the stored 
*     topocentric frequency will be converted back to LSRK radio velocity, 
*     this time in units of "km/s", before being returned.
*
*     The default value for this attribute is 30 GHz.

*  Applicability:
*     DSBSpecFrame
*        All DSBSpecFrames have this attribute.

*  Note:
*     - The attributes which define the transformation to or from topocentric 
*     frequency should be assigned their correct values before accessing
*     this attribute. These potentially include System, Unit, StdOfRest, 
*     GeoLon, GeoLat, Epoch, RefRA, RefDec and RestFreq.

*att--
*/
/* The central frequency (topocentric frequency in Hz). */
astMAKE_CLEAR(DSBSpecFrame,DSBCentre,dsbcentre,AST__BAD)
astMAKE_GET(DSBSpecFrame,DSBCentre,double,3.0E10,((this->dsbcentre!=AST__BAD)?this->dsbcentre:3.0E10))
astMAKE_SET(DSBSpecFrame,DSBCentre,double,dsbcentre,value)
astMAKE_TEST(DSBSpecFrame,DSBCentre,( this->dsbcentre != AST__BAD ))


/*
*att++
*  Name:
*     IF

*  Purpose:
*     The intermediate frequency in a dual sideband spectrum.

*  Type:
*     Public attribute.

*  Synopsis:
*     Floating point.

*  Description:
*     This attribute specifies the (topocentric) intermediate frequency in 
*     a dual sideband spectrum. Its sole use is to determine the local 
*     oscillator (LO) frequency (the frequency which marks the boundary 
*     between the lower and upper sidebands). The LO frequency is
*     equal to the sum of the centre frequency and the intermediate
*     frequency. Here, the "centre frequency" is the topocentric
*     frequency in Hz corresponding to the current value of the DSBCentre
*     attribute. The value of the IF attribute may be positive or
*     negative: a positive value results in the LO frequency being above
*     the central frequency, whilst a negative IF value results in the LO
*     frequency being below the central frequency. The sign of the IF
*     attribute value determines the default value for the SideBand 
*     attribute.
*
*     When setting a new value for this attribute, the units in which the
*     frequency value is supplied may be indicated by appending a suitable 
*     string to the end of the formatted value. If the units are not 
*     specified, then the supplied value is assumed to be in units of GHz. 
*     For instance, the following strings all result in an IF of 4 GHz being 
*     used: "4.0", "4.0 GHz", "4.0E9 Hz", etc.
*
*     When getting the value of this attribute, the returned value is
*     always in units of GHz. The default value for this attribute is 4 GHz.

*  Applicability:
*     DSBSpecFrame
*        All DSBSpecFrames have this attribute.

*att--
*/
/* The intermediate frequency (topocentric in Hz). */
astMAKE_CLEAR(DSBSpecFrame,IF,ifr,AST__BAD)
astMAKE_GET(DSBSpecFrame,IF,double,4.0E9,((this->ifr!=AST__BAD)?this->ifr:4.0E9))
astMAKE_SET(DSBSpecFrame,IF,double,ifr,value)
astMAKE_TEST(DSBSpecFrame,IF,( this->ifr != AST__BAD ))

/*
*att++
*  Name:
*     SideBand

*  Purpose:
*     Indicates which sideband a dual sideband spectrum represents.

*  Type:
*     Public attribute.

*  Synopsis:
*     String.

*  Description:
*     This attribute indicates whether the DSBSpecFrame currently
*     represents its lower or upper sideband. When querying the current 
*     value, the returned string is always one of "usb" (for upper) or 
*     "lsb" (for lower). When setting a new value, any of the strings "lsb", 
*     "usb", "observed" or "image" may be supplied (case insensitive). The 
*     "observed" sideband is which ever sideband (upper or lower) contains 
*     the central spectral position given by attribute DSBCentre, and the 
*     "image" sideband is the other sideband. It is the sign of the IF
*     attribute which determines if the observed sideband is the upper or 
*     lower sideband. The default value for SideBand is the observed
*     sideband.

*  Applicability:
*     DSBSpecFrame
*        All DSBSpecFrames have this attribute.

*att--
*/
/* Protected access to the SideBand attribute uses non-zero value to
   indicate USB and zero to indicate LSB. */
astMAKE_CLEAR(DSBSpecFrame,SideBand,sideband,-9999)
astMAKE_SET(DSBSpecFrame,SideBand,int,sideband,(value?USB:LSB))
astMAKE_TEST(DSBSpecFrame,SideBand,( this->sideband != -9999 ))
astMAKE_GET(DSBSpecFrame,SideBand,int,USB,(this->sideband == -9999 ? 
((astGetIF( this )>0)?LSB:USB):this->sideband))

/* Copy constructor. */
/* ----------------- */
/* None needed */

/* Destructor. */
/* ----------- */
/* None needed */

/* Dump function. */
/* -------------- */
static void Dump( AstObject *this_object, AstChannel *channel ) {
/*
*  Name:
*     Dump

*  Purpose:
*     Dump function for DSBSpecFrame objects.

*  Type:
*     Private function.

*  Synopsis:
*     void Dump( AstObject *this, AstChannel *channel )

*  Description:
*     This function implements the Dump function which writes out data
*     for the DSBSpecFrame class to an output Channel.

*  Parameters:
*     this
*        Pointer to the DSBSpecFrame whose data are being written.
*     channel
*        Pointer to the Channel to which the data are being written.
*/

/* Local Variables: */
   AstDSBSpecFrame *this;        /* Pointer to the DSBSpecFrame structure */
   double dval;                  /* Attribute value */
   int ival;                     /* Attribute value */
   int set;                      /* Is attribute set? */

/* Check the global error status. */
   if ( !astOK ) return;

/* Obtain a pointer to the DSBSpecFrame structure. */
   this = (AstDSBSpecFrame *) this_object;

/* In the case of attributes, we first use the appropriate (private)
   Test...  member function to see if they are set. If so, we then use
   the (private) Get... function to obtain the value to be written
   out.

   For attributes which are not set, we use the astGet... method to
   obtain the value instead. This will supply a default value
   (possibly provided by a derived class which over-rides this method)
   which is more useful to a human reader as it corresponds to the
   actual default attribute value.  Since "set" will be zero, these
   values are for information only and will not be read back. */

/* DSBCentre */
/* --------- */
   set = TestDSBCentre( this );
   dval = set ? GetDSBCentre( this ) : astGetDSBCentre( this );
   astWriteDouble( channel, "DSBCen", set, 1, dval, "Central frequency (Hz topo)" );

/* IF */
/* -- */
   set = TestIF( this );
   dval = set ? GetIF( this ) : astGetIF( this );
   astWriteDouble( channel, "IF", set, 1, dval, "Intermediate frequency (Hz)" );

/* SideBand */
/* -------- */
   set = TestSideBand( this );
   ival = set ? GetSideBand( this ) : astGetSideBand( this );
   astWriteString( channel, "SideBn", set, 1, ival ? "USB" : "LSB", 
                   ival ? "Represents upper sideband" : 
                          "Represents lower sideband");
}

/* Standard class functions. */
/* ========================= */
/* Implement the astIsADSBSpecFrame and astCheckDSBSpecFrame functions using the macros
   defined for this purpose in the "object.h" header file. */
astMAKE_ISA(DSBSpecFrame,SpecFrame,check,&class_init)
astMAKE_CHECK(DSBSpecFrame)

AstDSBSpecFrame *astDSBSpecFrame_( const char *options, ... ) {
/*
*++
*  Name:
c     astDSBSpecFrame
f     AST_DSBSPECFRAME

*  Purpose:
*     Create a DSBSpecFrame.

*  Type:
*     Public function.

*  Synopsis:
c     #include "dsbspecframe.h"
c     AstDSBSpecFrame *astDSBSpecFrame( const char *options, ... )
f     RESULT = AST_DSBSPECFRAME( OPTIONS, STATUS )

*  Class Membership:
*     DSBSpecFrame constructor.

*  Description:
*     This function creates a new DSBSpecFrame and optionally initialises its
*     attributes.
*
*     A DSBSpecFrame is a specialised form of SpecFrame which represents 
*     positions in a spectrum obtained using a dual sideband instrument.
*     Such an instrument produces a spectrum in which each point contains 
*     contributions from two distinctly different frequencies, one from
*     the "lower side band" (LSB) and one from the "upper side band" (USB). 
*     Corresponding LSB and USB frequencies are connected by the fact
*     that they are an equal distance on either side of a fixed central
*     frequency known as the "Local Oscillator" (LO) frequency.
*
*     When quoting a position within such a spectrum, it is necessary to
*     indicate whether the quoted position is the USB position or the 
*     corresponding LSB position. The SideBand attribute provides this
*     indication.
*
*     In practice, the LO frequency is specified by giving the distance
*     from the LO frequency to some "central" spectral position. Typically 
*     this central position is that of some interesting spectral feature. 
*     The distance from this central position to the LO frequency is known 
*     as the "intermediate frequency" (IF). The value supplied for IF can
*     be a signed value in order to indicate whether the LO frequency is
*     above of below the central position.

*  Parameters:
c     options
f     OPTIONS = CHARACTER * ( * ) (Given)
c        Pointer to a null-terminated string containing an optional
c        comma-separated list of attribute assignments to be used for
c        initialising the new DSBSpecFrame. The syntax used is identical to
c        that for the astSet function and may include "printf" format
c        specifiers identified by "%" symbols in the normal way.
f        A character string containing an optional comma-separated
f        list of attribute assignments to be used for initialising the
f        new DSBSpecFrame. The syntax used is identical to that for the
f        AST_SET routine.
c     ...
c        If the "options" string contains "%" format specifiers, then
c        an optional list of additional arguments may follow it in
c        order to supply values to be substituted for these
c        specifiers. The rules for supplying these are identical to
c        those for the astSet function (and for the C "printf"
c        function).
f     STATUS = INTEGER (Given and Returned)
f        The global status.

*  Returned Value:
c     astDSBSpecFrame()
f     AST_DSBSPECFRAME = INTEGER
*        A pointer to the new DSBSpecFrame.

*  Notes:
*     - A null Object pointer (AST__NULL) will be returned if this
c     function is invoked with the AST error status set, or if it
f     function is invoked with STATUS set to an error value, or if it
*     should fail for any reason.
*--
*/

/* Local Variables: */
   AstDSBSpecFrame *new;        /* Pointer to new DSBSpecFrame */
   va_list args;                /* Variable argument list */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* Initialise the DSBSpecFrame, allocating memory and initialising the
   virtual function table as well if necessary. */
   new = astInitDSBSpecFrame( NULL, sizeof( AstDSBSpecFrame ), !class_init, &class_vtab,
                              "DSBSpecFrame" );

/* If successful, note that the virtual function table has been
   initialised. */
   if ( astOK ) {
      class_init = 1;

/* Obtain the variable argument list and pass it along with the options string
   to the astVSet method to initialise the new DSBSpecFrame's attributes. */
      va_start( args, options );
      astVSet( new, options, args );
      va_end( args );

/* If an error occurred, clean up by deleting the new object. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return a pointer to the new DSBSpecFrame. */
   return new;
}

AstDSBSpecFrame *astDSBSpecFrameId_( const char *options, ... ) {
/*
*  Name:
*     astDSBSpecFrameId_

*  Purpose:
*     Create a DSBSpecFrame.

*  Type:
*     Private function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     AstDSBSpecFrame *astDSBSpecFrameId_( const char *options, ... ) 

*  Class Membership:
*     DSBSpecFrame constructor.

*  Description:
*     This function implements the external (public) interface to the
*     astDSBSpecFrame constructor function. It returns an ID value (instead
*     of a true C pointer) to external users, and must be provided
*     because astDSBSpecFrame_ has a variable argument list which cannot be
*     encapsulated in a macro (where this conversion would otherwise
*     occur).
*
*     The variable argument list also prevents this function from
*     invoking astDSBSpecFrame_ directly, so it must be a re-implementation
*     of it in all respects, except for the final conversion of the
*     result to an ID value.

*  Parameters:
*     As for astDSBSpecFrame_.

*  Returned Value:
*     The ID value associated with the new DSBSpecFrame.
*/

/* Local Variables: */
   AstDSBSpecFrame *new;        /* Pointer to new DSBSpecFrame */
   va_list args;                /* Variable argument list */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* Initialise the DSBSpecFrame, allocating memory and initialising the
   virtual function table as well if necessary. */
   new = astInitDSBSpecFrame( NULL, sizeof( AstDSBSpecFrame ), !class_init, &class_vtab,
                              "DSBSpecFrame" );

/* If successful, note that the virtual function table has been
   initialised. */
   if ( astOK ) {
      class_init = 1;

/* Obtain the variable argument list and pass it along with the options string
   to the astVSet method to initialise the new DSBSpecFrame's attributes. */
      va_start( args, options );
      astVSet( new, options, args );
      va_end( args );

/* If an error occurred, clean up by deleting the new object. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return an ID value for the new DSBSpecFrame. */
   return astMakeId( new );
}

AstDSBSpecFrame *astInitDSBSpecFrame_( void *mem, size_t size, int init,
                                       AstDSBSpecFrameVtab *vtab, const char *name ) {
/*
*+
*  Name:
*     astInitDSBSpecFrame

*  Purpose:
*     Initialise a DSBSpecFrame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     AstDSBSpecFrame *astInitDSBSpecFrame( void *mem, size_t size, int init,
*                               AstDSBSpecFrameVtab *vtab, const char *name )

*  Class Membership:
*     DSBSpecFrame initialiser.

*  Description:
*     This function is provided for use by class implementations to initialise
*     a new DSBSpecFrame object. It allocates memory (if necessary) to accommodate
*     the DSBSpecFrame plus any additional data associated with the derived class.
*     It then initialises a DSBSpecFrame structure at the start of this memory. If
*     the "init" flag is set, it also initialises the contents of a virtual
*     function table for a DSBSpecFrame at the start of the memory passed via the
*     "vtab" parameter.

*  Parameters:
*     mem
*        A pointer to the memory in which the DSBSpecFrame is to be initialised.
*        This must be of sufficient size to accommodate the DSBSpecFrame data
*        (sizeof(DSBSpecFrame)) plus any data used by the derived class. If a value
*        of NULL is given, this function will allocate the memory itself using
*        the "size" parameter to determine its size.
*     size
*        The amount of memory used by the DSBSpecFrame (plus derived class data).
*        This will be used to allocate memory if a value of NULL is given for
*        the "mem" parameter. This value is also stored in the DSBSpecFrame
*        structure, so a valid value must be supplied even if not required for
*        allocating memory.
*     init
*        A logical flag indicating if the DSBSpecFrame's virtual function table is
*        to be initialised. If this value is non-zero, the virtual function
*        table will be initialised by this function.
*     vtab
*        Pointer to the start of the virtual function table to be associated
*        with the new DSBSpecFrame.
*     name
*        Pointer to a constant null-terminated character string which contains
*        the name of the class to which the new object belongs (it is this
*        pointer value that will subsequently be returned by the astGetClass
*        method).

*  Returned Value:
*     A pointer to the new DSBSpecFrame.

*  Notes:
*     -  A null pointer will be returned if this function is invoked with the
*     global error status set, or if it should fail for any reason.
*-
*/

/* Local Variables: */
   AstDSBSpecFrame *new;        /* Pointer to new DSBSpecFrame */

/* Check the global status. */
   if ( !astOK ) return NULL;

/* If necessary, initialise the virtual function table. */
   if ( init ) astInitDSBSpecFrameVtab( vtab, name );

/* Initialise. */
   new = NULL;

/* Initialise a SpecFrame structure (the parent class) as the first component
   within the DSBSpecFrame structure, allocating memory if necessary. Specify that
   the SpecFrame should be defined in both the forward and inverse directions. */
   new = (AstDSBSpecFrame *) astInitSpecFrame( mem, size, 0,
                                       (AstSpecFrameVtab *) vtab, name );
   if ( astOK ) {

/* Initialise the DSBSpecFrame data. */
/* --------------------------------- */
      new->dsbcentre = AST__BAD;
      new->ifr = AST__BAD;
      new->sideband = -9999;

/* If an error occurred, clean up by deleting the new DSBSpecFrame. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return a pointer to the new DSBSpecFrame. */
   return new;
}

AstDSBSpecFrame *astLoadDSBSpecFrame_( void *mem, size_t size,
                           AstDSBSpecFrameVtab *vtab, const char *name,
                           AstChannel *channel ) {
/*
*+
*  Name:
*     astLoadDSBSpecFrame

*  Purpose:
*     Load a DSBSpecFrame.

*  Type:
*     Protected function.

*  Synopsis:
*     #include "dsbspecframe.h"
*     AstDSBSpecFrame *astLoadDSBSpecFrame( void *mem, size_t size,
*                               AstDSBSpecFrameVtab *vtab, const char *name,
*                               AstChannel *channel )

*  Class Membership:
*     DSBSpecFrame loader.

*  Description:
*     This function is provided to load a new DSBSpecFrame using data read
*     from a Channel. It first loads the data used by the parent class
*     (which allocates memory if necessary) and then initialises a
*     DSBSpecFrame structure in this memory, using data read from the input
*     Channel.
*
*     If the "init" flag is set, it also initialises the contents of a
*     virtual function table for a DSBSpecFrame at the start of the memory
*     passed via the "vtab" parameter.


*  Parameters:
*     mem
*        A pointer to the memory into which the DSBSpecFrame is to be
*        loaded.  This must be of sufficient size to accommodate the
*        DSBSpecFrame data (sizeof(DSBSpecFrame)) plus any data used by derived
*        classes. If a value of NULL is given, this function will
*        allocate the memory itself using the "size" parameter to
*        determine its size.
*     size
*        The amount of memory used by the DSBSpecFrame (plus derived class
*        data).  This will be used to allocate memory if a value of
*        NULL is given for the "mem" parameter. This value is also
*        stored in the DSBSpecFrame structure, so a valid value must be
*        supplied even if not required for allocating memory.
*
*        If the "vtab" parameter is NULL, the "size" value is ignored
*        and sizeof(AstDSBSpecFrame) is used instead.
*     vtab
*        Pointer to the start of the virtual function table to be
*        associated with the new DSBSpecFrame. If this is NULL, a pointer
*        to the (static) virtual function table for the DSBSpecFrame class
*        is used instead.
*     name
*        Pointer to a constant null-terminated character string which
*        contains the name of the class to which the new object
*        belongs (it is this pointer value that will subsequently be
*        returned by the astGetClass method).
*
*        If the "vtab" parameter is NULL, the "name" value is ignored
*        and a pointer to the string "DSBSpecFrame" is used instead.

*  Returned Value:
*     A pointer to the new DSBSpecFrame.

*  Notes:
*     - A null pointer will be returned if this function is invoked
*     with the global error status set, or if it should fail for any
*     reason.
*-
*/

/* Local Constants. */
#define KEY_LEN 50               /* Maximum length of a keyword */

/* Local Variables: */
   AstDSBSpecFrame *new;         /* Pointer to the new DSBSpecFrame */
   char *text;                   /* Pointer to string value */

/* Initialise. */
   new = NULL;

/* Check the global error status. */
   if ( !astOK ) return new;

/* If a NULL virtual function table has been supplied, then this is
   the first loader to be invoked for this DSBSpecFrame. In this case the
   DSBSpecFrame belongs to this class, so supply appropriate values to be
   passed to the parent class loader (and its parent, etc.). */
   if ( !vtab ) {
      size = sizeof( AstDSBSpecFrame );
      vtab = &class_vtab;
      name = "DSBSpecFrame";

/* If required, initialise the virtual function table for this class. */
      if ( !class_init ) {
         astInitDSBSpecFrameVtab( vtab, name );
         class_init = 1;
      }
   }

/* Invoke the parent class loader to load data for all the ancestral
   classes of the current one, returning a pointer to the resulting
   partly-built DSBSpecFrame. */
   new = astLoadSpecFrame( mem, size, (AstSpecFrameVtab *) vtab, name,
                           channel );
   if ( astOK ) {

/* Read input data. */
/* ================ */
/* Request the input Channel to read all the input data appropriate to
   this class into the internal "values list". */
       astReadClassData( channel, "DSBSpecFrame" );

/* Now read each individual data item from this list and use it to
   initialise the appropriate instance variable(s) for this class. */

/* In the case of attributes, we first read the "raw" input value,
   supplying the "unset" value as the default. If a "set" value is
   obtained, we then use the appropriate (private) Set... member
   function to validate and set the value properly. */

/* DSBCentre */
/* --------- */
      new->dsbcentre = astReadDouble( channel, "dsbcen", AST__BAD );
      if ( TestDSBCentre( new ) ) SetDSBCentre( new, new->dsbcentre );

/* IF */
/* -- */
      new->ifr = astReadDouble( channel, "if", AST__BAD );
      if ( TestIF( new ) ) SetIF( new, new->ifr );

/* SideBand */
/* -------- */
      text = astReadString( channel, "sidebn", " " );
      if( astOK ) {
         if( !strcmp( text, " " ) ) {
            new->sideband = -9999;
         } else if( !strcmp( text, "USB" ) ) {
            new->sideband = USB;
         } else if( !strcmp( text, "LSB" ) ) {
            new->sideband = LSB;
         } else {
            astError( AST__ATTIN, "astRead(%s): Invalid SideBand description "
                      "\"%s\".", astGetClass( channel ), text );
         }
         if ( TestSideBand( new ) ) SetSideBand( new, new->sideband );
      }

/* If an error occurred, clean up by deleting the new DSBSpecFrame. */
      if ( !astOK ) new = astDelete( new );
   }

/* Return the new DSBSpecFrame pointer. */
   return new;
}

/* Virtual function interfaces. */
/* ============================ */
/* These provide the external interface to the virtual functions defined by
   this class. Each simply checks the global error status and then locates and
   executes the appropriate member function, using the function pointer stored
   in the object's virtual function table (this pointer is located using the
   astMEMBER macro defined in "object.h").

   Note that the member function may not be the one defined here, as it may
   have been over-ridden by a derived class. However, it should still have the
   same interface. */

double astGetImagFreq_( AstDSBSpecFrame *this ) {
   if ( !astOK ) return AST__BAD;
   return (**astMEMBER(this,DSBSpecFrame,GetImagFreq))( this );
}

