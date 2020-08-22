import sys
import argparse
import os
import re

from library_config import LibraryConfig 

class ParseExpression:

    def get_element(self, list_contents, element, expression_elements):
        return list_contents[expression_elements[element]]

    def parse_input_line(self, line, file_contents, expression_elements):
        l = [x.strip() for x in line.split(",")]
        class Contents: pass
        c = Contents()
        c.unit_type = self.get_element(l, "type", expression_elements)
        c.library = self.get_element(l, "library", expression_elements)
        c.unit = self.get_element(l, "unit", expression_elements)
        file_contents.append(c)

    def write_dependencies(self, file_name, file_contents):
        with open(file_name + ".i", 'w') as fh:
            print(file_name + ":", file=fh, end="")
            for c in file_contents:
                if "dependency" in c.unit_type:
                    print(" " + c.library + "." + c.unit, file=fh, end="") 

    def convert_variables_to_cmake_syntax(self, text):
        return re.sub(r"\$(?!\{)([\w\_\d]+)", lambda m: "${" + m.group(1) + "}", text)

    def write_cmake_include_file(self, file_name, file_contents, config):
        name = os.path.join("cmake", file_name + ".inc")
        libraries = []
        with open(name, 'w') as fh:
            for c in file_contents:
                library = c.library.lower()
                if "dependency" in c.unit_type:
                    if library not in libraries:
                        library_path = self.convert_variables_to_cmake_syntax(config.get_library_path(library))
                        libraries.append(library)
                        print("list(APPEND _dependency_libraries " + library_path + ")", file=fh) 
                elif "entity" in c.unit_type:
                    print("list(APPEND _executables " + c.unit.lower() + ")", file=fh)
                    
    def parse_input(self, file_name, expression_format, config):
        file_contents = []
        expression_elements = {}
        index = 0
        for i in expression_format.split(','):
            expression_elements[i] = index
            index += 1
        line_number = 1
        for line in sys.stdin:
            try:
                self.parse_input_line(line, file_contents, expression_elements)
            except ValueError:
                print("Line number " + str(line_number) + ": \"" + line + "\", but expected the line of format: " + expression_format)
                line_number += 1

        self.write_dependencies(file_name, file_contents)

        self.write_cmake_include_file(file_name, file_contents, config)

if __name__ == "__main__":

    argument_parser = argparse.ArgumentParser(description="Parses hdlc expression into dependency files")

    argument_parser.add_argument("-f", dest="file_name", required=True, type=str, help="Name of file which is piped into script")
    argument_parser.add_argument("-e", dest="expression_format", required=True, type=str, help="The format of the input lines")
    argument_parser.add_argument("-v", dest="verbose", action="store_true", default=False, help="verbose")
    argument_parser.add_argument("-c", dest="config_file_name", type=str, required=True,
                                 help="Name of configuration ini file containing library path description")

    args = argument_parser.parse_args()

    config = LibraryConfig(args.config_file_name)

    p = ParseExpression()

    p.parse_input(args.file_name, args.expression_format, config)

