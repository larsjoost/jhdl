import configparser
import os

class LibraryConfigContents:

    def print(self):
        for i in self.config.sections():
            print(i + ": " + str(self.config[i]))

    def set(self, unit_type, unit, file_name):
        if unit_type not in self.config.sections():
            self.config[unit_type] = {}
        self.config[unit_type][unit.lower()] = file_name
        self.dirty = True
            
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
            config = LibraryConfigContents()
            config.file_name = self.get_library_config_file_name(library) 
            config.config = configparser.ConfigParser()
            config.config.read(config.file_name)
            config.dirty = False
            library_config[l] = config
        return config

    def get_unit_file_name(self, unit, library, unit_type):
        config = self.load_library_config_file(library, self.library_config)
        try:
            file_name = config.config[unit_type][unit.lower()]
        except KeyError:
            self.debug(unit + " " + library + " " + unit_type)
            # config.print()
            file_name = None
        return file_name
    
    def set_unit_file_name(self, unit, library, unit_type, file_name):
        print("# set_unit_file_name", unit, library, unit_type, file_name)
        config = self.load_library_config_file(library, self.library_config)
        # config.print()
        config.set(unit_type, unit, file_name)
        #config.print()
        
        
    def save(self):
        for library in self.library_config.keys():
            c = self.library_config[library]
            with open(c.file_name, 'w') as f:
                if c.dirty:
                    print("Writing config file " + c.file_name)
                    c.config.write(f)
