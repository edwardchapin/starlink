dnl    Process this file with autoconf to produce a configure script
AC_REVISION($Revision$)
  
dnl    Initialisation: package name and version number
AC_INIT([one],[1.5-1],[starlink@jiscmail.ac.uk])
AC_CONFIG_AUX_DIR([build-aux])
  
dnl    Require autoconf-2.50 at least
AC_PREREQ([2.69])
dnl    Require Starlink automake
AM_INIT_AUTOMAKE(1.8.2-starlink)
  
dnl    Sanity-check: name a file in the source directory -- if this
dnl    isn't found then configure will complain
AC_CONFIG_SRCDIR([one_err.msg])
 
  
dnl    Include defaults for Starlink configurations
STAR_DEFAULTS
  
dnl    Find required versions of the programs we need for configuration
AC_PROG_CC
AC_PROG_FC
LT_INIT

dnl    Check if sys/wait.h exists and is POSIX compatible
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_FUNCS(memcpy)
AC_CHECK_FUNCS(isspace)
AC_CHECK_FUNCS(bcopy)
AC_CHECK_FUNCS(strlcpy)
AC_CHECK_FUNCS(strlcat)

STAR_CNF_COMPATIBLE_SYMBOLS
 
dnl    Declare the build and use dependencies for this package
STAR_DECLARE_DEPENDENCIES([build], [sae ems cnf prm starutil])
STAR_DECLARE_DEPENDENCIES([link],  [ems cnf psx starutil])
 
dnl    List the sun/ssn/... numbers which document this package and
dnl    which are present as .tex files in this directory.
STAR_LATEX_DOCUMENTATION(sun234)
 
STAR_MESSGEN(one_err.msg)
 
 
dnl    If you wish to configure extra files, you can add them to this
dnl    declaration.
AC_CONFIG_FILES(Makefile component.xml)
AC_CONFIG_HEADERS( config.h )

dnl    This is the bit that does the actual work
AC_OUTPUT
