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

void NodeNetwork::printHexBuffer(const char *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (buffer[i] < 0x10) Serial.print("0"); // pad with 0
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
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
    String NodeMac=ETH.macAddress();
    NodeMac.replace(":","");
    NodeName = "KM07T"+NodeMac;
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
        // delay(500);
        vTaskDelay(pdMS_TO_TICKS(500));
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

    if(index_client<MAX_TELNET_CLIENTS)
    {
        if (!telnetClients[index_client] || !telnetClients[index_client].connected()) {
            if (telnetClients[index_client]) {
                telnetClients[index_client].stop();
                logMessage("❌ Client Disconnected");
            }
            if (telnetServer.hasClient()) {
                telnetClients[index_client] = telnetServer.available();
                logMessage("✅ New Client Connected");
            }
        }
        if (telnetClients[index_client] && telnetClients[index_client].available()) {
            queueClientData(index_client);
        }
        if(activeClient==-1)
            index_client++;
    }
    else
    {
        index_client=0;
    }
}

void NodeNetwork::queueClientData(int clientIndex) {
    if (activeClient != -1) {
        logMessage("⚠️ Another client is being processed, delaying Client " + String(clientIndex));
        return;
    }
    activeClient = clientIndex;  // Mark this client as active
    logMessage("📩 Received from Client" + String(clientIndex));
    memset(rxBuffer[clientIndex], 0, BUFFER_SIZE);
    int len = telnetClients[clientIndex].readBytes(rxBuffer[clientIndex], BUFFER_SIZE - 1);
    
    if (len > 0) {
        rxBuffer[clientIndex][len] = '\0';
        printHexBuffer(rxBuffer[clientIndex],len);
        // Send data to Serial1
        for (int i = 0; i < len; i++) {
            serial1->write(rxBuffer[clientIndex][i]);
        }

        waitForSerialResponse(clientIndex); // Wait for the serial response
    }

    while (telnetClients[clientIndex].available()) {
        telnetClients[clientIndex].read(); // discard extra bytes
    }
    activeClient = -1; // Reset active client after processing
}


void NodeNetwork::waitForSerialResponse(int clientIndex) {
    unsigned long startTime = millis();
    int totalLen = 0;
    
    while ((millis() - startTime) < 500) {  // 3-second timeout
        if (serial1->available()) {
            while (serial1->available()) {
                char c = serial1->read();
                txBuffer[clientIndex][totalLen++] = c;
                if (totalLen >= BUFFER_SIZE - 1) break; // Avoid buffer overflow
            }

            txBuffer[clientIndex][totalLen] = '\0';  // Null terminate the response
            sendClientResponse(clientIndex, totalLen);
            return;
        }
        // delay(5);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    // logMessage("⚠️ Serial Response Timeout for Client " + String(clientIndex));
    if(telnetClients[clientIndex].available())
        {
        totalLen=telnetClients[clientIndex].available();
        totalLen=telnetClients[clientIndex].readBytes(rxBuffer[clientIndex], BUFFER_SIZE - 1);
        rxBuffer[clientIndex][0]='\0';
        }
}


void NodeNetwork::sendClientResponse(int clientIndex,int len) {
    if((len>0)&&(telnetClients[clientIndex].connected())) 
    {
        printHexBuffer(txBuffer[clientIndex],len);
        telnetClients[clientIndex].write(txBuffer[clientIndex],len);
        memset(txBuffer[clientIndex], 0, BUFFER_SIZE);
    }
}

void NodeNetwork::taskTelnet(void *param)
{
    NodeNetwork *self = static_cast<NodeNetwork*>(param);
    while (true) {
        self->handleTelnetClients();
        vTaskDelay(pdMS_TO_TICKS(50)); // Allow parallel execution with main loop
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

void NodeNetwork::taskMDNS(void *param)
{
    NodeNetwork *self = static_cast<NodeNetwork*>(param);

    // Wait for Ethernet to connect before starting mDNS
    while (!ETH.linkUp()) {
        self->logMessage("⏳ Waiting for Ethernet link before starting mDNS...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    self->logMessage("🌐 Starting mDNS service...");

    if (MDNS.begin(self->NodeName.c_str())) {  // Change to your preferred hostname
        self->logMessage("✅ mDNS started: http://"+self->NodeName);

        // Optional: advertise telnet service
        MDNS.addService("Modbus", "tcp", TELNET_PORT);

        // You can also advertise other services, like HTTP or custom ones
        MDNS.addService("Discovery device", "udp", DISCOVERY_PORT);
        // MDNS.addService("modbus", "tcp", 502);
    } else {
        self->logMessage("❌ Failed to start mDNS responder");
        vTaskDelete(NULL); // Terminate the task if mDNS fails
    }

    // Keep the task alive (optional logging or future use)
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(60000)); // Just sleep, no MDNS.update() needed on ESP32
    }
}

void NodeNetwork::listenForDiscoveryTask(void* param) {
    char incomingPacket[255];
    NodeNetwork *self = static_cast<NodeNetwork*>(param);

    while (!ETH.linkUp()) {
        self->logMessage("⏳ Waiting for Ethernet link before starting Discovery Service port 8888...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    self->udp.begin(DISCOVERY_PORT);

    while (true) {
            int packetSize = self->udp.parsePacket();
            if (packetSize) {
                int len = self->udp.read(incomingPacket, sizeof(incomingPacket) - 1);
                if (len > 0) {
                    incomingPacket[len] = '\0';
                }

                Serial.print("Discovery request from: ");
                Serial.println(self->udp.remoteIP());

                if (strcmp(incomingPacket, "Primus") == 0) {
                    // Serial.println("Valid discovery request received. Sending response...");
                    self->logMessage("Valid discovery request received. Sending response...");
                    self->udp.beginPacket(self->udp.remoteIP(), self->udp.remotePort());
                    self->udp.println("KM-07T");
                    self->udp.println("📌 MAC Address: " + ETH.macAddress());
                    self->udp.println(self->useDHCP ? "🌍 IP Address (DHCP): " + ETH.localIP().toString() : "🌍 IP Address (Static): " + self->localIP.toString());
                    self->udp.println("🛤️  Gateway: " + self->gateway.toString());
                    self->udp.println("📡 Subnet: " + self->subnet.toString());
                    self->udp.println("mDNS:"+self->NodeName);
                    self->udp.endPacket();
                }
            }
        vTaskDelay(pdMS_TO_TICKS(100));  // Slight delay to yield to other tasks
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

    xTaskCreatePinnedToCore(
        taskMDNS,
        "MDNSTask",
        4096,
        this,
        1,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        listenForDiscoveryTask,
        "listenDiscoveryTask",
        4096,
        this,
        1,
        NULL,
        1
    );
}
