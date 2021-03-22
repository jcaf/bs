#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC68042.h"
#include <SPI.h>

void setup() {
  Serial.begin(115200);//x debugging
  
  digitalWrite(10,HIGH);
  digitalWrite(4,HIGH);
  //
  pinMode(10,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(53,OUTPUT);//SPI.begin se encarga de poner SS x hardware como salida
  
  digitalWrite(LTC6804_CS,HIGH);
  pinMode(LTC6804_CS,OUTPUT);
  
  LTC6804_initialize();

//  open_wire();//while(1);
  
  
  //set_adc(MD_FILTERED,DCP_ENABLED,CELL_CH_ALL, AUX_CH_ALL);
  set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL, AUX_CH_ALL);
  //set_adc(MD_NORMAL,DCP_ENABLED,CELL_CH_ALL, AUX_CH_ALL);
  //set_adc(MD_NORMAL,DCP_DISABLED,CELL_CH_ALL, AUX_CH_ALL);

  write_cfg(0x1F,0,0,0.5,5.73,0x0FFF,0);

  LTC6804_adcv();
  delay(210);
  
  LTC6804_adax();
  delay(210);

  read_config();
  /*
  //
  set_stat(MD_FILTERED ,STAT_CH_ALL);
  LTC6804_adstat();
  delay(135);//134ms ALL
  //
  read_stata();
  read_statb();
  */
/*
delay(5000);
   read_config();//se resetea el modulo
  while(1);
*/
}

//
#define FACTDIV 10000.0 //10E4
#define NUMDECIMALS 3//4
//
#define NUM_IC 8//16

uint16_t cell_codes[NUM_IC][12];
 
uint16_t aux_codes[NUM_IC][6];

int dcp=0;


#define FAC_CORREC +0.004

//#define FAC_CORREC -0.0025

void loop() {
  //
  /*
  if (dcp==0)
  {
    set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL, AUX_CH_ALL);
    dcp=1;
    Serial.println("DCP DISABLED"); 
  }
  else
  {
    set_adc(MD_FILTERED,DCP_ENABLED,CELL_CH_ALL, AUX_CH_ALL);
    dcp=0;
    Serial.println("DCP ENABLED"); 
  }
  */

  write_cfg(0x1F,0,0,0.5,5.73,0xFFFF,0);
  
  LTC6804_adcv();
  delay(210);//0.201317s
  
  LTC6804_adax();
  delay(210);//0.201317s
  
  LTC6804_rdcv(CELL_CH_ALL,NUM_IC,cell_codes);
  LTC6804_rdaux(AUX_CH_ALL ,NUM_IC,aux_codes);
  
  for (int ic=0; ic<NUM_IC; ic++)
  {  
    for (int i=0; i<12; i++)
    {
      Serial.print("IC address "); Serial.print(ic);Serial.print(" - CELDA"); Serial.print(i+1); Serial.print(": ");  Serial.print((cell_codes[ic][i]/FACTDIV)+FAC_CORREC, NUMDECIMALS); Serial.println("V");
      }
     
  /* 
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 1 SUMA: "); Serial.println( ((cell_codes[ic][0]/FACTDIV)-0.0025) + ((cell_codes[ic][1]/FACTDIV)-0.0025) + ((cell_codes[ic][2]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 2 SUMA: "); Serial.println( ((cell_codes[ic][3]/FACTDIV)-0.0025) + ((cell_codes[ic][4]/FACTDIV)-0.0025) + ((cell_codes[ic][5]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 3 SUMA: "); Serial.println( ((cell_codes[ic][6]/FACTDIV)-0.0025) + ((cell_codes[ic][7]/FACTDIV)-0.0025) + ((cell_codes[ic][8]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);
      Serial.print("IC address:"); Serial.print(0);Serial.print(" CELDA12V 4 SUMA: "); Serial.println( ((cell_codes[ic][9]/FACTDIV)-0.0025) + ((cell_codes[ic][10]/FACTDIV)-0.0025) + ((cell_codes[ic][11]/FACTDIV)-0.0025) -0.01, NUMDECIMALS);

      Serial.print("IC address:"); Serial.print(0);Serial.print(" SUMA STACK: "); Serial.println( ((cell_codes[ic][0]/FACTDIV)-0.0025) + ((cell_codes[ic][1]/FACTDIV)-0.0025) + ((cell_codes[ic][2]/FACTDIV)-0.0025) +
      ((cell_codes[ic][3]/FACTDIV)-0.0025) + ((cell_codes[ic][4]/FACTDIV)-0.0025) + ((cell_codes[ic][5]/FACTDIV)-0.0025)+
      ((cell_codes[ic][6]/FACTDIV)-0.0025) + ((cell_codes[ic][7]/FACTDIV)-0.0025) + ((cell_codes[ic][8]/FACTDIV)-0.0025)+
      ((cell_codes[ic][9]/FACTDIV)-0.0025) + ((cell_codes[ic][10]/FACTDIV)-0.0025) + ((cell_codes[ic][11]/FACTDIV)-0.0025), NUMDECIMALS);

*/
      
/*
      set_stat(MD_FILTERED ,STAT_CH_ALL);
  LTC6804_adstat();
  delay(135);//134ms ALL
  //
  read_stata();
  read_statb();
  */  
      //GPIO1-5

      Serial.print("IC address:"); Serial.print(ic);Serial.print(" GPIO"); Serial.print(1); Serial.print(" - CURRENT SENSE: ");  Serial.print(aux_codes[ic][0]/FACTDIV, NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(ic);Serial.print(" GPIO"); Serial.print(2); Serial.print(" - SELECT 12V/2V: ");  Serial.print(aux_codes[ic][1]/FACTDIV, NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(ic);Serial.print(" GPIO"); Serial.print(3); Serial.print(" - STACK UNION 24 CELDAS: ");  Serial.print(aux_codes[ic][2]/FACTDIV, NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(ic);Serial.print(" GPIO"); Serial.print(4); Serial.print(" - NTC10K / VREF2 BUFF OPAMP: ");  Serial.print(aux_codes[ic][3]/FACTDIV, NUMDECIMALS); Serial.print(" -->TEMPERATURA: ");  Serial.print(ntc10k(aux_codes[ic][3], aux_codes[ic][5]));Serial.println(" C");
      Serial.print("IC address:"); Serial.print(ic);Serial.print(" GPIO"); Serial.print(5); Serial.print(" - VOLT.REF LTC6655CHMS-3.3V 0.05% 5ppm/C: ");  Serial.print(aux_codes[ic][4]/FACTDIV, NUMDECIMALS); Serial.println("V");
      Serial.print("IC address:"); Serial.print(ic);Serial.print(" VREF2(2.985V-3.015V) ");  Serial.print(": ");  Serial.print(aux_codes[ic][5]/FACTDIV, NUMDECIMALS);Serial.println("V");
  
  Serial.println("");

  
  }
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void write_config()
{
  uint8_t _config[NUM_IC][6];
  
  LTC6804_wrcfg(NUM_IC,_config);
  }

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void read_config()
{
  uint8_t r_config[NUM_IC][8];
  //
  LTC6804_rdcfg(NUM_IC,r_config);
  
  Serial.println("----------- READ CONFIG -------------");
for (int ic=0; ic<NUM_IC; ic++)
{  
  for (int i=0; i<8; i++)
  {
    Serial.print("IC address "); Serial.print(ic);Serial.print(" - CFGR"); Serial.print(i); Serial.print(": ");  Serial.println(r_config[ic][i],HEX); 
    }
  
  }
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
// Nominal temperature value for the thermistor
#define TERMISTORNOMINAL 10000
// Nominl temperature depicted on the datasheet
#define TEMPERATURENOMINAL 25
// Number of samples 
#define NUMAMOSTRAS 5
// Beta value for our thermistor
#define BCOEFFICIENT 3435
// Value of the series resistor
#define SERIESRESISTOR 10000

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
  uint8_t r_stata[NUM_IC][8];
  int error;
  //
  error = LTC6804_rdstat('A',NUM_IC,r_stata);
  
  if ( error!= 0)
    Serial.println("error stat");
  
  Serial.println("----------- STAT A -------------");
  for (int ic=0; ic<NUM_IC; ic++)
  {  
    //for (int i=0; i<8; i++)
    //{  Serial.print("IC address "); Serial.print(ic);Serial.print(" - STATA"); Serial.print(i); Serial.print(": ");  Serial.println(r_stata[ic][i],HEX); }
    
    Serial.print("IC address "); Serial.print(ic);Serial.print(" - SUM ALL CELL SOC: "); Serial.print(  ( ( ((uint16_t)r_stata[ic][1]<<8) | r_stata[ic][0])*20.0) /FACTDIV, NUMDECIMALS); Serial.println("V");
    
    float dietemp=(((((uint16_t)r_stata[ic][3]<<8) | r_stata[ic][2])/FACTDIV)/7.5e-3) - 273;
    Serial.print("IC address "); Serial.print(ic);Serial.print(" - INTERNAL DIE TEMP.: "); Serial.print(dietemp, NUMDECIMALS); Serial.println("C");
    Serial.print("IC address "); Serial.print(ic);Serial.print(" - ANALOG POWER VA [4.5V-5.5V]: "); Serial.print( ( ((uint16_t)r_stata[ic][5]<<8) | r_stata[ic][4])/FACTDIV, NUMDECIMALS); Serial.println("V");
    
    //print received PEC [6], [7]
    
  }
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
void read_statb()
{
  uint8_t r_statb[NUM_IC][8];
  int error;
  
  error = LTC6804_rdstat('B',NUM_IC,r_statb);
  
  if ( error!= 0)
    Serial.println("error stat");
  
  Serial.println("----------- STAT B -------------");
  for (int ic=0; ic<NUM_IC; ic++)
  {  
    /*
    for (int i=0; i<8; i++)
    {  
      Serial.print("IC address "); Serial.print(ic);Serial.print(" - STATB"); Serial.print(i); Serial.print(": ");  Serial.println(r_statb[ic][i],HEX); 
      }
      */
    Serial.print("IC address "); Serial.print(ic);Serial.print(" - DIGITAL POWER VD [2.7V-3.6V]: "); Serial.print(  (((uint16_t)+[ic][1]<<8) | r_statb[ic][0]) /FACTDIV, NUMDECIMALS); Serial.println("V");
     
    //
    uint8_t *p, base, count_cell;
    //
    p = &r_statb[ic][2];
    count_cell=1;
    
    for (int i=0; i<3; i++,p++)
    {
      base=0;
      for (int x=0; x<4; x++, base+=2, count_cell++)
      {
        Serial.print("IC address "); Serial.print(ic);Serial.print(" - C");Serial.print(count_cell);Serial.print("UV: "); Serial.println( (*p & (1<<base)   )>>base );
        Serial.print("IC address "); Serial.print(ic);Serial.print(" - C");Serial.print(count_cell);Serial.print("OV: "); Serial.println( (*p & (1<<(base+1)) )>> (base+1) );
        
      }
    }
     
    
    //print received PEC [6], [7]
    
  }
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
/*
void write_cfg()
{
  uint8_t cfg[1][6];
  
  cfg[0][0]=B11111000;
  cfg[0][1]=0;
  cfg[0][2]=0xF0;
  cfg[0][3]=0xFF;
  cfg[0][4]=0;
  cfg[0][5]=0;
  
  LTC6804_wrcfg(1, cfg);

  }
*/

/*****************************************************************************
VUV = VOV = 12 BITS = 0x0FFF = 4095

ADC Range aprox. -0.8192V -> 5.7344VV = 65536 = 2^16

float VUV >= 0.0016V
float VOV<= 5.73V

NOTE> SWRT Read-only
******************************************************************************/
void write_cfg(uint8_t GPIO5_1, uint8_t REFON, uint8_t ADCOPT, float VUV, float VOV, uint16_t DCC12_1, uint8_t DCTO)
{
  uint8_t cfg[1][6];
  
  uint16_t vuv,vov;
  vuv= (uint16_t)  ( ( (VUV/100E-6)/16) -1);
  vov= (uint16_t)  ( (VOV/100E-6)/16);

  //printf("vuv 0x%X \n", vuv);
  //printf("vov 0x%X \n", vov);

  cfg[0][0]=((GPIO5_1 & 0x1F)<<3) | ((REFON & 0x01)<<2) | ((ADCOPT & 0x01));
  cfg[0][1]=(uint8_t)vuv;
  cfg[0][2]=(((uint8_t)vov)<<4) |  (((uint8_t) (vuv>>8)) & 0x0F);
  cfg[0][3]=(uint8_t)(vov>>4);
  cfg[0][4]= (uint8_t)DCC12_1;
  cfg[0][5]=  ((DCTO<<4)&0xF0) | ( (DCC12_1>>8) & 0x0F);

  LTC6804_wrcfg(1, cfg);

  }

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
 #define NUM_PASS 2
 void open_wire(void)
 {
  uint8_t i;
  uint16_t cell_PU[NUM_IC][12];
  uint16_t cell_PD[NUM_IC][12];
  float cell_DIFF[NUM_IC][12];

  //1
  set_adow(MD_FILTERED, PULLUP_CURRENT, DCP_DISABLED, CELL_CH_ALL );
  for (i=0; i<NUM_PASS; i++)
  {
    LTC6804_adow();
    delay(210);
  }
  LTC6804_rdcv(CELL_CH_ALL,NUM_IC,cell_PU);

  //2
  set_adow(MD_FILTERED, PULLDOWN_CURRENT, DCP_DISABLED, CELL_CH_ALL );
  for (i=0; i<NUM_PASS; i++)
  {
    LTC6804_adow();
    delay(210);
  }
  LTC6804_rdcv(CELL_CH_ALL, NUM_IC , cell_PD);

  //
  for (int ic=0; ic<NUM_IC; ic++)
  {  
    for (int i=0; i<12; i++)
    {
      Serial.print("IC address "); Serial.print(ic);Serial.print(" - CELDA PU"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_PU[ic][i]); 
      Serial.print("IC address "); Serial.print(ic);Serial.print(" - CELDA PD"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_PD[ic][i]); 
      }
  }
  //
  for (int ic=0; ic<NUM_IC; ic++)
  {  
    for (int i=0; i<12; i++)
    {
      cell_DIFF[ic][i] = (cell_PU[ic][i]/FACTDIV) - (cell_PD[ic][i]/FACTDIV);
      Serial.print("IC address "); Serial.print(ic);Serial.print(" - CELDA DIFF"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_DIFF[ic][i]); 
      }
  }
  
  
  for (int ic=0; ic<NUM_IC; ic++)
  {  
    for (int i=1; i<=10; i++)//celda1..11
    {
      if (cell_DIFF[ic][i] < -0.4)//< -400mV
        {
          Serial.print("IC address "); Serial.print(ic);Serial.print(" - SIN CONEXION: "); Serial.print("CELDA"); Serial.println(i);
        }
      }
  }
  
  }
 
