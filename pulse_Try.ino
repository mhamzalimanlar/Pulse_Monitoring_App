#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>
//const char ssid[] = "FiberHGW_ZTRT2A_2.4GHz";
//const char pass[] = "jdANsNf9UY";
const char ssid[] = "Redmi";
const char pass[] = "16150680";
WiFiClient client;

String apiKey ="5YLUTW90PC2299F9";// Enter the Write API key 
const char* server = "api.thingspeak.com";


const int OUTPUT_TYPE = SERIAL_PLOTTER;
const int PULSE_INPUT = 0;   
const int PULSE_BLINK = D0;    
const int PULSE_FADE = 5;
const int THRESHOLD = 550;   

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor;
void setup() 
{
  Serial.begin(115200);
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  
  Serial.begin(115200);
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
  
  if (!pulseSensor.begin()) 
  {
    for(;;)
    {
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(50);
    }
  }
  WiFi.mode(WIFI_STA);
  
}  
void loop() 
{
  if (pulseSensor.sawNewSample()) 
  {
    if (--samplesUntilReport == (byte) 0) 
    {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      pulseSensor.outputSample();
      if (pulseSensor.sawStartOfBeat()) 
      {
        pulseSensor.outputBeat();
      }
    }
  } 
    int myBPM = pulseSensor.getBeatsPerMinute();

    if (myBPM < 100 && myBPM > 50){
      
                         if (client.connect(server,80))   // api.thingspeak.com ,sv ıp de girilebilir.  80 = standart TCP/IP portu
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(myBPM);
                             postStr +="&field2=";
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");  
                             client.print(postStr);
 
                             Serial.print("BPM: ");
                             Serial.print(myBPM);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(1000);

}
  }
