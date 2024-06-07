import ctypes
import time
import matplotlib.pyplot as plt
import numpy as np

# Load the shared libraries
libhamt = ctypes.CDLL('./libhamt.so')
liblinkedlist = ctypes.CDLL('./liblinkedlist.so')

# Define the structures for HAMT and LinkedList
class SearchResult(ctypes.Structure):
    _fields_ = [("values", ctypes.POINTER(ctypes.c_int)), ("valueCount", ctypes.c_int)]

class LinkedListHAMT(ctypes.Structure):
    pass

# Define the functions for LinkedListHAMT
liblinkedlist.createLinkedListHAMT.restype = ctypes.POINTER(LinkedListHAMT)
liblinkedlist.addLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int, ctypes.c_int]
liblinkedlist.updateLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int, ctypes.c_int]
liblinkedlist.deleteLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int]
liblinkedlist.searchLinkedListHAMT.argtypes = [ctypes.POINTER(LinkedListHAMT), ctypes.c_int]
liblinkedlist.searchLinkedListHAMT.restype = SearchResult

class LinkedList(ctypes.Structure):
    pass

# Define the functions for LinkedList
liblinkedlist.createLinkedList.restype = ctypes.POINTER(LinkedList)
liblinkedlist.addLinkedList.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int, ctypes.c_int]
liblinkedlist.updateLinkedList.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int, ctypes.c_int]
liblinkedlist.deleteLinkedList.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int]
liblinkedlist.searchLinkedList.argtypes = [ctypes.POINTER(LinkedList), ctypes.c_int]
liblinkedlist.searchLinkedList.restype = SearchResult

# Experiment 1: Time Complexity Comparison
def experiment_time_complexity():
    sizes = [100, 1000, 10000, 100000]
    hamt_times = []
    std_times = []

    for size in sizes:
        ll_hamt = liblinkedlist.createLinkedListHAMT()
        ll_std = liblinkedlist.createLinkedList()

        # Measure time for HAMT
        start = time.time()
        for i in range(size):
            liblinkedlist.addLinkedListHAMT(ll_hamt, i, i)
        hamt_times.append(time.time() - start)

        # Measure time for Standard Linked List
        start = time.time()
        for i in range(size):
            liblinkedlist.addLinkedList(ll_std, i, i)
        std_times.append(time.time() - start)

    # Plotting the results
    plt.plot(sizes, hamt_times, label='HAMT')
    plt.plot(sizes, std_times, label='Standard Linked List')
    plt.xlabel('Number of Elements')
    plt.ylabel('Time (seconds)')
    plt.title('Time Complexity Comparison')
    plt.legend()
    plt.savefig('time_complexity_comparison.png')
    plt.show()

# Experiment 2: Space Complexity Comparison
def experiment_space_complexity():
    sizes = [100, 1000, 10000, 100000]
    hamt_space = []
    std_space = []

    for size in sizes:
        ll_hamt = liblinkedlist.createLinkedListHAMT()
        ll_std = liblinkedlist.createLinkedList()

        # Measure space for HAMT
        for i in range(size):
            liblinkedlist.addLinkedListHAMT(ll_hamt, i, i)
        hamt_space.append(ctypes.sizeof(ll_hamt.contents))

        # Measure space for Standard Linked List
        for i in range(size):
            liblinkedlist.addLinkedList(ll_std, i, i)
        std_space.append(ctypes.sizeof(ll_std.contents))

    # Plotting the results
    plt.plot(sizes, hamt_space, label='HAMT')
    plt.plot(sizes, std_space, label='Standard Linked List')
    plt.xlabel('Number of Elements')
    plt.ylabel('Space (bytes)')
    plt.title('Space Complexity Comparison')
    plt.legend()
    plt.savefig('space_complexity_comparison.png')
    plt.show()

# Experiment 3: HAMT Operations Time Complexity with Different BIT_SEG Sizes
def experiment_hamt_operations():
    bit_seg_sizes = [1, 2, 4, 8, 16]
    operation_times = {'insert': [], 'update': [], 'delete': [], 'search': []}
    size = 10000

    for bit_seg in bit_seg_sizes:
        # Recompile HAMT library with new BIT_SEG size
        with open('hamt.c', 'r') as file:
            data = file.read()

        data = data.replace('int BIT_SEG = 4;', f'int BIT_SEG = {bit_seg};')

        with open('hamt.c', 'w') as file:
            file.write(data)

        subprocess.run(['make', 'clean'])
        subprocess.run(['make'])

        libhamt = ctypes.CDLL('./libhamt.so')

        # Measure time for each operation
        ll_hamt = liblinkedlist.createLinkedListHAMT()

        start = time.time()
        for i in range(size):
            liblinkedlist.addLinkedListHAMT(ll_hamt, i, i)
        operation_times['insert'].append(time.time() - start)

        start = time.time()
        for i in range(size):
            liblinkedlist.updateLinkedListHAMT(ll_hamt, i, i + 1)
        operation_times['update'].append(time.time() - start)

        start = time.time()
        for i in range(size):
            liblinkedlist.deleteLinkedListHAMT(ll_hamt, i)
        operation_times['delete'].append(time.time() - start)

        start = time.time()
        for i in range(size):
            liblinkedlist.searchLinkedListHAMT(ll_hamt, i)
        operation_times['search'].append(time.time() - start)

    # Plotting the results
    plt.plot(bit_seg_sizes, operation_times['insert'], label='Insert')
    plt.plot(bit_seg_sizes, operation_times['update'], label='Update')
    plt.plot(bit_seg_sizes, operation_times['delete'], label='Delete')
    plt.plot(bit_seg_sizes, operation_times['search'], label='Search')
    plt.xlabel('BIT_SEG Size')
    plt.ylabel('Time (seconds)')
    plt.title('HAMT Operations Time Complexity with Different BIT_SEG Sizes')
    plt.legend()
    plt.savefig('hamt_operations_time_complexity.png')
    plt.show()

if __name__ == "__main__":
    experiment_time_complexity()
    experiment_space_complexity()
    experiment_hamt_operations()
