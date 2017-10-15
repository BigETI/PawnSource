/*  Pawn compiler - Error message system
 *  In fact a very simple system, using only 'panic mode'.
 *
 *  Copyright (c) ITB CompuPhase, 1997-2006
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: sc5.c 3579 2006-06-06 13:35:29Z thiadmer $
 */
#include <assert.h>
#if defined	__WIN32__ || defined _WIN32 || defined __MSDOS__
  #include <io.h>
#endif
#if defined LINUX || defined __GNUC__
  #include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>     /* ANSI standardized variable argument list functions */
#include <string.h>
#if defined FORTIFY
  #include <alloc/fortify.h>
#endif
#include "sc.h"

static char *errmsg[] = {
/*001*/  "expected token: \"%s\", but found \"%s\"\n",
/*002*/  "only a single statement (or expression) can follow each \"case\"\n",
/*003*/  "declaration of a local variable must appear in a compound block\n",
/*004*/  "function \"%s\" is not implemented\n",
/*005*/  "function may not have arguments\n",
/*006*/  "must be assigned to an array\n",
/*007*/  "operator cannot be redefined\n",
/*008*/  "must be a constant expression; assumed zero\n",
/*009*/  "invalid array size (negative, zero or out of bounds)\n",
/*010*/  "invalid function or declaration\n",
/*011*/  "invalid outside functions\n",
/*012*/  "invalid function call, not a valid address\n",
/*013*/  "no entry point (no public functions)\n",
/*014*/  "invalid statement; not in switch\n",
/*015*/  "\"default\" case must be the last case in switch statement\n",
/*016*/  "multiple defaults in \"switch\"\n",
/*017*/  "undefined symbol \"%s\"\n",
/*018*/  "initialization data exceeds declared size\n",
/*019*/  "not a label: \"%s\"\n",
/*020*/  "invalid symbol name \"%s\"\n",
/*021*/  "symbol already defined: \"%s\"\n",
/*022*/  "must be lvalue (non-constant)\n",
/*023*/  "array assignment must be simple assignment\n",
/*024*/  "\"break\" or \"continue\" is out of context\n",
/*025*/  "function heading differs from prototype\n",
/*026*/  "no matching \"#if...\"\n",
/*027*/  "invalid character constant\n",
/*028*/  "invalid subscript (not an array or too many subscripts): \"%s\"\n",
/*029*/  "invalid expression, assumed zero\n",
/*030*/  "compound statement not closed at the end of file (started at line %d)\n",
/*031*/  "unknown directive\n",
/*032*/  "array index out of bounds (variable \"%s\")\n",
/*033*/  "array must be indexed (variable \"%s\")\n",
/*034*/  "argument does not have a default value (argument %d)\n",
/*035*/  "argument type mismatch (argument %d)\n",
/*036*/  "empty statement\n",
/*037*/  "invalid string (possibly non-terminated string)\n",
/*038*/  "extra characters on line\n",
/*039*/  "constant symbol has no size\n",
/*040*/  "duplicate \"case\" label (value %d)\n",
/*041*/  "invalid ellipsis, array size is not known\n",
/*042*/  "invalid combination of class specifiers\n",
/*043*/  "character constant exceeds range for packed string\n",
/*044*/  "positional parameters must precede all named parameters\n",
/*045*/  "too many function arguments\n",
/*046*/  "unknown array size (variable \"%s\")\n",
/*047*/  "array sizes do not match, or destination array is too small\n",
/*048*/  "array dimensions do not match\n",
/*049*/  "invalid line continuation\n",
/*050*/  "invalid range\n",
/*051*/  "invalid subscript, use \"[ ]\" operators on major dimensions\n",
/*052*/  "multi-dimensional arrays must be fully initialized\n",
/*053*/  "exceeding maximum number of dimensions\n",
/*054*/  "unmatched closing brace (\"}\")\n",
/*055*/  "start of function body without function header\n",
/*056*/  "arrays, local variables and function arguments cannot be public (variable \"%s\")\n",
/*057*/  "unfinished expression before compiler directive\n",
/*058*/  "duplicate argument; same argument is passed twice\n",
/*059*/  "function argument may not have a default value (variable \"%s\")\n",
/*060*/  "multiple \"#else\" directives between \"#if ... #endif\"\n",
/*061*/  "\"#elseif\" directive follows an \"#else\" directive\n",
/*062*/  "number of operands does not fit the operator\n",
/*063*/  "function result tag of operator \"%s\" must be \"%s\"\n",
/*064*/  "cannot change predefined operators\n",
/*065*/  "function argument may only have a single tag (argument %d)\n",
/*066*/  "function argument may not be a reference argument or an array (argument \"%s\")\n",
/*067*/  "variable cannot be both a reference and an array (variable \"%s\")\n",
/*068*/  "invalid rational number precision in #pragma\n",
/*069*/  "rational number format already defined\n",
/*070*/  "rational number support was not enabled\n",
/*071*/  "user-defined operator must be declared before use (function \"%s\")\n",
/*072*/  "\"sizeof\" operator is invalid on \"function\" symbols\n",
/*073*/  "function argument must be an array (argument \"%s\")\n",
/*074*/  "#define pattern must start with an alphabetic character\n",
/*075*/  "input line too long (after substitutions)\n",
/*076*/  "syntax error in the expression, or invalid function call\n",
/*077*/  "malformed UTF-8 encoding, or corrupted file: %s\n",
/*078*/  "function uses both \"return\" and \"return <value>\"\n",
/*079*/  "inconsistent return types (array & non-array)\n",
/*080*/  "unknown symbol, or not a constant symbol (symbol \"%s\")\n",
/*081*/  "cannot take a tag as a default value for an indexed array parameter (symbol \"%s\")\n",
/*082*/  "user-defined operators and native functions may not have states\n",
/*083*/  "a function or variable may only belong to a single automaton (symbol \"%s\")\n",
/*084*/  "state conflict: one of the states is already assigned to another implementation (symbol \"%s\")\n",
/*085*/  "no states are defined for symbol \"%s\"\n",
/*086*/  "unknown automaton \"%s\"\n",
/*087*/  "unknown state \"%s\" for automaton \"%s\"\n",
/*088*/  "public variables and local variables may not have states (symbol \"%s\")\n",
/*089*/  "state variables may not be initialized (symbol \"%s\")\n",
/*090*/  "public functions may not return arrays (symbol \"%s\")\n",
/*091*/  "ambiguous constant; tag override is required (symbol \"%s\")\n"
};

static char *fatalmsg[] = {
/*100*/  "cannot read from file: \"%s\"\n",
/*101*/  "cannot write to file: \"%s\"\n",
/*102*/  "table overflow: \"%s\"\n",
          /* table can be: loop table
           *               literal table
           *               staging buffer
           *               option table (response file)
           *               peephole optimizer table
           */
/*103*/  "insufficient memory\n",
/*104*/  "invalid assembler instruction \"%s\"\n",
/*105*/  "numeric overflow, exceeding capacity\n",
/*106*/  "compiled script exceeds the maximum memory size (%ld bytes)\n",
/*107*/  "too many error messages on one line\n",
/*108*/  "codepage mapping file not found\n",
/*109*/  "invalid path: \"%s\"\n",
/*110*/  "assertion failed: %s\n",
/*111*/  "user error: %s\n"
};

static char *warnmsg[] = {
/*200*/  "symbol \"%s\" is truncated to %d characters\n",
/*201*/  "redefinition of constant/macro (symbol \"%s\")\n",
/*202*/  "number of arguments does not match definition\n",
/*203*/  "symbol is never used: \"%s\"\n",
/*204*/  "symbol is assigned a value that is never used: \"%s\"\n",
/*205*/  "redundant code: constant expression is zero\n",
/*206*/  "redundant test: constant expression is non-zero\n",
/*207*/  "unknown #pragma\n",
/*208*/  "function with tag result used before definition, forcing reparse\n",
/*209*/  "function \"%s\" should return a value\n",
/*210*/  "possible use of symbol before initialization: \"%s\"\n",
/*211*/  "possibly unintended assignment\n",
/*212*/  "possibly unintended bitwise operation\n",
/*213*/  "tag mismatch\n",
/*214*/  "possibly a \"const\" array argument was intended: \"%s\"\n",
/*215*/  "expression has no effect\n",
/*216*/  "nested comment\n",
/*217*/  "loose indentation\n",
/*218*/  "old style prototypes used with optional semicolumns\n",
/*219*/  "local variable \"%s\" shadows a variable at a preceding level\n",
/*220*/  "expression with tag override must appear between parentheses\n",
/*221*/  "label name \"%s\" shadows tag name\n",
/*222*/  "number of digits exceeds rational number precision\n",
/*223*/  "redundant \"sizeof\": argument size is always 1 (symbol \"%s\")\n",
/*224*/  "indeterminate array size in \"sizeof\" expression (symbol \"%s\")\n",
/*225*/  "unreachable code\n",
/*226*/  "a variable is assigned to itself (symbol \"%s\")\n",
/*227*/  "more initiallers than enum fields\n",
/*228*/  "length of initialler exceeds size of the enum field\n",
/*229*/  "index tag mismatch (symbol \"%s\")\n",
/*230*/  "no implementation for state \"%s\" in function \"%s\", no fall-back\n",
/*231*/  "state specification on forward declaration is ignored\n",
/*232*/  "output file is written, but with compact encoding disabled\n",
/*233*/  "state variable \"%s\" shadows a global variable\n",
/*234*/  "function is deprecated (symbol \"%s\") %s\n",
/*235*/  "public function lacks forward declaration (symbol \"%s\")\n",
/*236*/  "unknown parameter in substitution (incorrect #define pattern)\n",
/*237*/  "user warning: %s\n"
};

static char *errmsgrus[]={
#ifndef SCPACK
	/*001*/  "ожидался токен \"%s\", но найден \"%s\"\n",
	/*002*/  "только одно действие (или выражение) может следовать после каждого \"case\"\n",
	/*003*/  "объявление локальной переменной должно быть в составном блоке\n",
	/*004*/  "функция \"%s\" не реализована\n",
	/*005*/  "у функции не должно быть аргументов\n",
	/*006*/  "должно быть связано с массивом\n",
	/*007*/  "оператор не может быть переобъявлен\n",
	/*008*/  "должно быть постоянным выражением; принят ноль\n",
	/*009*/  "неверный размер массива (отрицательный/нулевой размер или выход за пределы массива)\n",
	/*010*/  "неверная функция или объявление\n",
	/*011*/  "неверная внешняя функция\n",
	/*012*/  "неверный вызов функции, неправильный адрес\n",
	/*013*/  "отсутствует точка входа (нет public-функций)\n",
	/*014*/  "неверное действие; за пределами switch\n",
	/*015*/  "вариант \"default\" должен быть последним в конструкции switch\n",
	/*016*/  "несколько \"default\" в \"switch\"\n",
	/*017*/  "необъявленный символ \"%s\"\n",
	/*018*/  "инициализационные данные превышают объявленный размер\n",
	/*019*/  "неизвестная метка: \"%s\"\n",
	/*020*/  "неверное название идентификатора \"%s\"\n",
	/*021*/  "идентификатор \"%s\" уже объявлен\n",
	/*022*/  "должно быть lvalue (не константа)\n",
	/*023*/  "присваивание массива должно быть простым присваиванием\n",
	/*024*/  "\"break\" или \"continue\" вне цикла\n",
	/*025*/  "заголовок функции отличается от прототипа\n",
	/*026*/  "несоответствие \"#if...\"\n",
	/*027*/  "неверная символьная константа\n",
	/*028*/  "неверный индекс (не массив или слишком много индексов): \"%s\"\n",
	/*029*/  "неправильное выражение, принят ноль\n",
	/*030*/  "составное действие не закрыто в конце файла (начало на строке %d)\n",
	/*031*/  "неизвестная директива\n",
	/*032*/  "выход за пределы массива (переменная \"%s\")\n",
	/*033*/  "массив должен быть проиндексирован (переменная \"%s\")\n",
	/*034*/  "у аргумента нет значения по умолчанию (аргумент %d)\n",
	/*035*/  "несовпадение типов аргументов (аргумент %d)\n",
	/*036*/  "пустое действие\n",
	/*037*/  "неправильная строка (возможно, незавершённая строка)\n",
	/*038*/  "дополнительные символы в конце строки\n",
	/*039*/  "у константы нет размера\n",
	/*040*/  "повторяющиеся метки \"case\" (значение %d)\n",
	/*041*/  "неверный эллипсис, размер массива неизвестен\n",
	/*042*/  "неверная комбинация определений\n",
	/*043*/  "символьная константа выходит за пределы значений для упакованной строки\n",
	/*044*/  "позициональные параметры должны быть перед именованными параметрами\n",
	/*045*/  "слишком много аргументов функции\n",
	/*046*/  "неизвестный размер массива (переменная \"%s\")\n",
	/*047*/  "размеры массивов не совпадают или размер целевого массива слишком мал\n",
	/*048*/  "измерения массивов не совпадают\n",
	/*049*/  "неверное продолжение строки\n",
	/*050*/  "неверный диапазон\n",
	/*051*/  "неверный индекс, используйте операторы \"[ ]\" на старших измерениях\n",
	/*052*/  "многомерные массивы должны быть полностью инициализированы\n",
	/*053*/  "превышено максимальное количество измерений\n",
	/*054*/  "несоответствующая закрывающая скобка (\"}\")\n",
	/*055*/  "начало тела функции без заголовка\n",
	/*056*/  "массивы, локальные переменные и аргументы функций не могут иметь атрибута public (переменная \"%s\")\n",
	/*057*/  "незавершённое выражение перед директивой компилятора\n",
	/*058*/  "повторяющийся аргумент; один и тот же аргумент передан дважды\n",
	/*059*/  "у аргументов функции не должно быть значения по умолчанию (переменная \"%s\")\n",
	/*060*/  "множественные директивы \"#else\" перед \"#if ... #endif\"\n",
	/*061*/  "директива \"#elseif\" не должна быть после директивы \"#else\"\n",
	/*062*/  "количество операндов не соответствуют оператору\n",
	/*063*/  "тип результата функции оператора \"%s\" должен быть \"%s\"\n",
	/*064*/  "невозможно изменить встроенные операторы\n",
	/*065*/  "аргумент функции может иметь только один тип (аргумент %d)\n",
	/*066*/  "аргумент функции не должен быть массивом или передаваться по ссылке (аргумент \"%s\")\n",
	/*067*/  "переменная cannot be both a reference and an array (переменная \"%s\")\n",
	/*068*/  "неверная точность рациональных чисел в #pragma\n",
	/*069*/  "формат рациональных чисел уже объявлен\n",
	/*070*/  "поддержка рациональных чисел не включена\n",
	/*071*/  "пользовательский оператор должен быть объявлен перед использованием (функция \"%s\")\n",
	/*072*/  "оператор \"sizeof\" не должен применяться на символах \"function\"\n",
	/*073*/  "аргумент функции должен быть массивом (аргумент \"%s\")\n",
	/*074*/  "шаблон #define должен начинаться с символа лат. алфавита (A-Z, a-z)\n",
	/*075*/  "слишком длинная строка (после замены)\n",
	/*076*/  "синтаксическая ошибка в выражении или неправильный вызов функции\n",
	/*077*/  "неправильная кодировка UTF-8 или файл повреждён: %s\n",
	/*078*/  "функция использует и \"return\", и \"return <значение>\"\n",
	/*079*/  "несовместимый возвращаемый тип (массив и немассив)\n",
	/*080*/  "неизвестный идентификатор или константа (идентификатор \"%s\")\n",
	/*081*/  "невозможно принять тип, как значение по умолчанию для параметра индексированного массива (идентификатор \"%s\")\n",
	/*082*/  "пользовательские операторы и native-функции не должны иметь состояний (state)\n",
	/*083*/  "функция или переменная может принадлежать только одному автоматону (идентификатор \"%s\")\n",
	/*084*/  "конфликт состояний: одно из состояний уже присвоено другой реализации (идентификатор \"%s\")\n",
	/*085*/  "no states are defined for идентификатора \"%s\"\n",
	/*086*/  "неизвестный автоматон \"%s\"\n",
	/*087*/  "неизвестное состояние \"%s\" для автоматона \"%s\"\n",
	/*088*/  "public-переменные и локальные переменные не могут иметь состояний (идентификатор \"%s\")\n",
	/*089*/  "state-переменные не могут быть инициализированы (идентификатор \"%s\")\n",
	/*090*/  "public-функции не могут возвращать массивы (идентификатор \"%s\")\n",
	/*091*/  "неопределённая константа; требуется указание типа (идентификатор \"%s\")\n"
#else
	"\271pect\235\306k\212:\227\316bu\202fo\217\206\216\012",
	"\201l\223\252s\203g\352\315e\234\202(\255\363\201) c\355f\240\344w ea\275 \042c\351e\042\012",
	"\233cl\333\237\304\252\344c\337\327\324appe\204 \372\252\343\364o\217\206\236ock\012",
	"\366\227 \272\242i\364le\234t\274\012",
	"\273\367\242\340\376\265t\314",
	"\360a\253gn\235\306 \355\256y\012",
	"\353\224\255c\226\242\313\220\326\274\012",
	"\360\252\354\202\363\201; \351\375m\235z\210o\012",
	"\266\303\335\200(nega\205ve\316z\210\371\255ou\202\304bo\217ds\232",
	"\266\273\255\233cl\333\214\012",
	"\266out\222d\200\366\314",
	"\266\273c\211l\316\242\252\261add\220s\314",
	"\221 \212tr\223po\203\202(\221 pu\236\332 \366s\232",
	"\266\315e\234t; \242\372s\362t\275\012",
	"\042\302a\342t\330c\351\200\360\270\200l\351\202c\351\200\372s\362t\275 \315e\234t\012",
	"m\342\205p\352\302a\342t\207\372\042s\362t\275\042\012",
	"\217\326\235\277\012",
	"\203i\205\211iza\237d\224\252\271ce\274\207\233cl\204\235\335\322",
	"\242\252lab\341\347",
	"\266\250 nam\200\216\012",
	"\250 \211\220ad\223\326\274\347",
	"\360l\243u\200(n\201-\354t\232",
	"\303a\253gn\234\202\360\222\364\352a\253gn\234t\012",
	"\042b\220ak\330\255\042\307t\203ue\330\272ou\202\304\307t\271t\012",
	"\273head\357\334ff\210\207from pro\306typ\322",
	"\221 \345\275\357\042#if...\042\012",
	"\266\275\333ct\264\354t\012",
	"\266\375bscrip\202(\242\355\303\255\306\371m\226\223\375bscripts)\347",
	"\266\363\201\316\351\375m\235z\210o\012",
	"\343\364o\217\206\315e\234\202\242c\344s\235a\202\270\200\212\206\304\361\352(\231\204t\235a\202l\203\200%d\232",
	"\217k\221w\336\334\220c\205v\322",
	"\303\203\233x ou\202\304bo\217d\207(\327\216\232",
	"\303\360\203\233x\235(\327\216\232",
	"\312do\310\242\340\376\252\302a\342\202\243u\200(\312%d\232",
	"\312typ\200mis\345\275 (\312%d\232",
	"e\364t\223\315e\234t\012",
	"\266\231r\357(po\253\236\223n\201-\365m\203\224\235\231r\203g\232",
	"\271t\241 \275\333c\365\207\331l\203\322",
	"\354\202\250 \340\207\221 \335\322",
	"dupl\332\224\200\042c\351e\330lab\341 (\243u\200%d\232",
	"\266\341lip\222s\316\303\335\200\272\242k\221wn\012",
	"\266\343\230\203a\237\304cl\351\207speci\361\210\314",
	"\275\333ct\264\354\202\271ce\274\207r\226g\200f\255pack\235\231r\203g\012",
	"po\222\214\337p\333me\365\207\324\311c\274\200\211l nam\235p\333me\365\314",
	"\306\371m\226\223\273\265t\314",
	"\217k\221w\336\303\335\200(\327\216\232",
	"\303\335\310d\371\242\345\275\316\255\233\231\203a\237\303\272\306\371sm\211l\012",
	"\303\334\234\222\201\207d\371\242\345\275\012",
	"\266l\203\200\307t\203ua\214\012",
	"\266r\226g\322",
	"\266\375bscript\316\251\200\042[ ]\330\353\224\225\207\331\305j\255\334\234\222\201\314",
	"m\342\205-\334\234\222\201\337\256y\207\360f\342l\223\203i\205\211iz\274\012",
	"\271ce\274\357\305ximum \374\230\264\304\334\234\222\201\314",
	"\217\345\275\235c\344s\357b\241c\200(\042}\042\232",
	"\231\204\202\304\273bod\223\362\270ou\202\273head\210\012",
	"\256ys\316\344c\337\301\310\226\206\273\265t\207c\226\242\313pu\236\332 (\327\216\232",
	"\217f\203ish\235\363\331be\370\200\343\364il\264\334\220c\205v\322",
	"dupl\332\224\200\265t; sam\200\312\272p\351s\235tw\332\322",
	"\273\312\367\242\340\376\252\302a\342\202\243u\200(\327\216\232",
	"m\342\205p\352\042#\341se\330\334\220c\205v\310betwe\212 \042#if ... #\212\334f\042\012",
	"\042#\341seif\330\334\220c\205\376f\240\344w\207\355\042#\341se\330\334\220c\205v\322",
	"\374\230\264\304\353\226d\207do\310\242\361\202\270\200\353\224\225\012",
	"\273\220s\342\202\373\304\353\224\225\227 \360\216\012",
	"c\226\242\275\226g\200\311\326\235\353\224\225\314",
	"\273\312\367\201l\223\340\376\252s\203g\352\373(\312%d\232",
	"\273\312\367\242\313\252\220f\210\212c\200\312\255\355\303(\312\216\232",
	"\327c\226\242\313bo\270 \252\220f\210\212c\200\226\206\355\303(\327\216\232",
	"\266\241\214\337\374\230\264\311ci\222\331\372#p\241g\305\012",
	"\241\214\337\374\230\264\370\305\202\211\220ad\223\326\274\012",
	"\241\214\337\374\230\264\375pp\225\202wa\207\242\212\254\274\012",
	"\251\210-\326\235\353\224\255\360\233cl\204\235be\370\200\251\200(\366\227\232",
	"\042\335e\267\330\353\224\255\272\266\331\042\366\330\250\314",
	"\273\312\360\355\303(\312\216\232",
	"#\326\200p\224\365\336\324\231\204\202\362\270 \355\211p\340be\205c \275\333c\365\012",
	"\203pu\202l\203\200\306\371l\201\260(aft\264\375bs\205tu\214s\232",
	"\246n\325x \210r\255\372\270\200\363\201\316\255\266\273c\211l\012",
	"m\211\370m\235UTF-8 \212\343d\203g\316\255c\225rupt\235\361le: \213\012",
	"\273\251\310bo\270 \042\220turn\330\226\206\042\220tur\336<\243ue>\042\012",
	"\203\307\222\231\212\202\220tur\336typ\310(\303& n\201-\256y\232",
	"\217k\221w\336\250\316\255\242\252\354\202\250 \323",
	"c\226\242\325k\200\252\373a\207\252\302a\342\202\243u\200f\255\355\203\233x\235\303p\333met\264\323",
	"\251\210-\326\235\353\224\225\207\226\206na\205\376\366\207\367\242\340\376\315e\314",
	"\252\273\255\327\367\201l\223b\341\201\260\306 \252s\203g\352au\306\345\331\323",
	"\315\200\307fl\332t: \201\200\304\270\200\315\310\272\211\220ad\223a\253gn\235\306 a\221\270\264i\364le\234\325\237\323",
	"\221 \315\310\204\200\326\235f\255\277\012",
	"\217k\221w\336au\306\345\201\321",
	"\217k\221w\336\315\200\216 f\255au\306\345\201\321",
	"pu\236\332 \301\310\226\206\344c\337\301\310\367\242\340\376\315\310\323",
	"\315\200\301\310\367\242\313\203i\205\211iz\235\323",
	"pu\236\332 \366\207\367\242\220tur\336\256y\207\323",
	"a\230i\262ou\207\354t; \373ov\210rid\200\272\220qui\220\206\323"
#endif
};

static char *fatalmsgrus[]={
#ifndef SCPACK
	/*100*/  "невозможно прочесть файл: \"%s\"\n",
	/*101*/  "невозможно записать файл: \"%s\"\n",
	/*102*/  "переполнение таблицы: \"%s\"\n",
	/* table can be: loop table
	*               literal table
	*               staging buffer
	*               option table (response file)
	*               peephole optimizer table
	*/
	/*103*/  "нехватка памяти\n",
	/*104*/  "неверная ассемблерная инструкция \"%s\"\n",
	/*105*/  "численное переполнение\n",
	/*106*/  "скомпилированный скрипт превышает максимальный размер памяти (%ld байт)\n",
	/*107*/  "слишком много ошибок на одной строке\n",
	/*108*/  "не найден файл кодировок\n",
	/*109*/  "неверный путь: \"%s\"\n",
	/*110*/  "проверка assert провалена: %s\n",
	/*111*/  "пользовательская ошибка: %s\n",
#else
	"c\226\242\220a\206from \361le\347",
	"c\226\242writ\200\306 \361le\347",
	"t\254\200ov\210f\344w\347",
	"\203\375ff\332i\212\202mem\225y\012",
	"\266\351se\230l\264\203\231ruc\214\321",
	"\374m\210\332 ov\210f\344w\316\271ce\274\357capacity\012",
	"\343\364il\235scrip\202\271ce\274\207\270\200\305ximum mem\225\223\335\200(%l\206bytes\232",
	"\306\371m\226\223\210r\255messag\310\331\201\200l\203\322",
	"\343\233pag\200\305pp\357\361\352\242fo\217d\012",
	"\266p\224h\347",
	"\351s\210\237fail\274: \213\012",
	"\251\264\210r\225: \213\012"
#endif
};

static char *warnmsgrus[]={
#ifndef SCPACK
	/*200*/  "символ \"%s\" обрезан до %d символов\n",
	/*201*/  "повторное объявление константы/макроса (идентификатор \"%s\")\n",
	/*202*/  "количество аргументов не совпадает с объявлением\n",
	/*203*/  "идентификатор не используется: \"%s\"\n",
	/*204*/  "идентификатору присвоено значение, но он не используется: \"%s\"\n",
	/*205*/  "лишний код: ложное постоянное выражение\n",
	/*206*/  "лишний код: постоянное выражение истинно\n",
	/*207*/  "неизвестная #pragma\n",
	/*208*/  "функция с типовым результатом используется перед объявлением, инициирована переобработка\n",
	/*209*/  "функция \"%s\" должна возвращать значение\n",
	/*210*/  "возможное использование идентификатора без инициализации: \"%s\"\n",
	/*211*/  "возможно непреднамеренное присваивание\n",
	/*212*/  "возможно непреднамеренная побитовая операция\n",
	/*213*/  "несовпадение типов\n",
	/*214*/  "возможно рассчитывался постоянный массив (const): \"%s\"\n",
	/*215*/  "выражение не имеет эффекта\n",
	/*216*/  "вложенный комментарий\n",
	/*217*/  "неправильная табуляция\n",
	/*218*/  "старый стиль прототипов с необязательным символом \";\" в конце\n",
	/*219*/  "переменная \"%s\" уже объявлена в более глобальной зоне видимости\n",
	/*220*/  "выражение с указанием типа должно быть перед круглыми скобками\n",
	/*221*/  "название метки \"%s\" перекрывает тип с таким же названием\n",
	/*222*/  "количество цифр превышает предел точности рациональных чисел\n",
	/*223*/  "лишний оператор \"sizeof\": размер операнда всегда 1 (идентификатор \"%s\")\n",
	/*224*/  "неизвестный размер массива в выражении \"sizeof\" (идентификатор \"%s\")\n",
	/*225*/  "недостижимый код\n",
	/*226*/  "присваивание переменной самой себе (идентификатор \"%s\")\n",
	/*227*/  "инициализаторов больше, чем полей в enum\n",
	/*228*/  "длина инициализатора превышает размер поля enum\n",
	/*229*/  "несовпадение типов индекса (идентификатор \"%s\")\n",
	/*230*/  "нет реализации для состояния \"%s\" в функции \"%s\"\n",
	/*231*/  "спецификация состояния в опережающем объявлении проигнорирована\n",
	/*232*/  "выходной файл записан, но компактная кодировка отключена\n",
	/*233*/  "state-переменная \"%s\" перекрывает глобальную переменную с тем же названием\n",
	/*234*/  "устаревшая функция (идентификатор \"%s\") %s\n",
	/*235*/  "public-функция нуждается в опережающем объявлении (идентификатор \"%s\")\n",
	/*236*/  "неизвестный параметр в выражении для замены (неправильное выражение #define)\n",
	/*237*/  "пользовательское предупреждение: %s\n",
#else
	"\277 \272tr\244\224\235\306 %\206\275\333c\365\314",
	"\220\326i\237\304\354t/\305cr\371\323",
	"\374\230\264\304\265t\207do\310\242\345\275 \326i\214\012",
	"\250 \272nev\264\251\274\347",
	"\250 \272a\253gn\235\252\243u\200\270a\202\272nev\264\251\274\347",
	"\220d\217d\226\202\343\233: \354\202\363\331\272z\210o\012",
	"\220d\217d\226\202te\231: \354\202\363\331\272n\201-z\210o\012",
	"\217k\221w\336#p\241g\305\012",
	"\273\362\270 \373\220s\342\202\251\235be\370\200\326i\214\316\370c\357\220p\204s\322",
	"\366\227 sho\342\206\220tur\336\252\243u\322",
	"po\253\236\200\251\200\304\250 be\370\200\203i\205\211iza\214\347",
	"po\253\236\223\217\203t\212\233\206a\253gn\234t\012",
	"po\253\236\223\217\203t\212\233\206bit\362s\200\353a\214\012",
	"\373mis\345\275\012",
	"po\253\236\223\252\042\346\330\303\312wa\207\203t\212\233d\347",
	"\363\331\340\207\221 effect\012",
	"ne\231\235\343m\234t\012",
	"\344os\200\203d\212\325\214\012",
	"\240\206\231y\352pro\306typ\310\251\235\362\270 \350\214\337sem\332\240umn\314",
	"\344c\337\327\216 s\340dow\207\252\327a\202\252\311c\274\357lev\341\012",
	"\363\331\362\270 \373ov\210rid\200\324appe\204 betwe\212 p\204\212\270ese\314",
	"lab\341 nam\200\216 s\340dow\207\373nam\322",
	"\374\230\264\304\334git\207\271ce\274\207\241\214\337\374\230\264\311ci\222\201\012",
	"\220d\217d\226\202\042\335e\267\042: \312\335\200\272\211way\2071 \323",
	"\203\233\365m\203\224\200\303\335\200\372\042\335e\267\330\363\331\323",
	"\217\220a\275\254\200\343\233\012",
	"\252\327\272a\253gn\235\306 its\341f \323",
	"m\225\200\203i\205\211l\210\207\270\355\212um \361\341d\314",
	"l\212g\270 \304\203i\205\211l\264\271ce\274\207\335\200\304\270\200\212um \361\341d\012",
	"\203\233x \373mis\345\275 \323",
	"\221 i\364le\234\325\237f\255\315\200\216 \372\366\227\316\221 f\211l-back\012",
	"\315\200specif\332a\237\331\370w\204\206\233cl\333\237\272ig\221\220d\012",
	"outpu\202\361\352\272writt\212\316bu\202\362\270 \343\364ac\202\212\343d\357\334s\254\274\012",
	"\315\200\327\216 s\340dow\207\252g\344b\337\301\322",
	"\273\272\233\311c\224\235\317) \213\012",
	"pu\236\332 \273lack\207\370w\204\206\233cl\333\237\323",
	"\217k\221w\336p\333met\264\372\375bs\205tu\237(\203c\225\220c\202#\326\200p\224\365n\232"
#endif
};

#define NUM_WARNINGS    (sizeof warnmsg / sizeof warnmsg[0])
static struct s_warnstack {
  unsigned char disable[(NUM_WARNINGS + 7) / 8]; /* 8 flags in a char */
  struct s_warnstack *next;
} warnstack;

int errflag;
static int errstart;    /* line number at which the instruction started */
static int errline;     /* forced line number for the error message */

/*  error
 *
 *  Outputs an error message (note: msg is passed optionally).
 *  If an error is found, the variable "errflag" is set and subsequent
 *  errors are ignored until lex() finds a semicolumn or a keyword
 *  (lex() resets "errflag" in that case).
 *
 *  Global references: inpfname   (reffered to only)
 *                     fline      (reffered to only)
 *                     fcurrent   (reffered to only)
 *                     errflag    (altered)
 */
SC_FUNC int error(int number,...)
{
static char *prefix[3]={ "error", "fatal error", "warning" };
static int lastline,errorcount;
static short lastfile;
  char *msg,*pre;
  char *msgrus;
  va_list argptr;

  /* errflag is reset on each semicolon.
   * In a two-pass compiler, an error should not be reported twice. Therefore
   * the error reporting is enabled only in the second pass (and only when
   * actually producing output). Fatal errors may never be ignored.
   */
  if ((errflag || sc_status!=statWRITE) && (number<100 || number>=200))
    return 0;

  /* also check for disabled warnings */
  if (number>=200) {
    int index=(number-200)/8;
    int mask=1 << ((number-200)%8);
    if ((warnstack.disable[index] & mask)!=0)
      return 0;
  } /* if */

  if (number<100){
    msg=errmsg[number-1];
	msgrus=errmsgrus[number-1];
    pre=prefix[0];
    errflag=TRUE;       /* set errflag (skip rest of erroneous expression) */
    errnum++;
  } else if (number<200){
    msg=fatalmsg[number-100];
	msgrus=fatalmsgrus[number-100];
    pre=prefix[1];
    errnum++;           /* a fatal error also counts as an error */
  } else {
    msg=warnmsg[number-200];
	msgrus=warnmsgrus[number-200];
    pre=prefix[2];
	if(number!=217 && number!=203 && number!=204) warnnum++;
  } /* if */

  assert(errstart<=fline);
  if (errline>0)
    errstart=errline;
  else
    errline=fline;
  assert(errstart<=errline);
  va_start(argptr,number);
  if (strlen(errfname)==0) {
    int start= (errstart==errline) ? -1 : errstart;
	pc_error_rus(number,msgrus,inpfname,start,errline,argptr);
    if (pc_error(number,msg,inpfname,start,errline,argptr)) {
      if (outf!=NULL) {
        pc_closeasm(outf,TRUE);
        outf=NULL;
      } /* if */
      longjmp(errbuf,3);        /* user abort */
    } /* if */
  } else {
    FILE *fp=fopen(errfname,"a");
    if (fp!=NULL) {
      if (errstart>=0 && errstart!=errline)
        fprintf(fp,"%s(%d -- %d) : %s %03d: ",inpfname,errstart,errline,pre,number);
      else
        fprintf(fp,"%s(%d) : %s %03d: ",inpfname,errline,pre,number);
      vfprintf(fp,msg,argptr);
      fclose(fp);
    } /* if */
  } /* if */
  va_end(argptr);

  if (number>=100 && number<200 || errnum>25){
    if (strlen(errfname)==0) {
      va_start(argptr,number);
      pc_error(0,"\nCompilation aborted.\n\n",NULL,0,0,argptr);
	  pc_error_rus(0,"\nКомпиляция остановлена.\n\n",NULL,0,0,argptr);
      va_end(argptr);
    } /* if */
    if (outf!=NULL) {
      pc_closeasm(outf,TRUE);
      outf=NULL;
    } /* if */
    longjmp(errbuf,2);          /* fatal error, quit */
  } /* if */

  errline=-1;
  /* check whether we are seeing many errors on the same line */
  if ((errstart<0 && lastline!=fline) || lastline<errstart || lastline>fline || fcurrent!=lastfile)
    errorcount=0;
  lastline=fline;
  lastfile=fcurrent;
  if (number<200)
    errorcount++;
  if (errorcount>=3)
    error(107);         /* too many error/warning messages on one line */

  return 0;
}

SC_FUNC void errorset(int code,int line)
{
  switch (code) {
  case sRESET:
    errflag=FALSE;      /* start reporting errors */
    break;
  case sFORCESET:
    errflag=TRUE;       /* stop reporting errors */
    break;
  case sEXPRMARK:
    errstart=fline;     /* save start line number */
    break;
  case sEXPRRELEASE:
    errstart=-1;        /* forget start line number */
    errline=-1;
    break;
  case sSETPOS:
    errline=line;
    break;
  } /* switch */
}

/* pc_enablewarning()
 * Enables or disables a warning (errors cannot be disabled).
 * Initially all warnings are enabled. The compiler does this by setting bits
 * for the *disabled* warnings and relying on the array to be zero-initialized.
 *
 * Parameter enable can be:
 *  o  0 for disable
 *  o  1 for enable
 *  o  2 for toggle
 */
int pc_enablewarning(int number,int enable)
{
  int index;
  unsigned char mask;

  if (number<200)
    return FALSE;       /* errors and fatal errors cannot be disabled */
  number -= 200;
  if (number>=NUM_WARNINGS)
    return FALSE;

  index=number/8;
  mask=(unsigned char)(1 << (number%8));
  switch (enable) {
  case 0:
    warnstack.disable[index] |= mask;
    break;
  case 1:
    warnstack.disable[index] &= (unsigned char)~mask;
    break;
  case 2:
    warnstack.disable[index] ^= mask;
    break;
  } /* switch */

  return TRUE;
}

/* pc_pushwarnings()
 * Saves currently disabled warnings, used to implement #pragma warning push
 */
int pc_pushwarnings()
{
  void *p;
  p=calloc(sizeof(struct s_warnstack),1);
  if (p==NULL) {
    error(103); /* insufficient memory */
    return FALSE;
  }
  memmove(p,&warnstack,sizeof(struct s_warnstack));
  warnstack.next=(s_warnstack*)p;
  return TRUE;
}

/* pc_popwarnings()
 * This function is the reverse of pc_pushwarnings()
 */
int pc_popwarnings()
{
  void *p;
  if (warnstack.next==NULL)
    return FALSE; /* nothing to do */
  p=warnstack.next;
  memmove(&warnstack,p,sizeof(struct s_warnstack));
  free(p);
  return TRUE;
}

