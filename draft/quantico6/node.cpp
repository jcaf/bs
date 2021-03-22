/****************************************************************************
* Project: Battery System
*
* 2016-07-15-17.38
* jcaf @ jcafOpenSuse
*
* Copyright 2013 Juan Carlos Agüero Flores
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
#include "node.h"
#include "fuelsensor.h"
#include "spline.h"

void sptable_set_node_X(uint8_t inode, double X)
{
    fuelsensor.fsEE.tank.irregular.spline.node[inode].X = X;
}
void sptable_set_node_A(uint8_t inode,double A)
{
    fuelsensor.fsEE.tank.irregular.spline.node[inode].A = A;
}
void sptable_set_node(uint8_t inode, double X, double A)
{
    sptable_set_node_X(inode, X);
    sptable_set_node_A(inode, A);
}
double sptable_get_node_X(uint8_t inode)
{
    return fuelsensor.fsEE.tank.irregular.spline.node[inode].X;
}
double sptable_get_node_A(uint8_t inode)
{
    return fuelsensor.fsEE.tank.irregular.spline.node[inode].A;
}

void sptable_numberofNodes_inc(uint8_t i)//uint8_t i=1
{
    fuelsensor.fsEE.tank.irregular.spline.numberofNodes+=i;
}
void sptable_numberofNodes_dec(uint8_t i)//uint8_t i=1
{
    fuelsensor.fsEE.tank.irregular.spline.numberofNodes-=i;
}
void sptable_set_numberofNodes(uint8_t inode)
{
    fuelsensor.fsEE.tank.irregular.spline.numberofNodes = inode;
}
uint8_t sptable_get_numberofNodes(void)
{
    return fuelsensor.fsEE.tank.irregular.spline.numberofNodes;
}
uint8_t sptable_numberofNodes_EQ_maxNode(void)
{
    return (fuelsensor.fsEE.tank.irregular.spline.numberofNodes == SPLINE_NODES_MAX);
}
uint8_t sptable_numberofNodes_EQ_minNode(void)
{
    return (fuelsensor.fsEE.tank.irregular.spline.numberofNodes == SPLINE_NODES_MIN);
}
uint8_t sptable_numberofNodes_GTEQ_minNode(void)
{
    return (fuelsensor.fsEE.tank.irregular.spline.numberofNodes >= SPLINE_NODES_MIN);
}

uint8_t sptable_node_GTEQ_maxNode(uint8_t inode)
{
    return (inode >= SPLINE_NODES_MAX);
}

uint8_t sptable_node_GTEQ_minNode(uint8_t inode)//>=SPLINE_NODES_MIN
{
    return (inode >= SPLINE_NODES_MIN);
}


/** \brief Insert new node to the right of current inode
 *
 * \param inode int8_t
 * \param X float
 * \param A float
 * \return uint8_t 1= if was space to insert the new node, 0 = there was no
 *
 */

uint8_t node_insert_rigth(int8_t inode, float X, float A)//inode has been incremented already & point to place-holder entered-number by user
{
    uint8_t cod_ret = 0;
    //if (!sptable_node_GTEQ_maxNode(inode))//there is sufficiente space ?
    {
        if (inode < sptable_get_numberofNodes())//is necessary shift to right?
        {
            for (int8_t i = sptable_get_numberofNodes(); i > inode; --i)
            {
                sptable_set_node(i, sptable_get_node_X(i-1), sptable_get_node_A(i-1));
            }
        }
        sptable_set_node(inode, X, A);

        sptable_numberofNodes_inc();//fuelsensor.fsEE.tank.irregular.spline.numberofNodes++;

        cod_ret = 1;
    }
    return cod_ret;
}


/** \brief
 *
 * \param inode int8_t*
 * \return int8_t fuelsensor.fsEE.tank.irregular.spline.numberofNodes (number of Nodes)
 *
 */
int8_t node_del(int8_t *inode)
{
    if (*inode < (sptable_get_numberofNodes()-1) )
    {
        sptable_numberofNodes_dec();

        uint8_t numberofNodes = sptable_get_numberofNodes();
        for (int8_t i= *inode; i< numberofNodes; ++i)//shift to left
        {
            sptable_set_node(i, sptable_get_node_X(i+1), sptable_get_node_A(i+1));
        }
    }
    else //en la ultima posc.
    {
        sptable_numberofNodes_dec();

        if (sptable_get_numberofNodes() > 0)
           --*inode; // q es lo mismo: (*inode)--; parsing from right to left (have the same precedence)
    }
    return sptable_get_numberofNodes();
}
