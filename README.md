# Bitcoin Server Interface Design

s_tec, pmienk, evoskuil

8/22/2014

## Concepts

The client's two major areas of interest are transaction discovery and transaction maintenance. Transaction discovery involves searching the blockchain for transactions that are of interest to the wallet. Once the client identifies the transactions it cares about, it needs to track their state as they become broadcast, confirmed, and so forth. Doing this generally involves tracking the state of the blockchain itself.

## Goals

- Provide support for optimal implementation of common bitcoin clients.
  - Full-Chain
  - Header-Only (SPV)
  - Server-Trusting (see below for details)
    - Caching
    - Stateless
- The server should not be required to maintain any session state.
- The client should not be required to provide any identifying information.
- The protocol should allow client privacy to the extent possible, leaving tradeoffs between privacy and performance to the caller.
- The protocol should be extensible while allowing backward and forward compatibility without version negotiation.
- The protocol should be defined in an IDL.
- Available IDL should provide tooling for generation of client-server stubs in C/C++.
- Available IDL tooling should implement marshalling in C/C++.

## Encoding

The focus of this document is not the wire encoding, but the messaging semantics. The protocol may be encoded via any means. For example, it is possible to encode in JSON, served over WebSockets. The initial implementation will likely encode using Google Protocol Buffers, using ZeroMQ transport with privacy and compression. Additional protocols may be efficiently layered over ZMQ (e.g. using in-process communication).

## Principles

All queries involving sensitive data use prefixes. This way, the client can select its desired level of privacy. Fewer bits give more privacy at the expense of efficiency. Prefixes are anticipated for bitcoin addresses, stealth addresses and transaction hashes.

The server always returns the block height and hash together. Both are useful for different reasons, and returning either one by itself would be a complicating denormalization.

All data requests use a pagination scheme. The client specifies an optional starting point and an optional hint indicating the number of desired results per page. The server returns results in whole-block increments of increasing block height, and always returns at least one block's worth of data (which may be an empty list if there is none to return).

Each paginated result contains the height and hash of the next-higher block, which the client can use for its next query. This will be some block above the last result in the output list.

If the starting point is missing, the query only includes the mempool. A missing hint means "return as many results as possible." If there are multiple items in the filter list, the server returns the union of the results. An empty filter list returns all transactions.

The presence of the next-block hash allows the server to detect the presence of an apparent block fork between two page requests. The server will return an error, and the client can restart its queries from an earlier point. The server will always return results that are consistent with respect to the ending block hash.

In addition, each result includes the height and hash of the current highest block. This way, clients can determine how fresh their query results are.

An error in the starting point (non-existent block height or non-matching block hash) results in an error message. There is no other possibility of an invalid yet parseable client message in the protocol. A tx_filter.bits value that exceeds the length in bits of the tx_filter.prefix is treated as a valid sentinel for "all bits" of the prefix.

## Complex Types

- header (see libbitcoin block_header_type)
- tx (see libbitcoin transaction_type)
- utput
  - uint32_t index
  - uint64_t satoshis
  - data_chunk script
- block_id
  - uint32_t? height (default = unverified, use hash)
  - uint8_t[32]? hash (default = unverified, use height)
- block_location
  - block_id? identity (missing unless requested)
  - list of uint8_t[32]? branch (missing unless requested)
- tx_filter
  - enum uint8_t context {tx | address | stealth} (default=tx)
  - uint32_t? bits (default = all)
  - data_chunk prefix
- tx_hash_result
  - uint8_t[32] tx_hash
  - block_location location
- tx_result
  - tx transaction
  - block_location location
- utxo_result
  - uint8_t[32] tx_hash
  - block_location location
  - list of output

## Merkle Branch Encoding

block_location.branch is a hash list in depth-first order such that a properly-ordered combination with the hash of the corresponding transaction produces the Merkle root of the corresponding block.

## Prefix Filters

In the case of filters the caller provides as prefix only the full or partial hash (byte-aligned). In other words, for addresses the prefix is against the RIPEMD160 hash payload, not the Base58Check encoding wrapper. Prefix comparisons for stealth addresses are independently documented. Address and transaction prefix comparisons are performed in a similar manner (TBD).

## Messages

### Blockchain

- Get block headers
  - in?:  start block_id (default = get block height)
  - in?:  results_per_page uint32_t (hint, default = all)
  - ut:   list of header
  - ut?:  next block_id (missing = no more blockchain results)
  - ut?:  top block_id (only included on the last page)

### Transactions

- Get transactions
  - in?:  start block_id (default = tx mempool only)
  - in?:  results_per_page uint32_t (hint, default = all)
  - in:   list of tx_filter (empty = all)
  - in?:  enum result-type {tx_hash | tx_data | utxo} (default = tx_hash)
  - in?:  enum location-format {none | block | merkle} (default = none)
  - ut:   list of {tx_hash_result | tx_result | utxo_result}
  - ut?:  next block_id (missing = last page)
  - ut?:  top block_id (missing = not last page)

### Broadcast

- Validate transaction (primarily for client-side debugging)
  - in:   tx
- Send transaction
  - in:   tx

## Client Types

### This protocol targets four major client types:

- Full-chain caching clients
- SPV caching clients
- Server-trusting caching clients
- Server-trusting stateless clients

Caching clients store transactions locally, while stateless clients do not (they might store addresses or other things, though). Besides increasing speed and enabling verification, maintaining a local transaction database creates a place to store per-transaction non-blockchain meta-data, such as categories, payee names, and so forth.

SPV and full-chain clients can verify the contents of their transaction database by checking transaction hashes against block headers. Additionally, full-chain clients can verify that inputs connect to outputs. Server-trusting clients don't maintain any of this information, so a malicious server can easily convince them that they have received non-existent payments. Non-trusting clients are immune to these attacks.

Server-trusting caching clients are doubly-sensitive to attack, since they will happily store the incorrect data in their local database where it might persist for a long time. Stateless clients are somewhat less sensitive, since they will recover as soon as they connect to an honest server.

There are some realistic scenarios where a trusting client makes sense. For example, an e-commerce platform might run its own internal obelisk servers. In this case, there is no need to verify data coming from the same security zone.

All caching client types stay up-to-date with the blockchain in a similar way. The client runs a periodic polling process that checks for new blocks. Upon discovering a new block, the polling process probes for chain forks. To do this, the polling process works backwards from the newest block, making queries with the starting block_id set to known points along the chain. A fork exists as long as these queries fail. Once this is done, the caching client re-queries any transactions or addresses, starting from the highest level it is still synced to.

Although SPV clients can verify any information the server provides, they cannot detect missing data. Therefore, SPV clients may still choose to periodically re-scan their addresses starting from the genesis block. Full-chain clients don't need this re-scan, and trusting clients have no reason to assume that new data would be any more reliable than old data.

## Usage Examples

Get the current block height, and nothing more:
```
get_headers {}
```
Determine if a particular block is still on the main chain:
```
get_headers
{
  start = { height, hash }
  results_per_page = 0
}
```
Download as many block headers as possible, starting from the genesis block:
```
get_headers
{
  start = { height = 0 }
}
```
Download as many block headers as possible, starting where the previous query left off:
```
get_headers
{
  start = { height, hash }
}
```
Get all transaction hashes for a wallet with two addresses, starting at the genesis block:
```
get_transactions
{
  start = { height = 0 }
  filter = 
    [
      { context = address, prefix = 0x21 }, 
      { context = address, bits = 12, prefix = 0x08b7 }
    ]
}
```
Get all transaction data for a wallet with two addresses, starting at a particular block:
```
get_transactions
{
  start = { height, hash }
  filter =
  [
    { context = address, prefix = 0x21 }, 
    { context = address, bits = 12, prefix = 0x08b7 }
  ]
  result_type = tx_data
}
```
Get all utxo's for a single address:
```
get_transactions
{
  start = { height = 0 }
  filter = [{ context = address, bits = 12, prefix = 0x08b7 }]
  result_type = utxo
}
```
Has my transaction been confirmed yet?
```
get_transactions
{
  start = { height = 0 }
  filter = 
    [{ prefix = 0x94b43df27e205d8a261531fe1fc0c2e5fc226a87e6a9e1c68ab9113eb36cbf4a }]
  result_type = tx_hash
  location_format = block_id
}
```
Get all the transactions in a particular block:
```
get_transactions
{
  start = { height, hash }
  results_per_page = 0
}
```
Get all stealth transactions in the mempool:
```
get_transactions
{
  filter = [{ context = stealth }]
}
```
## Old Obelisk Protocol

Here is the old obelisk protocol for comparison:

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

All these features are more than covered by the new protocol. The only capability the new protocol removes is address.subscribe, but this requires session-state on the server. Clients can poll for the same information in any case.
