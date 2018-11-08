import matplotlib.pyplot as plt

def display_temperature():
	temperature_file = open('temperature.txt', 'r')
	temps = []

	for line in temperature_file:
		temps.append(float(line))

	plt.subplot(222)
	plt.plot(temps)
	plt.title('temperature')
	plt.grid(True)

def display_location():
	# open the generated location file, print pos 
	location_file = open('position.txt', 'r')
	x = []
	y = []

	for line in location_file:
		position = line.split(',')
		x.append(float(position[0]))
		y.append(float(position[1][:len(position[1]) - 1]))
		# print("x: " + position[0] + " y: " + position[1], end="")

	plt.subplot(221)
	plt.plot(x, y)
	plt.title('location')
	plt.grid(True)


def main():
	plt.figure(1)
	display_location()
	display_temperature()
	plt.show()

if __name__ == "__main__":
	main()