/* IMPORTANT: Read the comment below on how to compile this file.  It
 * is NOT sufficient to just hand it to the C compiler.  It must first
 * be preprocessed.
 */
/*
 * This file has been generated by
 *
 *                 D. Richard Hipp
 *                 drh@world.std.com
 *
 * The contributions of Dr. Hipp are placed in the public domain.  However,
 * portions of this file are subject to the copyrights shown below.
 *
 * The changes necessary to use TclX are by Michael Schumacher at
 * mike@hightec.saarlink.de.
 *
 * Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
/*
**
** The function Et_Eval() is an extension of Tcl_VarEval().
**
** The first and second arguments are the name of the file which
** contains the call to Et_Eval() and the line number within that
** file from which Et_Eval() was called.  This information is used
** to print an error message to stderr if necessary.
**
** The third argument is a format string built from three characters:
** 's', 'd', and 'f'.  This format string determines the number and
** type of subsequent arguments.  Each 's' in the format string
** corresponds to a string argument, each 'd' cooresponds to an integer,
** and each 'f' corresponds to a double.
**
** A format string may now also contain a 'q' character.  'q' means that
** the any of the characters which have special meaning to the Tcl
** interpreter (any of  "{", "}", "[", "\"", "\\" or "$") are escaped.
**
** The operation of Et_Eval() is to concatenate all arguments and
** send the result to Tcl_GlobalEval().  The return of Tcl_GlobalEval()
** is returned from Et_Eval().
**
** In addition to Et_Eval(), this file defines three other functions:
**
**      Et_EvalInt()
**      Et_EvalString()
**      Et_EvalDouble()
**
** In each of these other functions, the value returned is not the status
** string from Tcl_VarEval(), but rather the result string.  The result
** string is converted to an integer or a double for Et_EvalInt() and
** Et_EvalDouble(), respectively.
**
**      Et_EvalInclude()
**
** This function is used to implement the ET_INCLUDE() macro of the ET
** system.  It takes two arguments which are the name of a file and
** null-terminated string containing the contents of that file.  The
** the function simply calls Tcl_GlobalEval on the string, and then prints
** an appropriate error message if something goes wrong.
**
** The Tcl/Tk interpreter used by all these functions is obtained from
** the global variable "Tcl_Interp *Et_Interp;".  This variable
** should be set to a valid interpreter before calling any function
** in this file.
**
** The function Et_InstallCmd() creates a new command for the
** Tcl/Tk interpreter pointed to by the global variable Et_Interp.
** This function was added in support of the ET_INSTALL_COMMANDS macro.
**
** All of the above functions are normally called from ET macros, not
** directly by the user.  The next set of functions are called directly
** by the user:
**
**     Et_Init(int *,char **);
**     Et_ReadStdin(void);
**     Et_MainLoop(void);
**
** The Et_Init() function should be the first function called from within
** the main() procedure of an application.  This function initializes the
** ET interpreter.  Command line arguments used by the interpreter are
** removed from  argc and argv.
**
** Et_MainLoop() should be the last function in the main() procedure.  It
** implements the event loop.
**
** If the function Et_ReadStdin() is called prior to Et_MainLoop(), then
** arrangements are made to read standard input and pass the results to
** the ET interpreter.
**
********************* HOW TO COMPILE THIS FILE **************************
**
** This file is mostly C code, but it is not completely C code.  It can
** not be handed to the C compiler directory, but must first be preprocessed
** using the "et2c" macro preprocessors.
**
** This version of the ET library file is for use with Tk version 8.0 and
** Tcl version 8.0.
*/

#include <tk.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

/*
** The following variable points to the Tcl/Tk interpreter which is
** used by all functions in this file.
*/
Tcl_Interp *Et_Interp = 0;

/* The next variables are made available as a convenience to the
** ET programmer. */
Tk_Window Et_MainWindow;      /* The main window of the application */
Display *Et_Display;          /* The X11 display holding the main window */

/* This flag is TRUE if all scripts processed by ET() macros should
** be printed to standard output. This variable can only be set from
** within the debugger. */
static int et_trace_flag = 0;

/*
 * Estimate space needed to expand a command using formatted varargs.
 */
static int Et_EstCmdLen( const char *format, va_list ap )
{
    int i;                     /* Loop counter */
    int spaceNeeded;           /* Amount of spaced needed to hold
                                * the entire Tcl command string */
    char *cp;                  /* Pointer to quoted string */
    char c;                    /* Next character un quoted string */

    /* Make a pass thru the argument list and tally up the size of the
     * buffer needed */
    spaceNeeded = 1;   /* One for the null-terminator */
    for ( i = 0; format[i]; i++ ) {
        switch( format[i] ){
        case 's':    /* A string */
            spaceNeeded += strlen(va_arg(ap,char*));
            break;

        case 'd':    /* An integer */
            spaceNeeded += 20;
            (void)va_arg(ap,int);
            break;

        case 'f':    /* A double */
            spaceNeeded += 20;
            (void)va_arg(ap,double);
            break;

        case 'q':    /* A quoted string */
            cp = va_arg(ap,char*);
            while( (c=*cp++)!=0 ){
                if( c=='$' || c=='[' || c=='\"' || c=='\\' ||
                    c=='{' || c=='}' ){
                    spaceNeeded++;
                }
                spaceNeeded++;
            }
            break;
        }
    }
    return spaceNeeded;
}


/*
** This is the function which does most of the work.  All the Et_EvalXXXX()
** functions call this core to process the variable-length argument list,
** and then do different things with the return parameter
*/
static int Et_VaEval(
   const char *fileName,      /* Name of the file containing the ET macro */
   int lineNumber,            /* Line number of start of ET macro */
   int spaceNeeded,           /* Buffer size needed to expand Tcl command */
   const char *format,        /* Format string */
   va_list ap                 /* Pointer to the argument list */
)
{
  int i;                     /* Loop counter */
  int retc;                  /* Return code from Tcl_Eval() */
  char *buf;                 /* Buffer to hold the Tcl command string  */
  char *cp;                  /* For scanning strings */
  char c;                    /* Next character being scanned */
  char *next;                /* Pointer to the next unused slot in buf[] */
  char smallSpace[40];       /* A space large enough to hold the ascii
                             ** value of an integer or floating point number */
  char bigSpace[1000];       /* The initial buffer.  Hopefully this is enough,
                             ** but if not we can malloc() for more */

  if( Et_Interp==0 ){
    fprintf(stderr,
      "ERROR: %s line %d: Et_Interp does not point to a valid Tk interpreter\n",
      fileName,lineNumber);
    return TCL_ERROR;
  }

  /* Get a buf which is large enough to hold it all */
  if( spaceNeeded<=sizeof(bigSpace) ){
    buf = bigSpace;
  }else{
    extern char *malloc();
    buf = malloc( spaceNeeded );
    if( buf==0 ){
       sprintf(bigSpace,"File %s line %d: can't allocate %d bytes of memory",
          fileName,lineNumber,spaceNeeded);
       Tcl_AppendResult(Et_Interp,bigSpace,0);
       return TCL_ERROR;
    }
  }

  /* Now build the Tcl command string */
  next = buf;
  for(i=0; format[i]; i++){
    char *str;       /* Pointer to a source string which is to be
                     ** copied into buf */
    switch( format[i] ){
      case 's':    /* A string */
        str = va_arg(ap,char*);
        break;

      case 'd':    /* An integer */
        str = smallSpace;
        sprintf(str,"%d",va_arg(ap,int));
        break;

      case 'f':    /* A double */
        str = smallSpace;
        sprintf(str,"%g",va_arg(ap,double));
        break;

      case 'q':    /* A quoted string */
        str = va_arg(ap,char*);
        while( (c=*str)!=0 ){
          if( c=='$' || c=='[' || c=='\"' || c=='\\' || c=='{' || c=='}' ){
            *next++ = '\\';
          }
          *next++ = c;
          str++;
        }
        break;

      default:     /* Can't happen */
        str = "";
        break;
    }
    while( (*next++ = *str++)!=0 );
    next--;
  }

  /* Evaluate the Tcl command string.  Generate an error message
  ** if an error occurs */
  retc = Tcl_GlobalEval(Et_Interp,buf);
  if( et_trace_flag ){
    char *cp = buf;
    while( *cp ){
      if( *cp=='\\' && cp[1]=='\n' ){ *cp=' '; cp[1]=' '; }
      cp++;
    }
    printf("%s\n",buf);
  }
  if( retc==TCL_ERROR ){
    char buf[40];
    fprintf(stderr,"ERROR at %s line %d: %s\n",
       fileName,Et_Interp->errorLine + lineNumber - 1,Et_Interp->result);
    sprintf(buf,"%d",Et_Interp->errorLine + lineNumber + 1);
    Tcl_VarEval(Et_Interp,"tkerror {",fileName," line ",buf,"}",0);
  }

  /* clean up and return */
  if( buf!=bigSpace ) free(buf);
  return retc;
}

/*
** This version of Et_Eval() evaluates a single string as the Tcl command,
** just as Tcl_Eval() would.  The difference is in the processing of
** error message.  This function is called by the ET_INCLUDE() macro.
** Et_Eval() could have been used, but this version avoids an extra
** copy of the script from one buffer into another.
*/
void Et_EvalInclude(
   const char *zFilename, /* The file from which the script was 
                             originally taken */
   char *script           /* The Tcl/Tk command script */
)
{
  if( Et_Interp==0 ){
    fprintf(stderr,
      "ERROR: %s: Et_Interp does not point to a valid Tk interpreter\n",
      zFilename);
    return;
  }
  if( Tcl_GlobalEval(Et_Interp,script)==TCL_ERROR ){
    char buf[40];
    fprintf(stderr,"ERROR at %s line %d: %s\n",
       zFilename,Et_Interp->errorLine,Et_Interp->result);
    sprintf(buf,"%d",Et_Interp->errorLine);
    Tcl_VarEval(Et_Interp,"tkerror {",zFilename," line ",buf,"}",0);
  }
}

/*
** Execute the Tcl/Tk command string and return an status code
*/
int Et_Eval(const char *filename, int lineNumber, const char *format, ...){
  va_list ap;
  int retc;
  int spaceNeeded;

  va_start(ap,format);
  spaceNeeded=Et_EstCmdLen(format,ap);
  va_end(ap);

  va_start(ap,format);
  retc = Et_VaEval(filename,lineNumber,spaceNeeded,format,ap);
  va_end(ap);
  return retc;
}

/*
** Execute the Tcl/Tk command string and return the result string
*/
char *
Et_EvalString(const char *filename, int lineNumber, const char *format, ...){
  va_list ap;
  int status;
  int spaceNeeded;

  va_start(ap,format);
  spaceNeeded=Et_EstCmdLen(format,ap);
  va_end(ap);

  va_start(ap,format);
  status = Et_VaEval(filename,lineNumber,spaceNeeded,format,ap);
  va_end(ap);
  return Et_Interp->result;
}

/*
** Execute the Tcl/Tk command string and return the result converted
** into an integer
*/
int
Et_EvalInt(const char *filename, int lineNumber, const char *format, ...){
  va_list ap;
  int status;
  int spaceNeeded;

  va_start(ap,format);
  spaceNeeded=Et_EstCmdLen(format,ap);
  va_end(ap);

  va_start(ap,format);
  status = Et_VaEval(filename,lineNumber,spaceNeeded,format,ap);
  va_end(ap);
  return atoi(Et_Interp->result);
}

/*
** Execute the Tcl/Tk command string and return the result converted
** into an double
*/
double
Et_EvalDouble(const char *filename, int lineNumber, const char *format, ...){
  va_list ap;
  int status;
  int spaceNeeded;

  va_start(ap,format);
  spaceNeeded=Et_EstCmdLen(format,ap);
  va_end(ap);

  va_start(ap,format);
  status = Et_VaEval(filename,lineNumber,spaceNeeded,format,ap);
  va_end(ap);
  return atof(Et_Interp->result);
}

/*
** Install a new command into the interpreter.
*/
void
Et_InstallCommand(
  char *zName,
  int (*cmdProc)(void*,struct Tcl_Interp*,int,char**)
){
  Tcl_CreateCommand(Et_Interp,zName,cmdProc,Tk_MainWindow(Et_Interp),0);
}

/*
** Initialize the Tcl/Tk interpreter.
**
** This is a highly modified version of "main()" from the
** file "tkMain.c" in the standard Tcl/Tk distribution.
*/
void
Et_Init(int *pargc, char **argv)
{
    char *args;

    if( getenv("DISPLAY")==0 ){
      fprintf(stderr,"%s: DISPLAY environment variable is not set.\n",argv[0]);
      exit(1);
    }
    Tcl_FindExecutable(argv[0]);
    Et_Interp = Tcl_CreateInterp();

    /*
     * Make command-line arguments available in the Tcl variables "argc"
     * and "argv".
     */

    args = Tcl_Merge(*pargc-1, argv+1);
    Tcl_SetVar(Et_Interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    ET( set argc %d(*pargc-1) );
    ET( set argv0 "%q(argv[0])" );

    /*
     * Set the "tcl_interactive" variable.
     */
    ET( set tcl_interactive %d(isatty(0)) );

    /*
     * Invoke application-specific initialization.
     */
    Tk_Init(Et_Interp);
    Tcl_StaticPackage(Et_Interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);

    /* Set the et_trace_flag */
    Tcl_LinkVar(Et_Interp, "et_trace_flag", (char*)&et_trace_flag,TCL_LINK_INT);

    /* Set up some convenience global variables.
    **
    ** This only works if there is a single display and screen.  But, then
    ** again, ET only works with a single screen...
    */
    Et_MainWindow = Tk_MainWindow(Et_Interp);
    Et_Display = Tk_Display(Et_MainWindow);

    /*
     * Create variables "cmd_name" and "cmd_dir" which contain the
     * base name of the application and the name of the directory
     * which contains the executable for the application.
     */
    ET(
      set cmd_name [file tail $argv0]
      set cmd_dir [file dirname $argv0]
      if "![file readable $argv0] || ![file isfile $argv0]" {
        foreach i [split $env(PATH) :] {
          if "[file readable $i/$cmd_name] && [file isfile $i/$cmd_name]" {
            set cmd_dir $i
            break;
          }
        }
      }
    );
    TkPlatformInit(Et_Interp);
}

/* Platform-specific initialization
*/
int TkPlatformInit( Tcl_Interp *interp )
{
  extern void TkCreateXEventSource();
  TkCreateXEventSource();

 /* Load the Tcl/Tk startup scripts.
  */
  ET( set tk_library {}; set tcl_library {} );
  ET_INCLUDE( init.tcl );

  /* The native "tk.tcl" file won't work in ET.  The following is a
  ** substitute. */
  ET(
# tk.tcl --
#
# Initialization script normally executed in the interpreter for each
# Tk-based application.  Arranges class bindings for widgets.
#
# SCCS: @(#) tk.tcl 1.98 97/10/28 15:21:04
#
# Copyright (c) 1992-1994 The Regents of the University of California.
# Copyright (c) 1994-1996 Sun Microsystems, Inc.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.

# Insist on running with compatible versions of Tcl and Tk.

package require -exact Tk 8.0
package require -exact Tcl 8.0

# Add Tk's directory to the end of the auto-load search path, if it
# isn't already on the path:

if {[info exists auto_path]} {
    if {[lsearch -exact $auto_path $tk_library] < 0} {
        lappend auto_path $tk_library
    }
}

# Turn off strict Motif look and feel as a default.

set tk_strictMotif 0

# tkScreenChanged --
# This procedure is invoked by the binding mechanism whenever the
# "current" screen is changing.  The procedure does two things.
# First, it uses "upvar" to make global variable "tkPriv" point at an
# array variable that holds state for the current display.  Second,
# it initializes the array if it didn't already exist.
#
# Arguments:
# screen -              The name of the new screen.

proc tkScreenChanged screen {
    set x [string last . $screen]
    if {$x > 0} {
        set disp [string range $screen 0 [expr {$x - 1}]]
    } else {
        set disp $screen
    }

    uplevel #0 upvar #0 tkPriv.$disp tkPriv
    global tkPriv
    global tcl_platform

    if {[info exists tkPriv]} {
        set tkPriv(screen) $screen
        return
    }
    set tkPriv(activeMenu) {}
    set tkPriv(activeItem) {}
    set tkPriv(afterId) {}
    set tkPriv(buttons) 0
    set tkPriv(buttonWindow) {}
    set tkPriv(dragging) 0
    set tkPriv(focus) {}
    set tkPriv(grab) {}
    set tkPriv(initPos) {}
    set tkPriv(inMenubutton) {}
    set tkPriv(listboxPrev) {}
    set tkPriv(menuBar) {}
    set tkPriv(mouseMoved) 0
    set tkPriv(oldGrab) {}
    set tkPriv(popup) {}
    set tkPriv(postedMb) {}
    set tkPriv(pressX) 0
    set tkPriv(pressY) 0
    set tkPriv(prevPos) 0
    set tkPriv(screen) $screen
    set tkPriv(selectMode) char
    if {[string compare $tcl_platform(platform) "unix"] == 0} {
        set tkPriv(tearoff) 1
    } else {
        set tkPriv(tearoff) 0
    }
    set tkPriv(window) {}
}

# Do initial setup for tkPriv, so that it is always bound to something
# (otherwise, if someone references it, it may get set to a non-upvar-ed
# value, which will cause trouble later).

tkScreenChanged [winfo screen .]

# tkEventMotifBindings --
# This procedure is invoked as a trace whenever tk_strictMotif is
# changed.  It is used to turn on or turn off the motif virtual
# bindings.
#
# Arguments:
# n1 - the name of the variable being changed ("tk_strictMotif").

proc tkEventMotifBindings {n1 dummy dummy} {
    upvar $n1 name

    if {$name} {
        set op delete
    } else {
        set op add
    }

    event $op <<Cut>> <Control-Key-w>
    event $op <<Copy>> <Meta-Key-w>
    event $op <<Paste>> <Control-Key-y>
}

#----------------------------------------------------------------------
# Define the set of common virtual events.
#----------------------------------------------------------------------

switch $tcl_platform(platform) {
    "unix" {
        event add <<Cut>> <Control-Key-x> <Key-F20>
        event add <<Copy>> <Control-Key-c> <Key-F16>
        event add <<Paste>> <Control-Key-v> <Key-F18>
        event add <<PasteSelection>> <ButtonRelease-2>
        trace variable tk_strictMotif w tkEventMotifBindings
        set tk_strictMotif $tk_strictMotif
    }
    "windows" {
        event add <<Cut>> <Control-Key-x> <Shift-Key-Delete>
        event add <<Copy>> <Control-Key-c> <Control-Key-Insert>
        event add <<Paste>> <Control-Key-v> <Shift-Key-Insert>
        event add <<PasteSelection>> <ButtonRelease-2>
    }
    "macintosh" {
        event add <<Cut>> <Control-Key-x> <Key-F2>
        event add <<Copy>> <Control-Key-c> <Key-F3>
        event add <<Paste>> <Control-Key-v> <Key-F4>
        event add <<PasteSelection>> <ButtonRelease-2>
        event add <<Clear>> <Clear>
    }
}

# ----------------------------------------------------------------------
# Read in files that define all of the class bindings.
# ----------------------------------------------------------------------

#if {$tcl_platform(platform) != "macintosh"} {
#    source $tk_library/button.tcl
#    source $tk_library/entry.tcl
#    source $tk_library/listbox.tcl
#    source $tk_library/menu.tcl
#    source $tk_library/scale.tcl
#    source $tk_library/scrlbar.tcl
#    source $tk_library/text.tcl
#}

# ----------------------------------------------------------------------
# Default bindings for keyboard traversal.
# ----------------------------------------------------------------------

bind all <Tab> {tkTabToWindow [tk_focusNext %W]}
bind all <Shift-Tab> {tkTabToWindow [tk_focusPrev %W]}

# tkCancelRepeat --
# This procedure is invoked to cancel an auto-repeat action described
# by tkPriv(afterId).  It's used by several widgets to auto-scroll
# the widget when the mouse is dragged out of the widget with a
# button pressed.
#
# Arguments:
# None.

proc tkCancelRepeat {} {
    global tkPriv
    after cancel $tkPriv(afterId)
    set tkPriv(afterId) {}
}

# tkTabToWindow --
# This procedure moves the focus to the given widget.  If the widget
# is an entry, it selects the entire contents of the widget.
#
# Arguments:
# w - Window to which focus should be set.

proc tkTabToWindow {w} {
    if {"[winfo class $w]" == "Entry"} {
        $w select range 0 end
        $w icur end
    }
    focus $w
}
  );
  ET_INCLUDE( bgerror.tcl );
  ET_INCLUDE( button.tcl );
  ET_INCLUDE( clrpick.tcl );
  ET_INCLUDE( comdlg.tcl );
  ET_INCLUDE( dialog.tcl );
  ET_INCLUDE( entry.tcl );
  ET_INCLUDE( focus.tcl );
  ET_INCLUDE( listbox.tcl );
  ET_INCLUDE( menu.tcl );
  ET_INCLUDE( msgbox.tcl );
  ET_INCLUDE( obsolete.tcl );
  ET_INCLUDE( optMenu.tcl );
  ET_INCLUDE( palette.tcl );
  ET_INCLUDE( parray.tcl );
  ET_INCLUDE( scale.tcl );
  ET_INCLUDE( scrlbar.tcl );
  ET_INCLUDE( tearoff.tcl );
  ET_INCLUDE( text.tcl );
  ET_INCLUDE( tkfbox.tcl );
  ET_INCLUDE( xmfbox.tcl );
  ET( proc init {} {# do nothing}; init );
  ET(set Win32 0);
  return ET_OK;
}

/*
** Process Tk events until the last window is destroyed, then return.
*/
void
Et_MainLoop(){
    Tk_MainLoop();
}

/*
** Static variables used by the interactive Tcl/Tk processing
*/
static Tcl_DString command;     /* Used to assemble lines of terminal input
                                 * into Tcl commands. */
static Tcl_DString line;        /* Used to read the next line from the
                                 * terminal input. */
static int tty;                 /* Non-zero means standard input is a
                                 * terminal-like device.  Zero means it's
                                 * a file. */


/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *      Issue a prompt on standard output, or invoke a script
 *      to issue the prompt.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      A prompt gets output, and a Tcl script may be evaluated
 *      in interp.
 *
 *----------------------------------------------------------------------
 */

static void Prompt(
    Tcl_Interp *interp,                 /* Interpreter to use for prompting. */
    int partial                         /* Non-zero means there already
                                         * exists a partial command, so use
                                         * the secondary prompt. */
)
{
    char *promptCmd;
    int code;
    Tcl_Channel outChannel, errChannel;

    errChannel = Tcl_GetChannel(interp, "stderr", NULL);

    promptCmd = Tcl_GetVar(interp,
        partial ? "tcl_prompt2" : "tcl_prompt1", TCL_GLOBAL_ONLY);
    if (promptCmd == NULL) {
defaultPrompt:
        if (!partial) {

            /*
             * We must check that outChannel is a real channel - it
             * is possible that someone has transferred stdout out of
             * this interpreter with "interp transfer".
             */

            outChannel = Tcl_GetChannel(interp, "stdout", NULL);
            if (outChannel != (Tcl_Channel) NULL) {
                Tcl_Write(outChannel, "% ", 2);
            }
        }
    } else {
        code = Tcl_Eval(interp, promptCmd);
        if (code != TCL_OK) {
            Tcl_AddErrorInfo(interp,"\n    (script that generates prompt)");

            /*
             * We must check that errChannel is a real channel - it
             * is possible that someone has transferred stderr out of
             * this interpreter with "interp transfer".
             */
            errChannel = Tcl_GetChannel(interp, "stderr", NULL);
            if (errChannel != (Tcl_Channel) NULL) {
                Tcl_Write(errChannel, interp->result, -1);
                Tcl_Write(errChannel, "\n", 1);
            }
            goto defaultPrompt;
        }
    }
    outChannel = Tcl_GetChannel(interp, "stdout", NULL);
    if (outChannel != (Tcl_Channel) NULL) {
        Tcl_Flush(outChannel);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *      This procedure is invoked by the event dispatcher whenever
 *      standard input becomes readable.  It grabs the next line of
 *      input characters, adds them to a command being assembled, and
 *      executes the command if it's complete.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Could be almost arbitrary, depending on the command that's
 *      typed.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
static void StdinProc(
    ClientData clientData,              /* Not used. */
    int mask                            /* Not used. */
)
{
    static int gotPartial = 0;
    char *cmd;
    int code, count;
    Tcl_Channel chan = (Tcl_Channel) clientData;

    count = Tcl_Gets(chan, &line);

    if (count < 0) {
        if (!gotPartial) {
            if (tty) {
                Tcl_Exit(0);
            } else {
                Tcl_DeleteChannelHandler(chan, StdinProc, (ClientData) chan);
            }
            return;
        } else {
            count = 0;
        }
    }

    (void) Tcl_DStringAppend(&command, Tcl_DStringValue(&line), -1);
    cmd = Tcl_DStringAppend(&command, "\n", -1);
    Tcl_DStringFree(&line);

    if (!Tcl_CommandComplete(cmd)) {
        gotPartial = 1;
        goto prompt;
    }
    gotPartial = 0;

    /*
     * Disable the stdin channel handler while evaluating the command;
     * otherwise if the command re-enters the event loop we might
     * process commands from stdin before the current command is
     * finished.  Among other things, this will trash the text of the
     * command being evaluated.
     */

    Tcl_CreateChannelHandler(chan, 0, StdinProc, (ClientData) chan);
    code = Tcl_RecordAndEval(Et_Interp, cmd, TCL_EVAL_GLOBAL);
    Tcl_CreateChannelHandler(chan, TCL_READABLE, StdinProc,
            (ClientData) chan);
    Tcl_DStringFree(&command);
    if (*Et_Interp->result != 0) {
        if ((code != TCL_OK) || (tty)) {
            /*
             * The statement below used to call "printf", but that resulted
             * in core dumps under Solaris 2.3 if the result was very long.
             *
             * NOTE: This probably will not work under Windows either.
             */

            puts(Et_Interp->result);
        }
    }

    /*
     * Output a prompt.
     */

    prompt:
    if (tty) {
        Prompt(Et_Interp, gotPartial);
    }
    Tcl_ResetResult(Et_Interp);
}

/*
** Make arrangements to read and interpret standard input
*/
void
Et_ReadStdin()
{
  Tcl_Channel inChannel;

  tty = isatty(0);
  inChannel = Tcl_GetStdChannel(TCL_STDIN);
  if (inChannel) {
    Tcl_CreateChannelHandler(inChannel, TCL_READABLE, StdinProc,
                             (ClientData) inChannel);
  }
  if( tty ){
    ET(
      if ![info exists tcl_prompt1] {
        set tcl_prompt1 { puts -nonewline stdout [file tail $argv0]> }
      }
      if ![info exists tcl_prompt2] {
        set tcl_prompt2 { puts -nonewline stdout => }
      }
    );
    Prompt(Et_Interp, 0);
  }
  Tcl_DStringInit(&command);
}
