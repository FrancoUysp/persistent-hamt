import subprocess
import random
from colorama import Fore, init

# Initialize colorama
init(autoreset=True)

def run_hamt_program(commands):
    process = subprocess.Popen(['./hamt'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output, error = process.communicate(commands)
    return output, error

def main():
    # Generate a large number of commands
    commands = ""
    for i in range(1000):
        key = random.randint(0, 10000)
        value = random.randint(0, 10000)
        commands += f"insert {key} {value}\n"

    for i in range(1000):
        key = random.randint(0, 10000)
        value = random.randint(0, 10000)
        commands += f"update {key} {value} {value + 100}\n"
    
    for i in range(500):
        key = random.randint(0, 10000)
        value = random.randint(0, 10000)
        commands += f"delete {key} {value + 100}\n"

    for i in range(1000):
        key = random.randint(0, 10000)
        commands += f"search {key} 2500\n"

    commands += "print 2500\n"

    output, errors = run_hamt_program(commands)
    
    if errors:
        print(Fore.RED + "Errors:\n" + errors)
    else:
        print(Fore.GREEN + "Output:\n" + output)

if __name__ == "__main__":
    main()
