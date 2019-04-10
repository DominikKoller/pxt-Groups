namespace PartiesInternal {

    // TODO make this a hashmap
    export type PartyTable = PartyMember[];

    /** Represents a member of the party */
    export class PartyMember {

        address: int32;

        /** time of the last message received, in milliseconds of running time */
        lastSeen: int32;

        /** last message id. These are increasing */
        lastMessageId: int8;

        constructor(address: number, lastSeen: number, lastMessageId: number){
            this.address = address;
            this.lastSeen = lastSeen;
            this.lastMessageId = lastMessageId;
        }
    }

    export function findAddress(table: PartyMember[], address: number): PartyMember {
        for (let member of table) {
            if(member.address == address)
                return member;
        }
        return undefined;
    }
}