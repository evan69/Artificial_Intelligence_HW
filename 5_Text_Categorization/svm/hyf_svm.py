# coding:utf8
# by Evan Hou (2014011433)
import os,sys

#import libsvm.python.svmutil
import re,collections,math,random,csv

argc = len(sys.argv)

c_name = ["c1_atheism","c2_sci.crypt","c3_talk.politics.guns","c4_comp.sys.mac.hardware"]
c_list = ["/data/c1_atheism/","/data/c2_sci.crypt/","/data/c3_talk.politics.guns/","/data/c4_comp.sys.mac.hardware/"]

S = 10
#if argc == 2:
#	S = 1
divide_sets = [] #contain S sets, each of the set contains kind and ID of the file
for i in range(10):
	divide_sets.append(set())

datapat = re.compile(r'[^a-z|A-Z]')
cur = os.getcwd()
num = 0
ID = [[],[],[],[],[],[],[],[],[],[]]
answer = [[],[],[],[],[],[],[],[],[],[]]
result = [[],[],[],[],[],[],[],[],[],[]]

outdata = [[],[]]
csvout = []
csvfile = open("output.csv","wb")

def divide():
	global cur,c_name,divide_sets
	for c in c_name:  
		file_list = os.listdir(cur + '/data/' + c + '/')
		for file in file_list:
			rand_num = random.randint(0,S-1)
			divide_sets[rand_num].add((c,file))
		
def getData(index):
	global c_name,datapat,cur,outdata,num,answer
	#outdata.append(("training set",''))
	#outdata.append(("ID","label"))
	
	tfij = collections.defaultdict(lambda:0)
	dfi = collections.defaultdict(lambda:0)
	#vocab = set()
	num = 0
	#alldata = ''
	for c in c_name:
		file_list = os.listdir(cur + '/data/' + c + '/')
		num = num + len(file_list) #number of each kind of files
		for file in file_list: #for each file
			#if (c,file) in divide_sets[index]:#learn except index
			#	continue
			#outdata.append((file,c))
			fileobj = open(cur + '/data/' + c + '/' + file,'r')
			essay = fileobj.read()
			essay = datapat.sub(' ',essay)
			#alldata = alldata + essay
			essay = essay.split(' ')
			#print essay
			for word in essay:
				if word == '':
					continue
				if (c,file) not in divide_sets[index]:
				#learn except index
					tfij[(word,c,file)] = tfij[(word,c,file)] + 1
					dfi[word] = dfi[word] + 1
				else:
					tfij[(word,c,file)] = tfij[(word,c,file)] + 1
					dfi[word] = dfi[word]
			fileobj.close()
	'''			
	if argc == 2:	
		fileobj = open(argv[1],'r')
		essay = fileobj.read()
		essay = datapat.sub(' ',essay)
		essay = essay.split(' ')
		#print essay
		for word in essay:
			if word == '':
				continue
			#learn except index
			tfij[(word,'test')] = tfij[(word,'test')] + 1
			dfi[word] = dfi[word] + 1
		fileobj.close()
	'''			
	#print alldata	
	#vocab = set(alldata.split(' '))
	#vocab.remove('')
	#convert to set and remove empty string
	return (tfij,dfi)
	
def produceWj(index):
	global cur,c_name,answer,outdata
	outdata = [[],[]]
	wj = collections.defaultdict(lambda:[])
	tfij,dfi = getData(index)
	vocab = dfi.keys()
	N = 12 * num # - len(divide_sets[index])
	#print vocab
	i = 0
	str12 = ['',''] 
	fileobj = [open('HYF_test' + str(index) + '.txt','w'),open('HYF_train' + str(index) + '.txt','w')]
	for i in range(4):
		c = c_name[i]
		file_list = os.listdir(cur + '/data/' + c + '/')
		for file in file_list:
			flag = -1
			if (c,file) in divide_sets[index]:
				flag = 0 
				answer[index].append(i+1)
			else:
				flag = 1 #训练集
				ID[index].append(file)
			outdata[flag].append((file,c))
			#sys.stdout.write(str(i+1) + ' ')
			#str12[flag] = str12[flag] + str(i+1) + ' '
			fileobj[flag].write(str(i+1) + ' ')
			demen = 1
			for word in vocab:
				if word != '':
					#if demen == 1:
					#	print word
					#wj[(word,c,file)].append((1 + math.log(tfij[(word,c,file)]) * dfi[word])
					if tfij[(word,c,file)] == 0:
						#sys.stdout.write(str(demen) + ':' + str(0.0) + ' ')
						#str12[flag] = str12[flag] + str(demen) + ':' + str(0.0) + ' '
						fileobj[flag].write(str(demen) + ':' + str(0.0) + ' ')
					else:
						#sys.stdout.write(str(demen) + ':' + str((1 + math.log(tfij[(word,c,file)]) * dfi[word])) + ' ')
						idfi = math.log(1.0 * N / (dfi[word] + 1))
						#str12[flag] = str12[flag] + str(demen) + ':' + str((1 + math.log(tfij[(word,c,file)])) * idfi) + ' '
						fileobj[flag].write(str(demen) + ':' + str((1 + math.log(tfij[(word,c,file)])) * idfi) + ' ')
					demen = demen + 1
			#str12[flag] = str12[flag] + '\n'
			fileobj[flag].write('\n')

	fileobj[0].close()
	fileobj[1].close()
			
divide()
if argc == 1:
	'''
	for i in range(S):
		print "processing " + str(i+1) + "th time, 10 in total..."
		csvout.append(('',''))
		csvout.append(("test " + str(i) + "th time",''))
		csvout.append(("training set",''))
		csvout.append(("ID","label"))
		produceWj(i)
		csvout = csvout + outdata[1]
		
		csvout.append(('',''))
		csvout.append(("testing set",''))
		csvout.append(("ID","answer",'result'))
		csvout = csvout + outdata[0]
	'''
		
	
	from libsvm.python.svmutil import *
	for index in range(10):
	
		print "\n------------------------------------------------------\nprocessing " + str(index+1) + "th time, 10 in total..."
		csvout.append(('',''))
		csvout.append(("test " + str(index) + "th time",''))
		csvout.append(("training set",''))
		csvout.append(("ID","label"))
		produceWj(index)
		csvout = csvout + outdata[1]
		
		#print "\n------------------------------------------------------\nprocessing " + str(index+1) + "th time, 10 in total..."
		y,x = svm_read_problem('HYF_train' + str(index) + '.txt')
		yt,xt = svm_read_problem('HYF_test' + str(index) + '.txt')
		m = svm_train(y,x)
		res = svm_predict(yt,xt,m)
		result[index] = res[0]
		
		csvout.append(("",''))
		csvout.append(("testing set",''))
		csvout.append(("ID","answer","result"))

		#print answer[index]
		#print result[index]
		for iter in range(len(res[0])):
			#print iter
			csvout.append((ID[index][iter],c_name[answer[index][iter]-1],c_name[int(result[index][iter] + 0.5)-1]))
		
	right = 0
	for index in range(S):
		for i in range(len(answer[index])):
			if answer[index][i] * 1.0 - result[index][i] < 0.3 and answer[index][i] * 1.0 - result[index][i] > -0.3:
				right = right + 1
	print "\n------------------------------------------------------"
	print "\n\nTotally Accuracy: " + str(100.0 * right / num) + "%"
	
	writer = csv.writer(csvfile)
	writer.writerows(csvout)
	csvfile.close()
'''
else:
	tfij,dfi = getData(0)
	vocab = dfi.keys()
	fileobj = open(argv[1],'r')
	essay = fileobj.read()
	fileobj.close()
	essay = datapat.sub(' ',essay)
	essay = essay.split(' ')
	str12 = ['','']
	fileobj = open('HYF_test' + str(index) + '.txt','w')
	fileobj.write('0 ');
	demen = 1
	for word in vocab:
		if word == '':
			continue
		if word in essay:
			if tfij[(word,c,file)] == 0:
				fileobj.write(str(demen) + ':' + str(0.0) + ' ')
			else:
				idfi = math.log(1.0 * N / (dfi[word] + 1))
				fileobj.write(str(demen) + ':' + str((1 + math.log(tfij[(word,c,file)])) * idfi) + ' ')
		else:
			if tfij[(word,c,file)] == 0:
				fileobj.write(str(demen) + ':' + str(0.0) + ' ')
			else:
				idfi = math.log(1.0 * N / (dfi[word] + 1))
				fileobj.write(str(demen) + ':' + str((1 + math.log(tfij[(word,c,file)])) * idfi) + ' ')
		demen = demen + 1
	fileobj.close()
'''		