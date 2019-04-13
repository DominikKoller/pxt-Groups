namespace PartiesInternal {
    control.inBackground(function () {
        while (true) {
            PartiesInternal.sendHeartbeat();
            basic.pause(PartiesInternal.getHeartbeatFrequency());
        }
    });

    PartiesInternal.onDataReceived(() => PartiesInternal.receiveData());
}