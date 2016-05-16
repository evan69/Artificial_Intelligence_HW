import sys,re
if len(sys.argv) < 3:
	print "Usage:python cal_all.py [lower bound] [higher bound]"
begin = int(sys.argv[1])
end = int(sys.argv[2])
i = begin
win = 0
tot = 0
while i < end + 1:
	print "process result" + str(i) + ".txt..."
	file = open("result" + str(i) + ".txt","r")
	data = file.read()
	pattern = re.compile(r"ratio of A wins : (0|0\.5|1)\n")
	a = pattern.search(data)
	tot = tot + 1
	if a != None:
		
		if a.group(1) == r"0.5":
			win = win + 0.5
		else:
			win = win + float(a.group(1))
		print "result:" + a.group(1) + "\n"
	else:
		win = win + 1
		print "bug in this round\n"
	i = i + 2
	
print "\n" + str(win) + "/" + str(tot)
print "percentage:" + str(win / tot)