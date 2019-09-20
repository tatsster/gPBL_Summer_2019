// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "SPIFFS.h"

#define DHTPIN 5
#define DHTTYPE DHT11 // DHT 11

#define yellow 12 // pin 9
#define green 14  // pin 18

// Replace with your network credentials
const char *ssid = "DLS-Pro";
const char *password = "Abcd@1234";
// const char* ssid = "Ngocthien_01";
// const char* password =  "huynhgiatrang";

// Create SMTP Server path
char Server[] = "smtp.mailgun.org";

bool PrvState = 0;

int Control = 0;

bool Open = 0;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/test");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
 
  if(type == WS_EVT_CONNECT){
 
    Serial.println("Websocket client connection received");
 
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected");
    Serial.println("-----------------------");
    Open = 0;
  } else if(type == WS_EVT_DATA){
    
    Serial.print("Data received: ");

    String message;
    Open = 1;
    
    for(int i=0; i < len; i++) {
          Serial.print((char) data[i]);
          message = message + (char) data[i];
    }

    if(message == "ON"){
      Serial.println("1");
      digitalWrite(green, HIGH);
      digitalWrite(yellow, LOW);
      Control = 1;
    }
    else if(message == "OFF"){
      Serial.println("0");
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
      Control = 2;
    }
    
    Serial.println();
  }
}

String readDHTTemperature()
{
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float t = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    }
    else
    {
        Serial.println(t);
        return String(t);
    }
}

String readDHTHumidity()
{
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    if (isnan(h))
    {
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    }
    else
    {
        Serial.println(h);
        return String(h);
    }
}

String readPower()
{
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    }
    else if (h > 75 && Open == 0)
    {
        Serial.println("ON");
        Control = 0;
        return "ON";
    }
    else if (h <= 75 && Open == 0)
    {
        Serial.println("OFF");
        Control = 0;
        return "OFF";
    }
    else if (Control == 1){
        Serial.println("ON");
        return "ON";
    }
    else if (Control == 2){
        Serial.println("OFF");
        return "OFF";
    }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Room And Air Conditioner Observation Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-snowflake" style="color:#00add6;"></i>
    <span class="dht-labels">Air-conditioner</span>
    <span id="power">%POWER%</span>
    <sup class="units"></sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 500 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 500 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("power").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/power", true);
  xhttp.send();
}, 500 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String &var)
{
    //Serial.println(var);
    if (var == "TEMPERATURE")
    {
        return readDHTTemperature();
    }
    else if (var == "HUMIDITY")
    {
        return readDHTHumidity();
    }
    else if (var == "POWER")
    {
        return readPower();
    }
    return String();
}

byte emailResp()
{
    byte responseCode;
    byte readByte;
    int loopCount = 0;

    while (!espClient.available())
    {
        delay(1);
        loopCount++;
        // Wait for 20 seconds and if nothing is received, stop.
        if (loopCount > 20000)
        {
            espClient.stop();
            Serial.println(F("\r\nTimeout"));
            return 0;
        }
    }

    responseCode = espClient.peek();
    while (espClient.available())
    {
        readByte = espClient.read();
        Serial.write(readByte);
    }

    if (responseCode >= '4')
    {
        //  efail();
        return 0;
    }
    return 1;
}

byte sendEmail(float humidity, float temp, bool status)
{
    if (espClient.connect(Server, 587) == 1)
    {
        Serial.println(F("connected"));
    }
    else
    {
        Serial.println(F("connection failed"));
        return 0;
    }
    if (!emailResp())
        return 0;
    //
    Serial.println(F("Sending EHLO"));
    espClient.println("EHLO www.example.com");
    if (!emailResp())
        return 0;
    //
    /*Serial.println(F("Sending TTLS"));
  espClient.println("STARTTLS");
  if (!emailResp()) 
  return 0;*/
    //
    Serial.println(F("Sending auth login"));
    espClient.println("AUTH LOGIN");
    if (!emailResp())
        return 0;
    //
    Serial.println(F("Sending username"));
    // Change this to your base64, ASCII encoded username
    /*
  For example, the email address test@gmail.com would be encoded as dGVzdEBnbWFpbC5jb20=
  */
    espClient.println("cG9zdG1hc3RlckBzYW5kYm94MTE1ZDBjOTBlNDczNDcyNzkxY2MwMmEwMDlmYTMzNGEubWFpbGd1bi5vcmc="); //base64, ASCII encoded Username
    if (!emailResp())
        return 0;
    //
    Serial.println(F("Sending password"));
    // change to your base64, ASCII encoded password
    /*
  For example, if your password is "testpassword" (excluding the quotes),
  it would be encoded as dGVzdHBhc3N3b3Jk
  */
    espClient.println("ZjE4Mzk3YjNlNTQ2NWQ4ZWMxNzMxNTJlNTE5NDUyZWQtYzI3YmY2NzItNmJkYTJkNDM="); //base64, ASCII encoded Password
    if (!emailResp())
        return 0;
    //
    Serial.println(F("Sending From"));
    // change to sender email address
    espClient.println(F("MAIL From: gunshot1st@gmail.com"));
    if (!emailResp())
        return 0;
    // change to recipient address
    Serial.println(F("Sending To"));
    espClient.println(F("RCPT To: gunshot1st@gmail.com"));
    if (!emailResp())
        return 0;
    //
    Serial.println(F("Sending DATA"));
    espClient.println(F("DATA"));
    if (!emailResp())
        return 0;
    Serial.println(F("Sending email"));
    // change to recipient address
    espClient.println(F("To:  gunshot1st@gmail.com"));
    // change to your address
    espClient.println(F("From: gunshot1st@gmail.com"));
    espClient.println(F("Subject: ESP8266 test e-mail\r\n"));
    espClient.println(F("This is is a HUMIDITY & TEMP e-mail sent from ESP8266.\n"));
    espClient.print(F("Humidity: "));
    espClient.println(String(humidity) + "%");
    espClient.print(F("Temperature: "));
    espClient.println(String(temp) + "*C");

    espClient.print(F("Conditioner: "));
    if (status)
        espClient.println(F("ON"));
    else
        espClient.println(F("OFF"));

    //
    espClient.println(F("."));
    if (!emailResp())
        return 0;
    //
    Serial.println(F("Sending QUIT"));
    espClient.println(F("QUIT"));
    if (!emailResp())
        return 0;
    //
    espClient.stop();
    Serial.println(F("disconnected"));
    return 1;
}

void setup()
{
    // Serial port for debugging purposes
    Serial.begin(115200);
    pinMode(green, OUTPUT);
    pinMode(yellow, OUTPUT);

    if(!SPIFFS.begin()){
       Serial.println("An Error has occurred while mounting SPIFFS");
       return;
    }

    dht.begin();

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readDHTTemperature().c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readDHTHumidity().c_str());
    });
    server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", readPower().c_str());
    });

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
   
    server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/ws.html", "text/html");
    });

    // Start server
    server.begin();
}

void loop()
{
    delay(500);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    if (h > 75 && Open == 0)
    {
        digitalWrite(green, HIGH);
        digitalWrite(yellow, LOW);
    }
    else if (h <= 75 && Open == 0)
    {
        digitalWrite(green, LOW);
        digitalWrite(yellow, HIGH);
    }

    bool CurrentState = PrvState;
    if (h > 75 && Open == 0)
        CurrentState = 1;
    else if (h <= 75 && Open == 0)
        CurrentState = 0;

    if (CurrentState != PrvState)
    {
        byte ret = sendEmail(h, t, CurrentState);
        PrvState = CurrentState;
    }
}
