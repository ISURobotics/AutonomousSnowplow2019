import random
loc = open('practice_loc.txt', 'w')

temp = 50.0
x = 4.0
y = 4.0

for l in range(0, 1000):
	temp += random.uniform(-1.0, 1.0)
	x += (random.random() - 0.5) / 4.0

	y += (random.random() - 0.5) / 4.0

	loc.write(str(x) + ',' + str(y) + '\n')
	# f.write(str(temp) + '\n')