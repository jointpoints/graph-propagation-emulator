from math import atan, cos, sin




# Signum function
sgn = lambda x : (x > 0) - (x < 0)





# Careful readline
def read_line(stream):
	if (line := stream.readline()[:-1]): 
		return line
	else:
		print('ERROR. Unable to read line from file.')
		quit()





# pi / 3
pi_3 = 3.14159265358979323846 / 3.0






# 1. Find out file to read the data from
file_opened = False
while not file_opened:
	file_name = input('Enter name of data file: ')
	try:
		file = open(file_name, 'r')
		file_opened = True
	except FileNotFoundError:
		print('ERROR. Specified file was not found, try again.')
	except Exception as error:
		print(error)

# Find out epsilon
epsilon = float(read_line(file))
# Find out the number of points
s = int(read_line(file))



# 2. Read points one by one and correct them on the fly
if (s < 3):
	raise NotImplementedError
point_rot_cart = [0.0] * (s - 1)
new_point_rot_cyln = [0.0] * (s - 1)
# Read the first point separately
x_prev = float(read_line(file))
sum_prev = 0.0
r_curr = 0.0
# Read the second point separately
x_curr = float(read_line(file))
sum_prev = -2**-.5 * x_prev
point_rot_cart[0] = sum_prev + .5**.5 * x_curr
x_prev = x_curr
r_curr = point_rot_cart[0]**2
# Read all other points
for x_i in range(2, s):
	# Read point
	x_curr = float(read_line(file))
	# NatCart --> RotCart
	sum_prev = ((x_i - 1) / (x_i + 1))**.5 * sum_prev - (x_i**2 + x_i)**-.5 * x_prev
	point_rot_cart[x_i - 1] = sum_prev + (x_i / (x_i + 1))**.5 * x_curr
	x_prev = x_curr
	# RotCart --> RotCyln
	new_point_rot_cyln[x_i - 2] = (sgn(point_rot_cart[x_i - 2]) * atan(r_curr**.5 / point_rot_cart[x_i - 1])) % pi_3
	r_curr = r_curr + point_rot_cart[x_i - 1]**2
file.close()



# 3. Make up transformed point
new_point_rot_cart = [0.0] * (s - 1)
new_point_nat_cart = [0.0] * s
r_curr = r_curr**.5
trig_curr = 1.0
for phi_i in range(s - 2):
	new_point_rot_cart[-phi_i - 1] = r_curr * trig_curr * cos(new_point_rot_cyln[-phi_i - 2])
	trig_curr *= sin(new_point_rot_cyln[-phi_i - 2])
new_point_rot_cart[0] = r_curr * trig_curr
sum_prev = 0.0
for n_i in range(s - 2):
	new_point_nat_cart[-n_i - 1] = sum_prev + ((s - n_i - 1) / (s - n_i))**.5 * new_point_rot_cart[-n_i - 1]
	sum_prev -= ((s - n_i - 1) * (s - n_i))**-.5 * new_point_rot_cart[-n_i - 1]
new_point_nat_cart[1] = sum_prev + .5**.5 * new_point_rot_cart[0]
new_point_nat_cart[0] = sum_prev - .5**.5 * new_point_rot_cart[0]



print('RotCart old :', point_rot_cart)
print('RotCyln new :', [r_curr] + new_point_rot_cyln[:-1])
print('RotCart new :', new_point_rot_cart)
print('NatCart new :', new_point_nat_cart)
