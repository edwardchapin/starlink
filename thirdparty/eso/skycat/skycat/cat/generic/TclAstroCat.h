// -*-c++-*-
#ifndef _TclAstroCat_h_
#define _TclAstroCat_h_

/*
 * E.S.O. - VLT project/ESO Archive
 * $Id: TclAstroCat.h,v 1.18 1998/08/21 10:45:37 abrighto Exp $
 *
 * TclAstroCat.h - Tcl interface to the AstroCatalog C++ class for 
 * 	 	  accessing astronomical catalogs
 *
 * See the man page for a complete description.
 *
 * who             when       what
 * --------------  --------   ----------------------------------------
 * Allan Brighton  26 Sep 95  Created
 */


#include "TclCommand.h"
#include "AstroCatalog.h"


/*
 * This class declares the methods used to implement the Tcl astrocat
 * command for accessing astronomical catalogs.
 */
class TclAstroCat : public TclCommand {
protected:
    AstroCatalog* cat_;		// pointer to current open catalog
    WorldOrImageCoords pos1_, pos2_;	// saved positions from last query
    double equinox_;		// saved equinox from last query
    FILE* feedback_;		// file ptr for feedback during xfer, if set
    QueryResult* result_;	// saved pointer to results of previous query
   
    // call a member function by name
    virtual int call(const char* name, int len, int argc, char* argv[]);
    
    // convert tcl list to QueryResult given column headings
    virtual int getQueryResult(int numCols, char** colNames, const char* list, 
			       double equinox, QueryResult& r);

    // Save (or insert) query results to the given file.
    virtual int saveQueryResult(const char* filename, int numCols, char** colNames, 
				char* info, int iflag, double equinox = 2000.);
    // Remove query results from the given file.
    virtual int removeQueryResult(const char* filename, int numCols, char** colNames, 
				  char* info, double equinox = 2000.);

    // append the given keyword/value pair to the list
    virtual void appendKeyVal(const char* keyword, const char* value);
    virtual int appendKeyListVal(const char* keyword, const char* value);
    virtual int appendListVal(const char* value);

    // convert tcl list cat entry to config file format
    virtual int tclListToConfigStreamValue(const char* tclList, ostream& os);
    virtual int tclListToConfigStreamLine(const char* tclList, ostream& os);
    virtual int tclListToConfigStream(const char* tclList, ostream& os);

public:
    // constructor
    TclAstroCat(Tcl_Interp*, const char* cmdname, const char* instname);
    ~TclAstroCat();

    // entry point from Tcl
    static int astroCatCmd(ClientData, Tcl_Interp* interp, int argc, char* argv[]);

    // -- tcl subcommands --
    virtual int authorizeCmd(int argc, char* argv[]);
    virtual int checkCmd(int argc, char* argv[]);
    virtual int checkrowCmd(int argc, char* argv[]);
    virtual int closeCmd(int argc, char* argv[]);
    virtual int copyrightCmd(int argc, char* argv[]);
    virtual int helpCmd(int argc, char* argv[]);
    virtual int dec_colCmd(int argc, char* argv[]);
    virtual int entryCmd(int argc, char* argv[]);
    virtual int feedbackCmd(int argc, char* argv[]);
    virtual int getcolCmd(int argc, char* argv[]);
    virtual int getidposCmd(int argc, char* argv[]);
    virtual int getimageCmd(int argc, char* argv[]);
    virtual int getpreviewCmd(int argc, char* argv[]);
    virtual int hascolCmd(int argc, char* argv[]);
    virtual int headingsCmd(int argc, char* argv[]);
    virtual int id_colCmd(int argc, char* argv[]);
    virtual int infoCmd(int argc, char* argv[]);
    virtual int is_tcsCmd(int argc, char* argv[]);
    virtual int iswcsCmd(int argc, char* argv[]);
    virtual int ispixCmd(int argc, char* argv[]);
    virtual int loadCmd(int argc, char* argv[]);
    virtual int longnameCmd(int argc, char* argv[]);
    virtual int moreCmd(int argc, char* argv[]);
    virtual int openCmd(int argc, char* argv[]);
    virtual int plotCmd(int argc, char* argv[]);
    virtual int queryCmd(int argc, char* argv[]);
    virtual int queryposCmd(int argc, char* argv[]);
    virtual int ra_colCmd(int argc, char* argv[]);
    virtual int reloadCmd(int argc, char* argv[]);
    virtual int removeCmd(int argc, char* argv[]);
    virtual int rootCmd(int argc, char* argv[]);
    virtual int saveCmd(int argc, char* argv[]);
    virtual int searchcolsCmd(int argc, char* argv[]);
    virtual int servtypeCmd(int argc, char* argv[]);
    virtual int shortnameCmd(int argc, char* argv[]);
    virtual int showcolsCmd(int argc, char* argv[]);
    virtual int sortcolsCmd(int argc, char* argv[]);
    virtual int sortorderCmd(int argc, char* argv[]);
    virtual int symbolCmd(int argc, char* argv[]);
    virtual int urlCmd(int argc, char* argv[]);
    virtual int x_colCmd(int argc, char* argv[]);
    virtual int y_colCmd(int argc, char* argv[]);
};

#endif /* _TclAstroCat_h_ */

