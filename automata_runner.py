import os
import time

from matplotlib import pyplot as plt


proc_num = [2, 3, 4]
times = []

for proc in proc_num:
    start_time = time.time()
    command = 'mpirun --allow-run-as-root -n {} ./automata'.format(proc)
    print(command)
    os.system(command)
    finish_time = time.time()
    times.append(finish_time - start_time)


fig, ax = plt.subplots(nrows=1, ncols=1)

ax.title.set_text('Times of program execution for different proc num')
ax.set_ylabel('time (s)')
ax.set_xlabel('proc num')
ax.plot(proc_num, times)
fig.savefig('automata.png')   # save the figure to file
plt.close(fig)
