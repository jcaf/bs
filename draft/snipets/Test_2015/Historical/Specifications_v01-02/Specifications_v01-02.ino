
/*---------------------------------------------------------------------------*/
// LIBRARIES.SDCARD
// include the SD library:
#include <SPI.h>
#include <SD.h>
/*---------------------------------------------------------------------------*/
// LIBRARIES.LCD DISPLAY
#include <LiquidCrystal.h>
//RS/E/DB4/DB5/DB6/DB7
//RW=GND
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

/*---------------------------------------------------------------------------*/
// LIBRARIES. Ethernet
#include <Ethernet.h>

/*---------------------------------------------------------------------------*/
// VARIABLES.SDCARD
// Set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
// Arduino Ethernet shield: pin 4
const int chipSelect = 4;

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
// VARIABLES.LCD DISPLAY
String LINE01="";
String LINE02="";
String LINE03="";
String LINE04="";
/*---------------------------------------------------------------------------*/
// VARIABLES.ETHERNET

byte Mac[]={0x00,0x00,0x00,0x00,0x00,0x00};
//String Mac=""; 
//byte Mac[] =""; 
//String mac1={0x90,0xA2,0XDA,0X0E,0X08,0X51}; //MAC de la tarjeta Ethernet shield
//mac[]=mac1;
//byte ip[] = { 192,168,1,60 };                 //Direccion IP del arduino
int port=80;
EthernetClient client;

/*---------------------------------------------------------------------------*/

void setup()
{
  // Open serial communications:
  Serial.begin(9600);
  
  /*---------------------------------------------------------------------------*/
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //BACKLIGHT
  pinMode(17,OUTPUT);
  digitalWrite(17,HIGH);

  /*---------------------------------------------------------------------------*/
  // SD CARD Initialization:
  int visa3= sdcard_init();

  /*---------------------------------------------------------------------------*/  
  if(visa3==1){
    PrintLCDAnimation();
  }
  
  /*---------------------------------------------------------------------------*/  
  ethernet_connection();
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
 //int counter3=0;
 String mode1;
 
 LINE01="TEST H10:";
 // we'll use the initialization code from the utility libraries
 // since we're just testing if the card is working!
 if (!card.init(SPI_HALF_SPEED, chipSelect)) {
   LINE02="FAIL-> H10.1";
 } else {
     // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
     if (!volume.init(card)) {
       LINE02= "FAIL-> H10.2";
       visa=0;
     }else{
       LINE02=String("Format: FAT")+String(volume.fatType());
       visa=1;
     }  
 }
 PrintLCD(LINE01,LINE02,"","",0,"Override");
 PrintSerial(LINE01,LINE02,"","");  
 //------------------------------------------------------------------------------------------------------------     
 //Check the files and variables:
 
 if(visa==1){
   
   SD.begin(chipSelect);
   
   myFile = SD.open("/config");
   if (myFile) {
    LINE01="Reading File...";
    LINE02="/config";
    PrintLCD(LINE01,LINE02,"","",0,"Override");
    PrintSerial(LINE01,LINE02,"","");
    
    // close the file:
    myFile.close();
    visa1=1;
   } else {
    // if the file didn't open, print an error:
    visa1=0;
    
    LINE01="FAIL-> H10.3";
    LINE02="/config";
    PrintLCD(LINE01,LINE02,"","",0,"Override");
    PrintSerial(LINE01,LINE02,"","");
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
          LINE01="Reading File...";
          LINE02=FileName;
          PrintLCD(LINE01,LINE02,"","",0,mode1);
          PrintSerial(LINE01,LINE02,"","");
          
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
               LINE01=settingName;
               LINE02=settingValue;
               PrintLCD(LINE01,LINE02,"","",0,mode1);
               PrintSerial(LINE01,LINE02,"","");   
               
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
          
          LINE01=String("FAIL->")+String(error1);
          LINE02=FileName;
          PrintLCD(LINE01,LINE02,"","",0,"Production");
          PrintSerial(LINE01,LINE02,"","");
        }
        
     }  //Final del FOR
  
   }
   
 //Print results of FILES and VARIABLES found:
 if(counter1 !=FilesNumber){
   LINE01= "FAIL-> H10.10";
   if(counter1==(FilesNumber-1)){
     LINE02=String("Missing ")+String(FilesNumber-counter1)+String(" File");
   }else{
     LINE02=String("Missing ")+String(FilesNumber-counter1)+String(" Files");      
   }
   
   PrintLCD(LINE01,LINE02,"","",0,"Production");
   PrintSerial(LINE01,LINE02,"","");
 }else{
   counter2=CheckMainVariables();
   if(counter2 != 0){
     LINE01= "FAIL-> H10.11";
     if(counter2==1){
       LINE02= String("Missing ")+String(counter2)+String(" Var");  
     }else{
       LINE02= String("Missing ")+String(counter2)+String(" Vars");  
     }
     PrintLCD(LINE01,LINE02,"","",0,"Production");
     PrintSerial(LINE01,LINE02,"","");   
   }else{  
     visa2=1;
     
     LINE01= "TEST H10: OK";
     LINE02=String("Files found: ")+String(counter1)+String("/")+String(FilesNumber);
     LINE03=String("Vars  found: ")+String(VarsNumber)+String("/")+String(VarsNumber);
     
     PrintLCD(LINE01,LINE02,"","",0,"Production");
     PrintSerial(LINE01,LINE02,LINE03,""); 
   }
 }
} 

return visa2;
}  

void PrintLCD(String _LINE01, String _LINE02, String _LINE03, String _LINE04, int _TIME, String mode)
{
  int option=0;
  
  if(mode=="Override"){
    option=1;
  }
  
  if(FunctionMode=="Debug"){
    option=1;
  }
  
  if(FunctionMode=="Production"){
    if(mode=="Production"){
      option=1;
    }else{
      option=0;
    }
  }
  if(option==1){  
    
    if(_LINE01 != ""){
      lcd.setCursor(0, 0);
      lcd.print("                    ");
      lcd.setCursor(0, 0);
      lcd.print(_LINE01);
    }  
    if(_LINE02 != ""){
      lcd.setCursor(0, 1);
      lcd.print("                    ");
      lcd.setCursor(0, 1);
      lcd.print(_LINE02);
    }  
    if(_LINE03 != ""){
      lcd.setCursor(0, 2);
      lcd.print("                    ");
      lcd.setCursor(0, 2);
      lcd.print(_LINE03);
    }  
    if(_LINE04 != ""){
      lcd.setCursor(0, 3);
      lcd.print("                    ");
      lcd.setCursor(0, 3);
      lcd.print(_LINE04);
    }
  
    if(_TIME != 0){
      delay(_TIME);  
    }else{
      delay(3000);
    }
  }
}

void PrintLCDAnimation()
{
//    lcd.noBlink();
//    delay(3000);
//    // Turn on the blinking cursor:
//    lcd.blink();
//    delay(3000);
//
//    // Turn on the cursor:
//    lcd.cursor();
//    delay(500);
//    // Turn off the cursor:
//    lcd.noCursor();
//    delay(500);
    
    // Turn off the display:
    lcd.noDisplay();
    delay(3000);
     // Turn on the display:
    lcd.display();
    //delay(500);
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
        if(sub2=="B")
        {  dig[k3+1]= pow(16,4-k3)*12; }
        if(sub2=="D")
        {  dig[k3+1]= pow(16,4-k3)*13; }
        if(sub2=="E")
        {  dig[k3+1]= pow(16,4-k3)*14; }
        if(sub2=="F")
        {  dig[k3+1]= pow(16,4-k3)*15; }
        if(sub2=="0")
        {  dig[k3+1]= 0; }

        //if(sub2.toInt() != 0)
        //{  dig[k2+1]= pow(16,4-k2)*sub2.toInt(); }
        //Serial.print(" / ");
        //Serial.print(dig[k3+1]);
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
    LINE01=String("FAIL->")+String(error2);
    LINE02=String("Missing :")+String(error3);
    PrintLCD(LINE01,LINE02,"","",0,"Production");
    PrintSerial(LINE01,LINE02,LINE03,""); 
  } 

  return k1;  
}  

void ethernet_connection()
{
  //Ethernet communication
  //Connect with DHCP
  LINE01="ACQUIRING IP";
  LINE02="ADDRESS ...";
  PrintSerial(LINE01,LINE02,"","");
  PrintLCD(LINE01,LINE02,"","",0,"Production");
  
  if(!Ethernet.begin(Mac))  //Ethernet.begin(mac, ip);
  //if(!Ethernet.begin(mac,ip))  //Ethernet.begin(mac, ip);
  {
    Serial.println("Could not configure Ethernet with DHCP");
    
    LINE01="NO ETHERNET";
    LINE02="CONNECTION";
    PrintSerial(LINE01,LINE02,"","");
    PrintLCD(LINE01,LINE02,"","",0,"Production");
    //return; //Solo en el caso que queramos que el programa se plante y retorne para intentar reconectarse. Esto podria ser si fuera imprescindible la conexion
    
  }
  else
  {
    //Serial.println("Ethernet Configured !");
    //Serial.print("Client Started.\nLocal IP: ");
    Serial.println(Ethernet.localIP());
    
    LINE01="IP ADDRESS:";
    LINE02="Ethernet.localIP()";
    PrintSerial(LINE01,LINE02,"","");
    PrintLCD(LINE01,LINE02,"","",0,"Production");
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
