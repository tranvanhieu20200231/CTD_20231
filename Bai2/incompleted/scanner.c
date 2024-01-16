/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank()
{
  while (readChar() != EOF && charCodes[currentChar] == CHAR_SPACE)
  {
    // Keep reading until a non-space character is encountered.
  }
}

// Skip the comment section, starting from the current character.
void skipComment()
{
  char prevChar = currentChar;

  // Continue reading until the end of the file or the comment closing symbols are found.
  while (readChar() != EOF &&
         charCodes[prevChar] != CHAR_TIMES &&
         charCodes[currentChar] != CHAR_RPAR)
  {
    prevChar = currentChar;
  }

  // Move to the next character after the comment closing symbols.
  readChar();
}

// Read an identifier or keyword.
Token *readIdentKeyword(void)
{
  Token *token = makeToken(TK_IDENT, lineNo, colNo);

  int i = 0;
  token->string[i] = currentChar;

  // Continue reading while the current character is a letter or digit.
  while (readChar() != EOF &&
         (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT))
  {
    i++;
    token->string[i] = currentChar;
  }

  // Null-terminate the string.
  token->string[++i] = '\0';

  // Check if the string is a keyword and update the token type accordingly.
  TokenType keywordType = checkKeyword(token->string);
  if (keywordType != TK_NONE)
  {
    token->tokenType = keywordType;
  }

  return token;
}

// Read a number.
Token *readNumber(void)
{
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);

  int i = 0;
  token->string[i] = currentChar;

  // Continue reading while the current character is a digit.
  while (readChar() != EOF && charCodes[currentChar] == CHAR_DIGIT)
  {
    i++;
    token->string[i] = currentChar;
  }

  // Null-terminate the string.
  token->string[++i] = '\0';

  return token;
}

// Read a character constant.
Token *readConstChar(void)
{
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  readChar(); // Consume the opening single quote.

  int i = 0;
  token->string[i] = currentChar;

  // Continue reading until the closing single quote is found.
  while (readChar() != EOF && charCodes[currentChar] != CHAR_SINGLEQUOTE)
  {
    i++;
    token->string[i] = currentChar;
  }

  // Null-terminate the string.
  token->string[++i] = '\0';

  // Move to the next character after the closing single quote.
  if (currentChar != EOF)
  {
    readChar();
  }

  return token;
}

Token *getToken(void)
{
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();

  case CHAR_LETTER:
    return readIdentKeyword();

  case CHAR_DIGIT:
    return readNumber();

  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SINGLEQUOTE:
    return readConstChar();

  case CHAR_LPAR:
    token = makeToken(SB_LPAR, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_TIMES)
    {
      free(token);
      readChar();
      skipComment();
      return getToken();
    }
    else
    {
      return token;
    }

  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;

  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    return token;

  case CHAR_COLON:
    token = makeToken(SB_COLON, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;

  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;

  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;

  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;

  case CHAR_SLASH:
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;

  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;

  case CHAR_EXCLAIMATION:
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token = makeToken(SB_NEQ, lineNo, colNo);
      readChar();
      return token;
    }
    else
    {
      token = makeToken(TK_NONE, lineNo, colNo);
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
      readChar();
      return token;
    }

  case CHAR_LT:
    token = makeToken(SB_LT, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_LE;
      readChar();
    }
    return token;

  case CHAR_GT:
    token = makeToken(SB_GT, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_GE;
      readChar();
    }
    return token;

  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;

  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

/******************************************************************/

void printToken(Token *token)
{

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    printf("TK_NONE\n");
    break;
  case TK_IDENT:
    printf("TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    printf("TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    printf("TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    printf("TK_EOF\n");
    break;

  case KW_PROGRAM:
    printf("KW_PROGRAM\n");
    break;
  case KW_CONST:
    printf("KW_CONST\n");
    break;
  case KW_TYPE:
    printf("KW_TYPE\n");
    break;
  case KW_VAR:
    printf("KW_VAR\n");
    break;
  case KW_INTEGER:
    printf("KW_INTEGER\n");
    break;
  case KW_CHAR:
    printf("KW_CHAR\n");
    break;
  case KW_ARRAY:
    printf("KW_ARRAY\n");
    break;
  case KW_OF:
    printf("KW_OF\n");
    break;
  case KW_FUNCTION:
    printf("KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    printf("KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    printf("KW_BEGIN\n");
    break;
  case KW_END:
    printf("KW_END\n");
    break;
  case KW_CALL:
    printf("KW_CALL\n");
    break;
  case KW_IF:
    printf("KW_IF\n");
    break;
  case KW_THEN:
    printf("KW_THEN\n");
    break;
  case KW_ELSE:
    printf("KW_ELSE\n");
    break;
  case KW_WHILE:
    printf("KW_WHILE\n");
    break;
  case KW_DO:
    printf("KW_DO\n");
    break;
  case KW_FOR:
    printf("KW_FOR\n");
    break;
  case KW_TO:
    printf("KW_TO\n");
    break;

  case SB_SEMICOLON:
    printf("SB_SEMICOLON\n");
    break;
  case SB_COLON:
    printf("SB_COLON\n");
    break;
  case SB_PERIOD:
    printf("SB_PERIOD\n");
    break;
  case SB_COMMA:
    printf("SB_COMMA\n");
    break;
  case SB_ASSIGN:
    printf("SB_ASSIGN\n");
    break;
  case SB_EQ:
    printf("SB_EQ\n");
    break;
  case SB_NEQ:
    printf("SB_NEQ\n");
    break;
  case SB_LT:
    printf("SB_LT\n");
    break;
  case SB_LE:
    printf("SB_LE\n");
    break;
  case SB_GT:
    printf("SB_GT\n");
    break;
  case SB_GE:
    printf("SB_GE\n");
    break;
  case SB_PLUS:
    printf("SB_PLUS\n");
    break;
  case SB_MINUS:
    printf("SB_MINUS\n");
    break;
  case SB_TIMES:
    printf("SB_TIMES\n");
    break;
  case SB_SLASH:
    printf("SB_SLASH\n");
    break;
  case SB_LPAR:
    printf("SB_LPAR\n");
    break;
  case SB_RPAR:
    printf("SB_RPAR\n");
    break;
  case SB_LSEL:
    printf("SB_LSEL\n");
    break;
  case SB_RSEL:
    printf("SB_RSEL\n");
    break;
  }
}

int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR)
  {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}
