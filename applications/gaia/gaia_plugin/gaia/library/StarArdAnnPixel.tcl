#+
#  Name:
#     StarArdAnnPixel

#  Type of Module:
#     [incr Tcl] class

#  Purpose:
#     Defines a class of ARD Pixel with an annulus of the same shape.

#  Description:
#     This class creates an ARD canvas Pixel item and draws an
#     annulus at the given scale factor about it. The annulus is
#     updated with the ARD Pixel.

#  Invocations:
#
#        StarArdAnnPixel object_name [configuration options]
#
#     This creates an instance of a StarArdAnnPixel object. The return is
#     the name of the object.
#
#        object_name configure -configuration_options value
#
#     Applies any of the configuration options (after the instance has
#     been created).
#
#        object_name method arguments
#
#     Performs the given method on this object.

#  Configuration options:
#
#        -scale factor
#
#     The factor by which the annulus is larger than the ordinary ARD
#     Pixel.

#  Configuration options:
#     See public variable defintions.

#  Methods:
#     See method definitions.

#  Inheritance:
#     This object inherits StarArdPixel.

#  Copyright:
#     Copyright (C) 1998 Central Laboratory of the Research Councils

#  Authors:
#     PDRAPER: Peter Draper (STARLINK - Durham University)
#     {enter_new_authors_here}

#  History:
#     21-JUN-1996 (PDRAPER):
#        Original version.
#     20-AUG-1996 (PDRAPER):
#        Converted to itcl2.0.
#     {enter_further_changes_here}

#-

#.

class gaia::StarArdAnnPixel {

   #  Inheritances:
   #  -------------

   inherit StarArdPixel

   #  Constructor:
   #  ------------
   constructor {args} {
      set notify_created_cmd_ [code $this create_first_annulus_]
      set notify_update_cmd_  [code $this redraw_annulus_]
      eval configure $args
   }

   #  Destructor:
   #  -----------
   destructor  {
      if { $annulus_id_ != {} } {
         $canvas delete $annulus_id_
      }
   }

   #  Methods:
   #  --------

   #  Create the annulus from an interactive call.
   private method create_first_annulus_ {args} {
      set created_ 1
      set notify_created_cmd_ {}
      create_annulus_
   }

   #  Create the annulus.
   private method create_annulus_ {args} {
      if { $show_annulus } { 
         set annulus_id_ [eval $canvas create rectangle $coords \
                             -outline $deselected_colour]
         redraw_annulus_
         $canvas addtag $annulus_tag withtag $annulus_id_
      }
   }

   #  Redraw the annulus. Note we need to correct for image scale
   #  factor (pixel is an image pixel not a canvas pixel).
   private method redraw_annulus_ {} {
      if { $annulus_id_ != {} } {
         lassign $coords x1 y1 x2 y2
         lassign [$rtdimage scale] xs ys
         set realscale [expr $scale*$xs]
         set xr1 [expr $x1-$realscale]
         set yr1 [expr $y1-$realscale]
         set xr2 [expr $x1+$realscale]
         set yr2 [expr $y1+$realscale]
         $canvas coords $annulus_id_ $xr1 $yr1 $xr2 $yr2
      }
   }

   #  Return ARD description of annulus. Uses the getard method with
   #  falsified coords.
   public method getann {} {
      if { $annulus_id_ != {} } { 
         set oldcoords $coords
         set coords [$canvas coords $annulus_id_]
         set ard [getard 0]
         set coords $oldcoords
         return $ard
      }
   }

   #  Configuration options: (public variables)
   #  ----------------------

   #  The annulus scaling factor.
   public variable scale {5} {
      if { $annulus_id_ != {} } {
         redraw_annulus_
      }
   }

   #  Whether to show annulus or not.
   public variable show_annulus {1} {
      if { $show_annulus } {
         if { $annulus_id_ == {} && $created_ } {
            create_annulus_
            set notify_update_cmd_  [code $this redraw_annulus_]
         }
      } else {
         if { $annulus_id_ != {} } {
            $canvas delete $annulus_id_
            set annulus_id_ {}
            set notify_update_cmd_  {}
         }
      }
   }

   #  Canvas tag to add this object to.
   public variable annulus_tag {ArdPixel} {}

   #  Protected variables: (available to instance)
   #  --------------------

   #  Canvas id of annulus.
   protected variable annulus_id_ {}

   #  Whether annulus object creation has been invoked interactively
   #  or not. In which case show_annulus will change the state.
   protected variable created_ 0

   #  Common variables: (shared by all instances)
   #  -----------------


#  End of class definition.
}
