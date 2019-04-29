namespace parties {

    let stringCallback: (s: string) => void = () => {};
    let numberCallback: (n: number) => void = () => {};

    export function onStringReceived(c: (receivedString: string) => void) {
        stringCallback = c;
    }

    export function onNumberReceived(cb: (receivedNo: number) => void) {
        numberCallback = cb;
    }

    //% block
    export function me(): number { return 0 }

    //% block
    export function firstPartyMember(): number { return 0 }

    //% block
    export function wholeParty(): number { return -1 }

    //% block
    export function message(): string { return "" }

    //% block
    export function joinParty(name: string) { }

    // //% block
    // export function randomPartyMember(): number {
    //     return 0;
    // }

    //% block="send %value to %name"
    export function sendStringTo(value: string, name: number) { }

    //% block="on receive"
    export function onReceive(f: () => void) { f(); };

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
