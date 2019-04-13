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
