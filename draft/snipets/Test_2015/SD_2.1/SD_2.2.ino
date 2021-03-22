/*---------------------------------------------------------------------------*/
// LIBRARIES.LCD DISPLAY
#include <LiquidCrystal.h>
//RS/E/DB4/DB5/DB6/DB7
//RW=GND
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

  // VARIABLES.LCD DISPLAY
  String LINE00="                    ";
  String LINE01="";
  String LINE02="";
  String LINE03="";
  String LINE04="";
  #define backLight 28

  #define LCD_ROW 4
  #define LCD_COL 20

/*---------------------------------------------------------------------------*/
// LIBRARIES.SDCARD
// include the SD library:
#include <SPI.h>
#include <SD.h>

  // VARIABLES.SDCARD
  // Set up variables using the SD utility library functions:
  Sd2Card card;
  SdVolume volume;
  SdFile root;

  // Arduino Ethernet shield: pin 4
  #define chipSelect 4
  
  File myFile;
  int FilesNumber=6;
  
  String SerialNumber="";
  String Server1IP="";
  String Host="";
  String Directory="";
  String ApiKeyW="";
  
  int VarsNumber=6;

/*---------------------------------------------------------------------------*/
// LIBRARIES. Ethernet
#include <Ethernet.h>

  // VARIABLES.ETHERNET

  byte Mac[6]={0x00,0x00,0x00,0x00,0x00,0x00};
  int port=80;
  EthernetClient client;
/*---------------------------------------------------------------------------*/
// LIBRARIES. LTC6804
#include <stdint.h>
#include "Linduino.h"
#include "LTC68042.h"
#include "LT_SPI.h"
#include <SPI.h>

  // VARIABLES. LTC6804
  #define __PRODUCTION 0
  #define __DEBUG 1

  //#define FUNCT_MODE __PRODUCTION
  #define FUNCT_MODE __DEBUG

  #define FACTDIV 10000.0 //10E4
  #define NUMDECIMALS 3//4

  #define NUM_IC 1//8
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

  uint8_t open_wire_num_pass=3;//100;//>=2 si FILTERED MODE 

  int dcp=0;




/*---------------------------------------------------------------------------*/
// VARIABLES. DIGITAL OUTPUTS. LEDS
#define LED1 34   // SD Card Error 
#define LED2 35   // Ethernet Connection Error
#define LED3 36   // UV Error
#define LED4 37   // OV Error
#define LED5 38   // 
#define LED6 39   //

/*---------------------------------------------------------------------------*/
// VARIABLES. DIGITAL OUTPUTS. RELES
#define RELE1 29
#define RELE2 30
#define RELE3 31
#define RELE4 32
#define RELE5 33

/*---------------------------------------------------------------------------*/
void setup()
{
  // Open serial communications:
  #if FUNCT_MODE == __DEBUG
    Serial.begin(115200);//x debugging
  #endif
  
  /*---------------------------------------------------------------------------*/
  // Set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // BACKLIGHT
  pinMode(backLight,OUTPUT);
  digitalWrite(backLight,HIGH);

  LINE01="BATTERY MONITOR";
  LINE02="";
  LINE03="Developed by:";
  LINE04="Quantico Services";
  PrintSerial(LINE01,LINE02,LINE03,LINE04);
  PrintLCD(LINE01,LINE02,LINE03,LINE04,0,0,0,0,0);

  lcd.clear();

  /*---------------------------------------------------------------------------*/  
  // DIGITAL OUTPUTS: 
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW);
  digitalWrite(LED5,LOW);
  digitalWrite(LED6,LOW);
  
  digitalWrite(RELE1,LOW);
  digitalWrite(RELE2,LOW);
  digitalWrite(RELE3,LOW);
  digitalWrite(RELE4,LOW);
  digitalWrite(RELE5,LOW);


  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);
  pinMode(RELE1,OUTPUT);
  pinMode(RELE2,OUTPUT);
  pinMode(RELE3,OUTPUT);
  pinMode(RELE4,OUTPUT);
  pinMode(RELE5,OUTPUT);

 
  /*---------------------------------------------------------------------------*/
  // SD CARD Initialization:
  SdCardEnable();  
  sdcardInit();
  lcd.clear();

  /*---------------------------------------------------------------------------*/  
  // Ethernet Initialization:
  EthernetEnable();
  EthernetConnection();
  lcd.clear();

  /*---------------------------------------------------------------------------*/  
  // LTC Initialization:
  LTC6804Enable();  
  LTC6804_initialize();

  //#define MD_FAST 1
  //#define MD_NORMAL 2
  //#define MD_FILTERED 3

  //set_adc(MD_FILTERED,DCP_ENABLED,CELL_CH_ALL, AUX_CH_ALL);
  set_adc(MD_FILTERED,DCP_DISABLED,CELL_CH_ALL, AUX_CH_ALL);
  set_stat(MD_FILTERED ,STAT_CH_ALL);
}

void loop(void) {
  
  LTC6804Measurement();
  // Server
  // Menu
}



int sdcardInit()
{
 //------------------------------------------------------------------------------------------------------------     
 //Check the SD (if inserted , if correctly formatted):
 
 int visa=0;  
 int visa1=0;
 int visa2=0;
 int k5=0;

 char character;
 char* FileName;
 char* FilePath="/config/";
 String FileRoute;
 
 String settingName="";
 String settingValue="";
 
 String error1;
 int counter1=0;
 int counter2=0;
 String mode1;
 
 LINE01="Initializing ...";
 LINE02="Running TEST: H10";
 // we'll use the initialization code from the utility libraries
 // since we're just testing if the card is working!
 if (!card.init(SPI_HALF_SPEED, chipSelect)) {
   LINE03="FAIL-> H10.1";
   digitalWrite(LED1,HIGH);
 } else {
     // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
     if (!volume.init(card)) {
       LINE03= "FAIL-> H10.2";
       digitalWrite(LED1,HIGH);
       visa=0;
     }else{
       LINE03=String("Format: FAT")+String(volume.fatType());
       visa=1;
     }  
 }
  PrintSerial(LINE01,LINE02,LINE03,"");  
  PrintLCD(LINE01,LINE02,LINE03,"",0,0,0,0,0);

 //------------------------------------------------------------------------------------------------------------     
 //Check the files and variables:
 
  if(visa==1){
    
    SD.begin(chipSelect);

    myFile = SD.open("/config");
    if (myFile) {
    LINE03="Reading File...";
    LINE04="/config";
    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);

    // close the file:
    myFile.close();
    visa1=1;
  }else{
    // if the file didn't open, print an error:
    visa1=0;

    LINE03="FAIL-> H10.3";
    digitalWrite(LED1,HIGH);
    LINE04="/config";
    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
  }    
   
  if (visa1==1){   
    for (int thisCounter=1; thisCounter <=FilesNumber; thisCounter++){ 
      switch (thisCounter) {
        case 1:    
          FileName="funcmod.txt";
          mode1="Production";
          error1="H10.4";
          break;
        case 2:    
          FileName="specs.txt";
          mode1="Debug";
          error1="H10.5";
          break;
        case 3:    
          FileName="setcon.txt";
          mode1="Production";
          error1="H10.6";
          break;
        case 4:   
          FileName="setacq.txt";
          mode1="Debug";
          error1="H10.7";
          break;
        case 5:    
          FileName="setup.txt";
          mode1="Debug";
          error1="H10.8";
          break;
        case 6:    
          FileName="corfact.txt";
          mode1="Debug";
          error1="H10.9";
          break;
      } 
    
      FileRoute= String(FilePath)+String(FileName);
        
      char _FileRoute[FileRoute.length()+1];    
      //Convertir el STRING en CHAR para que lo pueda leer el SD.open(CHAR)
      FileRoute.toCharArray(_FileRoute,sizeof(_FileRoute));

      // Open the file for reading:
      myFile = SD.open(_FileRoute);
        if (myFile) {
          LINE03="Reading File...";
          LINE04=FileName;
          PrintSerial("","",LINE03,LINE04);
          PrintLCD("","",LINE03,LINE04,0,0,0,0,0);

          
          // read from the file until there's nothing else in it:
          while (myFile.available()) {
             character = myFile.read();
             while((myFile.available()) && (character != '[')){
               character = myFile.read();
             }
             character = myFile.read();
             while((myFile.available()) && (character != '=')){
               settingName = settingName + character;
               character = myFile.read();
             }
             character = myFile.read();
             while((myFile.available()) && (character != ']')){
               settingValue = settingValue + character;
               character = myFile.read();
             }
             if(character == ']'){
               StoreMainVariables(settingName,settingValue);
               
               //Debuging Printing
                LINE03=settingName;
                LINE04=settingValue;
               
                PrintSerial("","",LINE03,LINE04); 
                PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
  
               // Reset Strings
               settingName ="";
               settingValue =""; 
             }
           }
          //-----------------------------------------------------     
          // close the file:
          myFile.close();
          counter1++;
        }else{
          // if the file didn't open, print an error:

          LINE03=String("FAIL->")+String(error1);
          digitalWrite(LED1,HIGH);
          LINE04=FileName;
          PrintSerial("","",LINE03,LINE04);
          PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
        }
    }  //Final del FOR
  } //visa1
   
 //Print results of FILES and VARIABLES found:
  if(counter1 !=FilesNumber){
    LINE03= "FAIL-> H10.10";
    digitalWrite(LED1,HIGH);
    if(counter1==(FilesNumber-1)){
     LINE04=String("Missing ")+String(FilesNumber-counter1)+String(" File");
    }else{
      LINE04=String("Missing ")+String(FilesNumber-counter1)+String(" Files");      
    }
   
    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
   
  }else{
    counter2=CheckMainVariables();
    k5=1; // In order to avoid another execution of this function: CheckMainVariables() 
   
   if(counter2 > 0)//any error?
   {
     LINE03= "FAIL-> H10.11";
     digitalWrite(LED1,HIGH);
     if(counter2==1)
     {
       LINE04= String("Missing ")+String(counter2)+String(" Var");  
     }else
     {
       LINE04= String("Missing ")+String(counter2)+String(" Vars");  
     }

     PrintSerial("","",LINE03,LINE04); 
     PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
       
   }
   else
   {  
     visa2=1;
     
     LINE02= "TEST H10: OK";
     digitalWrite(LED1,LOW);

     LINE03=String("Files found: ")+String(counter1)+String("/")+String(FilesNumber);
     LINE04=String("Vars  found: ")+String(VarsNumber)+String("/")+String(VarsNumber);
     
     PrintSerial("",LINE02,LINE03,LINE04); 
     PrintLCD("",LINE02,LINE03,LINE04,0,0,0,0,0);
     
   }

 }
} 

if(k5==0){
  CheckMainVariables();
}

return visa2;
}  

void PrintLCD(String _LINE01, String _LINE02, String _LINE03, String _LINE04, int _OffsetL1, int _OffsetL2, int _OffsetL3, int _OffsetL4, int _TIME)
{ 
    char _line01[LCD_COL+1],_line02[LCD_COL+1],_line03[LCD_COL+1],_line04[LCD_COL+1];

    if (_LINE01.length() > LCD_COL )
    {
      _LINE01.toCharArray(_line01, LCD_COL-2); //17 chars + ...
      for (int i=LCD_COL-3; i<LCD_COL;i++)
        {_line01[i]='.';}//add 3 ...
        //add null terminator
        _line01[LCD_COL]='\0';
        _LINE01 = _line01;//reassign
    }

    if (_LINE02.length() > LCD_COL )
    {
      _LINE02.toCharArray(_line02, LCD_COL-2); //17 chars + ...
      for (int i=LCD_COL-3 ; i<LCD_COL;i++)
        {_line02[i]='.';}//add 3 ...
        //add null terminator
        _line02[LCD_COL]='\0';
        _LINE02 = _line02;//reassign
    }

    if (_LINE03.length() > LCD_COL )
    {
      _LINE03.toCharArray(_line03, LCD_COL-2); //17 chars + ...
      for (int i=LCD_COL-3  ; i<LCD_COL;i++)
        {_line03[i]='.';}//add 3 ...
        //add null terminator
        _line03[LCD_COL]='\0';
        _LINE03 = _line03;//reassign
    }

    if (_LINE04.length() > LCD_COL )
    {
      _LINE04.toCharArray(_line04, LCD_COL-2); //17 chars + ...
      for (int x=LCD_COL-3 ; x<20; x++)
        {_line04[x]='.';}//add 3 ...
        
        //add null terminator
        _line04[LCD_COL]=0;
        _LINE04 = _line04;//reassign
    }



  if(_LINE01 != ""){
    lcd.setCursor(0, 0);
    lcd.print("                    ");
    lcd.setCursor(_OffsetL1, 0);
    lcd.print(_LINE01);
  }  
  if(_LINE02 != ""){
    lcd.setCursor(0, 1);
    lcd.print("                    ");
    lcd.setCursor(_OffsetL2, 1);
    lcd.print(_LINE02);
  }  
  if(_LINE03 != ""){
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(_OffsetL3, 2);
    lcd.print(_LINE03);
  }  
  if(_LINE04 != ""){
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(_OffsetL4, 3);
    lcd.print(_LINE04);
  }

  if(_TIME != 0){
    delay(_TIME);  
  }else{
    delay(500);
  }
}

void PrintSerial(String _LINE01, String _LINE02, String _LINE03, String _LINE04)
{
  if(_LINE01 != ""){
    Serial.println(_LINE01);
  }
  if(_LINE02 != ""){
    Serial.println(_LINE02);
  }
  if(_LINE03 != ""){
    Serial.println(_LINE03);
  }
  if(_LINE04 != ""){
    Serial.println(_LINE04);
  }
}

void StoreMainVariables(String var_name, String var_value){
  int k1=0;
  
  if(var_name=="SerialNumber"){
    SerialNumber=var_value;
    k1=1;
  }
  if(var_name=="Server1IP"){
    Server1IP=var_value;
    k1=1;
  }

  #define arr_leng 30
  #define buff_length 10
  
  char arr[arr_leng];
  char buff[buff_length];
  
  uint8_t ca,cb,cm;

  if(var_name=="Mac")
  {
      var_value.toCharArray(arr,arr_leng);
      ca=cb=cm=0;

      while (arr[ca] != '\0')
      {
        if (arr[ca]==',')
        {
          Mac[cm] =  (uint8_t) strtoul(buff, NULL, 16);
          cm++;
          ca++;//necesita 1 incremento x saltarse la coma
          cb=0;
          //limpieza del buffer
          for (int i=0; i<buff_length; i++)
              buff[i]=0;

        }
        buff[cb]=arr[ca];
        cb++;
        //
        ca++;
      }
    Mac[5] = (uint8_t) strtoul(buff, NULL, 16); 

    k1=1;

  }
  
  
  if(var_name=="Host"){
    Host=var_value;
    k1=1;
  }  
  if(var_name=="Directory"){
    Directory=var_value;
    k1=1;
  }  
  if(var_name=="ApiKeyW"){
    ApiKeyW=var_value;
    k1=1;
  }  
  
  if(k1==1)
  {
    Serial.print("->");
    Serial.print(var_name);
    Serial.print(" = ");
    Serial.println(var_value);
    
  }
}  

int CheckMainVariables(){
  int k1=0;
  String error2="";
  String error3="";
  
  #define defaultSerialNumber "D0000001"
  #define defaultServer1IP "xxxxx"
  #define defaultHost "quanticoservices.com"
  #define defaultDirectory "monitor-v01-01"
  #define defaultApiKeyW "xxxxx"
  byte defaultMac[6]={0x90,0xA2,0xDA,0x0E,0x08,0x55};

  
  if(SerialNumber==""){
    error2="H10.4.2";
    error3="SerialNumber";

    // Asign default values:
    SerialNumber=defaultSerialNumber;
    Serial.println("defaultSerialNumber stored");

    k1++;
  }
  
  if(Server1IP==""){
    error2="H10.5.1";
    error3="Server1IP";

    // Asign default values:
    Server1IP=defaultServer1IP;
    Serial.println("defaultServer1IP stored");

    k1++;
  }
  if(Host==""){
    error2="H10.5.2";
    error3="Host";

    // Asign default values:
    Host=defaultHost;
    Serial.println("defaultHost stored");

    k1++;
  }  
  if(Directory==""){
    error2="H10.5.3";
    error3="Directory";

    // Asign default values:
    Directory=defaultDirectory;
    Serial.println("defaultDirectory stored");

    k1++;
  }  
    
    
  if(ApiKeyW==""){
    error2="H10.5.7";
    error3="ApiKeyW";

    // Asign default values:
    ApiKeyW=defaultApiKeyW;
    Serial.println("defaultApiKeyW stored");

    k1++;
  } 

  //  
  uint8_t count_mac0=0;
  for (int i=0; i<6; i++){
    if (Mac[i] == 0) {count_mac0++;}
  }

  if (count_mac0 == 6)
  {
    error2="H10.5.8";
    error3="Mac";

    // Asign default values:
    Mac[1]= defaultMac[1];
    Mac[2]= defaultMac[2];
    Mac[3]= defaultMac[3];
    Mac[4]= defaultMac[4];
    Mac[5]= defaultMac[5];
    Serial.println("defaultMac stored");

    k1++;
  }



  if(k1 != 0){
    LINE03=String("FAIL->")+String(error2);
    digitalWrite(LED1,HIGH);
    LINE04=String("Missing :")+String(error3);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
    PrintSerial("","",LINE03,LINE04); 
  } 

  return k1;  
}  

String DisplayAddress(IPAddress(address))
{
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
}
void EthernetConnection()
{
  //Connect with DHCP
  LINE01="Initializing ...";
  LINE03="ACQUIRING IP";
  LINE04="ADDRESS ...";
  PrintSerial(LINE01,"",LINE03,LINE04);
  PrintLCD(LINE01,"",LINE03,LINE04,0,0,0,0,0);
  
  if(!Ethernet.begin(Mac))  //Ethernet.begin(mac, ip);
  {
    //Serial.println("Could not configure Ethernet with DHCP");
    
    LINE03="NO ETHERNET";
    LINE04="CONNECTION";
    digitalWrite(LED2,HIGH);

    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
    //return; 
    
  }
  else
  {
    Serial.println(Ethernet.localIP());
    
    LINE03="IP ADDRESS:";
    digitalWrite(LED2,LOW);
    LINE04=DisplayAddress(Ethernet.localIP());
    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
  } 
}



void SdCardEnable()
{
  //LTC6820. El Pin 5 del arduino va al enable del LTC6820 (U30). Este PIN debe estar a 5V para deshabilitar el circuito integrado
  digitalWrite(5,HIGH);
  pinMode(5,OUTPUT);
  
  //El pin de habilitacion del ETHERNET es el 10. Debe de estar en HIGH para deshabilitarse
  digitalWrite(10,HIGH);
  pinMode(10,OUTPUT);

  //El pin de habilitacion del SDCARD a HIGH para deshabilitarse
  digitalWrite(4,LOW);
  pinMode(4,OUTPUT);
  
  //Con lo anterior deshabilito los LTC y el ETHERNET
}

void EthernetEnable()
{
  //LTC6820. El Pin 5 del arduino va al enable del LTC6820 (U30). Este PIN debe estar a 5V para deshabilitar el circuito integrado
  digitalWrite(LTC6804_CS,HIGH);
  pinMode(LTC6804_CS,OUTPUT);
  
  //El pin de habilitacion del SDCARD a HIGH para deshabilitarse
  digitalWrite(4,HIGH);
  pinMode(4,OUTPUT);

  //El pin de habilitacion del ETHERNET es el 10. Debe de estar en HIGH para deshabilitarse
  digitalWrite(10,LOW);
  pinMode(10,OUTPUT);
}

void LTC6804Enable()
{
  //El pin de habilitacion del ETHERNET es el 10. Debe de estar en HIGH para deshabilitarse
  digitalWrite(10,HIGH);
  pinMode(10,OUTPUT);
  
  //El pin de habilitacion del SDCARD a HIGH para deshabilitarse
  digitalWrite(4,HIGH);
  pinMode(4,OUTPUT);

  //LTC6820. El Pin 5 del arduino va al enable del LTC6820 (U30). Este PIN debe estar a 5V para deshabilitar el circuito integrado
  /* En este momento aseguramos que este deshabilitado. Luego en la funcion de inicializacion
  se encargaran de ponerlo a LOW*/
  digitalWrite(LTC6804_CS,HIGH); 
  pinMode(LTC6804_CS,OUTPUT);

  pinMode(53,OUTPUT);//SPI.begin se encarga de poner SS x hardware como salida
}


void check_gpio(void)
{
  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {
    //check  Select2_12V
    if (gpio_f_matrix[num_ic][1] > 4.0)//
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
//OJO: Como los IC se alimentan de su propio stack,
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
//set_adow(MD_NORMAL, PULLUP_CURRENT, DCP_DISABLED, CELL_CH_ALL );
//set_adow(MD_NORMAL, PULLUP_CURRENT, DCP_ENABLED, CELL_CH_ALL );

  set_adow(MD_FILTERED, PULLUP_CURRENT, DCP_ENABLED, CELL_CH_ALL );
  for (i=0; i<NUM_PASS; i++)
  {
    LTC6804_adow();
    delay(202);
    //delay(102);
  }
  LTC6804_rdcv(CELL_CH_ALL,NUM_IC,cell_PU);

  //step 2
  set_adow(MD_FILTERED, PULLDOWN_CURRENT, DCP_DISABLED, CELL_CH_ALL );

  //set_adow(MD_NORMAL, PULLDOWN_CURRENT, DCP_DISABLED, CELL_CH_ALL );
  //set_adow(MD_NORMAL, PULLDOWN_CURRENT, DCP_ENABLED, CELL_CH_ALL );
  //set_adow(MD_FILTERED, PULLDOWN_CURRENT, DCP_ENABLED, CELL_CH_ALL );


  for (i=0; i<NUM_PASS; i++)
  {
    LTC6804_adow();
    delay(202);
    //delay(102);
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
      //Serial.print("IC address "); Serial.print(num_ic);Serial.print(" - CELDA DIFF"); Serial.print(i+1); Serial.print(": ");  Serial.println(cell_DIFF[num_ic][i]);
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


void LTC6804Measurement(){

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
      Serial.println(" >_ BATTERY STATUS ______________________________________________________________________");
      Serial.println("");
  
  Serial.println("  ________________________________");
  Serial.println("");

  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {

    if (ltc_connected_matrix[num_ic] == 1)
      {
        Serial.print("  |  IC address: "); Serial.print(num_ic);Serial.println("    ONLINE        |"); 
      }
      else{
        Serial.print("  |  IC address: "); Serial.print(num_ic);Serial.println("    OFFLINE    *  |"); 
      }
  }
  Serial.println("  ________________________________");

  for (int num_ic=0; num_ic<NUM_IC; num_ic++)
  {

      Serial.println("");

      if (ltc_connected_matrix[num_ic] == 1)
      {

        for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
        {
          Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - CELDA"); Serial.print(num_cell+1); Serial.print(": ");  Serial.print( cell_voltage_f_matrix[num_ic][num_cell], NUMDECIMALS); Serial.println("V");
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
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(1); Serial.print(" - CURRENT SENSE: ");  Serial.print(gpio_f_matrix[num_ic][0], NUMDECIMALS); Serial.println(" V");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(2); Serial.print(" - SELECT 12V/2V: ");  Serial.print(gpio_f_matrix[num_ic][1], NUMDECIMALS); Serial.println(" V");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(3); Serial.print(" - STACK UNION 24 CELDAS: ");  Serial.print(gpio_f_matrix[num_ic][2], NUMDECIMALS); Serial.println(" V");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(4); Serial.print(" - NTC10K / VREF2 BUFF OPAMP: ");  Serial.print(gpio_f_matrix[num_ic][3], NUMDECIMALS); Serial.print(" -->TEMPERATURA: ");  Serial.print(ntc10k(gpio_matrix[num_ic][3], gpio_matrix[num_ic][5]));Serial.println(" °C");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" GPIO"); Serial.print(5); Serial.print(" - VOLT.REF LTC6655CHMS-3.3V 0.05% 5ppm/C: ");  Serial.print(gpio_f_matrix[num_ic][4], NUMDECIMALS); Serial.println(" V");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" VREF2(2.985V-3.015V) ");  Serial.print(": ");  Serial.print(gpio_f_matrix[num_ic][5], NUMDECIMALS);Serial.println(" V");

        Serial.println("");

        //Display UV
        for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
        {
          Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - C");Serial.print(num_cell+1);Serial.print(" UV: "); Serial.println(uv_flag_matrix[num_ic][num_cell] );
        }
        
        Serial.println("");

        //Display OV
        for (int num_cell=0; num_cell<NUM_CELL; num_cell++)
        {
          Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - C");Serial.print(num_cell+1);Serial.print(" OV: "); Serial.println(ov_flag_matrix[num_ic][num_cell] );
        }
        
        Serial.println("");
        
        for (int num_cell=0; num_cell<(NUM_CELL-1); num_cell++)//open wire for C1-C11
        {
          if (openwire_flag_matrix[num_ic][num_cell] == 1)
          {
            Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - DISCONNECTED: "); Serial.print("Cell "); Serial.println(num_cell+1);
          }
        }
        
        //Serial.println("");
       
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - SUM ALL CELL SOC:             "); Serial.print(  sum_all_cells[num_ic], NUMDECIMALS); Serial.println(" V");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - INTERNAL DIE TEMP.:           "); Serial.print(internal_die_temperature_IC[num_ic], NUMDECIMALS); Serial.println(" C");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - ANALOG POWER VA [4.5V-5.5V]:  "); Serial.print(analog_psv_matrix[num_ic], NUMDECIMALS); Serial.println(" V");
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - DIGITAL POWER VA [2.7V-3.6V]: "); Serial.print(digital_psv_matrix[num_ic], NUMDECIMALS); Serial.println(" V");
     
        Serial.println("");

        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" -STACK UNION : ");Serial.println( stack_union[num_ic]);
        
        Serial.println("");
        
        Serial.print("  IC address "); Serial.print(num_ic);Serial.print(" - 2/12V : ");Serial.println(select_2_12V[num_ic]);
        
        Serial.println("");
      }
      else{
        //Serial.print(" IC address:"); Serial.print(num_ic);Serial.println(" OFFLINE"); 
        //Serial.println("");
      }
  }
  #endif


}
