      BLOCKDATA UTILBD
C
C The common block IUTLCM contains integer utility variables which are
C user-settable by the routine SETUSV and user-retrievable by the
C routine GETUSV.
C
      COMMON /IUTLCM/ IU(100)
C
C The common block VCTSEQ contains variables realizing the buffering
C scheme used by PLOTIT for pen moves.  The dimension of QX and QY must
C be an even number greater than or equal to the value of IU(5).  The
C dimension of IF must be half that of QX and QY.
C
      COMMON /VCTSEQ/ NQ,QX(50),QY(50),NF,IF(25)
C
C In the common block PLTCM are recorded the coordinates of the last
C point to which a pen move was requested by a call to PLOTIT.
C
      COMMON /PLTCM/ JX,JY
C
C IU(1) contains the log scaling parameter, which may take on the
C following possible values:
C
C     1 = linear-linear
C     2 = log-linear
C     3 = linear-log
C     4 = log-log
C
      DATA IU(1) / 1 /
C
C IU(2) specifies the mirror-imaging of the x and y axes, as follows:
C
C     1 = x normal, y normal
C     2 = x normal, y reversed
C     3 = x reversed, y normal
C     4 = x reversed, y reversed
C
      DATA IU(2) / 1 /
C
C IU(3) specifies the assumed resolution of the plotter in the x
C direction.  Plotter x coordinates are assumed to lie between 1 and
C 2**IU(3), inclusive.
C
      DATA IU(3) / 10 /
C
C IU(4) specifies the assumed resolution of the plotter in the y
C direction.  Plotter y coordinates are assumed to lie between 1 and
C 2**IU(4), inclusive.
C
      DATA IU(4) / 10 /
C
C IU(5) specifies the size of the buffers used by PLOTIT.  Its value
C must be greater than or equal to 2 and not greater than the dimension
C of the variables QX and QY.  Using the value 2 effectively turns off
C the buffering.
C
      DATA IU(5) / 50 /
C
C IU(6) specifies the current metacode unit, which is machine-dependent.
C At NCAR, the value "1" currently (1/11/85) causes metacode to be
C written on the file "GMETA".  Eventually, it will cause output to be
C written on unit number 1.  At that point, the value, on the Cray at
C least, should be changed to "4H$PLT", so that output will come out on
C the old familiar dataset.
C
      DATA IU(6) / 1 /
C
C IU(7), IU(8), IU(9), and IU(10) specify color and intensity, in the
C following way (letting IR=IU(7), IG=IU(8), IB=IU(9), and IN=IU(10)):
C
C     The red intensity is IR/(IR+IG+IB)*IN/10000.
C     The green intensity is IG/(IR+IG+IB)*IN/10000.
C     The blue intensity is IB/(IR+IG+IB)*IN/10000.
C
C The GKS calls to set these intensities are executed in response to a
C "CALL SETUSV ('IN',IN)", using the existing values of IR, IG, and IB.
C Thus, to completely determine the color and the intensity, the user
C must execute four calls, as follows:
C
C     CALL SETUSV ('IR',IR)
C     CALL SETUSV ('IG',IG)
C     CALL SETUSV ('IB',IB)
C     CALL SETUSV ('IN',IN)
C
C The default values create a white line at .8 x maximum intensity.
C
      DATA IU(7) / 1 /
      DATA IU(8) / 1 /
      DATA IU(9) / 1 /
C
      DATA IU(10) / 8000 /
C
C IU(11) and IU(12) specify, respectively, the last color index used
C and the maximum number of color indices it is permissible to use.
C
      DATA IU(11) / 0 /
      DATA IU(12) / 1 /
C
C IU(13)/1000 specifies the current line width scale factor.
C
      DATA IU(13) / 1000 /
C
C IU(14)/1000 specifies the current marker size scale factor.
C
      DATA IU(14) / 1000 /
C
C IU(15) through IU(100) are currently undefined.
C
C Initialization for the routine PLOTIT:  For values of I between 1 and
C NQ, (QX(I),QY(I)) is a point to which a pen move has been requested
C by a past call to PLOTIT.  The coordinates are stated in the fractional
C coordinate system.  For values of I between 1 and NF, IF(I) is the
C index, in QX and QY, of the coordinates of a point to which a pen-up
C move was requested.  NQ and NF are never allowed to be less than one.
C
      DATA NQ,QX(1),QY(1),NF,IF(1) / 1 , 0. , 0. , 1 , 1 /
C
C JX and JY are the coordinates, in the metacode system, of the last
C point to which a pen move was requested by a call to PLOTIT.
C
      DATA JX,JY / 0 , 0 /
C
      END
