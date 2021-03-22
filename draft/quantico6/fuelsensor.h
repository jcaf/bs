#ifndef FUELSENSOR_H_
#define FUELSENSOR_H_

#include "spline.h"

enum FS_OUTPUTTYPE {LENGTH=0, VOLUME};

struct _fsEEPROM
{
    uint8_t model;

    struct _outputType
    {
        FS_OUTPUTTYPE type;

        struct _length
        {
            uint8_t units;
        } length;

        struct _volume
        {
            uint8_t units;
        } volume;

    } outputType;

    struct _tank
    {
        uint8_t type;

        struct _rectangular
        {
            struct _area
            {
                float length;
                float width;
            } area;

        } rectangular;

        struct _irregular
        {
            struct _spline
            {
                //
                struct _node
                {
                    float X;//x
                    float A;//f(x)

                } node[SPLINE_NODES_MAX];//#define SPLINE_NODES_MAX in spline.h

                int8_t numberofNodes;
                //

                struct _node_units
                {
                    struct _length
                    {
                        uint8_t units;//fixed to cm RESERVERD
                    }length;

                    struct _volume
                    {
                        uint8_t units;//gallons/liters
                    }volume;

                }node_units;

            } spline;

        } irregular;

    } tank;

    struct _calib//ration
    {
        struct _tank
        {
            float innertank;//d1 cm
            float zero2full;//d2 cm
            float full2upper;//d3 cm

        } tank;

    } calib;

    struct
    {
        unsigned full_calib:1;
        unsigned zero_calib:1;
        unsigned lengthsTankdepth_consistent:1;
        unsigned rectangular_area_consistent:1;
        unsigned irregular_spline_nodetable_consistent:1;
        unsigned enable_get_volume:1;
        unsigned __a:2;
    } bf;
};

//struct _spline_polyvector
//{
//    float A[SPLINE_NODES_MAX];
//    float B[SPLINE_NODES_MAX];
//    float C[SPLINE_NODES_MAX];
//    float D[SPLINE_NODES_MAX];
//};


struct _fuelsensor
{
    float value;
    struct
    {
        unsigned do_fullzero:1;
        unsigned __a:7;

    } bf;

    struct _request
    {
        uint8_t cmd;
        int8_t response_ext;

        struct
        {
            unsigned pending:1;
            unsigned __a:7;

        } bf;


    }request;

    struct _fsEEPROM fsEE;
};
#define EEPROM_ADDR_FSEE 0x0000
extern struct _fuelsensor fuelsensor;

void fuelsensor_D(void);

enum
{
    FS_CMD_CODE_SET_BAUDRATE,
    FS_CMD_CODE_ZERO_ADJ,
    FS_CMD_CODE_FULLRANGE_ADJ,
    FS_CMD_CODE_SET_FUELTANKVOL,
    FS_CMD_CODE_READ_FUELLEVEL,
    FS_CMD_CODE_READSET_SENSORID,
    FS_CMD_CODE_READSET_DAMPING,
    FS_CMD_CODE_FACTORY_RESET,

};
void fuelsensor_init(void);
void fuelsensor_check_consistency(void);
void fuelsensor_job(void);
//

#define FS_MODEL_JT606X 0
//#define FS_MODEL_ 1
//#define FS_MODEL_ 2

#define FS_DO_FULL_CALIB 1
#define FS_DO_ZERO_CALIB 0
//
#define FS_UNITMEA_LENGTH_PERCENTAGE 0
#define FS_UNITMEA_LENGTH_CENTIMETERS 1
#define FS_UNITMEA_LENGTH_METERS 2

#define FS_UNITMEA_VOLUME_GALLONS 0
#define FS_UNITMEA_VOLUME_LITERS 1

#define FS_TANK_TYPE_RECTANGULAR 0
#define FS_TANK_TYPE_IRREGULAR 1

//ERRORS
#define FS_VALUE_ERROR_SPLINE_OUT_LIMITS -1
#define FS_VALUE_ERROR_NO_CALIBRATED -2
#define FS_VALUE_ERROR_NEGATIVE -3


String fuelsensor_reading(void);

void fuelsensor_inhibit_comm(void);
void fuelsensor_set_comm(void);

#include <EEPROM.h>
#define fuelsensor_loaddata() do{EEPROM.get(EEPROM_ADDR_FSEE, fuelsensor.fsEE);}while(0)
#define fuelsensor_savedata() do{EEPROM.put(EEPROM_ADDR_FSEE, fuelsensor.fsEE);}while(0)


#endif // FUELSENSOR_H_


