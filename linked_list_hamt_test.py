import ctypes
from ctypes import *

# Load the shared libraries
libhamt = ctypes.CDLL('./libhamt.so')
liblinkedlisthamt = ctypes.CDLL('./liblinkedlisthamt.so')

# Define the C structures and functions

class SearchResult(Structure):
    """
    Represents the result of a search operation in a linked list HAMT.

    Attributes:
        values (POINTER(c_int)): A pointer to an array of integers containing the search results.
        valueCount (c_int): The number of values found in the search.
    """
    _fields_ = [("values", POINTER(c_int)), ("valueCount", c_int)]

libhamt.createVersionedHAMT.restype = POINTER(c_void_p)

liblinkedlisthamt.createLinkedListHAMT.restype = POINTER(c_void_p)
liblinkedlisthamt.ll_add.argtypes = [POINTER(c_void_p), c_int, c_int]
liblinkedlisthamt.ll_update.argtypes = [POINTER(c_void_p), c_int, c_int]
liblinkedlisthamt.ll_delete.argtypes = [POINTER(c_void_p), c_int]
liblinkedlisthamt.ll_search.restype = SearchResult
liblinkedlisthamt.ll_search.argtypes = [POINTER(c_void_p), c_int]

def print_colored(text, color):
    """
    Prints text in the specified color.

    Args:
        text (str): The text to print.
        color (str): The color to use for printing.
    """
    colors = {'red': '\033[91m', 'green': '\033[92m', 'end': '\033[0m'}
    print(f"{colors[color]}{text}{colors['end']}")

# Test case functions

def test_add():
    """
    Tests the add function of the linked list HAMT.
    """
    list = liblinkedlisthamt.createLinkedListHAMT()
    liblinkedlisthamt.ll_add(list, 0, 10)
    liblinkedlisthamt.ll_add(list, 1, 20)
    liblinkedlisthamt.ll_add(list, 2, 30)
    result = liblinkedlisthamt.ll_search(list, 20)
    if result.valueCount == 1 and result.values[0] == 20:
        print_colored("test_add: PASSED", 'green')
    else:
        print_colored("test_add: FAIL", 'red')

def test_update():
    """
    Tests the update function of the linked list HAMT.
    """
    list = liblinkedlisthamt.createLinkedListHAMT()
    liblinkedlisthamt.ll_add(list, 0, 10)
    liblinkedlisthamt.ll_add(list, 1, 20)
    liblinkedlisthamt.ll_add(list, 2, 30)
    liblinkedlisthamt.ll_update(list, 1, 25)
    result = liblinkedlisthamt.ll_search(list, 25)
    if result.valueCount == 1 and result.values[0] == 25:
        print_colored("test_update: PASSED", 'green')
    else:
        print_colored("test_update: FAIL", 'red')

def test_delete():
    """
    Tests the delete function of the linked list HAMT.
    """
    list = liblinkedlisthamt.createLinkedListHAMT()
    liblinkedlisthamt.ll_add(list, 0, 10)
    liblinkedlisthamt.ll_add(list, 1, 20)
    liblinkedlisthamt.ll_add(list, 2, 30)
    liblinkedlisthamt.ll_delete(list, 1)
    result = liblinkedlisthamt.ll_search(list, 20)
    if result.valueCount == 0:
        print_colored("test_delete: PASSED", 'green')
    else:
        print_colored("test_delete: FAIL", 'red')

def test_search():
    """
    Tests the search function of the linked list HAMT.
    """
    list = liblinkedlisthamt.createLinkedListHAMT()
    liblinkedlisthamt.ll_add(list, 0, 10)
    liblinkedlisthamt.ll_add(list, 1, 20)
    liblinkedlisthamt.ll_add(list, 2, 30)
    result = liblinkedlisthamt.ll_search(list, 20)
    if result.valueCount == 1 and result.values[0] == 20:
        print_colored("test_search: PASSED", 'green')
    else:
        print_colored("test_search: FAIL", 'red')

# Run tests
if __name__ == "__main__":
    test_add()
    test_update()
    test_delete()
    test_search()
