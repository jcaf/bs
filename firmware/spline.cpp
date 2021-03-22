/****************************************************************************
* Project: ${PROJECT_FILENAME}
*
* spline.cpp
*
*
* Created: 10/09/2016 06:03:12 p.m.
*  Author: jcaf
*
* Copyright 2016 Juan Carlos AgÃ¼ero Flores
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
****************************************************************************/
#include <Arduino.h>
#include "system.h"
#include "utils.h"
#include "spline.h"
#include "fuelsensor.h"
#include "EEPROM.h"
#include "node.h"

//float get_spline(float xi, float *X, float *A, uint8_t A_Length)
//float spline_poly_get_coeffs(float *X, float *A, uint8_t A_Length)


void spline_poly_get_coeffs(void)
{
    //    float X[SPLINE_NODES_MAX];//x
    //    float A[SPLINE_NODES_MAX];//f(x)
    //    float B[SPLINE_NODES_MAX];
    //    float C[SPLINE_NODES_MAX];
    //    float D[SPLINE_NODES_MAX];
    //    float Lmin;
    //    float Lmax;//limits
    //    int8_t n;//max num node
    struct _splEE splEE;

    float H[SPLINE_NODES_MAX];
    float L[SPLINE_NODES_MAX];
    float U[SPLINE_NODES_MAX];
    float Z[SPLINE_NODES_MAX];
    float alfa[SPLINE_NODES_MAX];
    int8_t i,j;

    //fill vectors
    uint8_t numberofNodes = sptable_get_numberofNodes();
    for (i=0; i<numberofNodes; i++ )
    {
        splEE.X[i]= sptable_get_node_X(i);
        splEE.A[i]= sptable_get_node_A(i);
    }
    //splEE.n= A_Length -1;
    splEE.n= numberofNodes -1;

    splEE.Lmin = splEE.X[0];
    splEE.Lmax = splEE.X[splEE.n];

    //Step 1
    for (i=0; i<=(splEE.n-1); i++)
        H[i] = splEE.X[i+1] - splEE.X[i];

    //Step 2
    for (i=1; i<=splEE.n-1; i++)
        alfa[i] = ( (3.0/H[i]) * (splEE.A[i+1]-splEE.A[i]) ) - ( (3.0/H[i-1])*(splEE.A[i] - splEE.A[i-1]));


    //Step 3
    L[0] = 1;
    U[0] = 0;
    Z[0] = 0;

    //Step 4
    for (i=1; i<=splEE.n-1; i++)
    {
        L[i] = 2*(splEE.X[i+1] - splEE.X[i-1]) - (H[i-1]*U[i-1]);
        U[i] = H[i]/L[i];
        Z[i] = (alfa[i] - (H[i-1]*Z[i-1]))/L[i];
    }

    //Step 5
    L[splEE.n] = 1;
    Z[splEE.n] = 0;
    splEE.C[splEE.n] = 0;

    //Step 6
    for ( j=splEE.n-1; j >= 0; j--)
    {
        splEE.C[j] = Z[j]-(U[j]*splEE.C[j+1]);
        splEE.B[j] = (splEE.A[j+1]-splEE.A[j]) / H[j]- H[j]*(splEE.C[j+1] + 2*splEE.C[j])/3;
        splEE.D[j] = (splEE.C[j+1]-splEE.C[j])/ (3*H[j]);
    }
    //
    //#include "myEEPROM.h"
    //save  to eeprom
#define EEPROM_ADDR_SPLINE_POLY_X (sizeof(struct _fsEEPROM) + 0)//depues de la structura mapeada #define EEPROM_ADDR_FSEE 0x0000
#define EEPROM_ADDR_SPLINE_POLY_A (EEPROM_ADDR_SPLINE_POLY_X + sizeof(float[SPLINE_NODES_MAX]))
#define EEPROM_ADDR_SPLINE_POLY_B (EEPROM_ADDR_SPLINE_POLY_A + sizeof(float[SPLINE_NODES_MAX]))
#define EEPROM_ADDR_SPLINE_POLY_C (EEPROM_ADDR_SPLINE_POLY_B + sizeof(float[SPLINE_NODES_MAX]))
#define EEPROM_ADDR_SPLINE_POLY_D (EEPROM_ADDR_SPLINE_POLY_C + sizeof(float[SPLINE_NODES_MAX]))

    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_X, splEE.X);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_A, splEE.A);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_B, splEE.B);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_C, splEE.C);
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_D, splEE.D);

#define EEPROM_ADDR_SPLINE_POLY_LMIN (EEPROM_ADDR_SPLINE_POLY_D + sizeof(float[SPLINE_NODES_MAX]))
#define EEPROM_ADDR_SPLINE_POLY_LMAX (EEPROM_ADDR_SPLINE_POLY_LMIN + sizeof(float))
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_LMIN, splEE.Lmin);//float
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_LMAX, splEE.Lmax);

#define EEPROM_ADDR_SPLINE_POLY_N (EEPROM_ADDR_SPLINE_POLY_LMAX + sizeof(float))
    EEPROM.put(EEPROM_ADDR_SPLINE_POLY_N, splEE.n);//int8_t
}

float spline_poly_eval(float xi)
{
    float X[SPLINE_NODES_MAX];//x
    float A[SPLINE_NODES_MAX];//f(x)
    float B[SPLINE_NODES_MAX];
    float C[SPLINE_NODES_MAX];
    float D[SPLINE_NODES_MAX];
    int8_t n;//max num node
    float Lmin, Lmax;//limits

    //
    int8_t i,j;
    int8_t pos;
    float yi;//output
    uint8_t innerlimits;

    //////////////////////////////////////////////////////////////////////////////////
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_X, X);
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_A, A);
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_B, B);
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_C, C);
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_D, D);

    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_LMIN, Lmin);
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_LMAX, Lmax);
    EEPROM.get(EEPROM_ADDR_SPLINE_POLY_N, n);

    //////////////////////////////////////////////////////////////////////////////////
    //    if (fuelsensor.fsEE.bf.irregular_spline_nodetable_consistent)
    //    {
    innerlimits=0;
    if (xi >= Lmin)
    {
        if (xi<Lmax)
        {
            for (j=0; j<=n-1; j++)
            {
                if (xi >= X[j])
                {
                    if (xi < X[j+1])
                        pos = j;
                }
            }
            j = pos;

            //OUTPUT
            yi = A[j] + B[j]*(xi-X[j]) + C[j]*pow((xi-X[j]), 2) + D[j]*pow((xi-X[j]), 3);

            innerlimits=1;
        }
        else if (xi == Lmax)
        {
            yi = A[n];
            innerlimits=1;
        }
    }

    if (innerlimits == 0)
    {
        yi = FS_VALUE_ERROR_SPLINE_OUT_LIMITS;//-1;//printf("OUT OF LIMITS \n");
    }
    //    }
    //    else
    //    {
    //        yi= -2;
    //    }

    return yi;
}


