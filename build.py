
import sys
import yaml
sys.path.append('../my-build-tools/')

from builder import builder
tools = builder.tools

###############################################################################

VERSION_CODE = 0.2
SUPPORT_LANG = set()
SCRIPT_PATH = "./examples/sınav.tr"
SCRIPT_DIR, SCRIPT_FILE, SCRIPT_EXT, SCRIPT_NAME = tools.tools.parse_path(SCRIPT_PATH)

###############################################################################

DST_DIR = "./build/"
DST_ALIAS = DST_DIR + "script"
DST_SCRIPT = DST_ALIAS + "." + SCRIPT_EXT

###############################################################################

# c conf
tplYamlDeclare = """char* {0} = \"{0}\";"""
tplYamlDefine = """#define {0} \"{0}\""""
#
tplYamlLineLanguage = """{{&{0}, "{1}"}},"""
tplYamlLineToken = """{{"{0}", "{1}"}},"""
tplYamlLineAlias = """{{"{0}", "{1}"}},"""
#
tmplYamlNameSize = "YAML_SIZE_{0}_{1}"
tmplYamlNameConf = "YAML_CONF_{0}_{1}"
# 
tplYamlFilter = """    if (pct_cstr_ends_with(tp, "{0}") == 0) {{return {1};}}"""
#
tplYamlBody = """
#define {0} {2}
static const PAIR_{3} {1}[{0}] = {{
{4}
}};
"""

def readYaml(fromPath):
    _configs = tools.files.read(fromPath, 'utf-8')
    configs = yaml.safe_load(_configs)
    aliasesArr = []
    languagesMap = {}
    for alias, pairs in configs.items():
        # aliases
        aliasesArr.append(alias)
        # languages
        for lang, value in pairs.items():
            if lang not in languagesMap:
                languagesMap[lang] = {}
            languagesMap[lang][alias] = value
    return aliasesArr, languagesMap

def formatVariables(template, aliases):
    variables = []
    for alias in aliases:
        variable = template.format(alias)
        variables.append(variable)
    return "\n".join(variables)

def formatBodies(template, name, laguages):
    bodies = []
    for lang, pairs in laguages.items():
        lang = lang.upper()
        lines = []
        for alias, value in pairs.items():
            line = template.format(alias, value)
            lines.append(line)
        _length = len(lines)
        _lines = "\n    ".join(lines)
        #
        txtYamlNameSize = tmplYamlNameSize.format(name, lang) 
        txtYamlNameConf = tmplYamlNameConf.format(name, lang) 
        #
        body = tplYamlBody.format(txtYamlNameSize, txtYamlNameConf, _length, name, "    " + _lines)
        bodies.append(body)
    _bodies = "\n".join(bodies)
    return _bodies

def formatFilters(name, laguages):
    filterSizes = []
    filterConfs = []
    for lang, pairs in laguages.items():
        lang = lang.upper()
        #
        txtYamlNameSize = tmplYamlNameSize.format(name, lang) 
        txtYamlNameConf = tmplYamlNameConf.format(name, lang) 
        #
        filterSize = tplYamlFilter.format(lang.lower(), txtYamlNameSize)
        filterConf = tplYamlFilter.format(lang.lower(), txtYamlNameConf)
        filterSizes.append(filterSize)
        filterConfs.append(filterConf)
    _filterSizes = "\n".join(filterSizes)
    _filterConfs = "\n".join(filterConfs)
    return [_filterSizes, _filterConfs]

###############################################################################

def _onMacro(arg1, arg2, arg3, arg4):
    def onMacro(code, command, argument = None):
        if command == "VARIABLES":
            return arg1
        elif command == "BODIES":
            return arg2
        elif command == "FILTER_SIZE":
            return arg3
        elif command == "FILTER_CONF":
            return arg4
    return onMacro

###############################################################################

def _Yaml2Template(name, varTpl, lineTpl):
    _name = name.lower()
    aliases, laguages = readYaml(f"./uyghur/others/multilangs/{_name}.yml")
    #
    variables = formatVariables(varTpl, aliases)
    bodies = formatBodies(lineTpl, name, laguages)
    [filterSize, filterConf] = formatFilters(name, laguages)
    #
    for lang in laguages:
        SUPPORT_LANG.add(lang)
    # 
    bldr = builder.code()
    bldr.setInput(f"./uyghur/others/templates/{_name}.tpl.h")
    bldr.setComment("//")
    bldr.setOutput(f"{DST_DIR}{_name}.h")
    bldr.onMacro(_onMacro(variables, bodies, filterSize, filterConf))
    bldr.start()
    pass

###############################################################################

_Yaml2Template("LANGUAGES", tplYamlDeclare, tplYamlLineLanguage)
_Yaml2Template("LETTERS", tplYamlDefine, tplYamlLineToken)
_Yaml2Template("ALIASES", tplYamlDefine, tplYamlLineAlias)

###############################################################################

def _onMacro():
    def onMacro(code, command, argument = None):
        if command == "VERSION_CODE":
            return code.format(VERSION_CODE)
        elif command == "VERSION_NAME":
            return code.format(VERSION_CODE)
        elif command == "SCRIPT_PATH":
            return code.format(DST_SCRIPT)
        elif command == "SCRIPT_NAME":
            return code.format("*." + SCRIPT_EXT)
        elif command == "LANGUAGE_COUNT":
            return code.format(len(SUPPORT_LANG))
        elif command == "LANGUAGE_ARRAY":
            return code.format('", "'.join(SUPPORT_LANG))
    return onMacro

# configs
bldr = builder.code()
bldr.setInput("./uyghur/others/templates/configs.tpl.h")
bldr.setComment("//")
bldr.setOutput(DST_DIR + "configs.h")
bldr.onMacro(_onMacro())
bldr.onLine(lambda line: line)
bldr.start()


###############################################################################

# script
tools.files.mk_folder(DST_DIR)
tools.files.delete(DST_SCRIPT)
if SCRIPT_PATH is not None and tools.files.is_file(SCRIPT_PATH):
    tools.files.copy(SCRIPT_PATH, DST_SCRIPT)
else:
    help_text = " # yardem \n ikrangha [merhaba, uyghur script qa xosh kepsiz ...] yezilsun\n"
    tools.files.write(DST_SCRIPT, help_text, 'utf-8')

###############################################################################

# task
task = builder.c()
task.setDebug(True)
task.setInput('./main.c')
task.setLibs([
    "incbin",
    # "string",
    "utf8",
    "raylib",
    "raygui",
])
task.addLib("utf8iter", {
    "URL": "git@github.com:adrianwk94/utf8-iterator.git",
    "BRANCH": "master",
    "DIR_I": "./source/",
})
task.setOutput(DST_ALIAS)
# task.setIcon('./resources/icon.ico')
task.addWarnings(False, [
    "unused-result",
    "discarded-qualifiers",
    "attributes",
    "incompatible-pointer-types"
])
# task.addFlags(["-I ../pure-c-tools/"])
task.start()
task.run()
