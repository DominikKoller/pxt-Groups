namespace Aodv
{
    /** The routing table; used for storing info about routes to other nodes  */
    type RoutingTable = Route[];

    /** Represents a route from this node to another node */
    class Route 
    {
        /** The address of this route's destination node */
        destinationAddress: number;

        /** The most up-to-date sequence number of the destination node */
        destinationSequenceNumber: number;

        isDestinationSequenceNumberValid: boolean;

        routeState: RouteState;

        /**  Number of hops needed to reach the destination node */
        hopCount: number;

        /** The address of the first intermediate node on this route. Might be the destination node itself */
        nextHop: number;

        /** Addresses of neighbours likely to use this node as a next hop towards this route's destination */
        listOfPrecursors: number[];

        /** Number of milliseconds after which the route should be expired or deleted */
        lifetime: number;

        // N.B. Haven't implemented 'network interface'. 
    }

    enum RouteState { Valid, Invalid, Repairable, BeingRepaired };
}