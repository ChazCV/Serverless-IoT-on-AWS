/* ESP32 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP32 with AWS IoT.
 *  
 * Author: Anthony Elder 
 * Modified by Stephen Borsay for Udemy
 * License: Apache License v2
 */
#include <WiFiClientSecure.h>
#include <PubSubClient.h> // install with Library Manager, I used v2.6.0

const char* ssid = "TE-DB";
const char* password = "A9yb&3793RR$";

const char* awsEndpoint = "a1ypprxhwj7wh3-ats.iot.us-east-1.amazonaws.com"; //found in 'Settings' in AWS IoT Core 

// Update the two certificate strings below. Paste in the text of your AWS 
// device certificate and private key. Add a quote character at the start
// of each line and a backslash, n, quote, space, backslash at the end 
// of each line:

// xxxxxxxxxx-certificate.pem.crt
const char* certificate_pem_crt = \

"-----BEGIN CERTIFICATE-----\n" \
"XXXXXXXXXXXXXXXBAgIVAI7n0dti6sMuimCVANnjs+8i8TUiMA0GCSqGSIb3DQEB\n" \
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTExMDYxODI1\n" \
"NDFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDRSWA/2xSg/OYOOM6d\n" \
"2smJIEn3VIgmqCEmrg1+6vdoKBLxgMh194z2Ns83siRG9GPf7+v5oWk0Bu9kDKNl\n" \
"vquAnBO3+eW1Sgg3JE9SP5utExYGDXud1im6dlG/YbnN8gWCG3W0Abx1vfsiJMP7\n" \
"yhez8Xp4lb+/fSDH/vbi5IqYLsCsUgSB7hrg0a8zqXa16lq5FjTgUk9CxFDb5V+z\n" \
"ipXLfcXboisLIAjJMhNXnef+CpM4rQJulf1eZxCV3P9Du8eFpGKx1VFm8/D1pwrh\n" \
"GJ1N9kFuSWfQHhj+gA383OL7andGE9h2097O4KaqXW9coCZVKv3AjX2WWqczL3uV\n" \
"6AOdAgMBAAGjYDBeMB8GA1UdIwQYMBaAFOwXufzDBIb6LGYzX9hsuuTkGBggMB0G\n" \
"A1UdDgQWBBQRfgU2R8TDTFz6lgUra5M/9pFn6jAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAB4tmHWDNBu74BDIn+f32c1ED\n" \
"r+HJjZAVWBwG9v7ubFu/uA+TaHYT+KBaAfB4NfAlOmTpMZN6egLkKfRNNDIA+tAH\n" \
"PV/QBDZWjCR3YS5sAUIrnSSRzFGRtAFtscYJhJ73Ahhob8H6zXyH2XQRft4663dI\n" \
"fqMlXsofXS7QH/mhy1zv13su8EaS/UNJRMvB/+ESyEwkQ2BhzpE8TjaBXTkyZuKw\n" \
"wSBoHO71UqJGhmLkDLp99hKc90KWh08v9jmNBfbTZvIUcCsIz6EhcbfUAg8oBSE0\n" \
"yuzWG1P4ZwUk70SDcHhDty8GZ9oajrCsLTcv7zrw4aUtfuIFOqXLybqVOCaGOw==\n" \ 
"-----END CERTIFICATE-----\n";

// xxxxxxxxxx-private.pem.key
const char* private_pem_key = \

"-----BEGIN RSA PRIVATE KEY-----\n" \
"XXXXXXXXXXXXXXXA0UlgP9sUoPzmDjjOndrJiSBJ91SIJqghJq4Nfur3aCgS8YDI\n" \
"dfeM9jbPN7IkRvRj3+/r+aFpNAbvZAyjZb6rgJwTt/nltUoINyRPUj+brRMWBg17\n" \
"ndYpunZRv2G5zfIFght1tAG8db37IiTD+8oXs/F6eJW/v30gx/724uSKmC7ArFIE\n" \
"ge4a4NGvM6l2tepauRY04FJPQsRQ2+Vfs4qVy33F26IrCyAIyTITV53n/gqTOK0C\n" \
"bpX9XmcQldz/Q7vHhaRisdVRZvPw9acK4RidTfZBbkln0B4Y/oAN/Nzi+2p3RhPY\n" \
"dtPezuCmql1vXKAmVSr9wI19llqnMy97legDnQIDAQABAoIBAAKo/RkyrqtxK3do\n" \
"z2+ANWmRyH7lSym6n1k/gxpm4CgpwjvhmCqvr9H9Vlrt37orJw3Undo5a/3mTKqn\n" \
"4nfLmaBz22hOO9Y3D62rv3pbJp5njLMc/sI905/0GwvksRjmB53kvXVBRGT2ujdv\n" \
"4jHUGsMFwbnApYRIkd8Y5YhywTQ39gQeC30suTikIlqSbXnT5KJkKcKvsfnAbsx0\n" \
"bp+7grEYZjkmki7f7KJgJpVWOQBg8JhbRHBFMiIsMQORPDI+NfaHtgUKxBKykCju\n" \
"gkkf7ljslfMrfLJvB5gGJetoc/ilLS+19O6VZVN3bChpL4OKE+WnsOdjMLPzmfWL\n" \
"fIoYiAECgYEA9TpqOwUbWuFjAXIU+lTjn/jJCmnkvX3S0tJ9AXxkZBzfN0BlDlMC\n" \
"XkEfaARLqj05YbvwfikmnXbER9hE3TjHh8uq+Pv8ZzK432X9/kTLuNKksan3sDgP\n" \
"e94sQmKrNm5cTw2K9fMsk04W9UdkQGG2duZvOINW8iPqtfNy0jj6rZ0CgYEA2nrM\n" \
"jmoC4MTuhtEDaishocVH5Q3MVCrZWh1QELgu+d7XUFuvpByT1Dw68PAdlcUjQa8t\n" \
"+1jopKHGEaZ3781dSkFL6fCcpOn4bGBvNKzURt6avj9r4WoK6tzLqymZbr+32eNc\n" \
"vXgui3fz6V7fZDtE03ixw62qEgLJF+08yhFNzgECgYB7WW+30kDJPNe1EXI13N9G\n" \
"RziwsUUqf5C9FL1mMvC4XsF0pEJwqxZk3LL0ejypG/SyEXvNqdtPlz7xuHojIH1U\n" \
"9ABDD1UNf7j4PfA9ptMmW2YWK514GSrIrp9qoQDn9ykdZn2Aa1n/mmb353oo3D2Q\n" \
"nyZQsdfZInHcJeGalqiZDQKBgQDZ6N0ZtlbEhOc7hEEZpYdX6IL6zLZdxJchMFdp\n" \
"Nat1AXRT6/7VesNfTeuj4HpBpWyy2NzN8zGm8suxUw4RGg3QQCxNWvWB7vMedVi1\n" \
"eyQGw4Qn+O3K2I+nDS3+u/ES6xmesw0O1U3nQW7/9uZs/Eh7e8gh2Zble+3CdZbK\n" \
"HFQ0AQKBgQCQWrO4PaQQIZ/ZPi9/lKZLRHAb30gx1Qpgpv2pbpxVyIKRfOdDvpHg\n" \
"Fp+icoIMvHcPoJPz13QYujADzmKpLviKYXDDojJ6sCrPmieV0wAEsF19zbOkMZgr\n" \
"guGf0uCCv7CpEoRvaVUAX12/Kp9iA03b5hc3hTqBTMSKirdRlouugA==\n" \
"-----END RSA PRIVATE KEY-----\n";

/* root CA can be downloaded in:
  https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
*/
const char* rootCA = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";


WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); delay(50); Serial.println();
  Serial.println("ESP32 AWS GPS Weather Example by SDB for Udemy");
  Serial.printf("SDK version: %s\n", ESP.getSdkVersion());

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  //If you need to increase buffer size, then you need to change MQTT_MAX_PACKET_SIZE in PubSubClient.h
  char fakeData[128];
  int t =  random(30,110); //fake number range, adjust as you like
  int h =  random(40,100);

  int cityNumber = random(0,4); //range is 0- total # cities, range gets rounded down by random function so result is always n-1
  float latt;
  float lon;
// Moab, Tuscon, Chicago, Stevenson
  float gps [][2] = {{38.5743966, -109.5689282},{32.1558328, -111.0238918}, {41.881832, -87.623177},{45.6944496, -121.9115935} };
  latt = gps[cityNumber][0];
  lon =  gps[cityNumber][1];

//Don't overflow your buffer! use short names and data types, the MQTT library has package size limitations per cycle
  snprintf(fakeData, sizeof(fakeData),"{\"uptime\":%lu,\"intemp\":%d,\"inhumid\":%d,\"lat\":%2.7f,\"long\":%3.7f}", millis() / 1000, t, h, latt, lon );


  if (millis() - lastPublish > 10000) {
    boolean rc = pubSubClient.publish("outTopic", fakeData);
    Serial.print("Published, rc="); Serial.print( (rc ? "OK: " : "FAILED: ") );
    Serial.println(fakeData);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthingXXXX");
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
