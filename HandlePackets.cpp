#include "pxt.h"
#include <algorithm>
#include <vector>
using namespace pxt;

#define MAX_PAYLOAD_LENGTH 19
#define PREFIX_LENGTH 10

enum PacketType {
    HEARTBEAT = 6,
};

struct Prefix {
    PacketType type;
    int8_t messageId;
    int32_t origAddress;
    int32_t destAddress;
    int8_t hopCount;
};

// Packet Spec

// | 0       | 1        | 2...5       | 6 ... 9     | 10       | 11... 28 |
// ------------------------------------------------------------------------
// | type   | messageID | origAddress | destAddress | hopCount | payload  |

namespace PartiesInternal {

    bool radioEnabled = false;

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
    }
}
