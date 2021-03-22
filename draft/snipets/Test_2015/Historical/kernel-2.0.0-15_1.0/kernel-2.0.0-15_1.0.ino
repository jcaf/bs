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
  
  String PartNumber="";
  String SerialNumber="";
  String SoftwareVersion="";
  String _Mac="";
  //={0x00,0x00,0x00,0x00,0x00,0x00};
  String Server1IP="";
  String Host="";
  String Directory="";
  String CustomerID="";
  String UserID="";
  String ApiKeyR="";
  String ApiKeyW="";
  String FunctionMode="";
  String Language="";
  int VarsNumber=13;

/*---------------------------------------------------------------------------*/
// LIBRARIES. Ethernet
#include <Ethernet.h>

// VARIABLES.ETHERNET

byte Mac[]={0x90,0xA2,0XDA,0X0E,0X08,0X51};
//String Mac=""; 
//byte Mac[] =""; 
//String mac1={0x90,0xA2,0XDA,0X0E,0X08,0X51}; //MAC de la tarjeta Ethernet shield
//mac[]=mac1;
//byte ip[] = { 192,168,1,60 };                 //Direccion IP del arduino
int port=80;
EthernetClient client;

/*---------------------------------------------------------------------------*/
// VARIABLES. DIGITAl OUTPUTS. LEDS
#define LED1 34
#define LED2 35
#define LED3 36
#define LED4 37
#define LED5 38
#define LED6 39

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
  Serial.begin(9600);
  
  /*---------------------------------------------------------------------------*/
  // Set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  //BACKLIGHT
  pinMode(28,OUTPUT);
  digitalWrite(28,HIGH);

  LINE01="BATTERY MONITOR";
  LINE02="";
  LINE03="Developed by:";
  LINE04="Quantico Services";
  PrintSerial(LINE01,LINE02,LINE03,LINE04);
  PrintLCD(LINE01,LINE02,LINE03,LINE04,0,0,0,0,0);
  // delay(5000);
  lcd.clear();

  /*---------------------------------------------------------------------------*/  
  //DIGITAL OUTPUTS: 
  
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
 
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  digitalWrite(LED3,HIGH);
  digitalWrite(LED4,HIGH);
  digitalWrite(LED5,HIGH);
  digitalWrite(LED6,HIGH);
  
  digitalWrite(RELE1,HIGH);
  digitalWrite(RELE2,HIGH);
  digitalWrite(RELE3,HIGH);
  digitalWrite(RELE4,HIGH);
  digitalWrite(RELE5,HIGH);
    
  /*---------------------------------------------------------------------------*/
  // SD CARD Initialization:
  SdCardEnable();  
  int visa3= sdcard_init();

  /*---------------------------------------------------------------------------*/  
  EthernetEnable();
  EthernetConnection();
}

void loop(void) {

}

int sdcard_init()
{
 //------------------------------------------------------------------------------------------------------------     
 //Check the SD (if inserted , if correctly formatted):
 
 int visa=0;  
 char character;
 char* FileName;
 char* FilePath="/config/";
 String FileRoute;
 
 String settingName="";
 String settingValue="";
 
 int visa1=0;
 int visa2=0;
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
 } else {
     // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
     if (!volume.init(card)) {
       LINE03= "FAIL-> H10.2";
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
   } else {
    // if the file didn't open, print an error:
    visa1=0;
    
    LINE03="FAIL-> H10.3";
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
               WriteMainVariables(settingName,settingValue);
               
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
        } else {
          // if the file didn't open, print an error:
          
          LINE03=String("FAIL->")+String(error1);
          LINE04=FileName;
          PrintSerial("","",LINE03,LINE04);
          PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
          
        }
        
     }  //Final del FOR
  
   }
   
 //Print results of FILES and VARIABLES found:
 if(counter1 !=FilesNumber){
   LINE03= "FAIL-> H10.10";
   if(counter1==(FilesNumber-1)){
     LINE04=String("Missing ")+String(FilesNumber-counter1)+String(" File");
   }else{
     LINE04=String("Missing ")+String(FilesNumber-counter1)+String(" Files");      
   }
   
   PrintSerial("","",LINE03,LINE04);
   PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
   
 }else{
   counter2=CheckMainVariables();
   if(counter2 != 0){
     LINE03= "FAIL-> H10.11";
     if(counter2==1){
       LINE04= String("Missing ")+String(counter2)+String(" Var");  
     }else{
       LINE04= String("Missing ")+String(counter2)+String(" Vars");  
     }
     PrintSerial("","",LINE03,LINE04); 
     PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
       
   }else{  
     visa2=1;
     
     LINE02= "TEST H10: OK";
     LINE03=String("Files found: ")+String(counter1)+String("/")+String(FilesNumber);
     LINE04=String("Vars  found: ")+String(VarsNumber)+String("/")+String(VarsNumber);
     
     PrintSerial("",LINE02,LINE03,LINE04); 
     PrintLCD("",LINE02,LINE03,LINE04,0,0,0,0,0);
     
   }
 }
} 

return visa2;
}  

void PrintLCD(String _LINE01, String _LINE02, String _LINE03, String _LINE04, int _OffsetL1, int _OffsetL2, int _OffsetL3, int _OffsetL4, int _TIME)
{ 
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
    delay(1000);
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

void WriteMainVariables(String var_name, String var_value){
  int k1=0;
  
  if(var_name=="PartNumber"){
    PartNumber=var_value;
    k1=1;
  }
  if(var_name=="SerialNumber"){
    SerialNumber=var_value;
    k1=1;
  }
  if(var_name=="SoftwareVersion"){
    SoftwareVersion=var_value;
    k1=1;
  }
  if(var_name=="Server1IP"){
    Server1IP=var_value;
    k1=1;
  }
  /*
  if(var_name=="Mac"){
    _Mac=var_value;
    //---------------------------------
    for(int k2=1;k2<=26;k2=k2+5)
    {
      String sub1= _Mac.substring(k2,k2+4);
      Serial.print("sub1 ");
      Serial.print(k2);
      Serial.print(" = ");     
      Serial.print(sub1);
      Serial.print(" | ");
      
      float dig[]={0,0,0,0,0};
      float number;
       
      for(int k3=0;k3<4;k3++)
      {
        String sub2=sub1.substring(k3,k3+1);
        Serial.print(" < ");
        Serial.print(sub2);
        Serial.print(" > ");
        
        if(sub2=="A")
        {  dig[k3+1]= pow(16,4-k3)*10; }
        if(sub2=="B")
        {  dig[k3+1]= pow(16,4-k3)*11; }
        if(sub2=="C")
        {  dig[k3+1]= pow(16,4-k3)*12; }
        if(sub2=="D")
        {  dig[k3+1]= pow(16,4-k3)*13; }
        if(sub2=="E")
        {  dig[k3+1]= pow(16,4-k3)*14; }
        if(sub2=="F")
        {  dig[k3+1]= pow(16,4-k3)*15; }
        if(sub2=="0")
        {  dig[k3+1]= 0; }
        //if(sub2 != "A")
        //{ dig[k3+1]= pow()*}
        if(sub2.toInt() != 0)
        {  dig[k3+1]= pow(16,4-k3)*sub2.toInt(); }
        Serial.print(" / ");
        Serial.print(dig[k3+1]);
        Serial.print(" | ");
        
      }
      Serial.print("Number= ");
      number= dig[1]+dig[2]+dig[3]+dig[4];
      Serial.println(number);
      Serial.println("");
    }
    

    //---------------------------------
    k1=1;
  }
  */
  if(var_name=="Host"){
    Host=var_value;
    k1=1;
  }  
  if(var_name=="Directory"){
    Directory=var_value;
    k1=1;
  }  
  if(var_name=="CustomerID"){
    CustomerID=var_value;
    k1=1;
  }  
  if(var_name=="UserID"){
    UserID=var_value;
    k1=1;
  }  
  if(var_name=="ApiKeyR"){
    ApiKeyR=var_value;
    k1=1;
  }  
  if(var_name=="ApiKeyW"){
    ApiKeyW=var_value;
    k1=1;
  }  
  if(var_name=="FunctionMode"){
    FunctionMode=var_value;
    k1=1;
  }  
  if(var_name=="Language"){
    Language=var_value;
    k1=1;
  } 
  
  if(k1==1){
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
  
  if(PartNumber==""){
    error2="H10.4.1";
    error3="PartNumber";
    k1++;
  }
  if(SerialNumber==""){
    error2="H10.4.2";
    error3="SerialNumber";
    k1++;
  }
  if(SoftwareVersion==""){
    error2="H10.4.3";
    error3="SoftwareVersion";
    k1++;
  }
  if(Server1IP==""){
    error2="H10.5.1";
    error3="Server1IP";
    k1++;
  }
  if(Host==""){
    error2="H10.5.2";
    error3="Host";
    k1++;
  }  
  if(Directory==""){
    error2="H10.5.3";
    error3="Directory";
    k1++;
  }  
  if(CustomerID==""){
    error2="H10.5.4";
    error3="CustomerID";
    k1++;
  }  
  if(UserID==""){
    error2="H10.5.5";
    error3="UserID";
    k1++;
  }  
  if(ApiKeyR==""){
    error2="H10.5.6";
    error3="ApiKeyR";
    k1++;
  }  
  if(ApiKeyW==""){
    error2="H10.5.7";
    error3="ApiKeyW";
    k1++;
  }  
  if(FunctionMode==""){
    error2="H10.8.1";
    error3="FunctionMode";
    k1++;
  }  
  if(Language==""){
    error2="H10.7.1";
    error3="Language";
    k1++;
  } 
  /*
  if(Mac==""){
    error2="H10.X.Y";
    error3="Mac";
    k1++;
  }
  */
  if(k1 != 0){
    LINE03=String("FAIL->")+String(error2);
    LINE04=String("Missing :")+String(error3);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
    PrintSerial("","",LINE03,LINE04); 
  } 

  return k1;  
}  

void EthernetConnection()
{
  //Ethernet communication
  //Connect with DHCP
  LINE03="ACQUIRING IP";
  LINE04="ADDRESS ...";
  PrintSerial("","",LINE03,LINE04);
  PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
  
  if(!Ethernet.begin(Mac))  //Ethernet.begin(mac, ip);
  //if(!Ethernet.begin(mac,ip))  //Ethernet.begin(mac, ip);
  {
    Serial.println("Could not configure Ethernet with DHCP");
    
    LINE03="NO ETHERNET";
    LINE04="CONNECTION";
    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
    //return; //Solo en el caso que queramos que el programa se plante y retorne para intentar reconectarse. Esto podria ser si fuera imprescindible la conexion
    
  }
  else
  {
    //Serial.println("Ethernet Configured !");
    //Serial.print("Client Started.\nLocal IP: ");
    Serial.println(Ethernet.localIP());
    
    IPAddress LocalIP =Ethernet.localIP();
    LINE03="IP ADDRESS:";
    //LINE04=LocalIP;
    LINE04="xxxxxxxxxx";
    PrintSerial("","",LINE03,LINE04);
    PrintLCD("","",LINE03,LINE04,0,0,0,0,0);
  } 
}


int x2i(char *s) 
{
 int x = 0;
 for(;;) {
   char c = *s;
   if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0'; 
   }
   else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10; 
   }   
   else if (c >= 'a' && c <= 'f') {
      x *= 16;
      x += (c - 'a') + 10;
   }
   else break;
   s++;
 }
 return x;
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
