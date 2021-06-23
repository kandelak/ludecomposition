#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "matr_utilities.h"

int tests()
{

       
        return 0;
}
/*
Eingaben für den OnlineRechner
############### Eingabe: 7x7-Matrix (Q mit Zahlen über 8 Stellen)
############### 2 2 76 1200 -134535 -1543 24 -45.423 8 1 45098 4.9876 768.234 -3
65 0 5 0 124 345.75 -214
8961234 1 -2 -423 -1.346345 -5 -430.4569
537808 76.966359 -829386 933768 3372.22177 -664105.0054 -3.55908
645 262161.9 -8924 -198.47847 77405 -298598.539 4.43
-635.39543 -8763 470150.09 -31685.3 -51 -180299 73

############### Eingabe: 6x6-Matrix (Q mit Zahlen mit max 6 Stellen)
############### 655.08 132.35 -444.69 -293.09 -33.3688 480.9 -153.38 -338.36
403.94 -611.8 43545 -3.698 455.2 -532.68 858.73 -794.67 -444.9 -3.0944 -409.17
-583.37 -202.31 -547.79 -3581.37 -337.7 55.4283 727.074 751.8 -36.649 -342.610
5452.07 -3.8745 -127.561 -641.719 31.6518 -10065 0.2249



Notes:

{53, 71, -73, -31},
{0, 0, 0, 0},
{84, 53, 62, -66},
{50, -19, 53, 68}
ERROR: matrix is singular (auch wenn eine splate nur 0er)

Matrix muss laut kit Satz 2.11 regular sein damit P*A eine L*R hat
(Sei A∈RN,N regulär. Dann existiert eine Permutationsmatrix P,
so dass PA eine LR-Zerlegung PA=LR besitzt und für die Einträge|L[m,n]| ≤
1gilt.)


Beste möglichen Laufzeiten: Kit S.49
Auswertung von P benötigt O(N2)Operationen,
während das Lösen von mit der LR-Zerlegung O(N3) Operationen benötigt.


Pseudo-Code
https://de.wikipedia.org/wiki/Gau%C3%9Fsches_Eliminationsverfahren#LR-Zerlegung
http://www.am.uni-duesseldorf.de/~helzel/Lehre/WiSe1920/CompLinA/vorl/VL8.pdf



Matrix-Calc (auch LR-Zerlegung mit P)
https://matrixcalc.org/de/
*/
