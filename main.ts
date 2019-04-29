/**
 * Provides access to communication functionality.
 */
//% color=#00d3ea weight=100

namespace parties {

    /*
    basic.forever(() => {
        basic.showNumber(PartiesInternal.getPartyRadioGroup());
        basic.showString(PartiesInternal.getPartyName());
    });
    */

    PartiesInternal.onStringReceived((s: string) => basic.showString(s));
    PartiesInternal.onNumberReceived((n: number) => basic.showNumber(n));

    /*
    input.onButtonPressed(Button.A, () => PartiesInternal.unicastNumber(7777, PartiesInternal.randomPartyMember()));
    input.onButtonPressed(Button.B, () => PartiesInternal.unicastString("abc", PartiesInternal.randomPartyMember()));
    */
    
    joinParty("xyz");
    input.onButtonPressed(Button.A, () => PartiesInternal.broadcastNumber(7777));
    input.onButtonPressed(Button.B, () => PartiesInternal.broadcastString("abc"));
    

    //% block
    export function me(): number { return 0 }

    //% block
    export function firstPartyMember(): number { return 0 }

    //% block
    export function wholeParty(): number { return -1 }

    //% block
    export function message(): string { return "" }

    //% block
    export function joinParty(name: string) { PartiesInternal.joinParty(name) }

    // //% block
    // export function randomPartyMember(): number {
    //     return 0;
    // }

    //% block="send %value to %name"
    export function sendStringTo(value: string, name: number) { }

    //% block="on receive"
    export function onReceive(f: () => void) { f(); };
}