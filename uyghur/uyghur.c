// main program

#ifndef H_UYGHUR
#define H_UYGHUR

#include "others/header.h"

#include "tokenizer.c"
#include "parser.c"
#include "executer.c"
#include "debug.c"
#include "bridge.c"

void Uyghur_init(Uyghur *this)
{
    //
}

Uyghur *Uyghur_new()
{
    Uyghur *uyghur = malloc(sizeof(Uyghur));
    Uyghur_init(uyghur);
    uyghur->tokenizer = Tokenizer_new(uyghur);
    uyghur->parser = Parser_new(uyghur);
    uyghur->executer = Executer_new(uyghur);
    uyghur->debug = Debug_new(uyghur);
    uyghur->bridge = Bridge_new(uyghur);
    return uyghur;
}

Value *Uyghur_runCode(Uyghur *this, char *code, char *path)
{
    if (path == NULL) path = UG_SCRIPT_NAME;
    log_warn("uyghur·run: %s", path);
    utils_set_languages(this, path);
    utils_add_tokens(this, path);
    log_warn("uyghur·tokenize");
    Token *headToken = Tokenizer_parseCode(this->tokenizer, path, code);
    // log_warn("uyghur·parse");
    // Leaf *headLeaf = Parser_parseTokens(this->parser, headToken);
    // log_warn("uyghur·execute");
    // Value *moduleBox = Executer_executeTree(this->executer, path, headLeaf);
    // log_warn("uyghur·runned!");
    // return moduleBox;
    return NULL;
}

Value *Uyghur_runPath(Uyghur *this, char *path)
{
    char *code = file_read(path);
    if (code == NULL) return NULL;
    return Uyghur_runCode(this, code, path);
}

Value *Uyghur_runArgs(Uyghur *this, int argc, char const *argv[])
{
    if (argc <= 1) return NULL;
    char *path = (char *)argv[1];
    bool exist = file_exist(path);
    tools_assert(exist, "%s:[%s]", LANG_ERR_NO_INPUT_FILE, path);
    Uyghur_runPath(this, path);

}

void Uyghur_free(Uyghur *this)
{
    Executer_free(this->executer);
    Parser_free(this->parser);
    Tokenizer_free(this->tokenizer);
    free(this);
}

#endif
