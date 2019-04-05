// Auto-generated. Do not edit.
declare namespace Aodv {

    /**
     * Send the data in a buffer on the radio.
     * Note: The buffer must have length <= 28
     */
    //% shim=Aodv::sendRawPacket
    function sendRawPacket(data: Buffer): void;

    /**
     * Read a packet from the queue of received packets and extract the
     * relevant data from it.
     *
     * A call to this function will be followed by `receivedType()` to get the
     * type of packet that is received, then calls to `received*()` to get the
     * rest of the data in the packet.
     */
    //% shim=Aodv::receivePacket
    function receivePacket(): void;

    /**
     * Register a function to be called when a radio packet is received
     * Note: Only one function can be registered at once, so the radio module
     * will have to be disabled.
     */
    //% shim=Aodv::onDataReceived
    function onDataReceived(body: () => void): void;

    /**
     * Return the type of packet that was last received.
     */
    //% shim=Aodv::receivedType
    function receivedType(): number;

    /**
     * Return the flags from the last received packet.
     */
    //% shim=Aodv::receivedFlags
    function receivedFlags(): number;

    /**
     * Return the prefix size from the last received packet.
     */
    //% shim=Aodv::receivedPrefixSize
    function receivedPrefixSize(): number;

    /**
     * Return the hop count from the last received packet.
     */
    //% shim=Aodv::receivedHopCount
    function receivedHopCount(): number;

    /**
     * Return the destination address from the last received packet.
     */
    //% shim=Aodv::receivedDestAddress
    function receivedDestAddress(): number;

    /**
     * Return the destination sequence number from the last received packet.
     */
    //% shim=Aodv::receivedDestSeqNum
    function receivedDestSeqNum(): number;

    /**
     * Return the origin address from the last received packet.
     */
    //% shim=Aodv::receivedOrigAddress
    function receivedOrigAddress(): number;

    /**
     * Return the origin sequence number from the last received packet.
     */
    //% shim=Aodv::receivedOrigSeqNum
    function receivedOrigSeqNum(): number;
}

// Auto-generated. Do not edit. Really.
