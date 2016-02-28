import csv

a={}
with open('Mapping.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
    	a[row['key']] = row['value']

f = open('b','r')
token = ''
token_list={}
for line in f:
	key1 = line.split(':')[0]
	value = line.split(':')[1].strip()
	token_list[a[key1]]=[]
	printstring = "FOLLOW (" +a[key1]+") = {"
	for i in range(len(value)):
		if i%2 ==1:
			token += value[i]
			printstring += a[token]+','
			if a[token] not in token_list[a[key1]]:
				token_list[a[key1]].append(a[token])
			token = ''
		else:
			token += value[i]
	printstring = printstring[:-1] 		
	printstring += '}'

#print token_list.keys()
j=1
for k in sorted(token_list.keys()):
	if j<10:
		s = '0'+str(j)+ ") FOLLOW (" + k +") = {" 
	else:
		s = str(j)+ ") FOLLOW (" + k +") = {" 
	for i in token_list[k]:
		s += i +','
	if len(token_list[k])!=0:
		s = s[:-1]
	s += '}'
	print s
	j = j+1
