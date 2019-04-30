// Auto-generated. Do not edit.
declare namespace parties {

    /** 
     * Configures the party name and radio group. 
     */
    //% weight=60
    //% blockId=join_party block="join party %name" shim=parties::joinParty
    function joinParty(name: string): void;

    /**
     * Filters out old entries in the table
     * Also call at HEARTBEAT_FREQUENCY from TS
     */
    //% shim=parties::filterTable
    function filterTable(): void;

    /**
     * Read a packet from the queue of received packets and react accordingly
     */
    //% shim=parties::receiveData
    function receiveData(): void;

    /** 
     * To be called at HEARTBEAT_FREQUENCY.
     */
    //% shim=parties::sendHeartbeat
    function sendHeartbeat(): void;

    /**
     * Send a string to all micro:bits in the party.
     */
    //% shim=parties::broadcastString
    function broadcastString(message: string): void;

    /**
     * Send a string to the micro:bit with the specified address
     */
    //% shim=parties::unicastString
    function unicastString(message: string, destAddress: uint32): void;

    /**
     * Send a number to all micro:bits in the party.
     */
    //% weight=60
    //% blockId=party_broadcast_number block="send %value to all party members" shim=parties::broadcastNumber
    function broadcastNumber(number: number): void;

    /**
     * Send a number to the micro:bit with the specified address
     */
    //% weight=60
    //% blockId=party_unicast_number block="send %number to %destAddress" shim=parties::unicastNumber
    function unicastNumber(number: number, destAddress: number): void;

    /**
     * Use this only to call receiveData from Typescript
     * (workaround, cannot figure out how to pass c++ function to registerWithDal)
     * Note: Only one function can be registered at once, so the radio module
     * will have to be disabled.
     */
    //% shim=parties::onDataReceived
    function onDataReceived(body: () => void): void;

    /**
     * Use as frequency to call sendHeartbeat
     */
    //% shim=parties::getHeartbeatFrequency
    function getHeartbeatFrequency(): int32;

    /**
     * Numer of Party Members
     */
    //% weight=40
    //% blockId=party_size block="party size" shim=parties::partySize
    function partySize(): int32;

    /**
     * Random Party Member
     */
    //% weight=60
    //% blockId=random_party_member block="random party member" shim=parties::randomPartyMember
    function randomPartyMember(): uint32;

    /**
     * For TS to check whether there is a new payload to react to
     */
    //% shim=parties::receivedPayloadType
    function receivedPayloadType(): PayloadType;

    /**
     * Get the received string
     */
    //% shim=parties::receivedStringPayload
    function receivedStringPayload(): string;

    /**
     * Get the received number
     */
    //% shim=parties::receivedNumberPayload
    function receivedNumberPayload(): int32;
}

// Auto-generated. Do not edit. Really.
