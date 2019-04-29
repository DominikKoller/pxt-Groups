// Auto-generated. Do not edit.
declare namespace PartiesInternal {

    /** 
     * Configures the party name and radio group. 
     */
    //% shim=PartiesInternal::joinParty
    function joinParty(name: string): void;

    /**
     * Filters out old entries in the table
     * Also call at HEARTBEAT_FREQUENCY from TS
     */
    //% shim=PartiesInternal::filterTable
    function filterTable(): void;

    /**
     * Read a packet from the queue of received packets and react accordingly
     */
    //% shim=PartiesInternal::receiveData
    function receiveData(): void;

    /** 
     * To be called at Heartbeat Frequency
     */
    //% shim=PartiesInternal::sendHeartbeat
    function sendHeartbeat(): void;

    /**
     * Send a string to all micro:bits in the party.
     */
    //% shim=PartiesInternal::broadcastString
    function broadcastString(message: string): void;

    /**
     * Send a string to the micro:bit with the specified address
     */
    //% shim=PartiesInternal::unicastString
    function unicastString(message: string, destAddress: uint32): void;

    /**
     * Send a number to all micro:bits in the party.
     */
    //% shim=PartiesInternal::broadcastNumber
    function broadcastNumber(number: int32): void;

    /**
     * Send a number to the micro:bit with the specified address
     */
    //% shim=PartiesInternal::unicastNumber
    function unicastNumber(number: int32, destAddress: uint32): void;

    /**
     * Use this only to call receiveData from Typescript
     * (workaround, cannot figure out how to pass c++ function to registerWithDal)
     * Note: Only one function can be registered at once, so the radio module
     * will have to be disabled.
     */
    //% shim=PartiesInternal::onDataReceived
    function onDataReceived(body: () => void): void;

    /**
     * Use as frequency to call sendHeartbeat
     */
    //% shim=PartiesInternal::getHeartbeatFrequency
    function getHeartbeatFrequency(): int32;

    /**
     * Numer of Party Members
     */
    //% shim=PartiesInternal::numberOfPartyMembers
    function numberOfPartyMembers(): int32;

    /**
     * Random Party Member
     */
    //% shim=PartiesInternal::randomPartyMember
    function randomPartyMember(): uint32;

    /**
     * For TS to check whether there is a new payload to react to
     */
    //% shim=PartiesInternal::receivedPayloadType
    function receivedPayloadType(): PayloadType;

    /**
     * Get the received string
     */
    //% shim=PartiesInternal::receivedStringPayload
    function receivedStringPayload(): string;

    /**
     * Get the received number
     */
    //% shim=PartiesInternal::receivedNumberPayload
    function receivedNumberPayload(): int32;
}

// Auto-generated. Do not edit. Really.
