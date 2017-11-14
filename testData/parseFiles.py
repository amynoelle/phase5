import csv

names = ["CPUNiceSet","CPUNoNice","CPUvIO","CPUvIO1","CPUvIO2","IONiceSet","IONoNice"]

def parse(name):
	file = open(name+".txt","r")
	new = open(name+"NEW.txt","w+")

	for lines in file:
		if lines[0] == '*':
			new.write(lines)

	file.close()
	new.close()

def makeCleanData(name): #don't put .txt after name on invocation
	file = open(name+".txt","r")
	new = open(name+".csv","w+")
	new.write("TID, Nice, Tick\n")
	for lines in file:
		a,TID,c,Nice,e,Tick = lines.split(" ")
		
		TID = TID[:-1]
		Nice = Nice[:-1]

		new.write(TID+", "+Nice+", "+Tick)

	file.close()
	new.close()

def doAll():
	for x in names:
		parse(x)
		makeCleanData(x+"NEW")