#include <SPI.h>
#include <Ethernet.h>
#include <MFRC522.h>

MFRC522 rfid(9, 10);

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  rfid.PCD_Init();
  while (!Serial) {
  }
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server address: ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  String content= "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  EthernetClient client = server.available();
  if (client) {
    boolean currLB = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currLB) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 10");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>");
          client.println("Rfid Reader");
          client.println("</title>");
          client.println("/head");
          client.println("<body>");
          client.println("User Id:");
          client.println(content.substring(1));
          client.println("Detected");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currLB = true;
        } else if (c != '\r') {
          currLB = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}
