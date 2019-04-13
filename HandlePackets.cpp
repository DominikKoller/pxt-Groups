#include "pxt.h"
using namespace pxt;

#define MAX_DATA_PAYLOAD_LENGTH 19
#define DATA_PACKET_PREFIX_SIZE 10

enum PacketType {
    HEARTBEAT = 6,
    BROADCAST = 7,
    UNICAST_STRING = 8,
    UNICAST_NUMBER = 9
};

//Data Packet Spec : size 29 bytes (maximum possible)
// | 0     | 1 ... 4       | 5 ... 8           | 9       | 10... 28 |
// ------------------------------------------------------------------
// | type  | senderID      | receiverID        |hopCount|  payload  |


//Heart Beat Packet Spec: size 11 bytes

// | 0       | 1        | 2...5       | 6 ... 9     | 10      |
// -----------------------------------------------------------
// | type   | messageID | origAddress | destAddress | hopCount |

namespace PartiesInternal {

    bool radioEnabled = false;
    const uint32_t myID = microbit_serial_number();
    // Variables storing the data from the most recently received packet
    uint8_t type;
    uint8_t messageId;
    uint32_t origAddress; //senderID
    uint32_t destAddress; //receiverID
    uint8_t hopCount;
    String payloadString; // may be NULL before first packet
    int payloadInt = 0;

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
    /* todo: segue the String datatype to the c++ string datatype...
    void _joinParty(String partyName){
        std::hash<std::string> hash_fn;
        int id = hash_fn(partyName)%256;
        uBit.radio.setGroup(id);
    }
    
   */

    /**
     * Send the data in a buffer on the radio.
     * Note: The buffer must have length <= 28
     */
    //%
    void sendRawPacket(Buffer data) {
        uBit.radio.datagram.send(data->data, data->length);
    }
    
    
    String getStringValue(uint8_t* buf, uint8_t maxLength) {
        // First byte is the string length
        uint8_t len = min_(maxLength, buf[0]); //just in case user string was initially too big
        return mkString((char*)buf+1, len); //cheeky typecast?
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
    
    
    void unpackPayLoadString(uint8_t* buf){
        payloadString = getStringValue(buf+DATA_PACKET_PREFIX_SIZE, MAX_DATA_PAYLOAD_LENGTH-1 );
    }
    
    void unpackPayloadNumber(uint8_t* buf){
        memcpy(&payloadInt, buf+DATA_PACKET_PREFIX_SIZE, MAX_DATA_PAYLOAD_LENGTH);
    }
    
    
    /** Initialises prefix of packet to be sent*/
    
    void setPacketPrefix(uint8_t buf[], uint32_t receiverid, PacketType type) {
        buf[0] = type;
        memcpy(buf+1, &myID, 4);
        memcpy(buf+5, &receiverid, 4);
        hopCount = 1;
        memcpy(buf+9, &hopCount, 1);
    }
    
    /**
     * Broadcasts a string to a targeted uBit
     */
    //%
    void sendString(String msg, uint32_t receiverid ) {
        if (radioEnable() != MICROBIT_OK || NULL == msg) return;
        
        uint8_t buf[32];
        uint8_t hopCount = 1;

        memset(buf, 0, 32);
        setPacketPrefix(buf, receiverid, PacketType::UNICAST_STRING);
        int stringLen = copyStringValue(buf + DATA_PACKET_PREFIX_SIZE, msg, MAX_DATA_PAYLOAD_LENGTH  - 1);
        
        uBit.radio.datagram.send(buf, DATA_PACKET_PREFIX_SIZE + stringLen);
    }
    
    
    /**
     * Broadcasts a string to a targeted uBit
     */
    //%
    
    void sendNumber(TNumber value, uint32_t receiverid) {
        if (radioEnable() != MICROBIT_OK) return;
        
        int iv = toInt(value);
        double dv = toDouble(value);
        if (iv == dv) {
            uint8_t length = DATA_PACKET_PREFIX_SIZE + sizeof(int);
            uint8_t buf[length];
            memset(buf, 0, length);
            setPacketPrefix(buf, receiverid, PacketType::UNICAST_NUMBER);
            memcpy(buf + DATA_PACKET_PREFIX_SIZE, &iv, sizeof(int));
            uBit.radio.datagram.send(buf, length);
        } else {
            uint8_t length = DATA_PACKET_PREFIX_SIZE + sizeof(double);
            uint8_t buf[length];
            memset(buf, 0, length);
            setPacketPrefix(buf, receiverid, PacketType::UNICAST_NUMBER);
            memcpy(buf + DATA_PACKET_PREFIX_SIZE, &dv, sizeof(double));
            uBit.radio.datagram.send(buf, length);
        }
    }
    
    
    /**
     * Take a heartbeat buffer and unpack the values inside it into variables.
     */
    void unpackHrtBtPacket(uint8_t* buf) {
        memcpy(&messageId,   buf+1,  1);
        memcpy(&origAddress, buf+2,  4);
        memcpy(&destAddress, buf+6,  4);
        memcpy(&hopCount,    buf+10, 1);
    }

    /**
     * Take a data buffer and unpack the values inside it into variables.
     */
    void unpackDataPacket(uint8_t* buf) {
        memcpy(&origAddress, buf+1,  4);
        memcpy(&destAddress, buf+5,  4);
        memcpy(&hopCount,    buf+9, 1);
        //now to extract the payload depending on the type
        switch (type){
            case PacketType::UNICAST_STRING:
                unpackPayLoadString(buf);
                break;
            case PacketType::UNICAST_NUMBER:
                unpackPayloadNumber(buf);
                break;
            default:
                break;
                
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
    void receivePacket() {
        PacketBuffer p = uBit.radio.datagram.recv();
        uint8_t* buf = p.getBytes();
        memcpy(&type,        buf,    1); //unwrapping initiates by extracting type
        switch (type){ //different unwrapping procedures for different packets
            case PacketType::HEARTBEAT:
                unpackHrtBtPacket(buf);
                break;
            case PacketType::UNICAST_STRING:
            case PacketType::UNICAST_NUMBER:
                unpackDataPacket(buf);
                break;
                
            default: break;
        }
    }


    /**
     * Register a function to be called when a radio packet is received
     * Note: Only one function can be registered at once, so the radio module
     * will have to be disabled.
     */
    //%
    void onDataReceived(Action body) {
       if (radioEnable() != MICROBIT_OK) return;
       registerWithDal(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, body);
    }
    

    /**
     * Return the type of packet that was last received.
     */
    //%
    TNumber receivedType() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(type);
    }

    /**
     * Return the message ID from the last received packet.
     */
    //%
    TNumber receivedMessageId() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(messageId);
    }

    /**
     * Return the origin address from the last received packet.
     */
    //%
    TNumber receivedOrigAddress() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(origAddress);
    }

    /**
     * Return the destination address from the last received packet.
     */
    //%
    TNumber receivedDestAddress() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(destAddress);
    }

    /**
     * Return the hop count from the last received packet.
     */
    //%
    TNumber receivedHopCount() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(hopCount);
    }
    
    /**
     * Return the string payload from the last received packet.
     */
    //%
    String receivedString() {
        if (radioEnable() != MICROBIT_OK || NULL == payloadString) return mkString("", 0);
        return payloadString;
    }
    
    
    /**
     * Return the number payload from the last received packet.
     */
    //%
    TNumber receivedInt() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(payloadInt);
    }
    

}
