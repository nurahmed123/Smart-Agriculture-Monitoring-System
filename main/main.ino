#include <SoftwareSerial.h>

SoftwareSerial SIM900A(22, 23); // gsm module connected here

///TODO: blynk and online setup
#define BLYNK_TEMPLATE_ID "TMPL6wzvPdyex"
#define BLYNK_TEMPLATE_NAME "wro"
#define BLYNK_AUTH_TOKEN "YQAifrXtD2LdoG2flvf-wqFvx47rcXJM"

#define BLYNK_PRINT Serial

#define DHTTYPE DHT11
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "DHT.h"

char ssid[] = "THE_SHADOW";
char pass[] = "@123456789@";

///TODO: tempareture and humadity
const int humadity_sensor = 8;
DHT dht(humadity_sensor, DHTTYPE);
float tempareture, humadity;

const int tempareture_pin_blynk = V1;
const int humadity_pin_blynk = V2;

///TODO: soil moisture
const int soil_moisture_pin = 29;
const int soil_moisture_pin_blynk = V3;
int soil_moisture_value;

///TODO:: solar control
const int upper_ldr = 14;
const int lower_ldr = 15;
const int error = 10;

int upper_ldr_value;
int lower_ldr_value;

int upper_ldr_value_filter;
int lower_ldr_value_filter;

int servo_starting_point = 90;
int servo_pin = 5;
Servo solarServo;

const int water_pump_pin = 12;


///TODO: Sending message
String alert_massage = "danger dhau danger vhag jau ji";
String phoneNumber = "+8801886772094";

void setup() {
  ///TODO: communication with app
  Serial.begin(115200);
  SIM900A.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();

  ///TODO: solar 
  solarServo.attach(servo_pin);

  pinMode(upper_ldr, INPUT);
  pinMode(lower_ldr, INPUT);
  pinMode(soil_moisture_pin, INPUT);
  pinMode(water_pump_pin, OUTPUT);
  
  solarServo.write(servo_starting_point);
}

///TODO: Send message and call
void sendSMS(String message){
  Serial.println ("Sending Message please wait….");
  SIM900A.println("AT+CMGF=1"); //Text Mode initialisation
  delay(1000);
  Serial.println ("Set SMS Number");
  SIM900A.println("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);
  Serial.println ("Set SMS Content");
  SIM900A.println(message);// Messsage content
  delay(100);
  Serial.println ("Done");
  SIM900A.println((char)26);// delay(1000);
  Serial.println ("Message sent succesfully");
}
void make_call(){
  Serial.println("Making a phone call...");

  // AT commands to call a number
  SIM900A.println("AT");
  delay(500);
  SIM900A.println("ATD" + phoneNumber + ";");
  delay(5000); // Adjust the delay based on your requirements
  SIM900A.println("ATH");

  Serial.println("Phone call completed.");
}



///TODO: water pump
BLYNK_WRITE(V0) {
  int waterpump_relayState = param.asInt(); // Read the button state

  // Update the corresponding relay state
  digitalWrite(water_pump_pin, waterpump_relayState);
}

///TODO: read DHT11 sensor idata and sent it to the server
void read_DHT11_sensor_and_send_data(){
  
  humadity = dht.readHumidity();
  tempareture = dht.readTemperature();
  Blynk.virtualWrite(humadity_pin_blynk, humadity);
  Blynk.virtualWrite(tempareture_pin_blynk, tempareture);
  
}

///TODO: soil_moisture
void read_soil_moisture_sensor_and_send_data()(
  soil_moisture_value = anlogRead(soil_moisture_pin);
  Blynk.virtualWrite(soil_moisture_pin_blynk, soil_moisture_value);
)

void solar_monnitor(){

  upper_ldr_value = analogRead(upper_ldr);
  lower_ldr_value = analogRead(lower_ldr);

  upper_ldr_value_filter = abs(upper_ldr_value - lower_ldr_value);
  lower_ldr_value_filter = abs(lower_ldr_value - upper_ldr_value);

  if((upper_ldr_value_filter <= error) || (lower_ldr_value_filter <= error)){
    return;
  }else{
    if(upper_ldr_value > lower_ldr_value){
      servo_starting_point -= 1;
    }
    
    if(upper_ldr_value < lower_ldr_value){
      servo_starting_point += 1;
    }
  }

  solarServo.write(servo_starting_point);

}

void loop() {
  Blynk.run();
  solar_monnitor();
  read_DHT11_sensor_and_send_data();
}
