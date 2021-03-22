#include <stdint.h>

#include "Linduino.h"

#include "LTC68042.h"
#include <SPI.h>

#define __PRODUCTION 0
#define __DEBUG 1

//#define FUNCT_MODE __PRODUCTION
#define FUNCT_MODE __DEBUG

//
#define FACTDIV 10000.0 //10E4
#define NUMDECIMALS 3//4
//
#define NUM_IC 2//8
#define NUM_CELL 12

float FACT_CORR = +0.004; // for FLUKE 175

float cell_voltage_f_matrix[NUM_IC][NUM_CELL]; //12 cell + SUMA TOTAL
float gpio_f_matrix[NUM_IC][6];
uint8_t uv_flag_matrix[NUM_IC][NUM_CELL];
uint8_t ov_flag_matrix[NUM_IC][NUM_CELL];
uint8_t openwire_flag_matrix[NUM_IC][NUM_CELL];
uint8_t ltc_connected_matrix[NUM_IC];
uint8_t stack_union[NUM_IC];
uint8_t select_2_12V[NUM_IC];

//internal register 
float digital_psv_matrix[NUM_IC];//digital power supply, nominal:[2.7V-3.6V]
float analog_psv_matrix[NUM_IC];//analog power supply, nominal: [4.5V-5.5V]
float internal_die_temperature_IC[NUM_IC];   
float sum_all_cells[NUM_IC];


//VUV = VOV = 12 BITS = 0x0FFF = 4095
//ADC Range aprox. -0.8192V -> 5.7344VV = 65536 = 2^16
float uv = 0.5 + FACT_CORR;//>= 0.0016V 
float ov = 2 + FACT_CORR;//<= 5.73V

uint8_t open_wire_num_pass=3;//>=2 si FILTERED MODE 

int dcp=0;

void setup() 
{
  #if FUNCT_MODE == __DEBUG
  Serial.begin(115200);//x debugging
  #endif

  digitalWrite(10,HIGH);
  digitalWrite(4,HIGH);
  //
  pinMode(10,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(53,OUTPUT);//SPI.begin se encarga de poner SS x hardware como salida

  digitalWrite(LTC6804_CS,HIGH);
  pinMode(LTC6804_CS,OUTPUT);

  LTC6804_initialize();

  //#define MD_FAST 1
  //#define MD_NORMAL 2
  //#define MD_FILTERED 3

  //set_adc(MD_FILTERED,DCP_ENABLED,CELL_CH_ALL, AUX_CH_ALL);
  set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL, AUX_CH_ALL);
  set_stat(MD_FILTERED ,STAT_CH_ALL);
}


void loop() 
{
  uint16_t cell_voltage_matrix[NUM_IC][NUM_CELL]; //12 cell + SUMA TOTAL
  uint16_t gpio_matrix[NUM_IC][6];

  //
  write_cfg(0x1F,0,0,uv,ov,0x0FFF,0);
  LTC6804_adax();//CONVERSION ADC DE LOS GPIOS
  delay(202);//0.201317s
  LTC6804_rdaux(AUX_CH_ALL, NUM_IC, gpio_matrix);//LECTURA DE LA CONVERSION PREVIA HACIA LA MATRIZ gpio_matrix
  gpio2float(gpio_matrix);
  check_gpio();
  //


  //TABLA 36
  //Param: PULLDOWN GPIO(NO UTILIZADO), REFADC(NO UTILIZADO), OPTADC(NO UTILIZADO), uv, ov, DCC[12..1]=0b00001111 11111111 HABILITA SWITCH BALANCEO, TIEMPO DE DESCARGA DCTO(NO UTILIZADO) 
  write_cfg(0x1F,0,0,uv,ov,0x0FFF,0);//always send for RESET config by Watchdog(LTC hardware)
  LTC6804_adcv();//CONVERSION ADC DE LAS CELDAS
  delay(202);//0.201317s
  LTC6804_rdcv(CELL_CH_ALL,NUM_IC,cell_voltage_matrix);//LECTURA DE LA CONVERSION PREVIA HACIA LA MATRIZ cell_voltage
  cell2float(cell_voltage_matrix);
  //Inicio secuencia para adstat()
  write_cfg(0x1F,0,0,uv,ov,0x0FFF,0);
  delay(135);
  //set_stat(MD_FILTERED ,STAT_CH_ALL); //previamente fue establecido en el setup()
  LTC6804_adstat();
  delay(135);//134ms ALL
  read_stata();//llenado de vectores sum_all_cells[],analog_psv_matrix[], internal_die_temperature_IC[]
  read_statb();//llenado de las matrices uv_flag_matrix y ov_flag_matrix
  //Fin secuencia para adstat()
  
  open_wire(open_wire_num_pass);//Verifica por cualquier celda abierta y llena el vector openwire_flag_matrix

  #if FUNCT_MODE == __DEBUG
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
      Serial.println(" >_ BATTERY STATUS ______________________________");
      
      if (ltc_connected_matrix[num_ic] == 1)
      {
        Serial.print("IC address:"); Serial.print(num_ic);Serial.println(" CONNECTED OK"); 
      }
      
      Serial.println("");
   
      for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
      {
        Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - CELDA"); Serial.print(num_cell+1); Serial.print(": ");  Serial.print( cell_voltage_f_matrix[num_ic][num_cell], NUMDECIMALS); Serial.println("V");
      }
      /*
      Serial.println("");

      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 1 SUMA: "); Serial.println( ((cell_voltage_matrix[num_ic][0]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][1]/FACTDIV)-0.00) + ((cell_voltage_matrix[num_ic][2]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 2 SUMA: "); Serial.println( ((cell_voltage_matrix[num_ic][3]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][4]/FACTDIV)-0.00) + ((cell_voltage_matrix[num_ic][5]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 3 SUMA: "); Serial.println( ((cell_voltage_matrix[num_ic][6]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][7]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][8]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 4 SUMA: "); Serial.println( ((cell_voltage_matrix[num_ic][9]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][10]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][11]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      
      Serial.println("");
      
      Serial.print("IC address:"); Serial.print(0);Serial.print(" SUMA STACK: "); Serial.println( ((cell_voltage_matrix[num_ic][0]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][1]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][2]/FACTDIV)-0.0025) +
      ((cell_voltage_matrix[num_ic][3]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][4]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][5]/FACTDIV)-0.0025)+
      ((cell_voltage_matrix[num_ic][6]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][7]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][8]/FACTDIV)-0.0025)+
      ((cell_voltage_matrix[num_ic][9]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][10]/FACTDIV)-0.0025) + ((cell_voltage_matrix[num_ic][11]/FACTDIV)-0.0025), NUMDECIMALS);
      */
      Serial.println("");
      
      //GPIO1-5
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(1); Serial.print(" - CURRENT SENSE: ");  Serial.print(gpio_f_matrix[num_ic][0], NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(2); Serial.print(" - SELECT 12V/2V: ");  Serial.print(gpio_f_matrix[num_ic][1], NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(3); Serial.print(" - STACK UNION 24 CELDAS: ");  Serial.print(gpio_f_matrix[num_ic][2], NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(4); Serial.print(" - NTC10K / VREF2 BUFF OPAMP: ");  Serial.print(gpio_f_matrix[num_ic][3], NUMDECIMALS); Serial.print(" -->TEMPERATURA: ");  Serial.print(ntc10k(gpio_matrix[num_ic][3], gpio_matrix[num_ic][5]));Serial.println(" C");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(5); Serial.print(" - VOLT.REF LTC6655CHMS-3.3V 0.05% 5ppm/C: ");  Serial.print(gpio_f_matrix[num_ic][4], NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" VREF2(2.985V-3.015V) ");  Serial.print(": ");  Serial.print(gpio_f_matrix[num_ic][5], NUMDECIMALS);Serial.println("V");

      //Display UV
      for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
      {
        Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - C");Serial.print(num_cell+1);Serial.print("UV: "); Serial.println(uv_flag_matrix[num_ic][num_cell] );
      }
      
      Serial.println("");

      //Display OV
      for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
      {
        Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - C");Serial.print(num_cell+1);Serial.print("OV: "); Serial.println(ov_flag_matrix[num_ic][num_cell] );
      }
      
      Serial.println("");
      
      for (int num_cell=0; num_cell<(NUM_CELL-1); num_cell++)//open wire for C1-C11
      {
        if (openwire_flag_matrix[num_ic][num_cell] == 1)
        {
          Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - SIN CONEXION: "); Serial.print("Celda "); Serial.println(num_cell+1);
        }
      }
      
      Serial.println("");
     
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - SUM ALL CELL SOC: "); Serial.print(  sum_all_cells[num_ic], NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - INTERNAL DIE TEMP.: "); Serial.print(internal_die_temperature_IC[num_ic], NUMDECIMALS); Serial.println("C");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - ANALOG POWER VA [4.5V-5.5V]: "); Serial.print(analog_psv_matrix[num_ic], NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - DIGITAL POWER VA [2.7V-3.6V]: "); Serial.print(digital_psv_matrix[num_ic], NUMDECIMALS); Serial.println("V");
   
      Serial.println("");

      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" -STACK UNION : ");Serial.println( stack_union[num_ic]);
      
      Serial.println("");
      
      Serial.print("IC address:"); Serial.print(num_ic);Serial.print(" - 2/12V : ");Serial.println(select_2_12V[num_ic]);
      
      Serial.println("");
  }
  #endif

}

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void check_gpio(void)
{
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    //check  Select2_12V
    if (gpio_f_matrix[num_ic][1] > 4.0)
    {
      select_2_12V[num_ic] = 12;
      set_adc(MD_FILTERED,DCP_ENABLED,CELL_CH_ALL, AUX_CH_ALL);
    }
    else if (gpio_f_matrix[num_ic][1] < 1.0)
    {
      select_2_12V[num_ic] = 2;
      set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL, AUX_CH_ALL);
    }

    //check stack_union
    if (gpio_f_matrix[num_ic][2] > 4.0)
    {
      stack_union[num_ic] =0;
    }
    else if (gpio_f_matrix[num_ic][2] < 0.8)
    {
      stack_union[num_ic] =1;
    }
  }

}

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void cell2float(uint16_t cell_voltage_matrix[][NUM_CELL])
{
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
      for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
      {
        cell_voltage_f_matrix[num_ic][num_cell] =  (cell_voltage_matrix[num_ic][num_cell] / FACTDIV) + FACT_CORR;
      }
    }

}
/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
void gpio2float(uint16_t gpio_matrix[][6])
{
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
      for (int num_cell=0; num_cell<6; num_cell++)
      {
        gpio_f_matrix[num_ic][num_cell] =  (gpio_matrix[num_ic][num_cell] / FACTDIV) + FACT_CORR;
      }
    }

}

/////////////////////////////////////////////////////////////////////////////////////
//Calculate temperature using the Beta Factor equation
/////////////////////////////////////////////////////////////////////////////////////
#define TERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 25
#define NUMAMOSTRAS 5
#define BCOEFFICIENT 3435
#define SERIESRESISTOR 10000 // Value of the series resistor

float ntc10k(float media, uint16_t vref2)
{
  float temperatura;
  media = (vref2 / media) - 1;
  media = SERIESRESISTOR / media;
  //Calculate temperature using the Beta Factor equation
  temperatura = media / TERMISTORNOMINAL;     // (R/Ro)
  temperatura = log(temperatura); // ln(R/Ro)
  temperatura /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  temperatura += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  temperatura = 1.0 / temperatura;                 // Invert the value
  temperatura -= 273.15;                         // Convert it to Celsius
  return temperatura;
  }
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void read_stata()
{
  uint8_t r_stata[NUM_IC][8];//[6] y [7] = PEC
  int error;
  float dietemp;
  //
  error = LTC6804_rdstat('A',NUM_IC,r_stata);

  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    //for (int i=0; i<8; i++)
    //{  Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - STATA"); Serial.print(i); Serial.print(": ");  Serial.println(r_stata[num_ic][i],HEX); }

    sum_all_cells[num_ic] = (( ((uint16_t)r_stata[num_ic][1]<<8) | r_stata[num_ic][0]) * 20.0) /FACTDIV;
    
    dietemp=(((((uint16_t)r_stata[num_ic][3]<<8) | r_stata[num_ic][2])/FACTDIV)/7.5e-3) - 273;
    
    internal_die_temperature_IC[num_ic] = dietemp;   
    
    analog_psv_matrix[num_ic] = ( ((uint16_t)r_stata[num_ic][5]<<8) | r_stata[num_ic][4])/FACTDIV;
    
    //print received PEC [6], [7]
  }
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void read_statb()
{
  uint8_t r_statb[NUM_IC][8];
  uint8_t c,cbit;

  int error;

  error = LTC6804_rdstat('B',NUM_IC,r_statb);
//return;
  //_____________
  
  for (int num_ic=0; num_ic< NUM_IC; num_ic++)
  {
    c=0;

    for (int index=2; index<5; index++)
    {
      cbit=0;  
      
      for (int num_it=0; num_it<4; num_it++)
      {
        uv_flag_matrix[num_ic][c] = (r_statb[num_ic][index] & (1<<cbit)) >>cbit;
        cbit++;
        ov_flag_matrix[num_ic][c] = (r_statb[num_ic][index] & (1<<cbit)) >>cbit;
        cbit++;
        //
        c++;
      }
    }
    
    //__ADD. ltc_connected
    digital_psv_matrix[num_ic]= ( ( (uint16_t)r_statb[num_ic][1]<<8) | r_statb[num_ic][0]) / FACTDIV;

    if (digital_psv_matrix[num_ic] > 6.0)
      ltc_connected_matrix[num_ic] = 0;  
    else
      ltc_connected_matrix[num_ic] = 1; 
    //__
   

  }
}

/*****************************************************************************
VUV = VOV = 12 BITS = 0x0FFF = 4095

ADC Range aprox. -0.8192V -> 5.7344VV = 65536 = 2^16

float VUV >= 0.0016V
float VOV<= 5.73V

NOTE> SWRT Read-only
******************************************************************************/
void write_cfg(uint8_t GPIO5_1, uint8_t REFON, uint8_t ADCOPT, float VUV, float VOV, uint16_t DCC12_1, uint8_t DCTO)
{
  //uint8_t cfg[1][6];
  uint8_t cfg[NUM_IC][6];


  uint16_t vuv,vov;
  vuv= (uint16_t)  ( ( (VUV/100E-6)/16) -1);
  vov= (uint16_t)  ( (VOV/100E-6)/16);

  //printf("vuv 0x%X \n", vuv);
  //printf("vov 0x%X \n", vov);
  /*
  cfg[0][0]=((GPIO5_1 & 0x1F)<<3) | ((REFON & 0x01)<<2) | ((ADCOPT & 0x01));
  cfg[0][1]=(uint8_t)vuv;
  cfg[0][2]=(((uint8_t)vov)<<4) |  (((uint8_t) (vuv>>8)) & 0x0F);
  cfg[0][3]=(uint8_t)(vov>>4);
  cfg[0][4]= (uint8_t)DCC12_1;
  cfg[0][5]=  ((DCTO<<4)&0xF0) | ( (DCC12_1>>8) & 0x0F);
  */
  for (int num_ic=0; num_ic< NUM_IC; num_ic++)
  {
    cfg[num_ic][0]=((GPIO5_1 & 0x1F)<<3) | ((REFON & 0x01)<<2) | ((ADCOPT & 0x01));
    cfg[num_ic][1]=(uint8_t)vuv;
    cfg[num_ic][2]=(((uint8_t)vov)<<4) |  (((uint8_t) (vuv>>8)) & 0x0F);
    cfg[num_ic][3]=(uint8_t)(vov>>4);
    cfg[num_ic][4]= (uint8_t)DCC12_1;
    cfg[num_ic][5]=  ((DCTO<<4)&0xF0) | ( (DCC12_1>>8) & 0x0F);
  }  

  LTC6804_wrcfg(NUM_IC, cfg);

  }
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void read_config()
{
  uint8_t r_config[NUM_IC][8];
  //
  LTC6804_rdcfg(NUM_IC,r_config);
  
  #if FUNCT_MODE == __DEBUG
  Serial.println("READ CONFIG");
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    for (int i=0; i<8; i++)
    {
      Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - CFGR"); Serial.print(i); Serial.print(": ");  Serial.println(r_config[num_ic][i],HEX);
    }

  }
  #endif
}

/////////////////////////////////////////////////////////////////////////////////////
//OJO: como los IC se alimentan de su propio stack,
//es esa config. no es posible determinar si esta abierto el punto C0 y el C12
//Por lo tanto solo se puede determinar un openwire entre la celda 1 y la celda 11
//Y de manera certera siempre y cuando no existan mas de 2 openwire consecutivos.
//Si ese fuera el caso, toma la posicion mas inferior.

/////////////////////////////////////////////////////////////////////////////////////
 //#define NUM_PASS 2
 void open_wire(uint8_t NUM_PASS)
 {
  uint8_t i;
  uint16_t cell_PU[NUM_IC][12];
  uint16_t cell_PD[NUM_IC][12];
  float cell_DIFF[NUM_IC][12];

  //step 1
  set_adow(MD_FILTERED, PULLUP_CURRENT, DCP_DISABLED, CELL_CH_ALL );
  for (i=0; i<NUM_PASS; i++)
  {
    LTC6804_adow();
    delay(202);
  }
  LTC6804_rdcv(CELL_CH_ALL,NUM_IC,cell_PU);

  //step 2
  set_adow(MD_FILTERED, PULLDOWN_CURRENT, DCP_DISABLED, CELL_CH_ALL );
  for (i=0; i<NUM_PASS; i++)
  {
    LTC6804_adow();
    delay(202);
  }
 LTC6804_rdcv(CELL_CH_ALL, NUM_IC , cell_PD);

  //
  #if FUNCT_MODE == __DEBUG
  /*
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    for (int i=0; i<12; i++)
    {
      Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - CELDA PU"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_PU[num_ic][i]);
      Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - CELDA PD"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_PD[num_ic][i]);
      }
  }
  */
  #endif
  //
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    for (int i=0; i<12; i++)
    {
      cell_DIFF[num_ic][i] = (cell_PU[num_ic][i]/FACTDIV) - (cell_PD[num_ic][i]/FACTDIV);
      #if FUNCT_MODE == __DEBUG
    //  Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - CELDA DIFF"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_DIFF[num_ic][i]);
      #endif
      }
  }


  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    for (int num_cell=1; num_cell<=10; num_cell++)//celda1..11
    {
      if (cell_DIFF[num_ic][num_cell] < -0.4)//< -400mV
      {
        openwire_flag_matrix[num_ic][num_cell-1] = 1;            
        
        #if FUNCT_MODE == __DEBUG
        //Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - SIN CONEXION: "); Serial.print("CELDA"); Serial.println(num_cell);
        #endif
      }
      else
      {
        openwire_flag_matrix[num_ic][num_cell-1] = 0;          
      }        
    }
  }
}


