# $Id$
#
# gaiaInit.tcl - This script generates a mktclapp config file on stdout.
#
# who             when       what
# --------------  ---------  ----------------------------------------
# Peter W. Draper 12 May 99  Created, based on skyCatInit.tcl
proc main {} {
   global argv argc env argv0
   
   if {$argc != 1} {
      puts "usage: gaiaInit.tcl output_file_name"
      exit 1
   }
   set outfile [lindex $argv 0]
   set fd [open $outfile w]
   
   #  Set names of source directories.
   set tcl_library      /home/pdraper/gaia/top/built/lib/tcl8.0
   set tk_library       /home/pdraper/gaia/top/built/lib/tk8.0
   set itcl_library     /home/pdraper/gaia/top/built/lib/itcl3.0
   set itk_library      /home/pdraper/gaia/top/built/lib/itk3.0
   set tclutil_library  /home/pdraper/gaia/top/built/lib/tclutil
   set astrotcl_library /home/pdraper/gaia/top/built/lib/astrotcl
   set cat_library      /home/pdraper/gaia/top/built/lib/cat
   set rtd_library      /home/pdraper/gaia/top/built/lib/rtd
   set skycat_library   /home/pdraper/gaia/top/built/lib/skycat
   set gaia_library     /home/pdraper/gaia/top/built/lib/gaia
   set blt_library      /home/pdraper/gaia/top/built/lib/blt2.4
   set iwidgets_library /home/pdraper/gaia/top/built/lib/iwidgets3.0.0
   set iscripts_library /home/pdraper/gaia/top/built/lib/iwidgets3.0.0/scripts
   set tclx_library     /home/pdraper/gaia/top/built/lib/tclX8.0.3
   
    set srcdirs [list \
		     $tcl_library \
		     $tk_library \
		     $itcl_library \
		     $itk_library \
		     $blt_library \
		     $tclx_library \
		     $iwidgets_library \
		     $iscripts_library \
		     $tclutil_library \
		     $astrotcl_library \
		     $rtd_library \
		     $cat_library \
		     $skycat_library \
		     $gaia_library]
   
   #  Set location of startup script.
   puts $fd "-main-script \"${gaia_library}/gaiaMain.tcl\""

   #  Binary is standalone.
   puts $fd "-standalone"
   
   #  Set location of Tcl and Tk.
   puts $fd "-tcl-library \"$tcl_library\""
   puts $fd "-tk-library \"$tk_library\""

   #  Name of the Et_AppInit file.
   puts $fd "EtAppInit.C"
   
   #  Now add all the Tcl file from everywhere.
   foreach dir $srcdirs {
      if {[file exists $dir/tclIndex]} {
         puts $fd "-dont-strip-tcl \"$dir/tclIndex\""
      }
      set tclfiles [glob -nocomplain $dir/*.tcl $dir/*.itk $dir/*.itcl]
      foreach file "$tclfiles" {
         puts $fd "-dont-strip-tcl \"$file\""
      }
   }
   
   #  Add special TclX file.
   if {[info exists tclx_library] && [file exists $tclx_library/tcl.tlib]} {
      puts $fd "-dont-strip-tcl \"$tclx_library/tcl.tlib\""
   } else {
      puts "$argv0: can't find TclX library files"
      exit 1
   }
   
   #  BLT has some special files.
   if {[info exists blt_library] && [file exists $blt_library/bltGraph.pro]} {
      set profiles [glob -nocomplain $blt_library/*.pro]
      foreach file "$profiles" {
         puts $fd "-dont-strip-tcl \"$file\""
      }
   } else {
      puts "$argv0: can't find BLT library files"
      exit 1
   }
}

main
exit

