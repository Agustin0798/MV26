#include "main.h"
 
void error(int ce)
{
    if (ce == InstInv)
        printf("\nINSTRUCCION INVALIDA\n");
    else if (ce == DivCero)
        printf("\nDIVICION POR CERO\n");
    else if (ce == FalloSeg)
        printf("\nFALLO DE SEGMENTO\n");
    exit(1);
}