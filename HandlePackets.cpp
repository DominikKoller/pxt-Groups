#include "pxt.h"
using namespace pxt;

enum PacketType {
    RREQ = 6,
    RREP = 7,
    RERR = 8,
    RREP_ACK = 9
};

namespace Aodv {
    
    bool radioEnabled = false;

    // Variables storing the data from the most recently received packet
    uint8_t type;
    uint8_t flags;
    uint8_t prefixSize;
    uint8_t hopCount;
    uint32_t destAddress;
    uint32_t destSeqNum;
    uint32_t origAddress;
    uint32_t origSeqNum;
    uint8_t rreqid;


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

    /**
     * Send the data in a buffer on the radio.
     * Note: The buffer must have length <= 28
     */
    //%
    void sendRawPacket(Buffer data) {
        uBit.radio.datagram.send(data->data, data->length);
    }

    /**
     * Take a buffer and unpack the values inside it into variables.
     */
    // TODO: Implement the rest of these, possibly introducing a few new variables
    void unpackRREQ(uint8_t* buf) {
        memcpy(&flags,       buf+1,  1);
        memcpy(&rreqid,  buf+2,  1);
        memcpy(&hopCount,    buf+3,  1);
        memcpy(&destAddress, buf+4,  4);
        memcpy(&destSeqNum,  buf+8,  4);
        memcpy(&origAddress, buf+12, 4);
        memcpy(&origSeqNum,  buf+16, 4);
    
    }
    
    
    void unpackRREP(uint8_t* buf) {
        memcpy(&flags,       buf+1,  1);
        memcpy(&prefixSize,  buf+2,  1);
        memcpy(&hopCount,    buf+3,  1);
        memcpy(&destAddress, buf+4,  4);
        memcpy(&destSeqNum,  buf+8,  4);
        memcpy(&origAddress, buf+12, 4);
        memcpy(&origSeqNum,  buf+16, 4);
    }
    void unpackRERR(uint8_t* buf) {
        memcpy(&flags, buf+1, 1);
        memcpy(&destAddress, buf+2,  4);
        memcpy(&destSeqNum, buf+6, 4);
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
        memcpy(&type, buf, 1);
        switch (type) {
            case PacketType::RREQ:
                unpackRREQ(buf);
                break;
            case PacketType::RREP:
                unpackRREP(buf);
                break;
            case PacketType::RERR:
                unpackRERR(buf);
                break;
            case PacketType::RREP_ACK:
                break;
            default: // unknown packet
                return;
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
     * Return the flags from the last received packet.
     */
    //%
    TNumber receivedFlags() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(flags);
    }

    /**
     * Return the prefix size from the last received packet.
     */
    //%
    TNumber receivedPrefixSize() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(prefixSize);
    }
    
    /**
     * Return the rreqid from the last received RREQ packet.
     */
    //%
    
    TNumber receivedRREQID() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(rreqid);
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
     * Return the destination address from the last received packet.
     */
    //%
    TNumber receivedDestAddress() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(destAddress);
    }

    /**
     * Return the destination sequence number from the last received packet.
     */
    //%
    TNumber receivedDestSeqNum() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(destSeqNum);
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
     * Return the origin sequence number from the last received packet.
     */
    //%
    TNumber receivedOrigSeqNum() {
        if (radioEnable() != MICROBIT_OK) return fromInt(0);
        return fromInt(origSeqNum);
    }
}
