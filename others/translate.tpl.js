
let TRANSLATE_PROJECT_REPO = '{}'; // [M[ PROJECT_REPO ]M]

let TRANSLATE_LETTERS_MAP = {
// [M[ LETTERS_MAP ]M]
};

let TRANSLATE_LANGS_ARR = [
    // [M[ LANGS_ARR ]M]
];

let TRANSLATE_LANGS_MAP = {};
Object.keys(TRANSLATE_LETTERS_MAP).forEach((name) => {
    let info = TRANSLATE_LETTERS_MAP[name];
    Object.keys(info).forEach((lang) => {
        let letter = info[lang];
        TRANSLATE_LANGS_MAP[letter] = lang;
    });
});
let TRANSLATE_LANG_KEYS = Object.keys(TRANSLATE_LANGS_MAP);
TRANSLATE_LANG_KEYS.sort((a, b) => b.length - a.length);

function __translate_ug_code(from, to, code) {
    let text = code.substring(0);
    //
    let convertMap = {};
    Object.keys(TRANSLATE_LETTERS_MAP).forEach((key) => {
        let info = TRANSLATE_LETTERS_MAP[key];
        let fromLang = info[from];
        let toLang = info[to];
        convertMap[fromLang] = toLang;
    });
    let keys = Object.keys(convertMap);
    keys.sort((a, b) => b.length - a.length);
    //
    function block2translated(block) {
        for (let i = 0; i < keys.length; i++) {
            const key = keys[i];
            const value = convertMap[key];
            if (key == block) {
                return value;
            }
            // text = text.replaceAll(key, value);
        }
        return block;
    }
    //
    let chars = Array.from(text.replace(/\r\n/g, "\n"));
    let lines = [];
    let isString = false;
    let line = "";
    for (let i = 0; i < chars.length; i++) {
        const char = chars[i];
        if (char == "[") isString = true;
        if (char == "]") isString = false;
        if (char != '\n' || isString) {
            line = line + char;
        } else {
            lines.push(line);
            line = "";
        }
    }
    if (line.length > 0) {
        lines.push(line);
        line = "";
    }
    //
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        let blocks = line.split(" ");
        for (let j = 0; j < blocks.length; j++) {
            const block = blocks[j];
            let translated = block2translated(block);
            blocks[j] = translated;
        }
        lines[i] = blocks.join(" ");
    }
    text = lines.join("\n");
    //
    return text;
}
function translate_ug_code(langOrLangs, code) {
    //
    let _from = null;
    for (let i = 0; i < TRANSLATE_LANG_KEYS.length; i++) {
        const tran = TRANSLATE_LANG_KEYS[i];
        const lang = TRANSLATE_LANGS_MAP[tran];
        if (code.includes(tran)) {
            _from = lang;
            break;
        }
    }
    if (typeof _from != 'string') throw new Error("origin lang invalid");
    // 
    let langs = [];
    if (typeof langOrLangs == 'string') {
        langs.push(langOrLangs);
    } else if (langOrLangs instanceof Array) {
        langs = langOrLangs;
    } else {
        throw new Error("target lang invalid");
    }
    if (langs.length <= 0) langs = TRANSLATE_LANGS_ARR;
    let texts = {};
    for (let index = 0; index < langs.length; index++) {
        const lang = langs[index];
        texts[lang] = __translate_ug_code(_from, lang, code);
    }
    if (typeof langOrLangs == 'string') {
        return texts[langOrLangs];
    } else {
        return texts;
    }
}

if (typeof global !== 'undefined') {
    global.translate_ug_code = translate_ug_code;
    global.TRANSLATE_LANGS_ARR = TRANSLATE_LANGS_ARR;
}
