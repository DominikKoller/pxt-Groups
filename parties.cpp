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
    UNICAST_NUMBER = 9,
    BROADCAST_STRING = 10,
    BROADCAST_NUMBER = 11
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

namespace parties {

    bool radioEnabled = false;

    uint8_t ownMessageId = 0;

    std::vector<PartyMember> partyTable;

    Payload lastPayload;
    PayloadType lastPayloadType = NONE;

    std::optional<std::string> status{};

    void setStatus(string s) = {status = s;}
    

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
            case PacketType::BROADCAST_STRING:
                // First byte of payload is string length
                uint8_t stringLen;
                memcpy(&stringLen, buf + PREFIX_LENGTH, 1);

                uBit.radio.datagram.send(buf, PREFIX_LENGTH + stringLen + 1);
                break;

            case PacketType::UNICAST_NUMBER:
            case PacketType::BROADCAST_NUMBER:
                uBit.radio.datagram.send(buf, PREFIX_LENGTH + sizeof(int));
                break;

            default:
                break;
        }
    }

    struct hasAddress {
        hasAddress(uint32_t address) : address(address) {}
        int operator()(PartyMember partyMember) { return partyMember.address == address; }

        private:
            uint32_t address;
    };

    void addNewPartyMember(Prefix prefix)
    {
        PartyMember newMember;
        newMember.address = prefix.origAddress;
        newMember.lastSeen = system_timer_current_time();
        newMember.lastMessageId = prefix.messageId;
        partyTable.push_back(newMember);
    }

    /** 
     * Checks whether this message has been seen before, and if not, updates the party table.
     */
    bool messageSeenBefore(Prefix prefix) {
        std::vector<PartyMember>::iterator sender = 
            std::find_if (partyTable.begin(), partyTable.end(), hasAddress(prefix.origAddress));

        if (sender == partyTable.end()) {
            // Sender not recognised, so create a new entry for them in partyTable.
            addNewPartyMember(prefix);
            return false;
        }
        else if (prefix.messageId > sender->lastMessageId) {
            // We haven't seen this message before, so update partyTable.
            sender->lastMessageId = prefix.messageId;
            sender->lastSeen = system_timer_current_time();
            return false;
        }
        return true;
    }

    Payload getStringPayload(uint8_t* buf, uint8_t maxLength) {
        // First byte is the string length
        uint8_t len = min_(maxLength, buf[0]);
        Payload payload;
        payload.stringValue = mkString((char*)buf + 1, len);
        return payload;
    }

    void receiveString(uint8_t* buf) {
        lastPayload = getStringPayload(buf + PREFIX_LENGTH, MAX_PAYLOAD_LENGTH - 1);
        lastPayloadType = PayloadType::STRING;
    }

    void receiveNumber(uint8_t* buf) {
        Payload payload;
        memcpy(&payload.numValue, buf+PREFIX_LENGTH, sizeof(int));
        lastPayload = payload;
        lastPayloadType = PayloadType::NUM;
    }    

     //was previously commented out to find weirdo bug in online editor...bug still not found

     uint8_t copyStringValue(uint8_t* buf, String data, uint8_t maxLength) {
         uint8_t len = min_(maxLength, data->);

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

        // Only handle the packet if it's not been seen (and thus handled) already.
        if (!messageSeenBefore(prefix))
        {
            switch(prefix.type)
            {
                case PacketType::HEARTBEAT:
                    rebound(prefix, buf);
                    break;

                case PacketType::UNICAST_STRING:
                    if (prefix.destAddress == microbit_serial_number()) {
                        receiveString(buf);
                    }
                    else rebound(prefix, buf);    
                    break;
                
                case PacketType::UNICAST_NUMBER:
                    if (prefix.destAddress == microbit_serial_number()) {
                        receiveNumber(buf);
                    }
                    else rebound(prefix, buf);
                    break;

                case PacketType::BROADCAST_STRING:
                    receiveString(buf);
                    rebound(prefix, buf);
                    break;

                case PacketType::BROADCAST_NUMBER:
                    receiveNumber(buf);
                    rebound(prefix, buf);
                    break;

                default: 
                    break;
            }
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
        if (radioEnable() != MICROBIT_OK) return;
        ownMessageId++;

        uint8_t buf[PREFIX_LENGTH+MAX_PAYLOAD_LENGTH];
        Prefix prefix;
        prefix.type         = PacketType::HEARTBEAT;
        prefix.messageId    = ownMessageId;
        prefix.origAddress  = microbit_serial_number();
        prefix.destAddress  = 0;
        prefix.hopCount     = 1;


        int stringlen = 1; 
        setPacketPrefix(buf, prefix);
        if (status.has_value) {
            buf[PREFIX_LENGTH] = 1 //indicates that there's a status
            stringLen += copyStringValue(buf + PREFIX_LENGTH+1, status.value, MAX_PAYLOAD_LENGTH  - 2);
        }
        else {
            buf[PREFIX_LENGTH] = 0; //indicates that there's no status 
        }
        uBit.radio.datagram.send(buf, PREFIX_LENGTH+stringlen);
    }

    

    // commented out to find weirdo bug in online editor
    // void sendString(String msg, PacketType packetType, uint32_t destAddress) {
    //     if (radioEnable() != MICROBIT_OK || NULL == msg) return;

    //     ownMessageId++;
    //     uint8_t buf[32];
    //     memset(buf, 0, 32);

    //     Prefix prefix;
    //     prefix.type = packetType;
    //     prefix.messageId = ownMessageId;
    //     prefix.origAddress = microbit_serial_number();
    //     prefix.destAddress = destAddress;
    //     prefix.hopCount = 1;
    //     setPacketPrefix(buf, prefix);

    //     int stringLen = copyStringValue(buf + PREFIX_LENGTH, msg, MAX_PAYLOAD_LENGTH  - 1);
        
    //     uBit.radio.datagram.send(buf, PREFIX_LENGTH + stringLen);
    // }

    // commented out to find weirdo bug in online editor
    // /**
    //  * Send a string to all micro:bits in the party.
    //  */
    // //%
    // void broadcastString(String message) {
    //     sendString(message, PacketType::BROADCAST_STRING, 0);
    // }

    // commented out to find weirdo bug in online editor
    // /**
    //  * Send a string to the micro:bit with the specified address
    //  */
    // //%
    // void unicastString(String message, uint32_t destAddress) {
    //     sendString(message, PacketType::UNICAST_STRING, destAddress);
    // }

    void sendNumber(int num, PacketType packetType, uint32_t destAddress){
        if (radioEnable() != MICROBIT_OK) return;

        ownMessageId++;
        uint8_t buf[32];
        memset(buf, 0, 32);

        Prefix prefix;
        prefix.type = packetType;
        prefix.messageId = ownMessageId;
        prefix.origAddress = microbit_serial_number();
        prefix.destAddress = destAddress;
        prefix.hopCount = 1;
        setPacketPrefix(buf, prefix);

        memcpy(buf + PREFIX_LENGTH, &num , sizeof(int));

        uBit.radio.datagram.send(buf, PREFIX_LENGTH + sizeof(int));
    }
    
    /**
     * Send a number to all micro:bits in the party.
     */
    //% weight=60
    //% blockId=party_broadcast_number block="Send %value to all party members"
    void broadcastNumber(TNumber number) {
        sendNumber(toInt(number), PacketType::BROADCAST_NUMBER, 0);
    }

    /**
     * Send a number to the micro:bit with the specified address
     */
    //% weight=60
    //% blockId=party_unicast_number block="Send %number to %destAddress"
    void unicastNumber(TNumber number, TNumber destAddress) {
        sendNumber(toInt(number), PacketType::UNICAST_NUMBER, toInt(destAddress));
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
    //% weight=40
    //% blockId=party_size block="party size"
    int partySize() { return partyTable.size(); }

    /**
     * Random Party Member
     */
    //% weight=60
    //% blockId=random_party_member block="random party member"
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
