#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2


int indicator = 4;
int heater = 5;
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 2, 178 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 2, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
EthernetServer server(80);                             //server port     
String readString;

int setTemp;
int temperature;
int temperatureMax = 50;
int temperatureMin = 10;
int heaterState = 0;


void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  sensors.begin();
  pinMode(indicator, OUTPUT);
  pinMode(heater, OUTPUT);

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() 
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  temperature = sensors.getTempCByIndex(0);  
  Serial.print("Temperature =");
  Serial.println(temperature);
  
  EthernetClient client = server.available();
  if (client) 
  {
    while (client.connected()) 
    {   
      if (client.available())
      {
        char c = client.read();
     
        //read char by char HTTP request
        if (readString.length() < 100)
        {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') 
         {          
           Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://oprinter.pl/arduino/ethernetcss.css' />");
           client.println("<TITLE>Kontroler Temperatury</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Kontroler Temperatury</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Temperatura -</a>");
           client.println("<a href=\"/?button1off\"\">Temperatura +</a><br />");   
           client.println("<br />");     
           
           client.print("Zadana temperatura: ");
           client.print(setTemp);
           client.println("<br />");

           client.print("Aktualna temperatura: ");
           client.print(temperature);
           client.println("<br />");

           client.print("Stan lodowki: ");
           client.print(heaterState);
           client.println("<br />");

           client.print("Histereza: 2");
           client.println("<br />"); 
           client.println("</BODY>");
           client.println("</HTML>");
     
           delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
           if (readString.indexOf("?button1off") >0)
           {
                setTemp = setTemp+1;
                  if(setTemp>temperatureMax)
                    {
                      setTemp= temperatureMax;
                    }
           }
           if (readString.indexOf("?button1on") >0)
           {
                setTemp = setTemp-1;
                if(setTemp<temperatureMin)
                    {
                      setTemp= temperatureMin;
                    }
           }
            readString="";  
         }
       }
    }
  }

  if(temperature>setTemp)
      {
        digitalWrite(indicator,HIGH);
        digitalWrite(indicator,HIGH);
        heaterState = 1;
      }
  if(temperature<=setTemp)
      {
        digitalWrite(indicator,LOW);
        digitalWrite(indicator,LOW);
        heaterState = 0;
      }
}

