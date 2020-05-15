#include <EtherCard.h>

//host

const char web[] PROGMEM = "unoweather.azurewebsites.net";

static byte mac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

//buffer

byte Ethernet::buffer[900];
Stash stash;
static byte session;

void setup () {
  Serial.begin(9600);
  Serial.println("\n Start connection !");

//ethernet status

  if (ether.begin(sizeof Ethernet::buffer, mac) == 0)
    Serial.println(F("Connection to mudule, failed..."));
  if (!ether.dhcpSetup())
    Serial.println(F("Wrong module configuration..."));

  ether.printIp("ARDUINO:  ", ether.myip);
  ether.printIp("ROUTER:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  if (!ether.dnsLookup(web))
    Serial.println(F("Connection failed :( "));
  ether.printIp("SRV: ", ether.hisip);

  dataSend();
}

//main

void loop () {

  ether.packetLoop(ether.packetReceive());

  const char* status = ether.tcpReply(session);
  if (status != 0) {
    Serial.println("Server response !");
    Serial.println(status);
  }
}

//data send

static void dataSend () {
  byte send = stash.create();
  
  stash.print("{\"Temp\":15}");
  
  stash.save();
  int stash_size = stash.size();

   Stash::prepare(PSTR("POST /api/data HTTP/1.1" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "Content-Type: application/json" "\r\n"
                      "\r\n"
                      "$H"),
                 web, stash_size, send);
                 
  session = ether.tcpSend();
}

