#ifndef PARSER_H
#define PARSER_H

#include "model.h"
#include <string>

using std::string;

const int TOKEN_BUFF_SIZE = 65536;
const int PARSE_STACK_SIZE = 262144;

/** @class Tokenizor
 * Break down the input string stream into tokens
 */
class Tokenizor {
    private:
        FILE *stream;
        char *buff_ptr;
        bool escaping;
    public:
        Tokenizor();
        /** Set the stream to be read from (without setting this, the default
         * would be stdin) */
        void set_stream(FILE *stream);
        /** Extract the next token
         * @param ret the extracted token
         * @return false if nothing can be read further
         * */
        bool get_token(string &ret);
};

/** @class ASTGenerator
 * Read the tokens and build up an Abstract Syntax Tree (which is in effect a
 * Pair)
 */
class ASTGenerator {
    private:
        /** Convert the string to an internal object */
        static EvalObj* to_obj(const string &);
    public:
        ASTGenerator();
        /** Read tokens from Tokenizor tk, then return a AST
         * @param tk pointer to a Tokenizor
         * @return Abstract Syntax Tree
         */
        Pair *absorb(Tokenizor *tk);
};

#endif
