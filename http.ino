#include <WiFi.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;
WiFiServer server(80);

String rootHTML = ""
  "<!DOCTYPE html><html><head><title>HTTP Captive Portal</title></head><body><h1>HTTP</h1>"
  "<p><a href=\"/link.pdf\" download>Download File</a></p>"
  "<form action=\"/form.pdf\"><label for=\"fname\">Name:</label><br><input type=\"text\" id=\"fname\" name=\"fname\" value=\"doc.pdf\"><br><input type=\"submit\" value=\"Submit\"></form>"
  "</body></html>";

String sampleFile = "%PDF-1.7 @@ Not a real PDF @@";

String getLink = "GET /link.pdf";
String getForm = "GET /form.pdf";
String getConnectivity1 = "GET /generate_204";
String getConnectivity2 = "GET /gen_204";
String getFavicon = "GET /favicon.ico";

String request;

void setup() { 
  Serial.begin(115200);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("http","http1234");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();

  Serial.println("Captive Portal Started!");
}

void loop() {
  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            //We've recieved a blank line assume end of request
            if (request.indexOf(getConnectivity1) >= 0  || request.indexOf(getConnectivity2) >= 0){
              Serial.println("Conectivity Request");
              client.println("HTTP/1.1 204 OK");
              client.println("Content-type:text/html");
              client.println();
              break;
            } else if (request.indexOf(getFavicon) >= 0) {
              Serial.println("Favicon Request");
              client.println("HTTP/1.1 404 OK");
              client.println("Content-type:text/html");
              client.println();
              break;
            } else if (request.indexOf(getLink) >= 0) {
              Serial.println("Link Request");
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:application/pdf");
              client.println();
              client.print(sampleFile);
              break;
            } else if(request.indexOf(getForm) >= 0) {
              Serial.println("Form Request");
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:application/pdf");
              client.println();
              client.print(sampleFile);
              break;
            } else{
              //Send rootPage
              Serial.println("root Request");
              Serial.println(request);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              client.print(rootHTML);
              break;
            }
            
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    request = "";
    
    client.stop();

    Serial.println("Client Disconect");
  }
}
