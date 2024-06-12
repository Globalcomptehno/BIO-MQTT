#include <TinyGsmClient.h>
#include <PubSubClient.h>

// Вставьте настройки GSM модуля
#define MODEM_RST            5
#define MODEM_PWRKEY         4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define SerialMon            Serial
#define SerialAT             Serial1

// Вставьте настройки вашей SIM карты
const char apn[]      = "yourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Вставьте данные вашего MQTT брокера
const char* broker = "mqtt.example.com";
const char* topic = "test/topic";

// Вставьте данные вашего устройства
const char* mqttUser = "yourMQTTUsername";
const char* mqttPassword = "yourMQTTPassword";

// Инициализация библиотеки
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

void setup() {
  // Настройка последовательного монитора
  SerialMon.begin(115200);
  delay(10);

  // Настройка связи с GSM модулем
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Настройка GSM модуля
  SerialMon.println("Initializing modem...");
  modem.restart();

  SerialMon.print("Connecting to ");
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    while (true);
  }
  SerialMon.println(" success");

  // Настройка MQTT
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();
}

void reconnect() {
  while (!mqtt.connected()) {
    SerialMon.print("Connecting to MQTT...");
    if (mqtt.connect("ESP32Client", mqttUser, mqttPassword)) {
      SerialMon.println(" connected");
      mqtt.subscribe(topic);
    } else {
      SerialMon.print(" failed, rc=");
      SerialMon.print(mqtt.state());
      SerialMon.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  SerialMon.print("Message arrived [");
  SerialMon.print(topic);
  SerialMon.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    SerialMon.print((char)payload[i]);
  }
  SerialMon.println();
}