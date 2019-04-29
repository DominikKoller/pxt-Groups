namespace PartiesInternal {

    let stringCallback: (s: string) => void;
    let numberCallback: (n: number) => void; 

    export function onStringReceived(c: (receivedString: string) => void) {
        stringCallback = c;
    }

    export function onNumberReceived(cb: (receivedNo: number) => void) {
        numberCallback = cb;
    }

    // basic.forever will call inBackground with while(true) and basic.pause(20)
    // using control.inBackground to avoid that
    // see https://makecode.microbit.org/device/reactive
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
