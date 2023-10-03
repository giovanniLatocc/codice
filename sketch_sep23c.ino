#define ESP_DRD_USE_SPIFFS true
 
// Include Libraries
 
// WiFi Library
#include <WiFi.h>
// File System Library
#include <FS.h>
// SPI Flash Syetem Library
#include <SPIFFS.h>
// WiFiManager Library
#include <WiFiManager.h>
// Arduino JSON library
#include <ArduinoJson.h>

#include <PubSubClient.h>
 
#include <DHT.h>


// JSON configuration file (nome del file di configurazione)
#define JSON_CONFIG_FILE "/fileDiConfigurazione7.json"
//Se si ha già caricato lo sketch sulla scheda ma si vuole aggiungere nuovi campi di testo personalizzati bisogna creare un nuovo file di configurazione cambiando il nome del file (JSON_CONFIG_FILE) successivamente sarà possibile aggiungere i nuovi campi

 
// Flag booleano per indicare che doremmo salvare la configurazione
bool shouldSaveConfig = false;
 
// Variabili che contengono i dati dei campi di testo personalizzati 
char testString26[50] = "testString26"; //Variabile stringa (Array di max 50 caratteri) con valore predefinito testString26
int valore1;
char *comboBox1[] = {"OUTPUT", "INPUT"};
char testString25[50] = "testString25"; //Variabile stringa (Array di max 50 caratteri) con valore predefinito testString25
int valore2;
char *comboBox2[] = {"OUTPUT", "INPUT"};
char testString32[50] = "testString32"; //Variabile stringa (Array di max 50 caratteri) con valore predefinito testString32
int valore3;
char *comboBox3[] = {"OUTPUT", "INPUT"};
char testString19[50] = "testString19";
int valore4;
char *comboBox4[] = {"OUTPUT", "INPUT"};
//Se si ha già caricato lo sketch sulla scheda ma si vuole aggiungere nuovi campi di testo personalizzati bisogna creare un nuovo file di configurazione cambiando il nome del file (JSON_CONFIG_FILE) successivamente sarà possibile aggiungere i nuovi campi

char mqtt_server[40] = "MQTT Server"; //Variabile stringa (Array di max 40 caratteri) con valore predefinito MQTT Server
char mqtt_port[6] = "8080"; //Variabile stringa (Array di max 6 caratteri) con valore predefinito 8080
char mqtt_topic[40] = "MQTT Topic";


// Oggetto di tipo WiFiManager
WiFiManager wm;

WiFiClient wifi;
PubSubClient client(wifi);

#define TRIGGER_PIN 4 //pin collegato al pulsante Down per resettare la rete wifi

#define PIN_26 26 //pin26 collegato al led verde
#define PIN_25 25 //pin25 collegato al led rosso
#define PIN_32 32 //pin32 collegato al relè

#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
long lastMsg = 0;

//Funzione di salvataggio dei file di configurazione in formato Json
void saveConfigFile(){

  Serial.println(F("Saving configuration..."));
  
  // Creazione di un documento JSON con il relativo spazio disponibile e gli elementi da salvare al suo interno
  StaticJsonDocument<600> json;
  json["testString26"] = testString26;
  json["valore1"] = valore1;
  json["testString25"] = testString25;
  json["valore2"] = valore2;
  json["testString32"] = testString32;
  json["valore3"] = valore3;
  json["testString19"] = testString19;
  json["valore4"] = valore4;

  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_topic"] = mqtt_topic;

  

 
  // Istruzioni per aprire il file di configurazione e scrivrci all'interno i dati da salvare 
  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile){
    // Se non si riesce ad aprire il file di configurazione viene stampato il seguente messaggio
    Serial.println("failed to open config file for writing");
  }
 
  // Istruzioni per serializzare i dati Json all'interno del file di configurazione
  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0){
    // Istruzione che viene stampata se si ha un errore durante la scrittura dei dati
    Serial.println(F("Failed to write to file"));
  }
  // Chiusura del file di configurazione
  configFile.close();
}
 
//Funzione di caricamento dei file di configurazione
bool loadConfigFile(){

  // Istruzione per formattare l'area di memoria dello SPIFFS
  // SPIFFS.format();
 
  //Lettura dei file di configurazione dal file system 
  Serial.println("Mounting File System...");
 
  if (SPIFFS.begin(false) || SPIFFS.begin(true)){
    Serial.println("mounted file system");
    // istruzione che verifica l'esistenza del file system
    if (SPIFFS.exists(JSON_CONFIG_FILE)){
      // Se il file esiste lo leggiamo e lo carichiamo
      Serial.println("reading config file");
      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
      if (configFile){
        Serial.println("Opened configuration file");
        //Istruzione che crea un documento Json
        StaticJsonDocument<600> json;
        //Deserializziamo i dati e li carichiamo nel documento json
        DeserializationError error = deserializeJson(json, configFile);
        serializeJsonPretty(json, Serial);
        if (!error){
          //Istruzioni per analizzare il file json
          Serial.println("Parsing JSON");
 
          strcpy(testString26, json["testString26"]);
          valore1 = json["valore1"].as<int>();
          strcpy(testString25, json["testString25"]);
          valore2 = json["valore2"].as<int>();
          strcpy(testString32, json["testString32"]);
          valore3 = json["valore3"].as<int>();
          strcpy(testString19, json["testString19"]);
          valore4 = json["valore4"].as<int>();
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_topic, json["mqtt_topic"]);
 
          return true;
        }
        else{
          //Istruzione da chiamare se si verifica un errore durante il caricamento del json
          Serial.println("Failed to load json config");
        }
      }
    }
  }
  else{
    // Erroe durante il montaggio del file system
    Serial.println("Failed to mount FS");
  }
 
  return false;
}
 
//Funzione di callback chiamata quando si salverà il file di configurazione 
void saveConfigCallback(){
// Callback notifying us of the need to save configuration
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
 
//Funzione di callback chiamata quando viene avviata la modalità di configurazione
void configModeCallback(WiFiManager *myWiFiManager){
// Called when config mode launched
  Serial.println("Entered Configuration Mode");
 
  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
 
  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}

String getCustomParamValue(WiFiManager *myWiFiManager, String name)
{
  String value;

  int numArgs = myWiFiManager->server->args();
  for (int i = 0; i < numArgs; i++) {
    Serial.println(myWiFiManager->server->arg(i));
  }
  if (myWiFiManager->server->hasArg(name))
  {
    value = myWiFiManager->server->arg(name);
  }
  return value;
}

void setup(){

  // Setup Serial monitor
  Serial.begin(9600);
  delay(10);

  dht.begin(); //inizilizziamo il sensore DHT22
  pinMode(TRIGGER_PIN, INPUT_PULLUP); //configurazione pin 4 come ingresso

  //Cambia in true durante il test per forzare la configurazione ogni volta che eseguiamo
  bool forceConfig = false;
 
  bool spiffsSetup = loadConfigFile();
  if (!spiffsSetup){
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }
 
  // Istruzione per impostare la modalità Station dell'ESP
  WiFi.mode(WIFI_STA);

 
  // Istruzione che cancella la configurazione di rete corrente ogni volta che ESP32 viene avviato.(istruzione utile in fase di sviluppo)
  //wm.resetSettings();
 
  // Chiamata alla funzione di callback per il salvataggio dei file di configurazione
  wm.setSaveConfigCallback(saveConfigCallback);
 
  // Chiamata alla funzione di callback quando la connessione al WiFi precedente fallisce ed entra in modalità Access Point
  wm.setAPCallback(configModeCallback);
 
  // Custom elements
 
  // Istruzioni per dichiarare campi di testo personalizzati
  char convertedValue1[6];
  WiFiManagerParameter campo_PIN_26("key_text", "Configura Pin 26", testString26, 50);
  //ComboBox1
  const char *valore_selezionato_str1 = R"(
  <br/><label for='valore1'>Input/Output PIN 26</label>
  <select name="comboBox1" id="valore1" onchange="document.getElementById('key_custom1').value = this.value">
    <option value="0">OUTPUT</option>
    <option value="1">INPUT</option>
  </select>
  <script>
    document.getElementById('valore1').value = "%d";
    document.querySelector("[for='key_custom1']").hidden = true;
    document.getElementById('key_custom1').hidden = true;
  </script>
  )";

  char bufferStr1[700];

  sprintf(bufferStr1, valore_selezionato_str1, valore1);

  Serial.print(bufferStr1);

  WiFiManagerParameter custom_field1(bufferStr1);

  sprintf(convertedValue1, "%d", valore1);

  WiFiManagerParameter custom_hidden1("key_custom1", "Will be hidden", convertedValue1, 2); //istruzione che serve per slavare il valore selezionato dalla comboBOx ma che verrà nascosto

//---------------------------------------------------------------------------------------------------------------------------------------------
  char convertedValue2[6];
  WiFiManagerParameter campo_PIN_25("key_text2", "Configura Pin 25", testString25, 50);
  //ComboBox2
  const char *valore_selezionato_str2 = R"(
  <br/><label for='valore2'>Input/Output PIN 25</label>
  <select name="comboBox2" id="valore2" onchange="document.getElementById('key_custom2').value = this.value">
    <option value="0">OUTPUT</option>
    <option value="1">INPUT</option>
  </select>
  <script>
    document.getElementById('valore2').value = "%d";
    document.querySelector("[for='key_custom2']").hidden = true;
    document.getElementById('key_custom2').hidden = true;
  </script>
  )";

  char bufferStr2[700];

  sprintf(bufferStr2, valore_selezionato_str2, valore2);

  Serial.print(bufferStr2);

  WiFiManagerParameter custom_field2(bufferStr2);

  sprintf(convertedValue2, "%d", valore2);

  WiFiManagerParameter custom_hidden2("key_custom2", "Will be hidden", convertedValue2, 2); //istruzione che serve per slavare il valore selezionato dalla comboBOx ma che verrà nascosto


//---------------------------------------------------------------------------------------------------------------------------------------------
  char convertedValue3[6];
  WiFiManagerParameter campo_PIN_32("key_text3", "Configura Pin 32", testString32, 50);
  //ComboBox3
  const char *valore_selezionato_str3 = R"(
  <br/><label for='valore3'>Input/Output PIN 32</label>
  <select name="comboBox3" id="valore3" onchange="document.getElementById('key_custom3').value = this.value">
    <option value="0">OUTPUT</option>
    <option value="1">INPUT</option>
  </select>
  <script>
    document.getElementById('valore3').value = "%d";
    document.querySelector("[for='key_custom3']").hidden = true;
    document.getElementById('key_custom3').hidden = true;
  </script>
  )";

  char bufferStr3[700];

  sprintf(bufferStr3, valore_selezionato_str3, valore3);

  Serial.print(bufferStr3);

  WiFiManagerParameter custom_field3(bufferStr3);

  sprintf(convertedValue3, "%d", valore3);

  WiFiManagerParameter custom_hidden3("key_custom3", "Will be hidden", convertedValue3, 2); //istruzione che serve per slavare il valore selezionato dalla comboBOx ma che verrà nascosto

//---------------------------------------------------------------------------------------------------------------------------------------------
  char convertedValue4[6];
  WiFiManagerParameter campo_PIN_19("key_text4", "Configura Pin 19", testString19, 50);
  //ComboBox4
  const char *valore_selezionato_str4 = R"(
  <br/><label for='valore4'>Input/Output PIN 19</label>
  <select name="comboBox4" id="valore4" onchange="document.getElementById('key_custom4').value = this.value">
    <option value="0">OUTPUT</option>
    <option value="1">INPUT</option>
  </select>
  <script>
    document.getElementById('valore4').value = "%d";
    document.querySelector("[for='key_custom4']").hidden = true;
    document.getElementById('key_custom4').hidden = true;
  </script>
  )";

  char bufferStr4[700];

  sprintf(bufferStr4, valore_selezionato_str4, valore4);

  Serial.print(bufferStr4);

  WiFiManagerParameter custom_field4(bufferStr4);

  sprintf(convertedValue4, "%d", valore4);

  WiFiManagerParameter custom_hidden4("key_custom4", "Will be hidden", convertedValue4, 2); //istruzione che serve per slavare il valore selezionato dalla comboBOx ma che verrà nascosto

//---------------------------------------------------------------------------------------------------------------------------------------------
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_topic("topic", "mqtt topic", mqtt_topic, 40);

  
  
  // Istruzioni per aggiungere i campi di testo personalizzati nella pagina web
  wm.addParameter(&custom_hidden1);
  wm.addParameter(&custom_field1);
  wm.addParameter(&campo_PIN_26);

  wm.addParameter(&custom_hidden2);
  wm.addParameter(&custom_field2);
  wm.addParameter(&campo_PIN_25);

  wm.addParameter(&custom_hidden3);
  wm.addParameter(&custom_field3);
  wm.addParameter(&campo_PIN_32);

  wm.addParameter(&custom_hidden4);
  wm.addParameter(&custom_field4);
  wm.addParameter(&campo_PIN_19);

  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_topic);


 
  if (forceConfig){

    // istruzione per l'avvio forzato del WiFiManager
    if (!wm.startConfigPortal("ESP32 AP", "password")){
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else{

    //istruzione per l'avvio di WiFiManager
    if (!wm.autoConnect("ESP32 AP", "password")){
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }
 

  //istruzioni per mostrare sul serial monitor l'indirizzo IP 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Istruzioni che stampano sul serial monitor il contenuto dei campi personalizzati
  strncpy(testString26, campo_PIN_26.getValue(), sizeof(testString26));
  Serial.print("testString26: ");
  Serial.println(testString26);

  strncpy(testString25, campo_PIN_25.getValue(), sizeof(testString25));
  Serial.print("testString25: ");
  Serial.println(testString25);

  strncpy(testString32, campo_PIN_32.getValue(), sizeof(testString32));
  Serial.print("testString32: ");
  Serial.println(testString32);

  strncpy(testString19, campo_PIN_19.getValue(), sizeof(testString19));
  Serial.print("testString19: ");
  Serial.println(testString19);

  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tmqtt_port : " + String(mqtt_port));
  Serial.println("\tmqtt_topic : " + String(mqtt_topic));

  valore1 = atoi(custom_hidden1.getValue());
  Serial.print("Valore1 Selezionato: ");
  Serial.println(comboBox1[valore1]);

  valore2 = atoi(custom_hidden2.getValue());
  Serial.print("Valore2 Selezionato: ");
  Serial.println(comboBox2[valore2]);

  valore3 = atoi(custom_hidden3.getValue());
  Serial.print("Valore3 Selezionato: ");
  Serial.println(comboBox3[valore3]);

  valore4 = atoi(custom_hidden4.getValue());
  Serial.print("Valore4 Selezionato: ");
  Serial.println(comboBox4[valore4]);

  if(valore1 == 0){
    pinMode(PIN_26, OUTPUT); //configurazione pin 26 come uscita
  } else if(valore1 == 1){
    pinMode(PIN_26, INPUT); //configurazione pin 26 come ingresso
  }

  if(valore2 == 0){
    pinMode(PIN_25, OUTPUT); //configurazione pin 25 come uscita
  } else if(valore2 == 1){
    pinMode(PIN_25, INPUT); //configurazione pin 25 come ingresso
  }
  
  if(valore3 == 0){
    pinMode(PIN_32, OUTPUT); //configurazione pin 32 come uscita
  } else if(valore3 == 1){
    pinMode(PIN_32, INPUT); //configurazione pin 32 come ingresso
  }
  


  //Salvataggio dei parametri personalizzati nel file system
  if (shouldSaveConfig){
    saveConfigFile();
  }

// istruzioni per il broker MQTT
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(callback);
  
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());
  if(client.connect(mqtt_topic)){
  Serial.println("MQTT connesso");
  client.subscribe(mqtt_topic); //topic

  } else {
    Serial.println(" ");
    Serial.println("MQTT non connesso");
    Serial.print("failed, rc=");
    Serial.println(client.state());

  }

}
 
 
void loop() {

//Inizio istruzione per richiamare WiFiManager con pulsante
//Se il pulsante TRIGGER_PIN viene premuto viene eseguita l'istruzione resetSettings()
  if (digitalRead(TRIGGER_PIN) == LOW) {
    WiFiManager wfm;    

    //resetta credenziali di rete
    wfm.resetSettings();

    //per risalire alla pagina web di configurazione della WiFi bisogna premere il pulsante Down
    //successivamente premere il pulsante RSET dell'ESP32, collegarsi alla rete WiFi dell'ESP in modalità access Point (ESP AP)
    //fornire le credenziali di rete, attendere che il modulo si connetti alla rere WiFi specificata
    //infine premere nuovamente il pulsante RESET per permettere ad Alexa di collegarsi al microcontrollore 
  
  }
// Fine istruzione per richiamare WiFiManager con pulsante

  client.loop();

}


//metodo richiamato per il broker MQTT
void callback(char* topic, byte* payload, unsigned int length){
  String msg;
  for (int i = 0; i < length; i++){
    msg += (char)payload[i];

  }

  if(strcmp(topic, mqtt_topic) == 0){
    Serial.print("*****topic:");
    Serial.println(topic);
    Serial.print("*****msg:");
    Serial.println(msg);
    String Stringa1 = "/on";
    String Stringa2 = "/off";
    int intero1 = 0;
    int intero2 = 1;

    if(valore1 == intero1){
      if(msg == testString26 + Stringa1){
        digitalWrite(PIN_26, HIGH);
        Serial.println("PIN_26 ON");
      } 
      if(msg == testString26 + Stringa2) {
        digitalWrite(PIN_26, LOW);
        Serial.println("PIN_26 OFF");
      }
    } 
    /*
    if(msg == testString26){
      if(valore1 == intero2){
        Serial.println("SONO QUI");
        client.publish(mqtt_topic, "risultato sensore PIN 26");
      }
    }
    */

    if(valore2 == intero1){
      if(msg == testString25 + Stringa1){
        digitalWrite(PIN_25, HIGH);
        Serial.println("PIN_25 ON");
      } 
      if(msg == testString25 + Stringa2) {
        digitalWrite(PIN_25, LOW);
        Serial.println("PIN_25 OFF");
      }
    }

    if(valore3 == intero1){
      if(msg == testString32 + Stringa1){
        digitalWrite(PIN_32, HIGH);
        Serial.println("PIN_32 ON");
      } 
      if(msg == testString32 + Stringa2) {
        digitalWrite(PIN_32, LOW);
        Serial.println("PIN_32 OFF");
      }
    }

    if(valore4 == intero2){
      if(msg == testString19){
        char temp[] = "temperatura: ";
        char humi[] = "umidità: ";
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        //È necessario convertire la variabile float della temperatura in un array di caratteri, in modo da poter pubblicare la lettura della temperatura nell'argomento /temperature :

        char tempString[8];
        dtostrf(t, 1, 2, tempString);

        int lunghezza_totale = strlen(temp) + strlen(tempString) + 1; // +1 per il carattere terminatore '\0'

        // Dichiarazione di una variabile char per la stringa risultante
        char stringa_risultante[lunghezza_totale];

        // Concatena le due stringhe
        strcpy(stringa_risultante, temp);
        strcat(stringa_risultante, tempString);

        Serial.print("temperatura: ");
        Serial.println(t);
        client.publish(mqtt_topic, stringa_risultante);

        //È necessario convertire la variabile float dell'umidità in un array di caratteri, in modo da poter pubblicare la lettura dell'umidità nell'argomento /umidita :

        char humString[8];
        dtostrf(h, 1, 2, humString);

        int lunghezza_totale2 = strlen(humi) + strlen(humString) + 1; // +1 per il carattere terminatore '\0'

        // Dichiarazione di una variabile char per la stringa risultante
        char stringa_risultante2[lunghezza_totale2];

        // Concatena le due stringhe
        strcpy(stringa_risultante2, humi);
        strcat(stringa_risultante2, humString);

        Serial.print("umidità: ");
        Serial.println(h);
        client.publish(mqtt_topic, stringa_risultante2);
      } 
    }
  }
}
