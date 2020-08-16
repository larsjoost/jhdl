import sys
import argparse

def parse_input_line(line, file_contents):
    try:
        unit_type, file_name, library, unit = line.split(",")
        try:
            contents = file_contents[file_name] 
        except KeyError:
            contents = []
        contents.append((unit_type.strip(), library.strip(), unit.strip()))
        file_contents[file_name] = contents
    except ValueError:
        print("Received the following line: " + line + ", but expected the line of format: <file name>: <library>.<unit>")

def write_dependencies(file_contents):
    for i in file_contents.keys():
        file_name = i + ".i"
        with open(file_name, 'w') as fh:
            print(i + ":", file=fh, end="")
            for unit_type, library, unit in file_contents[i]:
                print(" " + library + "." + unit, file=fh, end="") 

                
argument_parser = argparse.ArgumentParser(description="Parses hdlc expression into dependency files")

argument_parser.add_argument("-f", dest="file_name", type=str, help="Name of file which is piped into script")
args = argument_parser.parse_args()

file_contents = {}

for line in sys.stdin:
    parse_input_line(line, file_contents)

write_dependencies(file_contents)

