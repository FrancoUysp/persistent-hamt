import ctypes
from ctypes import *

# Load the shared library
liblinkedlist = ctypes.CDLL('./liblinkedlist.so')

# Define the C structures and functions
class Node(Structure):
    pass

Node._fields_ = [("data", c_int), ("next", POINTER(Node))]

class LinkedList(Structure):
    _fields_ = [("head", POINTER(Node)), ("size", c_int)]

liblinkedlist.createLinkedList.restype = POINTER(LinkedList)
liblinkedlist.add.argtypes = [POINTER(LinkedList), c_int, c_int]
liblinkedlist.update.argtypes = [POINTER(LinkedList), c_int, c_int]
liblinkedlist.delete.argtypes = [POINTER(LinkedList), c_int]
liblinkedlist.search.restype = POINTER(Node)
liblinkedlist.search.argtypes = [POINTER(LinkedList), c_int]

def print_colored(text, color):
    colors = {'red': '\033[91m', 'green': '\033[92m', 'end': '\033[0m'}
    print(f"{colors[color]}{text}{colors['end']}")

# Test case functions
def test_add():
    list = liblinkedlist.createLinkedList()
    liblinkedlist.add(list, 0, 10)
    liblinkedlist.add(list, 1, 20)
    liblinkedlist.add(list, 2, 30)
    node = liblinkedlist.search(list, 20)
    if list.contents.size == 3 and node and node.contents.data == 20:
        print_colored("test_add: PASSED", 'green')
    else:
        print_colored("test_add: FAIL", 'red')

def test_update():
    list = liblinkedlist.createLinkedList()
    liblinkedlist.add(list, 0, 10)
    liblinkedlist.add(list, 1, 20)
    liblinkedlist.add(list, 2, 30)
    liblinkedlist.update(list, 1, 25)
    node = liblinkedlist.search(list, 25)
    if node and node.contents.data == 25:
        print_colored("test_update: PASSED", 'green')
    else:
        print_colored("test_update: FAIL", 'red')

def test_delete():
    list = liblinkedlist.createLinkedList()
    liblinkedlist.add(list, 0, 10)
    liblinkedlist.add(list, 1, 20)
    liblinkedlist.add(list, 2, 30)
    liblinkedlist.delete(list, 1)
    node = liblinkedlist.search(list, 20)
    if list.contents.size == 2:
        print_colored("test_delete: PASSED", 'green')
    else:
        print_colored("test_delete: FAIL", 'red')

def test_search():
    list = liblinkedlist.createLinkedList()
    liblinkedlist.add(list, 0, 10)
    liblinkedlist.add(list, 1, 20)
    liblinkedlist.add(list, 2, 30)
    node = liblinkedlist.search(list, 20)
    if node and node.contents.data == 20:
        print_colored("test_search: PASSED", 'green')
    else:
        print_colored("test_search: FAIL", 'red')

# Run tests
if __name__ == "__main__":
    test_add()
    test_update()
    test_delete()
    test_search()
