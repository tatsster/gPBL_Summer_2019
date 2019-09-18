#include <ESP8266WiFi.h>
#include <DHT.h>

// DHT Temp sensor
#define DHTPIN 5  
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// Wifi info
const char* ssid = "DLS-Pro"; // Enter the SSID of your WiFi Network.
const char* password = "Abcd@1234";// Enter the Password of your WiFi Network.
char server[] = "smtp.mailgun.org"; // The SMTP Server 

WiFiClient espClient;
void setup()
{
//  Sensor setup
  dht.begin();
//  WIFI setup
  Serial.begin(9600);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 }

void loop()
{
//  DHT Temperature Calculate
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

// WIFI connect - Send mail  
  byte ret = sendEmail(h, t); // Send email
  delay(300000);
}

byte sendEmail(float humidity, float temp)
{
  if (espClient.connect(server, 587) == 1) 
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
  espClient.println("ZjE4Mzk3YjNlNTQ2NWQ4ZWMxNzMxNTJlNTE5NDUyZWQtYzI3YmY2NzItNmJkYTJkNDM=");//base64, ASCII encoded Password
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
