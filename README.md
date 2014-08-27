# Bitcoin Server Protocol

[swansontec](https://github.com/swansontec), [pmienk](https://github.com/pmienk), [evoskuil](https://github.com/evoskuil)

8/22/2014

## Concepts

A client's two major areas of interest are transaction discovery and transaction maintenance. Transaction discovery involves searching the blockchain for transactions that are of interest to the wallet. Once the client identifies the transactions it cares about, it needs to track their state as they become broadcast, confirmed, and so forth. Doing this generally involves tracking the state of the blockchain itself.

## Goals

- Provide support for optimal implementation of common bitcoin clients.
  - [Full-Chain](https://en.bitcoin.it/wiki/Thin_Client_Security#Full-Chain_Clients)
  - [Header-Only](https://en.bitcoin.it/wiki/Thin_Client_Security#Header-Only_Clients) (SPV)
  - [Server-Trusting](https://en.bitcoin.it/wiki/Thin_Client_Security#Server-Trusting_Clients) (see below for details)
    - Caching
    - Stateless
- The server should not be required to maintain any session state.
- The client should not be required to provide any identifying information.
- The protocol should allow client privacy, leaving tradeoffs between privacy and performance to the caller.
- The protocol should be extensible while allowing backward and forward compatibility without version negotiation.
- The protocol should be defined in an IDL.
- Available IDL should provide tooling for generation of client-server stubs in C/C++.
- Available IDL tooling should implement marshalling in C/C++.

## Wire Encoding

The focus of this document is not the wire encoding, but the messaging semantics. The protocol may be encoded via any means. For example, it is possible to encode in [JSON](http://en.wikipedia.org/wiki/JSON) and serve up over [WebSockets](http://en.wikipedia.org/wiki/WebSocket). The initial libbitcoin implementation will likely encode using [Protocol Buffers](https://developers.google.com/protocol-buffers/docs/proto) and a [ZeroMQ](http://zeromq.org) transport with [privacy](http://curvezmq.org/), [compression](http://www.zlib.net) and support [onion routing](https://www.torproject.org). Additional protocols may be efficiently layered over ZMQ (e.g. using in-process communication).

## Principles
### Privacy
Transaction queries, all of which would otherwise carry [taintable](https://bitcointalk.org/index.php?topic=92416.0) information, use `prefix` filters. This allows a caller to select its desired level of privacy to prevent correlation of transactions with the caller's IP address, or with each other. Fewer bits give more privacy at the expense of efficiency. Prefixes are defined for a bitcoin `address`, `stealth` addresses and `transaction` hashes.

Note that without prefix filters transaction query taint cannot be avoided using onion routing alone unless each individual request is made on a distinct channel, as the requests can still be correlated to each other.

Send/Verify calls are inherently compromising as they allow correlation of the caller's IP address with the transaction. This can only be avoided using an onion router, such as [Tor](https://www.torproject.org) or [I2P](https://geti2p.net/en), to proxy the communication to the server.

Block header queries are not considered privacy-compromising with the exception that, without using opion routing, the caller exposes the calling IP address as hosting a bitcoin client.

### Pagination
All queries use a pagination scheme. The caller specifies an optional starting point and an optional target for the desired number of results per page. The server returns results in whole-block increments of increasing block height. The server always returns at least one block's worth of data (which may be an empty list if there is none to return) unless zero results per page is specified (in which case an empty list is returned).


### Block Correlation
The server always returns block height and hash as a `block_id` tuple to uniquely identify blocks. A caller may specify one, the other, or both of `block_id.hash` and `block_id.height`. The server validates whatever parts of `block_id`  are specified against the current chain and each other, returning an error if the request is off-chain.

Each paginated result contains the height and hash of the **next** block not included in the result set, which a client can use for chaining page requests. The last page always includes any matching transactions from the transaction memory pool and includes the **top** `block_id` instead of the **next** `block_id`.

If the **start** `block_id` is not specified the server returns results only from the memory pool. An unspecified page size allows the server to determine the page size, generally as large as practical. In the case of multiple query filters the server returns the union of results. An empty query returns all transactions.

The presence of the **next** `block_id.hash` allows the server to detect the presence of an apparent block fork between two page requests. The server will return an error, and the client can restart its queries from an earlier point. The server will always return results that are consistent with respect to the ending block hash.

The server signals the caller of a fork (or bad caller input input) by validating **start** `block_id` against the current chain, returning an error code if the specified `block_id` is not on the chain. There is no other possibility of pareseable input causing an error result (although server failures can produce errors). If a `tx_filter.bits` value that exceeds the length in bits of the corresponding `tx_filter.prefix` it is treated as a valid sentinel for "all bits" of the prefix.

## Complex Types

- libbitcoin::data_chunk bytes
- libbitcoin::hash_digest digest
- libbitcoin::block_header_type header
- libbitcoin::transaction_type tx
- enum {none | block | merkle} locations
- enum {hash | utxo | transaction} results
- enum {address | stealth | transaction} filters
- output
  - uint32_t **index**
  - uint64_t **satoshis**
  - bytes **script**
- block_id
  - uint32_t? **height** (default = unverified, use hash)
  - digest? **hash** (default = unverified, use height)
- block_location
  - block_id? **identity** (missing unless requested)
  - list of digest **branch** (empty unless requested)
- filter
  - filters **filter_type** (default = transaction)
  - uint32_t? **bits** (default = all)
  - bytes **prefix**
- tx_hash_result
  - digest **hash**
  - block_location **location**
- tx_result
  - tx **transaction**
  - block_location **location**
- utxo_result
  - digest **hash**
  - block_location **location**
  - list of output **outputs**

## Merkle Branch Encoding

The transaction's [Merkle branch](https://github.com/bitcoin/bips/blob/master/bip-0037.mediawiki#Partial_Merkle_branch_format) is encoded in `block_location.branch` as a hash list in depth-first order such that a properly-ordered combination with the hash of the corresponding transaction produces the [Merkle root](https://en.bitcoin.it/wiki/Protocol_specification#Merkle_Trees) of the corresponding block.

## Prefix Filters

In the case of filters the caller provides as prefix only the full or partial hash (byte-aligned). In other words, for addresses the prefix is against the RIPEMD160 hash payload, not the Base58Check encoding wrapper. Prefix comparisons for stealth addresses are independently documented. Address and transaction prefix comparisons are performed in a similar manner (TBD).

## Messages

### Blockchain

- Get Block Headers
  - in?:  block_id **start** (default = get block height)
  - in?:  uint32_t **results_per_page** (default = all, 0 = none)
  - out:  list of header **headers** (empty = zero page requested)
  - out?: block_id **next** (missing = last page)
  - out?: block_id **top** (missing = not last page)

### Transactions

- Get Transactions
  - in?:  block_id **start** (default = tx mempool only)
  - in?:  uint32_t **results_per_page** (default = all, 0 = start block only)
  - in?:  results **result_type** (default = hash)
  - in?:  locations **location_type** (default = none)
  - in:   list of filter **query** (empty = all)
  - out:  list of {tx_hash_result} **hashes**
  - out:  list of {utxo_result} **outputs**
  - out:  list of {tx_result} **transactions**
  - out?: block_id **next** (missing = last page)
  - out?: block_id **top** (missing = not last page)

### Broadcast

- Validate Transaction (primarily for client-side debugging)
  - in:   tx **transaction**
- Send Transaction
  - in:   tx **transaction**

## Usage Examples

Get the current block height, and nothing more:
```
get_headers {}
```
Determine if a particular block is still on the main chain:
```
get_headers
{
  start = 
  {
    height = 317792, 
    hash = 0x000000000000000018b01e93c7caaed765b7ff478f2dcc7ae6364bfcf97fe2f8
  },
  results_per_page = 0
}
```
Get all block headers, starting from the genesis block:
```
get_headers
{
  start = { height = 0 }
}
```
Get all block headers, starting where the previous query left off:
```
get_headers
{
  start = 
  {
    height = 317792, 
    hash = 0x000000000000000018b01e93c7caaed765b7ff478f2dcc7ae6364bfcf97fe2f8
  }
}
```
Get all transaction hashes for a wallet with two addresses, starting at the genesis block, with a target page size of 10 transactions:
```
get_transactions
{
  start = { height = 0 },
  filter = 
  [
    { prefix = 0x21 },
    { bits = 12, prefix = 0x08b7 }
  ],
  results_per_page = 10
}
```
Get all transaction data for a wallet with two addresses, starting at a particular block:
```
get_transactions
{
  start = 
  {
    height = 317792, 
    hash = 0x000000000000000018b01e93c7caaed765b7ff478f2dcc7ae6364bfcf97fe2f8
  },
  filter =
  [
    { prefix = 0x21 },
    { bits = 12, prefix = 0x08b7 }
  ],
  result_type = transaction
}
```
Get all utxo's for a single address:
```
get_transactions
{
  start = { height = 0 },
  query = [{ bits = 12, prefix = 0x08b7 }],
  result_type = utxo
}
```
Has my transaction been confirmed yet?
```
get_transactions
{
  start = { height = 0 },
  query = 
  [
    {
      filter_type = address, 
      prefix = 0x94b43df27e205d8a261531fe1fc0c2e5fc226a87e6a9e1c68ab9113eb36cbf4a 
    }
  ],
  result_type = hash
  location_type = block
}
```
Get all the transactions in a particular block:
```
get_transactions
{
  start = 
  {
    height = 317792, 
    hash = 0x000000000000000018b01e93c7caaed765b7ff478f2dcc7ae6364bfcf97fe2f8
  },
  results_per_page = 0
}
```
Get all stealth transactions in the mempool:
```
get_transactions
{
  query = [{ filter_type = stealth }]
}
```

## Client Types

### This protocol supports four common client types:

- Full-chain caching clients
- SPV caching clients
- Server-trusting caching clients
- Server-trusting stateless clients

Caching clients store transactions locally, while stateless clients do not (they might store addresses or other metadata). Aside from reduced user interface latency and enabling verification, maintaining a local transaction database allows maintenance of per-transaction non-blockchain meta-data, such as categories, counterparty names, etc.

SPV and full-chain clients can verify the contents of their transaction database by checking transaction hashes against block headers. Additionally, full-chain clients can verify that inputs connect to outputs. Server-trusting clients don't maintain the blockchain, so a malicious server can easily convince them that they have received non-existent payments. With basic precautions non-trusting clients are immune to this class of attack.

Server-trusting caching clients are doubly-sensitive to attack, since they may receive and cache invalid data locally where it would persist until the cache is refreshed or a possibly if a blockchain fork is signalled. Stateless clients are somewhat less sensitive, since they will recover as soon as they connect to an honest server.

There are realistic scenarios where a trusting client makes sense. For example, an e-commerce platform might run its own internal bitcoin servers. In this case there is no need to verify data coming from the same security zone.

All caching client types stay up-to-date with the blockchain in a similar way. The client periodically polls for new blocks. Upon discovering a new block the client probes its cache for a chain fork. To do this, the client walks down the chain from its highest block, making server queries with the **start** `block_id` set to blocks along the chain. Each block that produces a failure response is identified as being on a pruned fork. Once the highest non-forked block in the cache is located the client queries the server in order to update the block information for all transactions (or addresses) that are associated with pruned blocks.

Although SPV clients can verify information the server provides, only a full-chain client can detect missing transactions. Therefore SPV clients may still choose to periodically re-scan their addresses starting from the genesis block. Trusting clients have no reason to assume that new data would be any more reliable than old data, although a periodic update can mitigate perpetuating errors, as previously described.

## Old Obelisk Protocol

The old obelisk protocol is included for comparison:

### Blockchain

- fetch_last_height()
- fetch_block_header(height)
- fetch_block_header(hash)

### Transactions

- fetch_history(const payment_address& address, size_t from_height=0)
- fetch_stealth(const stealth_prefix& prefix, size_t from_height=0)
- fetch_transaction_index(const hash_digest& tx_hash)
- fetch_transaction(const hash_digest& tx_hash)
- fetch_unconfirmed_transaction(const hash_digest& tx_hash)
- fetch_history(const payment_address& address, size_t from_height=0)
- subscribe(const address_prefix& prefix)

### Broadcast

- validate(tx_data)
- broadcast(tx_data)

These features are a subset of new protocol with the exception of address.subscribe, which has been removed. The subscription required per caller session-state to be maintained by the server. The new protocol is stateless and therefore requires callers to poll for the same information.
