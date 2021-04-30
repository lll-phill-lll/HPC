import os
import time

from matplotlib import pyplot as plt
import pandas as pd


vector_sizes = [2**5, 2**10, 2**13, 2**16, 2**19]
times = []
number_of_pings = 2**20
bytes_in_mbytes = 2**20
size_of_int = 2**2

for vector_size in vector_sizes:
    start_time = time.time()
    os.system('mpirun --allow-run-as-root -n 4 ./ping_pong_2 {}'.format(vector_size))
    finish_time = time.time()
    times.append(finish_time - start_time)
    print('VS: {}, time: {}'.format(vector_size, finish_time - start_time))


fig, ax = plt.subplots(nrows=1, ncols=1)

ax.title.set_text('Times of program execution for 2^20 ping-pongs')
ax.set_ylabel('time (s)')
ax.set_xlabel('array size')
ax.plot(vector_sizes, times)
fig.savefig('ping_pong_times.png')   # save the figure to file
plt.close(fig)

cols_names = ['Size(bytes)', '# Iterations', 'Total time (secs)', 'Time per message', 'Bandwidth (MB/s)']

rows = []

for i, vector_size in enumerate(vector_sizes):
    size_i = vector_size * size_of_int
    time_i = times[i]
    time_per_message = time_i / number_of_pings
    bandwidth = number_of_pings * size_of_int * vector_size / (time_i * bytes_in_mbytes)
    row_i = [size_i, number_of_pings, time_i, time_per_message, bandwidth]
    rows.append(row_i)

# rows to cols
cols = list(map(list, zip(*rows)))

table = {}

for i, col_name in enumerate(cols_names):
    table[col_name] = cols[i]

df = pd.DataFrame(table, columns = cols_names)

f = open("ping_pong_table.md", "w")
f.write(df.to_markdown())
f.close()
