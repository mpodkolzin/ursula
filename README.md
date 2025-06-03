# Ursula Message Queue

**Ursula** is a high-performance, Kafka-like distributed message queue written in C++. It supports topic-partitioned logs, consumer groups, offset management, and gRPC + REST APIs.

## Features

- ✅ Topic and partition-based message storage  
- ✅ Log-structured storage with segmented files  
- ✅ Asynchronous writes with `PartitionWorker` threads  
- ✅ gRPC interface for produce/consume  
- ✅ Streaming consume support for high-throughput readers  
- ✅ Consumer group abstraction with offset management  
- ✅ Persistent offset storage via pluggable `ConsumerOffsetStore`  
- ✅ File-backed offset store (`FileOffsetStore`)  
- ✅ Multithreaded benchmark producer and consumer (Python)  
- ✅ REST API server (basic setup)

---

## Project Structure

```
ursula/
├── include/               # Public headers (Broker, Partition, ConsumerGroup, etc.)
├── src/                   # Implementation files
│   ├── broker/            # Core broker logic
│   ├── producer/          # PartitionWorker, async producer flow
│   ├── server/            # gRPC + REST servers
│   ├── offset_store/      # Offset persistence backends
├── proto/                 # Protobuf service definition (broker.proto)
├── tools/                 # Benchmark clients (Python gRPC producer/consumer)
├── tests/                 # Unit and integration tests
├── CMakeLists.txt         # Project configuration
└── README.md              # You're here!
```

---

## Usage

### Build

```bash
mkdir build && cd build
cmake ..
make -j
```

### Run Broker

```bash
./bin/ursula-server
```

### Python Benchmark Producer

```bash
cd tools/
python3 grpc_producer.py
```

### Python Benchmark Consumer

```bash
python3 grpc_consumer.py
```

---

## gRPC API

Defined in `proto/broker.proto`, including:

- `Produce(topic, key, payload)`
- `Consume(topic, partition, offset)`
- `ConsumeStream(...)`
- `CommitOffset(group_id, topic, partition, offset)`
- `GetCommittedOffset(group_id, topic, partition)`
- `Subscribe(group_id, topic)` *(optional enhancement)*

---

## Consumer Groups

- Created on-demand using `ConsumerGroupManager`
- Tracks committed offsets per `(group, topic, partition)`
- Offset persistence backed by `ConsumerOffsetStore` (e.g. `FileOffsetStore`)
- Polling updates in-memory offset and returns latest record
- Commit writes latest offset to store

---

## TODO

- [ ] Add support for consumer rebalancing / partition assignment  
- [ ] Improve durability and crash recovery  
- [ ] Add RocksDB-backed offset store  
- [ ] Advanced metrics and observability  
- [ ] REST API completion  

---

## License

MIT License. See `LICENSE` file.
