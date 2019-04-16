namespace PartiesInternal {

    let stringCallback: (s: string) => void;

    export function onStringReceived(c: (receivedString: string) => void) {
        stringCallback = c;
    }

    basic.forever(() => {
        PartiesInternal.filterTable();
        PartiesInternal.sendHeartbeat();
        basic.pause(PartiesInternal.getHeartbeatFrequency());
    });

    PartiesInternal.onDataReceived(() => {
        PartiesInternal.receiveData();
        switch(PartiesInternal.receivedPayloadType()){
            case PayloadType.STRING:
                stringCallback(PartiesInternal.receivedStringPayload());
                break;
            default: break;
        }
    });
}
