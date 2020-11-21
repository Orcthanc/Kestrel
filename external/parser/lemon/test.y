%include{
#include <stdio.h>
#include <assert.h>
}

%syntax_error{ fprintf( stderr, "Syntax error\n" ); }
%left PLUS.
%type expr { float }
%token_type { float }
program ::= expr(A).{ printf( "Res: %d\n", A ); }
expr(A) ::= expr(B) PLUS expr(C).{ A = B + C; }
expr(A) ::= LPARA expr(B) RPARA.{ A = B; }
expr(A) ::= FLOATL(B).{ A = B; }
