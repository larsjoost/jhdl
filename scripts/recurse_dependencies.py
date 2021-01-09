import sys
import argparse
import subprocess
import os
import glob

from library_config import LibraryConfig 

class FileContents:

    def __init__(self, verbose=False):
        self.verbose = verbose
        self.contents = {}

    def debug(self, msg):
        if self.verbose:
            print("# " + msg)
        
    def add_dependency(self, library, unit, unit_type, contents):
        pass
    
    def add_unit(self, library, unit, unit_type, contents):
        self.debug(library + " " + unit + " " + contents)
        l = library.lower()
        u = unit.lower()
        try:
            library_contents = self.contents[l] 
        except KeyError:
            library_contents = {}
        try:
            unit_contents = library_contents[u]
        except KeyError:
            library_contents[u] = contents
            self.contents[l] = library_contents

    def to_string(self):
        return str(self.contents)

    def __str__(self):
        return self.to_string()

    def __repr__(self):
        return self.to_string()

        
class RecurseDependencies:

    def __init__(self, config_file_name, verbose=False):
        self.verbose = verbose
        self.parsed_files = {}
        self.config = LibraryConfig(config_file_name)
        
    def debug(self, msg):
        if self.verbose:
            print(msg)

    def error(self, msg):
        print("#Error: " + msg)
        
    def get_library_path(self, library):
        return self.config.get_library_path(library)
        
    def parse_line(self, line, file_name):
        unit_type, library, unit = line.split(",")
        class Contents: pass
        c = Contents()
        c.unit_type = unit_type.strip()
        c.library = library.strip()
        c.unit = unit.strip()
        c.file_name = file_name
        return c
 
    def parse_file(self, file_name, library, visitor):
        if file_name not in self.parsed_files:
            self.debug("Parsing file " + file_name + " in library " + library)
            self.parsed_files[file_name] = True
            result = subprocess.run(["hdlc", "-l", library, "-e", "%t,%l,%u", file_name], capture_output=True)
            self.debug(result.stderr.decode('utf-8'))
            for line in result.stdout.decode('utf-8').splitlines():
                line_content = self.parse_line(line, file_name)
                if line_content.unit_type in ["entity_dependency", "package_dependency"]:
                    dependency_type = line_content.unit_type.split('_')[0]
                    visitor.add_dependency(line_content.library, line_content.unit, dependency_type, file_name)
                    self.debug(file_name + ": Searching library " + line_content.library + " for " + line_content.unit + " " + dependency_type)
                    self.parse_unit(line_content.library, line_content.unit, dependency_type, visitor)
                elif line_content.unit_type in ["entity", "package"]:
                    visitor.add_unit(line_content.library, line_content.unit, line_content.unit_type, file_name)
                    self.config.set_unit_file_name(line_content.unit, line_content.library, line_content.unit_type, file_name)
                        
    def get_vhdl_files_in_library(self, library):
        library_path = self.get_library_path(library)
        files = []
        for file_extension in ["vhd", "vhdl"]:
            vhdl_files = os.path.join(os.path.expandvars(library_path), "*." + file_extension)
            files.extend(glob.glob(vhdl_files))
            self.debug("VHDL files matching " + vhdl_files + " in " + library + " : " + str(files)) 
        return files
    
    def parse_files_in_library(self, library, visitor):
        files = self.get_vhdl_files_in_library(library)
        for i in files:
            self.parse_file(i, library, visitor)
        
    def get_unit_file_name(self, unit, library, unit_type, visitor):
        file_name = self.config.get_unit_file_name(unit, library, unit_type)
        if file_name is None:
            self.debug("Did not find " + unit_type + " " + unit + " in library " + library + " config file")
            self.parse_files_in_library(library, visitor)
        file_name = self.config.get_unit_file_name(unit, library, unit_type)
        if file_name is None:
            self.error("Could not get file name of " + unit_type + " " + unit + " in library " + library)
        return file_name
    
    def parse_unit(self, library, unit, dependency_type, visitor):
        file_name = self.get_unit_file_name(unit, library, dependency_type, visitor)

    def save_config_files(self):
        self.config.save()
        
if __name__ == "__main__":        
    argument_parser = argparse.ArgumentParser(description="Parses hdlc expression into dependency files")

    argument_parser.add_argument("-f", dest="file_name", type=str, required=True, help="Name of VHDL file to parse")
    argument_parser.add_argument("-c", dest="config_file_name", type=str, required=True,
                                 help="Name of configuration ini file containing library path description")
    argument_parser.add_argument("-l", dest="library", type=str, default="work", help="Name of VHDL library")
    
    args = argument_parser.parse_args()

    visitor = FileContents(verbose=True)

    r = RecurseDependencies(config_file_name=args.config_file_name, verbose=False)

    r.parse_file(args.file_name, args.library, visitor)
    r.save_config_files()
    
