# pxt-groups

This package helps you build multiplayer games with the microbit.
All microbits using this package will be able to:

Get a list of all other microbits in the area - we call them 'Party Members'.
You can send messages to every Party Member individually, or to all at once.
You can set your own status, and see the current status of all other Party Members at any time.


The low-level part of this is undocumented so far, here's a rough overview:
Every Member sends out a heartbeat at a given frequency, containing amongst other things the senders' address (microbit serial number) and a integer that is the current status of the Member (user-chosen).

A Member keeps a list of all Members it has received messages from.
Upon receiving a heartbeat, a Member will update that table. The table is regularly cleared of old entries, ie of Members that haven't been sending messages for a while.

Sending messages to a specific Member works by putting a Destination Address into the message. Only the member with this address will receive the message.

## License



## Supported targets

* for PXT/microbit
(The metadata above is needed for package search.)

## API

* setStatus (status: number)
 ```
 Set your own status, for others to see.
 ```
* getStatus (): number
 ```
 Your own current status, as last set by setStatus.
 ```
* broadcastNumber(value: number)
 ```
Sends a number to all Party Members.
 ```
* unicastNumber(value: number, member: PartyMember)
 ```
Sends a number to a Party Member
 ```
* partySize(): number
 ```
Returns the current size of the party (not including yourself) 
 ```
* onNumberReceived(cb: (receivedNo: number) => void)
```
Registers code to run when the radio receives a number in the party
```
* allPartyMembers(): PartyMember[]
```
A list of all current party members
```
* randomPartyMember(): PartyMember
```
A random current party member
```
* statusOf(member: PartyMember): number
```
The status of the given party member
```