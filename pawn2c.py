import os
import sys
import re

e_arrays = re.compile(r"new\s+(const)?\s*([a-zA-Z_][_a-zA-Z0-9]+)\s*([\[\{].*[\}\]])+\s*=\s*([\[\{][\s\S]*?[\]\}])\s*;")
e_arg = re.compile(r"(?P<isConst>const)?\s*(?P<type>[_a-zA-Z0-9:]+)(?P<isArray>\[\])?\s*(?P<name>[_a-zA-Z0-9:]+)?")
e_func = re.compile(r"^([_a-zA-Z][_a-zA-Z0-9]+\s+)?([_a-zA-Z][_a-zA-Z0-9]+)\((.*)\)\s*{",flags=re.MULTILINE)
e_case = re.compile(r"case\s*(\d.*)*\:")
e_switches = re.compile(r"case")

def scanBasicReplacement(lines):
    linesRes = re.sub(r'#.*$', '', lines,flags=re.MULTILINE)
    linesRes = re.sub(r'forward.*$|native.*$', '', linesRes,flags=re.MULTILINE)
    linesRes = re.sub(r'public\s+', '', linesRes,flags=re.MULTILINE)
    #linesRes = re.sub(r"]\s*=\s*0\s*;", "]={0};", linesRes, flags=re.MULTILINE)
    linesRes = re.sub(r'\{\s*0\s*,?\s*(...)?\s*\}', '0', linesRes,flags=re.MULTILINE)
    linesRes = re.sub(r"bool\:", "bool ", linesRes, flags=re.MULTILINE)
    return linesRes

def scanCase(lines):
    do = True
    prevSymbol = 0
    endSymbol = len(lines)
    resultLines = ""
    while(do):        
        #pdb.set_trace()
        match = e_case.search(lines,prevSymbol)
        if ((match is None) == True):
            break
        resultLines += lines[prevSymbol:match.span()[0]] #add previous block of code
        prevSymbol = match.span()[1]        
        #if (prevSymbol == len(lines)):
        #    break
        numberStr = match.groups()[0]
        numbers = numberStr.split(",")
        for number in numbers:
            resultLines += "case "
            resultLines += number.strip()
            resultLines += ":" 
    if (len(lines) - 1 != endSymbol):
        resultLines += lines[prevSymbol:len(lines)]
    return resultLines

def newArray(isPorted,parsed):
    resultLines = ""
    def doSize(sizeStr, count):
        unparsed = list(sizeStr)
        parsed = [x for x in unparsed if x != " "]
        depth = -1
        open = False
        predefined = False
        result = []
        for symbol in parsed:
            if symbol == "[" or symbol == "{":
                depth += 1
                open = True
                result.append("[")
            elif open == True:
                if symbol == "]" or symbol == "}":
                    if predefined == False:
                        result.extend(list(str(count[depth])))
                    result.append("]")
                    open = False
                else:
                    result.append(symbol)
                    predefined = True
            elif open == False:
                if symbol == "]" or symbol == "}":
                    result.append("]")

        return "".join(result)
    def doInit(initStr):
        unparsed = list(initStr)      
        parsed = []
        for symbol in unparsed:
            if (symbol == "["):
                parsed.append("{")
            elif (symbol == "]"):
                parsed.append("}")
            else:
                parsed.append(symbol)
        #a,b,c = 0
        #for symbol in parsed:
        #    if (symbol == "{" and a == 0):
        #        a += 0
        #    elif ("}"):
        #        a
        return "".join(parsed)
    def scanInit(init):
        unparsed = list(init) 
        unparsed = [x for x in unparsed if x != " "]     
        parsed = []    
        depth = -1
        maxDepth = 0
        count = [0,0,0]
        countCounted = [False,False,False]
        for symbol in unparsed:
            if (symbol == "{"):
                depth += 1 
                if (countCounted[depth] == False):
                    count[depth] += 1
                               
            if (symbol == ","):
                if (countCounted[depth] == False):
                    count[depth] += 1
            if (symbol == "}"):
                countCounted[depth] = True
                depth -= 1
        #a,b,c = 0
        #for symbol in parsed:
        #    if (symbol == "{" and a == 0):
        #        a += 0
        #    elif ("}"):
        #        a
        return count
    init = doInit(parsed["initializer"])
    count = scanInit(init)
    if isPorted == False:
        resultLines += "new" + " "
        resultLines += (parsed["const"] + " ") if not (parsed["const"] is None) else ""
        resultLines += parsed["name"] + doSize(parsed["size"]) + " = " + parsed["initializer"] + ";\n"
    else:
        resultLines += "new" + " " + parsed["name"] + doSize(parsed["size"],count) + " = " + init + ";"        
    return resultLines
    
def scanArray(lines):
    do = True
    prevSymbol = 0
    endSymbol = len(lines)
    resultLines = ""
    while(do):
        match = e_arrays.search(lines,prevSymbol)
        if ((match is None) == True):
            break
        resultLines += lines[prevSymbol:match.span()[0]] #add previous block of code
        prevSymbol = match.span()[1]        
        #if (prevSymbol == len(lines) ):
        #    break
        parsed = {}
        parsed["const"], parsed["name"], parsed["size"],parsed["initializer"] = match.groups()   
        resultLines += newArray(isPorted=True,parsed=parsed)
    if (len(lines) - 1 != endSymbol):
        resultLines += lines[prevSymbol:len(lines)]
    return resultLines

def newFunction(isPorted,parsed):
    splitted = []
    if not (parsed["parameters"] is None):
        splitted = parsed["parameters"].split(",")
    else:
        splitted = []
    resultLines = ""
    def doArg(isPorted, argStr):
        if not argStr is None:
            argStr = argStr.strip()
        match = e_arg.search(argStr)
        if match is None:
            return ""
        isConst, type, isArray, name = match.groups()
        resultLine = ""
        if not isPorted:
            if isConst:
                resultLine += "const" + " "
            if not isArray:
                resultLine += type
            else:
                resultLine += type + "[]"
        else:
            if not isArray:
                resultLine += "cell" + " "
            else:
                resultLine += "cell*" + " "
            resultLine += type
        if name:
            resultLine += ' ' + name
        return resultLine    
    funcProto = ""
    first = True
    if isPorted == True:
        funcProto += "cell" + " "
    funcProto += parsed["name"] + "("
    for arg in splitted:
        if not (arg is None):
            if first == False:
                funcProto += "," + " "
            else:
                first = False
            funcProto += doArg(isPorted = isPorted,argStr = arg)            
    resultLines += funcProto + ")" + "\n" + "{"
    funcProto += ")" + ";" + "\n"
    return funcProto,resultLines

def scanFunctions(lines, isPorted = False):
    do = True
    prevSymbol = 0
    endSymbol = len(lines)
    resultLines = ""
    resultProto = ""
    while(do):
        match = e_func.search(lines,prevSymbol)
        if ((match is None) == True):
            break
        resultLines += lines[prevSymbol:match.span()[0]] #add previous block of code
        prevSymbol = match.span()[1]        
        if (prevSymbol == len(lines)):
            break
        parsed = {}
        type, parsed["name"], parsed["parameters"] = match.groups()
        proto, func = newFunction(isPorted=isPorted,parsed=parsed)
        resultLines += func
        resultProto += scanBasicReplacement(proto)
      
    #pdb.set_trace()
    if (len(lines) - 1 != endSymbol):
        resultLines += lines[prevSymbol:len(lines)]
    return resultProto, resultLines

def ScanSwitches(lines):
    do = True
    prevSymbol = 0
    endSymbol = len(lines)
    resultLines = ""
    while(do):
        match = e_switches.search(lines,prevSymbol)
        if ((match is None) == True):
            break
        resultLines += lines[prevSymbol:match.span()[0]] #add previous block of code
        prevSymbol = match.span()[1]        
        if (prevSymbol == len(lines)):
            break
        resultLines += "break;case"
    if (len(lines) - 1 != endSymbol):
        resultLines += lines[prevSymbol:len(lines)]
    return resultLines

def MakeSource(lines,isPorted):
    #pdb.set_trace()
    resSource = scanBasicReplacement(lines)
    resSource = scanArray(resSource)
      
    #pdb.set_trace()
    resSource = ScanSwitches(resSource)
    resSource = scanCase(resSource)
    resHeader = '#include "pawn2c.h"\n'
    resHeader += "#define new signed\n"
    resHeader += "#define bool int\n"
    resHeader += "#define false 0\n"
    resHeader += "#define true 1\n"
    resHeaderTmp, resTmp = scanFunctions(resSource,True)
    resSource = resTmp
    resHeader += resHeaderTmp
    return resHeader, resSource

def CreateFile(fileName):
    if (not os.path.isfile(fileName)):
        raise Exception("No file found")
    if os.name == 'nt':
        if (os.path.isfile("pawncc.exe")):
            os.system("pawncc.exe " + fileName + " -l PAWN2C=")
        else:
            raise Exception("No pawncc.exe found")
    else:
        if (os.path.isfile("./pawncc")):
            os.system("./pawncc " + fileName + " -l PAWN2C=")
        else:
            raise Exception("No pawncc found")

    basicFileName = re.sub(r"\.pwn|\.lst", "",fileName)
    lines = open(basicFileName + ".lst").read()
    basicFileName = basicFileName + "_ported"
    header, ported = MakeSource(lines,True)
    headerFile = open(basicFileName + ".h", "w")
    headerFile.write(header)
    headerFile.close()
    portedFile = open(basicFileName + ".c", "w")
    portedFile.write("#include \"" + os.path.basename(basicFileName) + ".h" + "\"\n")
    portedFile.write("typedef unsigned short uint16_t;\n")
    portedFile.write("#include \"resources.h\"\n")
    portedFile.write("#define run pawn_run\n")
    portedFile.write("#define pow pawn_pow\n\n")

    portedFile.write(ported)
    portedFile.close()


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise Exception("Invalid arguments. Proper usage: python3 pawn2c.py filename.pwn")
    CreateFile(sys.argv[1])
