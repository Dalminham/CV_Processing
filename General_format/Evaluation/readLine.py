
readpath = "./Array/s.txt"
writepath = "./Array/sdot.txt"

file = open(readpath)
data = file.readlines()
list = []
for line in data:
    list.append(int(line))

with open(writepath, 'w')as f:
    for i in range(len(list)):
        f.write(f"{list[i]}, ")
        if i%64 == 0:
            f.write('\n')
