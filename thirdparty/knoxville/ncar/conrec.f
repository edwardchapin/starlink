      SUBROUTINE CONREC (Z,L,M,N,FLO,HI,FINC,NSET,NHI,NDOT)
      EXTERNAL        CONBD
      SAVE
C
      CHARACTER*1     IGAP       ,ISOL       ,RCHAR
      CHARACTER       ENCSCR*22  ,IWORK*126
      DIMENSION       LNGTHS(5)  ,HOLD(5)    ,WNDW(4)    ,VWPRT(4)
      DIMENSION       Z(L,N)     ,CL(40)     ,RWORK(40)  ,LASF(13)
      COMMON /INTPR/ PAD1, FPART, PAD(8)
      COMMON /CONRE1/ IOFFP      ,SPVAL
      COMMON /CONRE3/ IXBITS     ,IYBITS
      COMMON /CONRE4/ ISIZEL     ,ISIZEM     ,ISIZEP     ,NREP       ,
     1                NCRT       ,ILAB       ,NULBLL     ,IOFFD      ,
     2                EXT        ,IOFFM      ,ISOLID     ,NLA        ,
     3                NLM        ,XLT        ,YBT        ,SIDE
      COMMON /CONRE5/ SCLY
      COMMON /RECINT/ IRECMJ     ,IRECMN     ,IRECTX
      DATA  LNGTHS(1),LNGTHS(2),LNGTHS(3),LNGTHS(4),LNGTHS(5)
     1      /  12,       3,        20,       9,        17       /
      DATA  ISOL, IGAP /'$', ''''/
C
C ISOL AND IGAP (DOLLAR-SIGN AND APOSTROPHE) ARE USED TO CONSTRUCT PAT-
C TERNS PASSED TO ROUTINE DASHDC IN THE SOFTWARE DASHED-LINE PACKAGE.
C
C
C
C THE FOLLOWING CALL IS FOR GATHERING STATISTICS ON LIBRARY USE AT NCAR
C
      CALL Q8QST4 ('GRAPHX','CONREC','CONREC','VERSION 01')
C
C NONSMOOTHING VERSION
C
C
C
C  CALL RESET FOR COMPATIBILITY WITH ALL DASH ROUTINES(EXCEPT DASHLINE)
C
      CALL RESET
C
C  GET NUMBER OF BITS IN INTEGER ARITHMETIC
C
      IARTH = I1MACH(8)
      IXBITS = 0
      DO 101 I=1,IARTH
         IF (M .LE. (2**I-1)) GO TO 102
         IXBITS = I+1
  101 CONTINUE
  102 IYBITS = 0
      DO 103 I=1,IARTH
         IF (N .LE. (2**I-1)) GO TO 104
         IYBITS = I+1
  103 CONTINUE
  104 IF ((IXBITS*IYBITS).GT.0 .AND. (IXBITS+IYBITS).LE.24) GO TO 105
C
C REPORT ERROR NUMBER ONE
C
      IWORK =    'CONREC  - DIMENSION ERROR - M*N .GT. (2**IARTH)    M =
     +            N = '
      WRITE (IWORK(56:62),'(I6)') M
      WRITE (IWORK(73:79),'(I6)') N
      CALL SETER( IWORK, 1, 1 )
      RETURN
  105 CONTINUE
C
C INQUIRE CURRENT TEXT AND LINE COLOR INDEX
C
      CALL GQTXCI ( IERR, ITXCI )
      CALL GQPLCI ( IERR, IPLCI )
C
C SET LINE AND TEXT ASF TO INDIVIDUAL
C
      CALL GQASF ( IERR, LASF )
      LSV3  = LASF(3)
      LSV10 = LASF(10)
      LASF(3)  = 1
      LASF(10) = 1
      CALL GSASF ( LASF )
C
      GL = FLO
      HA = HI
      GP = FINC
      MX = L
      NX = M
      NY = N
      IDASH = NDOT
      NEGPOS = ISIGN(1,IDASH)
      IDASH = IABS(IDASH)
      IF (IDASH.EQ.0 .OR. IDASH.EQ.1) IDASH = ISOLID
C
C SET CONTOUR LEVELS.
C
      CALL CLGEN (Z,MX,NX,NY,GL,HA,GP,NLA,NLM,CL,NCL,ICNST)
C
C FIND MAJOR AND MINOR LINES
C
      IF (ILAB .NE. 0) CALL REORD (CL,NCL,RWORK,NML,NULBLL+1)
      IF (ILAB .EQ. 0) NML = 0
C
C SAVE CURRENT NORMALIZATION TRANS NUMBER NTORIG AND LOG SCALING FLAG
C
      CALL GQCNTN ( IERR, NTORIG )
      CALL GETUSV ('LS',IOLLS)
C
C SET UP SCALING
C
      CALL GETUSV ( 'YF' , IYVAL )
      SCLY = 1.0 / ISHIFT ( 1, 15 - IYVAL )
C
      IF (NSET) 106,107,111
  106 CALL GQNT ( NTORIG,IERR,WNDW,VWPRT )
      X1 = VWPRT(1)
      X2 = VWPRT(2)
      Y1 = VWPRT(3)
      Y2 = VWPRT(4)
C
C SAVE NORMALIZATION TRANS 1
C
      CALL GQNT (1,IERR,WNDW,VWPRT)
C
C DEFINE NORMALIZATION TRANS AND LOG SCALING
C
      CALL SET(X1, X2, Y1, Y2, 1.0, FLOAT(NX), 1.0, FLOAT(NY), 1)
      GO TO 111
  107 CONTINUE
      X1 = XLT
      X2 = XLT+SIDE
      Y1 = YBT
      Y2 = YBT+SIDE
      X3 = NX
      Y3 = NY
      IF (AMIN1(X3,Y3)/AMAX1(X3,Y3) .LT. EXT) GO TO 110
      IF (NX-NY) 108,110,109
  108 X2 = SIDE*X3/Y3+XLT
      GO TO 110
  109 Y2 = SIDE*Y3/X3+YBT
C
C SAVE NORMALIZATION TRANS 1
C
  110 CALL GQNT ( 1, IERR, WNDW, VWPRT )
C
C DEFINE NORMALIZATION TRANS 1 AND LOG SCALING
C
      CALL SET(X1,X2,Y1,Y2,1.0,X3,1.0,Y3,1)
C
C DRAW PERIMETER
C
      CALL PERIM (NX-1,1,NY-1,1)
  111 IF (ICNST .NE. 0) GO TO 124
C
C SET UP LABEL SCALING
C
      IOFFDT = IOFFD
      IF (GL.NE.0.0 .AND. (ABS(GL).LT.0.1 .OR. ABS(GL).GE.1.E5))
     1    IOFFDT = 1
      IF (HA.NE.0.0 .AND. (ABS(HA).LT.0.1 .OR. ABS(HA).GE.1.E5))
     1    IOFFDT = 1
      ASH = 10.**(3-IFIX(ALOG10(AMAX1(ABS(GL),ABS(HA),ABS(GP)))-5000.)-
     1                                                             5000)
      IF (IOFFDT .EQ. 0) ASH = 1.
      IF (IOFFM .NE. 0) GO TO 115
      IWORK = 'CONTOUR FROM              TO              CONTOUR INTERVAL
     1 OF              PT(3,3)=              LABELS SCALED BY'
      HOLD(1) = GL
      HOLD(2) = HA
      HOLD(3) = GP
      HOLD(4) = Z(3,3)
      HOLD(5) = ASH
      NCHAR = 0
      DO 114 I=1,5
         WRITE ( ENCSCR, '(G13.5)' ) HOLD(I)
         NCHAR = NCHAR+LNGTHS(I)
         DO 113 J=1,13
            NCHAR = NCHAR+1
            IWORK(NCHAR:NCHAR) = ENCSCR(J:J)
  113    CONTINUE
  114 CONTINUE
      IF (ASH .EQ. 1.) NCHAR = NCHAR-13-LNGTHS(5)
C
C SET TEXT INTENSITY TO LOW, AND WRITE TITLE USING NORMALIZATION
C TRANS NUMBER 0
C
      CALL GSTXCI (IRECTX)
      CALL GSELNT (0)
      CALL WTSTR ( 0.5, 0.015625, IWORK(1:NCHAR), 0, 0, 0 )
      CALL GSELNT (1)
C
C
C
C                       * * * * * * * * * *
C                            * * * * * * * * * *
C
C
C PROCESS EACH LEVEL
C
  115 FPART = .5
C
      DO 123 I=1,NCL
         CONTR = CL(I)
         NDASH = IDASH
         IF (NEGPOS.LT.0 .AND. CONTR.GE.0.) NDASH = ISOLID
C
C CHANGE 10 BIT PATTERN TO 10 CHARACTER PATTERN.
C
         DO 116 J=1,10
            IBIT = IAND(ISHIFT(NDASH,(J-10)),1)
            RCHAR = IGAP
            IF (IBIT .NE. 0) RCHAR = ISOL
            IWORK(J:J) = RCHAR
  116    CONTINUE
         IF (I .GT. NML) GO TO 121
C
C SET UP MAJOR LINE (LABELED)
C
C
C NREP REPITITIONS OF PATTERN PER LABEL.
C
         NCHAR = 10
         IF (NREP .LT. 2) GO TO 119
         DO 118 J=1,10
            NCHAR = J
            RCHAR = IWORK(J:J)
            DO 117 K=2,NREP
               NCHAR = NCHAR+10
               IWORK(NCHAR:NCHAR) = RCHAR
  117       CONTINUE
  118    CONTINUE
  119    CONTINUE
C
C PUT IN LABEL.
C
         CALL ENCD (CONTR,ASH,ENCSCR,NCUSED,IOFFDT)
         DO 120 J=1,NCUSED
            NCHAR = NCHAR+1
            IWORK(NCHAR:NCHAR) = ENCSCR(J:J)
  120    CONTINUE
         GO TO 122
C
C SET UP MINOR LINE (UNLABELED).
C
  121    CONTINUE
C
C SET LINE INTENSITY TO LOW
C
         CALL GSPLCI ( IRECMN )
         NCHAR = 10
  122    CALL DASHDC ( IWORK(1:NCHAR),NCRT, ISIZEL )
C
C
C DRAW ALL LINES AT THIS LEVEL.
C
         CALL STLINE (Z,MX,NX,NY,CONTR)
C
C SET LINE INTENSITY TO HIGH
C
         CALL GSPLCI ( IRECMJ )
C
  123 CONTINUE
C
C FIND RELATIVE MINIMUMS AND MAXIMUMS IF WANTED, AND MARK VALUES IF
C WANTED.
C
      IF (NHI .EQ. 0) CALL MINMAX (Z,MX,NX,NY,ISIZEM,ASH,IOFFDT)
      IF (NHI .GT. 0) CALL MINMAX (Z,MX,NX,NY,ISIZEP,-ASH,IOFFDT)
      FPART = 1.
      GO TO 127
  124 CONTINUE
         IWORK = 'CONSTANT FIELD'
      WRITE( ENCSCR, '(G22.14)' ) GL
      DO 126 I=1,22
         IWORK(I+14:I+14) = ENCSCR(I:I)
  126 CONTINUE
C
C WRITE TITLE USING NORMALIZATION TRNS 0
C
      CALL GSELNT (0)
      CALL WTSTR ( 0.09765, 0.48825, IWORK(1:36), 3, 0, -1 )
C
C RESTORE NORMALIZATION TRANS 1, LINE AND TEXT INTENSITY TO ORIGINAL
C
  127 IF (NSET.LE.0) THEN
          CALL SET(VWPRT(1),VWPRT(2),VWPRT(3),VWPRT(4),
     -             WNDW(1),WNDW(2),WNDW(3),WNDW(4),IOLLS)
      END IF
      CALL GSPLCI ( IPLCI )
      CALL GSTXCI ( ITXCI )
C
C SELECT ORIGINAL NORMALIZATION TRANS NUMBER NTORIG, AND RESTORE ASF
C
      CALL GSELNT ( NTORIG )
      LASF(3)  = LSV3
      LASF(10) = LSV10
      CALL GSASF ( LASF )
C
      RETURN
C
C
      END
