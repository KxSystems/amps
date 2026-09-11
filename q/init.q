clib:use`.clib

clib.init[];

// @overview
// Creates a AMPS client and establishes a logon with the AMPS server.
// 
// Supported integer options:
// .kamps.connFixedRetryDelay - Length of time in milliseconds before
// attempting to reconnect
// .kamps.connFixedRetryTimeout - Length of time in milliseconds to continue
// reconnect retries
// .kamps.connPubStoreMemSize - Number of blocks to grow the memory backed
// publish store by when capacity has been exceeded
//
// Supported string options:
// .kamps.connPubStoreFileName - Name to use for the file backed published
// store
// .kamps.connBookStoreFileName - Name to use for the mmap file backed
// bookmard store
// .kamps.connDisconnectHandler - Handler to be invoked when the client is
// unexpectedly disconnected from amps server {fn (#disconnectMessage)}
//
// @param clientName Name of client to be created (has to be unique for amps
// server) {string|symbol} 
// @param URIs       List of uniform resource identifier for amps transport 
// to be connected {(string|symbol)[]}
// @param options    Connection options or generic null (::) to use the 
// defaults.  Dictionary key must be a 11h list.  Values list can be 7h, 11h 
// or mixed list of -7|-11|4h. {dictionary}
//
// @return Reference to the amps client session {foreign}
//
// @throws an error if connection cannot be established
create:{[clientName; URIs; options]
    handle:clib.create[clientName; URIs; options];
    clib.connect handle;
    handle
    }

// @overview
// Publishes data using a given client and topic to publish on. The data can
// be any type and lets the client code perform serialization.
//
// Supported options:
// None (currently) - specify ::
//
// @param handle 	Reference to the amps client session {foreign}
// @param topic  	Topic to publish on {string|symbol}
// @param options   Publisher options or generic null (::) to use the 
// defaults.  Dictionary key must be a 11h list.  Values list can be 7h, 11h 
// or mixed list of -7|-11|4h. {dictionary}
// @param data 	 	Data to publish {(byte|char)[]}
//
// @return {fn (any)}
publish:{[handle; topic; options; data]
    clib.publishBinary[handle;topic;options;data]
    }

// @overview
// Publishes data using a given client and topic to publish on. Uses the 
// sequence id for message deduplication in the AMPS transaction log.
//
// Supported options:
// None (currently) - specify ::
//
// @param handle 	Reference to the amps client session {foreign}
// @param topic  	Topic to publish on {string|symbol}
// @param options   Publisher options or generic null (::) to use the 
// defaults.  Dictionary key must be a 11h list.  Values list can be 7h, 11h 
// or mixed list of -7|-11|4h. {dictionary}
// @param data 	 	Data to publish {(byte|char)[]}
// @param seq    	Sequence number {long}
//
// @return {fn (long; any)}
publishAt:{[handle; topic; options; data; seq]
    clib.publishBinaryAt[handle;topic;options;data;seq]
    }

bookmarkRecent:`recent;
bookmarkEpoch:`$"0";
bookmarkNow:`$"0|1|";


export:([create;clib.connect;clib.isConnected;clib.disconnect;
         publish;publishAt;clib.subscribe;clib.discard;clib.unsubscribe;
         bookmarkRecent;bookmarkEpoch;bookmarkNow])
