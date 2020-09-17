import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import collections

file_name = input("Enter file:") + ".txt"

lines = []                                
with open (file_name, 'rt') as myfile:    
    for line in myfile:                   
        lines.append(line.rstrip('\n')) 

latency = []
success = 0
count = 0
if(file_name == "3B.txt"):
	count = -1
for i in lines:
	if i[0: 2] != '64':
		if count <= 0:
			count += 1
		else:
			break
	else:
		success += 1
	
print(success, count)
for i in range(1, success+1):
	latency.append(float(lines[i][lines[i].find("time") + 5: lines[i].find("time") + 9]))
	
	
#print("Latency = ", latency)
length = len(lines)
idx_loss = lines[length - 2].find("received")
print("loss = ", lines[length - 2][idx_loss + 10: lines[length - 2].find("%")])

"""
idx_rtt = lines[length - 1].find("=")
first_occurr = lines[length - 1].find('/', idx_rtt)
print("min = ", lines[length - 1][idx_rtt + 2: first_occurr])

second_occurr = lines[length - 1].find('/', first_occurr + 1)
print("avg = ", lines[length - 1][first_occurr + 1: second_occurr])

third_occurr = lines[length - 1].find('/', second_occurr + 1)
print("max = ", lines[length - 1][second_occurr + 1: third_occurr])

fourth_occurr = lines[length - 1].find(' ', third_occurr + 1)
print("mdev = ", lines[length - 1][third_occurr + 1: fourth_occurr])
"""

min_latency = np.min(np.array(latency))
max_latency = np.max(np.array(latency))
mean_latency = np.mean(np.array(latency))
median_latency = np.median(np.array(latency))

print("Min =", min_latency)
print("Max =", max_latency)
print("Mean =", mean_latency)
print("Median =", median_latency)

x = latency
hist, bins = np.histogram(x, bins=180, range=(min_latency - 5, mean_latency + 50))
width = 0.7 * (bins[1] - bins[0])
center = (bins[:-1] + bins[1:]) / 2
plt.bar(center, hist, align='center', width=width)
plt.xlabel("Latency (in ms) ")
plt.ylabel("Frequency")
plt.title("ping -p ff00 172.217.160.174")
plt.show()

"""
freq = {} 
for item in latency:
	item = round(float(item))
	if (item in freq):
		freq[item] += 1
	else: 
		freq[item] = 1

freq = collections.OrderedDict(sorted(freq.items()))



plt.figure(figsize=(20,3))
plt.bar(range(len(freq)), list(freq.values()), align='center', width=0.7)
plt.xticks(range(len(freq)), list(freq.keys()), rotation=40, fontsize=7)
plt.yticks(fontsize=7)

#plt.tick_params(axis='x', which='major', labelsize=4)

ax = plt.axes()
ax.xaxis.set_major_locator(ticker.MultipleLocator(5))
ax.xaxis.set_minor_locator(ticker.MultipleLocator(1))
plt.show()"""
