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
    commands = """insert 24 100
insert 25 200
insert 26 300
search 24
search 25
search 26
update 24 100 150
update 25 200 250
update 26 300 350
search 24
search 25
search 26
delete 24 150
delete 25 250
delete 26 350
search 24
search 25
search 26
insert 100 1000
insert 200 2000
insert 300 3000
search 100
search 200
search 300
print"""
    
    output, errors = run_hamt_program(commands)
    
    if errors:
        print(Fore.RED + "Errors:\n" + errors)
    else:
        print(Fore.GREEN + "Output:\n" + output)

if __name__ == "__main__":
    main()
