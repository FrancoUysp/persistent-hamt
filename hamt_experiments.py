import ctypes
import time
import matplotlib.pyplot as plt
import tracemalloc

# Load the shared libraries
liblinkedlist = ctypes.CDLL('./liblinkedlist.so')
liblinkedlisthamt = ctypes.CDLL('./liblinkedlisthamt.so')

# Define the C structures
class LinkedList(ctypes.Structure):
    pass

class LinkedListHAMT(ctypes.Structure):
    pass

# Define the C functions for std linked list
liblinkedlist.createLinkedListSTD.restype = ctypes.POINTER(LinkedList)
liblinkedlist.addLinkedListSTD.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int, ctypes.c_int]
liblinkedlist.updateLinkedListSTD.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int, ctypes.c_int]
liblinkedlist.deleteLinkedListSTD.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int]
liblinkedlist.searchLinkedListSTD.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int]
liblinkedlist.searchLinkedListSTD.restype = ctypes.c_int

# Define the C functions for HAMT linked list
liblinkedlisthamt.createLinkedListHAMT.restype = ctypes.POINTER(LinkedListHAMT)
liblinkedlisthamt.addLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int, ctypes.c_int]
liblinkedlisthamt.updateLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int, ctypes.c_int]
liblinkedlisthamt.deleteLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int]
liblinkedlisthamt.searchLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int]
liblinkedlisthamt.searchLinkedListHAMT.restype = ctypes.c_int

# Helper function to measure time
def measure_time(func, *args):
    start_time = time.perf_counter()
    func(*args)
    end_time = time.perf_counter()
    return end_time - start_time

# Experiment 1: Time complexity of std linked list vs HAMT
def experiment_time_complexity():
    sizes = [10, 100, 1000, 10000, 100000]
    std_times = []
    hamt_times = []

    for size in sizes:
        # Measure time for std linked list
        ll_std = liblinkedlist.createLinkedListSTD()
        std_time = sum(measure_time(liblinkedlist.addLinkedListSTD, ll_std, i, i) for i in range(size))
        std_times.append(std_time)

        # Measure time for HAMT linked list
        ll_hamt = liblinkedlisthamt.createLinkedListHAMT()
        hamt_time = sum(measure_time(liblinkedlisthamt.addLinkedListHAMT, ll_hamt, i, i) for i in range(size))
        hamt_times.append(hamt_time)

    # Plot results
    plt.figure()
    plt.plot(sizes, std_times, label='Standard Linked List')
    plt.plot(sizes, hamt_times, label='HAMT Linked List')
    plt.xlabel('Size')
    plt.ylabel('Time (s)')
    plt.xscale('log')
    plt.yscale('log')
    plt.title('Time Complexity')
    plt.legend()
    plt.savefig('time_complexity.png')
    plt.show()

# Experiment 2: Space complexity of std linked list vs HAMT
def experiment_space_complexity():
    sizes = [10, 100, 1000, 10000, 100000]
    std_spaces = []
    hamt_spaces = []

    for size in sizes:
        tracemalloc.start()

        # Measure space for std linked list
        ll_std = liblinkedlist.createLinkedListSTD()
        for i in range(size):
            liblinkedlist.addLinkedListSTD(ll_std, i, i)
        snapshot_std = tracemalloc.take_snapshot()
        std_space = sum(stat.size for stat in snapshot_std.statistics('filename'))
        std_spaces.append(std_space)

        tracemalloc.stop()
        tracemalloc.start()

        # Measure space for HAMT linked list
        ll_hamt = liblinkedlisthamt.createLinkedListHAMT()
        for i in range(size):
            liblinkedlisthamt.addLinkedListHAMT(ll_hamt, i, i)
        snapshot_hamt = tracemalloc.take_snapshot()
        hamt_space = sum(stat.size for stat in snapshot_hamt.statistics('filename'))
        hamt_spaces.append(hamt_space)

        tracemalloc.stop()

    # Plot results
    plt.figure()
    plt.plot(sizes, std_spaces, label='Standard Linked List')
    plt.plot(sizes, hamt_spaces, label='HAMT Linked List')
    plt.xlabel('Size')
    plt.ylabel('Space (bytes)')
    plt.xscale('log')
    plt.title('Space Complexity')
    plt.legend()
    plt.savefig('space_complexity.png')
    plt.show()

# Experiment 3: Time complexity of HAMT operations over different BIT_SEG sizes
def experiment_bit_seg_time_complexity():
    bit_segs = [2, 4, 8, 16]
    sizes = [10, 100, 1000, 10000]
    times = {bit_seg: [] for bit_seg in bit_segs}

    for bit_seg in bit_segs:
        for size in sizes:
            # Assuming BIT_SEG is globally changeable in the C library
            ctypes.c_int.in_dll(liblinkedlisthamt, 'BIT_SEG').value = bit_seg
            ll_hamt = liblinkedlisthamt.createLinkedListHAMT()
            hamt_time = sum(measure_time(liblinkedlisthamt.addLinkedListHAMT, ll_hamt, i, i) for i in range(size))
            times[bit_seg].append(hamt_time)

    # Plot results
    plt.figure()
    for bit_seg, time_values in times.items():
        plt.plot(sizes, time_values, label=f'BIT_SEG = {bit_seg}')
    plt.xlabel('Size')
    plt.ylabel('Time (s)')
    plt.xscale('log')
    plt.yscale('log')
    plt.title('HAMT Time Complexity with Different BIT_SEG Sizes')
    plt.legend()
    plt.savefig('bit_seg_time_complexity.png')
    plt.show()

if __name__ == "__main__":
    experiment_time_complexity()
    experiment_bit_seg_time_complexity()
