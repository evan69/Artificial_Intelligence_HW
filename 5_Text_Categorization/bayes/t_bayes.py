# coding:utf8
# by Evan Hou (2014011433)
import os,sys,re,collections,math,random,csv

argc = len(sys.argv)

c_name = ["c1_atheism","c2_sci.crypt","c3_talk.politics.guns","c4_comp.sys.mac.hardware"]
c_list = ["/data/c1_atheism/","/data/c2_sci.crypt/","/data/c3_talk.politics.guns/","/data/c4_comp.sys.mac.hardware/"]

S = 10
if argc == 2:
	S = 1
divide_sets = []
for i in range(S):
	divide_sets.append(set())

datapat = re.compile(r'[^a-z|A-Z]')
vocab = set()
cur = os.getcwd()

outdata = []
csvfile = open("output.csv","wb")

def divide():
	if S == 1:
		return
	global cur,c_name,divide_sets
	for c in c_name:  
		file_list = os.listdir(cur + '/data/' + c + '/')
		for file in file_list:
			rand_num = random.randint(0,S-1)
			divide_sets[rand_num].add((c,file))
		
def learn(index):
	global c_name,datapat,vocab,cur,outdata
	outdata.append(("training set",''))
	outdata.append(("ID","label"))
	
	c_text_k = dict()
	pc = dict()
	wjck = dict()
	vocab = set()
	num = 0
	alldata = ''
	for c in c_name:  
		file_list = os.listdir(cur + '/data/' + c + '/')
		num = num + len(file_list) #number of each kind of files
		c_text_k[c] = ''
		for file in file_list: #for each file
			if (c,file) in divide_sets[index]:#learn except index
				continue
			outdata.append((file,c))
			fileobj = open(cur + '/data/' + c + '/' + file,'r')
			essay = fileobj.read()
			#c_essay[c].append(essay)
			c_text_k[c] = c_text_k[c] + essay
			fileobj.close()
		alldata = alldata + c_text_k[c]
		#combine all files
		c_text_k[c] = datapat.sub(' ',c_text_k[c]).split(' ')
		#devide essay into words
		
	alldata = datapat.sub(' ',alldata)
	vocab = set(alldata.split(' '))
	vocab.remove('')
	#convert to set and remove empty string
	len_vocab = len(vocab)

	for c in c_name:
		raw_data = ''
		pc[c] = 1.0 * len(file_list) / num;
		n = 0
		nj = collections.defaultdict(lambda:0)
		for item in c_text_k[c]:
			if item != '':
				n = n + 1
				nj[item] = nj[item] + 1
		for word in vocab:
			wjck[(word,c)] = 1.0 * (nj[word] + 1) / (n + len_vocab)
			
	return (pc,wjck)
	
def classify_text(doc):
	global c_name,datapat,vocab,cur
	doc = datapat.sub(' ',doc).split(' ')
	val = collections.defaultdict(lambda:1)
	maxval = -100000000.0
	maxc = 'init'
	for c in c_name:
		val[c] = math.log(pc[c])
		for word in doc:
			if word in vocab:
				val[c] = val[c] + math.log(wjck[(word,c)])
		if val[c] > maxval:
			maxval = val[c]
			maxc = c
	return maxc

divide()
if argc == 1:
	percentage = collections.defaultdict(lambda:[0,0,0.0])
	for i in range(S):
		print "round " + str(i)
		outdata.append(())
		outdata.append(("test " + str(i) + "th time",''))
		pc,wjck = learn(i)
		outdata.append(())
		outdata.append(("testing set",''))
		outdata.append(("ID","answer","result"))
		for c in c_name:
			tot = 0
			right = 0
			cur = os.getcwd()
			file_list = os.listdir(cur + '/data/' + c + '/')
			for file in file_list:
				if (c,file) in divide_sets[i]:
					percentage[c][0] = percentage[c][0] + 1
					fileobj = open(cur + '/data/' + c + '/' + file,'r')
					essay = fileobj.read()
					fileobj.close()
					result = classify_text(essay)
					#print "result:" + classify_text(essay) + " answer:" + c
					if c == result:
						percentage[c][1] = percentage[c][1] + 1
					outdata.append((file,c,result))
					
	for c in c_name:
		percentage[c][2] = 100.0 * percentage[c][1] / percentage[c][0]
		print c,str(percentage[c][2]) + '%'

else:
	pc,wjck = learn(0)
	fileobj = open(sys.argv[1],'r')
	essay = fileobj.read()
	result = classify_text(essay)
	fileobj.close()
	print result
	
#output
writer = csv.writer(csvfile)
writer.writerows(outdata)
csvfile.close()
	