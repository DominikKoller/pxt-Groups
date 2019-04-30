/**
 * Radio communication for groups
 */
//% color=#00d3ea weight=100 icon="\uf2b5"
namespace parties {

    let stringCallback: (s: string) => void = () => {};
    let numberCallback: (n: number) => void = () => {};

    /**
     * Registers code to run when the radio receives a string in the party
     */
    //% block="on string received"
    //% blockId=party_on_string
    //% draggableParameters=reporter
    export function onStringReceived(c: (receivedString: string) => void) {
        stringCallback = c;
    }

    /**
     * Registers code to run when the radio receives a number in the party
     */
    //% block="on number received"
    //% blockId=party_on_number
    //% draggableParameters=reporter
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
