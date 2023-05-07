# This is a sample Python script.
import io
import pathlib
# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

import sys
import argparse
from array import array
from pathlib import Path
import os
import shutil

def process_template(item, templateDict):
    item_name = item.name
    item = item.rename(item.parent.joinpath(item_name % templateDict))
    if os.path.isdir(item):
        for filename in os.listdir(item):
            process_template(item.joinpath(Path(filename)), templateDict)
    else:
        file = open(item, 'r')
        content = file.read()
        content = content % templateDict
        file.close()
        file = open(item, 'w')
        file.write(content)
        file.close()
    pass

def main():
    parser = argparse.ArgumentParser(description="Generate modules using templates")
    parser.add_argument("tArgs", metavar="Arg", type=str, nargs="+",
                        help="Argument values to be given to the template generator")
    parser.add_argument('-t', '--template', dest='templateName', type=str,
                        help='Path to the local template to use')
    parser.add_argument('-o', '--output', dest='outDir', type=str,
                        help='Output directory for generated content')
    args = parser.parse_args()
    templatePath = Path(__file__).parent.absolute().joinpath('../Templates/' + args.templateName)
    outDir = Path(os.getcwd()).absolute().joinpath(args.outDir)
    print('Template: ' + str(templatePath))
    print('Output Dir: ' + str(outDir))

    templateArgs = dict()
    for tArg in args.tArgs:
        if "=" in tArg:
            name = tArg.split("=")[0]
            templateArgs[name] = tArg.split("=")[1]
            templateArgs[name + "_Lower"] = tArg.split("=")[1].lower()
            templateArgs[name + "_Upper"] = tArg.split("=")[1].upper()
            if name == 'Name':
                templateArgs[name + "_API"] = tArg.split("=")[1].upper() + "_API"
        else:
            if tArg == "AutoRTTI":
                templateArgs[tArg] = "AUTORTTI"
            else:
                templateArgs[tArg] = True
    if "AutoRTTI" in templateArgs:
        templateArgs["AutoRTTI_Header"] = f'#include "{templateArgs["Name"]}.private.h"'
    else:
        templateArgs["AutoRTTI_Header"] = ""
    if not "AutoRTTI" in templateArgs:
        templateArgs["AutoRTTI"] = ""

    print("Generating from template with following args:")
    print(templateArgs)

    topLevelFiles = []
    for filename in os.listdir(templatePath):
        topLevelFiles.append(filename)
        src = templatePath.joinpath(filename)
        dst = outDir.joinpath(filename)
        if os.path.isdir(src):
            shutil.copytree(src, dst)
            process_template(dst, templateArgs)

    print("Successfuly generated from template at: " + str(outDir))
    return 0


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

