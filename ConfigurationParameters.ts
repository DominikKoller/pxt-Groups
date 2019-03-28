namespace Aodv 
{
    /** See section 10 of the memo for info about what all these parameters do: 
     * https://tools.ietf.org/html/rfc3561#section-10
     * 
     * ConfigurationParameters should be a singleton class - a node should only have one set of configuration parameters.
     * However, TypeScript doesn't allow static (i.e. singleton) classes, so I've used a pattern suggested on stackExchange 
     * instead. Usage: just type 'ConfigurationParameters.Instance' whenever you want the parameters, e.g:
     * 
     * var wait = ConfigurationParameters.Instance.nextHopWait
     */
    class ConfigurationParameters 
    {
        /** This member holds the underlying singleton class... */
        private static instance: ConfigurationParameters

        /** ...and this getter exposes it */
        public static get Instance() 
        {
            // If this is the first time Instance() has been called, then 'instance' will be null, so initialise it.
            if (this.instance == null) 
            {
                this.instance = new this()
            }
            return this.instance;
        }

        activeRouteTimeout: number;
        allowedHelloLoss: number;
        helloInterval: number;
        localAddTtl: number;
        myRouteTimeout: number;
        netDiameter: number;
        netTraversalTime: number;
        nextHopWait: number;
        nodeTraversalTime: number;
        pathDiscoveryTime: number;
        rerrRateLimit: number;
        ringTraversalTime: number;
        rreqRetries: number;
        rreqRateLimit: number;
        timeoutBuffer: number;
        ttlStart: number;
        ttlIncrement: number;
        ttlThreshold: number;

        get DeletePeriod() { return 5 * Math.max(this.activeRouteTimeout, this.helloInterval) }
        get MaxRepairTtl() { return 0.3 * this.netDiameter }
        get MyRouteTimeout() { return 2 * this.activeRouteTimeout }
        get NetTraversalTime() { return 2 * this.nodeTraversalTime * this.netDiameter }
        get BlacklistTimeout() { return this.rreqRetries * this.netTraversalTime }
        get NextHopWait() { return this.nodeTraversalTime + 10 }
        get PathDiscoveryTime() { return 2 * this.netTraversalTime }
        get RingTraversalTime() { return 2 * this.nodeTraversalTime * (this.TtlValue + this.timeoutBuffer) }

        // To-do: implement 'TtlValue' and 'MinRepairTtl'. See AODV memo.
        get TtlValue() { return 0  }
        get MinRepairTtl() { return 0 }

        /** Private constructor means the only way to access this class is via the getter 'Instance()'. Values concerning
         * time are given in milliseconds. 
         */
        private constructor() 
        {
            this.activeRouteTimeout = 3000;
            this.allowedHelloLoss = 2;
            this.helloInterval = 1000;
            this.localAddTtl = 2;
            this.netDiameter = 35;
            this.nodeTraversalTime = 40;
            this.rerrRateLimit = 10;
            this.rreqRetries = 2;
            this.rreqRateLimit = 10;
            this.timeoutBuffer = 2;
            this.ttlStart = 1;
            this.ttlIncrement = 2;
            this.ttlThreshold = 7;
        }
    }
}
