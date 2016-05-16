import sys,os
if len(sys.argv) < 3:
	print "Usage:python run_all.py [lower bound] [higher bound]"
begin = int(sys.argv[1])
end = int(sys.argv[2])
i = begin
while i <= end:
	command = "Compete Strategy.dll " + str(i) + ".dll time" + str(i) + ".txt 1 > result" + str(i) + ".txt";
	print command
	os.system(command)
	i = i + 2