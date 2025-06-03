import grpc
import time
import threading
import logging
import statistics
import random
from concurrent.futures import ThreadPoolExecutor
from tqdm import tqdm
import broker_pb2
import broker_pb2_grpc

logging.basicConfig(level=logging.INFO)

# Configuration
NUM_TOPICS = 10
NUM_PARTITIONS = 5  
NUM_THREADS = 4
MESSAGES_PER_THREAD = 10000

def random_topic():
    return f"topic_{random.randint(0, NUM_TOPICS - 1)}"

def random_partition():
    return random.randint(0, NUM_PARTITIONS - 1)

def consume_records(stub, thread_id, count, latencies, bar):
    topic = random_topic()
    partition_id = random_partition()
    group_id = f"group_{thread_id}"
    offset = 0
    logging.info(f"[Thread-{thread_id}] Starting consumption, {count} messages")


    try:
        stub.Subscribe(broker_pb2.SubscribeRequest(
            group_id=group_id,
            topic=topic
        ))
    except grpc.RpcError as e:
        logging.warning(f"[Thread-{thread_id}] Subscribe failed: {e}")


    for i in range(count):
        start = time.perf_counter()
        try:
            response = stub.Consume(broker_pb2.ConsumeRequest(
                topic=topic,
                group_id=group_id,
                partition_id=partition_id,
                offset=offset
            ))
            latency = (time.perf_counter() - start) * 1000
            latencies.append(latency)
            offset += 1
            if i % 100 == 0:
                #logging.info(f"[Thread-{thread_id}] Committing offset {offset} for topic {topic}, partition {partition_id}")
                commit_offset = stub.CommitOffset(broker_pb2.CommitOffsetRequest(
                    group_id=group_id,
                    topic=topic,
                    partition_id=partition_id,
                ))
            bar.update(1)
        except grpc.RpcError as e:
            #logging.warning(f"[Thread-{thread_id}] Consume failed: {e}")
            pass
        bar.update(1)


def stream_consume_records(stub, thread_id, count, latencies, bar):
    offset = 0 
    topic = random_topic()
    partition_id = random_partition()
    request = broker_pb2.ConsumeRequest(
        offset=offset,
        topic=topic,
        partition_id=partition_id
    )
    try:
        stream = stub.ConsumeStream(request)
        for i, response in enumerate(stream):
            start = time.perf_counter()
            latency = (time.perf_counter() - start) * 1000
            latencies.append(latency)
            bar.update(1)
            if i + 1 >= count:
                break
    except grpc.RpcError as e:
        logging.warning(f"Stream consume failed: {e}")



def main():
    channel = grpc.insecure_channel("localhost:50051")
    stub = broker_pb2_grpc.BrokerServiceStub(channel)

    all_latencies = []
    bars = [tqdm(total=MESSAGES_PER_THREAD, position=i, desc=f"Thread-{i}") for i in range(NUM_THREADS)]

    with ThreadPoolExecutor(max_workers=NUM_THREADS) as executor:
        futures = []
        for i in range(NUM_THREADS):
            latencies = []
            all_latencies.append(latencies)
            futures.append(executor.submit(consume_records, stub, i, MESSAGES_PER_THREAD, latencies, bars[i]))

        for f in futures:
            f.result()

    all_flat = [lat for l in all_latencies for lat in l]
    if all_flat:
        logging.info(f"Avg latency: {statistics.mean(all_flat):.2f} ms")
        logging.info(f"P95 latency: {statistics.quantiles(all_flat, n=100)[94]:.2f} ms")
    else:
        logging.warning("No successful reads recorded.")

if __name__ == "__main__":
    main()
