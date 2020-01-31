#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <IRremote.h>
#include "EmonLib.h"
#include <WiFiUdp.h>
#include <SPI.h>
#include <NTPClient.h>

/*userID = "5e31f245d2ca69456015048d";
luz1ID = "5e31f27bd2ca69456015048f";
luz2ID = "5e31f280d2ca694560150490";
luz3ID = "5e31f284d2ca694560150491";
 arID = "5e31f28fd2ca694560150492";
tvID = "5e31f25bd2ca69456015048e";*/

//Fuso Horário, no caso horário de verão de Brasília
int timeZone = -3;

//Struct com os dados do dia e hora
struct Date
{
  int dayOfWeek;
  int day;
  int month;
  int year;
  int hours;
  int minutes;
  int seconds;
};

//Socket UDP que a lib utiliza para recuperar dados sobre o horário
WiFiUDP udp;

//Objeto responsável por recuperar dados sobre horário
NTPClient ntpClient(
    udp,                 //socket udp
    "0.br.pool.ntp.org", //URL do servwer NTP
    timeZone * 3600,     //Deslocamento do horário em relacão ao GMT 0
    60000);              //Intervalo entre verificações online

//--------FIM DA HORA-------//

const char *SSID = "Pex_Movel2.4";
const char *PASSWORD = "pexgeral@2020";
WiFiClient wifiClient;

const char *BROKER_MQTT = "18.231.176.98";
int BROKER_PORT = 1883;
#define LAMPADA1 18
#define LAMPADA2 19
#define LAMPADA3 21

#define CURRENT_CAL 1.8

#define ID_MQTT "AD005"
#define TOPIC_SUBSCRIBEluz1 "5e31f27bd2ca69456015048f/delta"
#define TOPIC_SUBSCRIBEluz2 "5e31f280d2ca694560150490/delta"
#define TOPIC_SUBSCRIBEluz3 "5e31f284d2ca694560150491/delta"
#define TOPIC_SUBSCRIBEar "5e31f28fd2ca694560150492/delta"
#define TOPIC_SUBSCRIBEtv "5e31f25bd2ca69456015048e/delta"
#define TOPIC_PUBLISHluz1 "5e31f245d2ca69456015048d/5e31f27bd2ca69456015048f/update"
#define TOPIC_PUBLISHluz2 "5e31f245d2ca69456015048d/5e31f280d2ca694560150490/update"
#define TOPIC_PUBLISHluz3 "5e31f245d2ca69456015048d/5e31f284d2ca694560150491/update"
#define TOPIC_PUBLISHar "5e31f245d2ca69456015048d/5e31f28fd2ca694560150492/update"
#define TOPIC_PUBLISHtv "5e31f245d2ca69456015048d/5e31f25bd2ca69456015048e/update"

#define FBluz1 35
#define FBluz2 34
#define FBluz3 32
#define FBar 25
#define FBtv 33
#define ruido 0.08

IRsend irsendTV(23);
IRsend irsendAC(27);
PubSubClient MQTT(wifiClient);

int frequencia = 38;

//variaveis ac
unsigned int desligar[] = {9050, 4500, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 1700, 600, 550, 550, 600, 550, 600, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int ligar[] = {9100, 4500, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura16[] = {9050, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 550, 600, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 550, 600, 600, 1650, 600, 550, 550, 600, 600, 550, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 550, 600, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550};
unsigned int temperatura17[] = {9100, 4500, 600, 550, 600, 550, 550, 600, 600, 1650, 600, 550, 550, 600, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 550, 600, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 550, 600, 550, 600, 600, 1650, 600, 550, 550, 600, 600, 550, 600, 550, 550, 600, 600, 1650, 600, 1650, 600, 550, 600, 550, 550, 600, 600, 550, 600};
unsigned int temperatura18[] = {9050, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 1650, 600, 1650, 550, 600, 550, 600, 550, 600, 550, 600, 550};
unsigned int temperatura19[] = {9050, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura20[] = {9050, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 1650, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura21[] = {9050, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 600, 550, 1650, 600, 1650, 600, 600, 550, 600, 550, 600, 600, 550, 600};
unsigned int temperatura22[] = {9100, 4500, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 1700, 550, 1700, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550};
unsigned int temperatura23[] = {9100, 4500, 600, 550, 600, 550, 550, 600, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 550, 600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 550, 600, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura24[] = {9050, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 550, 600, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 550, 1700, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura25[] = {9100, 4500, 600, 550, 550, 600, 550, 600, 550, 1700, 550, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 600, 550, 600, 550, 600, 1650, 550, 600, 600, 550, 600, 550, 600, 550, 550, 600, 550, 1700, 600, 1650, 600, 550, 550, 600, 600, 550, 550, 600, 600};
unsigned int temperatura26[] = {9100, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura27[] = {9050, 4500, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 600, 1650, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 1700, 550, 1700, 550, 600, 550, 600, 550, 600, 550, 600, 550};
unsigned int temperatura28[] = {9100, 4500, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 1650, 600, 1650, 600, 1650, 600, 550, 550, 600, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 500, 650, 550, 600, 1600, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura29[] = {9100, 4450, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 650, 500, 650, 500, 650, 500, 650, 500, 650, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};
unsigned int temperatura30[] = {9100, 4450, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 1650, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 600, 550, 650, 500, 600, 550, 650, 1600, 650, 500, 650, 550, 600, 550, 600, 550, 600, 550, 600, 1650, 600, 1650, 600, 550, 600, 550, 600, 550, 600, 550, 600};

//variaveis tv
unsigned int volumemais[] = {2650, 900, 450, 900, 450, 450, 400, 450, 1350, 1350, 450, 400, 450, 450, 450, 450, 400, 500, 450, 400, 450, 450, 400, 500, 450, 400, 450, 450, 450, 450, 850, 900, 450, 450, 450, 450, 400, 450, 450};
unsigned int volumemenos[] = {2650, 900, 400, 900, 450, 450, 450, 450, 1350, 1300, 400, 500, 400, 450, 450, 450, 450, 450, 450, 450, 450, 400, 450, 450, 450, 450, 400, 450, 450, 450, 900, 900, 400, 450, 450, 450, 850};
unsigned int tvon[] = {2650, 900, 400, 900, 450, 450, 450, 450, 400, 950, 850, 450, 450, 450, 400, 450, 450, 450, 450, 450, 400, 500, 400, 450, 450, 450, 450, 450, 400, 450, 450, 450, 450, 450, 900, 400, 450, 900, 450, 450, 400}; // UNKNOWN 9EFD9986

void feedback(void *param);
void mantemConexoesTask(void *param);
void conectaWifi();
void conectaMQTT();
void recebePacote(char *topic, byte *payload, unsigned int length);
void setupNTP();
Date getDate();

EnergyMonitor EMluz1;
EnergyMonitor EMluz2;
EnergyMonitor EMluz3;
EnergyMonitor EMar;
EnergyMonitor EMtv;

int estadoAnterior[5] = {0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(115200);

  EMluz1.current(FBluz1, CURRENT_CAL);
  EMluz2.current(FBluz2, CURRENT_CAL);
  EMluz3.current(FBluz3, CURRENT_CAL);
  EMar.current(FBar, CURRENT_CAL);
  EMtv.current(FBtv, CURRENT_CAL);
  pinMode(LAMPADA1, OUTPUT);
  pinMode(LAMPADA2, OUTPUT);
  pinMode(LAMPADA3, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);

  conectaWifi();

  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(recebePacote);
  conectaMQTT();

  //HORA
  setupNTP();

  Serial.print("Memoria livre: ");
  Serial.println(esp_get_free_heap_size());

  xTaskCreate(
      mantemConexoesTask,
      "Mantem conexoes",
      10000,
      NULL,
      0,
      NULL);

  xTaskCreate(
      feedback,
      "feedback",
      10000,
      NULL,
      0,
      NULL);

  //Over the air section
  ArduinoOTA.setHostname("Sala-wil");
  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready OTA ESP8266");
}

void loop()
{
  ArduinoOTA.handle();
  MQTT.loop();
}

void mantemConexoesTask(void *param)
{
  while (1)
  {
    conectaWifi();
    if (!MQTT.connected())
    {
      conectaMQTT();
    }
    vTaskDelay(30000 / portTICK_RATE_MS);
  }
}

Date getDate()
{
  //Recupera os dados de data e horário usando o client NTP
  char *strDate = (char *)ntpClient.getFormattedDate().c_str();

  //Passa os dados da string para a struct
  Date date;
  sscanf(strDate, "%d-%d-%dT%d:%d:%dZ",
         &date.year,
         &date.month,
         &date.day,
         &date.hours,
         &date.minutes,
         &date.seconds);

  //Dia da semana de 0 a 6, sendo 0 o domingo
  date.dayOfWeek = ntpClient.getDay();
  return date;
}

void setupNTP()
{
  ntpClient.begin();

  Serial.println("Waiting for first update");
  while (!ntpClient.update())
  {
    Serial.print(".");
    ntpClient.forceUpdate();
    delay(500);
  }

  Serial.println();
  Serial.println("First Update Complete");
}

void conectaWifi()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    return;
  }

  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("   Aguarde!");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);
  Serial.print("   IP obtido: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT()
{
  while (!MQTT.connected())
  {
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado ao Broker com sucesso!");
      MQTT.subscribe(TOPIC_SUBSCRIBEluz1);
      MQTT.subscribe(TOPIC_SUBSCRIBEluz2);
      MQTT.subscribe(TOPIC_SUBSCRIBEluz3);
      MQTT.subscribe(TOPIC_SUBSCRIBEar);
      MQTT.subscribe(TOPIC_SUBSCRIBEtv);
    }
    else
    {
      Serial.println("Nao foi possivel conectar-se ao broker");
      Serial.println("Tentando novamente em 5s");
      vTaskDelay(5000);
    }
  }
}

void recebePacote(char *topic, byte *payload, unsigned int length)
{
  String topico = String(topic);
  String msg;

  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  if (topico == TOPIC_SUBSCRIBEluz1)
  {
    if (msg == "{\"estado\":\"ligado\"}")
    {
      digitalWrite(LAMPADA1, !digitalRead(LAMPADA1));
      Serial.println("lampada1");
      MQTT.publish(TOPIC_PUBLISHluz1, "{\"estado\":\"ligado\"}");
      estadoAnterior[0] = 1;
    }
    else
    {
      digitalWrite(LAMPADA1, !digitalRead(LAMPADA1));
      Serial.println("lampada1");
      MQTT.publish(TOPIC_PUBLISHluz1, "{\"estado\":\"desligado\"}");
      estadoAnterior[0] = 0;
    }
  }
  if (topico == TOPIC_SUBSCRIBEluz2)
  {
    if (msg == "{\"estado\":\"ligado\"}")
    {
      digitalWrite(LAMPADA2, !digitalRead(LAMPADA2));
      Serial.println("lampada2");
      MQTT.publish(TOPIC_PUBLISHluz2, "{\"estado\":\"ligado\"}");
      estadoAnterior[1] = 1;
    }
    else
    {
      digitalWrite(LAMPADA2, !digitalRead(LAMPADA2));
      Serial.println("lampada2");
      MQTT.publish(TOPIC_PUBLISHluz2, "{\"estado\":\"desligado\"}");
      estadoAnterior[1] = 0;
    }
  }
  if (topico == TOPIC_SUBSCRIBEluz3)
  {
    if (msg == "{\"estado\":\"ligado\"}")
    {
      digitalWrite(LAMPADA3, !digitalRead(LAMPADA3));
      Serial.println("lampada3");
      MQTT.publish(TOPIC_PUBLISHluz3, "{\"estado\":\"ligado\"}");
      estadoAnterior[2] = 1;
    }
    else
    {
      digitalWrite(LAMPADA3, !digitalRead(LAMPADA3));
      Serial.println("lampada3");
      MQTT.publish(TOPIC_PUBLISHluz3, "{\"estado\":\"desligado\"}");
      estadoAnterior[2] = 0;
    }
  }

  if (topico == TOPIC_SUBSCRIBEar)
  {
    if (msg == "{\"estado\":\"ligado\"}")
    {
      irsendAC.sendRaw(ligar, sizeof(ligar) / sizeof(ligar[0]), frequencia);
      Serial.println("Comando enviado: Ligar AR");
      MQTT.publish(TOPIC_PUBLISHar, "{\"estado\":\"ligado\"}");
      estadoAnterior[3] = 1;
    }

    if (msg == "{\"estado\":\"desligado\"}")
    {
      irsendAC.sendRaw(desligar, sizeof(desligar) / sizeof(desligar[0]), frequencia);
      Serial.println("Comando enviado: Desligar AR");
      MQTT.publish(TOPIC_PUBLISHar, "{\"estado\":\"desligado\"}");
      estadoAnterior[3] = 0;
    }

    if (msg == "{\"temperatura\":16}")
    {
      irsendAC.sendRaw(temperatura16, sizeof(temperatura16) / sizeof(temperatura16[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 16");
    }

    if (msg == "{\"temperatura\":17}")
    {
      irsendAC.sendRaw(temperatura17, sizeof(temperatura17) / sizeof(temperatura17[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 17");
    }

    if (msg == "{\"temperatura\":18}")
    {
      irsendAC.sendRaw(temperatura18, sizeof(temperatura18) / sizeof(temperatura18[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 18");
    }

    if (msg == "{\"temperatura\":19}")
    {
      irsendAC.sendRaw(temperatura19, sizeof(temperatura19) / sizeof(temperatura19[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 19");
    }

    if (msg == "{\"temperatura\":20}")
    {
      irsendAC.sendRaw(temperatura20, sizeof(temperatura20) / sizeof(temperatura20[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 20");
    }

    if (msg == "{\"temperatura\":21}")
    {
      irsendAC.sendRaw(temperatura21, sizeof(temperatura21) / sizeof(temperatura21[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 21");
    }

    if (msg == "{\"temperatura\":22}")
    {
      irsendAC.sendRaw(temperatura22, sizeof(temperatura22) / sizeof(temperatura22[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 22");
    }

    if (msg == "{\"temperatura\":23}")
    {
      irsendAC.sendRaw(temperatura23, sizeof(temperatura23) / sizeof(temperatura23[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 23");
    }

    if (msg == "{\"temperatura\":24}")
    {
      irsendAC.sendRaw(temperatura24, sizeof(temperatura24) / sizeof(temperatura24[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 24");
    }

    if (msg == "{\"temperatura\":25}")
    {
      irsendAC.sendRaw(temperatura25, sizeof(temperatura25) / sizeof(temperatura25[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 25");
    }

    if (msg == "{\"temperatura\":26}")
    {
      irsendAC.sendRaw(temperatura26, sizeof(temperatura26) / sizeof(temperatura26[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 26");
    }

    if (msg == "{\"temperatura\":27}")
    {
      irsendAC.sendRaw(temperatura27, sizeof(temperatura27) / sizeof(temperatura27[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 27");
    }

    if (msg == "{\"temperatura\":28}")
    {
      irsendAC.sendRaw(temperatura28, sizeof(temperatura28) / sizeof(temperatura28[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 28");
    }

    if (msg == "{\"temperatura\":29}")
    {
      irsendAC.sendRaw(temperatura29, sizeof(temperatura29) / sizeof(temperatura29[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 29");
    }

    if (msg == "{\"temperatura\":30}")
    {
      irsendAC.sendRaw(temperatura30, sizeof(temperatura30) / sizeof(temperatura30[0]), frequencia);
      Serial.println("Comando enviado: TEMPERATURA 30");
    }
  }

  if (topico == TOPIC_SUBSCRIBEtv)
  {
    if (msg == "{\"estado\":\"ligado\"}")
    {
      irsendTV.sendRaw(tvon, sizeof(tvon) / sizeof(tvon[0]), frequencia);
      Serial.println("tv ligada");
      MQTT.publish(TOPIC_PUBLISHtv, "{\"estado\":\"ligado\"}");
      estadoAnterior[4] = 1;
    }
    else
    {
      irsendTV.sendRaw(tvon, sizeof(tvon) / sizeof(tvon[0]), frequencia);
      Serial.println("tv desligada");
      MQTT.publish(TOPIC_PUBLISHtv, "{\"estado\":\"desligado\"}");
      estadoAnterior[4] = 0;
    }

    if (msg == "{\"ultimoBotao\":\"volumemais\"}")
    {
      irsendTV.sendRaw(volumemais, sizeof(volumemais) / sizeof(volumemais[0]), frequencia);
      Serial.println("volume tv mais");
    }

    if (msg == "{\"ultimoBotao\":\"volumemenos\"}")
    {
      irsendTV.sendRaw(volumemenos, sizeof(volumemenos) / sizeof(volumemenos[0]), frequencia);
      Serial.println("volume tv menos");
    }
  }
}

void feedback(void *param)
{
  while (1)
  {
    EMluz1.calcVI(17, 100);
    EMluz2.calcVI(17, 100);
    EMluz3.calcVI(17, 100);
    EMar.calcVI(17, 100);
    EMtv.calcVI(17, 100);

    if (EMluz1.Irms - ruido > 0.01)
    {
      if (estadoAnterior[0] == 0)
      {
        estadoAnterior[0] = 1;
        MQTT.publish(TOPIC_PUBLISHluz1, "{\"estado\":\"ligado\"}");
      }
    }

    if (EMluz1.Irms - ruido <= 0.01)
    {
      if (estadoAnterior[0] == 1)
      {
        estadoAnterior[0] = 0;
        MQTT.publish(TOPIC_PUBLISHluz1, "{\"estado\":\"desligado\"}");
      }
    }

    if (EMluz2.Irms - ruido > 0.01)
    {
      if (estadoAnterior[1] == 0)
      {
        estadoAnterior[1] = 1;
        MQTT.publish(TOPIC_PUBLISHluz2, "{\"estado\":\"ligado\"}");
      }
    }
    if (EMluz2.Irms - ruido <= 0.01)
    {
      if (estadoAnterior[1] == 1)
      {
        estadoAnterior[1] = 0;
        MQTT.publish(TOPIC_PUBLISHluz2, "{\"estado\":\"desligado\"}");
      }
    }

    if (EMluz3.Irms - ruido > 0.01)
    {
      if (estadoAnterior[2] == 0)
      {
        estadoAnterior[2] = 1;
        MQTT.publish(TOPIC_PUBLISHluz3, "{\"estado\":\"ligado\"}");
      }
    }
    if (EMluz3.Irms - ruido <= 0.01)
    {
      if (estadoAnterior[2] == 1)
      {
        estadoAnterior[2] = 0;
        MQTT.publish(TOPIC_PUBLISHluz3, "{\"estado\":\"desligado\"}");
      }
    }

    if (EMar.Irms > 0.01)
    {
      if (estadoAnterior[3] == 0)
      {
        estadoAnterior[3] = 1;
        MQTT.publish(TOPIC_PUBLISHar, "{\"estado\":\"ligado\"}");
      }
    }

    if (EMar.Irms <= 0.01)
    {
      if (estadoAnterior[3] == 1)
      {
        estadoAnterior[3] = 0;
        MQTT.publish(TOPIC_PUBLISHar, "{\"estado\":\"desligado\"}");
      }
    }

    if (EMtv.Irms - ruido > 0.01)
    {
      if (estadoAnterior[4] == 0)
      {
        estadoAnterior[4] = 1;
        MQTT.publish(TOPIC_PUBLISHtv, "{\"estado\":\"ligado\"}");
      }
    }
    if (EMtv.Irms - ruido <= 0.01)
    {
      if (estadoAnterior[4] == 1)
      {
        estadoAnterior[4] = 0;
        MQTT.publish(TOPIC_PUBLISHtv, "{\"estado\":\"desligado\"}");
      }
    }

    vTaskDelay(5000 / portTICK_RATE_MS);
  }
}
