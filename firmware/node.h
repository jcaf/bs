#ifndef NODE_H_
#define NODE_H_

void    sptable_set_node_X(uint8_t inode, double X);
void    sptable_set_node_A(uint8_t inode,double A);
void    sptable_set_node(uint8_t inode, double X, double A);
double  sptable_get_node_X(uint8_t inode);
double  sptable_get_node_A(uint8_t inode);
void    sptable_numberofNodes_inc(uint8_t i=1);
void    sptable_numberofNodes_dec(uint8_t i=1);
uint8_t sptable_get_numberofNodes(void);
void    sptable_set_numberofNodes(uint8_t inode);
uint8_t sptable_node_GTEQ_maxNode(uint8_t inode);
uint8_t sptable_node_GTEQ_minNode(uint8_t inode);//>=SPLINE_NODES_MIN
uint8_t sptable_numberofNodes_EQ_maxNode(void);
uint8_t sptable_numberofNodes_EQ_minNode(void);
uint8_t sptable_numberofNodes_GTEQ_minNode(void);

uint8_t node_insert_rigth(int8_t inode, float X, float A);
int8_t  node_del(int8_t *inode);


#endif // NODE_H_
