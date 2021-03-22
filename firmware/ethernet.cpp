#include <Ethernet.h>
#include <EthernetDHCP.h>
#include "utility/w5100.h"
#include "utility/socket.h"
#include <SPI.h>
#include "system.h"
#include "ethernet.h"
#include "lcd_fxaux.h"
#include "LTC68042.h"
#include "boot.h"
#include "ltc6804.h"
#include "rtcc.h"
#include "analog_input.h"
#include "digital_input.h"
#include "leds.h"
#include "rele.h"
#include "fuelsensor.h"
#include "sysprint.h"
#include "mySPI.h"



#define ETH_DEBUG

#ifdef ETH_DEBUG
    #define client_print(s) do{client.print(s);Serial.print(s);}while(0)
    #define client_println(s) do{client.println(s);Serial.println(s);}while(0)

    #define client_print_2arg(s,a1) do{client.print(s,a1);Serial.print(s,a1);}while(0)
    #define client_println_2arg(s,a1) do{client.println(s,a1);Serial.println(s,a1);}while(0)
#else
    #define client_print(s) do{client.print(s);}while(0)
    #define client_println(s) do{client.println(s);}while(0)

    #define client_print_2arg(s,a1) do{client.print(s,a1);}while(0)
    #define client_println_2arg(s,a1) do{client.println(s,a1);}while(0)
#endif// ETH_DEBUG



//uint8_t eth_mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };//from SDCARD
uint8_t eth_server[4];// = { 192,168,0,35 };
uint16_t eth_server_port;
//const char eth_filePath[] PROGMEM ="/drafts/jsondecode_nuevo.php";
//#define ETH_FILEPATH_LENGTH sizeof(eth_filePath)/sizeof(eth_filePath[0])
EthernetClient client;

void eth_datasend_stackPush(uint8_t idx);
void eth_datasend_stackPop(int8_t count=1);
uint8_t eth_datasend_stackGetLength(void);
void eth_datasend_stackReset(void);
void eth_datasend_setFreq(uint8_t freq, uint8_t pop=0);
#define ETH_DATASEND_SETFREQ_ALWAYS 0
#define ETH_DATASEND_SETFREQ_STOP 1
#define ETH_DATASEND_SETFREQ_ONCE_AND_POP 2

static uint16_t eth_get_totalDataLength(uint8_t total_registros);
static uint16_t eth_get_length_register(uint8_t idx);
static void eth_send_postdata(uint8_t total_registros);
static void eth_send_postdata_register(uint8_t idx);

uint8_t MAC_String2HexArray(const char *setting_value, uint8_t *MAC);
void IP_String2int(const char *serverIP, uint8_t *IP);

struct _eth_con eth_con;

uint8_t id_send=0;

//const char* ip_to_str(const uint8_t* ipAddr)
//{
//    static char buf[16];
//    sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
//    return buf;
//}

void ip_to_str(const uint8_t* ipAddr, char *str_dest)
{
    //static char buf[16];
    sprintf(str_dest, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    //return buf;
}


/*String DisplayAddress(IPAddress address)
{
    return String(address[0]) + "." +String(address[1]) + "." +String(address[2]) + "." +String(address[3]);
}*/

void eth_SPI_access(void)
{
    SPI.endTransaction();
    SPI_deselectAllperipherals();
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    //SPI.begin();
}

//return
//0: IP no configured
//1: IP is configured via DHCP
uint8_t eth_manage_DHCP_IP(void)
{

    uint8_t cod_ret=0;

    static DhcpState prevState = DhcpStateNone;//none is setting by ethernetDHCP??

    DhcpState state = EthernetDHCP.poll();

    if (prevState != state)
    {
        sysPrint_serialPrint("");

        switch (state)
        {
            case DhcpStateDiscovering:
                sysPrint_serialPrintln_P(PSTR("\t\t\t\t Discovering DHCP server"));
                break;
            case DhcpStateRequesting:
                sysPrint_serialPrintln_P(PSTR("\t\t\t\t Requesting lease"));
                break;
            case DhcpStateRenewing:
                sysPrint_serialPrintln_P(PSTR("\t\t\t\t Renewing lease"));
                break;
            case DhcpStateLeased:
                sysPrint_serialPrintln_P(PSTR("\t\t\t\t Obtained lease!"));
                //
                const byte* ipAddr = EthernetDHCP.ipAddress();
                const byte* gatewayAddr = EthernetDHCP.gatewayIpAddress();
                const byte* dnsAddr = EthernetDHCP.dnsIpAddress();

                char str_dest[16];
                sysPrint_serialPrint_P(PSTR("My IP address is "));
                ip_to_str(ipAddr, str_dest);
                sysPrint_serialPrintln(str_dest);

                sysPrint_serialPrint_P(PSTR("Gateway IP address is "));
                ip_to_str(gatewayAddr, str_dest);
                sysPrint_serialPrintln(str_dest);

                sysPrint_serialPrint_P(PSTR("DNS IP address is "));
                ip_to_str(dnsAddr, str_dest);
                sysPrint_serialPrintln(str_dest);
                //
                eth_con.bf.IP_configured = 1;

                cod_ret = 1;

                break;
        }

        prevState = state;
    }

    return cod_ret;
}


int8_t link_status_changed;// = -1;

uint8_t eth_get_link_status(void)
{
    static unsigned long  millis_last;
    static uint8_t sm0;
    //static int8_t link_status_triggered = -1;


    uint8_t link_status = !ReadPin(PORTRxLINKSTATUS, PINxLINKSTATUS);//0=link ok, 1=link bad

    //sysPrint_serialPrintln("linkstatus = " + String(link_status) );

    if (sm0 == 0)
    {
        if (link_status_changed != link_status )
        {
            link_status_changed = link_status;//save the change
            millis_last = millis();
            sm0++;
        }
    }
    else if (sm0 == 1)
    {
        if (millis() - millis_last >100)//20ms
        {
            if (link_status_changed == link_status)
            {
                //accept the change

                if (link_status == ETH_LINK_PIN_IS_UP)
                {
                    sysPrint_serialPrintln_P(PSTR("\t\t\t\t ETH_LINK_PIN_IS_UP") );

                    client.flush();
                    client.stop();
                    //eth_SPI_access();
                    uint8_t eth_mac[6];

                    char setting_value_dest[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
                    mainvar_getvalue("MAC", setting_value_dest);
                    MAC_String2HexArray(setting_value_dest, eth_mac);
                    EthernetDHCP.begin(eth_mac, 1);//poll
                }
                else//ETH_LINK_PIN_IS_DOWN
                {
                    sysPrint_serialPrintln_P(PSTR("\t\t\t\t\ETH_LINK_PIN_IS_DOWN") );

                    eth_con.bf.IP_configured = 0;
                }

                eth_con.bf.link = link_status;
            }

            link_status_changed = link_status;//igualar el cambio para la sgte. deteccion
            sm0 = 0x00;
        }
    }

    return eth_con.bf.link;
}

void eth_init(void)
{
    unsigned long last_millis;

    eth_SPI_access();

    client.setClientTimeout(500);//200ms its OK to J.P server

    W5100.setRetransmissionTime(500);//x 0.1ms = 50 ms
    W5100.setRetransmissionCount(2);

    //
    ConfigInputPin(DDRxLINKSTATUS, PINxLINKSTATUS);
    PinTo1(PORTWxLINKSTATUS, PINxLINKSTATUS);


    //
    eth_con.bf.link = !ReadPin(PORTRxLINKSTATUS, PINxLINKSTATUS);
    link_status_changed = eth_con.bf.link;
    //eth_get_link_status();
    //

    char setting_value_dest[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
    mainvar_getvalue("Server1IP", setting_value_dest);
    IP_String2int(setting_value_dest, eth_server);

    mainvar_getvalue("Server1Port", setting_value_dest);
    eth_server_port = (uint16_t)(strtod(setting_value_dest, (char**) 0));


    sysPrint_serialPrintln("");

    lcd.clear();
    sysPrint_lcd_serialPrintln_P(1,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("Set up network"));
    delay(SDCARD_BOOT_LCD_TIME2VIEW);

    if (eth_con.bf.link == ETH_LINK_PIN_IS_UP)
    {
        //
        uint8_t eth_mac[6];
        char str[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
        mainvar_getvalue("MAC", str);
        MAC_String2HexArray(str, eth_mac);

        EthernetDHCP.begin(eth_mac, 1);//poll

        //
        sysPrint_serialPrintln("");

        //lcd.clear();
        sysPrint_lcd_serialPrintln_P(0,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("IP ADDRESS"));
        sysPrint_lcd_serialPrintln_P(1,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("Please wait while"));
        sysPrint_lcd_serialPrintln_P(2,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("acquiring an IP"));
        sysPrint_lcd_serialPrintln_P(3,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("address..."));


#define ETH_INIT_DHCP_IP_WAIT_TIME_MAX 10000// 10s
        last_millis = millis();
        while ( ( millis() - last_millis ) <  ETH_INIT_DHCP_IP_WAIT_TIME_MAX)
        {
            eth_manage_DHCP_IP();
            if  (eth_con.bf.IP_configured == 1)
                break;
        }

        lcd.clear();

        if (eth_con.bf.IP_configured == 1)
        {
            const byte* ipAddr = EthernetDHCP.ipAddress();
            const byte* gatewayAddr = EthernetDHCP.gatewayIpAddress();
            const byte* dnsAddr = EthernetDHCP.dnsIpAddress();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("My IP address:"));

            char str_dest[16];
            ip_to_str(ipAddr,str_dest);
            sysPrintLCDan_XY_wlineClr_wEOL3dots(2,LCDAN_STR_FORMAT_ALIGN_CENTER, str_dest);
            delay(SDCARD_BOOT_LCD_TIME2VIEW);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("Gateway IP address:"));

            ip_to_str(gatewayAddr,str_dest);
            sysPrintLCDan_XY_wlineClr_wEOL3dots(2,LCDAN_STR_FORMAT_ALIGN_CENTER, str_dest);
            delay(SDCARD_BOOT_LCD_TIME2VIEW);
            lcd.clear();
            sysPrintLCDan_XY_wlineClr_wEOL3dots_P(1,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("DNS IP address:"));

            ip_to_str(dnsAddr,str_dest);
            sysPrintLCDan_XY_wlineClr_wEOL3dots(2,LCDAN_STR_FORMAT_ALIGN_CENTER, str_dest);
            delay(SDCARD_BOOT_LCD_TIME2VIEW);

        }
        else
        {
            sysPrint_serialPrintln("");
            sysPrint_lcd_serialPrintln_P(2,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("Fail IP config"));
            delay(SDCARD_BOOT_LCD_TIME2VIEW);
        }
    }
    else
    {
        sysPrint_serialPrintln("");
        sysPrint_lcd_serialPrintln_P(2,LCDAN_STR_FORMAT_ALIGN_CENTER,PSTR("Error: Link"));
        delay(SDCARD_BOOT_LCD_TIME2VIEW);
    }



    lcd.clear();

}


uint8_t MAC_String2HexArray(const char *setting_value, uint8_t *MAC)//< uint8_t MAC[6]
{
#define arr_leng 40
#define buff_length 10
#define _buff_clear() do{for (size_t i=0;i<buff_length;i++){buff[i]=0;} }while(0)

    uint8_t parseok=0;
    //char arr[arr_leng];
    char buff[buff_length];
    uint8_t ca,cb,cm;
    //uint8_t MAC[6];
    ca=cb=cm=0x00;

    //setting_value.toCharArray(arr,setting_value.length()+1);
    _buff_clear();

    while (setting_value[ca] != '\0')
    {
        if (setting_value[ca]==',')
        {
            MAC[cm++] =  (uint8_t) strtoul(buff, NULL, 16);
            ca++;//1++ for skip comma
            _buff_clear();
            cb=0;
        }
        buff[cb++]=setting_value[ca++];
    }
    MAC[5] = (uint8_t) strtoul(buff, NULL, 16);

    parseok = 1;

    return parseok;
    //    for (int i=0; i<6; i++) Serial.println(MAC[i]);
}

void IP_String2int(const char *serverIP, uint8_t *IP)//< uint8_t IP[4]
{
#define arr_leng 40
#define buff_length 10
#define _buff_clear() do{for (size_t i=0;i<buff_length;i++){buff[i]=0;} }while(0)

    //char arr[arr_leng];
    char buff[buff_length];
    uint8_t ca,cb,cm;
    ca=cb=cm=0x00;

    //serverIP.toCharArray(arr,serverIP.length()+1);
    _buff_clear();

    while (serverIP[ca] != '\0')
    {
        if (serverIP[ca]=='.')
        {
            IP[cm++] =  String(buff).toInt();
            ca++;//1++ for skip comma
            _buff_clear();
            cb=0;
        }
        buff[cb++]=serverIP[ca++];
    }
    IP[3] = String(buff).toInt();

    //    for (int i=0; i<4; i++)
    //    {
    //        Serial.println(IP[i]);
    //    }

}

//
#define VECTOR_DIMM_LENGTH 10
uint8_t vector_dimm[VECTOR_DIMM_LENGTH]=
{
    NUM_BOARD,
    NUM_IC,
    LTC_NUM_CELLS,
    NUM_CELL_2V_MAX,
    NUM_CELL_12V_MAX,
    NUM_TOTAL_REGAUX,
    NUM_ANALOG_INPUT_MAX,
    NUM_DIG_INPUT_MAX,
    NUM_LEDS_MAX,
    NUM_RELE_MAX,
};

#define LENGTH_TO_RESOLVE -1


const struct _v2eth_server
{
    const uint8_t idx;
    const char name[4];//m99 \0
    const void *pointer;
    const int16_t dim[2];//fil,col
    const uint8_t element_size;

    const char aux0[30];
}
v2eth_server[] PROGMEM=
{
    {idx_id_send, "id",(uint8_t *)&id_send,{0,0}, sizeof(id_send),""},
    {idx_date_str, "d1",(char *)date_str,{DATE_STR_LENGTH-1,0}, sizeof(date_str[0]),""},
    {idx_time_str, "d2",(char *)time_str,{TIME_STR_LENGTH-1,0}, sizeof(time_str[0]),""},
    {idx_ltc_connected, "d3",(uint8_t *)ltc_connected,{NUM_IC,0}, sizeof(ltc_connected[0]),""},
    {idx_select_2_12V, "d4",(uint8_t *)select_2_12V,{NUM_IC,0}, sizeof(select_2_12V[0]),""},
    {idx_stack_union, "d5",(uint8_t *)stack_union,{NUM_IC,0}, sizeof(stack_union[0]),""},

//    {idx_vcellF1, "d6",(float *)&vcellF[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(vcellF[0][0]),""},
    {idx_vcellF1, "d6",(float *)&vcellF[0][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[0][0]),""},
    {idx_vcellF2, "d7",(float *)&vcellF[2][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[2][0]),""},
    {idx_vcellF3, "d8",(float *)&vcellF[4][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[4][0]),""},
    {idx_vcellF4, "d9",(float *)&vcellF[6][0],{NUM_STACKS_IN_BOARD,LTC_NUM_CELLS}, sizeof(vcellF[6][0]),""},

    {idx_uv_flag, "d10",(uint8_t *)&uv_flag[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(uv_flag[0][0]),""},
    {idx_ov_flag, "d11",(uint8_t *)&ov_flag[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(ov_flag[0][0]),""},
    {idx_openwire_flag, "d12",(uint8_t *)&openwire_flag[0][0],{NUM_IC,LTC_NUM_CELLS}, sizeof(openwire_flag[0][0]),""},

    {idx_stack_voltage_ifcF, "d13",(float *)stack_voltage_ifcF,{NUM_IC,0}, sizeof(stack_voltage_ifcF[0]),""},
    {idx_stack_currentF, "d14",(float *)stack_currentF,{NUM_IC,0}, sizeof(stack_currentF[0]),""},
    {idx_stack_temperatureF, "d15",(float *)stack_temperatureF,{NUM_IC,0}, sizeof(stack_temperatureF[0]),""},
    //    {idx_internal_die_tempF, "d16",(float *)internal_die_tempF,{NUM_IC,0}, sizeof(internal_die_tempF[0]),""},
    //    {idx_internal_socF, "d17",(float *)internal_socF,{NUM_IC,0}, sizeof(internal_socF[0]),""},

    {idx_analog_input, "d16",(float *)analog_input,{NUM_ANALOG_INPUT_MAX,0}, sizeof(analog_input[0]),""},
    {idx_digital_input, "d17",(uint8_t *)digital_input,{NUM_DIG_INPUT_MAX,0}, sizeof(digital_input[0]),""},
    {idx_out_leds, "d18",(uint8_t *)out_leds,{NUM_LEDS_MAX,0}, sizeof(out_leds[0]),""},
    {idx_do_rele, "d19",(uint8_t *)do_rele,{NUM_RELE_MAX,0}, sizeof(do_rele[0]),""},
    {idx_fuelsensor_value, "d20",(float *)&fuelsensor.value,{0,0}, sizeof(float),""},
    //
    {idx_mainvar_ApiKeyW, "c1",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"ApiKeyW"  },
    {idx_mainvar_SerialNumber, "c2",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"SerialNumber"  },
    {idx_sdcardVar_PartNumber, "c3",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"PartNumber"  },
    {idx_sdcardVar_SoftwareVersion, "c4",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"SoftwareVersion"  },
    {idx_mainvar_FunctionMode, "c5",(void *)0x00,{LENGTH_TO_RESOLVE,0}, sizeof(char),"FunctionMode" },

    {idx_vector_dimm, "c6",(uint8_t *)vector_dimm,{VECTOR_DIMM_LENGTH,0}, sizeof(vector_dimm[0]),""},

    {idx_fuelsensor_model, "c7",(uint8_t *)&fuelsensor.fsEE.model,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_type, "c8",(uint8_t*)&fuelsensor.fsEE.outputType.type,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_length_units, "c9",(uint8_t *)&fuelsensor.fsEE.outputType.length.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_outputtype_volume_units, "c10",(uint8_t *)&fuelsensor.fsEE.outputType.volume.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_type, "c11",(uint8_t *)&fuelsensor.fsEE.tank.type,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_rectangular_area_length, "c12",(float *)&fuelsensor.fsEE.tank.rectangular.area.length,{0,0}, sizeof(float),""},
    {idx_fuelsensor_tank_rectangular_area_width, "c13",(float *)&fuelsensor.fsEE.tank.rectangular.area.width,{0,0}, sizeof(float),""},
    {idx_fuelsensor_tank_irregular_spline_node_units_length_units, "c14",(uint8_t *)&fuelsensor.fsEE.tank.irregular.spline.node_units.length.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_tank_irregular_spline_node_units_volume_units, "c15",(uint8_t *)&fuelsensor.fsEE.tank.irregular.spline.node_units.volume.units,{0,0}, sizeof(uint8_t),""},
    {idx_fuelsensor_calib_tank_innertank, "c16",(float *)&fuelsensor.fsEE.calib.tank.innertank,{0,0}, sizeof(float),""},
    {idx_fuelsensor_calib_tank_zero2full, "c17",(float *)&fuelsensor.fsEE.calib.tank.zero2full,{0,0}, sizeof(float),""},
    {idx_fuelsensor_calib_tank_full2upper, "c18",(float *)&fuelsensor.fsEE.calib.tank.full2upper,{0,0}, sizeof(float),""},


    {idx_sdcardVar_FACTCORR_2V, "c19",(float *)&corfact_vars[FCOR_2V].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C1, "c20",(float *)&corfact_vars[FCOR_12V_C1].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C2, "c21",(float *)&corfact_vars[FCOR_12V_C2].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C3, "c22",(float *)&corfact_vars[FCOR_12V_C3].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_FACTCORR_12V_C4, "c23",(float *)&corfact_vars[FCOR_12V_C4].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_shuntVoltFullScale, "c24",(float *)&setacq_vars[SHUNT_VOLT_FULLSCALE].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_shuntRatedCurrent, "c25",(float *)&setacq_vars[SHUNT_RATED_CURRENT].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_uv_2v, "c26",(float *)&setacq_vars[UV_2V].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_ov_2v, "c27",(float *)&setacq_vars[OV_2V].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_uv_12v_S3C, "c28",(float *)&setacq_vars[UV_12V_S3C].value,{0,0}, sizeof(float),""  },
    {idx_sdcardVar_ov_12v_S3C, "c29",(float *)&setacq_vars[OV_12V_S3C].value,{0,0}, sizeof(float),""  },


};

#define NUM_VECTOR2SERVER sizeof(v2eth_server)/sizeof(v2eth_server[0])

uint8_t eth_datasend[NUM_VECTOR2SERVER];

struct _eth
{
    struct _datasend
    {
        uint8_t frequency;
        uint8_t number_of_pops;

    } datasend;

    struct _stack
    {
        int8_t sp;//deep max 127
    } stack;
} eth;


void eth_datasend_stackPush(uint8_t idx)
{
    eth_datasend[eth.stack.sp] = idx;

    if (++eth.stack.sp > NUM_VECTOR2SERVER)
        eth.stack.sp = 0;
}

void eth_datasend_stackPop(int8_t count)//int8_t count=1
{
    for (int i = 0; i < count; ++i)
    {
        if (--eth.stack.sp < 0)
            eth.stack.sp = NUM_VECTOR2SERVER;
    }
}
uint8_t eth_datasend_stackGetLength(void)
{
    return (uint8_t)eth.stack.sp;
}
void eth_datasend_stackReset(void)//reset sp
{
    eth.stack.sp = 0;
}


void eth_datasend_setFreq(uint8_t freq, uint8_t pop)//reset sp
{
    eth.datasend.frequency = freq;
    eth.datasend.number_of_pops = pop;//valid only for ETH_DATASEND_SETFREQ_ONCE_AND_POP
}
uint8_t eth_datasend_getFreq(void)
{
    return eth.datasend.frequency;
}

void eth_datasend_parsingAfterSend(void)
{
    if (eth_datasend_getFreq() == ETH_DATASEND_SETFREQ_ONCE_AND_POP)
    {
        eth_datasend_stackPop(eth.datasend.number_of_pops);
    }

    if (eth_datasend_stackGetLength()==0)
    {
        eth_datasend_setFreq(ETH_DATASEND_SETFREQ_STOP);
    }
    else
    {
        eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ALWAYS); //x defect
    }

}
//////////////////////////////////////////////////////////////////////
struct _eth_app eth_app;

uint8_t eth_app_datasend_getData(uint8_t idx)
{
    return eth_app.send.v[idx];
}

void eth_app_datasend_stackPush(uint8_t idx)
{
    eth_app.send.v[eth_app.stack.sp] = idx;

    if (++eth_app.stack.sp > ETH_APP_SENDDATA_REQUEST_MAX_IDX)
        eth_app.stack.sp = 0;
}

void eth_app_datasend_stackPop(int8_t count)//int8_t count=1
{
    for (int i = 0; i < count; ++i)
    {
        if (--eth_app.stack.sp < 0)
            eth_app.stack.sp = ETH_APP_SENDDATA_REQUEST_MAX_IDX;
    }
}
uint8_t eth_app_datasend_stackGetLength(void)
{
    return (uint8_t)eth_app.stack.sp;
}
void eth_app_datasend_stackReset(void)//reset sp
{
    eth_app.stack.sp = 0;
}
//////////////////////////////////////////////////////////////////////
uint8_t eth_scheduler_uploadData_job(void)
{
    uint8_t cod_ret = 0x00;
    static uint8_t sm0;

    if (1)//(eth_con.bf.IP_configured == 1)
    {
        eth_datasend_stackReset();

        eth_datasend_stackPush(idx_id_send);

        switch (sm0)
        {
            case 0:
                eth_datasend_stackPush(idx_date_str);
                eth_datasend_stackPush(idx_time_str);
                eth_datasend_stackPush(idx_ltc_connected);
                eth_datasend_stackPush(idx_select_2_12V);
                eth_datasend_stackPush(idx_stack_union);

                sm0++;
                break;

            case 1:
                eth_datasend_stackPush(idx_vcellF1);
                sm0++;
                break;

            case 2:
                eth_datasend_stackPush(idx_vcellF2);
                sm0++;
                break;

            case 3:
                eth_datasend_stackPush(idx_vcellF3);
                sm0++;
                break;

            case 4:
                eth_datasend_stackPush(idx_vcellF4);
                sm0++;
                break;

            case 5:
                eth_datasend_stackPush(idx_uv_flag);
                sm0++;
                break;

            case 6:
                eth_datasend_stackPush(idx_ov_flag);
                sm0++;
                break;

            case 7:
                eth_datasend_stackPush(idx_openwire_flag);
                sm0++;
                break;

            case 8:
                eth_datasend_stackPush(idx_stack_voltage_ifcF);
                eth_datasend_stackPush(idx_stack_currentF);
                eth_datasend_stackPush(idx_stack_temperatureF);

                sm0++;
                break;

            case 9:
                eth_datasend_stackPush(idx_analog_input);
                eth_datasend_stackPush(idx_digital_input);
                eth_datasend_stackPush(idx_out_leds);
                eth_datasend_stackPush(idx_do_rele);
                eth_datasend_stackPush(idx_fuelsensor_value);

                //sm0=0;
                sm0++;
                break;

        }
        //
        eth_datasend_stackPush(idx_mainvar_ApiKeyW);
        eth_datasend_stackPush(idx_mainvar_SerialNumber);

        if (eth_app.send.bf.request)
        {
            eth_app.send.bf.request = 0;

            uint8_t app_datasend_stackLength =  eth_app_datasend_stackGetLength();

            for (uint8_t i=0; i<app_datasend_stackLength; i++ )
            {
                eth_datasend_stackPush( eth_app_datasend_getData(i) );
            }
            eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ONCE_AND_POP, app_datasend_stackLength  );

            eth_app_datasend_stackReset();
        }

        cod_ret = eth_uploadData_job();

        if (sm0 == 10)
        {
            sm0=0;
            id_send++;
        }
    }

    return cod_ret;
}
//////////////////////////////////////////////////////////////////////
uint8_t eth_uploadData_job1(void);

uint8_t eth_scheduler_uploadData_job1(void)
{
    uint8_t cod_ret = 0x00;
    static uint8_t sm0;
    static uint8_t sm1;
    if (1)//(eth_con.bf.IP_configured == 1)
    {
        if (sm1 == 0)
        {
            eth_datasend_stackReset();

            eth_datasend_stackPush(idx_id_send);

            switch (sm0)
            {
                case 0:
                    eth_datasend_stackPush(idx_date_str);
                    eth_datasend_stackPush(idx_time_str);
                    eth_datasend_stackPush(idx_ltc_connected);
                    eth_datasend_stackPush(idx_select_2_12V);
                    eth_datasend_stackPush(idx_stack_union);

                    sm0++;
                    break;

                case 1:
                    eth_datasend_stackPush(idx_vcellF1);
                    sm0++;
                    break;

                case 2:
                    eth_datasend_stackPush(idx_vcellF2);
                    sm0++;
                    break;

                case 3:
                    eth_datasend_stackPush(idx_vcellF3);
                    sm0++;
                    break;

                case 4:
                    eth_datasend_stackPush(idx_vcellF4);
                    sm0++;
                    break;

                case 5:
                    eth_datasend_stackPush(idx_uv_flag);
                    sm0++;
                    break;

                case 6:
                    eth_datasend_stackPush(idx_ov_flag);
                    sm0++;
                    break;

                case 7:
                    eth_datasend_stackPush(idx_openwire_flag);
                    sm0++;
                    break;

                case 8:
                    eth_datasend_stackPush(idx_stack_voltage_ifcF);
                    eth_datasend_stackPush(idx_stack_currentF);
                    eth_datasend_stackPush(idx_stack_temperatureF);

                    sm0++;
                    break;

                case 9:
                    eth_datasend_stackPush(idx_analog_input);
                    eth_datasend_stackPush(idx_digital_input);
                    eth_datasend_stackPush(idx_out_leds);
                    eth_datasend_stackPush(idx_do_rele);
                    eth_datasend_stackPush(idx_fuelsensor_value);

                    //sm0=0;
                    sm0++;
                    break;

            }
            //
            eth_datasend_stackPush(idx_mainvar_ApiKeyW);
            eth_datasend_stackPush(idx_mainvar_SerialNumber);

            if (eth_app.send.bf.request)
            {
                eth_app.send.bf.request = 0;

                uint8_t app_datasend_stackLength =  eth_app_datasend_stackGetLength();

                for (uint8_t i=0; i<app_datasend_stackLength; i++ )
                {
                    eth_datasend_stackPush( eth_app_datasend_getData(i) );
                }
                eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ONCE_AND_POP, app_datasend_stackLength  );

                eth_app_datasend_stackReset();
            }

            sm1++;
        }
        if (sm1 == 1)
        {
            cod_ret = eth_uploadData_job1();

            if (cod_ret>0)
            {
                if (cod_ret == 1)
                {
                    if (sm0 == 10)
                    {
                        sm0=0;
                        id_send++;//ciclico repetitivo 0-->255->0...
                    }
                    sm1 = 0;
                    //sm1++;
                }
                else if (cod_ret == 2)
                {
                }
            }
        }


    }

    return cod_ret;
}

uint8_t eth_scheduler_uploadData_job2(void)
{
    uint8_t cod_ret = 0x00;
    //static uint8_t sm0;
    static uint8_t sm1;

    if (sm1 == 0)
    {
        eth_datasend_stackReset();

        eth_datasend_stackPush(idx_id_send);
        eth_datasend_stackPush(idx_date_str);
        eth_datasend_stackPush(idx_time_str);
        eth_datasend_stackPush(idx_ltc_connected);
        eth_datasend_stackPush(idx_select_2_12V);
        eth_datasend_stackPush(idx_stack_union);
        eth_datasend_stackPush(idx_vcellF1);
        eth_datasend_stackPush(idx_vcellF2);
        eth_datasend_stackPush(idx_vcellF3);
        eth_datasend_stackPush(idx_vcellF4);
        eth_datasend_stackPush(idx_uv_flag);
        eth_datasend_stackPush(idx_ov_flag);
        eth_datasend_stackPush(idx_openwire_flag);
        eth_datasend_stackPush(idx_stack_voltage_ifcF);
        eth_datasend_stackPush(idx_stack_currentF);
        eth_datasend_stackPush(idx_stack_temperatureF);
        eth_datasend_stackPush(idx_analog_input);
        eth_datasend_stackPush(idx_digital_input);
        eth_datasend_stackPush(idx_out_leds);
        eth_datasend_stackPush(idx_do_rele);
        eth_datasend_stackPush(idx_fuelsensor_value);
        //
        eth_datasend_stackPush(idx_mainvar_ApiKeyW);
        eth_datasend_stackPush(idx_mainvar_SerialNumber);


        eth_datasend_stackPush(idx_sdcardVar_PartNumber);
        eth_datasend_stackPush(idx_sdcardVar_SoftwareVersion);
        eth_datasend_stackPush(idx_mainvar_FunctionMode);
        eth_datasend_stackPush(idx_vector_dimm);
        eth_datasend_stackPush(idx_fuelsensor_model);
        eth_datasend_stackPush(idx_fuelsensor_outputtype_type);
        eth_datasend_stackPush(idx_fuelsensor_outputtype_length_units);
        eth_datasend_stackPush(idx_fuelsensor_outputtype_volume_units);
        eth_datasend_stackPush(idx_fuelsensor_tank_type);
        eth_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_length);
        eth_datasend_stackPush(idx_fuelsensor_tank_rectangular_area_width);
        eth_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_length_units);
        eth_datasend_stackPush(idx_fuelsensor_tank_irregular_spline_node_units_volume_units);
        eth_datasend_stackPush(idx_fuelsensor_calib_tank_innertank);
        eth_datasend_stackPush(idx_fuelsensor_calib_tank_zero2full);
        eth_datasend_stackPush(idx_fuelsensor_calib_tank_full2upper);
        eth_datasend_stackPush(idx_sdcardVar_FACTCORR_2V);
        eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C1);
        eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C2);
        eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C3);
        eth_datasend_stackPush(idx_sdcardVar_FACTCORR_12V_C4);
        eth_datasend_stackPush(idx_sdcardVar_shuntVoltFullScale);
        eth_datasend_stackPush(idx_sdcardVar_shuntRatedCurrent);
        eth_datasend_stackPush(idx_sdcardVar_uv_2v);
        eth_datasend_stackPush(idx_sdcardVar_ov_2v);
        eth_datasend_stackPush(idx_sdcardVar_uv_12v_S3C);
        eth_datasend_stackPush(idx_sdcardVar_ov_12v_S3C);



        if (eth_app.send.bf.request)
        {
            eth_app.send.bf.request = 0;

            uint8_t app_datasend_stackLength =  eth_app_datasend_stackGetLength();

            for (uint8_t i=0; i<app_datasend_stackLength; i++ )
            {
                eth_datasend_stackPush( eth_app_datasend_getData(i) );
            }
            eth_datasend_setFreq(ETH_DATASEND_SETFREQ_ONCE_AND_POP, app_datasend_stackLength  );

            eth_app_datasend_stackReset();
        }

        sm1++;
    }
    if (sm1 == 1)
    {
        cod_ret = eth_uploadData_job1();

        if (cod_ret>0)
        {
            if (cod_ret == 1)
            {
                id_send++;//ciclico repetitivo 0-->255->0...
            }
            sm1 = 0;
        }
        else if (cod_ret == 2)
        {
        }

    }

    return cod_ret;
}
void ShowSocketStatus();


static int8_t eth_send_postdata1(uint8_t total_registros);
/////////////////////////////////////////////
//Return
//0: job is going
//1: job is finished
//2:
uint8_t eth_uploadData_job1(void)
{
    uint8_t cod_ret = 0;
    char setting_value_dest[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
    //static
    int8_t eth_data2send_length =  eth_datasend_stackGetLength();;

    static uint8_t sm0;
    if (sm0 == 0)
    {
        if (eth_data2send_length > 0)
        {
            client.flush();
            client.stop();

//            eth_server[0]= 192;
//            eth_server[1]= 168;
//            eth_server[2]= 1;
//            eth_server[3]= 38;

            //if (client.connect(eth_server, 8085))
            if (client.connect(eth_server, eth_server_port))//added 2020
            {
                sysPrint_serialPrintln_P(PSTR("xConexion to server!"));

                client_print(F("POST "));
                //client_print("/api/battery");
                mainvar_getvalue("Directory", setting_value_dest);   // Llena el buffer con el valor de la variable indicada
                client_print(setting_value_dest);
                client_println(F(" HTTP/1.1"));

                client_print(F("Host: "));
                mainvar_getvalue("Host", setting_value_dest);
                client_println(setting_value_dest);

                client_println(F("api-key-write:asdfgfdssdf"));

                client_println(F("Content-Type: application/json"));//client_println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
                client_println(F("Connection: close"));
                client_println(F("User-Agent: BatterySystem/1.00"));
                //
                client_print(F("Content-Length: "));
                uint16_t jsonLength = eth_get_totalDataLength(eth_data2send_length);
                client_println(jsonLength);
                //
                client_println();
                sm0++;
            }
            else
            {
                sysPrint_serialPrintln_P(PSTR("Conexion failed!"));
                //sm0 = 2;
                cod_ret = 2;
                sm0 = 0x00;
            }
        }
    }

    if (sm0 == 1)
    {
        if (eth_send_postdata1(eth_data2send_length))
        {
            eth_datasend_parsingAfterSend();//add
            cod_ret = 1;

            sm0 = 0x00;
        }
    }

    //ShowSocketStatus();
    return cod_ret;
}

void eth_job(void)
{
    static uint8_t sm0;
    static long millis_last;

    eth_SPI_access();

    eth_get_link_status();

    if (eth_con.bf.link)
    {
        eth_manage_DHCP_IP();

        if (eth_con.bf.IP_configured)
        {
            if (sm0 == 0)
            {
                //if (eth_scheduler_uploadData_job1() == 1)
                if (eth_scheduler_uploadData_job2() == 1)
                {
                    sm0++;
                }

            } else if (sm0 == 1)
            {
                if (eth_rxDataFromServer() )
                {
                    //sm0 = 0;
                    sm0++;
                    millis_last = millis();
                }


            }
            //sysPrint_serialPrintln(String(sm0) );

            if (sm0 == 2)
            {
                //if (millis() - millis_last >=0)// 25000)
                if (millis() - millis_last > 25000)
                {
                    sm0 = 0;
                }

            }
        }

    }
    else
    {
        sm0 = 0x00;
    }

    //    sysPrint_serialPrintln(String(sm0) );

}

#define ETH_RXDATA_FROM_SERVER_TIMEOUT_READING 40000 //10s
#define ETH_RXDATA_FROM_SERVER_TIMECPU_READING 10 //50ms

uint8_t eth_rxDataFromServer(void)
{
    static uint8_t sm0;
    uint8_t cod_ret=0;

    static unsigned long timeout_last_millis;
    //static
    unsigned long timecpu_last_millis;

    if (1)//(client.connected())
    {
        if (sm0 == 0)
        {
            timeout_last_millis = millis();
            sm0++;
        }
        if (sm0 == 1)
        {
            timecpu_last_millis = millis();
            while ( (millis() - timecpu_last_millis) < ETH_RXDATA_FROM_SERVER_TIMECPU_READING)////read all data as posible
            {
                if (client.available())
                {
                    char rx= client.read();
                    sysPrint_serialWrite_char(rx );

                    if (rx == '?')
                    {
                        sm0=2;
                        break;
                    }
                }

                if (client.connected() == false)
                {
                    sm0 =2;
                    break;
                }
            }

        }

        if ( (millis() - timeout_last_millis) > ETH_RXDATA_FROM_SERVER_TIMEOUT_READING)
        {
            sm0 = 2;
        }


        if (sm0 == 2)
        {
            client.flush();
            client.stop();

            sysPrint_serialPrintln_P(PSTR("client.stop"));
            sm0 = 0x00;
            cod_ret = 1;
        }
    }

    return cod_ret;
}

static uint16_t eth_get_totalDataLength(uint8_t total_registros)
{
    uint16_t acc=0;
    if (total_registros > 0)
    {
        for (uint8_t v=0; v < total_registros; v++)
        {
            acc+=eth_get_length_register(eth_datasend[v]);

            if (v < (total_registros-1))
                acc+=1;// comma between vectors
        }
        acc+=2; // open{ }close
    }
    return acc;
}
//#define V2SERVER_GETDATALENGTH_DEBUG
static uint16_t eth_get_length_register(uint8_t idx)
{
    //uint8_t idx;
    int16_t dimx, dimy;
    uint8_t element_size;
    char name[4];
    void *pointer;
    char buf[15];
    char aux0[30];
    uint16_t acc=0;
    uint16_t acc1;
    //
    uint8_t fil;
    uint8_t col;

    //1.-
    //idx       = pgm_read_byte_near((const uint8_t *) &v2eth_server[idx].idx);
    strcpy_P(name, (const char *) &v2eth_server[idx].name);
    dimx    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[0]);
    dimy    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[1]);
    element_size = pgm_read_byte_near((const uint8_t *) &v2eth_server[idx].element_size);

    if (dimx == LENGTH_TO_RESOLVE )
    {
        strcpy_P(aux0, (char *) &v2eth_server[idx].aux0);

        //--------
        char setting_value[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];

        if (strcmp(aux0,"PartNumber")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("PartNumber"),aux0) )
            {
                //Serial.println(String(aux0));
                dimx= strlen(aux0);
            }
        }
        else if (strcmp(aux0,"SoftwareVersion")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SoftwareVersion"),aux0) )
            {
                dimx= strlen(aux0);
                //Serial.println(String(aux0));
            }
        }
        //--------
        else
        {
            char t[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
            mainvar_getvalue(aux0, t);

            //dimx    = t.length();
            dimx    = strlen(t);

            //t.toCharArray(aux0, dimx+1);
            strcpy(aux0, t);



            //pointer = (void *)aux0;
            //dimx    = strlen(aux0);
        }

    }
    else
    {
        pointer = (void *) pgm_read_word(&(v2eth_server[idx].pointer));
    }


#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println(idx);
    Serial.println(dimx);
    Serial.println(dimy);
    Serial.println(element_size);
#endif

    //2.-
    //# of bytes containing inside vectors
    if ((dimx>0) || (dimy>0))
    {
        if ((idx==idx_ltc_connected)||(idx==idx_stack_union)||
                (idx==idx_digital_input)||(idx==idx_out_leds)||(idx==idx_do_rele))
        {
            acc += dimx;
        }
        else if ( (idx==idx_uv_flag)||(idx==idx_ov_flag)||(idx==idx_openwire_flag) )
        {
            acc += (dimx*dimy);
        }
        else if (idx==idx_time_str)
        {
            acc += dimx;//time_str;
        }
        else if (idx==idx_date_str)
        {
            acc += dimx;//date_str
        }
        else if (
            (idx==idx_mainvar_ApiKeyW) || (idx==idx_mainvar_SerialNumber) || (idx==idx_mainvar_FunctionMode)
            ||(idx==idx_sdcardVar_PartNumber)
            ||(idx==idx_sdcardVar_SoftwareVersion)

        )
        {
            acc += strlen(aux0);
        }
        else
        {
            //-------
            if ((dimx>0) && (dimy==0))//1 array
            {
                fil = 1;
                col = dimx;
            }
            else
            {
                fil = dimx;
                col = dimy;
            }
            //-------

            for (uint8_t f=0; f<fil; f++)
            {
                for (uint8_t c=0; c<col; c++)
                {
                    if ((idx==idx_stack_voltage_ifcF)||(idx==idx_stack_currentF)||(idx==idx_stack_temperatureF)
                            //||(idx==idx_vcellF)
                            //||(idx==idx_vgpioF)
                            //||(idx==idx_internal_die_tempF)
                            //||(idx==idx_internal_socF)
                            || (idx==idx_analog_input)

                            //++
                            || (idx == idx_vcellF1)
                            || (idx == idx_vcellF2)
                            || (idx == idx_vcellF3)
                            || (idx == idx_vcellF4)
                       )
                    {
                        dtostrf(*(float *)pointer,4,3,buf);//acc += sprintf(buf, "%.3f", *(float *)pointer);
                        acc+= strlen(buf);

#ifdef V2SERVER_GETDATALENGTH_DEBUG
                        Serial.print(*(float *)pointer,3);
                        Serial.print(" ");
#endif
                    }
                    else if ((idx==idx_select_2_12V) ||(idx==idx_vector_dimm))
                    {
                        itoa(*(uint8_t *)pointer, buf, 10);//acc += sprintf(buf, "%u", *(uint8_t *)pointer);
                        acc+= strlen(buf);
                    }

                    pointer = (uint8_t *)pointer + element_size;        //(uint8_t *) for arithmetic between "bytes"
                }
#ifdef V2SERVER_GETDATALENGTH_DEBUG
                Serial.println();
#endif
            }
        }
    }
    else
    {
        //variables
        if (
            (idx==idx_fuelsensor_value)
            ||(idx==idx_fuelsensor_tank_rectangular_area_length)||(idx==idx_fuelsensor_tank_rectangular_area_width)
            ||(idx==idx_fuelsensor_calib_tank_innertank)||(idx==idx_fuelsensor_calib_tank_zero2full)||(idx==idx_fuelsensor_calib_tank_full2upper)



            ||(idx==idx_sdcardVar_FACTCORR_12V_C1)
            ||(idx==idx_sdcardVar_FACTCORR_12V_C2)
            ||(idx==idx_sdcardVar_FACTCORR_12V_C3)
            ||(idx==idx_sdcardVar_FACTCORR_12V_C4)

            ||(idx==idx_sdcardVar_shuntVoltFullScale)
            ||(idx==idx_sdcardVar_shuntRatedCurrent)

            ||(idx==idx_sdcardVar_uv_2v)
            ||(idx==idx_sdcardVar_ov_2v)
            ||(idx==idx_sdcardVar_uv_12v_S3C)
            ||(idx==idx_sdcardVar_ov_12v_S3C)

        )
        {
            dtostrf(*(float *)pointer,4,3,buf);//3 decimals
            acc+= strlen(buf);
        }
        else if (idx==idx_sdcardVar_FACTCORR_2V)
        {
            dtostrf(*(float *)pointer,4,4,buf);//4 decimals
            acc+= strlen(buf);
        }
        else if (
            (idx==idx_fuelsensor_model)
            ||(idx==idx_fuelsensor_outputtype_type)||(idx==idx_fuelsensor_outputtype_length_units)||(idx==idx_fuelsensor_outputtype_volume_units)
            ||(idx==idx_fuelsensor_tank_type)
            ||(idx==idx_fuelsensor_tank_irregular_spline_node_units_length_units)||(idx==idx_fuelsensor_tank_irregular_spline_node_units_volume_units)
        )
        {
            acc+= 1;
        }
        else if (idx == idx_id_send)
        {
            itoa(*(uint8_t *)pointer, buf, 10);
            acc+= strlen(buf);

        }

    }
    //printf("num elementos %i \n",acc);

    ////////////////////////////////////
    /*
    [
    ["","","",""],
    ["","","",""],
    ["","","",""]
    ]

    [""]
    */
    //fil=col=1;

    acc1=0;

    if ((dimx>0) && (dimy==0))//1 array
    {
        fil = 1;
        col = dimx;
    }
    else if ((dimx==0) && (dimy==0))
    {
        fil= 1;
        col= 1;
    }
    else
    {
        fil = dimx;
        col = dimy;
    }
    //cuento numero de comas y demas dentro del valor del registro
    acc1+= (col-1);    //# comas
    acc1+= (col*2);    //# ""
    acc1+= 2;        //# brackets internos
    acc1*=fil;
    acc1+= (fil-1);  //# de comas entre registros
    if (dimy>0)
        acc1+= 2;    //brackets externos
    ////////////////////////////////////
    //cuento el nombre del registro
    acc1+=3; //"mxX":
    acc1+= strlen(name);//mxx


    ////////////////////////////////////
    acc+= acc1;//add to acc

#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println("acc is");//printf("acc is %i \n", acc);
    Serial.println( acc);
#endif


#ifdef V2SERVER_GETDATALENGTH_DEBUG
    Serial.println();
    Serial.println( acc);
#endif
    return acc;
}


static int8_t eth_send_postdata1(uint8_t total_registros)
{
    static uint8_t sm0;
    static uint8_t v;

    int8_t cod_ret=0;

    if (total_registros > 0)
    {
        if (sm0 == 0)
        {
            client_print(F("{"));
            sm0++;
        }
        if (sm0 == 1)
        {
            eth_send_postdata_register(eth_datasend[v]);

            if (v < (total_registros - 1))
                client_print(F(","));

            if (++v == total_registros)
            {
                client_println(F("}"));
                v = 0x00;
                sm0 = 0x00;

                cod_ret = 1;
            }
        }
    }
    return cod_ret;
}



static void eth_send_postdata_register(uint8_t idx)
{
    //uint8_t idx
    int16_t dimx, dimy;
    uint8_t element_size;
    char name[4];
    void *pointer;
    uint8_t fil;
    uint8_t col;
    char aux0[30];

    //1.- Set values
    //idx = pgm_read_byte_near((const uint8_t *) &v2eth_server[v].idx);
    strcpy_P(name, (const char *) &v2eth_server[idx].name);
    dimx    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[0]);
    dimy    = pgm_read_word_near((const int16_t *) &v2eth_server[idx].dim[1]);
    element_size = pgm_read_byte_near((const uint8_t *) &v2eth_server[idx].element_size);

    if (dimx == LENGTH_TO_RESOLVE )
    {
        strcpy_P(aux0, (const char *) &v2eth_server[idx].aux0);

        //--------
        //char setting_value[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
        if (strcmp(aux0,"PartNumber")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("PartNumber"),aux0) )
            {
            }
        }
        else if (strcmp(aux0,"SoftwareVersion")== 0)
        {
            if ( SD_get_setting_value_PSTR(PSTR("config/specs.txt"),PSTR("SoftwareVersion"),aux0) )
            {
            }
        }
        //--------
        else
        {
            char str[SDCARD_STR_VAR_PROPERTIES_MAX_SIZE];
            //mainvar_getvalue(aux0).toCharArray(aux0, 30);
            mainvar_getvalue(aux0, str);
            strcpy(aux0,str);

        }

        pointer = (void *)aux0;
        dimx    = strlen(aux0);
    }
    else
    {
        pointer = (void *) pgm_read_word(&(v2eth_server[idx].pointer));
    }

    //2.- Check/Interchange dimensions
    if ((dimx > 0) && (dimy == 0))//1 array
    {
        fil = 1;
        col = dimx;
    }
    else if ((dimx == 0) && (dimy == 0))
    {
        fil = 1;
        col = 1;
    }
    else
    {
        fil = dimx;
        col = dimy;
    }

    //3.- Print data
    //-----------------------------------
    //client_print(F("\""));
    client_print(F("\""));

    client_print(name);
    client_print(F("\":"));
    //-----------------------------------
    if ((dimy > 0) && (dimx > 0))//array bidimensional
        client_print(F("["));
    //-----------------------------------
    for (uint8_t f = 0; f < fil; f++)
    {
        client_print(F("["));
        for (uint8_t c = 0; c < col; c++)
        {
            client_print(F("\""));

            //++-print data
            if ((idx == idx_vector_dimm) || (idx == idx_ltc_connected) || (idx == idx_uv_flag) || (idx == idx_ov_flag) || (idx == idx_openwire_flag) || (idx == idx_stack_union) || (idx == idx_select_2_12V)
                    || (idx == idx_digital_input) || (idx == idx_out_leds) || (idx == idx_do_rele)
                    || (idx == idx_fuelsensor_model)
                    || (idx == idx_fuelsensor_outputtype_type) || (idx == idx_fuelsensor_outputtype_length_units) || (idx == idx_fuelsensor_outputtype_volume_units)
                    || (idx == idx_fuelsensor_tank_type)
                    || (idx == idx_fuelsensor_tank_irregular_spline_node_units_length_units) || (idx == idx_fuelsensor_tank_irregular_spline_node_units_volume_units)
                    || (idx == idx_id_send)
               )
            {
                client_print(*(uint8_t *) pointer);
            }
            else if ((idx == idx_stack_voltage_ifcF) || (idx == idx_stack_currentF) || (idx == idx_stack_temperatureF)
                     //|| (idx == idx_vcellF)
                     //|| (idx == idx_vgpioF)
                     //|| (idx == idx_internal_die_tempF)
                     //|| (idx == idx_internal_socF)
                     || (idx == idx_analog_input)
                     || (idx == idx_fuelsensor_value)
                     || (idx == idx_fuelsensor_tank_rectangular_area_length) || (idx == idx_fuelsensor_tank_rectangular_area_width)
                     || (idx == idx_fuelsensor_calib_tank_innertank) || (idx == idx_fuelsensor_calib_tank_zero2full) || (idx == idx_fuelsensor_calib_tank_full2upper)

                     ||(idx==idx_sdcardVar_FACTCORR_12V_C1)
                     ||(idx==idx_sdcardVar_FACTCORR_12V_C2)
                     ||(idx==idx_sdcardVar_FACTCORR_12V_C3)
                     ||(idx==idx_sdcardVar_FACTCORR_12V_C4)

                     ||(idx==idx_sdcardVar_shuntVoltFullScale)
                     ||(idx==idx_sdcardVar_shuntRatedCurrent)

                     ||(idx==idx_sdcardVar_uv_2v)
                     ||(idx==idx_sdcardVar_ov_2v)
                     ||(idx==idx_sdcardVar_uv_12v_S3C)
                     ||(idx==idx_sdcardVar_ov_12v_S3C)

                     //++
                     || (idx == idx_vcellF1)
                     || (idx == idx_vcellF2)
                     || (idx == idx_vcellF3)
                     || (idx == idx_vcellF4)

                    )
            {
                client_print_2arg(*(float *) pointer, 3);//3 decimals
            }
            else if (idx==idx_sdcardVar_FACTCORR_2V)
            {
                client_print_2arg(*(float *) pointer, 4);//4 decimals
            }
            else if (
                (idx == idx_time_str) || (idx == idx_date_str)
                || (idx == idx_mainvar_FunctionMode)
                || (idx == idx_mainvar_ApiKeyW)
                || (idx == idx_mainvar_SerialNumber)

                ||(idx==idx_sdcardVar_PartNumber)
                ||(idx==idx_sdcardVar_SoftwareVersion)
            )
            {
                client_print(*(char *) pointer);
            }
            //--+

            client_print(F("\""));

            if (col > 1)// # elements>1
            {
                if (c < (col - 1))
                    client_print(F(",")); //more elements
            }
            pointer = (uint8_t *) pointer + element_size;
        }
        client_print(F("]"));

        if ((dimy > 0) && (dimx > 0))//array bidimensional
        {
            if (f < (fil - 1))
                client_print(F(","));
        }
    }

    //-----------------------------------
    if (dimy > 0)//array bidimensional
        client_print(F("]"));
    //-----------------------------------
}
//////////////////////////////////////////////++++++++++++++++++++++++
//-- Code: ---unsigned long G_SocketCheckTimer = 0;
//extern unsigned long G_SocketConnectionTimers[MAX_SOCK_NUM]; //system millis
//unsigned long G_SocketConnectionTimes[MAX_SOCK_NUM];  //time hh:mm:ss
void ShowSocketStatus()
{

    //    ActivityWriteSPICSC("ETHERNET SOCKET LIST");
    //    ActivityWriteSPICSC("#:Status Port Destination DPort");
    //    ActivityWriteSPICSC("0=avail,14=waiting,17=connected,22=UDP");
    //    ActivityWriteSPICSC("1C=close wait");
    String l_line = "";
    l_line.reserve(64);
    char l_buffer[10] = "";

    for (uint8_t i = 0; i < MAX_SOCK_NUM; i++)
    {
        l_line = "#" + String(i);

        uint8_t s = W5100.readSnSR(i); //status
        l_line += ":0x";
        sprintf(l_buffer,"%x",s);
        l_line += l_buffer;
        //
        // if(s == 0x1C)
        //{close(i);}
        //
        l_line += " ";
        l_line += String(W5100.readSnPORT(i)); //port

        l_line += " D:";
        uint8_t dip[4];
        W5100.readSnDIPR(i, dip); //IP Address
        for (int j=0; j<4; j++)
        {
            l_line += int(dip[j]);
            if (j<3) l_line += ".";
        }
        l_line += " (";
        l_line += String(W5100.readSnDPORT(i)); //port on destination
        l_line += ") ";

        //        if (G_SocketConnectionTimes[i] != 0)
        //            l_line += TimeToHHMM(G_SocketConnectionTimes[i]);

        Serial.println(l_line);

        //ActivityWriteSPICSC(l_line);
    }
}
