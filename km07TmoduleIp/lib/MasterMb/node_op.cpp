#include "node_op.h"

// Constructor
NodeNetwork::NodeNetwork(IPAddress ip, IPAddress gw, IPAddress sb, IPAddress dnsServer, bool dhcp)
    : telnetServer(TELNET_PORT) {
    localIP = ip;
    gateway = gw;
    subnet = sb;
    dns = dnsServer;
    useDHCP = dhcp;
    serial_debug = true;
    serial = &Serial;
    serial1 = &Serial1;
    for (int i = 0; i < MAX_TELNET_CLIENTS; i++) {
        clientState[i] = IDLE;
        responseLength[i] = 0;
    }
    activeClient=-1;
}

void NodeNetwork::setSerial(Stream *serialPort, bool debug) {
    serial = serialPort;
    serial_debug = debug;
}

void NodeNetwork::setSerial1(Stream *serialPort, bool debug) {
    serial1 = serialPort;
    serial_debug1 = debug;
}

void NodeNetwork::logMessage(const String &message) {
    if (serial_debug && serial) {
        serial->println(message);
    }
}

void NodeNetwork::setIP(IPAddress ip, IPAddress gw, IPAddress sb, IPAddress dnsServer, bool dhcpMode) {
    localIP = ip;
    gateway = gw;
    subnet = sb;
    dns = dnsServer;
    useDHCP = dhcpMode;
}

void NodeNetwork::applyConfig() {
    if (useDHCP) {
        logMessage("📡 Using DHCP (Dynamic IP Mode)");
    } else {
        if (!ETH.config(localIP, gateway, subnet, dns)) {
            logMessage("❌ Failed to apply static IP configuration!");
            return;
        } else {
            logMessage("✅ Static IP Configuration Applied:");
        }
    }
    printNetworkInfo();
}

void NodeNetwork::printNetworkInfo() {
    logMessage("\n🌐 Network Details:");
    logMessage("📌 MAC Address: " + ETH.macAddress());
    logMessage(useDHCP ? "🌍 IP Address (DHCP): " + ETH.localIP().toString() : "🌍 IP Address (Static): " + localIP.toString());
    logMessage("🛤️  Gateway: " + gateway.toString());
    logMessage("📡 Subnet: " + subnet.toString());
    logMessage("🌍 DNS Server: " + dns.toString());
}

void NodeNetwork::startEthernet() {
    logMessage("🚀 Starting Ethernet...");
    if (!ETH.begin()) {
        logMessage("❌ Ethernet initialization failed!");
        return;
    }
    logMessage("✅ Ethernet Started Successfully!");

    while (!ETH.linkUp()) {
        logMessage(".");
        delay(500);
    }
    logMessage("\n🔗 Ethernet Connected!");

    applyConfig();
    startTelnetServer();
}

void NodeNetwork::stopNetwork() {
    logMessage("🛑 Stopping Ethernet...");
    logMessage("⚠️ Ethernet Stopped (ETH.end() not available).");
}

// Restart Ethernet (Directly restarts)
void NodeNetwork::restartNetwork() {
    logMessage("🔄 Restarting Ethernet...");
    startEthernet(); // Directly restart
}

// Reconfigure and restart Ethernet
void NodeNetwork::reconfigNetwork() {
    logMessage("⚙️ Reconfiguring Network...");
    applyConfig();
    startEthernet();
}

String NodeNetwork::getNetworkStatus() {
    if (!ETH.linkUp()) {
        return "🔴 Disconnected";
    }
    return useDHCP ? "🟢 Connected (DHCP)" : "🟢 Connected (Static IP)";
}

void NodeNetwork::startTelnetServer() {
    logMessage("🚀 Starting Telnet Server...");
    telnetServer.begin();
    telnetServer.setNoDelay(true);
}


void NodeNetwork::handleTelnetClients() {
    for (int i = 0; i < MAX_TELNET_CLIENTS; i++) {
        if (!telnetClients[i] || !telnetClients[i].connected()) {
            if (telnetClients[i]) {
                telnetClients[i].stop();
                logMessage("❌ Client Disconnected");
            }
            if (telnetServer.hasClient()) {
                telnetClients[i] = telnetServer.available();
                logMessage("✅ New Client Connected");
            }
        }
        if (telnetClients[i] && telnetClients[i].available()) {
            queueClientData(i);
        }
    }
}

void NodeNetwork::queueClientData(int clientIndex) {
    int len = telnetClients[clientIndex].readBytes(rxBuffer[clientIndex], BUFFER_SIZE - 1);
    if (len > 0) {
        rxBuffer[clientIndex][len] = '\0';
        logMessage("📩 Received from Client"+String(clientIndex)+": " + String(rxBuffer[clientIndex])+"size :"+String(len));
        // serial1->println(rxBuffer[clientIndex]); // Send to Serial
        uint8_t mb_length = len;
         int count_message = 0;
        for (count_message = 0; count_message < mb_length; count_message++)
          {
            serial1->write(rxBuffer[clientIndex][count_message]);
          }
        waitForSerialResponse(clientIndex);
    }
}

void NodeNetwork::waitForSerialResponse(int clientIndex) {
    unsigned long startTime = millis();
    int len_avaiable;
    while ((millis() - startTime) < 3000) { // 3-second timeout
        if (serial1->available()) {
            int len = serial1->available();
            serial1->readBytes(txBuffer[clientIndex], len);
            if (len > 0) {
                txBuffer[clientIndex][len] = '\0';
                logMessage("📤 Received from Serial: " + String(txBuffer[clientIndex]));
                sendClientResponse(clientIndex,len);
                return;
            }
            return;
        }
    }
    logMessage("⚠️ Serial Response Timeout for Client " + String(clientIndex));
}

void NodeNetwork::sendClientResponse(int clientIndex,int len) {
    if((len>0)&&(telnetClients[clientIndex].connected())) 
    {
        telnetClients[clientIndex].write(txBuffer[clientIndex],len);
        logMessage("📨 Sent to Client"+String(clientIndex)+": " + String(txBuffer[clientIndex])+"size :"+String(len));
        memset(txBuffer[clientIndex], 0, BUFFER_SIZE);
    }
}

void NodeNetwork::taskTelnet(void *param)
{
    NodeNetwork *self = static_cast<NodeNetwork*>(param);
    while (true) {
        self->handleTelnetClients();
        vTaskDelay(pdMS_TO_TICKS(20)); // Allow parallel execution with main loop
    }
}

void NodeNetwork::taskNetwork(void *param)
{
    NodeNetwork *self = static_cast<NodeNetwork*>(param);
    self->startEthernet();

    while (true) {
        self->logMessage("📢 Network Status: " + self->getNetworkStatus());

        if (!ETH.linkUp()) {
            self->logMessage("⚠️ Ethernet Disconnected! Retrying...");
            self->restartNetwork();
        }
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}

void NodeNetwork::runTask() {
    logMessage("Modbus over tcp telnet v1.00");
    logMessage("Max Client support :"+String(MAX_TELNET_CLIENTS));
    logMessage("Modbus Serial Port : UART1");
    logMessage("log message : UART0");
    
    xTaskCreatePinnedToCore(
        taskTelnet,
        "TelnetTask",
        // "handleTelnetClients_ex",
        4096,
        this,
        1,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        taskNetwork,
        "NetworkMonitorTask",
        4096,
        this,
        1,
        NULL,
        1
    );
}
