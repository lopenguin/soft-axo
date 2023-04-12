file = open('save.csv')

msg = ''.join([line for line in file]).replace(' ', '').replace('\n', '')
acc = 0
for i in range(20000):
    if msg[i] != chr(97 + i % 26): acc += 1
    print(msg[i])

print(acc / 20000)