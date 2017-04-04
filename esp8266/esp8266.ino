    #include <ESP8266WiFi.h>
    #include "WiFiUDP.h"
    #include "EmonLib.h"
    #include <Wire.h>
    #include <RTClib.h>
    #include<Time.h>

    
    RTC_DS1307 RTC;  
    const char* ssid     = "le06";
    const char* password = "junior89";
    IPAddress ipBroadCast;
    unsigned int udpRemotePort = 8123;
    unsigned int udplocalPort = 8124;
    const int UDP_PACKET_SIZE = 128;
    char udpBuffer[ UDP_PACKET_SIZE];
    WiFiUDP udp;
    EnergyMonitor emon1;
    tmElements_t tmSet;    
    
    // Setup the Wifi connection
    double read_pin_A0(){
      double Irms = emon1.calcIrms(1480);  // Calculate Irms only
      Serial.print(Irms*230.0);         // Apparent power
      Serial.print(" ");
      Serial.println(Irms);          // Irms
      return Irms*230.0;  // Irms
    }
    void connectWifi() {
      Serial.print("Connecting to ");
      Serial.println(ssid); 
      //  Try to connect to wifi access point
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      ipBroadCast = ~WiFi.subnetMask() | WiFi.gatewayIP();
    }
    // Send udp message
    void udpSend()
    {
      // read the current value of an analog pin

      double value = read_pin_A0();
      time_t ts = getTimestamp();
      String device_id =  String(ESP.getChipId(), HEX);  
      Serial.println(device_id);
      String payload = "{\"numValue\"=" + String(value) + ",\"device\"=" + device_id + ",\"time\"=" + ts + ",\"region\"=\"fr-gre\"}";
      payload.toCharArray(udpBuffer,128);
      //strcpy(udpBuffer, "");
      udp.beginPacket(ipBroadCast, udpRemotePort);
      udp.write(udpBuffer, sizeof(udpBuffer));
      udp.endPacket();
      Serial.print("Broadcast: ");
      Serial.println(udpBuffer);
    }  
    // returns the current time in seconds from january 1st 1970 at 00:00:00
    time_t getTimestamp(){
      DateTime now = RTC.now();
      tmSet.Year = now.year() - 1970;
      tmSet.Month = now.month();
      tmSet.Day = now.day();
      tmSet.Hour = now.hour();
      tmSet.Minute = now.minute();
      tmSet.Second = now.second();
      time_t t = makeTime(tmSet);
      Serial.println(t);
      return t;
    }
    
    // Setup hardware, serial port, and connect to wifi.
    void setup() {
      Serial.begin(115200); 
      emon1.current(A0, 60);
      // RTC initialization 
      Wire.begin();
      RTC.begin();
      // following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(__DATE__, __TIME__));
    
      //delay(10);
      // We start by connecting to a WiFi network
      connectWifi();  
      Serial.println("Starting UDP");
      // set udp port for listen
      udp.begin(udplocalPort);
      Serial.print("Local port: ");
      Serial.println(udp.localPort());
    }  
    // LOOP MAIN
    // Send udp packet each 10 secconds
    void loop() {
      udpSend();
      delay (10000);
    }
