import sys
import argparse
import subprocess
import configparser
import os
import glob

class FileContents:

    def __init__(self, verbose=False):
        self.verbose = verbose
        self.contents = {}

    def debug(self, msg):
        if self.verbose:
            print("# " + msg)
        
    def add_file_contents(self, library, unit, contents):
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

class LibraryConfig:

    def __init__(self, file_name, verbose=False):
        self.library_config = {}
        self.file_name = file_name
        self.config = configparser.ConfigParser()
        for i in file_name.split(';'):
            self.config.read(os.path.expandvars(i))

    def error(self, msg):
        print("# Error: " + msg)
            
    def get_library_path(self, library):
        l = library.lower()
        if l == "work":
            library_path = ""
        else:
            try:
                library_path = self.config['hdl'][l]
            except KeyError:
                library_path = None
                self.error("Cound not find path of library " + library)
        return library_path
            
    def get_library_config_file_name(self, library):
        library_path = self.get_library_path(library)
        library_config_file_name = os.path.join(os.path.expandvars(library_path), ".jhdl.ini") 
        return library_config_file_name
        
    def load_library_config_file(self, library, library_config):
        l = library.lower()
        try:
            config = library_config[l]
        except KeyError:
            config = configparser.ConfigParser()
            library_config_file_name = self.get_library_config_file_name(library) 
            config.read(library_config_file_name)
            library_config[l] = config
        return config

    def get_unit_file_name(self, unit, library, unit_type):
        config = self.load_library_config_file(library, self.library_config)
        try:
            file_name = config[unit_type][unit.lower()]
        except KeyError:
            print(unit + " " + library + " " + unit_type)
            self.print_config(config)
            file_name = None
        return file_name

    def print_config(self, config):
        for i in config.sections():
            print(i + ": " + str(config[i]))
    
    def set_unit_file_name(self, unit, library, unit_type, file_name):
        print("# set_unit_file_name", unit, library, unit_type, file_name)
        config = self.load_library_config_file(library, self.library_config)
        self.print_config(config)
        if unit_type not in config.sections():
            config[unit_type] = {}
        config[unit_type][unit.lower()] = file_name
        self.print_config(config)
        
        
    def save(self):
        for library in self.library_config.keys():
            file_name = self.get_library_config_file_name(library)
            with open(file_name, 'w') as f:
                print("Writing config file " + file_name)
                self.library_config[library].write(f)
        
class RecurseDependencies:

    def __init__(self, config_file_name, verbose=False):
        self.verbose = verbose
        self.parsed_files = {}
        self.config = LibraryConfig(config_file_name)
        
    def debug(self, msg):
        if self.verbose:
            print(msg)

    def error(self, msg):
        print("# Error: " + msg)
        
    def get_library_path(self, library):
        return self.config.get_library_path(library)
        
    def parse_line(self, line):
        unit_type, library, unit = line.split(",")
        return (unit_type.strip(), library.strip(), unit.strip())
 
    def parse_file(self, file_name, library, file_contents):
        print("Parsing file " + file_name + " in library " + library)
        if file_name not in self.parsed_files:
            self.parsed_files[file_name] = True
            result = subprocess.run(["hdlc", "-l", library, "-e", "%t,%l,%u", file_name], capture_output=True)
            self.debug(result.stderr.decode('utf-8'))
            for line in result.stdout.decode('utf-8').splitlines():
                unit_type, library, unit = self.parse_line(line)
                print(file_name + ": " + unit_type + " " + library + " " + unit)
                if unit_type in ["entity_dependency", "package_dependency"]:
                    dependency_type = unit_type.split('_')[0]
                    print(file_name + ": Searching library " + library + " for " + unit + " " + dependency_type)
                    self.parse_unit(library, unit, dependency_type, file_contents)
                elif unit_type in ["entity", "package"]:
                    file_contents.add_file_contents(library, unit, file_name)
                    self.config.set_unit_file_name(unit, library, unit_type, file_name)
                        
    def get_vhdl_files_in_library(self, library):
        library_path = self.get_library_path(library)
        files = []
        for file_extension in ["vhd", "vhdl"]:
            vhdl_files = os.path.join(os.path.expandvars(library_path), "*." + file_extension)
            files.extend(glob.glob(vhdl_files))
            print("VHDL files matching " + vhdl_files + " in " + library + " : " + str(files)) 
        return files
    
    def parse_files_in_library(self, library, file_contents):
        files = self.get_vhdl_files_in_library(library)
        for i in files:
            self.parse_file(i, library, file_contents)
        
    def get_unit_file_name(self, unit, library, unit_type, file_contents):
        file_name = self.config.get_unit_file_name(unit, library, unit_type)
        if file_name is None:
            print("Did not find " + unit_type + " " + unit + " in library " + library + " config file")
            self.parse_files_in_library(library, file_contents)
        file_name = self.config.get_unit_file_name(unit, library, unit_type)
        self.error("Could not get file name of " + unit_type + " " + unit + " in library " + library)
        return file_name
    
    def parse_unit(self, library, unit, dependency_type, file_contents):
        file_name = self.get_unit_file_name(unit, library, dependency_type, file_contents)

    def save_config_files(self):
        self.config.save()
        
if __name__ == "__main__":        
    argument_parser = argparse.ArgumentParser(description="Parses hdlc expression into dependency files")

    argument_parser.add_argument("-f", dest="file_name", type=str, required=True, help="Name of VHDL file to parse")
    argument_parser.add_argument("-c", dest="config_file_name", type=str, required=True,
                                 help="Name of configuration ini file containing library path description")
    argument_parser.add_argument("-l", dest="library", type=str, default="work", help="Name of VHDL library")
    args = argument_parser.parse_args()

    file_contents = FileContents(verbose=True)

    r = RecurseDependencies(config_file_name=args.config_file_name, verbose=False)

    r.parse_file(args.file_name, args.library, file_contents)
    r.save_config_files()
    
    print(file_contents)
