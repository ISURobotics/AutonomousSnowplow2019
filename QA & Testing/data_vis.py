import matplotlib.pyplot as plt
import math

def display_orientation():
	orientation_file = open('orientation.txt', 'r')
	orientations = []

	for line in orientation_file:
		orientations.append(float(line))

	plt.subplot(223)
	plt.plot(orientations)
	plt.title('heading')
	plt.grid(True)

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
	distances = []
	x_prev = 0
	y_prev = 0
	for line in location_file:
		position = line.split(',')
		x.append(float(position[0]))
		y.append(float(position[1][:len(position[1]) - 1]))
		# print("x: " + position[0] + " y: " + position[1], end="")

		distance_from_last_point = math.sqrt((float(position[0]) - x_prev)**2 + (float(position[1][:len(position[1]) - 1]) - y_prev)**2)
		# print(str(distance_from_last_point * 100))
		x_prev = float(position[0])
		y_prev = float(position[1][:len(position[1]) - 1])
		distances.append(distance_from_last_point * 100)

	plt.subplot(221)
	plt.axis('equal')
	plt.plot(x, y)
	plt.title('location')
	plt.grid(True)


def main():
	plt.figure(1)
	display_location()
	display_temperature()
	display_orientation()
	plt.show()

if __name__ == "__main__":
	main()