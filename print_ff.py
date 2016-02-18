import csv

a={}
with open('Mapping.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
    	a[row['key']] = row['value']

f = open('a','r')
token = ''
for line in f:
	key1 = line.split(':')[0]
	value = line.split(':')[1].strip()
	printstring = "FOLLOW (" +a[key1]+") = {"
	for i in range(len(value)):
		if i%2 ==1:
			token += value[i]
			printstring += a[token]+','
			token = ''
		else:
			token += value[i]
	printstring = printstring[:-1] 		
	printstring += '}'
	print printstring
