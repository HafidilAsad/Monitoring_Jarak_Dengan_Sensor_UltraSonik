#include <WiFi.h>
#include <ThingsBoard.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define WIFI_AP "Wokwi-GUEST"
#define WIFI_PASSWORD ""

#define TOKEN "eT06n2C46R2owg5vJteR"

#define pintriger 13
#define pinecho 12
long duration, distance;


char thingsboardServer[] = "thingsboard.cloud";

WiFiClient client1;
ThingsBoard jarak1(client1);

// Initialize DHT sensor.


ThingsBoard device1(client1);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(115200);

  delay(10);
  InitWiFi();
  lastSend = 0;
  pinMode(pintriger, OUTPUT);
  pinMode(pinecho, INPUT);
}

void loop()
{
  if ( !device1.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update dan kirim tiap 1 detik
    kirimDataKeThingsboard();
    lastSend = millis();
  }

  digitalWrite(pintriger,LOW);
  delayMicroseconds(2);
  digitalWrite(pintriger, HIGH); //Trig pin should be high for 10uS according to the working of the sensor.
  delayMicroseconds(10);
  digitalWrite(pintriger,LOW);
  duration = pulseIn(pinecho,HIGH);

  /* Duration includes time taken for the wave to transmit and receive. 
  We're only interested in the duration of the receiving wave.
  Speed of sound in cm per microsecond is 34300/1000000 = 0.0343 cm/us
  (Duration/2)*0.0343 = Duration/58.2
  */
  
  distance = duration/58.2;

  Serial.println(distance);
  delay(50);

  //return String(distance);
}

void kirimDataKeThingsboard()
{
  Serial.println("Mengambil data sensor");

  // Reading temperature atau humidity
 

  Serial.println("Mengirim data ke ThingsBoard:");
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.print(" cm");
  

  jarak1.sendTelemetryFloat("jarak", distance);
}

void InitWiFi()
{
  Serial.println("Menghubungkan ...");
  // mencoba koneksi ke WiFi

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Berhasil terhubung!");
}

void reconnect() {
  // diulang sampai reconnected
  while (!jarak1.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Berhasil terhubung!");
    }
    Serial.print("Menghubungkan ke ThingsBoard node ...");
    if ( jarak1.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " [ retrying in 5 seconds]" );
      // Wait 5 detik
      delay( 5000 );
    }
  }
}
