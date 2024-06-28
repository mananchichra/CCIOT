import matplotlib.pyplot as plt
import numpy as np
    
def changeState(curr_stage):
    if curr_stage == 'soak':
        return 'wash'
    elif curr_stage == 'wash':
        return 'wash_end'
    elif curr_stage == 'wash_end':
        return 'rinse'
    elif curr_stage == 'rinse':
        return 'rinse_end'
    elif curr_stage == 'rinse_end':
        return 'spin'
    else:
        return 'soak'

def mean(Last_k_points):
    return sum(Last_k_points)/len(Last_k_points)

def Stddev(Last_k_points):
    return np.std(Last_k_points)
    
def Detectcycle(Mpoints):
    M_size = len(Mpoints)
    curr_stage = 'soak'

    Stages = M_size*[0]
    K = 100
    Last_k_points = K*[0]
    
    K_mean = M_size*[0]
    K_dev = M_size*[0]
    OFFcount = 0

    for i in range(0,M_size):
        Last_k_points.pop(0)
        Last_k_points = Last_k_points + [Mpoints[i]]

        K_mean[i] = mean(Last_k_points)
        K_dev[i] = Stddev(Last_k_points)

        if K_mean[i]>0:
            OFFcount=0

        if K_mean[i] == 0:
            if OFFcount<=100:
                OFFcount=OFFcount+1
            if OFFcount>100:
                curr_stage = 'OFF'
        elif curr_stage == 'OFF' and K_mean[i] <0.5:
            curr_stage = 'soak'
        elif curr_stage == 'soak' and K_mean[i] > 3.5:
            curr_stage = changeState(curr_stage)
        elif curr_stage == 'wash' and K_mean[i] >1 and K_mean[i] <1.5 and K_dev[i] <1.5:
            curr_stage = changeState(curr_stage)
        elif curr_stage == 'wash_end' and K_mean[i] < 0.5:
            curr_stage = changeState(curr_stage)
        elif curr_stage == 'rinse' and K_mean[i] >1 and K_mean[i] <1.5 and K_dev[i] <1.5:
            curr_stage = changeState(curr_stage)
        elif curr_stage == 'rinse_end' and K_mean[i] < 0.5:
            curr_stage = changeState(curr_stage)

        Stages[i] = curr_stage

    return Stages,K_mean,K_dev

with open('Cycle_1.txt', 'r') as file:
    s = file.read()

s = s[:2011*73 + 72] + s[2011*73 + 84:]
s = s[:2553*73 + 72] + s[2553*73 + 73:]
s = s[:2554*73 + 72] + s[2554*73 + 73:]
s = s[:2555*73 + 72] + s[2555*73 + 73:]

line_size = 73

s_size = len(s)
row_size = s_size/line_size

row_size = int(row_size)

Max_Values2 = row_size*[0]
Vibration = row_size*[0]

for i in range(0,row_size):
    indices1 = line_size*i
    indices2 = line_size*i
    Max_Values2[i] = float(s[indices1+64:indices2+72])


Max_Values = Max_Values2

Mpoints = np.array(Max_Values)

# Stages,K_mean,K_dev= Detectcycle(Mpoints)

arr = [0]*len(Mpoints)
for i in range(1,len(Mpoints)):
    arr[i] = i*1.2


plt.plot(arr,Mpoints)
plt.ylabel('Maximum current per second')
plt.xlabel('Seconds')
plt.show()
# plt.plot(K_mean)
# plt.plot(K_dev)
# plt.title('Stages')
# plt.show()