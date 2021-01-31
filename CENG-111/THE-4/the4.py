def datum(x):
  return x[0]

def children(x):
  return x[1:]

def isleaf(tree):
  return type(tree[1]) != list

def cycle(q):
    d = []
    for i in range(len(q)):
        if i != 0:
            if q[i].isdigit():
                if q[i-1].isdigit():
                    d.append(int(q[i-1]+q[i]))
                else:
                    d.append(int(q[i]))
        else:
            if q[i].isdigit():
                d.append(int(q[i]))
    return max(d)

def ana(tree,current,valve_order):
	if '2' in str(tree) or '3' in str(tree) or '4' in str(tree) or '5' in str(tree) or '6' in str(tree) or '7' in str(tree) or '8' in str(tree) or '9' in str(tree) or '10' in str(tree) or '00' in str(tree) or '01' in str(tree) or '11' in str(tree):
		for j in children(tree):
			if isleaf(j):
				if j[1] == 1:
					tree.remove(j)
					current.append(j[0])
				else:
					j[1] -= 1
			else:
				ana(j,current,valve_order)
	else:
		if not "'"+datum(tree)+"'"in str(valve_order):
			current.append(datum(tree))
		
def chalchiuhtlicue(tree):
	valve_order = []
	current = []
	if type(tree[1]) == list:
		for i in range(cycle(str(tree))):
			current = []
			ana(tree,current,valve_order)
			valve_order.append(current)
		return valve_order
	else:
		if tree[1] == 1:
			return [[datum(tree)]]
		else:
			for i in range(tree[1]-1):
				valve_order.append([])
			valve_order.append([tree[0]])
			return valve_order
