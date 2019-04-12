namespace PartiesInternal {

    // using consts instead of configuration singleton to save space on device
    const MAX_HOP_COUNT = 1; //set to 1 to mute rebounds for now TODO: implement rebounds for data packets and set MAX_HOP_COUNT to 2.
    const HEARTBEAT_FREQUENCY = 1000;
    const KEEP_TIME = 7000;
    const REBOUND_MAXWAIT = 500;
    const REBOUND_PROBABILITY = 0.9;

    let partyTable: PartyTable = [];
    let ownMessageId: int8 = 0;

    let stringCallback: (s: string) => void;

    control.inBackground(function () {
        while (true) {
            partyTable = partyTable.filter(member => member.lastSeen + KEEP_TIME >= input.runningTime());
            sendHeartbeat();
            basic.pause(HEARTBEAT_FREQUENCY);
        }
    });

    
    export class Packet {

        /**
         * The number payload if a number was sent in this packet (via `` ``)
         * or 0 if this packet did not contain a number.
         */
        public receivedNumber: number;
        /**
         * The string payload if a string was sent in this packet (via `` ``)
         * or the empty string if this packet did not contain a string.
         */        
        public receivedString: string;
        /**
         * The uBit serial of the sender...
         */
        public senderID: number; 
        /**The uBit serial of the intended receiver */
        public receiverID: number; 
        /**The number of times the specific packet has been emitted */
        public hopCount: number;


 }

    /**Allows user to tell uBit the name of desired party to be joined 

    export function joinParty(partyName: string) {
        _joinParty(partyName)
    }
*/

    export function onStringReceived(c: (receivedString: string) => void) {
        stringCallback = c;
    }

    export function randomPartyMember(): number {
        if (partyTable.length > 0) {
            return partyTable[Math.randomRange(0, partyTable.length - 1)].address;
        }
        else return -1;
    }

    /**
     * Register a callback for the radio packets that will be sent.
     *
     * Checks the type of the packet and calls the corresponding
     * function with the data in the packet as arguments.
     */
    // TODO: Fill in the arguments for the other calls
    // TODO: clean up, call received*() only once
    // TODO: put this all in the cpp code!
    onDataReceived(() => {
        receivePacket(); 
        if (receivedOrigAddress() == control.deviceSerialNumber())
            return;

        const tp = receivedType();

        switch (tp) {
            case PacketType.HEARTBEAT:
                handleHeartbeat(
                    receivedMessageId(),
                    receivedOrigAddress()
                );
                break;
            case PacketType.BROADCAST:
                //          handleBroadcast(/* args */);
                break;
            case PacketType.UNICAST:
                handleUnicast(receivedString(), receivedOrigAddress(), receivedDestAddress(), receivedHopCount() )
                break;
            default: // unknown packet
                break;
        }
    });

    /**
     * Send the different types of packets for the protocol.
     */
    // TODO: Implement the rest of these
    function sendHeartbeat() {
        ownMessageId += 1;

        const buf = pins.createBuffer(11);
        buf.setNumber(NumberFormat.UInt8LE, 0, PacketType.HEARTBEAT);
        buf.setNumber(NumberFormat.UInt8LE, 1, ownMessageId);
        buf.setNumber(NumberFormat.UInt32LE, 2, control.deviceSerialNumber());
        buf.setNumber(NumberFormat.UInt32LE, 6, 0); //destination address
        buf.setNumber(NumberFormat.UInt8LE, 10, 1); //hop count
        sendRawPacket(buf);
    }

    /**
     * Functions to handle incoming packets.
     *
     * Note: Typescript will treat the 4 byte values as signed integers, so if
     * you need to compare two of these large values then first convert them to 
     * unsigned ints with (value>>>0). The type of this result is actually a
     * floating point value, so may not be exact.
     */
    // TODO: Implement all of these, can be moved into separate files if wanted

    function handleHeartbeat(messageId: number, origAddress: number) {
        const originator = findAddress(partyTable, origAddress);
        
        if (originator == undefined) {
            partyTable.push(new PartyMember(origAddress, input.runningTime(), messageId));
            rebound();
        }
        else if (messageId > originator.lastMessageId) {
            originator.lastMessageId = messageId;
            originator.lastSeen = input.runningTime();
            rebound();
        }
    }

    function handleBroadcast( /*args*/) { }

/** Handles incoming data packet by wrapping up its contents in the Packet class. Rejects the incoming data if it's not intended for this device. */
//maybe a bit too OO for now...
    function handleUnicast( receivedString: string, senderID: number, receiverID: number, hopCount: number): void {
        
        if (receiverID != control.deviceSerialNumber() ) {
            return;
        }

        const packet = new Packet();
        packet.receiverID = receiverID;
        packet.receivedString = receivedString;
        packet.senderID = senderID;
        packet.hopCount = hopCount; 
        
        stringCallback(receivedString);
        //return packet;
     }

    // TODO tidy: eg make a packet class, call receivedType() etc only once
    function rebound() {
        // check if the last message needs to be redistributed, then redistribute
        if (receivedHopCount() < MAX_HOP_COUNT) {
            // don't always rebound, to lessen the number of messages in the network
            // TODO make this a function of the number of nodes in the network
            if (Math.random() > REBOUND_PROBABILITY) return;

            // Microbits are bad at handling messages coming exactly at the same time
            basic.pause(Math.randomRange(0, REBOUND_MAXWAIT));

            const buf = pins.createBuffer(11);
            buf.setNumber(NumberFormat.UInt8LE, 0, receivedType());
            buf.setNumber(NumberFormat.UInt8LE, 1, receivedMessageId());
            buf.setNumber(NumberFormat.UInt32LE, 2, receivedOrigAddress());
            buf.setNumber(NumberFormat.UInt32LE, 6, receivedDestAddress());
            buf.setNumber(NumberFormat.UInt8LE, 10, receivedHopCount() + 1);
            sendRawPacket(buf);
        }
    }

    // Temp
    // TODO think about how to expose the lib functionality
    export function numberOfPartyMembers(): number { return partyTable.length; }
}