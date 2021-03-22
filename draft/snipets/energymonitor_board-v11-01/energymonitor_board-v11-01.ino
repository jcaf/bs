#include <LiquidCrystal.h>
#include <Ethernet.h>
#include <SPI.h>
/* ---------------------------------------------------------------------------------------------------- */
//Ethernet communication
byte mac[] = {  0x90,0xA2,0XDA,0X0E,0X08,0X51 }; //MAC de la tarjeta ethernet shield
//byte ip[] = { 192,168,1,60 };                 //Direccion IP del arduino
unsigned long id_number= 40773206;               //Identificador unico del equipo 
byte server[] = { 162,243,32,89 };               //Direccion ip del servidor Zeus
int port=80;
EthernetClient client;

volatile long previousMillis_1 = 0; 
long refresh_interval_1 = 600000;  

volatile long previousMillis_2 = 0; 
long refresh_interval_2 = 300000;  

volatile long previousMillis_3 = 0; 
long refresh_interval_3 = 180000;  

volatile long previousMillis_4 = 0; 
long refresh_interval_4 = 60000;  
/* ---------------------------------------------------------------------------------------------------- */
//RS/RW=GND/E/DB4/DB5/DB6/DB7
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

/* ---------------------------------------------------------------------------------------------------- */
//External Interrupt
const int button_int_18=5; //(interrupcion "5" esta en pin 18 en Arduino Mega 2560)
const int button_int_19=4; //(interrupcion "4" esta en pin 19 en Arduino Mega 2560)
volatile int selected=1;

/* ---------------------------------------------------------------------------------------------------- */
//Acquire Data
const int numCells=4;                         // Limit(Max "numCells" value) = 24 (for more cells measurement you need to modify Database configuration)  
//float total[numCells+1];                       // the running total
float total=0;
float average[numCells+1];                     // the average
int initial_cell=28;
float min_cell_value= 11.80;    //11.80
float max_cell_value= 13.50;    //13.50
int accuracy= 3; // Numero de digitos de precision
float min_batteryBank_value= 47.80;
float max_batteryBank_value= 52.50;

int alarm_low_voltage=2;
int alarm_high_voltage=2;

// Correction factor:
float sensorValue = 0;      
float vout= 5;               //The voltage between 5V and GRND pins
float conversion= vout/1023;

float correction_1= (12.37/12.24);  // (real/leido). El valor "Leido" se obtiene cuando se le aplica primero un "correction =1". El valor real es el del multimetro.
float correction_2= 1;              // (real/leido). El valor "Leido" se obtiene cuando se le aplica primero un "correction =1". El valor real es el del multimetro.

float automatic_correction_factor_1=1;
float automatic_correction_factor_2=1;

// Voltage Divider:
float resistance_4= 20;                        // 20 Kohm
float resistance_5= 10;                        // 10 Kohm
float resistance_6= 10;                        // 10 Kohm
float resistance_7= 10;                        // 10 Kohm

/*Pin40=0 : R4=20K, R5= 10K  Para leer 12VDC */
/*Pin40=1 : R6=10K, R7= 10K  Para leer 2VDC */

/* ---------------------------------------------------------------------------------------------------- */
int PIN_analogIn = A0;                     /*Analog input pin that the potentiometer is attached to*/
int PIN_selection=40;              /*Pin to select the voltage range of measurement*/  
int PIN_backlight=17;                      /*LCD BACKLIGHT*/
int PIN_5V_calibration=41;                 /*CALIBRATION RELAYS 5V*/
int PIN_3V_calibration=42;                 /*CALIBRATION RELAYS 3V*/
  
//04 DIGITAL OUTPUTS (RELAYS)
int dig_output_1=43;
int dig_output_2=44;
int dig_output_3=45;
int dig_output_4=46;

//04 DIGITAL OUTPUTS (LEDS)
int led_1=47;      // Ethernet PIN (azul)
int led_2=48;      // Low Level voltage alarm (azul)
int led_3=49;      // High level voltage alarm  (rojo)
int led_4=50;      // No utilizado

//03 DIGITAL INPUTS (RELAYS)
int dig_input_1=51;
int dig_input_2=52;
int dig_input_3=53;


void setup() 
{
  Serial.begin(9600);
  /* ---------------------------------------------------------------------------------------------------- */  
  digitalWrite(dig_output_1,HIGH);
  
  //BATTERIES//
  for(int thisCell=28;thisCell<=31;thisCell++)
  {                       
    pinMode(thisCell,OUTPUT);
    digitalWrite(thisCell,LOW);
  }
  
  pinMode(dig_output_1,OUTPUT);
  pinMode(dig_output_2,OUTPUT);
  pinMode(dig_output_3,OUTPUT);
  pinMode(dig_output_4,OUTPUT);
  
  digitalWrite(dig_output_1,LOW);
  digitalWrite(dig_output_2,LOW);
  digitalWrite(dig_output_3,LOW);
  digitalWrite(dig_output_4,LOW);
  
  pinMode(led_1,OUTPUT);
  pinMode(led_2,OUTPUT);
  pinMode(led_3,OUTPUT);
  pinMode(led_4,OUTPUT);
  
  digitalWrite(led_1,LOW);
  digitalWrite(led_2,LOW);
  digitalWrite(led_3,LOW);
  digitalWrite(led_4,LOW);

  pinMode(dig_input_1,INPUT);
  pinMode(dig_input_2,INPUT);
  pinMode(dig_input_3,INPUT);

  pinMode(PIN_analogIn,INPUT);
  
  pinMode(PIN_backlight,OUTPUT);
  digitalWrite(PIN_backlight,LOW);
  
  pinMode(PIN_5V_calibration,OUTPUT);  
  digitalWrite(PIN_5V_calibration,LOW);
  
  pinMode(PIN_3V_calibration,OUTPUT);
  digitalWrite(PIN_3V_calibration,LOW);

  

  /* ---------------------------------------------------------------------------------------------------- */  
  //Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  digitalWrite(PIN_backlight,HIGH);
  
  lcd_print_init();
  /* ---------------------------------------------------------------------------------------------------- */  
  ethernet_connection();

  /* ---------------------------------------------------------------------------------------------------- */  
  measurement_range();  
  measurement_calibration_5v();
  measurement_calibration_3v();
  correction_factors();

  generador_off();

  
}

void loop() 
{
  unsigned long currentMillis_1 = millis();  
  if(currentMillis_1 - previousMillis_1 >= refresh_interval_1) {
    // save the last time you changed the cell 
    previousMillis_1 = currentMillis_1;   
    
    ethernet_connection();
  }  
  //---------------------------------------------------------------
/*
  unsigned long currentMillis_2 = millis();  
  if(currentMillis_2 - previousMillis_2 >= refresh_interval_2) {
    // save the last time you changed the cell 
    previousMillis_2 = currentMillis_2;   
    
    generador_on();

  }  
  else
  {
    unsigned long currentMillis_3 = millis();  
    if(currentMillis_3 - previousMillis_3 >= refresh_interval_3) {
    // save the last time you changed the cell 
      previousMillis_3 = currentMillis_3;   
      
      generador_off();
    }  
  }
*/



  //---------------------------------------------------------------
  acquire_data();
  alarmas();
  
  //---------------------------------------------------------------
  unsigned long currentMillis_4 = millis();  
  if(currentMillis_4 - previousMillis_4 >= refresh_interval_4) {
    //Save the last time you changed the cell 
    previousMillis_4 = currentMillis_4;   
    
    database_update_2();

  }  
}

void lcd_print_init()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMPEL GROUP ");
  delay (5000);
  lcd.clear();  

}

void generador_off()
{
  digitalWrite(dig_output_1,HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GENERATOR OFF");
  delay (5000);
  lcd.clear(); 

}

void generador_on()
{
  digitalWrite(dig_output_1,LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GENERATOR ON");
  delay (5000);
  lcd.clear(); 
}

void measurement_range()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SELECT: PIN 40=");
  lcd.print(digitalRead(PIN_selection));

  lcd.setCursor(0, 1);

  if(digitalRead(PIN_selection)==0){
    lcd.print("12 VDC RANGE");
  }
  else
  {
    lcd.print("02 VDC RANGE");  
  }
  
  delay(5000);
  lcd.clear();

}

void measurement_calibration_5v()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rele 5.00:PIN=41");

  digitalWrite(PIN_5V_calibration,HIGH);
  delay(500);
  
  if(digitalRead(PIN_selection)==0)
  {
    sensorValue = (conversion*correction_1)*((resistance_4+resistance_5)/resistance_5)*analogRead(PIN_analogIn);
  }
  else
  {
    sensorValue = (conversion*correction_2)*((resistance_6+resistance_7)/resistance_7)*analogRead(PIN_analogIn);  
  }
  
  automatic_correction_factor_1=5.0/sensorValue;
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  lcd.print(sensorValue,2);
  lcd.print(" VDC");
  
  delay(5000);

  digitalWrite(PIN_5V_calibration,LOW);
}

void measurement_calibration_3v()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rele 3.30:PIN=42");

  digitalWrite(PIN_3V_calibration,HIGH);
  delay(500);
  
  if(digitalRead(PIN_selection)==0)
  {
    sensorValue = (conversion*correction_1)*((resistance_4+resistance_5)/resistance_5)*analogRead(PIN_analogIn);
  }
  else
  {
    sensorValue = (conversion*correction_2)*((resistance_6+resistance_7)/resistance_7)*analogRead(PIN_analogIn);  
  }
  
  automatic_correction_factor_2=3.3/sensorValue;
  
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  lcd.print(sensorValue,2);
  lcd.print(" VDC");
  
  delay(5000);

  digitalWrite(PIN_3V_calibration,LOW);
}

void correction_factors()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("f1(5.0)=");
  lcd.print(automatic_correction_factor_1,6);

  lcd.setCursor(0, 1);
  lcd.print("f2(3.3)=");  
  lcd.print(automatic_correction_factor_2,6);
  
  delay(5000);
}

void acquire_data()
{
  lcd.clear();
  
  lcd.setCursor(0, 0);
  //lcd.print("BATTERIES: PIN=");
  lcd.print("BATTERIES: ");
  
  total=0;

  for(int thisCell=1;thisCell<=numCells;thisCell++)
  {
    digitalWrite(thisCell+initial_cell-1,HIGH);
    delay(50);
       
    if(digitalRead(PIN_selection)==0)
    {
      average[thisCell] = (conversion*correction_1)*((resistance_4+resistance_5)/resistance_5)*analogRead(PIN_analogIn);
    }
    else
    {
      average[thisCell] = (conversion*correction_2)*((resistance_6+resistance_7)/resistance_7)*analogRead(PIN_analogIn);  
    }
    
    //lcd.setCursor(15, 0);
    //lcd.print(thisCell+initial_cell-1);
    lcd.setCursor(0, 1);
    lcd.print("CELL-");
    lcd.print(thisCell);
    lcd.print("=");  
    
    lcd.print(average[thisCell],2);
    lcd.print(" V");
    
    digitalWrite(thisCell+initial_cell-1,LOW);
    delay(1000);
    
    total=total+average[thisCell];

  }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BATTERIES: ");
    
    lcd.setCursor(0, 1);
    lcd.print("Total= ");
    lcd.setCursor(8, 1);
    lcd.print("        ");
    lcd.setCursor(8, 1);
    lcd.print(total);    
    lcd.print(" V");
    
    delay(3000);
    lcd.clear();
}

void ethernet_connection()
{
  //Ethernet communication
  //Connect with DHCP
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACQUIRING IP");
  lcd.setCursor(0, 1);
  lcd.print("ADDRESS ...");
    
  if(!Ethernet.begin(mac))  //Ethernet.begin(mac, ip);
  //if(!Ethernet.begin(mac,ip))  //Ethernet.begin(mac, ip);
  {
    Serial.println("Could not configure Ethernet with DHCP");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NO ETHERNET");
    lcd.setCursor(0, 1);
    lcd.print("CONNECTION");
    
    //return; //Solo en el caso que queramos que el programa se plante y retorne para intentar reconectarse. Esto podria ser si fuera imprescindible la conexion
    for(int c=1;c<=5;c++)
    {
      digitalWrite(led_3,HIGH);
      delay(500); 
      digitalWrite(led_3,LOW); 
      delay(500); 
    }
  }
  else
  {
    Serial.println("Ethernet Configured !");
    Serial.print("Client Started.\nLocal IP: ");
    Serial.println(Ethernet.localIP());
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP ADDRESS:");
    lcd.setCursor(0, 1);
    lcd.print(Ethernet.localIP());
    digitalWrite(led_1,HIGH); 
    delay(2000); 
  }
  
}

//Alarms:
void alarmas(){
  int low_counter=0;
  int high_counter=0;
  
  for(int thisCell = 1; thisCell <= numCells; thisCell++)
  {
    if(average[thisCell]<min_cell_value){
      low_counter++;
   
    }
    if(average[thisCell]>max_cell_value){
      high_counter++;
  
    }    
  }

  // LOW LIMIT
  if(low_counter>0){
    digitalWrite(led_3,HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LOW VOLTAGE ON:");
    lcd.setCursor(0, 1);
    lcd.print(low_counter);
    lcd.print(" CELLS");
    delay(2000);
  }
  else{
    digitalWrite(led_3,LOW);
    lcd.clear();
  }
  
  // HIGH LIMIT
  if(high_counter>0){
    digitalWrite(led_3,HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HIGH VOLTAGE ON:");
    lcd.setCursor(0, 1);
    lcd.print(high_counter);
    lcd.print(" CELLS");
    delay(2000);
  }
  else{
    digitalWrite(led_3,LOW);
    lcd.clear();
  }
  if(high_counter==0){
    if(low_counter==0){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NO ALARMS");
      generador_off();
      delay(2000);
    }
  }  
/*
  if(low_counter==0){
    if(high_counter==0){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("NO ALARMS");
      generador_off();
      delay(2000);
    }
  }
*/  
  //delay(3000);
  digitalWrite(led_3,LOW);
  //return;
  
  int alarm_low_voltage=2;
  int alarm_high_voltage=2;
  
  if(total < min_batteryBank_value){
    alarm_low_voltage=0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LOW VOLTAGE ON");
    lcd.setCursor(0, 1);
    lcd.print("BATTERY BANK");
    digitalWrite(led_3,HIGH);    
    delay(2000);
    generador_on();
    digitalWrite(led_3,LOW);    
  }
  if(total > max_batteryBank_value){
    alarm_high_voltage=0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HIGH VOLTAGE ON");
    lcd.setCursor(0, 1);
    lcd.print("BATTERY BANK");
    digitalWrite(led_3,HIGH);
    delay(2000);
    generador_off();
    digitalWrite(led_3,LOW);    
  }    

}

// Database update
void database_update_2()
{

  int k1;
  int k2;
  int node;
  
  if (client.connect(server, port)>0)  // Se conecta al servidor
  {        
    // Envia los datos utilizando GET
    //folder[50]= 'emoncmsv8';
    node=1;
    
    //client.print("GET /emoncmsv8/input/post.json?node=1&json={cell-1:2.1,cell-2:2.3,cell-3:2.3,cell-4:2.15,total_voltage:47.7,alarm_low_voltage:0,alarm_high_voltage:0}&apikey=892a9726c84cfd780f70d490176e6910");
    
    client.print("GET /");
    //client.print("rsm");
    client.print("energymonitor_server-v01-01");
    client.print("/input/post.json?node=");
    client.print(node);
    client.print("&json={");
    
    for(k1=1; k1<=numCells; k1++)
    {
      
      client.print("cell-");
      client.print(k1);
      client.print(":");
      client.print(average[k1],accuracy);
      client.print(",");          
      
    }
        
    client.print("total_voltage:");
    client.print(total,accuracy);
    client.print(",");
    
    client.print("alarm_low_voltage:");
    client.print(alarm_low_voltage);
    client.print(",");
    
    client.print("alarm_high_voltage:");
    client.print(alarm_high_voltage);
    client.print("}");
    client.print("&apikey=");
    client.print("892a9726c84cfd780f70d490176e6910");
    
       
    client.println(" HTTP/1.1");
    client.println("Host:quanticoservices.com");

    client.println("User-Agent: Arduino 1.0");
    
    Serial.println("Successfull connection to Zeus Server");

    digitalWrite(led_2,LOW);
    delay(500);
    digitalWrite(led_2,HIGH);
    delay(500);
    digitalWrite(led_2,LOW);
    delay(500);
    digitalWrite(led_2,HIGH);
    delay(500);
    digitalWrite(led_2,LOW);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SUCCESSFULL");
    lcd.setCursor(0, 1);
    lcd.print("UPLOAD TO ZEUS");
    delay(2000);
  }
  else
  {
    Serial.println("Failed connection to Zeus Server");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ZEUS-FAILED");
    lcd.setCursor(0, 1);
    lcd.print("CONNECTION");
    delay(2000);
  }
  if (client.connected()) {
  
  }
  else {
    //Serial.println("Desconectado");
  }
  
  client.println("Connection: close");
  client.println();
    
  client.stop();
  client.flush();
    
}  
