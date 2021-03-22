#ifndef MENU_FUELSENSOR_IRRTANK_COMMON_H_
#define MENU_FUELSENSOR_IRRTANK_COMMON_H_

///////////////////////////////////////////////////////////////////////////////////////////////
//setnum definitions for this application
//manual
#define X_SETNUM_CHARNUMBER_SELECTED 0
#define A_SETNUM_CHARNUMBER_SELECTED 1

#define X_ASCHAR_NUMINTEGERS 3
#define X_ASCHAR_NUMDECIMALS 2

#define A_ASCHAR_NUMINTEGERS 4
#define A_ASCHAR_NUMDECIMALS 2

//automatic
#define X_ASCHAR_BUFFWITDH (X_ASCHAR_NUMINTEGERS+1+X_ASCHAR_NUMDECIMALS)//integers+dp+decimals+'\0'
#define X_ASCHAR_DIGPOS_DP X_ASCHAR_NUMINTEGERS//coincide a nivel de indice
#define X_ASCHAR_DIGPOS_ENDOFDIGIT (X_ASCHAR_NUMINTEGERS+1+X_ASCHAR_NUMDECIMALS)-1

#define A_ASCHAR_BUFFWITDH (A_ASCHAR_NUMINTEGERS+1+A_ASCHAR_NUMDECIMALS)
#define A_ASCHAR_DIGPOS_DP A_ASCHAR_NUMINTEGERS//coincide a nivel de indice
#define A_ASCHAR_DIGPOS_ENDOFDIGIT (A_ASCHAR_NUMINTEGERS+1+A_ASCHAR_NUMDECIMALS)-1

extern int8_t inode;
void splineTable_node_checkAndset_lowerLimit(int8_t inode);//check to left
void splineTable_node_checkAndset_upperLimit(int8_t inode);//check to right

uint8_t mFuelSensor_irrTank_setnum_number_printxy_getFil(uint8_t selected);
uint8_t mFuelSensor_irrTank_setnum_number_printxy_getCol(uint8_t selected);

///////////////////////////////////////////////////////////////////////////////////////////////
//

//#define MGENERIC_CAPTION_DELETE 0 //for message in warning_minNode
//#define MGENERIC_CAPTION_EXIT 1 //for message in warning_minNode
uint8_t spTable_disp_warning_minNode(uint8_t node_operation);
//
uint8_t spTable_disp_warning_maxNode(void);
uint8_t spTable_disp_question_saveNewNode(void);
uint8_t spTable_disp_question_contEnterNewNode(void);
uint8_t spTable_disp_question_exit(void);
uint8_t spTable_disp_error_nodeEqualZero(void);
uint8_t spTable_disp_error_nodeEqualPrevNode(void);
//


#endif
