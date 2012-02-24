#include "sofam.h"

void iauNut00b(double date1, double date2, double *dpsi, double *deps)
/*
**  - - - - - - - - - -
**   i a u N u t 0 0 b
**  - - - - - - - - - -
**
**  Nutation, IAU 2000B model.
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards Of Fundamental Astronomy) software collection.
**
**  Status:  canonical model.
**
**  Given:
**     date1,date2   double    TT as a 2-part Julian Date (Note 1)
**
**  Returned:
**     dpsi,deps     double    nutation, luni-solar + planetary (Note 2)
**
**  Notes:
**
**  1) The TT date date1+date2 is a Julian Date, apportioned in any
**     convenient way between the two arguments.  For example,
**     JD(TT)=2450123.7 could be expressed in any of these ways,
**     among others:
**
**            date1          date2
**
**         2450123.7           0.0       (JD method)
**         2451545.0       -1421.3       (J2000 method)
**         2400000.5       50123.2       (MJD method)
**         2450123.5           0.2       (date & time method)
**
**     The JD method is the most natural and convenient to use in
**     cases where the loss of several decimal digits of resolution
**     is acceptable.  The J2000 method is best matched to the way
**     the argument is handled internally and will deliver the
**     optimum resolution.  The MJD method and the date & time methods
**     are both good compromises between resolution and convenience.
**
**  2) The nutation components in longitude and obliquity are in radians
**     and with respect to the equinox and ecliptic of date.  The
**     obliquity at J2000.0 is assumed to be the Lieske et al. (1977)
**     value of 84381.448 arcsec.  (The errors that result from using
**     this function with the IAU 2006 value of 84381.406 arcsec can be
**     neglected.)
**
**     The nutation model consists only of luni-solar terms, but
**     includes also a fixed offset which compensates for certain long-
**     period planetary terms (Note 7).
**
**  3) This function is an implementation of the IAU 2000B abridged
**     nutation model formally adopted by the IAU General Assembly in
**     2000.  The function computes the MHB_2000_SHORT luni-solar
**     nutation series (Luzum 2001), but without the associated
**     corrections for the precession rate adjustments and the offset
**     between the GCRS and J2000.0 mean poles.
**
**  4) The full IAU 2000A (MHB2000) nutation model contains nearly 1400
**     terms.  The IAU 2000B model (McCarthy & Luzum 2003) contains only
**     77 terms, plus additional simplifications, yet still delivers
**     results of 1 mas accuracy at present epochs.  This combination of
**     accuracy and size makes the IAU 2000B abridged nutation model
**     suitable for most practical applications.
**
**     The function delivers a pole accurate to 1 mas from 1900 to 2100
**     (usually better than 1 mas, very occasionally just outside
**     1 mas).  The full IAU 2000A model, which is implemented in the
**     function iauNut00a (q.v.), delivers considerably greater accuracy
**     at current dates;  however, to realize this improved accuracy,
**     corrections for the essentially unpredictable free-core-nutation
**     (FCN) must also be included.
**
**  5) The present function provides classical nutation.  The
**     MHB_2000_SHORT algorithm, from which it is adapted, deals also
**     with (i) the offsets between the GCRS and mean poles and (ii) the
**     adjustments in longitude and obliquity due to the changed
**     precession rates.  These additional functions, namely frame bias
**     and precession adjustments, are supported by the SOFA functions
**     iauBi00  and iauPr00.
**
**  6) The MHB_2000_SHORT algorithm also provides "total" nutations,
**     comprising the arithmetic sum of the frame bias, precession
**     adjustments, and nutation (luni-solar + planetary).  These total
**     nutations can be used in combination with an existing IAU 1976
**     precession implementation, such as iauPmat76,  to deliver GCRS-
**     to-true predictions of mas accuracy at current epochs.  However,
**     for symmetry with the iauNut00a  function (q.v. for the reasons),
**     the SOFA functions do not generate the "total nutations"
**     directly.  Should they be required, they could of course easily
**     be generated by calling iauBi00, iauPr00 and the present function
**     and adding the results.
**
**  7) The IAU 2000B model includes "planetary bias" terms that are
**     fixed in size but compensate for long-period nutations.  The
**     amplitudes quoted in McCarthy & Luzum (2003), namely
**     Dpsi = -1.5835 mas and Depsilon = +1.6339 mas, are optimized for
**     the "total nutations" method described in Note 6.  The Luzum
**     (2001) values used in this SOFA implementation, namely -0.135 mas
**     and +0.388 mas, are optimized for the "rigorous" method, where
**     frame bias, precession and nutation are applied separately and in
**     that order.  During the interval 1995-2050, the SOFA
**     implementation delivers a maximum error of 1.001 mas (not
**     including FCN).
**
**  References:
**
**     Lieske, J.H., Lederle, T., Fricke, W., Morando, B., "Expressions
**     for the precession quantities based upon the IAU /1976/ system of
**     astronomical constants", Astron.Astrophys. 58, 1-2, 1-16. (1977)
**
**     Luzum, B., private communication, 2001 (Fortran code
**     MHB_2000_SHORT)
**
**     McCarthy, D.D. & Luzum, B.J., "An abridged model of the
**     precession-nutation of the celestial pole", Cel.Mech.Dyn.Astron.
**     85, 37-49 (2003)
**
**     Simon, J.-L., Bretagnon, P., Chapront, J., Chapront-Touze, M.,
**     Francou, G., Laskar, J., Astron.Astrophys. 282, 663-683 (1994)
**
**  This revision:  2010 September 4
**
**  SOFA release 2010-12-01
**
**  Copyright (C) 2010 IAU SOFA Board.  See notes at end.
*/
{
   double t, el, elp, f, d, om, arg, dp, de, sarg, carg,
          dpsils, depsls, dpsipl, depspl;
   int i;

/* Units of 0.1 microarcsecond to radians */
   static const double U2R = DAS2R / 1e7;

/* ---------------------------------------- */
/* Fixed offsets in lieu of planetary terms */
/* ---------------------------------------- */

   static const double DPPLAN = -0.135 * DMAS2R;
   static const double DEPLAN =  0.388 * DMAS2R;

/* --------------------------------------------------- */
/* Luni-solar nutation: argument and term coefficients */
/* --------------------------------------------------- */

/* The units for the sine and cosine coefficients are */
/* 0.1 microarcsec and the same per Julian century    */

   static const struct {
      int nl,nlp,nf,nd,nom; /* coefficients of l,l',F,D,Om */
      double ps,pst,pc;     /* longitude sin, t*sin, cos coefficients */
      double ec,ect,es;     /* obliquity cos, t*cos, sin coefficients */

   } x[] = {

   /* 1-10 */
      { 0, 0, 0, 0,1,
         -172064161.0, -174666.0, 33386.0, 92052331.0, 9086.0, 15377.0},
      { 0, 0, 2,-2,2,
           -13170906.0, -1675.0, -13696.0, 5730336.0, -3015.0, -4587.0},
      { 0, 0, 2, 0,2,-2276413.0,-234.0, 2796.0, 978459.0,-485.0,1374.0},
      { 0, 0, 0, 0,2,2074554.0,  207.0, -698.0,-897492.0, 470.0,-291.0},
      { 0, 1, 0, 0,0,1475877.0,-3633.0,11817.0, 73871.0,-184.0,-1924.0},
      { 0, 1, 2,-2,2,-516821.0, 1226.0, -524.0, 224386.0,-677.0,-174.0},
      { 1, 0, 0, 0,0, 711159.0,   73.0, -872.0,  -6750.0,   0.0, 358.0},
      { 0, 0, 2, 0,1,-387298.0, -367.0,  380.0, 200728.0,  18.0, 318.0},
      { 1, 0, 2, 0,2,-301461.0,  -36.0,  816.0, 129025.0, -63.0, 367.0},
      { 0,-1, 2,-2,2, 215829.0, -494.0,  111.0, -95929.0, 299.0, 132.0},

   /* 11-20 */
      { 0, 0, 2,-2,1, 128227.0,  137.0,  181.0, -68982.0,  -9.0,  39.0},
      {-1, 0, 2, 0,2, 123457.0,   11.0,   19.0, -53311.0,  32.0,  -4.0},
      {-1, 0, 0, 2,0, 156994.0,   10.0, -168.0,  -1235.0,   0.0,  82.0},
      { 1, 0, 0, 0,1,  63110.0,   63.0,   27.0, -33228.0,   0.0,  -9.0},
      {-1, 0, 0, 0,1, -57976.0,  -63.0, -189.0,  31429.0,   0.0, -75.0},
      {-1, 0, 2, 2,2, -59641.0,  -11.0,  149.0,  25543.0, -11.0,  66.0},
      { 1, 0, 2, 0,1, -51613.0,  -42.0,  129.0,  26366.0,   0.0,  78.0},
      {-2, 0, 2, 0,1,  45893.0,   50.0,   31.0, -24236.0, -10.0,  20.0},
      { 0, 0, 0, 2,0,  63384.0,   11.0, -150.0,  -1220.0,   0.0,  29.0},
      { 0, 0, 2, 2,2, -38571.0,   -1.0,  158.0,  16452.0, -11.0,  68.0},

   /* 21-30 */
      { 0,-2, 2,-2,2,  32481.0,    0.0,    0.0, -13870.0,   0.0,   0.0},
      {-2, 0, 0, 2,0, -47722.0,    0.0,  -18.0,    477.0,   0.0, -25.0},
      { 2, 0, 2, 0,2, -31046.0,   -1.0,  131.0,  13238.0, -11.0,  59.0},
      { 1, 0, 2,-2,2,  28593.0,    0.0,   -1.0, -12338.0,  10.0,  -3.0},
      {-1, 0, 2, 0,1,  20441.0,   21.0,   10.0, -10758.0,   0.0,  -3.0},
      { 2, 0, 0, 0,0,  29243.0,    0.0,  -74.0,   -609.0,   0.0,  13.0},
      { 0, 0, 2, 0,0,  25887.0,    0.0,  -66.0,   -550.0,   0.0,  11.0},
      { 0, 1, 0, 0,1, -14053.0,  -25.0,   79.0,   8551.0,  -2.0, -45.0},
      {-1, 0, 0, 2,1,  15164.0,   10.0,   11.0,  -8001.0,   0.0,  -1.0},
      { 0, 2, 2,-2,2, -15794.0,   72.0,  -16.0,   6850.0, -42.0,  -5.0},

   /* 31-40 */
      { 0, 0,-2, 2,0,  21783.0,    0.0,   13.0,   -167.0,   0.0,  13.0},
      { 1, 0, 0,-2,1, -12873.0,  -10.0,  -37.0,   6953.0,   0.0, -14.0},
      { 0,-1, 0, 0,1, -12654.0,   11.0,   63.0,   6415.0,   0.0,  26.0},
      {-1, 0, 2, 2,1, -10204.0,    0.0,   25.0,   5222.0,   0.0,  15.0},
      { 0, 2, 0, 0,0,  16707.0,  -85.0,  -10.0,    168.0,  -1.0,  10.0},
      { 1, 0, 2, 2,2,  -7691.0,    0.0,   44.0,   3268.0,   0.0,  19.0},
      {-2, 0, 2, 0,0, -11024.0,    0.0,  -14.0,    104.0,   0.0,   2.0},
      { 0, 1, 2, 0,2,   7566.0,  -21.0,  -11.0,  -3250.0,   0.0,  -5.0},
      { 0, 0, 2, 2,1,  -6637.0,  -11.0,   25.0,   3353.0,   0.0,  14.0},
      { 0,-1, 2, 0,2,  -7141.0,   21.0,    8.0,   3070.0,   0.0,   4.0},

   /* 41-50 */
      { 0, 0, 0, 2,1,  -6302.0,  -11.0,    2.0,   3272.0,   0.0,   4.0},
      { 1, 0, 2,-2,1,   5800.0,   10.0,    2.0,  -3045.0,   0.0,  -1.0},
      { 2, 0, 2,-2,2,   6443.0,    0.0,   -7.0,  -2768.0,   0.0,  -4.0},
      {-2, 0, 0, 2,1,  -5774.0,  -11.0,  -15.0,   3041.0,   0.0,  -5.0},
      { 2, 0, 2, 0,1,  -5350.0,    0.0,   21.0,   2695.0,   0.0,  12.0},
      { 0,-1, 2,-2,1,  -4752.0,  -11.0,   -3.0,   2719.0,   0.0,  -3.0},
      { 0, 0, 0,-2,1,  -4940.0,  -11.0,  -21.0,   2720.0,   0.0,  -9.0},
      {-1,-1, 0, 2,0,   7350.0,    0.0,   -8.0,    -51.0,   0.0,   4.0},
      { 2, 0, 0,-2,1,   4065.0,    0.0,    6.0,  -2206.0,   0.0,   1.0},
      { 1, 0, 0, 2,0,   6579.0,    0.0,  -24.0,   -199.0,   0.0,   2.0},

   /* 51-60 */
      { 0, 1, 2,-2,1,   3579.0,    0.0,    5.0,  -1900.0,   0.0,   1.0},
      { 1,-1, 0, 0,0,   4725.0,    0.0,   -6.0,    -41.0,   0.0,   3.0},
      {-2, 0, 2, 0,2,  -3075.0,    0.0,   -2.0,   1313.0,   0.0,  -1.0},
      { 3, 0, 2, 0,2,  -2904.0,    0.0,   15.0,   1233.0,   0.0,   7.0},
      { 0,-1, 0, 2,0,   4348.0,    0.0,  -10.0,    -81.0,   0.0,   2.0},
      { 1,-1, 2, 0,2,  -2878.0,    0.0,    8.0,   1232.0,   0.0,   4.0},
      { 0, 0, 0, 1,0,  -4230.0,    0.0,    5.0,    -20.0,   0.0,  -2.0},
      {-1,-1, 2, 2,2,  -2819.0,    0.0,    7.0,   1207.0,   0.0,   3.0},
      {-1, 0, 2, 0,0,  -4056.0,    0.0,    5.0,     40.0,   0.0,  -2.0},
      { 0,-1, 2, 2,2,  -2647.0,    0.0,   11.0,   1129.0,   0.0,   5.0},

   /* 61-70 */
      {-2, 0, 0, 0,1,  -2294.0,    0.0,  -10.0,   1266.0,   0.0,  -4.0},
      { 1, 1, 2, 0,2,   2481.0,    0.0,   -7.0,  -1062.0,   0.0,  -3.0},
      { 2, 0, 0, 0,1,   2179.0,    0.0,   -2.0,  -1129.0,   0.0,  -2.0},
      {-1, 1, 0, 1,0,   3276.0,    0.0,    1.0,     -9.0,   0.0,   0.0},
      { 1, 1, 0, 0,0,  -3389.0,    0.0,    5.0,     35.0,   0.0,  -2.0},
      { 1, 0, 2, 0,0,   3339.0,    0.0,  -13.0,   -107.0,   0.0,   1.0},
      {-1, 0, 2,-2,1,  -1987.0,    0.0,   -6.0,   1073.0,   0.0,  -2.0},
      { 1, 0, 0, 0,2,  -1981.0,    0.0,    0.0,    854.0,   0.0,   0.0},
      {-1, 0, 0, 1,0,   4026.0,    0.0, -353.0,   -553.0,   0.0,-139.0},
      { 0, 0, 2, 1,2,   1660.0,    0.0,   -5.0,   -710.0,   0.0,  -2.0},

   /* 71-77 */
      {-1, 0, 2, 4,2,  -1521.0,    0.0,    9.0,    647.0,   0.0,   4.0},
      {-1, 1, 0, 1,1,   1314.0,    0.0,    0.0,   -700.0,   0.0,   0.0},
      { 0,-2, 2,-2,1,  -1283.0,    0.0,    0.0,    672.0,   0.0,   0.0},
      { 1, 0, 2, 2,1,  -1331.0,    0.0,    8.0,    663.0,   0.0,   4.0},
      {-2, 0, 2, 2,2,   1383.0,    0.0,   -2.0,   -594.0,   0.0,  -2.0},
      {-1, 0, 0, 0,2,   1405.0,    0.0,    4.0,   -610.0,   0.0,   2.0},
      { 1, 1, 2,-2,2,   1290.0,    0.0,    0.0,   -556.0,   0.0,   0.0}
   };

/* Number of terms in the series */
   const int NLS = (int) (sizeof x / sizeof x[0]);

/*--------------------------------------------------------------------*/

/* Interval between fundamental epoch J2000.0 and given date (JC). */
   t = ((date1 - DJ00) + date2) / DJC;

/* --------------------*/
/* LUNI-SOLAR NUTATION */
/* --------------------*/

/* Fundamental (Delaunay) arguments from Simon et al. (1994) */

/* Mean anomaly of the Moon. */
   el = fmod(485868.249036 + (1717915923.2178) * t, TURNAS) * DAS2R;

/* Mean anomaly of the Sun. */
   elp = fmod(1287104.79305 + (129596581.0481) * t, TURNAS) * DAS2R;

/* Mean argument of the latitude of the Moon. */
   f = fmod(335779.526232 + (1739527262.8478) * t, TURNAS) * DAS2R;

/* Mean elongation of the Moon from the Sun. */
   d = fmod(1072260.70369 + (1602961601.2090) * t, TURNAS) * DAS2R;

/* Mean longitude of the ascending node of the Moon. */
   om = fmod(450160.398036 + (-6962890.5431) * t, TURNAS) * DAS2R;

/* Initialize the nutation values. */
   dp = 0.0;
   de = 0.0;

/* Summation of luni-solar nutation series (smallest terms first). */
   for (i = NLS-1; i >= 0; i--) {

   /* Argument and functions. */
      arg = fmod( (double)x[i].nl  * el  +
                  (double)x[i].nlp * elp +
                  (double)x[i].nf  * f   +
                  (double)x[i].nd  * d   +
                  (double)x[i].nom * om, D2PI  );
      sarg = sin(arg);
      carg = cos(arg);

   /* Term. */
      dp += (x[i].ps + x[i].pst * t) * sarg + x[i].pc * carg;
      de += (x[i].ec + x[i].ect * t) * carg + x[i].es * sarg;
   }

/* Convert from 0.1 microarcsec units to radians. */
   dpsils = dp * U2R;
   depsls = de * U2R;

/* ------------------------------*/
/* IN LIEU OF PLANETARY NUTATION */
/* ------------------------------*/

/* Fixed offset to correct for missing terms in truncated series. */
   dpsipl = DPPLAN;
   depspl = DEPLAN;

/* --------*/
/* RESULTS */
/* --------*/

/* Add luni-solar and planetary components. */
   *dpsi = dpsils + dpsipl;
   *deps = depsls + depspl;

   return;

/*----------------------------------------------------------------------
**
**  Copyright (C) 2010
**  Standards Of Fundamental Astronomy Board
**  of the International Astronomical Union.
**
**  =====================
**  SOFA Software License
**  =====================
**
**  NOTICE TO USER:
**
**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING TERMS AND CONDITIONS
**  WHICH APPLY TO ITS USE.
**
**  1. The Software is owned by the IAU SOFA Board ("SOFA").
**
**  2. Permission is granted to anyone to use the SOFA software for any
**     purpose, including commercial applications, free of charge and
**     without payment of royalties, subject to the conditions and
**     restrictions listed below.
**
**  3. You (the user) may copy and distribute SOFA source code to others,
**     and use and adapt its code and algorithms in your own software,
**     on a world-wide, royalty-free basis.  That portion of your
**     distribution that does not consist of intact and unchanged copies
**     of SOFA source code files is a "derived work" that must comply
**     with the following requirements:
**
**     a) Your work shall be marked or carry a statement that it
**        (i) uses routines and computations derived by you from
**        software provided by SOFA under license to you; and
**        (ii) does not itself constitute software provided by and/or
**        endorsed by SOFA.
**
**     b) The source code of your derived work must contain descriptions
**        of how the derived work is based upon, contains and/or differs
**        from the original SOFA software.
**
**     c) The name(s) of all routine(s) in your derived work shall not
**        include the prefix "iau".
**
**     d) The origin of the SOFA components of your derived work must
**        not be misrepresented;  you must not claim that you wrote the
**        original software, nor file a patent application for SOFA
**        software or algorithms embedded in the SOFA software.
**
**     e) These requirements must be reproduced intact in any source
**        distribution and shall apply to anyone to whom you have
**        granted a further right to modify the source code of your
**        derived work.
**
**     Note that, as originally distributed, the SOFA software is
**     intended to be a definitive implementation of the IAU standards,
**     and consequently third-party modifications are discouraged.  All
**     variations, no matter how minor, must be explicitly marked as
**     such, as explained above.
**
**  4. In any published work or commercial products which includes
**     results achieved by using the SOFA software, you shall
**     acknowledge that the SOFA software was used in obtaining those
**     results.
**
**  5. You shall not cause the SOFA software to be brought into
**     disrepute, either by misuse, or use for inappropriate tasks, or
**     by inappropriate modification.
**
**  6. The SOFA software is provided "as is" and SOFA makes no warranty
**     as to its use or performance.   SOFA does not and cannot warrant
**     the performance or results which the user may obtain by using the
**     SOFA software.  SOFA makes no warranties, express or implied, as
**     to non-infringement of third party rights, merchantability, or
**     fitness for any particular purpose.  In no event will SOFA be
**     liable to the user for any consequential, incidental, or special
**     damages, including any lost profits or lost savings, even if a
**     SOFA representative has been advised of such damages, or for any
**     claim by any third party.
**
**  7. The provision of any version of the SOFA software under the terms
**     and conditions specified herein does not imply that future
**     versions will also be made available under the same terms and
**     conditions.
**
**  Correspondence concerning SOFA software should be addressed as
**  follows:
**
**      By email:  sofa@ukho.gov.uk
**      By post:   IAU SOFA Center
**                 HM Nautical Almanac Office
**                 UK Hydrographic Office
**                 Admiralty Way, Taunton
**                 Somerset, TA1 2DN
**                 United Kingdom
**
**--------------------------------------------------------------------*/
}