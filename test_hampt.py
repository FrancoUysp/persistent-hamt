import subprocess
from colorama import Fore, Style, init

# Initialize colorama
init(autoreset=True)

def run_hamt_program(commands):
    # Run the HAMT program
    process = subprocess.Popen(['./hamt'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate(input=commands.encode('utf-8'))
    
    # Decode the output
    output = stdout.decode('utf-8')
    errors = stderr.decode('utf-8')
    
    return output, errors

def main():
    commands = """
				insert 1 100
				insert 2 200
				insert 3 300
				insert 4 400
				insert 8 800
				insert 213 21300
				insert 23 2300
				insert 5 500
				search 1
				search 2
				search 3
				search 4
				search 8
				search 213
				search 23
				search 5
				search 6
				print
				"""
    
    output, errors = run_hamt_program(commands)
    
    if errors:
        print(Fore.RED + "Errors:\n" + errors)
    else:
        print(Fore.GREEN + "Program Output:\n" + output)
    
    # Perform tests on the output
    test_cases = {
        "Search key 1: 100": "Expected output for key 1 not found",
        "Search key 2: 200": "Expected output for key 2 not found",
        "Search key 3: 300": "Expected output for key 3 not found",
        "Search key 4: 400": "Expected output for key 4 not found",
        "Search key 8: 800": "Expected output for key 8 not found",
        "Search key 213: 21300": "Expected output for key 213 not found",
        "Search key 23: 2300": "Expected output for key 23 not found",
        "Search key 5: 500": "Expected output for key 5 not found",
        "Search key 6: -1": "Expected output for key 6 not found",
    }

    for expected_output, error_message in test_cases.items():
        try:
            assert expected_output in output, error_message
            print(Fore.GREEN + f"Test passed: {expected_output}")
        except AssertionError as e:
            print(Fore.RED + f"Test failed: {e}")

    print(Style.BRIGHT + "All tests completed!")

if __name__ == "__main__":
    main()
