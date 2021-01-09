import configparser
import os
import argparse

class IniFileReader:

    def __init__(self, file_name):
        self.config = configparser.ConfigParser()
        self.config.read(os.path.expandvars(file_name))

    def get_key_of_value(self, section, value):
        found_key = None
        for key, val in self.config.items(section):
            if value in val:
                print(key)

if __name__ == "__main__":

    argument_parser = argparse.ArgumentParser(description="Parses ini files")

    argument_parser.add_argument("-f", dest="file_name", required=True, type=str, help="Name of file which is piped into script")
    argument_parser.add_argument("-s", dest="section", required=True, type=str, help="The ini file section")
    argument_parser.add_argument("-a", dest="value", required=True, type=str, help="The value in the ini file item")
    argument_parser.add_argument("-v", dest="verbose", action="store_true", default=False, help="verbose")

    args = argument_parser.parse_args()

    x = IniFileReader(args.file_name)
    key = x.get_key_of_value(args.section, args.value)
    if key is not None:
        print(key.lower())                 
