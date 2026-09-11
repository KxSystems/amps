# AMPS Module Reference

The code examples below assume that the AMPS module has been loaded into a `.kamps` directory, i.e.
```q
.kamps:use`kx.amps
```
The type names used in this document have the following meanings:

|Type   |Description|
|-------|-----------|
|string |`char` list|
|integer|`short`,`int` or `long`|

## `create`
Creates an AMPS client and establishes a logon with the AMPS server.

### Parameters
|Name        |Type              |Description|
|------------|------------------|-----------|
|`clientName`|string/symbol     |Name of client to be created (has to be unique for AMPS server)|
|`URIs`      |string/symbol list|AMPS server URIs|
|`options`   |dictionary/null   |Connection options|

#### Options
The keys accepted by the `options` dictionary are below.

|Key                |Value  |Description|
|-------------------|-------|-----------|
|`fixedRetryDelay`  |integer|The delay (ms) to be used between reconnect attempts to the same server.|
|`fixedRetryTimeout`|integer|The maximum time (ms) to retry before giving up|
|`pubStoreMemResize`|integer|Use memory as the publish store. The value is the number of blocks to grow by when capacity has been exceeded|
|`pubStoreFileName` |string |Use a memory mapped file as the publish store. The value is the file name to use|
|`bookStoreFileName`|string |Use a memory mapped file as the bookmark store. The value is the file name to use|

## `isConnected`
Returns a boolean atom indicating whether the client is connected to the AMPS server.

### Parameters
|Name    |Type   |Description|
|--------|-------|-----------|
|`handle`|Foreign|AMPS client handle|

## `disconnect`
Disconnect the client from the AMPS server.

### Parameters
|Name    |Type   |Description|
|--------|-------|-----------|
|`handle`|Foreign|AMPS client handle|

## `publish`
Publishes data using a given client and topic. The calling code is required to perform the serialization, e.g. `-8!x`

### Parameters
|Name     |Type            |Description|
|---------|----------------|-----------|
|`handle` |Foreign         |AMPS client handle|
|`topic`  |string/symbol   |Topic to publish on|
|`options`|dictionary/null |Publisher options|
|`data`   |byte list/string|Connection options|

#### Options
No options are currently defined.

## `publishAt`
Publishes data using a given client and topic. The calling code is required to perform the serialization, e.g. `-8!x`.
The Sequence ID is used for message deduplication in the AMPS transaction log.

### Parameters
|Name     |Type            |Description|
|---------|----------------|-----------|
|`handle` ||Foreign        |AMPS client handle|
|`topic`  |string/symbol   |Topic to publish on|
|`options`|dictionary/null |Publisher options|
|`data`   |byte list/string|Connection options|
|`seq`    |int/long        |Sequence number|

#### Options
No options are currently defined.

## `subscribe`
Subscribes to a single topic and registers a callback for processing inbound data.

### Parameters
|Name      |Type           |Description|
|----------|---------------|-----------|
|`handle`  |Foreign        |Reference to AMPS client|
|`subName` |string/symbol  |Name of the subscription to create|
|`topic`   |string/symbol  |Name of the topic to subscribe to|
|`callback`|string/symbol  |Name of callback function to invoke|
|`options` |dictionary/null|Subscription options or null to use defaults|

#### Options
The keys accepted by the `options` dictionary are below.

|Key       |Value         |Description|
|----------|--------------|-----------|
|`timeout` |integer       |Maximum time to wait (ms) to receive an ack|
|`filter`  |string        |Optional filter expression for this subscription|
|`bookmark`|string/symbol |Optional Bookmark to supply with this subscription|
|`options` |string        |Optional options to supply with this subscription|

See the AMPS documentation on ["Filtering Subscriptions by Content"](https://crankuptheamps.com/docs/amps-user-guide/pub-sub/content) for information on filtering.

Special bookmark values:

|Bookmark|Description|
|--------|-----------|
|`recent`|Start the subscription at the first undiscarded message in the bookmark|
|`0`     |Start the subscription at the beginning of the journal|
|`0\|1\|`|Start the subscription at the point in time when AMPS processes the subscription|

### Callback
The subscription callback function receives a single dictionary argument with the following keys:

|Key            |Value    |Description|
|---------------|---------|-----------|
|`bookmark`     |string   |`"client name hash\|sequence id\|"`|
|`data`         |byte list|Serialized data|
|`topic`        |string   |Topic data was received on|
|`subId`        |string   |Name of subscription received on|
|`bookmarkSeqNo`|long     |Bookmark sequence number|

## `discard`
Discard a message from the client's bookmark store

### Parameters
|Name           |Type         |Description|
|---------------|-------------|-----------|
|`handle`       |Foreign      |Reference to AMPS client|
|`subName`      |string/symbol|Name of subscription to discard from|
|`bookmarkSeqNo`|long         |The bookmark sequence number to discard (passed to the subscription callback)|

### Example
```q
upd:{[x]
    .kamps.discard[h;x[`subId];x[`bookmarkSeqNo]];
    0N!xx::x
    }

.kamps.subscribe[h;sub1;`messages;`upd;::]
```

## `unsubscribe`
Unsubscribes from a named subscription.

### Parameters
|Name     |Type         |Description|
|---------|-------------|-----------|
|`handle` |Foreign      |Reference to AMPS client|
|`subName`|string/symbol|Name to the subscription|
