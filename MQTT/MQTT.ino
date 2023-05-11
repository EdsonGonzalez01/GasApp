/*************
 * Include Libraries
 **************/
#include <WiFi.h>
#include <PubSubClient.h>
/**************
 *
 * Define Constants
 **************/
#define WIFISSID "IoT" //WIFI SSID aqui
#define PASSWORD "1t3s0IoT23" // WIFI pwd
#define TOKEN "BBFF-WkTmRQU4Stx3sBzI0jsYlrkuv6oIoc" // Ubidots TOKEN name el mismo que usamos en clase
#define MQTT_CLIENT_NAME "UbidotsBroker-Gas" //ID del cliente, 8 a 12 chars alfanumericos (ASCII), debe ser random y unico dif a otros devices
#define VARIABLE_LABEL_gas "gas" // Variable Gas
#define DEVICE_LABEL "dev-ESP-SB" // Nombre del dispositivo a crear

char mqttBroker[]= "industrial.api.ubidots.com";
char payload[200]; // Leer y entender el payload aqui una de tantas referencias https://techterms.com/definition/payload
char topic[150]; //Espacio para el nombre del topico
// Space to store values to send
char str_gas[10];

const int AO_Pin=34; // Connect AO of MQ4 with Analog channel 0
int AO_Out; // stores analog output of MQ4 sensor

/**************
 * Funciones auxiliares
 **************/
WiFiClient ubidots;
PubSubClient client(ubidots);
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  Serial.write(payload, length);
  Serial.println(topic);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
/**************
 * Main Functions
 **************/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  Serial.println();
  Serial.print("Wait for WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  //Leer los datos del sensor
  Serial.println();
  Serial.println("A continuación los datos de los sensores:");
  Serial.println();
  // Publica en el topic de temperatura
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_gas); // Adds the variable label
  AO_Out= analogRead(AO_Pin);
  Serial.print("  1. El gas detectada en el sensor es de: ");
  Serial.println(AO_Out); // Imprime concentracion de gas en el serial monitor
  // numero maximo 4 precision 2 y convierte el valor a string
  dtostrf(AO_Out, 4, 2, str_gas);
  sprintf(payload, "%s {\"value\": %s", payload, str_gas); // formatea el mensaje a publicar
  sprintf(payload, "%s } }", payload); // cierra el mensaje
  Serial.println("  Enviando la Temp al virtualdevice en ubidots via MQTT....");
  client.publish(topic, payload);
  // Agregar una linea para separar temp de humedad
  Serial.println();
  client.loop();

  Serial.println();
  Serial.println("Esperaré 5 seg para leer nuevamente los sensores.....");
  Serial.println();
  Serial.println(".......................................................");
  delay(3500); // 12 segundos entre publicaciones en ubidots
}
