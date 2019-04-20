#include "pxt.h"
#include <algorithm>
#include <vector>
using namespace pxt;

#define MAX_PAYLOAD_LENGTH 19
#define PREFIX_LENGTH 11

#define MAX_HOP_COUNT 1
#define HEARTBEAT_FREQUENCY 1000
#define KEEP_TIME 7000
#define REBOUND_MAXWAIT 500
#define REBOUND_PROBABILITY 0.9

enum PacketType {
    HEARTBEAT = 7,
    UNICAST_STRING = 8,
    UNICAST_NUMBER = 9
};

// This is a workaround so TS knows which callback to use
// Would not be needed if we could use c++ callbacks, haven't figured out how
enum PayloadType {
    NONE = 0,
    STRING = 1,
    NUM = 2
};

struct Prefix {
    PacketType type;
    uint8_t messageId;
    uint32_t origAddress;
    uint32_t destAddress;
    uint8_t hopCount;
};

struct PartyMember {
    uint32_t address;
    uint32_t lastSeen;
    uint8_t lastMessageId;
};

struct Payload { String stringValue; int numValue; };

// Packet Spec

// | 0       | 1        | 2...5       | 6 ... 9     | 10       | 11... 28 |
// ------------------------------------------------------------------------
// | type   | messageID | origAddress | destAddress | hopCount | payload  |

namespace PartiesInternal {

    bool radioEnabled = false;

    uint8_t ownMessageId = 0;

    std::vector<PartyMember> partyTable;

    Payload lastPayload;
    PayloadType lastPayloadType = NONE;

    int radioEnable() {
        int r = uBit.radio.enable();
        if (r != MICROBIT_OK) {
            uBit.panic(43);
            return r;
        }
        if (!radioEnabled) {
            // TODO: Set the radio group based on which party we are joining
            uBit.radio.setGroup(1);//pxt::programHash());
            uBit.radio.setTransmitPower(6); // start with high power by default
            radioEnabled = true;
        }
        return r;
    }

    bool isOldEntry(PartyMember member){
        return (member.lastSeen + KEEP_TIME) < (system_timer_current_time());
    }

    /**
     * Filters out old entries in the table
     * Also call at HEARTBEAT_FREQUENCY from TS
     */
    //%
    void filterTable(){
        partyTable.erase(
            std::remove_if(partyTable.begin(), partyTable.end(), isOldEntry),
            partyTable.end()
        );
    }

    void sendRawPacket(Buffer data) {
        uBit.radio.datagram.send(data->data, data->length);
    }

    void rebound(Prefix prefix, uint8_t* buf) {
        // Limit the number of hops a packet can have
        if (prefix.hopCount >= MAX_HOP_COUNT) return;

        // Only rebound with a certain probability
        if (uBit.random(100) >= 100*REBOUND_PROBABILITY) return;

        // Wait for a small random amount of time before rebounding
        // This helps to avoid packet collisions with other microbits
        uBit.sleep(uBit.random(REBOUND_MAXWAIT));

        // Increment hop count by 1
        uint8_t hopCount = prefix.hopCount + 1;
        memcpy(buf+10, &hopCount, 1);

        // Send the correct number of bytes depending on the type of packet
        switch (prefix.type)
        {
            case PacketType::HEARTBEAT:
                uBit.radio.datagram.send(buf, PREFIX_LENGTH);
                break;

            case PacketType::UNICAST_STRING:
                // First byte of payload is string length
                uint8_t stringLen;
                memcpy(&stringLen, buf + PREFIX_LENGTH, 1);

                uBit.radio.datagram.send(buf, PREFIX_LENGTH + stringLen + 1);
                break;

            /*
            case PacketType::UNICAST_NUMBER:
                uBit.radio.datagram.send(buf, PREFIX_LENGTH + NUMBER_PAYLOAD_LENGTH);
                break;
            */
        }
    }

    struct hasAddress {
        hasAddress(uint32_t address) : address(address) {}
        int operator()(PartyMember partyMember) { return partyMember.address == address; }

        private:
            uint32_t address;
    };
    
    void receiveHeartbeat(Prefix prefix, uint8_t* buf) {
        // passing buf in anticipation of data transmitted on Heartbeat
        // c++ insanity. This just finds a party member in partyTable with the address prefix.origAddress
        std::vector<PartyMember>::iterator it = std::find_if (partyTable.begin(), partyTable.end(), hasAddress(prefix.origAddress));
        
        if(it == partyTable.end()){
            // originator was not in table
            PartyMember newMember;
            newMember.address = prefix.origAddress;
            newMember.lastSeen = system_timer_current_time();
            newMember.lastMessageId = prefix.messageId;
            partyTable.push_back(newMember);
            rebound(prefix, buf);
        } else if (prefix.messageId > it->lastMessageId){
            // we haven't seen this message before
            it->lastMessageId = prefix.messageId;
            it->lastSeen = system_timer_current_time();
            rebound(prefix, buf);
        }
    }

    Payload getStringPayload(uint8_t* buf, uint8_t maxLength) {
        // First byte is the string length
        uint8_t len = min_(maxLength, buf[0]);
        Payload payload;
        payload.stringValue = mkString((char*)buf + 1, len);
        return payload;
    }

    uint8_t copyStringValue(uint8_t* buf, String data, uint8_t maxLength) {
        uint8_t len = min_(maxLength, data->getUTF8Size());

        // One byte for length of the string
        buf[0] = len;

        if (len > 0) {
            memcpy(buf + 1, data->getUTF8Data(), len);
        }
        return len + 1;
    }

    /**
     * Read a packet from the queue of received packets and react accordingly
     */
    //%
    void receiveData() {
        PacketBuffer p = uBit.radio.datagram.recv();
        uint8_t* buf = p.getBytes();

        Prefix prefix;
        memcpy(&(prefix.type),        buf,    1);
        memcpy(&(prefix.messageId),   buf+1,  1);
        memcpy(&(prefix.origAddress), buf+2,  4);
        memcpy(&(prefix.destAddress), buf+6,  4);
        memcpy(&(prefix.hopCount),    buf+10, 1);

        if (prefix.origAddress == microbit_serial_number()) return;

        switch(prefix.type)
        {
            case PacketType::HEARTBEAT:
                receiveHeartbeat(prefix, buf);
                break;
            case PacketType::UNICAST_STRING:
                if(prefix.destAddress == microbit_serial_number()) {
                    lastPayload = getStringPayload(buf + PREFIX_LENGTH, MAX_PAYLOAD_LENGTH - 1);
                    lastPayloadType = PayloadType::STRING;
                }
                else rebound(prefix, buf);
                break;
                
            case PacketType::UNICAST_NUMBER:
                if(prefix.destAddress == microbit_serial_number()) {
                    Payload payload;
                    memcpy(&payload.numValue, buf+PREFIX_LENGTH, sizeof(int));
                    lastPayload = payload;
                    lastPayloadType = PayloadType::NUM;
                }
                else rebound(prefix, buf);
                break;
            default: break;
        }
    }

    void setPacketPrefix(uint8_t* buf, Prefix prefix) {
        memcpy(buf,     &(prefix.type), 1);
        memcpy(buf+1,   &(prefix.messageId), 1);
        memcpy(buf+2,   &(prefix.origAddress), 4);
        memcpy(buf+6,   &(prefix.destAddress), 4);
        memcpy(buf+10,  &(prefix.hopCount), 1);
    }

    /** 
     * To be called at Heartbeat Frequency
     */
    //%
    void sendHeartbeat(){
        ownMessageId++;

        uint8_t buf[PREFIX_LENGTH];
        Prefix prefix;
        prefix.type         = PacketType::HEARTBEAT;
        prefix.messageId    = ownMessageId;
        prefix.origAddress  = microbit_serial_number();
        prefix.destAddress  = 0;
        prefix.hopCount     = 1;

        setPacketPrefix(buf, prefix);
        uBit.radio.datagram.send(buf, PREFIX_LENGTH);
    }

    /**
     * Send a string to the micro:bit with the specified address
     */
    //%
    void sendString(String msg, uint32_t destAddress) {
        if (radioEnable() != MICROBIT_OK || NULL == msg) return;
        
        uint8_t buf[32];
        memset(buf, 0, 32);

        Prefix prefix;
        prefix.type = PacketType::UNICAST_STRING;
        prefix.messageId = ownMessageId;
        prefix.origAddress = microbit_serial_number();
        prefix.destAddress = destAddress;
        prefix.hopCount = 1;
        setPacketPrefix(buf, prefix);

        int stringLen = copyStringValue(buf + PREFIX_LENGTH, msg, MAX_PAYLOAD_LENGTH  - 1);
        
        uBit.radio.datagram.send(buf, PREFIX_LENGTH + stringLen);
    }
                         
    /**
      * Send a number to the micro:bit with the specified address
      */
    //%
    void sendNumber(int num,  uint32_t destAddress){
        if (radioEnable() != MICROBIT_OK) return;

        uint8_t buf[32];
        memset(buf, 0, 32);

        Prefix prefix;
        prefix.type = PacketType::UNICAST_NUMBER;
        prefix.messageId = ownMessageId;
        prefix.origAddress = microbit_serial_number();
        prefix.destAddress = destAddress;
        prefix.hopCount = 1;
        setPacketPrefix(buf, prefix);

        memcpy(buf + PREFIX_LENGTH, &num , sizeof(int));

        uBit.radio.datagram.send(buf, PREFIX_LENGTH + sizeof(int));
    }

    void resetPayload(){
        Payload empty;
        lastPayload = empty;
        lastPayloadType = PayloadType::NONE;
    }

    /**
     * Use this only to call receiveData from Typescript
     * (workaround, cannot figure out how to pass c++ function to registerWithDal)
     * Note: Only one function can be registered at once, so the radio module
     * will have to be disabled.
     */
    //%
    void onDataReceived(Action body) {
       if (radioEnable() != MICROBIT_OK) return;
       registerWithDal(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, body);
    }

    /**
     * Use as frequency to call sendHeartbeat
     */
    //%
    int getHeartbeatFrequency(){ return HEARTBEAT_FREQUENCY; }

    /**
     * Numer of Party Members
     */
    //%
    int numberOfPartyMembers() { return partyTable.size(); }

    /**
     * Random Party Member
     */
    //%
    uint32_t randomPartyMember() {
        if(partyTable.size() == 0)
            return -1;
        else return partyTable[uBit.random(partyTable.size())].address;
    }

    /**
     * For TS to check whether there is a new payload to react to
     */
    //%
    PayloadType receivedPayloadType() { return lastPayloadType; }

    /**
     * Get the received string
     */
    //%
    String receivedStringPayload() { 
        String message = lastPayload.stringValue;
        resetPayload();
        if (radioEnable() != MICROBIT_OK || NULL == message) return mkString("", 0);
        incrRC(message);
        return message;
    }
                         
    /**
     * Get the received number
     */
    //%
    int receivedNumberPayload () {
        int message = lastPayload.numValue;
        resetPayload();
        if (radioEnable() != MICROBIT_OK) return 0;
        return message;
    }
}
