def doDirectory(directoryPath, outputName):
    #out = open(outputName, "w")
    #out.close()
    for root, dir, files in os.walk(directoryPath):
        os.chdir(root)   
        os.system("echo > " + outputName)    
        for file in fnmatch.filter(files, '*.png'):
            os.system("xxd" + " -i " + file + " >> " + outputName)
            #out = subprocess.Popen(['xxd', '-i', file], 
            #    stdout=subprocess.PIPE, 
            #    stderr=subprocess.STDOUT)
            #stdout,stderr = out.communicate()
            #print(stdout.decode())
        a = open(outputName, "r")
        strings = a.readlines()
        a.close()
        names = []
        for file in fnmatch.filter(files, '*.png'):
            res = file.split(".")
            names.append({"name":("__" if res[0][0:1].isnumeric() else "") + res[0] + "_" + res[1],
                          "len":("__" if res[0][0:1].isnumeric() else "") + res[0] + "_" + res[1] + "_" + "len"})
        a = open(outputName, "a")
        a.write("void getRes(int num, void** ret, int* size){\n")
        a.write("void* __table[" + str(len(names)) + "] = {0};\n")
        a.write("int __table_len[" + str(len(names)) + "] = {0};\n")
        num = 0
        for name in names:
            _name = name["name"]
            _len = name["len"]
            a.write("__table_len[" + str(num) +"] = " + _len + "; ")
            a.write("__table[" + str(num) + "] = " + _name + ";\n")
            num = num + 1
            #a.write()
        a.write("if (num >= 0 && num < " + str(len(names)) + ")\n")
        a.write("{*ret = __table[num]; *size = __table_len[num];}\n")
        a.write("else\ngetRes(0,ret,size);")
        a.write("\n}\n")

            

if __name__ == '__main__':
    import os
    import fnmatch
    import subprocess
    import argparse
    import re
    parser = argparse.ArgumentParser(description='Convert png to c')
    parser.add_argument("--directory", "-d", help="Directory")
    parser.add_argument("--output","-o",help="Output file")
    args = parser.parse_args()
    doDirectory(args.directory, args.output)