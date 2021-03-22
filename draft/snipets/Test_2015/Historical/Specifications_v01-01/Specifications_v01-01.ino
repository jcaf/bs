
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
// VARIABLES.SDCARD
// Set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
// Arduino Ethernet shield: pin 4
const int chipSelect = 4;
File myFile;
int FilesNumber=6;
int counter=0;

String PartNumber="";
String SerialNumber="";
String SoftwareVersion="";
String Server1IP="";
String Host="";
String Directory="";
String CustomerID="";
String UserID="";
String ApiKeyR="";
String ApiKeyW="";
String FunctionMode="";
String Language="";
int VarsNumber=12;

/*---------------------------------------------------------------------------*/
// VARIABLES.LCD DISPLAY
String LINE01="";
String LINE02="";
String LINE03="";
String LINE04="";
/*---------------------------------------------------------------------------*/
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  /*---------------------------------------------------------------------------*/
  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //BACKLIGHT
  pinMode(17,OUTPUT);
  digitalWrite(17,HIGH);
  /*---------------------------------------------------------------------------*/

  // Initialization:
  int result_1= sdcard_init();
  
  // Scan for Files:
  if(result_1==1){
    int result_2=sdcard_scanFiles();
    if(result_2 !=FilesNumber){
      LINE01= "FAIL-> H10.10";
      if(result_2==(FilesNumber-1)){
        LINE02=String("Missing ")+String(FilesNumber-result_2)+String(" File");
      }else{
        LINE02=String("Missing ")+String(FilesNumber-result_2)+String(" Files");      
      }
      
      PrintLCD(LINE01,LINE02,"","");
      PrintSerial(LINE01,LINE02,"","");
    }else{
      counter=CheckMainVariables();
      if(counter != 0){
        LINE01= "FAIL-> H10.11";
        if(counter==1){
          LINE02= String("Missing ")+String(counter)+String(" Var");  
        }else{
          LINE02= String("Missing ")+String(counter)+String(" Vars");  
        }
        PrintLCD(LINE01,LINE02,"","");
        PrintSerial(LINE01,LINE02,"","");   
      }else{      
        LINE01= "TEST H10: OK";
        LINE02=String("Files found: ")+String(result_2)+String("/")+String(FilesNumber);
        LINE03=String("Variables found: ")+String(VarsNumber)+String("/")+String(VarsNumber);
        
        PrintLCD(LINE01,LINE02,"","");
        PrintSerial(LINE01,LINE02,LINE03,""); 
      }
    }  
  }  
  
}

void loop(void) {

}

int sdcard_init()
{
  int visa=0;
  
  LINE01="TEST H10:";
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    LINE02="FAIL-> H10.1";
  } else {
       // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
       if (!volume.init(card)) {
        LINE02= "FAIL-> H10.2";
      }else{
        LINE02=String("Format: FAT")+String(volume.fatType());
        visa=1;
      }  
  }
  
  PrintLCD(LINE01,LINE02,"","");
  PrintSerial(LINE01,LINE02,"","");
  
  return visa;
}

int sdcard_scanFiles()
{
 SD.begin(chipSelect);
 char character;
 char* FileName;
 char* FilePath="/config/";
 String FileRoute;
 
 String settingName="";
 String settingValue="";
 
 int visa1;
 String error;
 int counter1=0;
 
 myFile = SD.open("/config");
 if (myFile) {
  LINE01="Reading File...";
  LINE02="config";
  PrintLCD(LINE01,LINE02,"","");
  PrintSerial(LINE01,LINE02,"","");
  
  // close the file:
  myFile.close();
  visa1=1;
 } else {
  // if the file didn't open, print an error:
  visa1=0;
  
  LINE01="FAIL-> H10.3";
  LINE02="config";
  PrintLCD(LINE01,LINE02,"","");
  PrintSerial(LINE01,LINE02,"","");
 }    
 
 if (visa1==1){   
   for (int thisCounter=1; thisCounter <=FilesNumber; thisCounter++){ 
     switch (thisCounter) {
      case 1:    
        FileName="funcmod.txt";
        error="H10.4";
        break;
      case 2:    
        FileName="specs.txt";
        error="H10.5";
        break;
      case 3:    
        FileName="setcon.txt";
        error="H10.6";
        break;
      case 4:   
        FileName="setacq.txt";
        error="H10.7";
        break;
      case 5:    
        FileName="setup.txt";
        error="H10.8";
        break;
      case 6:    
        FileName="corfact.txt";
        error="H10.9";
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
        PrintLCD(LINE01,LINE02,"","");
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
             PrintLCD(LINE01,LINE02,"","");
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
        
        LINE01=String("FAIL->")+String(error);
        LINE02=FileName;
        PrintLCD(LINE01,LINE02,"","");
        PrintSerial(LINE01,LINE02,"","");
      }
      
   }  //Final del FOR

 } 
 return counter1;
}  

void PrintLCD(String _LINE01, String _LINE02, String _LINE03, String _LINE04, int _TIME)
{
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
  }
  
  if(_TIME != 0){
    delay(_TIME);  
  }else{
    delay(300);
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
  
  if(PartNumber==""){
    k1++;
  }
  if(SerialNumber==""){
    k1++;
  }
  if(SoftwareVersion==""){
    k1++;
  }
  if(Server1IP==""){
    k1++;
  }
  if(Host==""){
    k1++;
  }  
  if(Directory==""){
    k1++;
  }  
  if(CustomerID==""){
    k1++;
  }  
  if(UserID==""){
    k1++;
  }  
  if(ApiKeyR==""){
    k1++;
  }  
  if(ApiKeyW==""){
    k1++;
  }  
  if(FunctionMode==""){
    k1++;
  }  
  if(Language==""){
    k1++;
  } 
  
  return k1;  
}  
