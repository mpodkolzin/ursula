import requests
import json
import random
import string
import time

URL = "http://localhost:8080/offset"
TOPICS = ["topic1", "topic2"]
MESSAGES_PER_TOPIC = 1000

def random_string(length=16):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

payloads = []

for topic in TOPICS:
    for i in range(MESSAGES_PER_TOPIC):
        payloads.append({
            "key": f"{topic}-key-{i%100}",
            "topic": topic,
            "offset": i
        })

start_time = time.time()
success = 0

for payload in payloads:
    try:
        print(payload)
        response = requests.post(URL, data=json.dumps(payload), timeout=2)
        if response.status_code == 200:
            success += 1
            print(response.json()["payload"])
        else:
            print(response.json())
            print(response.status_code)
            
        
    except requests.exceptions.RequestException as e:
        print(f"Request failed: {e}")

end_time = time.time()
duration = end_time - start_time
rate = success / duration if duration > 0 else 0

print(f"\n✅ Consumed {success} messages in {duration:.2f} seconds")
print(f"⚡ Consumption rate: {rate:.2f} messages/sec")