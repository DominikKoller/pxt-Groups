namespace PartiesInternal {

    let stringCallback: (s: string) => void;
    let numberCallback: (n: number) => void; 

    export function onStringReceived(c: (receivedString: string) => void) {
        stringCallback = c;
    }

    export function onNumberReceived(cb: (receivedNo: number) => void) {
        numberCallback = cb;
    }

    control.inBackground(function () {
        while (true) {
            PartiesInternal.filterTable();
            PartiesInternal.sendHeartbeat();
            basic.pause(PartiesInternal.getHeartbeatFrequency());
        }
    });

    PartiesInternal.onDataReceived(() => {
        PartiesInternal.receiveData();
        switch(PartiesInternal.receivedPayloadType()){
            case PayloadType.STRING:
                stringCallback(PartiesInternal.receivedStringPayload());
                break;
            case PayloadType.NUM:
                numberCallback(PartiesInternal.receivedNumberPayload());
                break;


            default: break;
        }
    });
}