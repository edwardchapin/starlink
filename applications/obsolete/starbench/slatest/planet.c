/*
** Copyright (C) 2005 P.T.Wallace.
** Use for profit prohibited - enquiries to ptw@tpsoft.demon.co.uk.
*/
#include "sbmlib.h"
#include "sbmmac.h"
void sbmplanet(double qfoo,int qbar,double qbaz[6],int*Q0)
#define qfobar 0.01720209895
#define q1 (qfobar/86400.0)
#define q2 (36525.0*86400.0)
#define qfoobar 0.3977771559319137
#define Q3 0.9174820620691818
{int q4,qfOBAz,qfoobaz;double QQUUX,Q5,QFRED,qdog,qcat,QFISH
,QgASp,Q6,q7,q8,QBAD,qBuG,qsilly,QBUGGY[3],QMUM[3],qDAd,q9,
Q10,Q11,q12,Q13,Q14,qdisk,Q15,q16,q17,QEMPTY,q18,QFULL,qfast
,qsmall,QBIG,QOK,QHELLO,QBYE,QMAGIC,q19,q20,qobSCUrE,QSPEED,
qIndex,Q21;static double qbill[]={6023600.0,408523.5,
328900.5,3098710.0,1047.355,3498.5,22869.0,19314.0};static 
double q22[8][3]={{0.3870983098,0.0,0.0},{0.7233298200,0.0,
0.0},{1.0000010178,0.0,0.0},{1.5236793419,3e-10,0.0},{
5.2026032092,19132e-10,-39e-10},{9.5549091915,-0.0000213896,
444e-10},{19.2184460618,-3716e-10,979e-10},{30.1103868694,-
16635e-10,686e-10}};static double q23[8][3]={{252.25090552,
5381016286.88982,-1.92789},{181.97980085,2106641364.33548,
0.59381},{100.46645683,1295977422.83429,-2.04411},{
355.43299958,689050774.93988,0.94264},{34.35151874,
109256603.77991,-30.60378},{50.07744430,43996098.55732,
75.61614},{314.05500511,15424811.93933,-1.75083},{
304.34866548,7865503.20744,0.21103}};static double qjoe[8][3
]={{0.2056317526,0.0002040653,-28349e-10},{0.0067719164,-
0.0004776521,98127e-10},{0.0167086342,-0.0004203654,-
0.0000126734},{0.0934006477,0.0009048438,-80641e-10},{
0.0484979255,0.0016322542,-0.0000471366},{0.0555481426,-
0.0034664062,-0.0000643639},{0.0463812221,-0.0002729293,
0.0000078913},{0.0094557470,0.0000603263,0.0}};static double
 qemacs[8][3]={{77.45611904,5719.11590,-4.83016},{
131.56370300,175.48640,-498.48184},{102.93734808,11612.35290
,53.27577},{336.06023395,15980.45908,-62.32800},{14.33120687
,7758.75163,259.95938},{93.05723748,20395.49439,190.25952},{
173.00529106,3215.56238,-34.09288},{48.12027554,1050.71912,
27.39717}};static double q24[8][3]={{7.00498625,-214.25629,
0.28977},{3.39466189,-30.84437,-11.67836},{0.0,469.97289,-
3.35053},{1.84972648,-293.31722,-8.11830},{1.30326698,-
71.55890,11.95297},{2.48887878,91.85195,-17.66225},{
0.77319689,-60.72723,1.25759},{1.76995259,8.12333,0.08135}};
static double QVI[8][3]={{48.33089304,-4515.21727,-31.79892}
,{76.67992019,-10008.48154,-51.32614},{174.87317577,-
8679.27034,15.34191},{49.55809321,-10620.90088,-230.57416},{
100.46440702,6362.03561,326.52178},{113.66550252,-9240.19942
,-66.23743},{74.00595701,2669.15033,145.93964},{131.78405702
,-221.94322,-0.78728}};static double qrms[8][9]={{69613.0,
75645.0,88306.0,59899.0,15746.0,71087.0,142173.0,3086.0,0.0}
,{21863.0,32794.0,26934.0,10931.0,26250.0,43725.0,53867.0,
28939.0,0.0},{16002.0,21863.0,32004.0,10931.0,14529.0,
16368.0,15318.0,32794.0,0.0},{6345.0,7818.0,15636.0,7077.0,
8184.0,14163.0,1107.0,4872.0,0.0},{1760.0,1454.0,1167.0,
880.0,287.0,2640.0,19.0,2047.0,1454.0},{574.0,0.0,880.0,
287.0,19.0,1760.0,1167.0,306.0,574.0},{204.0,0.0,177.0,
1265.0,4.0,385.0,200.0,208.0,204.0},{0.0,102.0,106.0,4.0,
98.0,1367.0,487.0,204.0,0.0}};static double QfbI[8][9]={{4.0
,-13.0,11.0,-9.0,-9.0,-3.0,-1.0,4.0,0.0},{-156.0,59.0,-42.0,
6.0,19.0,-20.0,-10.0,-12.0,0.0},{64.0,-152.0,62.0,-8.0,32.0,
-41.0,19.0,-11.0,0.0},{124.0,621.0,-145.0,208.0,54.0,-57.0,
30.0,15.0,0.0},{-23437.0,-2634.0,6601.0,6259.0,-1507.0,-
1821.0,2620.0,-2115.0,-1489.0},{62911.0,-119919.0,79336.0,
17814.0,-24241.0,12068.0,8306.0,-4893.0,8902.0},{389061.0,-
262125.0,-44088.0,8387.0,-22976.0,-2093.0,-615.0,-9720.0,
6633.0},{-412235.0,-157046.0,-31430.0,37817.0,-9740.0,-13.0,
-7449.0,9644.0,0.0}};static double Qcia[8][9]={{-29.0,-1.0,
9.0,6.0,-6.0,5.0,4.0,0.0,0.0},{-48.0,-125.0,-26.0,-37.0,18.0
,-13.0,-20.0,-2.0,0.0},{-150.0,-46.0,68.0,54.0,14.0,24.0,-
28.0,22.0,0.0},{-621.0,532.0,-694.0,-20.0,192.0,-94.0,71.0,-
73.0,0.0},{-14614.0,-19828.0,-5869.0,1881.0,-4372.0,-2255.0,
782.0,930.0,913.0},{139737.0,0.0,24667.0,51123.0,-5102.0,
7429.0,-4095.0,-1976.0,-9566.0},{-138081.0,0.0,37205.0,-
49039.0,-41901.0,-33872.0,-27037.0,-12474.0,18797.0},{0.0,
28492.0,133236.0,69654.0,52322.0,-49577.0,-26430.0,-3593.0,
0.0}};static double Q25[8][10]={{3086.0,15746.0,69613.0,
59899.0,75645.0,88306.0,12661.0,2658.0,0.0,0.0},{21863.0,
32794.0,10931.0,73.0,4387.0,26934.0,1473.0,2157.0,0.0,0.0},{
10.0,16002.0,21863.0,10931.0,1473.0,32004.0,4387.0,73.0,0.0,
0.0},{10.0,6345.0,7818.0,1107.0,15636.0,7077.0,8184.0,532.0,
10.0,0.0},{19.0,1760.0,1454.0,287.0,1167.0,880.0,574.0,
2640.0,19.0,1454.0},{19.0,574.0,287.0,306.0,1760.0,12.0,31.0
,38.0,19.0,574.0},{4.0,204.0,177.0,8.0,31.0,200.0,1265.0,
102.0,4.0,204.0},{4.0,102.0,106.0,8.0,98.0,1367.0,487.0,
204.0,4.0,102.0}};static double Q26[8][10]={{21.0,-95.0,-
157.0,41.0,-5.0,42.0,23.0,30.0,0.0,0.0},{-160.0,-313.0,-
235.0,60.0,-74.0,-76.0,-27.0,34.0,0.0,0.0},{-325.0,-322.0,-
79.0,232.0,-52.0,97.0,55.0,-41.0,0.0,0.0},{2268.0,-979.0,
802.0,602.0,-668.0,-33.0,345.0,201.0,-55.0,0.0},{7610.0,-
4997.0,-7689.0,-5841.0,-2617.0,1115.0,-748.0,-607.0,6074.0,
354.0},{-18549.0,30125.0,20012.0,-730.0,824.0,23.0,1289.0,-
352.0,-14767.0,-2062.0},{-135245.0,-14594.0,4197.0,-4030.0,-
5630.0,-2898.0,2540.0,-306.0,2939.0,1986.0},{89948.0,2103.0,
8963.0,2695.0,3682.0,1648.0,866.0,-154.0,-1963.0,-283.0}};
static double QNASA[8][10]={{-342.0,136.0,-23.0,62.0,66.0,-
52.0,-33.0,17.0,0.0,0.0},{524.0,-149.0,-35.0,117.0,151.0,
122.0,-71.0,-62.0,0.0,0.0},{-105.0,-137.0,258.0,35.0,-116.0,
-88.0,-112.0,-80.0,0.0,0.0},{854.0,-205.0,-936.0,-240.0,
140.0,-341.0,-97.0,-232.0,536.0,0.0},{-56980.0,8016.0,1012.0
,1448.0,-3024.0,-3710.0,318.0,503.0,3767.0,577.0},{138606.0,
-13478.0,-4964.0,1441.0,-1319.0,-1482.0,427.0,1236.0,-9167.0
,-1918.0},{71234.0,-41116.0,5334.0,-4935.0,-1848.0,66.0,
434.0,-1748.0,3780.0,-701.0},{-47645.0,11647.0,2166.0,3194.0
,679.0,0.0,-244.0,-419.0,-2531.0,48.0}};static double QERR=
34.35,Q27=3034.9057,Q28=50.08,qgoogle=1222.1138,q29=238.96,
QYahoO=144.9600;static double Q30=238.956785,qtrick=144.96,
q31=-3.908202,Q32=40.7247248;struct QHINT{double q22;double 
Q33;};struct tm{int Q34;int QBLAcK;int q4;struct QHINT Q35[3
];};static struct tm q36[]={{0,0,1,{{-19798886e-6,
19848454e-6},{-5453098e-6,-14974876e-6},{66867334e-7,
68955876e-7}}},{0,0,2,{{897499e-6,-4955707e-6},{3527363e-6,
1672673e-6},{-11826086e-7,-333765e-7}}},{0,0,3,{{610820e-6,
1210521e-6},{-1050939e-6,327763e-6},{1593657e-7,-1439953e-7}
}},{0,0,4,{{-341639e-6,-189719e-6},{178691e-6,-291925e-6},{-
18948e-7,482443e-7}}},{0,0,5,{{129027e-6,-34863e-6},{
18763e-6,100448e-6},{-66634e-7,-85576e-7}}},{0,0,6,{{-
38215e-6,31061e-6},{-30594e-6,-25838e-6},{30841e-7,-5765e-7}
}},{0,1,-1,{{20349e-6,-9886e-6},{4965e-6,11263e-6},{-6140e-7
,22254e-7}}},{0,1,0,{{-4045e-6,-4904e-6},{310e-6,-132e-6},{
4434e-7,4443e-7}}},{0,1,1,{{-5885e-6,-3238e-6},{2036e-6,-
947e-6},{-1518e-7,641e-7}}},{0,1,2,{{-3812e-6,3011e-6},{-
2e-6,-674e-6},{-5e-7,792e-7}}},{0,1,3,{{-601e-6,3468e-6},{-
329e-6,-563e-6},{518e-7,518e-7}}},{0,2,-2,{{1237e-6,463e-6},
{-64e-6,39e-6},{-13e-7,-221e-7}}},{0,2,-1,{{1086e-6,-911e-6}
,{-94e-6,210e-6},{837e-7,-494e-7}}},{0,2,0,{{595e-6,-1229e-6
},{-8e-6,-160e-6},{-281e-7,616e-7}}},{1,-1,0,{{2484e-6,-
485e-6},{-177e-6,259e-6},{260e-7,-395e-7}}},{1,-1,1,{{839e-6
,-1414e-6},{17e-6,234e-6},{-191e-7,-396e-7}}},{1,0,-3,{{-
964e-6,1059e-6},{582e-6,-285e-6},{-3218e-7,370e-7}}},{1,0,-2
,{{-2303e-6,-1038e-6},{-298e-6,692e-6},{8019e-7,-7869e-7}}},
{1,0,-1,{{7049e-6,747e-6},{157e-6,201e-6},{105e-7,45637e-7}}
},{1,0,0,{{1179e-6,-358e-6},{304e-6,825e-6},{8623e-7,8444e-7
}}},{1,0,1,{{393e-6,-63e-6},{-124e-6,-29e-6},{-896e-7,-
801e-7}}},{1,0,2,{{111e-6,-268e-6},{15e-6,8e-6},{208e-7,-
122e-7}}},{1,0,3,{{-52e-6,-154e-6},{7e-6,15e-6},{-133e-7,
65e-7}}},{1,0,4,{{-78e-6,-30e-6},{2e-6,2e-6},{-16e-7,1e-7}}}
,{1,1,-3,{{-34e-6,-26e-6},{4e-6,2e-6},{-22e-7,7e-7}}},{1,1,-
2,{{-43e-6,1e-6},{3e-6,0e-6},{-8e-7,16e-7}}},{1,1,-1,{{-
15e-6,21e-6},{1e-6,-1e-6},{2e-7,9e-7}}},{1,1,0,{{-1e-6,15e-6
},{0e-6,-2e-6},{12e-7,5e-7}}},{1,1,1,{{4e-6,7e-6},{1e-6,0e-6
},{1e-7,-3e-7}}},{1,1,3,{{1e-6,5e-6},{1e-6,-1e-6},{1e-7,0e-7
}}},{2,0,-6,{{8e-6,3e-6},{-2e-6,-3e-6},{9e-7,5e-7}}},{2,0,-5
,{{-3e-6,6e-6},{1e-6,2e-6},{2e-7,-1e-7}}},{2,0,-4,{{6e-6,-
13e-6},{-8e-6,2e-6},{14e-7,10e-7}}},{2,0,-3,{{10e-6,22e-6},{
10e-6,-7e-6},{-65e-7,12e-7}}},{2,0,-2,{{-57e-6,-32e-6},{0e-6
,21e-6},{126e-7,-233e-7}}},{2,0,-1,{{157e-6,-46e-6},{8e-6,
5e-6},{270e-7,1068e-7}}},{2,0,0,{{12e-6,-18e-6},{13e-6,16e-6
},{254e-7,155e-7}}},{2,0,1,{{-4e-6,8e-6},{-2e-6,-3e-6},{-
26e-7,-2e-7}}},{2,0,2,{{-5e-6,0e-6},{0e-6,0e-6},{7e-7,0e-7}}
},{2,0,3,{{3e-6,4e-6},{0e-6,1e-6},{-11e-7,4e-7}}},{3,0,-2,{{
-1e-6,-1e-6},{0e-6,1e-6},{4e-7,-14e-7}}},{3,0,-1,{{6e-6,-
3e-6},{0e-6,0e-6},{18e-7,35e-7}}},{3,0,0,{{-1e-6,-2e-6},{
0e-6,1e-6},{13e-7,3e-7}}}};if(qbar<1||qbar>9){*Q0=-1;for(
qfOBAz=0;qfOBAz<=5;qfOBAz++)qbaz[qfOBAz]=0.0;return;}else{q4
=qbar-1;}if(qbar!=9){QQUUX=(qfoo-51544.5)/365250.0;*Q0=(fabs
(QQUUX)<=1.0)?0:1;Q5=q22[q4][0]+(q22[q4][1]+q22[q4][2]*QQUUX
)*QQUUX;q16=(3600.0*q23[q4][0]+(q23[q4][1]+q23[q4][2]*QQUUX)
*QQUUX)*DAS2R;QFRED=qjoe[q4][0]+(qjoe[q4][1]+qjoe[q4][2]*
QQUUX)*QQUUX;qdog=dmod((3600.0*qemacs[q4][0]+(qemacs[q4][1]+
qemacs[q4][2]*QQUUX)*QQUUX)*DAS2R,D2PI);qcat=(3600.0*q24[q4]
[0]+(q24[q4][1]+q24[q4][2]*QQUUX)*QQUUX)*DAS2R;QFISH=dmod((
3600.0*QVI[q4][0]+(QVI[q4][1]+QVI[q4][2]*QQUUX)*QQUUX)*DAS2R
,D2PI);QgASp=0.35953620*QQUUX;for(qfoobaz=0;qfoobaz<=7;
qfoobaz++){Q6=qrms[q4][qfoobaz]*QgASp;q7=Q25[q4][qfoobaz]*
QgASp;Q5+=(QfbI[q4][qfoobaz]*cos(Q6)+Qcia[q4][qfoobaz]*sin(
Q6))*1e-7;q16+=(Q26[q4][qfoobaz]*cos(q7)+QNASA[q4][qfoobaz]*
sin(q7))*1e-7;}Q6=qrms[q4][8]*QgASp;Q5+=QQUUX*(QfbI[q4][8]*
cos(Q6)+Qcia[q4][8]*sin(Q6))*1e-7;for(qfoobaz=8;qfoobaz<=9;
qfoobaz++){q7=Q25[q4][qfoobaz]*QgASp;q16+=QQUUX*(Q26[q4][
qfoobaz]*cos(q7)+QNASA[q4][qfoobaz]*sin(q7))*1e-7;}q16=dmod(
q16,D2PI);q8=qfobar*sqrt((1.0+1.0/qbill[q4])/(Q5*Q5*Q5));
sbmplanel(qfoo,1,qfoo,qcat,QFISH,qdog,Q5,QFRED,q16,q8,qbaz,&
qfoobaz);if(qfoobaz<0)*Q0=-2;}else{QQUUX=(qfoo-51544.5)/
36525.0;*Q0=QQUUX>=-1.15&&QQUUX<=1.0?0:-1;QBAD=(QERR+Q27*
QQUUX)*DD2R;qBuG=(Q28+qgoogle*QQUUX)*DD2R;qsilly=(q29+QYahoO
*QQUUX)*DD2R;for(qfOBAz=0;qfOBAz<3;qfOBAz++){QBUGGY[qfOBAz]=
0.0;QMUM[qfOBAz]=0.0;}for(qfoobaz=0;qfoobaz<(sizeof q36/
sizeof q36[0]);qfoobaz++){qDAd=(double)(q36[qfoobaz].Q34);q9
=(double)(q36[qfoobaz].QBLAcK);Q10=(double)(q36[qfoobaz].q4)
;Q11=qDAd*QBAD+q9*qBuG+Q10*qsilly;q12=(qDAd*Q27+q9*qgoogle+
Q10*QYahoO)*DD2R;Q13=sin(Q11);Q14=cos(Q11);for(qfOBAz=0;
qfOBAz<3;qfOBAz++){qdisk=q36[qfoobaz].Q35[qfOBAz].q22;Q15=
q36[qfoobaz].Q35[qfOBAz].Q33;QBUGGY[qfOBAz]=QBUGGY[qfOBAz]+
qdisk*Q13+Q15*Q14;QMUM[qfOBAz]=QMUM[qfOBAz]+(qdisk*Q14-Q15*
Q13)*q12;}}q16=(Q30+qtrick*QQUUX+QBUGGY[0])*DD2R;q17=(qtrick
+QMUM[0])*DD2R/q2;QEMPTY=(q31+QBUGGY[1])*DD2R;q18=QMUM[1]*
DD2R/q2;QFULL=Q32+QBUGGY[2];qfast=QMUM[2]/q2;qsmall=sin(q16)
;QBIG=cos(q16);QOK=sin(QEMPTY);QHELLO=cos(QEMPTY);QBYE=
qsmall*QHELLO;QMAGIC=QBIG*QHELLO;q19=QFULL*QMAGIC;q20=QFULL*
QBYE;qobSCUrE=QFULL*QOK;QSPEED=qfast*QMAGIC-QFULL*(QBIG*QOK*
q18+QBYE*q17);qIndex=qfast*QBYE+QFULL*(-qsmall*QOK*q18+
QMAGIC*q17);Q21=qfast*QOK+QFULL*QHELLO*q18;qbaz[0]=q19;qbaz[
1]=q20*Q3-qobSCUrE*qfoobar;qbaz[2]=q20*qfoobar+qobSCUrE*Q3;
qbaz[3]=QSPEED;qbaz[4]=qIndex*Q3-Q21*qfoobar;qbaz[5]=qIndex*
qfoobar+Q21*Q3;}}
