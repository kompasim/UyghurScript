// main program

#include "header.h"

#include "token.c"
#include "tokenizer.c"
#include "leaf.c"
#include "parser.c"

typedef struct {
    bool running;
    Tokenizer *tokenizer;
    Parser *parser;
} Uyghur;

Uyghur *Uyghur_new()
{
    Uyghur *uyghur = malloc(sizeof(Uyghur));
    uyghur->tokenizer = Tokenizer_new();
    uyghur->parser = Parser_new();
    return uyghur;
}

void Uyghur_execute(Uyghur *this, const char *path, const char *code)
{
    Token *headToken = Tokenizer_parseCode(this->tokenizer, path, code);
    Leaf *headLeaf = Parser_parseTokens(this->parser, headToken);
    printf("\n\n--->\n");
    Leaf_print(headLeaf);
    printf("-->\n\n");
}

void Uyghur_compile(Uyghur *this, const char *path)
{
    //
}

void Uyghur_run(Uyghur *this, const char *path)
{
    char *content = tools_read_file(path);
    tools_check(content != NULL, tools_format(LANG_ERR_NO_INPUT_FILE, path));
    Uyghur_execute(this, path, content);
}

void Uyghur_inport()
{
    //
}

void Uyghur_export()
{
    //
}

void Uyghur_free(Uyghur *this)
{
    Parser_free(this->parser);
    Tokenizer_free(this->tokenizer);
    free(this);
}
