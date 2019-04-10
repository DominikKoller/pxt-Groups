#include "pxt.h"
using namespace pxt;

enum PacketType {
    HEARTBEAT = 6,
    BROADCAST = 7,
    UNICAST = 8
};

namespace PartiesInternal {

    bool radioEnabled = false;

    // Variables storing the data from the most recently received packet
    uint8_t type;
    uint8_t messageId;
    uint32_t origAddress;
    uint32_t destAddress;
    uint8_t hopCount;

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
    void unpackPacket(uint8_t* buf) {
        memcpy(&type,        buf,    1);
        memcpy(&messageId,   buf+1,  1);
        memcpy(&origAddress, buf+2,  4);
        memcpy(&destAddress, buf+6,  4);
        memcpy(&hopCount,    buf+10, 1);
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
        unpackPacket(buf);
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
}