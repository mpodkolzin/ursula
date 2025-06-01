import grpc
import time
import logging
import random
import string
import statistics
import threading
from tqdm import tqdm
from concurrent.futures import ThreadPoolExecutor

import broker_pb2
import broker_pb2_grpc

# ==== Configuration ====
NUM_MESSAGES = 100
PAYLOAD_SIZE = 256
NUM_THREADS = 4
NUM_TOPICS = 10
KEYSPACE_SIZE = 100
GRPC_TARGET = 'localhost:50051'

# ==== Logging ====
logging.basicConfig(level=logging.INFO, format='%(asctime)s [%(levelname)s] %(message)s')

latencies = []
latencies_lock = threading.Lock()

def random_payload(size):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=size)).encode()

def random_topic():
    return f"topic_{random.randint(0, NUM_TOPICS - 1)}"

def random_key():
    return f"key_{random.randint(0, KEYSPACE_SIZE - 1)}"

def produce_messages(thread_id, num_messages, stub, progress_bar):
    for _ in range(num_messages):
        topic = random_topic()
        key = random_key()
        payload = random_payload(PAYLOAD_SIZE)
        request = broker_pb2.ProduceRequest(topic=topic, key=key, payload=payload)

        t0 = time.perf_counter()
        stub.Produce(request)
        t1 = time.perf_counter()

        latency_ms = (t1 - t0) * 1000
        with latencies_lock:
            latencies.append(latency_ms)
            progress_bar.update(1)

def main():
    messages_per_thread = NUM_MESSAGES // NUM_THREADS

    with tqdm(total=NUM_MESSAGES, desc="Producing") as progress_bar:
        with ThreadPoolExecutor(max_workers=NUM_THREADS) as executor:
            start_time = time.perf_counter()

            for i in range(NUM_THREADS):
                channel = grpc.insecure_channel(GRPC_TARGET)
                stub = broker_pb2_grpc.BrokerServiceStub(channel)
                executor.submit(produce_messages, i, messages_per_thread, stub, progress_bar)

            executor.shutdown(wait=True)
            end_time = time.perf_counter()

    total_time = end_time - start_time
    throughput = NUM_MESSAGES / total_time

    logging.info(f"Produced {NUM_MESSAGES} messages in {total_time:.2f}s")
    logging.info(f"Throughput: {throughput:.2f} messages/sec")
    logging.info(f"Avg latency: {statistics.mean(latencies):.2f} ms")
    logging.info(f"P95 latency: {statistics.quantiles(latencies, n=100)[94]:.2f} ms")
    logging.info(f"Max latency: {max(latencies):.2f} ms")

if __name__ == '__main__':
    main()
