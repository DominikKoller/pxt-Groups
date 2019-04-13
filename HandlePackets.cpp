#include "pxt.h"
#include <algorithm>
#include <vector>
using namespace pxt;

#define MAX_PAYLOAD_LENGTH 19
#define PREFIX_LENGTH 10

#define MAX_HOP_COUNT 1;
#define HEARTBEAT_FREQUENCY 1000;
#define KEEP_TIME 7000;
#define REBOUND_MAXWAIT 500;
#define REBOUND_PROBABILITY 0.9;

enum PacketType {
    HEARTBEAT = 6,
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

// Packet Spec

// | 0       | 1        | 2...5       | 6 ... 9     | 10       | 11... 28 |
// ------------------------------------------------------------------------
// | type   | messageID | origAddress | destAddress | hopCount | payload  |

namespace PartiesInternal {

    bool radioEnabled = false;

    uint8_t ownMessageId = 0;

    std::vector<PartyMember> partyTable;

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

    void sendRawPacket(Buffer data) {
        uBit.radio.datagram.send(data->data, data->length);
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
        } else if (prefix.messageId > it->lastMessageId){
            // we haven't seen this message before
            it->lastMessageId = prefix.messageId;
            it->lastSeen = system_timer_current_time();
        }
    }

    /**
     * Read a packet from the queue of received packets and extract the
     * relevant data from it.
     *
     * A call to this function will be followed by `receivedType()` to get the
     * type of packet that is received, then calls to `received*()` to get the
     * rest of the data in the packet.
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
}
