import sys
import copy

def getmap():
	with open(sys.argv[1],'r') as f:
		return f.readlines()

def rules():
	with open(sys.argv[2],'r') as d:
		return d.readlines()

def check_neighbours(i,j,current_map,width,length):
	neighbour = 0
	if width == 1 and length == 1:
		return neighbour	

	elif width == 1 and length >= 2:
		if length == 2:
			if i == 0:
				if current_map[i+1][j] == '*':
					neighbour += 1
			else:
				if current_map[i-1][j] == '*':
					neighbour += 1
		elif length > 2:
			if i == 0:
				if current_map[i+1][j] == '*':
					neighbour += 1
			elif i == length-1:
				if current_map[i-1][j] == '*':
					neighbour += 1
			else:
				if current_map[i+1][j] == '*':
					neighbour += 1
				if current_map[i-1][j] == '*':
					neighbour += 1
	elif length == 1 and width >= 2:
		if width == 2:
			if j == 0:
				if current_map[i][j+1] == '*':
					neighbour += 1
			else:
				if current_map[i][j-1] == '*':
					neighbour += 1
		elif width > 2:
			if j == 0:
				if current_map[i][j+1] == '*':
					neighbour += 1
			elif j == width-1:
				if current_map[i][j-1] == '*':
					neighbour += 1
			else:
				if current_map[i][j+1] == '*':
					neighbour += 1
				if current_map[i][j-1] == '*':
					neighbour += 1
	else:
		if i > 0  and i < length-1 and j > 0 and j < width-1:
			 if current_map[i-1][j-1] == '*':
				neighbour += 1
			 if current_map[i-1][j] == '*':
				neighbour += 1
			 if current_map[i-1][j+1] == '*':
				neighbour += 1
			 if current_map[i][j-1] == '*':
				neighbour += 1
			 if current_map[i][j+1] == '*':
				neighbour += 1
			 if current_map[i+1][j-1] == '*':
				neighbour += 1
			 if current_map[i+1][j] == '*':
				neighbour += 1
			 if current_map[i+1][j+1] == '*':
				neighbour += 1

		elif i == 0 and j > 0 and j < width-1:
			 if current_map[i][j-1] == '*':
				neighbour += 1
			 if current_map[i][j+1] == '*':
				neighbour += 1
			 if current_map[i+1][j-1] == '*':
				neighbour += 1
			 if current_map[i+1][j] == '*':
				neighbour += 1
			 if current_map[i+1][j+1] == '*':
				neighbour += 1
		elif j == width-1 and i > 0 and i < length-1:
			 if current_map[i-1][j-1] == '*':
				neighbour += 1
			 if current_map[i-1][j] == '*':
				neighbour += 1
			 if current_map[i][j-1] == '*':
				neighbour += 1
			 if current_map[i+1][j-1] == '*':
				neighbour += 1
			 if current_map[i+1][j] == '*':
				neighbour += 1
		elif j == 0 and i < length-1 and i > 0:
			 if current_map[i-1][j] == '*':
				neighbour += 1
			 if current_map[i-1][j+1] == '*':
				neighbour += 1
			 if current_map[i][j+1] == '*':
				neighbour += 1
			 if current_map[i+1][j] == '*':
				neighbour += 1
			 if current_map[i+1][j+1] == '*':
				neighbour += 1
		elif i == length-1 and j > 0 and j < width-1:
			 if current_map[i-1][j-1] == '*':
				neighbour += 1
			 if current_map[i-1][j] == '*':
				neighbour += 1
			 if current_map[i-1][j+1] == '*':
				neighbour += 1
			 if current_map[i][j-1] == '*':
				neighbour += 1
			 if current_map[i][j+1] == '*':
				neighbour += 1
	
		elif i == 0 and j == width-1:
			 if current_map[i][j-1] == '*':
				neighbour += 1
			 if current_map[i+1][j-1] == '*':
				neighbour += 1
			 if current_map[i+1][j] == '*':
				neighbour += 1
		elif i == 0 and j == 0:
			 if current_map[i][j+1] == '*':
				neighbour += 1
			 if current_map[i+1][j] == '*':
				neighbour += 1
			 if current_map[i+1][j+1] == '*':
				neighbour += 1
		elif i == length-1 and j == width-1 :
			 if current_map[i-1][j-1] == '*':
				neighbour += 1
			 if current_map[i-1][j] == '*':
				neighbour += 1
			 if current_map[i][j-1] == '*':
				neighbour += 1
		elif i == length-1 and j == 0:
			 if current_map[i-1][j] == '*':
				neighbour += 1
			 if current_map[i-1][j+1] == '*':
				neighbour += 1
			 if current_map[i][j+1] == '*':
				neighbour += 1

	return neighbour
	
def getrules(rulles,current_map,width,length):
	a = 0
	number_of_generation = int(sys.argv[3])
	while a < number_of_generation:
		cop = copy.deepcopy(current_map)
		for i in range(length):
			for j in range(width):
				for k in rulles:
					number = int(k[2])
					if current_map[i][j] == '*':
						if k[0] == '*':
							if k[1] == '<':
								if check_neighbours(i,j,current_map,width,length) < number:
									cop[i] = cop[i][:j] + k[3] + cop[i][j+1:]
									break
							elif k[1] == '=':
								if check_neighbours(i,j,current_map,width,length) == number:
									cop[i] = cop[i][:j] + k[3] + cop[i][j+1:]
									break
							else:
								if check_neighbours(i,j,current_map,width,length) > number:
									cop[i] = cop[i][:j] + k[3] + cop[i][j+1:]
									break
						else:
							continue
					elif current_map[i][j] == '-':
						if k[0] == '-':
							if k[1] == '<':
								if check_neighbours(i,j,current_map,width,length) < number:
									cop[i] = cop[i][:j] + k[3] + cop[i][j+1:]
									break
							elif k[1] == '=':
								if check_neighbours(i,j,current_map,width,length) == number:
									cop[i] = cop[i][:j] + k[3] + cop[i][j+1:]
									break
							else:
								if check_neighbours(i,j,current_map,width,length) > number:
									cop[i] = cop[i][:j] + k[3] + cop[i][j+1:]
									break
						else:
							continue
		current_map = cop				
		a += 1
	print ''.join(current_map)
						
def main():
	rulles = rules()
	current_map = getmap()
	width = len((open(sys.argv[1]).readlines()[0]).rstrip('\n'))
	length = len(open(sys.argv[1]).readlines())
	getrules(rulles,current_map,width,length)
	
if __name__ == '__main__':
    main()
