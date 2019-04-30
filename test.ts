// tests go here; this will not be compiled when this package is used as a library

namespace parties {

    parties.joinParty("b");

    input.onButtonPressed(Button.A, () => parties.broadcastNumber(1));
    input.onButtonPressed(Button.B, () => parties.broadcastString("x"));

    parties.onStringReceived((s: string) => basic.showString(s));
    parties.onNumberReceived((n: number) => basic.showNumber(n));
}
