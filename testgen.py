import random
import struct
from sys import exit

test_count = 10000000
cache_size = 10

bottom_val = 0
low_val =  9
high_val = 9
chance = 0 #chance to generate number between 1 and high

data = []

low_count = 0
high_count = 0

for i in range(test_count):
    if random.randint(1, 100) >= chance:
        data.append(random.randint(bottom_val, low_val))
        low_count += 1
    else:
        data.append(random.randint(low_val+1, high_val))
        high_count += 1

unique = len(set(data))
print(f"low calls: {low_count}, high calls: {high_count}, unique: {unique}")

with open("test/data.bin", "bw") as f:
    for i in ([cache_size, test_count] + data):
        f.write(struct.pack('<i', i)) #i = int -> 4 bytes

with open("test/data.txt", "w") as f:
    for i in ([cache_size, test_count] + data):
        f.write(str(i) + '\n')


#идеальный кэш

# print(data)

exit()

size = 1000
cache_array = []

fast_ideal_cache_calls = 0
slow_ideal_cache_calls = 0
remove_cache_calls = 0

latest_val = 0

for i in range(len(data)):
    val = data[i]
    if(val in cache_array):
        fast_ideal_cache_calls += 1
    else:
        if len(cache_array) == size:
            # print("searched for:", val)
            # print("cache rn:", cache_array)
            # print("data slice:", data[(i+1):])
            max_entrance_distance = 0
            dist = 0
            for j in cache_array:
                dist =  data[(i+1):].index(j) if (j in data[(i+1):]) else test_count+1
                if dist >= max_entrance_distance:
                    max_entrance_distance = dist
                    latest_val = j
            
            # print(latest_val)            
            cache_array.remove(latest_val)
            remove_cache_calls += 1
    
        cache_array.append(val)
        slow_ideal_cache_calls += 1


print(f"in ideal cache\nfast calls:{fast_ideal_cache_calls}\nslow calls:{slow_ideal_cache_calls}\nremove calls:{remove_cache_calls}")
