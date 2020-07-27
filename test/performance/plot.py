import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

def read_file(filename):
  labels = ["futures", "title", "wait", "exec", "duration", "us_future", "queue", "numa_sensitive", "num_threads", "info_string", "libcds"]
  data =  pd.read_csv(filename, sep=',', header=None)
  data.columns = labels
  return data

def get_files_data(threads):
  filenames = []
  for t in threads:
    filenames.append( "thread_" + str(t) + ".txt" )

  rawdata = []
  for f in filenames:
    rawdata.append(read_file(f))

  data = pd.concat(rawdata)
  return data

def get_overhead(threads, exec_type, data):
  with_libcds = []
  no_libcds = []
  for t in threads:
    with_libcds.append(data.loc[ (data["num_threads"] == t) & (data["libcds"] == 1) & (data["exec"] == exec_type) ]["us_future"].mean())
    no_libcds.append(data.loc[ (data["num_threads"] == t) & (data["libcds"] == 0) & (data["exec"] == exec_type) ]["us_future"].mean())

  return np.array(with_libcds) - np.array(no_libcds)


threads = [1, 2, 4, 6, 8, 10, 12, 14, 14, 16]
data = get_files_data(threads)

#print(data)
#remove whitespace
data = data.apply(lambda x: x.str.strip() if x.dtype == "object" else x)
#print(data)

exec = ["none", "parallel_executor", "thread_pool_executor"]

overhead_exec_none = get_overhead(threads, exec[0], data)
overhead_exec_parallel_executor = get_overhead(threads, exec[1], data)
overhead_exec_thread_pool_executor = get_overhead(threads, exec[2], data)


plt.plot(threads, overhead_exec_none, marker="x")
plt.plot(threads, overhead_exec_parallel_executor, marker="x")
plt.plot(threads, overhead_exec_thread_pool_executor, marker="x")
exec_labels = ["create_thread_hierarchical, latch, none", "apply parallel_executor", "apply thread_pool_executor"]
plt.legend(exec_labels, loc=1)

plt.ylabel('overhead in us/future')
plt.xlabel('Threads')
plt.title('Libcds Hazard Pointers Overhead Measured w/ 1M HPX Futures')

#plt.show()
plt.savefig('overhead_hazard_pointers.png')