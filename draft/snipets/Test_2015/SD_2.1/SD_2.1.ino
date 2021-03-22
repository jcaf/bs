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
// VARIABLES. DIGITAl OUTPUTS. LEDS
#define LED1 34   // SD Card Error 
#define LED2 35   // Ethernet Connection Error
#define LED3 36   // UV Error
#define LED4 37   // OV Error
#define LED5 38   // 
#define LED6 39   //

/*---------------------------------------------------------------------------*/
// VARIABLES. DIGITAl OUTPUTS. RELES
#define RELE1 29
#define RELE2 30
#define RELE3 31
#define RELE4 32
#define RELE5 33

/*---------------------------------------------------------------------------*/
void setup()
{
  // Open serial communications:
  Serial.begin(115200);
  
  /*---------------------------------------------------------------------------*/
  // Set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  //BACKLIGHT
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
  //DIGITAL OUTPUTS: 

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
  //lcd.clear();


}

void loop(void) {
  //Menu
  //Measurement
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
  
  //Con lo anterior deshabilito los LTC y el ETHERNET
}

void EthernetEnable()
{
  //LTC6820. El Pin 5 del arduino va al enable del LTC6820 (U30). Este PIN debe estar a 5V para deshabilitar el circuito integrado
  digitalWrite(5,HIGH);
  pinMode(5,OUTPUT);
  
  //El pin de habilitacion del ETHERNET es el 10. Debe de estar en HIGH para deshabilitarse
  digitalWrite(10,LOW);
  pinMode(10,OUTPUT);
  
  //El pin de habilitacion del SDCARD a HIGH para deshabilitarse
  digitalWrite(4,HIGH);
  pinMode(4,OUTPUT);
  

}
