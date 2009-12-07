      SUBROUTINE MEDCIRC(DATA,DOUT,NPOINT,MEDP,IWORK,WORK,IFAIL)
C
C
C     THIS ROUTINE MEDIAN FILTERS ARRAY DATA(NPOINT)
C     IT FITS AS MANY OF THE SPECIFIED SIZE OF FILTER
C     AS IT CAN GIVEN THE END CONSTRAINTS, AND USES THE MEDIAN
C     AS THE FILTERED VALUE. WRAPS ARRAY AROUND.
C     
C     PASSED: REAL*4 DATA(NPOINT) -- DATA ARRAY TO BE FILTERED
C     INTEGER*4 NPOINT -- NUMBER OF POINTS IN ARRAY
C     INTEGER*4 MEDP   -- WIDTH OF FILTER IN BINS (ODD)
C     INTEGER*4 IWORK -- WORK ARRAY OF AT LEAST MEDP POINTS
C     REAL*4    WORK  -- WORK ARRAY OF AT LEAST MEDP+2 POINTS
C     
C     RETURNED:
C     REAL*4 DOUT(NPOINT) -- FILTERED ARRAY
C     INTEGER*4 IFAIL -- ERROR RETURN = 1 IN CASE OF ERROR
C     
      INTEGER*4 MEDP, IWORK(1)
      REAL*4 DATA(NPOINT),DOUT(NPOINT), WORK(1)
C     
      IFAIL = 0
      IF(MEDP.EQ.1) THEN
         DO I=1,NPOINT
            DOUT(I) = DATA(I)
         END DO
         RETURN
      END IF
      IF(2*(MEDP/2)-MEDP.EQ.0) MEDP = MEDP - 1
      JSTOP = MIN(MEDP,NPOINT)
      DO J = 1, MEDP
         I = J - MEDP/2
         DO WHILE(I.LT.1)
            I = I + NPOINT
         END DO
         DO WHILE(I.GT.NPOINT)
            I = I - NPOINT
         END DO
         WORK(JSTOP+J) = DATA(I)
      END DO
      NACT = JSTOP
C     
C     SORT FIRST FILTER BLOCK
C     
      CALL SHELLSORT( NACT, WORK(JSTOP+1), IWORK)
      DO I = 1, NACT
         WORK(I) = WORK(JSTOP+IWORK(I))
      END DO
      DOUT(1) = WORK(NACT/2+1)
      DO I = 2, NPOINT
         JNEW = I + MEDP/2
         DO WHILE(JNEW.GT.NPOINT)
            JNEW = JNEW - NPOINT
         END DO
C     
C     UPDATE KEY INDEX ARRAY
C     DELETE FIRST POINT
C     
         IAD = 0
         DO J = 1, NACT
            IF(IWORK(J).EQ.1) IAD = 1
            IN = J + IAD
            IWORK(J) = IWORK(IN) - 1
            WORK(J)  = WORK(IN)
         END DO
         NACT = NACT -1
C     
C     ADD EXTRA POINT TO THE END
C     
         TEST = DATA(JNEW)
         NACT = NACT + 1
         ITEST = IFIND(WORK, NACT - 1, TEST)
         IF(ITEST.EQ.NACT) THEN
            IWORK(NACT) = NACT
            WORK(NACT)  = TEST
            GOTO 10
         END IF
         DO J = NACT, ITEST+1,-1
            IWORK(J) = IWORK(J-1)
            WORK(J)  = WORK(J-1)
         END DO
         WORK(ITEST)  = TEST
         IWORK(ITEST) = NACT
 10      DOUT(I) = WORK(NACT/2+1)
      END DO
      RETURN
      END

      SUBROUTINE MEDFILT(DATA,DOUT,NPOINT,MEDP,IFAIL)
*     
*     
*     THIS ROUTINE MEDIAN FILTERS ARRAY DATA(NPOINT)
*     IT FITS AS MANY OF THE SPECIFIED SIZE OF FILTER
*     AS IT CAN GIVEN THE END CONSTRAINTS, AND USES THE MEDIAN
*     AS THE FILTERED VALUE.
*     
*     PASSED: REAL*4 DATA(NPOINT) -- DATA ARRAY TO BE FILTERED
*     INTEGER*4 NPOINT -- NUMBER OF POINTS IN ARRAY
*     INTEGER*4 MEDP -- WIDTH OF FILTER IN BINS
*     
*     RETURNED:
*     REAL*4 DOUT(NPOINT) -- FILTERED ARRAY
*     INTEGER*4 IFAIL -- ERROR RETURN = 1 IN CASE OF ERROR
*     
      PARAMETER (MFMAX = 1001)
      INTEGER MEDP,IWORK(MFMAX)
      REAL DATA(NPOINT),DOUT(NPOINT)
      REAL WORK(MFMAX+2)
*     
      IFAIL = 0
      IF(MEDP.EQ.1) THEN
         DO I=1,NPOINT
            DOUT(I) = DATA(I)
         END DO
         RETURN
      END IF
      IF(2*(MEDP/2)-MEDP.EQ.0) MEDP = MEDP - 1
      IF(MEDP.GT.MFMAX) THEN
         IFAIL = 1
         RETURN
      END IF
      JSTART=1
      JSTOP = MIN(MEDP/2+1,NPOINT)
      NACT  = JSTOP
      DO J=1,NACT
         WORK(JSTOP+J)=DATA(J)
      END DO
*     
*     SORT FIRST FILTER BLOCK
*     
      CALL HEAPSORT( NACT, WORK(JSTOP+1), IWORK)
      DO I=1,NACT
         WORK(I) = WORK(JSTOP+IWORK(I))
      END DO
      DOUT(1) = WORK(NACT/2+1)
      DO I=2,NPOINT
*     
*     FIND START AND STOP POINTS FOR FILTER
*     
         JNEW1 = MAX(I - MEDP/2,1)
         JNEW2 = MIN(I + MEDP/2,NPOINT)
*     
*     UPDATE KEY INDEX ARRAY, IF START HAS CHANGED
*     DELETE FIRST POINT
*     
         IF(JNEW1 - JSTART .EQ. 1) THEN
            IAD = 0
            DO J=1,NACT
               IF(IWORK(J).EQ.1) IAD = 1
               IN = J + IAD
               IWORK(J) = IWORK(IN) - 1
               WORK(J)  = WORK(IN)
            END DO
            NACT = NACT -1
         END IF
*     
*     IF STOP HAS CHANGED
*     ADD EXTRA POINT TO THE END
*     
         IF(JNEW2 - JSTOP .EQ.1) THEN
C Bug fix! 31/08/2006 
C Spotted by Pierre Maxted
C            TEST  = DATA(JSTOP)
            TEST  = DATA(JNEW2)
            NACT  = NACT + 1
            ITEST = IFIND(WORK, NACT - 1, TEST)
            IF(ITEST.EQ.NACT) THEN
               IWORK(NACT) = NACT
               WORK(NACT)  = TEST
               GOTO 10
            END IF
            DO J=NACT,ITEST+1,-1
               IWORK(J) = IWORK(J-1)
               WORK(J)  = WORK(J-1)
            END DO
            WORK(ITEST) = TEST
            IWORK(ITEST) = NACT
         END IF
 10      DOUT(I) = WORK(NACT/2+1)
         JSTART  = JNEW1
         JSTOP   = JNEW2
      END DO
      RETURN
      END
      
      SUBROUTINE MEDFILT1(DATA,DOUT,NPOINT,MEDP,IWORK,WORK,IFAIL)
C     
C     
C     THIS ROUTINE MEDIAN FILTERS ARRAY DATA(NPOINT)
C     IT FITS AS MANY OF THE SPECIFIED SIZE OF FILTER
C     AS IT CAN GIVEN THE END CONSTRAINTS, AND USES THE MEDIAN
C     AS THE FILTERED VALUE.
C     
C     PASSED: REAL*4 DATA(NPOINT) -- DATA ARRAY TO BE FILTERED
C         INTEGER*4 NPOINT -- NUMBER OF POINTS IN ARRAY
C     INTEGER*4 MEDP   -- WIDTH OF FILTER IN BINS (ODD)
C     INTEGER*4 IWORK -- WORK ARRAY OF AT LEAST MEDP POINTS
C     REAL*4    WORK  -- WORK ARRAY OF AT LEAST MEDP+2 POINTS
C     
C     RETURNED:
C     REAL*4 DOUT(NPOINT) -- FILTERED ARRAY
C     INTEGER*4 IFAIL -- ERROR RETURN = 1 IN CASE OF ERROR
C     
      INTEGER*4 MEDP, IWORK(1)
      REAL*4 DATA(NPOINT),DOUT(NPOINT), WORK(1)
C     
      IFAIL = 0
      IF(MEDP.EQ.1) THEN
         DO I=1,NPOINT
            DOUT(I) = DATA(I)
         END DO
         RETURN
      END IF
      IF(2*(MEDP/2)-MEDP.EQ.0) MEDP = MEDP - 1
      JSTART=1
      JSTOP = MIN(MEDP/2+1,NPOINT)
      NACT = JSTOP
      DO J=1,NACT
         WORK(JSTOP+J)=DATA(J)
      END DO
C     
C     SORT FIRST FILTER BLOCK
C     
      CALL SHELLSORT( NACT, WORK(JSTOP+1), IWORK)
      DO I=1,NACT
         WORK(I) = WORK(JSTOP+IWORK(I))
      END DO
      DOUT(1) = WORK(NACT/2+1)
      DO I=2,NPOINT
C     
C     FIND START AND STOP POINTS FOR FILTER
C     
         JNEW1 = MAX(I - MEDP/2,1)
         JNEW2 = MIN(I + MEDP/2,NPOINT)
C     
C     UPDATE KEY INDEX ARRAY, IF START HAS CHANGED
C     DELETE FIRST POINT
C     
         IF(JNEW1 - JSTART .EQ. 1) THEN
            IAD = 0
            NACT = NACT -1
            DO J=1,NACT
               IF(IWORK(J).EQ.1) IAD = 1
               IN = J + IAD
               IWORK(J) = IWORK(IN) - 1
               WORK(J) = WORK(IN)
            END DO
         END IF
C     
C     IF STOP HAS CHANGED
C     ADD EXTRA POINT TO THE END
C     
         IF(JNEW2 - JSTOP .EQ.1) THEN
C Bug fix! 31/08/2006 
C Spotted by Pierre Maxted
C            TEST  = DATA(JSTOP)
            TEST  = DATA(JNEW2)
            NACT  = NACT + 1
            ITEST = IFIND(WORK, NACT - 1, TEST)
            IF(ITEST.EQ.NACT) THEN
               IWORK(NACT) = NACT
               WORK(NACT)  = TEST
               GOTO 10
            END IF
            DO J=NACT,ITEST+1,-1
               IWORK(J) = IWORK(J-1)
               WORK(J)  = WORK(J-1)
            END DO
            WORK(ITEST)  = TEST
            IWORK(ITEST) = NACT
         END IF
 10      DOUT(I) = WORK(NACT/2+1)
         JSTART = JNEW1
         JSTOP  = JNEW2
      END DO
      RETURN
      END
      
      


