import ctypes
import time
import matplotlib.pyplot as plt
import numpy as np
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
liblinkedlisthamt.ll_add.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int, ctypes.c_int]
liblinkedlisthamt.ll_update.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int, ctypes.c_int]
liblinkedlisthamt.ll_delete.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int]
liblinkedlisthamt.ll_search.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int]
liblinkedlisthamt.ll_search.restype = ctypes.c_int

# Helper function to measure time
def measure_time(func, *args):
    start_time = time.perf_counter()
    func(*args)
    end_time = time.perf_counter()
    return end_time - start_time

def experiment_time_complexity():
    sizes = [10, 100, 1000, 10000]
    operations = ['add', 'update', 'delete', 'search']
    std_times = {op: [] for op in operations}
    hamt_times = {op: [] for op in operations}

    for size in sizes:
        # Measure time for each operation for std linked list
        ll_std = liblinkedlist.createLinkedListSTD()
        ll_hamt = liblinkedlisthamt.createLinkedListHAMT()

        for i in range(size):
            liblinkedlist.addLinkedListSTD(ll_std, i, i)
            liblinkedlisthamt.ll_add(ll_hamt, i, i)

        # Add operation
        std_time = sum(measure_time(liblinkedlist.addLinkedListSTD, ll_std, i, i) for i in range(size)) / size
        hamt_time = sum(measure_time(liblinkedlisthamt.ll_add, ll_hamt, i, i) for i in range(size)) / size
        std_times['add'].append(std_time)
        hamt_times['add'].append(hamt_time)

        # Update operation
        std_time = sum(measure_time(liblinkedlist.updateLinkedListSTD, ll_std, i, i+1) for i in range(size)) / size
        hamt_time = sum(measure_time(liblinkedlisthamt.ll_update, ll_hamt, i, i+1) for i in range(size)) / size
        std_times['update'].append(std_time)
        hamt_times['update'].append(hamt_time)

        # Delete operation
        std_time = sum(measure_time(liblinkedlist.deleteLinkedListSTD, ll_std, i) for i in range(size)) / size
        hamt_time = sum(measure_time(liblinkedlisthamt.ll_delete, ll_hamt, i) for i in range(size)) / size
        std_times['delete'].append(std_time)
        hamt_times['delete'].append(hamt_time)

        # Search operation
        std_time = sum(measure_time(liblinkedlist.searchLinkedListSTD, ll_std, i) for i in range(size)) / size
        hamt_time = sum(measure_time(liblinkedlisthamt.ll_search, ll_hamt, i) for i in range(size)) / size
        std_times['search'].append(std_time)
        hamt_times['search'].append(hamt_time)

    # Plot results
    fig, axs = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle('Amortized Average Time Complexity')

    for i, op in enumerate(operations):
        ax = axs[i//2, i%2]
        ax.plot(sizes, std_times[op], label='Standard Linked List')
        ax.plot(sizes, hamt_times[op], label='HAMT Linked List')
        ax.set_xlabel('Size')
        ax.set_ylabel('Time per Operation (s)')
        ax.set_title(op.capitalize())
        ax.legend()
        ax.set_xscale('log')
        ax.set_yscale('log')

    plt.tight_layout()
    plt.savefig('amortized_time_complexity.png')
    plt.show()


# Experiment 2: Space complexity of std linked list vs HAMT
def experiment_space_complexity():
    sizes = [10, 100, 1000, 10000]
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
            liblinkedlisthamt.ll_add(ll_hamt, i, i)
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
    plt.title('Space Complexity')
    plt.legend()
    plt.savefig('space_complexity.png')
    plt.show()

# Experiment 3: Time complexity of HAMT operations over different BIT_SEG sizes
def experiment_bit_seg_time_complexity():
    bit_segs = [2, 4, 8, 16]
    sizes = [10, 100, 1000]
    times = {bit_seg: [] for bit_seg in bit_segs}

    for bit_seg in bit_segs:
        for size in sizes:
            # Assuming BIT_SEG is globally changeable in the C library
            ctypes.c_int.in_dll(liblinkedlisthamt, 'BIT_SEG').value = bit_seg
            ll_hamt = liblinkedlisthamt.createLinkedListHAMT()
            hamt_time = sum(measure_time(liblinkedlisthamt.ll_add, ll_hamt, i, i) for i in range(size))
            times[bit_seg].append(hamt_time)

    # Plot results
    plt.figure()
    for bit_seg, time_values in times.items():
        plt.plot(sizes, time_values, label=f'BIT_SEG = {bit_seg}')
    plt.xlabel('Size')
    plt.ylabel('Time (s)')
    plt.yscale('log')
    plt.title('HAMT Time Complexity with Different BIT_SEG Sizes')
    plt.legend()
    plt.savefig('bit_seg_time_complexity.png')
    plt.show()

if __name__ == "__main__":
    experiment_time_complexity()
    experiment_bit_seg_time_complexity()
