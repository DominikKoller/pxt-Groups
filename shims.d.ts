// Auto-generated. Do not edit.
declare namespace PartiesInternal {

    /**
     * Send the data in a buffer on the radio.
     * Note: The buffer must have length <= 28
     */
    //% shim=PartiesInternal::sendRawPacket
    function sendRawPacket(data: Buffer): void;

    /**
     * Read a packet from the queue of received packets and extract the
     * relevant data from it.
     *
     * A call to this function will be followed by `receivedType()` to get the
     * type of packet that is received, then calls to `received*()` to get the
     * rest of the data in the packet.
     */
    //% shim=PartiesInternal::receivePacket
    function receivePacket(): void;

    /**
     * Register a function to be called when a radio packet is received
     * Note: Only one function can be registered at once, so the radio module
     * will have to be disabled.
     */
    //% shim=PartiesInternal::onDataReceived
    function onDataReceived(body: () => void): void;

    /**
     * Return the type of packet that was last received.
     */
    //% shim=PartiesInternal::receivedType
    function receivedType(): number;

    /**
     * Return the message ID from the last received packet.
     */
    //% shim=PartiesInternal::receivedMessageId
    function receivedMessageId(): number;

    /**
     * Return the origin address from the last received packet.
     */
    //% shim=PartiesInternal::receivedOrigAddress
    function receivedOrigAddress(): number;

    /**
     * Return the destination address from the last received packet.
     */
    //% shim=PartiesInternal::receivedDestAddress
    function receivedDestAddress(): number;

    /**
     * Return the hop count from the last received packet.
     */
    //% shim=PartiesInternal::receivedHopCount
    function receivedHopCount(): number;

    /**
     * Return the string payload from the last received packet.
     */
    //% shim=PartiesInternal::receivedString
    function receivedString(): string;
}

// Auto-generated. Do not edit. Really.
