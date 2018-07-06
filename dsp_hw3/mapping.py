text = open('Big5-ZhuYin.map',encoding = 'cp950')

zhuyin_index = {}
for line in text:
	for i in line.strip('\n').split(' '):
		for j in i.split('/'):
			zhuyin_index.setdefault(j[0],[])
			zhuyin_index[j[0]].append(line.split(' ')[0])

file = open('ZhuYin-Big5.map','w')

for key in zhuyin_index:
	value = ' '.join(zhuyin_index[key])
	s = "{}\t{}\n".format(key, value)
	b = s.encode("cp950", "ignore")
	file.buffer.write(b)

