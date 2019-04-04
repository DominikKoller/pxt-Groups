# pxt-groups



## TODO

- [ ] Add a reference for your blocks here
- [ ] Add "icon.png" image (300x200) in the root folder
- [ ] Add "- beta" to the GitHub project description if you are still iterating it.
- [ ] Turn on your automated build on https://travis-ci.org
- [ ] Use "pxt bump" to create a tagged release on GitHub
- [ ] Get your package reviewed and approved https://makecode.microbit.org/packages/approval

Read more at https://makecode.microbit.org/packages/build-your-own

## License



## Supported targets

* for PXT/microbit
(The metadata above is needed for package search.)

## API

* joinParty (partyName: string): boolean
 ```
 Looks for an existing party called partyName. Creates a party with name partyName if it doesn't already exist and then joins it.
 ```
* randomPartyMember: number
 
 ```
 returns the µBit serial number of a random device in the party, which is a unique identifier for that microbit. 
 ```
* oldestPartyMember: number
 
 ```
 returns the µBit serial number of the device in the party which has stayed in the party the longest. NB* the time spent in the party is reset to 0 once device leaves.
 
 ```
 * send(someValue: Any, partyMember: number)
 ```
 Extends the send function so that now the message may be sent to a single, specified party member.
 ```
