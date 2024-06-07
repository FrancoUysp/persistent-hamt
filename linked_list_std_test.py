import ctypes
from ctypes import *

# Load the shared library
liblinkedlist = ctypes.CDLL('./liblinkedlist.so')

class LinkedList(Structure):
    """
    Represents a linked list.
    """
    pass

class Node(Structure):
    """
    Represents a node in the linked list.
    """
    pass

Node._fields_ = [("data", c_int), ("next", POINTER(Node))]
LinkedList._fields_ = [("head", POINTER(Node)), ("size", c_int)]

liblinkedlist.createLinkedListSTD.restype = POINTER(LinkedList)
liblinkedlist.addLinkedListSTD.argtypes = [POINTER(LinkedList), c_int, c_int]
liblinkedlist.updateLinkedListSTD.argtypes = [POINTER(LinkedList), c_int, c_int]
liblinkedlist.deleteLinkedListSTD.argtypes = [POINTER(LinkedList), c_int]
liblinkedlist.searchLinkedListSTD.restype = c_int
liblinkedlist.searchLinkedListSTD.argtypes = [POINTER(LinkedList), c_int]

def print_colored(text, color):
    """
    Prints the text in the specified color.

    Args:
        text (str): The text to be printed.
        color (str): The color of the text. Supported values: 'red', 'green'.
    """
    colors = {'red': '\033[91m', 'green': '\033[92m', 'end': '\033[0m'}
    print(f"{colors[color]}{text}{colors['end']}")

def test_add():
    """
    Test case for adding elements to the linked list.
    """
    list = liblinkedlist.createLinkedListSTD()
    liblinkedlist.addLinkedListSTD(list, 0, 10)
    liblinkedlist.addLinkedListSTD(list, 1, 20)
    liblinkedlist.addLinkedListSTD(list, 2, 30)
    found = liblinkedlist.searchLinkedListSTD(list, 20)
    if found:
        print_colored("test_add: PASSED", 'green')
    else:
        print(str(list.contents.size) + " " + str(found))
        print_colored("test_add: FAIL", 'red')

def test_update():
    """
    Test case for updating an element in the linked list.
    """
    list = liblinkedlist.createLinkedListSTD()
    liblinkedlist.addLinkedListSTD(list, 0, 10)
    liblinkedlist.addLinkedListSTD(list, 1, 20)
    liblinkedlist.addLinkedListSTD(list, 2, 30)
    liblinkedlist.updateLinkedListSTD(list, 1, 25)
    found = liblinkedlist.searchLinkedListSTD(list, 25)
    if found:
        print_colored("test_update: PASSED", 'green')
    else:
        print_colored("test_update: FAIL", 'red')

def test_delete():
    """
    Test case for deleting an element from the linked list.
    """
    list = liblinkedlist.createLinkedListSTD()
    liblinkedlist.addLinkedListSTD(list, 0, 10)
    liblinkedlist.addLinkedListSTD(list, 1, 20)
    liblinkedlist.addLinkedListSTD(list, 2, 30)
    liblinkedlist.deleteLinkedListSTD(list, 1)
    found = liblinkedlist.searchLinkedListSTD(list, 20)
    if not found:
        print_colored("test_delete: PASSED", 'green')
    else:
        print_colored("test_delete: FAIL", 'red')

def test_search():
    """
    Test case for searching an element in the linked list.
    """
    list = liblinkedlist.createLinkedListSTD()
    liblinkedlist.addLinkedListSTD(list, 0, 10)
    liblinkedlist.addLinkedListSTD(list, 1, 20)
    liblinkedlist.addLinkedListSTD(list, 2, 30)
    found = liblinkedlist.searchLinkedListSTD(list, 20)
    if found:
        print_colored("test_search: PASSED", 'green')
    else:
        print_colored("test_search: FAIL", 'red')

# Run tests
if __name__ == "__main__":
    test_add()
    test_update()
    test_delete()
    test_search()