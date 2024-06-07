import ctypes
from ctypes import *

# Load the shared library
libhamt = ctypes.CDLL('./libhamt.so')

# Define the C structures and functions
class SearchResult(Structure):
    _fields_ = [("values", POINTER(c_int)), ("valueCount", c_int)]

class HAMTNode(Structure):
    pass

class BitIndexNode(Structure):
    _fields_ = [("bitmap", c_int), ("subnodes", POINTER(POINTER(HAMTNode)))]

class LeafNode(Structure):
    _fields_ = [("key", c_uint32), ("values", POINTER(c_int)), ("valueCount", c_int)]

class UnionNode(Union):
    _fields_ = [("bitIndexNode", BitIndexNode), ("leafNode", LeafNode)]

HAMTNode._fields_ = [("type", c_int), ("node", UnionNode)]

class HAMT(Structure):
    _fields_ = [("root", POINTER(HAMTNode))]

class VersionedHAMT(Structure):
    _fields_ = [("versions", POINTER(POINTER(HAMT))), ("versionCount", c_int), ("currentVersion", c_int)]

# Define the functions
libhamt.createVersionedHAMT.restype = POINTER(VersionedHAMT)
libhamt.createHAMT.restype = POINTER(HAMT)
libhamt.createBitIndexNode.restype = POINTER(HAMTNode)
libhamt.createLeafNode.restype = POINTER(HAMTNode)
libhamt.hashFunction.restype = c_uint32
libhamt.insertHAMTRec.restype = POINTER(HAMTNode)
libhamt.searchHAMTRec.restype = SearchResult
libhamt.searchVersion.restype = SearchResult
libhamt.search.restype = SearchResult
libhamt.deleteHAMTRec.restype = POINTER(HAMTNode)
libhamt.updateHAMTRec.restype = POINTER(HAMTNode)

libhamt.insert.argtypes = [POINTER(VersionedHAMT), c_uint32, c_int]
libhamt.delete.argtypes = [POINTER(VersionedHAMT), c_uint32, c_int]
libhamt.update.argtypes = [POINTER(VersionedHAMT), c_uint32, c_int, c_int]

# Example usage and testing
def insert(vhamt, key, value):
    libhamt.insert(vhamt, key, value)

def delete(vhamt, key, value):
    libhamt.delete(vhamt, key, value)

def update(vhamt, key, old_value, new_value):
    libhamt.update(vhamt, key, old_value, new_value)

def search(vhamt, key):
    result = libhamt.search(vhamt, key)
    return result

def print_colored(text, color):
    colors = {'red': '\033[91m', 'green': '\033[92m', 'end': '\033[0m'}
    print(f"{colors[color]}{text}{colors['end']}")

# Test case functions

def test_insert_simple():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 4, 4)
    result= search(vhamt, 4)

    if result.valueCount == 1 and result.values[0] == 4:
        print_colored("test_insert_simple: PASSED", 'green')
    else:
        print_colored("test_insert_simple: FAIL", 'red')

def test_insert_max_depth():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 2863311530, 1)
    result= search(vhamt, 2863311530)

    if result.valueCount == 1 and result.values[0] == 1:
        print_colored("test_insert_max_depth: PASSED", 'green')
    else:
        print_colored("test_insert_max_depth: FAIL", 'red')

def test_insert_mid_level():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 1048576, 1)
    result = search(vhamt, 1048576)

    if result.valueCount == 1 and result.values[0] == 1:
        print_colored("test_insert_mid_level: PASSED", 'green')
    else:
        print_colored("test_insert_mid_level: FAIL", 'red')

def test_search_simple():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 4, 4)
    result= search(vhamt, 4)

    if result.valueCount == 1 and result.values[0] == 4:
        print_colored("test_search_simple: PASSED", 'green')
    else:
        print_colored("test_search_simple: FAIL", 'red')

def test_search_none():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 4, 4)
    result= search(vhamt, 0)

    if result.valueCount == 0 :
        print_colored("test_search_none: PASSED", 'green')
    else:
        print_colored("test_search_none: FAIL", 'red')

def test_search_multi():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 4, 39)
    insert(vhamt, 4, 42)
    result= search(vhamt, 4)

    if result.valueCount == 2 and [result.values[i] for i in range(result.valueCount)] == [39, 42]:
        print_colored("test_search_none: PASSED", 'green')
    else:
        print_colored("test_search_none: FAIL", 'red')

def test_delete():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 4, 4)
    delete(vhamt, 4, 4)
    result = search(vhamt, 4)

    if result.valueCount == 0:
        print_colored("test_delete: PASSED", 'green')
    else:
        print_colored("test_delete: FAIL", 'red')

def test_update():
    vhamt = libhamt.createVersionedHAMT()
    insert(vhamt, 4, 4)
    update(vhamt, 4, 4, 10)
    result = search(vhamt, 4)

    if result.valueCount == 1 and result.values[0] == 10:
        print_colored("test_update: PASSED", 'green')
    else:
        print_colored("test_update: FAIL", 'red')

def test_insertVersion():
    vhamt = libhamt.createVersionedHAMT()
    libhamt.insertVersion(vhamt, 4, 4, 0)
    result_v1 = libhamt.searchVersion(vhamt, 4, 1)
    result_v0 = libhamt.searchVersion(vhamt, 4, 0)

    if result_v1.valueCount == 1 and result_v1.values[0] == 4 and result_v0.valueCount == 0:
        print_colored("test_insertVersion: PASSED", 'green')
    else:
        print_colored("test_insertVersion: FAIL", 'red')

def test_deleteVersion():
    vhamt = libhamt.createVersionedHAMT()
    libhamt.insertVersion(vhamt, 4, 4, 0)
    libhamt.deleteVersion(vhamt, 4, 4, 1)
    result_v2 = libhamt.searchVersion(vhamt, 4, 2)
    result_v1 = libhamt.searchVersion(vhamt, 4, 1)
    result_v0 = libhamt.searchVersion(vhamt, 4, 0)

    if result_v2.valueCount == 0 and result_v1.valueCount == 1 and result_v1.values[0] == 4 and result_v0.valueCount == 0:
        print_colored("test_deleteVersion: PASSED", 'green')
    else:
        print_colored("test_deleteVersion: FAIL", 'red')

def test_updateVersion():
    vhamt = libhamt.createVersionedHAMT()
    libhamt.insertVersion(vhamt, 4, 4, 0)
    libhamt.updateVersion(vhamt, 4, 4, 10, 1)
    result_v2 = libhamt.searchVersion(vhamt, 4, 2)
    result_v1 = libhamt.searchVersion(vhamt, 4, 1)
    result_v0 = libhamt.searchVersion(vhamt, 4, 0)

    if result_v2.valueCount == 1 and result_v2.values[0] == 10 and result_v1.valueCount == 1 and result_v1.values[0] == 4 and result_v0.valueCount == 0:
        print_colored("test_updateVersion: PASSED", 'green')
    else:
        print_colored("test_updateVersion: FAIL", 'red')

def test_searchVersion():
    vhamt = libhamt.createVersionedHAMT()
    libhamt.insertVersion(vhamt, 4, 4, 0)
    result_v1 = libhamt.searchVersion(vhamt, 4, 1)
    result_v0 = libhamt.searchVersion(vhamt, 4, 0)

    if result_v1.valueCount == 1 and result_v1.values[0] == 4 and result_v0.valueCount == 0:
        print_colored("test_searchVersion: PASSED", 'green')
    else:
        print_colored("test_searchVersion: FAIL", 'red')

def test_partial_persistence():
    vhamt = libhamt.createVersionedHAMT()

    libhamt.insertVersion(vhamt, 1, 100, 0)
    libhamt.insertVersion(vhamt, 2, 200, 1)
    
    libhamt.updateVersion(vhamt, 1, 100, 150, 2)
    
    libhamt.deleteVersion(vhamt, 2, 200, 3)
    
    result_v0 = libhamt.searchVersion(vhamt, 1, 1)
    result_v1 = libhamt.searchVersion(vhamt, 2, 2)
    result_v2 = libhamt.searchVersion(vhamt, 1, 3)
    result_v3_1 = libhamt.searchVersion(vhamt, 1, 3)
    result_v3_2 = libhamt.searchVersion(vhamt, 2, 4)

    if (result_v0.valueCount == 1 and result_v0.values[0] == 100 and
        result_v1.valueCount == 1 and result_v1.values[0] == 200 and
        result_v2.valueCount == 1 and result_v2.values[0] == 150 and
        result_v3_1.valueCount == 1 and result_v3_1.values[0] == 150 and
        result_v3_2.valueCount == 0):
        print_colored("test_partial_persistence: PASSED", 'green')
    else:
        print_colored("test_partial_persistence: FAIL", 'red')


def test_full_persistence():
    vhamt = libhamt.createVersionedHAMT()

    libhamt.insertVersion(vhamt, 1, 100, 0)
    libhamt.insertVersion(vhamt, 2, 200, 1)
    libhamt.insertVersion(vhamt, 3, 300, 2)
    
    libhamt.updateVersion(vhamt, 1, 100, 150, 3)
    
    libhamt.deleteVersion(vhamt, 2, 200, 4)
    
    libhamt.insertVersion(vhamt, 4, 400, 5)
    libhamt.updateVersion(vhamt, 3, 300, 350, 6)
    libhamt.deleteVersion(vhamt, 1, 150, 7)

    result_v0 = libhamt.searchVersion(vhamt, 1, 1)
    result_v1 = libhamt.searchVersion(vhamt, 2, 2)
    result_v2 = libhamt.searchVersion(vhamt, 3, 3)
    result_v3 = libhamt.searchVersion(vhamt, 1, 4)
    result_v4 = libhamt.searchVersion(vhamt, 2, 5)
    result_v5 = libhamt.searchVersion(vhamt, 4, 6)
    result_v6 = libhamt.searchVersion(vhamt, 3, 7)
    result_v7 = libhamt.searchVersion(vhamt, 1, 8)

    if (result_v0.valueCount == 1 and result_v0.values[0] == 100 and
        result_v1.valueCount == 1 and result_v1.values[0] == 200 and
        result_v2.valueCount == 1 and result_v2.values[0] == 300 and
        result_v3.valueCount == 1 and result_v3.values[0] == 150 and
        result_v4.valueCount == 0 and
        result_v5.valueCount == 1 and result_v5.values[0] == 400 and
        result_v6.valueCount == 1 and result_v6.values[0] == 350 and
        result_v7.valueCount == 0):
        print_colored("test_full_persistence: PASSED", 'green')
    else:
        print_colored("test_full_persistence: FAIL", 'red')

# Run tests
if __name__ == "__main__":
    test_insert_simple()
    test_insert_max_depth()
    test_insert_mid_level()
    test_search_simple()
    test_search_none()
    test_search_multi()
    test_delete()
    test_update()
    test_insertVersion()
    test_deleteVersion()
    test_updateVersion()
    test_searchVersion()
    test_partial_persistence()
    test_full_persistence()


