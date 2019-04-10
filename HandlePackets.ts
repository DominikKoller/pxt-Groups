namespace NNR {

    const MAX_HOP_COUNT: int8 = 2;
    const HEARTBEAT_FREQUENCY = 200;
    const KEEP_TIME = 1000;

    let partyTable: PartyTable = [];
    let messageId: int8 = 0;

    /**
     * Register a callback for the radio packets that will be sent.
     *
     * Checks the type of the packet and calls the corresponding
     * function with the data in the packet as arguments.
     */
    // TODO: Fill in the arguments for the other calls

    control.inBackground(function () {
        while (true) {
            partyTable = partyTable.filter(member => member.lastSeen + KEEP_TIME >= input.runningTime());
            sendHeartbeat();
            basic.pause(HEARTBEAT_FREQUENCY);
        }
    });

    basic.forever(() => {
        basic.showNumber(partyTable.length);
    });

    onDataReceived(() => {
        receivePacket();
        const tp = receivedType();
        switch (tp) {
            case PacketType.HEARTBEAT:
                handleHeartbeat(
                    receivedMessageId(),
                    receivedOrigAddress(),
                    receivedHopCount()
                );
                break;
            case PacketType.BROADCAST:
                //          handleBroadcast(/* args */);
                break;
            case PacketType.UNICAST:
                // handleUnicast(/* args */);
                break;
            default: // unknown packet
                break;
        }
    });

    /**
     * Send the different types of packets for the AODV protocol.
     */
    // TODO: Implement the rest of these
    function sendHeartbeat() {
        messageId += 1;

        const buf = pins.createBuffer(11);
        buf.setNumber(NumberFormat.UInt8LE, 0, PacketType.HEARTBEAT);
        buf.setNumber(NumberFormat.UInt8LE, 1, messageId);
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
    function handleHeartbeat(messageId: number, origAddress: number, hopCount: number) {
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

    function rebound() {
        // TODO: check if the last message needs to be redistributed, then redistribute
    }
}