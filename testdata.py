filename = "./test/015.dat"

arr = []

with open(filename, "r") as f:
    size = int(f.readline())
    print(size)
    count = int(f.readline())
    print(count)

    data = f.readline().split(' ')

    for i in range(count):
        arr.append(int(data[i]))
    
    print(len(set(arr)))

