import csv

a={}

f = open('b','r')
token = ''
token_list={}
for line in f:
	key1 = line.split(':')[0]
	value = line.split(':')[1].strip()
	token_list[key1]=[]
	printstring = "" +key1+":"
	for i in range(len(value)):
		if i%2 ==1:
			token += value[i]
			
			if token not in token_list[key1]:
				token_list[key1].append(token)
				printstring += token
			token = ''
		else:
			token += value[i]
	print printstring
