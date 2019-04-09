namespace Aodv
{
    /**
     * Register a callback for the radio packets that will be sent.
     *
     * Checks the type of the packet and calls the corresponding
     * function with the data in the packet as arguments.
     */
    // TODO: Fill in the arguments for the other calls
    onDataReceived(() => {
        receivePacket();
        const tp = receivedType();
        switch (tp) {
            case PacketType.RREQ:
                handleRREQ(
                    receivedRREQID(),
                    receivedFlags(),                    
                    receivedHopCount(),
                    receivedDestAddress(),
                    receivedDestSeqNum(),
                    receivedOrigAddress(),
                    receivedOrigSeqNum()
                );
                break;
            case PacketType.RREP:
                handleRREP(
                    receivedFlags(),
                    receivedPrefixSize(),
                    receivedHopCount(),
                    receivedDestAddress(),
                    receivedDestSeqNum(),
                    receivedOrigAddress(),
                    receivedOrigSeqNum()
                );
                break;
            case PacketType.RERR:
                handleRERR(receivedFlags(), receivedDestAddress(), 
                receivedDestSeqNum(), );
                break;
            case PacketType.RREP_ACK:
                handleRREP_ACK();
                break;
            default: // unknown packet
                break;
        }
    });

    /**
     * Send the different types of packets for the AODV protocol.
     */
    // TODO: Implement the rest of these
    export function sendRREQ(rreqid: number, flags: number, hopCount: number, destAddress: number, 
        destSeqNum: number, origAddress: number, origSeqNum: number) : void {
        const buf = pins.createBuffer(20);
        buf.setNumber(NumberFormat.UInt8LE,  0,  PacketType.RREQ);
        buf.setNumber(NumberFormat.UInt8LE,  1,  flags);
        buf.setNumber(NumberFormat.UInt8LE,  2,  rreqid);
        buf.setNumber(NumberFormat.UInt8LE,  3,  hopCount);
        buf.setNumber(NumberFormat.UInt32LE, 4,  destAddress);
        buf.setNumber(NumberFormat.UInt32LE, 8,  destSeqNum);
        buf.setNumber(NumberFormat.UInt32LE, 12, origAddress);
        buf.setNumber(NumberFormat.UInt32LE, 16, origSeqNum);
        sendRawPacket(buf);
    }


    export function sendRREP(flags: number, prefixSize: number, hopCount: number,
                      destAddress: number, destSeqNum: number,
                      origAddress: number, origSeqNum: number): void {
        const buf = pins.createBuffer(20);
        buf.setNumber(NumberFormat.UInt8LE,  0,  PacketType.RREP);
        buf.setNumber(NumberFormat.UInt8LE,  1,  flags);
        buf.setNumber(NumberFormat.UInt8LE,  2,  prefixSize);
        buf.setNumber(NumberFormat.UInt8LE,  3,  hopCount);
        buf.setNumber(NumberFormat.UInt32LE, 4,  destAddress);
        buf.setNumber(NumberFormat.UInt32LE, 8,  destSeqNum);
        buf.setNumber(NumberFormat.UInt32LE, 12, origAddress);
        buf.setNumber(NumberFormat.UInt32LE, 16, origSeqNum);
        sendRawPacket(buf);
    }
    export function sendRERR(flags:number, unreachableDestAddress: number, unreachableDestSeqNum: number): void {
        const buf = pins.createBuffer (10);
        buf.setNumber(NumberFormat.UInt8LE, 0, PacketType.RERR); 
        buf.setNumber(NumberFormat.UInt8LE,  1,  flags);
        buf.setNumber(NumberFormat.UInt32LE, 2, unreachableDestAddress);
        buf.setNumber(NumberFormat.UInt32LE, 6, unreachableDestSeqNum);
        sendRawPacket(buf);
    }



    function sendRREQ_ACK(/* args */) {}

    /**
     * Functions to handle incoming packets.
     *
     * Note: Typescript will treat the 4 byte values as signed integers, so if
     * you need to compare two of these large values then first convert them to 
     * unsigned ints with (value>>>0). The type of this result is actually a
     * floating point value, so may not be exact.
     */
    // TODO: Implement all of these, can be moved into separate files if wanted
    function handleRREQ(rreqid: number, flags: number, hopCount: number, destAddress: number, 
        destSeqNum: number, origAddress: number, origSeqNum: number) {}


    function handleRREP(flags: number, prefixSize: number, hopCount: number,
                        destAddress: number, destSeqNum: number,
                        origAddress: number, origSeqNum: number) {}


    function handleRERR(flags: number, unreachableDestAddress: number, 
        unreachableDestSeqNum: Number) {

        }


    function handleRREP_ACK() {}
}
