/**
 * Provides access to communication functionality.
 */
//% color=#00d3ea weight=100

namespace parties {

    input.onButtonPressed(Button.A, function () {
        let r = PartiesInternal.randomPartyMember();
        basic.showNumber(r);
        PartiesInternal.sendString("hello", r);
    });

    PartiesInternal.onStringReceived(c => basic.showString(c));

    //% block
    export function me(): number { return 0 }

    //% block
    export function firstPartyMember(): number { return 0 }

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
}