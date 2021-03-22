
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
/*---------------------------------------------------------------------------*/
// VARIABLES.LCD DISPLAY
String LINE01;
String LINE02;
String LINE03;
String LINE04;
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

  //sdcard_init();
  //sdcard_read(0);
  //sdcard_read(1);
  sdcard_scanFiles();
}

void loop(void) {

}

void sdcard_init()
{
  LINE01=XTring(1);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    LINE02= XTring(3);
        
    //return;
  } else {
       // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
       if (!volume.init(card)) {
        LINE02= XTring(4);
      }else{
        LINE02=String(XTring(2))+String(volume.fatType());
        
      }  

  }
  
  PrintLCD();
  PrintSerial();
 
}

void sdcard_scanFiles()
{
 SD.begin(chipSelect);
 char* FileName;
 char* FilePath="/config/";
 String FileRoute;
 int visa=0;
 
 myFile = SD.open("/config");
 if (myFile) {
  LINE01="Reading File...";
  LINE02="config";
  PrintLCD();
  PrintSerial();
  
  // read from the file until there's nothing else in it:
 
  // close the file:
  myFile.close();
  visa=1;
 } else {
  // if the file didn't open, print an error:
  LINE01="Error opening ";
  LINE02=FileName;
  PrintLCD();
  PrintSerial();
 }    
 
 if (visa==1){   
   for (int thisCounter=1; thisCounter <=6; thisCounter++){ 
     switch (thisCounter) {
      case 1:    
        FileName="specs.txt";
        break;
      case 2:    
        FileName="setcon.txt";
        break;
      case 3:    
        FileName="setacq.txt";
        break;
      case 4:   
        FileName="setup.txt";
        break;
      case 5:    
        FileName="funcmod.txt";
        break;
      case 6:    
        FileName="corfact.txt";
        break;
      } 
  
      FileRoute= String(FilePath)+String(FileName);
      
      char _FileRoute[FileRoute.length()+1];    
      //Convertir el STRING en CHAR para que lo pueda leer el SD.open(CHAR)
      FileRoute.toCharArray(_FileRoute,sizeof(_FileRoute));
  
      Serial.println("--------------------");
      Serial.println(FilePath);
      Serial.println(FileName);
      Serial.println(FileRoute);
      Serial.println(FileRoute.length());
          
      Serial.print("_FileRoute: ");    
      Serial.println(_FileRoute);
      
      // Open the file for reading:
      myFile = SD.open(_FileRoute);
      if (myFile) {
        LINE01="Reading File...";
        LINE02=FileName;
        PrintLCD();
        PrintSerial();
        
        // read from the file until there's nothing else in it:
     
        // close the file:
        myFile.close();
      } else {
        // if the file didn't open, print an error:
        LINE01="Error opening ";
        LINE02=FileName;
        PrintLCD();
        PrintSerial();
      }
      delay(1000);
    
   }  
 } 
}  

void sdcard_read(int m)
{
  SD.begin(chipSelect);
  char* FileName;
  
  char* files[]={"specs.txt",
                 "setcon.txt",
                 "setacq.txt",
                 "setup.txt",
                 "funcmod.txt",                 
                 "corfact.txt"
                 }; 
                
  FileName= files[m];  
  
  // open the file for reading:
  myFile = SD.open(FileName);
  if (myFile) {
    LINE01="Reading File...";
    LINE02=FileName;
    PrintLCD();
    PrintSerial();
    
    // read from the file until there's nothing else in it:
    lcd.setCursor(0, 0);
    while (myFile.available()) {
      Serial.write(myFile.read());
      lcd.print(String(myFile.read()));
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    LINE01="Error opening ";
    LINE02=FileName;
    PrintLCD();
    PrintSerial();
  }
}

void sdcard_write()
{
  SD.begin(chipSelect);
    
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("specs.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to specs.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test1.txt");
  }

 
}

char* XTring(int m)
//String XTring(int m)
{
  //String LineMessage;
  char* LineMessage;
  char* messages[]={"                ",
                    "TEST  :H10",
                    "PASS.FAT",
                    "FAIL-> H10.1",
                    "FAIL-> H10.2"
                      };
  
  LineMessage= messages[m];                    

  return LineMessage;
}

void PrintLCD()
{
  lcd.setCursor(0, 0);
  lcd.print(LINE01);
 
  lcd.setCursor(0, 1);
  lcd.print(LINE02);
 
}

void PrintSerial()
{
  Serial.println(LINE01);
  Serial.println(LINE02);
}
