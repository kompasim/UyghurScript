

import os
import re
import sys
import json
import subprocess
import shutil
import yaml
sys.path.append('../my-build-tools/')

from builder import builder
tools = builder.tools

###############################################################################

PROJECT_NAME ="UyghurScript"
PROJECT_REPO ="https://github.com/Atypicalim/UyghurScript"
VERSION_CODE = 0.11
EXTENSION_VERSION = "1.0.11"
EXAMPLE_LANG = 'en'
SUPPORT_LANG = set()


###############################################################################

print("\nINITIALIZE_BASE:")

DIR_SOURCE = "./uyghur/"
DIR_EXAMPLE = "./examples/"
DIR_BUILD = "./build/"
tools.files.mk_folder(DIR_BUILD)

# source
NAME_INTERNALS = "internals"
NAME_EXTERNALS = "externals"
DIR_INTERNALS = tools.tools.append_path(DIR_SOURCE, NAME_INTERNALS)
DIR_EXTERNALS = tools.tools.append_path(DIR_SOURCE, NAME_EXTERNALS)

# example
DIR_SNIPPETS = tools.tools.append_path(DIR_EXAMPLE, "snippets")
DIR_FEATURES = tools.tools.append_path(DIR_EXAMPLE, "features")
DIR_LANGUAGE = tools.tools.append_path(DIR_EXAMPLE, "language")

DIR_SNIPPET_TRANS = tools.tools.append_path(DIR_BUILD, "snippets_trans")
DIR_SNIPPET_COLOR = tools.tools.append_path(DIR_BUILD, "snippets_color")
tools.files.delete(DIR_SNIPPET_TRANS)
tools.files.delete(DIR_SNIPPET_COLOR)
tools.files.mk_folder(DIR_SNIPPET_TRANS)
tools.files.mk_folder(DIR_SNIPPET_COLOR)

###############################################################################

# c conf
tplYamlDeclare = """char* {0} = \"{0}\";"""
tplYamlDefine = """#define {0} \"{0}\""""
#
tplYamlLineReference = """{{&{0}, "{1}"}},"""
tplYamlLineNormal = """{{"{0}", "{1}"}},"""
#
tmplYamlNameSize = "YAML_SIZE_{0}_{1}"
tmplYamlNameConf = "YAML_CONF_{0}_{1}"
# 
tplYamlFilter = """    if (pct_cstr_ends_with(tp, "{0}") == 0) {{return {1};}}"""
#
tplYamlConfig = """#define {0} {2}
static const PAIR_{3} {1}[{0}] = {{
{4}
}};
"""

############################################################################### utils

def readLines(filePath):
    _file = open(filePath, "r", encoding="utf-8")
    _content = _file.read().replace("\r\n", "\n")
    _lines = _content.split("\n")
    return _lines

def readYaml(fromPath):
    _configs = tools.files.read(fromPath, 'utf-8')
    configs = yaml.safe_load(_configs)
    return configs

def readLanguages(fileName):
    fromName = fileName.lower()
    fromPath = f"./uyghur/multilangs/{fromName}.yml"
    configs = readYaml(fromPath)
    langsArr = []
    namesArr = []
    mapLang2Name = {}
    mapName2Lang = {}
    for alias, pairs in configs.items():
        # names
        namesArr.append(alias)
        # 
        for lang, value in pairs.items():
            # langs
            if lang not in langsArr:
                langsArr.append(lang)
            #
            if lang not in mapLang2Name:
                mapLang2Name[lang] = {}
            mapLang2Name[lang][alias] = value
            #
            if alias not in mapName2Lang:
                mapName2Lang[alias] = {}
            mapName2Lang[alias][lang] = value
            #
    return langsArr, namesArr, mapLang2Name, mapName2Lang

############################################################################### yamls

langsArrLetters, namesArrLetters, mapLang2NameLetters, mapName2LangLetters = readLanguages("LETTERS")
langsArrLanguages, namesArrLanguages, mapLang2NameLanguages, mapName2LangLanguages = readLanguages("LANGUAGES")
langsArrAliases, namesArrAliases, mapLang2NameAliases, mapName2LangAliases = readLanguages("ALIASES")
langsArrConfigs, namesArrConfigs, mapLang2NameConfigs, mapName2LangConfigs = readLanguages("CONFIGS")

grammars = readYaml(f"./others/grammars.yml")
snippets = readYaml(f"./others/snippets.yml")
features = readYaml(f"./others/features.yml")

langArray = []
for lang in langsArrConfigs:
    print("language:", lang)
    SUPPORT_LANG.add(lang)

langNames = mapName2LangConfigs['LANG_LANGUAGE_FULLNAME']
langTrans = mapName2LangConfigs['LANG_LANGUAGE_TRANSLATION']

print("INITIALIZED!\n")
