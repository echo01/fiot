#ifndef NODE_OP_H
#define NODE_OP_H

#include <ETH.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#define MAX_TELNET_CLIENTS 4
#define TELNET_PORT 23
#define BUFFER_SIZE 512

class NodeNetwork {
private:
    IPAddress localIP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns;
    bool useDHCP;
    bool serial_debug;
    bool serial_debug1;
    Stream *serial;
    Stream *serial1;

    WiFiServer telnetServer;
    WiFiClient telnetClients[MAX_TELNET_CLIENTS];
    int index_client;
    String ClientIP;
    String ClientPort;

    char rxBuffer[MAX_TELNET_CLIENTS][BUFFER_SIZE];
    char txBuffer[MAX_TELNET_CLIENTS][BUFFER_SIZE];
    
    enum ClientState { IDLE, WAIT_SERIAL, SEND_RESPONSE };
    ClientState clientState[MAX_TELNET_CLIENTS];
    unsigned long serialWaitStart[MAX_TELNET_CLIENTS];
    int responseLength[MAX_TELNET_CLIENTS];

    int activeClient;
    // unsigned long tick_handel;

    void logMessage(const String &message);
    void handleTelnetClients();
    void queueClientData(int clientIndex);
    void waitForSerialResponse(int clientIndex);
    void sendClientResponse(int clientIndex,int len);
    void startTelnetServer();

public:
    static void taskTelnet(void *param);
    static void taskNetwork(void *param);

    NodeNetwork(IPAddress ip = IPAddress(192, 168, 1, 100), 
                IPAddress gw = IPAddress(192, 168, 1, 254), 
                IPAddress sb = IPAddress(255, 255, 255, 0), 
                IPAddress dnsServer = IPAddress(8, 8, 8, 8), 
                bool dhcp = false);

    void setIP(IPAddress ip, IPAddress gw, IPAddress sb, IPAddress dnsServer, bool dhcpMode = false);
    void setSerial(Stream *serialPort, bool debug = true);
    void setSerial1(Stream *serialPort1, bool debug = false);
    void applyConfig();
    void printNetworkInfo();
    void startEthernet();
    void restartNetwork();
    void reconfigNetwork();
    void stopNetwork();
    String getNetworkStatus();
    void runTask();
};

#endif // NODE_OP_H


