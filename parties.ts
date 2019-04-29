namespace parties {

    let stringCallback: (s: string) => void = () => {};
    let numberCallback: (n: number) => void = () => {};

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
            parties.filterTable();
            parties.sendHeartbeat();
            basic.pause(parties.getHeartbeatFrequency());
        }
    });

    parties.onDataReceived(() => {
        parties.receiveData();
        switch(parties.receivedPayloadType()){
            case PayloadType.STRING:
                stringCallback(parties.receivedStringPayload());
                break;
            case PayloadType.NUM:
                numberCallback(parties.receivedNumberPayload());
                break;


            default: break;
        }
    });
}
