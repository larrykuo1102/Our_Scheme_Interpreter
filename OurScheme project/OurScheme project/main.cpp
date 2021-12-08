

# include <iostream>
# include <iomanip>
# include <vector>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <sstream>
# include <map>
# include <stack>

using namespace std ;
static int utest = 0 ;

bool gFileeof = false ;
bool IsNotwhitespace( char & ch ) ;
void SkipComment() ;
string GetaToken( char ch ) ;
string ReadString( char ch ) ;
bool IsNumber( char ch ) ;
string ReadUntilwhitespace( char ch ) ;
bool HavedDot( string token ) ;
bool Isfloat( string token ) ;

string Changetofloat( string token ) ;
string gNewtoken ;
string Peekatoken() ;
string Readatoken() ;

int gColumn = 1 ;
int gRow = 1;
bool gGrammarFault = false ;
int gGrammarFaultKind = 0 ;
string gGrammarFaultString ;

struct Tree {
  Tree * left ;
  Tree * right ;
  string token ;
} ; //

struct AtomorSymbol {
  string type ;
  Tree * root ;
} ;

struct User_func {
  Tree * func ;
  Tree * root ;
  vector<string> argument ;
} ;

map<string, AtomorSymbol> gSymbolmap ;
map<string, User_func> gMyfunc ;
map<string,Tree*> ginternalfunc ;

vector< map<string,Tree*> > gLocalvariable ;

// stack< map<string,Tree*> > gLocalvariable ;

// stack<User_func> gLocalLamb ;
vector<User_func> gLocalLamb ;

bool Else_S_exp( vector<string> & alltoken, string & token, Tree * root ) ;
Tree * Eval_All( AtomorSymbol & newAtom, Tree * result, int sum ) ;

void SkipComment() {
  char ch ;
  int check = 0 ;
  ch = cin.peek() ;
  check = cin.peek() ;
  while ( ch != '\n' && check != -1 ) {
    scanf( "%c", &ch ) ;
    ch = cin.peek() ;
    check = cin.peek() ;
  } // while
  
  if ( ch == '\n' ) ch = cin.get() ;
} // SkipComment()

bool IsNotwhitespace( char & ch ) { // check
  int check ;
  ch = cin.peek() ;
  check = cin.peek() ;
  if ( check == -1 ) {
    gFileeof = true ;
    throw 2 ;
  } // if

  while ( ch == ' ' || ch == '\n' || ch == ';' ) { // 讀入直到不是空白
    ch = cin.get() ;
    gColumn ++ ; // 加一字元
    // cout << "white!" ;
    if ( ch == ';' ) {
      SkipComment();
      gRow ++ ;
      gColumn = 1 ;
    } // if
    
    if ( ch == '\n' ) {
      gRow ++ ;
      gColumn = 1 ;
    } // if
    
    ch = cin.peek() ;
    check = cin.peek() ;
    while ( ch == ';' && check != -1 ) {
      ch = cin.get() ;
      SkipComment();
      gRow ++ ;
      gColumn = 1 ;
      ch = cin.peek() ;
      check = cin.peek() ;
    } // while
    
    if ( check == -1 ) { // eof
      gFileeof = true ;
      throw 2 ;
    } // if
  } // while
  
  ch = cin.get() ;
  gColumn ++ ; // 加一字元
  return true ;
} // IsNotwhitespace()

bool IsNumber( char ch ) { // 判斷一個字是不是數字
  if ( ( ch >= 48 && ch <= 57 ) ) return true ;
  else if ( ch == '+' || ch == '-' ) return true ;
  else return false ;
} // IsNumber()

bool IsInteger( string token ) {
  int i = 0, sum = 0 ;
  if ( token[0] == '+' || token[0] == '-' ) {
    sum ++ ;
    i = 1 ;
    if ( token.size() == 1 ) return false ;
  } // if
  else
    i = 0 ;
  for ( i = i ; i < token.size() ; i ++ ) {
    if ( token[i] == '+' || token[i] == '-' )
      sum ++ ;
    if ( !IsNumber( token[i] ) )
      return false ;
  } // for
  
  if ( sum >= 2 )
    return false ;
  if ( token.size() == 0 )
    return false ;
  return true ;
} // IsInteger()

string GetaToken( char ch ) {
  string token ;
  int test = 0 ;
  char ch1 ;
  if ( ch == '(' ) {
    ch1 = cin.peek() ;
    test = cin.peek() ;
    while ( ( ch1 == ' ' || ch1 == '\n' ) && test != -1 ) {
      ch1 = cin.get() ;
      if ( ch1 == '\n' ) {
        gRow ++ ;
        gColumn = 1 ;
      } // if
      
      gColumn ++ ;
      ch1 = cin.peek() ;
      test = cin.peek() ;
    } // while
    
    if ( test == -1 ) { // eof
      gFileeof = true ;
      throw 2 ;
    } // if
      
    if ( ch1 == ')' ) {
      ch1 = cin.get() ;
      gColumn ++ ;
      token = "nil" ;
    } // if
    else
      token = token + ch ;
  } // if
  else if ( ch == ')' ) {
    token = token + ch ;
  } // else if
  else if ( ch == '\"' ) {
    token = ReadString( ch ) ;
  } // else if
  else if ( ch == '\'' ) {
    token = token + ch ;
  } // else if
  else {
    token = ReadUntilwhitespace( ch ) ;
    if ( HavedDot( token ) ) { // 有小數點
      if ( token == "." ) { // 只有一個.  是dot
        token = token ;
      } // if
      else if ( Isfloat( token ) ) {
        token = Changetofloat( token ) ;
      } // else if
      else { // a.a b.b 1.2bbb 有小數點 但是不是float
        token = token ;
      } // else
    } // if
    else { // 沒小數點
      if ( IsInteger( token ) ) { // Integer
        if ( token[0] == '+' )
          token.erase( 0, 1 ) ;
        token = token ;
      } // if
      else if ( token == "#t" || token == "t" ) { // t
        token = "#t" ;
      } // else if
      else if ( token == "nil" || token == "#f" ) { // nil null
        token = "nil" ;
      } // else if
      else {
        token = token ;
      } // else
    } // else
  } // else

  return token ;
} // GetaToken()

string Changetofloat( string token ) {
  if ( token[0] == '+' || token[0] == '-' ) { // +-開頭 +0.3 -0.3
    if ( token[1] == '.' ) { // +.3 -.3
      if ( token[0] == '+' ) { // 如果是+ 刪掉第一個 然後加回+0
        token.erase( 0, 1 ) ;
        token = "+0" + token ;
      } // if
      else {
        token.erase( 0, 1 ) ;
        token = "-0" + token ;
      } // else
    } // if
    else { // +3.3 -3.3
      token = token ; // 沒做事
    } // else
  } // if
  else {
    if ( token[0] == '.' ) { // .開頭 .3 .4
      token = '0' + token ;
    } // if
  } // else
  
  for ( int i = 0 ; i < token.size() ; i ++ ) {
    if ( token[i] == '.' ) {
      if ( i == token.size() -1 ) {
        token = token + '0' ;
      } // if
    } // if
  } // for
  
  return token ;
} // Changetofloat()

bool HavedDot( string token ) {
  for ( int i = 0 ; i < token.size() ; i ++ ) {
    if ( token[i] == '.' )
      return true ;
  } // for
  
  return false ;
} // HavedDot()

bool Isfloat( string token ) {
  int i = 0, dot = 0 ;
  bool check = false ;
  if ( token[0] == '-' || token[0] == '+' ) {
    i = 1 ;
  } // if
  else i = 0 ;
  for ( i = i ; i < token.size() ; i ++ ) {
    if ( token[i] == '.' ) dot ++ ;
    if ( token[i] >= 48 && token[i] <= 57 )
      check = true ;
    if ( ( token[i] >= 48 && token[i] <= 57 ) || token[i] == '.' ) {
      token = token ;
    } // if
    else {
      return false ;
    } // else
  } // for
  
  if ( check == false || !HavedDot( token ) || dot > 1 )
    return false ;
  return true ;
} // Isfloat()

bool IsString( string token ) {
  size_t size = token.size() ;
  if ( token[0] == '\"' && token[size-1] == '\"' ) {
    return true ;
  } // if
  else {
    return false ;
  } // else
  
  return true ;
} // IsString()

string ReadUntilwhitespace( char ch ) {
  string token ;
  char temp ;
  token = token + ch ;
  int check ;
  temp = cin.peek() ;
  check = cin.peek() ;
  if ( check == -1 ) {
    gFileeof = true ;
    throw 2 ;
  } // if
  
  while ( temp != ' ' && temp != '(' && temp != ')' && temp != '\n' && temp != ';' &&
          temp != '\"' && temp != '\'' ) {
    temp = cin.get() ;
    gColumn ++ ; // 加一字元
    token = token + temp ;
    temp = cin.peek() ;
    check = cin.peek() ;
    if ( check == -1 ) {
      gFileeof = true ;
      throw 2 ;
    } // if
  } // while
  
  return token ;
} // ReadUntilwhitespace()

string ReadString( char ch ) { // ""開頭結尾
  string token ;
  char temp, temp2 ;
  int check ;
  token = token + ch ;
  temp = cin.peek() ;
  check = cin.peek() ;
  if ( check == -1 ) {  // 如果eof
    gFileeof = true ;
    throw 2 ;
  } // if
  
  while ( temp != '"' && temp != '\n' && gFileeof == false ) { // 只要不是空白就繼續讀
    temp = cin.get() ;
    gColumn ++ ; // 加一字元
    if ( temp == '\\' ) { // 如果是跳脫字元 '\'
      temp2 = cin.peek() ; // 跳脫字元的下一個字
      if ( temp2 == 'n' ) {
        token = token + '\n' ;
        temp2 = cin.get() ;
        gColumn ++ ; // 加一字元
      } // if
      else if ( temp2 == 't' ) {
        token = token + temp + temp2  ;
        temp2 = cin.get() ;
        gColumn ++ ; // 加一字元
      } // else if
      else if ( temp2 == '\\' ) {
        token = token + '\\' ;
        temp2 = cin.get() ;
        gColumn ++ ; // 加一字元
      } // else if
      else if ( temp2 == '"' ) {
        token = token + '"' ;
        temp2 = cin.get() ;
        gColumn ++ ; // 加一字元
      } // else if
      else {
        token = token + temp ;
      } // else
    } // if
    else {
      token = token + temp ;
    } // else
    
    check = cin.peek() ;
    temp = cin.peek() ;
    if ( check == -1 ) { // 如果eof
      gFileeof = true ;
      throw 2 ;
    } // if
  } // while
  
  if ( temp == '\n' ) {
    temp = cin.get() ;
    gGrammarFault = true ;
    gGrammarFaultKind = 1 ;
    throw 1 ;
  } // if
  else if ( temp == '\"' ) {
    cin.get() ;
    gColumn ++ ; // 加一字元
    token = token + temp ;
    temp = cin.peek() ;
    check = cin.peek() ;
  } // else if
  
  return token ;
} // ReadString()

string Peekatoken() {
  string token ;
  char firstch ;
  
  if ( gNewtoken.empty() ) {
    if ( IsNotwhitespace( firstch ) ) {
      token = GetaToken( firstch ) ;
      gNewtoken = token ;
    } // if
  } // if
  else
    token = gNewtoken ;
  return token ;
} // Peekatoken()

string Readatoken() {
  string token ;
  token = gNewtoken ;
  gNewtoken.clear() ;
  return token ;
} // Readatoken()

void Printtree( Tree * root ) {
  Tree * temp = root ;
  if ( temp -> left != NULL ) Printtree( temp -> left ) ;
  if ( ! temp -> token.empty() ) cout << "tree " << temp -> token ;
  if ( temp -> right != NULL ) Printtree( temp -> right ) ;
} // Printtree()

bool Atom( vector<string> alltoken, string & token ) {
  string temp ;
  // cout << temp << "   1";
  if ( token.size() == 0 )
    return false ;
  if ( token == "." ) {
    gColumn -- ;
    gGrammarFault = true ;
    gGrammarFaultKind = 2 ;
    gGrammarFaultString = token ;
    throw 3 ;
  } // if
  else if ( IsInteger( token ) ) {
    return true ;
  } // if
  else if ( IsString( token ) ) {
    return true ;
  } // else if
  else if ( Isfloat( token ) ) {
    return true ;
  } // else if
  else if ( token == "#t" ) {
    return true ;
  } // else if
  else if ( token == "nil" ) {
    return true ;
  } // else if
  else if ( token == "(" ) {
    token = Readatoken() ;
    temp = Peekatoken() ;
    if ( temp == ")" ) {
      token = "nil" ; //
      alltoken.push_back( token ) ;
      return true ;
    } // if
    else {
      return false ;
    } // else
  } // else if
  else if ( token == ")" ) {
    gColumn -- ;
    gGrammarFault = true ;
    gGrammarFaultKind = 2 ;
    gGrammarFaultString = token ;
    throw 3 ;
  } // else if
  else if ( token == "'" ) { // SYMBOL
    return false ;
  } // else if
  else {
    return true ;
  } // else
  
  return false ;
} // Atom()

bool Is_Symbol( string token ) {
  if ( token == "." || token == "(" || token == ")" )
    return false ;
  else if ( token == "nil" || token == "#t"  || token == "'" )
    return false ;
  else if ( IsInteger( token ) || IsString( token ) || Isfloat( token )  )
    return false ;
  else
    return true ;
  
  return true ;
} // Is_Symbol()

bool S_exp( vector<string> & alltoken, string & token, Tree * root, bool else_S_exp ) {
  Tree * temp ;
  Tree * lefttree = new Tree ;
  lefttree -> left = NULL ;
  lefttree -> right = NULL ;
  Tree * righttree = new Tree ;
  righttree -> left = NULL ;
  righttree -> right = NULL ;
  bool check = true ;
  
  token = Peekatoken() ;
  if ( token != "nil" )
    alltoken.push_back( token ) ;
  if ( Atom( alltoken, token ) ) {
    Readatoken() ;
    if ( else_S_exp == false ) {
      lefttree -> token = token ;
      root -> left = lefttree ; // 往左建樹 給值
      root -> right = righttree ;
    } // if
    else {
      root -> token = token ;
    } // else
    
    return true ;
  } // if
  else if ( token == "(" ) {
    temp = root ;
    token = Peekatoken() ;
    if ( token == "(" ) {
      temp -> left = new Tree ;
      temp -> left -> left = NULL ;
      temp -> left -> right = NULL ;
      if ( ! S_exp( alltoken, token, temp -> left, false ) ) {
        throw 3 ;
      } // if
    } // if
    else {
      if ( ! S_exp( alltoken, token, temp, false ) ) {
        throw 3 ;
      } // if
    } // else
  
    root -> right = righttree ;
    temp = root -> right ; // 向右
    token = Peekatoken() ;
    while ( token != ")" && token != "." && check == true ) {
      if ( token == "(" ) {
        temp -> left = new Tree ;
        temp -> left -> right = NULL ;
        temp -> left -> left = NULL ;
        if ( ! S_exp( alltoken, token, temp -> left, false ) ) {
          check = false ;
          return false ;
        } // if
      } // if
      else {
        if ( ! S_exp( alltoken, token, temp, false ) ) {
          check = false ;
          return false ;
        } // if
      } // else
      
      token = Peekatoken() ;
      temp -> right = new Tree ;
      temp = temp -> right ;
      temp -> right = NULL ;
      temp -> left = NULL ;
    } // while
    
    if ( token == "." ) {
      token = Readatoken() ;
      token = Peekatoken() ;
      if ( token != ")" && S_exp( alltoken, token, temp, true ) ) { // else S_exp
        token = Peekatoken() ;
        if ( token == ")" ) {
          token = Readatoken() ;
          return true ;
        } // if
        else {
          token = Readatoken() ; // 之後再確認
          gColumn -- ;
          gGrammarFault = true ;
          gGrammarFaultKind = 3 ;
          gGrammarFaultString = token ;
          throw 3 ;
        } // else
      } // if
      else {
        gColumn -- ;
        token = Readatoken() ;
        gGrammarFault = true ;
        gGrammarFaultKind = 2 ;
        gGrammarFaultString = token ;
        throw 3 ;
      } // else
    } // if
    else if ( token == ")" ) {
      temp -> token = "nil" ;
      token = Readatoken() ; // 沒有接nil 如果有錯注意
      return true ;
      // 往右建樹
    } // else if
    else {
      gColumn -- ;
      gGrammarFault = true ;
      gGrammarFaultKind = 3 ;
      gGrammarFaultString = ")" ;
      throw 3 ;
    } // else
  } // else if
  else if ( token == "\'" ) {
    temp = root ;
    token = Readatoken() ;
    temp -> left = new Tree ;
    temp -> right = new Tree ;
    temp -> right -> left = NULL ;
    temp -> right -> right = NULL ;
    temp -> right -> token = "nil" ;
    
    temp -> left -> left = new Tree ;
    temp -> left -> right = new Tree ;
    
    temp -> left -> left -> token = "quote" ;
    temp -> left -> left -> left = NULL ;
    temp -> left -> left -> right = NULL ;
    
    temp -> left -> right -> left = new Tree ;
    temp -> left -> right -> left -> left = NULL ;
    temp -> left -> right -> left -> right = NULL ;
    
    temp -> left -> right -> right = new Tree ;
    temp -> left -> right -> right -> left = NULL ;
    temp -> left -> right -> right -> right = NULL ;
    temp -> left -> right -> right -> token = "nil" ;
    
    token = Peekatoken() ;
    if ( token == "(" ) {
      if ( S_exp( alltoken, token, temp -> left -> right -> left, false ) )
        return true ;
    } // if
    else if ( token == "'" ) {
      if ( ! S_exp( alltoken, token, temp -> left -> right, false ) )
        return false ;
    } // else if
    else {
      temp -> left -> right -> right -> token.empty() ;
      if ( S_exp( alltoken, token, temp -> left -> right -> left, true ) )
        return true ;
    } // else
    
    return true ;
  } // else if
  else {
    return false ;
  } // else
  
  return true ;
} // S_exp()

void Print_S_exp( Tree * root, int num, bool & firstLP, bool islefttree ) {
  Tree * temp = root ;
  Tree * cur ;
  
  if ( temp -> left != NULL ) {
    cur = temp -> left ;
    if ( cur -> token.empty() ) {
      if ( firstLP == false )
        for ( int i = 0 ; i < num + 1 ; i ++ )
          cout << "  " ;
      cout << "( " ;
      firstLP = true ;
    } // if
    
    islefttree = true ;
    Print_S_exp( temp -> left, num + 1, firstLP, islefttree ) ;
  } // if
  
  if ( ! temp -> token.empty() ) {
    if ( islefttree == true ) { // 如果是左子樹
      if ( firstLP == false ) {
        for ( int i = 0 ; i < num ; i ++ )
          cout << "  " ;
      } // if
      else
        firstLP = false ;
      if ( Isfloat( temp -> token ) ) {
        cout << fixed << setprecision( 3 ) << atof( temp -> token.c_str() ) << endl ;
      } // if
      else
        cout << temp -> token << endl ;
    } // if
    else { // 如果是右子樹
      if ( temp -> token != "nil" ) {
        if ( firstLP == false ) {
          for ( int i = 0 ; i < num ; i ++ )
            cout << "  " ;
        } // if
        else
          firstLP = false ;
        if ( Isfloat( temp -> token ) ) {
          cout << fixed << setprecision( 3 ) << atof( temp -> token.c_str() ) << endl ;
        } // if
        else
          cout << temp -> token << endl ;
      } // if
    } // else
    
  } // if
  
  if ( temp -> right != NULL ) {
    if ( !temp -> right -> token.empty() ) {
      num ++ ;
      if ( temp -> right -> token != "nil" ) {
        for ( int j = 0 ; j < num ; j ++ )
          cout << "  " ;
        cout << "." << endl ;
      } // if
    } // if
    
    islefttree = false ;
    Print_S_exp( temp -> right, num, firstLP, islefttree ) ;
    if ( !temp -> right -> token.empty() ) {
      num -- ;
      for ( int k = 0 ; k < num ; k ++ )
        cout << "  " ;
      cout << ")\n" ;
    } // if
  } // if
} // Print_S_exp()

string ErrorMessage() {
  // cout << gRow << " " << gColumn << endl ;
  stringstream ss, aa ;
  string token, row, column ;
  ss << gRow ;
  row = ss.str() ;
  aa << gColumn ;
  
  column = aa.str() ;
  if ( gGrammarFault ) {
    if ( gGrammarFaultKind == 1 ) {
      token = "ERROR (no closing quote) : END-OF-LINE encountered at Line " ;
      token = token + row + " Column " + column ;
    } // if
    else if ( gGrammarFaultKind == 2 ) {
      token = "ERROR (unexpected token) : atom or '(' expected when token at Line " ;
      token = token + row + " Column " + column + " is >>" + gGrammarFaultString + "<<" ;
    } // else if
    else if ( gGrammarFaultKind == 3 ) {
      token = "ERROR (unexpected token) : ')' expected when token at Line " ;
      token = token + row + " Column " + column + " is >>" + gGrammarFaultString + "<<" ;
    } // else if
    else if ( gGrammarFaultKind == 4 ) {
      token = "ERROR (no more input) : END-OF-FILE encountered" ;
    } // else if
    else if ( gGrammarFaultKind == 5 ) {
      token = "ERROR (unbound symbol) : " + gGrammarFaultString ;
    } // else if
    else if ( gGrammarFaultKind == 6 ) {
      token = "ERROR (incorrect number of arguments) : " + gGrammarFaultString ;
    } // else if
  } // if
  
  return token ;
} // ErrorMessage()

void ReadSExp( Tree * root, AtomorSymbol & newAtom ) {
  root = new Tree ;
  vector<string> temp, alltoken ;
  string token ;

  token = Peekatoken() ;
  alltoken.clear() ;
  if ( token == "(" ) {
    S_exp( alltoken, token, root, false ) ;
    alltoken.push_back( ")" ) ;
    if ( alltoken.size() == 3 )
      if ( alltoken[0] == "(" && alltoken[1] == "exit" && alltoken[2] == ")" )
        throw 4 ;
    newAtom.root = root ;
    newAtom.type = "S_exp" ;
  } // if
  else if ( token == "'" ) {
    root -> left = new Tree ;
    root -> left -> token = "quote" ;
    root -> left -> left = NULL ;
    root -> left -> right = NULL ;
    root -> right = new Tree ;
    Readatoken() ;
    string temp = Peekatoken() ;
    S_exp( alltoken, token, root -> right, false ) ;
    newAtom.root = root ;
    newAtom.type = "Quote" ;
    if ( temp == "'" )
      newAtom.type = "quote" ;
  } // else if
  else {
    S_exp( alltoken, token, root, false ) ;
    newAtom.root = root ;
    newAtom.type = "Atom" ;
  } // else
} // ReadSExp()

Tree * ReturnATree( string func ) {
  Tree * temp = new Tree ;
  temp -> left = new Tree ;
  temp -> left -> token = func ;
  temp -> left -> left = NULL ;
  temp -> left -> right = NULL ;
  temp -> right = new Tree ;
  temp -> right -> left = NULL ;
  temp -> right -> right = NULL ;
  // temp -> right -> token = "nil" ;
  return temp ;
} // ReturnATree()

void Internalfunc_define( map<string,Tree *> & internalfunc ) {
  internalfunc["define"] = ReturnATree( "#<procedure define>" ) ;
  internalfunc["cond"] = ReturnATree( "#<procedure cond>" ) ;
  internalfunc["cons"] = ReturnATree( "#<procedure cons>" ) ;
  internalfunc["list"] = ReturnATree( "#<procedure list>" ) ;
  internalfunc["not"] = ReturnATree( "#<procedure not>" ) ;
  internalfunc["and"] = ReturnATree( "#<procedure and>" ) ;
  internalfunc["or"] = ReturnATree( "#<procedure or>" ) ;
  internalfunc["if"] = ReturnATree( "#<procedure if>" ) ;
  internalfunc["clean-environment"] = ReturnATree( "#<procedure clean-environment>" ) ;
  internalfunc["car"] = ReturnATree( "#<procedure car>" ) ;
  internalfunc["cdr"] = ReturnATree( "#<procedure cdr>" ) ;
  internalfunc["atom?"] = ReturnATree( "#<procedure atom?>" ) ;
  internalfunc["pair?"] = ReturnATree( "#<procedure pair?>" ) ;
  internalfunc["list?"] = ReturnATree( "#<procedure list?>" ) ;
  internalfunc["null?"] = ReturnATree( "#<procedure null?>" ) ;
  internalfunc["integer?"] = ReturnATree( "#<procedure integer?>" ) ;
  internalfunc["real?"] = ReturnATree( "#<procedure real?>" ) ;
  internalfunc["number?"] = ReturnATree( "#<procedure number?>" ) ;
  internalfunc["string?"] = ReturnATree( "#<procedure string?>" ) ;
  internalfunc["boolean?"] = ReturnATree( "#<procedure boolean?>" ) ;
  internalfunc["symbol?"] = ReturnATree( "#<procedure symbol?>" ) ;
  internalfunc["+"] = ReturnATree( "#<procedure +>" ) ;
  internalfunc["-"] = ReturnATree( "#<procedure ->" ) ;
  internalfunc["*"] = ReturnATree( "#<procedure *>" ) ;
  internalfunc["/"] = ReturnATree( "#<procedure />" ) ;
  internalfunc[">"] = ReturnATree( "#<procedure >>" ) ;
  internalfunc[">="] = ReturnATree( "#<procedure >=>" ) ;
  internalfunc["<"] = ReturnATree( "#<procedure <>" ) ;
  internalfunc["<="] = ReturnATree( "#<procedure <=>" ) ;
  internalfunc["="] = ReturnATree( "#<procedure =>" ) ;
  internalfunc["string-append"] = ReturnATree( "#<procedure string-append>" ) ;
  internalfunc["string>?"] = ReturnATree( "#<procedure string>?>" ) ;
  internalfunc["string<?"] = ReturnATree( "#<procedure string<?>" ) ;
  internalfunc["string=?"] = ReturnATree( "#<procedure string=?>" ) ;
  internalfunc["eqv?"] = ReturnATree( "#<procedure eqv?>" ) ;
  internalfunc["equal?"] = ReturnATree( "#<procedure equal?>" ) ;
  internalfunc["begin"] = ReturnATree( "#<procedure begin>" ) ;
  internalfunc["quote"] = ReturnATree( "#<procedure quote>" ) ;
  internalfunc["exit"] = ReturnATree( "#<procedure exit>" ) ;
  internalfunc["let"] = ReturnATree( "#<procedure let>" ) ;
  internalfunc["lambda"] = ReturnATree( "#<procedure lambda>" ) ;
} // Internalfunc_define()

void Get_Arg( vector<Tree *> & all_arg, Tree * root ) {
  Tree * cur = root ;
  while ( cur -> right != NULL ) {
    if ( cur -> left != NULL )
      all_arg.push_back( cur -> left ) ;
    cur = cur -> right ;
  } // while
} // Get_Arg()

bool Pure_list( Tree * root ) {
  Tree * cur = root ;
  while ( cur -> right != NULL ) {
    if ( cur -> left != NULL ) {
      if ( ! cur -> right -> token.empty() && cur -> right -> token != "nil" ) {
        return false ;
      } // if
    } // if
    
    cur = cur -> right ;
  } // while
  
  return true ;
} // Pure_list()

void Return_Value( string token, bool & intorfloat, int & a, float & b ) {
  if ( HavedDot( token ) ) {
    if ( Isfloat( token ) ) {
      b = atof( token.c_str() ) ;
      intorfloat = false ;
    } // if
    else {
      cout << "ERROR (" << gGrammarFaultString << " with incorrect argument type) : " << token << endl ;
      throw 6 ;
    } // else
  } // if
  else {
    if ( IsInteger( token ) ) {
      a = atoi( token.c_str() ) ;
      intorfloat = true ;
    } // if
    else {
      cout << "ERROR (" << gGrammarFaultString << " with incorrect argument type) : " << token << endl ;
      throw 6 ;
    } // else
  } // else
} // Return_Value()

Tree * Initial_Result( Tree * result ) {
  result -> left = new Tree ;
  result -> left -> left = NULL ;
  result -> left -> right = NULL ;
  result -> right = NULL ;
  return result ;
} // Initial_Result()

void Initial_temp_struct( string type, AtomorSymbol & temp_struct ) {
  temp_struct.type = type ;
  temp_struct.root = new Tree ;
  temp_struct.root -> right = new Tree ;
  temp_struct.root -> right -> left = NULL ;
  temp_struct.root -> right -> right = NULL ;
} // Initial_temp_struct()

Tree * Function_cons( Tree * result, Tree * root, int sum ) {
  vector<Tree *> all_arg ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  // check argument 數量
  Get_Arg( all_arg, root -> right ) ;
  // check argument數量結束
  if ( all_arg.size() != 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ; // 數量錯了
  } // if
 
  if ( all_arg[0] -> token.empty() ) {
    temp_struct.type = "S_exp" ;
    temp_struct.root = all_arg[0] ;
    result -> left = Eval_All( temp_struct, result -> left, sum + 1 ) ;
    if ( result -> left -> left != NULL && result -> left -> left -> token == "nil" )
      result -> left = result -> left -> left ;
  } // if
  else {
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = all_arg[0] ;
    result -> left = Eval_All( temp_struct, result -> left, sum + 1 ) ;
    if ( all_arg[0] -> token == "nil" )
      result -> left = result -> left -> left ;
  } // else
  
  if ( all_arg[1] -> token.empty() ) {
    temp_struct.type = "S_exp" ;
    temp_struct.root = all_arg[1] ;
    result -> right = Eval_All( temp_struct, result -> right, sum + 1 ) ;
    if ( result -> right -> left != NULL && result -> right -> left -> token == "nil" )
      result -> right = result -> right -> left ;
  } // if
  else {
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = all_arg[1] ;
    result -> right = Eval_All( temp_struct, result -> right, sum + 1 ) ;
    if ( all_arg[1] -> token == "nil" )
      result -> right = result -> right -> left ;
      
  } // else

  return result ;
} // Function_cons()

Tree * Function_define( Tree * result, Tree * root, int sum ) {
  bool firstLP = true ;
  Tree * arg1 = new Tree ;
  vector<Tree *> all_arg, para ;
  string name = root -> left -> token ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  Get_Arg( all_arg,  root -> right ) ;
  
  
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( i == 0 ) {
      if ( all_arg.size() < 2 ) {
        cout << "ERROR (define format) : " ;
        throw 6 ;
      } // if
      
      if ( all_arg[i] -> token.empty() ) { // 第一個參數是()
        if ( ! Pure_list( all_arg[i] ) ) {
          cout << "ERROR (define format) : " ;
          throw 6 ;
        } // if
        
        if ( ginternalfunc.find( all_arg[i] -> left -> token ) != ginternalfunc.end() ) {
          cout << "ERROR (define format) : ( " ;
          Print_S_exp( root, 0, firstLP, false ) ;
          throw 6 ;
        } // if
      } // if
      else { // 第一個參數是 ａ
        if ( all_arg.size() != 2 ) {
          cout << "ERROR (define format) : " ;
          throw 6 ;
        } // if
        
        if ( ! Is_Symbol( all_arg[i] -> token ) ) {
          cout << "ERROR (DEFINE format) : ( " ;
          Print_S_exp( root, 0, firstLP, false ) ;
          throw 6 ;
        } // if
        
        if ( ginternalfunc.find( all_arg[i] -> token ) != ginternalfunc.end() ) {
          cout << "ERROR (DEFINE format) : ( " ;
          Print_S_exp( root, 0, firstLP, false ) ;
          throw 6 ;
        } // if
      } // else
    } // if
  } // for
  
  AtomorSymbol temp_map ;
  User_func temp_func ;
  if ( all_arg[0] -> token.empty() ) { // user define function ( define ( f a ) ( a ) )
    Get_Arg( para, all_arg[0] ) ;
    string funcname = para[0] -> token ;
    funcname = "#<procedure " + funcname + ">" ;
    temp_func.func = ReturnATree( funcname ) ;
    for ( int k = 1 ; k < para.size() ; k ++ ) {
      // 應該要是symbol
      if ( Is_Symbol(  para[k] -> token ) )
        temp_func.argument.push_back( para[k] -> token ) ;
    } // for
    
    temp_func.root = all_arg[all_arg.size()-1] ;
    gMyfunc[para[0] -> token] = temp_func ; //
    
    result = para[0] ;
  } // if
  else {
    if ( ! all_arg[1] -> token.empty() ) { // ( define a 1 )
      if ( ! Is_Symbol( all_arg[1] -> token ) ) { // 第２個 argument是一個普通的Atom
        if ( all_arg[1] -> token == "nil" ) {
          Initial_temp_struct( "Atom", temp_map ) ;
          temp_map.root -> left = all_arg[1] ;
        } // if
        else
          temp_map.root  = all_arg[1] ;
        if ( all_arg[0] -> token.empty() ) {
          
        } // if
        else {
          temp_map.type = "Atom" ;
          gSymbolmap[all_arg[0] -> token] = temp_map ;
        } // else
      } // if
      else { // 第２個 argument是一個 Symbol ( define a b )
        Initial_temp_struct( "Atom", temp_struct ) ;
        temp_struct.root -> left = all_arg[1] ;
        arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
        if ( ! arg1 -> token.empty() ) {
          arg1 -> left = NULL ;
          arg1 -> right = NULL ;
        } // if
        
        if ( gMyfunc.find( all_arg[1] -> token ) != gMyfunc.end() ) {
          temp_func = gMyfunc[all_arg[1] -> token] ;
          gMyfunc[all_arg[0] -> token] = temp_func ;
        } // if
        else {
          temp_map.root = arg1 ;
          gSymbolmap[all_arg[0]->token] = temp_map ;
          if ( gMyfunc.find( all_arg[0]->token ) != gMyfunc.end() )
            gMyfunc.erase( all_arg[0]->token ) ;
        } // else
        
        
      } // else
    } // if
    else { // ( define a '( 1 2 ) )
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[1];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      
      if ( arg1 -> token == "#<procedure lambda>" ) { // ( define a ( lambda () () ) )
        if ( gLocalLamb.empty() ) {
          temp_map.type = "Func" ;
          temp_map.root  = arg1 ;
          gSymbolmap[all_arg[0]->token] = temp_map ;
          if ( gMyfunc.find( all_arg[0]->token ) != gMyfunc.end() )
            gMyfunc.erase( all_arg[0]->token ) ;
        } // if
        else {
          temp_func = gLocalLamb.back() ;
          gMyfunc[all_arg[0]->token] = temp_func ;
        } // else
      } // if
      else if ( arg1 -> token[0] == '#' && arg1 -> token[1] == '<' ) {
        temp_map.type = "Func" ;
        temp_map.root  = arg1 ;
        gSymbolmap[all_arg[0]->token] = temp_map ;
        if ( gMyfunc.find( all_arg[0]->token ) != gMyfunc.end() )
          gMyfunc.erase( all_arg[0]->token ) ;
      } // else if
      else {
        temp_map.type = "S_exp" ;
        temp_map.root = arg1 ;
        gSymbolmap[all_arg[0]->token] = temp_map ;
        if ( gMyfunc.find( all_arg[0]->token ) != gMyfunc.end() )
          gMyfunc.erase( all_arg[0]->token ) ;
      } // else
    } // else
    
    result = all_arg[0] ;
  } // else
  
  
  return result ;
} // Function_define()

Tree * Function_list( Tree * result, Tree * root, int sum ) { // 編算數量邊找symbol
  vector<Tree *> all_arg, result_arg ;
  Tree * cur ;
  Tree * arg1 ;
  Tree * temp = new Tree ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  Get_Arg( all_arg,  root -> right ) ;

  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // else
    
    if ( arg1 -> left != NULL ) {
      if ( arg1 -> left -> token == "nil" || arg1 -> left -> token == "#t" )
        arg1 = arg1 -> left ;
    } // if
    
    result_arg.push_back( arg1 ) ;
  } // for
  
  result = temp ;
  for ( int i = 0 ; i < result_arg.size() ; i ++ ) {
    temp -> left = result_arg[i] ;
    temp -> right = new Tree ;
    temp = temp -> right ;
  } // for

  temp -> left = NULL ;
  temp -> right = NULL ;
  temp -> token = "nil" ;
  if ( result_arg.size() == 0 ) { // 如果一個argument都沒有的話 error
    result -> token.clear() ;
    result -> left = new Tree ;
    result -> left -> token = "nil" ;
    result -> left -> left = NULL ;
    result -> left -> right = NULL ;
    result -> right = NULL ;
  } // if
  
  return result ;
} // Function_list()

Tree * Function_car( Tree * result, Tree * root, int sum ) {
  Tree * cur ;
  Tree * arg1 ;
  bool firstLP = true ;
  AtomorSymbol temp_struct ;
  result = Initial_Result( result ) ;
  result -> right = new Tree ;
  result -> right -> left = NULL ;
  result -> right -> right = NULL ;
  temp_struct.type = "S_exp" ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
 
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else {
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( ! arg1 -> token.empty() ) {
    cout << "ERROR (car with incorrect argument type) : " ; // << arg1 -> token << endl ;
    Print_S_exp( arg1, 0, firstLP, false ) ;
    throw 6 ;
  } // if
  
  result = arg1 -> left ;
  if ( result -> token == "nil" ) {
    result = new Tree ;
    result -> left = arg1 -> right ;
    result -> right = new Tree ;
    result -> right -> left = NULL ;
    result -> right -> right = NULL ;
  } // if
  
  return result ;
} // Function_car()

Tree * Function_cdr( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  bool firstLP = true ;
  AtomorSymbol temp_struct ;
  result = Initial_Result( result ) ;
  result -> right = new Tree ;
  result -> right -> left = NULL ;
  result -> right -> right = NULL ;
  temp_struct.type = "S_exp" ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) { // 如果argument 是一個 s_expression
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ; // arg1 裡面有樹
  } // if
  else { // 如果argument 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  result = arg1 -> right ;
  if ( ! arg1 -> token.empty() ) {
    cout << "ERROR (cdr with incorrect argument type) : " ; // << arg1 -> token << endl ;
    Print_S_exp( arg1, 0, firstLP, false ) ;
    throw 6 ;
  } // if
  
  if ( result -> token == "nil" ) {
    result = new Tree ;
    result -> left = arg1 -> right ;
    result -> right = new Tree ;
    result -> right -> left = NULL ;
    result -> right -> right = NULL ;
  } // if
  
  return result ;
} // Function_cdr()

Tree * Function_Atom_or_Not( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    
    if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else
      result -> left -> token = "#t" ;
  } // if
  else {
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else
      result -> left -> token = "#t" ;
  } // else
  
  return result ;
} // Function_Atom_or_Not()

Tree * Function_Pair_or_Not( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  Tree * arg2 = new Tree ;
  arg2 -> right = new Tree ;
  arg2 -> right -> left = new Tree ;
  arg2 -> right -> right = NULL ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  
  if ( arg1 -> token.empty() ) { // 是一個S_exp
    temp_struct.root = arg1 ;
    arg2 -> right -> left = Eval_All( temp_struct, arg1, sum + 1 ) ;
    result = Function_Atom_or_Not( result, root, sum + 1 ) ;
    if ( result -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // if
  else { // 有symbol define的
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ; // ( define a '(3.4) )
    result = Function_Atom_or_Not( result, root, sum + 1 ) ;
    if ( result -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // else
  
  return result ;
} // Function_Pair_or_Not()

Tree * Function_List_or_Not( Tree * result, Tree * root, int sum ) {
  Tree * cur = root -> right ;
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = cur -> left ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( ! Pure_list( arg1 ) ) {
    result -> left -> token = "nil" ;
  } // if
  else
    result -> left -> token = "#t" ;
  /*
  if ( arg1 -> token.empty() ) {
    temp_struct.type = "S_exp" ;
    temp_struct.root = cur -> left ;
    arg1 = Function_Pair_or_Not( result, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Function_Pair_or_Not( result, arg1, sum + 1 ) ;
  } // else
  */
  if ( !arg1 -> token.empty() )
    result -> left -> token = "nil" ;
  else if ( arg1 -> left != NULL && arg1 -> left -> token == "#t" )
    result -> left -> token = "nil" ;
  else if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" )
    result -> left -> token = "nil" ;
   
  return result ;
} // Function_List_or_Not()

Tree * Function_Null_or_Not( Tree * result, Tree * root, int sum ) {
  Tree * cur = root -> right ;
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = cur -> left ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  
  if ( arg1 -> left != NULL ) {
    if ( arg1 -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // if
  else {
    if ( arg1 -> token == "nil" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // else
  
  return result ;
} // Function_Null_or_Not()

Tree * Function_Integer_or_Not( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else {
      if ( HavedDot( arg1 -> token ) )
        result -> left -> token = "nil" ;
      else {
        if ( IsInteger( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // else
    } // else
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else {
      if ( HavedDot( arg1 -> token ) )
        result -> left -> token = "nil" ;
      else {
        if ( IsInteger( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // else
    } // else
  } // else
  
  return result ;
} // Function_Integer_or_Not()

Tree * Function_Real_or_Not( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else {
      if ( HavedDot( arg1 -> token ) ) {
        if ( Isfloat( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // if
      else {
        if ( IsInteger( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // else
    } // else
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else {
      if ( HavedDot( arg1 -> token ) ) {
        if ( Isfloat( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // if
      else {
        if ( IsInteger( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // else
    } // else
  } // else
  
  return result ;
} // Function_Real_or_Not()

Tree * Function_Number_or_Not( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else {
      if ( HavedDot( arg1 -> token ) ) {
        if ( Isfloat( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // if
      else {
        if ( IsInteger( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // else
    } // else
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    if ( arg1 -> token.empty() )
      result -> left -> token = "nil" ;
    else {
      if ( HavedDot( arg1 -> token ) ) {
        if ( Isfloat( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // if
      else {
        if ( IsInteger( arg1 -> token ) )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // else
    } // else
  } // else
  
  return result ;
} // Function_Number_or_Not()

Tree * Function_String_or_Not( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( arg1 -> token.empty() )
    result -> left -> token = "nil" ;
  else {
    if ( IsString( arg1 -> token ) )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // else
  
  return result ;
} // Function_String_or_Not()

Tree * Function_Boolean_or_Not( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( arg1 -> token.empty() ) {
    if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else if ( arg1 -> left != NULL && arg1 -> left -> token == "#t" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // if
  else {
    if ( arg1 -> token == "nil" )
      result -> left -> token = "#t" ;
    else if ( arg1 -> token == "#t" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // else
  
  return result ;
} // Function_Boolean_or_Not()

Tree * Function_Symbol_or_Not( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  Tree * arg1 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1  ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( ! arg1 -> token.empty() && Is_Symbol( arg1 -> token ) )
    result -> left -> token = "#t" ;
  else
    result -> left -> token = "nil" ;

  return result ;
} // Function_Symbol_or_Not()

Tree * Function_Plus_Minus_Multi( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct, func_struct ;
  Tree * arg1 ;
  result = Initial_Result( result ) ;
  float myfloat = 0.0, tempfloat ;
  int myint = 0, tempint ;
  bool intorfloat = true, temp_intorfloat = true ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  func_struct.type = "Atom" ;
  func_struct.root = root ;
  string funcname = Eval_All( func_struct, result, sum + 1 ) -> token ;
  string name = funcname ;
  name.erase( 0, 12 ) ;
  name.erase( name.size() -1, 1 ) ;
  
  if ( all_arg.size() < 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = name ;
    throw 5 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      if ( arg1 -> right != NULL ) {
        cout << "ERROR (" << name << " with incorrect argument type) : ( " ;
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
      
      arg1 = arg1 ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // else
    
    if ( ! arg1 -> token.empty() ) {
      gGrammarFaultString = name ;
      Return_Value( arg1 -> token, temp_intorfloat, tempint, tempfloat ) ;
      if ( temp_intorfloat == false && intorfloat == true ) {
        intorfloat = false ;
        myfloat = myint ;
      } // if
      
      if ( intorfloat == true ) {
        if ( funcname == "#<procedure +>" ) {
          if ( temp_intorfloat == true ) myint += tempint ;
          else myint += tempfloat ;
        } // if
        else if ( funcname == "#<procedure ->" ) {
          if ( temp_intorfloat == true ) myint -= tempint ;
          else myint -= tempfloat ;
        } // if
        else if ( funcname == "#<procedure *>" ) {
          if ( temp_intorfloat == true ) myint *= tempint ;
          else myint *= tempfloat ;
        } // else if
        else if ( funcname == "#<procedure />" ) {
          if ( ( temp_intorfloat == false && tempfloat == 0 ) ||
               ( temp_intorfloat == true && tempint == 0 ) ) {
            cout << "ERROR (division by zero) : /" << endl ;
            throw 6 ;
          } // if
          
          if ( temp_intorfloat == true ) myint /= tempint ;
          else myint /= tempfloat ;
        } // else if
      } // if
      else {
        if ( funcname == "#<procedure +>" ) {
          if ( temp_intorfloat == true ) myfloat += tempint ;
          else myfloat += tempfloat ;
        } // if
        else if ( funcname == "#<procedure ->" ) {
          if ( temp_intorfloat == true ) myfloat -= tempint ;
          else myfloat -= tempfloat ;
        } // if
        else if ( funcname == "#<procedure *>" ) {
          if ( temp_intorfloat == true ) myfloat *= tempint ;
          else myfloat *= tempfloat ;
        } // else if
        else if ( funcname == "#<procedure />" ) {
          if ( ( temp_intorfloat == false && tempfloat == 0 ) ||
               ( temp_intorfloat == true && tempint == 0 ) ) {
            cout << "ERROR (division by zero) : /" << endl ;
            throw 6 ;
          } // if
          
          if ( temp_intorfloat == true ) myfloat /= tempint ;
          else myfloat /= tempfloat ;
        } // else if
      } // else
    } // if
    else {
      if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" ) {
        cout << "ERROR (" << name << " with incorrect argument type) : " ;
      } // if
      else
        cout << "ERROR (" << name << " with incorrect argument type) : ( " ;
      bool firstLP = true ;
      Print_S_exp( arg1, 0, firstLP, false ) ;
      throw 6 ;
    } // else
    
    if ( i == 0 ) {
      intorfloat = temp_intorfloat ;
      myfloat = tempfloat ;
      myint = tempint ;
    } // if
  } // for
  
  stringstream sstream;
  if ( intorfloat == true ) sstream << myint ;
  else sstream << myfloat ;
  
  string temp = sstream.str() ;
  // result -> left -> token = temp ;
  result -> token = temp ;
  if ( intorfloat == false )
    if ( ! HavedDot( temp ) )
      result -> token = result -> token + ".0"  ;
  result -> right = NULL ;
  result -> left = NULL ;
  
  return result ;
} // Function_Plus_Minus_Multi()

Tree * Function_Not( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 1 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  arg1 = all_arg[0] ;
  if ( arg1 -> token.empty() ) {
    temp_struct.root = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else { // 是一個symbol
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = arg1 ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( arg1 -> left != NULL ) {
    if ( arg1 -> left -> token == "nil" )
      result -> left -> token = "#t" ;
    else
      result -> left -> token = "nil" ;
  } // if
  else if ( arg1 -> token == "nil" )
    result -> left -> token = "#t" ;
  else
    result -> left -> token = "nil" ;
  
  return result ;
} // Function_Not()

Tree * Function_And( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  bool check = true ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() < 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() && check == true ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // else
    
    if ( arg1 -> left != NULL ) {
      if ( arg1 -> left -> token == "nil" ) {
        result -> token.clear() ;
        result = Initial_Result( result ) ;
        result -> left -> token = "nil" ;
        check = false ;
      } // if
      else if ( arg1 -> left -> token == "#t" ) {
        result -> token.clear() ;
        result -> token = "#t" ;  // modified
        result -> left = NULL ;
        result -> right = NULL ;
      } // else if
      else {
        result = arg1 ;
      } // else
    } // if
    else if ( arg1 -> token == "nil" ) {
      result -> token.clear() ;
      result = Initial_Result( result ) ;
      result -> left -> token = "nil" ;
      check = false ;
    } // else if
    else if ( arg1 -> token == "#t" ) {
      result -> token.clear() ;
      result = Initial_Result( result ) ;
      result -> left -> token = "#t" ;
    } // else if
    else {
      result = arg1 ;
    } // else
    
  } // for
  
  return result ;
} // Function_And()

Tree * Function_Or( Tree * result, Tree * root, int sum ) {
  Tree * cur = root -> right ;
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  bool check = true ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() < 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() && check == true ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
    } // else
  
  
    if ( arg1 -> left != NULL ) {
      if ( arg1 -> left -> token == "nil" ) {
        result -> token.clear() ;
        result = Initial_Result( result ) ;
        result -> left -> token = "nil" ;
      } // if
      else {
        result = arg1 ;
        check = false ;
      } // else
    } // if
    else if ( arg1 -> token == "nil" ) {
      result -> token.clear() ;
      result = Initial_Result( result ) ;
      result -> left -> token = "nil" ;
    } // else if
    else {
      result = arg1 ;
      check = false ;
    } // else
  } // for
  
  return result ;
} // Function_Or()

Tree * Function_Than( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  bool check = true ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg ;
  int tempint1, tempint2 ;
  float tempfloat1, tempfloat2 ;
  bool intorfloat1, intorfloat2 ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() < 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  Tree * arg1 = new Tree ;
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
      if ( arg1 -> right != NULL ) {
        cout << "ERROR (" << root -> left -> token << " with incorrect argument type) : " ;
        if ( arg1 -> right != NULL ) {
          if ( arg1 -> right -> left != NULL ) {
            cout << "( " ;
          } // if
          else {
            if ( ! arg1 -> right -> token.empty() )
              cout << "( " ;
          } // else
        } // if
        
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // else
    
    if ( ! arg1 -> token.empty() && ( IsInteger( arg1 -> token )
                                      || Isfloat( arg1 -> token ) ) )
      arg1 = new Tree ;
    else {
      cout << "ERROR (" << root -> left -> token << " with incorrect argument type) : " ;
      if ( arg1 -> right != NULL ) {
        if ( arg1 -> right -> left != NULL ) {
          cout << "( " ;
        } // if
        else {
          if ( ! arg1 -> right -> token.empty() )
            cout << "( " ;
        } // else
      } // if
      
      bool firstLP = true ;
      Print_S_exp( arg1, 0, firstLP, false ) ;
      throw 6 ;
    } // else
  } // for
  
  Return_Value( result_arg[0] -> token, intorfloat1, tempint1, tempfloat1 ) ;
  
  for ( int j = 1 ; j < result_arg.size() && check == true ; j ++ ) {
    Return_Value( result_arg[j] -> token, intorfloat2, tempint2, tempfloat2 ) ;
    if ( intorfloat1 == true )
      tempfloat1 = tempint1 ;
    if ( intorfloat2 == true )
      tempfloat2 = tempint2 ;
    
    if ( root -> left -> token == ">" ) {
      if ( tempfloat1 > tempfloat2 )
        check = true ;
      else
        check = false ;
    } // if
    else if ( root -> left -> token == ">=" ) {
      if ( tempfloat1 >= tempfloat2 )
        check = true ;
      else
        check = false ;
    } // else if
    else if ( root -> left -> token == "<" ) {
      if ( tempfloat1 < tempfloat2 )
        check = true ;
      else
        check = false ;
    } // else if
    else if ( root -> left -> token == "<=" ) {
      if ( tempfloat1 <= tempfloat2 )
        check = true ;
      else
        check = false ;
    } // else if
    else if ( root -> left -> token == "=" ) {
      if ( tempfloat1 == tempfloat2 )
        check = true ;
      else
        check = false ;
    } // else if
    
    intorfloat1 = intorfloat2 ;
    tempint1 = tempint2 ;
    tempfloat1 = tempfloat2 ;
 
  } // for
  
  if ( check == true )
    result -> left -> token = "#t" ;
  else
    result -> left -> token = "nil" ;
  
  return result ;
} // Function_Than()


Tree * Function_String_append( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg ;
  string string1, string2 ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() < 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  Tree * arg1 = new Tree ;
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
      if ( arg1 -> right != NULL ) {
        cout << "ERROR (" << root -> left -> token << " with incorrect argument type) : " ;
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // else
    
    arg1 = new Tree ;
  } // for
  
  if ( ! result_arg[0] -> token.empty() && IsString( result_arg[0] -> token ) ) {
    string1 = result_arg[0] -> token ;
    string1.erase( string1.size()-1, 1 ) ;
  } // if
  else {
    cout << "ERROR (" << root -> left -> token << " with incorrect argument type) : " ;
    bool firstLP = true ;
    Print_S_exp( result_arg[0], 0, firstLP, false ) ;
    throw 6 ;
  } // else
  
  for ( int j = 1 ; j < result_arg.size() ; j ++ ) {
    if ( ! result_arg[j] -> token.empty() && IsString( result_arg[j] -> token ) ) {
      string2 = result_arg[j] -> token ;
      string2.erase( 0, 1 ) ;
      string2.erase( string2.size()-1, 1 ) ;
      string1 = string1 + string2 ;
      string2.clear() ;
    } // if
    else {
      cout << "ERROR (" << root -> left -> token << " with incorrect argument type) : " ;
      bool firstLP = true ;
      Print_S_exp( result_arg[j], 0, firstLP, false ) ;
      throw 6 ;
    } // else
  } // for
  
  string1 = string1 + "\"" ;
  result -> token = string1 ;
  result -> left = NULL ;
  result -> right = NULL ;
  return result ;
} // Function_String_append()

Tree * Function_String_Than( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct, func_struct ;
  bool check = true ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg ;
  string string1, string2 ;
  Get_Arg( all_arg,  root -> right ) ;
  func_struct.type = "Atom" ;
  func_struct.root = root ;
  string funcname = Eval_All( func_struct, result, sum + 1 ) -> token ;
  if ( all_arg.size() < 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = funcname ;
    throw 5 ;
  } // if
  
  Tree * arg1 = new Tree ;
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
      if ( arg1 -> right != NULL ) {
        cout << "ERROR (" << funcname << " with incorrect argument type) : " ;
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
      
      if ( arg1 -> token.empty() || ! IsString( arg1 -> token ) ) {
        cout << "ERROR (" << funcname << " with incorrect argument type) : " ;
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
      if ( arg1 -> right != NULL ) {
        cout << "ERROR (" << funcname << " with incorrect argument type) : " ;
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
      
      if ( arg1 -> token.empty() || ! IsString( arg1 -> token ) ) {
        cout << "ERROR (" << funcname << " with incorrect argument type) : " ;
        bool firstLP = true ;
        Print_S_exp( arg1, 0, firstLP, false ) ;
        throw 6 ;
      } // if
    } // else
    
    arg1 = new Tree ;
  } // for
  
  string1 = result_arg[0] -> token ;
  for ( int j = 1 ; j < result_arg.size() && check == true ; j ++ ) {
    string2 = result_arg[j] -> token ;
    if ( funcname == "#<procedure string>?>" ) {
      if ( string1 > string2 )
        check = true ;
      else
        check = false ;
    } // if
    else if ( funcname == "#<procedure string<?>" ) {
      if ( string1 < string2 )
        check = true ;
      else
        check = false ;
    } // else if
    else if ( funcname == "#<procedure string=?>" ) {
      if ( string1 == string2 )
        check = true ;
      else
        check = false ;
    } // else if
    
    string1 = string2 ;
    string2.clear() ;
  } // for
  
  if ( check == true )
    result -> left -> token = "#t" ;
  else
    result -> left -> token = "nil" ;
  
  return result ;
} // Function_String_Than()

Tree * Function_Eqv( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  Tree * arg2 ;
  AtomorSymbol temp_struct ;
  bool check = true ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg  ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() && check == true ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // else
  } // for
  
  arg1 = result_arg[0] ;
  arg2 = result_arg[1] ;
  if ( arg1 == arg2 )
    result -> left -> token = "#t" ;
  else {
    if ( ! arg1 -> token.empty() && ! arg2 -> token.empty() ) {
      if ( IsString( arg1 -> token ) && IsString( arg2 -> token ) ) {
        if ( arg1 == arg2 )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // if
      else if ( arg1 -> token == arg2 -> token )
        result -> left -> token = "#t" ;
      else
        result -> left -> token = "nil" ;
    } // if
    else {
      if ( arg1 -> left != NULL && arg2 -> left != NULL ) {
        if ( arg1 -> left -> token == "nil" && arg1 -> left -> token == arg2 -> left -> token )
          result -> left -> token = "#t" ;
        else
          result -> left -> token = "nil" ;
      } // if
      else
        result -> left -> token = "nil" ;
    } // else
  } // else
  
  return result ;
} // Function_Eqv()

bool Is_Same_Sexp( Tree * arg1, Tree * arg2 ) {
  if ( arg1 == NULL && arg2 == NULL )
    return true ;
  if ( arg1 == NULL || arg2 == NULL )
    return false ;
  if ( ( arg1 -> token == arg2 -> token ) && Is_Same_Sexp( arg1 -> left, arg2 -> left )
       && ( Is_Same_Sexp( arg1 -> right, arg2 -> right ) ) )
    return true ;
  else
    return false ;
  return true  ;
} // Is_Same_Sexp()

Tree * Function_Equal( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  Tree * arg2 ;
  AtomorSymbol temp_struct ;
  bool check = true ;
  temp_struct.type = "S_exp" ;
  result -> left = new Tree ;
  result -> left -> left = NULL ;
  result -> left -> right = NULL ;
  result -> right = NULL ;
  vector<Tree *> all_arg, result_arg  ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 2 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() && check == true ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // else
  } // for
  
  arg1 = result_arg[0] ;
  arg2 = result_arg[1] ;
  if ( Is_Same_Sexp( arg1, arg2 ) ) result -> left -> token = "#t" ;
  else result -> left -> token = "nil" ;
  
  return result ;
} // Function_Equal()

Tree * Function_If( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  int check = 0 ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg  ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() != 2 && all_arg.size() != 3 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  if ( all_arg[0] -> token.empty() ) {
    temp_struct.type = "S_exp" ;
    temp_struct.root = all_arg[0];
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // if
  else {
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = all_arg[0] ;
    arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
  } // else
  
  if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" ) check = 2 ;
  else if ( arg1 -> token == "nil" ) check = 2 ;
  else check = 1 ;
  
  if ( check == 1 ) {
    if ( all_arg[1] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[1];
      result = Eval_All( temp_struct, result, sum + 1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[1] ;
      result = Eval_All( temp_struct, result, sum + 1 ) ;
    } // else
  } // if
  else {
    if ( all_arg.size() == 2 ) {
      cout << "ERROR (no return value) : ( " ;
      bool firstLP = true ;
      Print_S_exp( root, 0, firstLP, false ) ;
      throw 6 ;
    } // if
    else {
      if ( all_arg[2] -> token.empty() ) {
        temp_struct.type = "S_exp" ;
        temp_struct.root = all_arg[2];
        result = Eval_All( temp_struct, result, sum + 1 ) ;
      } // if
      else {
        Initial_temp_struct( "Atom", temp_struct ) ;
        temp_struct.root -> left = all_arg[2] ;
        result = Eval_All( temp_struct, result, sum + 1 ) ;
      } // else
    } // else
  } // else
  
  return result ;
} // Function_If()

Tree * Function_Cond( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  AtomorSymbol temp_struct, result_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg, middle_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  bool firstLP = true, check = false ;
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( ! Pure_list( all_arg[i] ) ) {
      cout << "ERROR (COND format) : ( " ;
      Print_S_exp( root, 0, firstLP, false ) ;
      throw 6 ;
    } // if
    else if ( ! all_arg[i] -> token.empty() ) {
      cout << "ERROR (COND format) : ( " ;
      Print_S_exp( root, 0, firstLP, false ) ;
      throw 6 ;
    } // if
  } // for
  
  if ( all_arg.size() < 1 ) {
    cout << "ERROR (COND format) : ( " ;
    Print_S_exp( root, 0, firstLP, false ) ;
    throw 6 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() ; i ++  ) {
    Get_Arg( result_arg, all_arg[i] -> right ) ;
    if ( result_arg.size() < 1 ) {
      cout << "ERROR (COND format) : ( " ;
      Print_S_exp( root, 0, firstLP, false ) ;
      throw 6 ;
    } // if
    
    result_arg.clear() ;
  } // for
  
  for ( int i = 0 ; i < all_arg.size() && check == false ; i ++ ) {
    if ( all_arg[i] -> left -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i] -> left ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      middle_arg.push_back( arg1 ) ;
    } // if
    else {
      if ( i == all_arg.size() - 1 && all_arg[i] -> left -> token == "else" ) {
        arg1 = new Tree ;
        arg1 -> token = "else" ;
        arg1 -> left = NULL ;
        arg1 -> right = NULL ;
      } // if
      else {
        Initial_temp_struct( "Atom", temp_struct ) ;
        temp_struct.root -> left = all_arg[i] -> left ;
        arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      } // else
      
      middle_arg.push_back( arg1 ) ;
    } // else
    
    
    if ( i == all_arg.size() -1 ) {
      if ( arg1 -> token == "else" )
        check = true ;
      else {
        if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" )
          check = false ;
        else
          check = true ;
      } // else
    } // if
    else {
      if ( arg1 -> left != NULL && arg1 -> left -> token == "nil" )
        check = false ;
      else
        check = true ;
    } // else
    
    if ( check == true ) {
      Get_Arg( result_arg, all_arg[i] -> right ) ;
      for ( int j = 0 ; j < result_arg.size() ; j ++ ) {
        if ( result_arg[j] -> token.empty() ) {
          result_struct.type = "S_exp" ;
          result_struct.root = result_arg[j] ;
          arg1 = Eval_All( result_struct, arg1, sum + 1 ) ;
        } // if
        else {
          Initial_temp_struct( "Atom", result_struct ) ;
          result_struct.root -> left = result_arg[j] ;
          arg1 = Eval_All( result_struct, arg1, sum + 1 ) ;
        } // else
        
        if ( j == result_arg.size() -1 ) {
          if ( arg1 -> token.empty() ) {
            result = arg1 ;
          } // if
          else
            result = arg1 ;
          // result -> left = arg1 ;
        } // if
      } // for
    } // if
    
  } // for

  
  if ( check == false ) {
    cout << "ERROR (no return value) : ( " ;
    Print_S_exp( root, 0, firstLP, false ) ;
    throw 6 ;
  } // if
  
  return result ;
} // Function_Cond()

Tree * Function_Begin( Tree * result, Tree * root, int sum ) {
  Tree * arg1 ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() < 1 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i];
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, arg1, sum + 1 ) ;
      result_arg.push_back( arg1 ) ;
    } // else
  } // for
  
  arg1 = result_arg.back() ;
  result = arg1 ;

  return result ;
} // Function_Begin()

void Function_CleanEnvironment() {
  gSymbolmap.clear() ;
  gMyfunc.clear() ;
} // Function_CleanEnvironment()


void Function_Exit( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() > 0 ) {
    gGrammarFault = true ;
    gGrammarFaultKind = 6 ;
    gGrammarFaultString = root -> left -> token ;
    throw 5 ;
  } // if
} // Function_Exit()

Tree * gLocal_variable ;
bool Find_Local_Variable( string token ) {
  vector< map<string,Tree*> > temp_stack ;
  map<string,Tree*> temp_map ;
  
  for ( int i = gLocalvariable.size() -1 ; i >= 0 ; i -- ) {
    if ( gLocalvariable[i].find( token ) != gLocalvariable[i].end() ) {
      gLocal_variable = gLocalvariable[i][token] ;
      return true ;
    } // if
  } // for
  /*
  temp_stack = gLocalvariable ;
  while ( ! temp_stack.empty() ) {
    temp_map = temp_stack.back() ;
    if ( temp_map.find( token ) != temp_map.end() ) {
      gLocal_variable = temp_map[token] ;
      return true ;
    } // if
    
    temp_stack.pop_back() ;
  } // while
  */
  
  return false ;
} // Find_Local_Variable()

Tree * Function_Let( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct, local_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  Tree * arg2 ;
  vector<Tree *> all_arg, result_arg, sub_arg ;
  map<string, Tree*> localmap ;
  Get_Arg( all_arg,  root -> right ) ;
  if ( all_arg.size() < 2 ) {
    cout << "ERROR (let format)" << endl ;
    throw 6 ;
  } // if
  
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( i == 0 ) { // 第一個argument的地方
      Get_Arg( result_arg, all_arg[i] ) ; //
      for ( int j = 0 ; j < result_arg.size() ; j ++ ) {
        if ( j == 0 && result_arg[j] -> left -> token.empty() ) { // 判斷第一個是要symbol
          cout << "ERROR (let format)" << endl ;
          throw 6 ;
        } // if
        
        Get_Arg( sub_arg, result_arg[i] ) ;
        if ( sub_arg.size() != 2 ) { // 總共必須要是兩個argument 不然就是( x 3 4 ) 之類
          cout << "ERROR (let format)" << endl ;
          throw 6 ;
        } // if
        
        sub_arg.clear() ;
      } // for
    } // if
  } // for
  
  for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
    if ( i == 0 ) {
      for ( int j = 0 ; j < result_arg.size() ; j ++ ) {
        arg2 = result_arg[j] -> right -> left ;
        if ( arg2 -> token.empty() ) {
          temp_struct.type = "S_exp" ;
          temp_struct.root = arg2 ;
          arg2 = Eval_All( temp_struct, arg2, sum + 1 ) ;
        } // if
        else {
          Initial_temp_struct( "Atom", temp_struct ) ;
          temp_struct.root -> left = arg2 ;
          arg2 = Eval_All( temp_struct, arg2, sum + 1 ) ;
        } // else
        
        localmap[result_arg[j] -> left -> token] = arg2 ;
      } // for
      
      gLocalvariable.push_back( localmap ) ;
    } // if
    
    if ( i == all_arg.size() -1 ) {
      arg2 = all_arg[i] ;
      if ( arg2 -> token.empty() ) {
        temp_struct.type = "S_exp" ;
        temp_struct.root = arg2 ;
        result = Eval_All( temp_struct, arg2, sum + 1 ) ;
      } // if
      else {
        Initial_temp_struct( "Atom", temp_struct ) ;
        temp_struct.root -> left = arg2 ;
        result = Eval_All( temp_struct, arg2, sum + 1 ) ;
      } // else
    } // if
  } // for
  
  gLocalvariable.pop_back() ;
  return result ;
} // Function_Let()

Tree * Function_Perform_Userfunc( string token, Tree * root, int sum ) {
  Tree * result = new Tree ;
  Tree * arg1 = new Tree ;
  vector<Tree *> all_arg ;
  User_func func ;
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  if ( token == "lambda" ) {
    func = gLocalLamb.back() ;
    gLocalLamb.pop_back() ;
  } // if
  else
    func = gMyfunc[token] ;
  Get_Arg( all_arg, root -> right ) ;
  map<string, Tree*> localmap ;
  string temp = func.func -> left -> token ;
  if ( temp.size() >= 13 && temp[0] == '#' ) {
    temp.erase( 0, 12 ) ;
    temp.erase( temp.size() -1, 1 ) ;
  } // if
  
  if ( all_arg.size() != func.argument.size() ) {
    cout << "ERROR (incorrect number of arguments) :" ;
    cout << temp << endl ;
    throw 6 ;
  } // if
  
  for ( int i = 0 ; i < func.argument.size() ; i ++ ) { // 定義 區域變數
    if ( all_arg[i] -> token.empty() ) {
      temp_struct.type = "S_exp" ;
      temp_struct.root = all_arg[i] ;
      arg1 = Eval_All( temp_struct, all_arg[i], sum + 1 ) ;
    } // if
    else {
      Initial_temp_struct( "Atom", temp_struct ) ;
      temp_struct.root -> left = all_arg[i] ;
      arg1 = Eval_All( temp_struct, all_arg[i], sum + 1 ) ;
    } // else
    
    if ( ! arg1 -> token.empty() &&  arg1 -> token[0] == '#' ) {
      if ( arg1 -> token.size() >= 13 ) {
        string changetemp = arg1 -> token ;
        changetemp.erase( 0, 12 ) ;
        changetemp.erase( changetemp.size() - 1, 1 ) ;
        if ( ginternalfunc.find( changetemp ) == ginternalfunc.end() ) {
          arg1 -> token = changetemp ;
        } // if
      } // if
    } // if
   
    localmap[func.argument[i]] = arg1;
  } // for
  
  gLocalvariable.push_back( localmap ) ; // push 到 stack
  if ( func.root -> token.empty() ) {
    temp_struct.type = "S_exp" ;
    temp_struct.root = func.root ;
    result =  Eval_All( temp_struct, result, sum + 1 ) ;
  } // if
  else {
    Initial_temp_struct( "Atom", temp_struct ) ;
    temp_struct.root -> left = func.root ;
    result =  Eval_All( temp_struct, result, sum + 1 ) ;
  } // else
  
  gLocalvariable.pop_back() ;
  return result ;
} // Function_Perform_Userfunc()

Tree * Function_Lambda( Tree * result, Tree * root, int sum ) {
  AtomorSymbol temp_struct ;
  temp_struct.type = "S_exp" ;
  result = Initial_Result( result ) ;
  vector<Tree *> all_arg, result_arg ;
  Get_Arg( all_arg,  root -> right ) ;
  User_func lamb_func ;
  string funcname = root -> left -> token ;
  if ( gLocalLamb.size() == 0 ) {
    if ( all_arg.size() < 2 ) {
      cout << "ERROR (lambda format)" << endl ;
      throw 6 ;
    } // if
    
    for ( int i = 0 ; i < all_arg.size() ; i ++ ) {
      if ( i == 0 ) {
        if ( ! all_arg[i] -> token.empty() ) {
          if ( all_arg[i] -> token != "nil" ) {
            cout << "ERROR (lambda format)" << endl ;
            throw 6 ;
          } // if
        } // if
        
        Get_Arg( result_arg, all_arg[i] ) ;
        for ( int j = 0 ; j < result_arg.size() ; j ++ ) {
          if ( result_arg[j] -> token.empty() ) { // 必須是symbol 所以token不能是空的
            cout << "ERROR (lambda format)" << endl ;
            throw 6 ;
          } // if
        } // for
      } // if
    } // for
    
    lamb_func.func = ReturnATree( "#<procedure " + funcname + ">" ) ;
    for ( int k = 0 ; k < result_arg.size() ; k ++ ) {
      // 應該要是symbol
      if ( Is_Symbol( result_arg[k] -> token ) )
        lamb_func.argument.push_back( result_arg[k] -> token ) ;
      else {
        cout << "ERROR (lambda format)" << endl ;
        throw 6 ;
      } // else
    } // for
    
    lamb_func.root = all_arg[all_arg.size()-1] ;
    gLocalLamb.push_back( lamb_func ) ;
    result = lamb_func.func -> left ;
  } // if
  else {
    lamb_func = gLocalLamb.back() ;
    if ( all_arg.size() != lamb_func.argument.size() ) {
      cout << "ERROR (incorrect number of arguments) : lambda expression" << endl ;
      throw 6 ;
    } // if
    
    result = Function_Perform_Userfunc( funcname, root, sum ) ;
  } // else
  
  
  return result ;
} // Function_Lambda()



Tree * Eval_All( AtomorSymbol & newAtom, Tree * result, int sum ) {
  bool firstLP = true ;
  string  funcname ;
  result = new Tree ;
  result -> left = NULL ;
  result -> right = NULL ;
  string token = newAtom.root -> left -> token ;
  if ( newAtom.type == "Atom" && ! Is_Symbol( token ) ) { // is an atom
    result = newAtom.root -> left ;
    if ( token == "nil" ) result = newAtom.root ;
    // cout << "is an atom " << newAtom.root -> left -> token << endl ;
  } // if
  else if ( newAtom.type == "Atom" && Is_Symbol( token ) ) {
    firstLP = true ;
    // cout << "is not S_exp and is a Symbol" << endl ;
    if ( gSymbolmap.find( token ) == gSymbolmap.end() &&
         ginternalfunc.find( token ) == ginternalfunc.end() &&
         gMyfunc.find( token ) == gMyfunc.end() && gLocalvariable.empty() ) {
      gGrammarFaultKind = 5 ;
      gGrammarFaultString = token ;
      gGrammarFault = true ;
      throw 5 ;
    } // if
    else {
      if ( gMyfunc.find( token ) != gMyfunc.end() ) {
        result = gMyfunc[token].func -> left ;
      } // if
      else if ( !gLocalvariable.empty() && Find_Local_Variable( token ) ) {
        result = gLocal_variable ;
      } // else if
      else if ( gSymbolmap.find( token ) != gSymbolmap.end() )
        result = gSymbolmap[token].root ;
      else if ( ginternalfunc.find( token ) != ginternalfunc.end() )
        result = ginternalfunc[token] -> left ;
      
      
      else {
        gGrammarFaultKind = 5 ;
        gGrammarFaultString = token ;
        gGrammarFault = true ;
        throw 5 ;
      } // else
    } // else
  } // else if
  else { // isn't bounded
    // cout << "Is a S_exp" << endl ;
    if ( token != "quote" && ! Pure_list( newAtom.root ) ) { // is not a pure list
      cout << "ERROR (non-list) : ( " ;
      Print_S_exp( newAtom.root, 0, firstLP, false ) ;
      throw 6 ;
    } // if
    else if ( ! Is_Symbol( token ) ) { // 第一個argument 是 atom 不是symbol
      cout << "ERROR (attempt to apply non-function) : " << token << endl ;
      throw 6 ;
    } // else if
    else if ( Is_Symbol( token ) && ! token.empty() ) { // 第一個argument 是 Symbol SYM
      // 第一個argument 是 Symbol SYM 而且是internal function
      // cout << "the first argument is a Symbol" << endl ;
      
      if ( ginternalfunc.find( token ) != ginternalfunc.end() ||
           gSymbolmap.find( token ) != gSymbolmap.end() ||
           ( ! gLocalvariable.empty() && Find_Local_Variable( token ) ) || gMyfunc.find( token )
           != gMyfunc.end() ) { //
        if ( gMyfunc.find( token ) != gMyfunc.end() )
          funcname = token ;
        else if ( ! gLocalvariable.empty() && Find_Local_Variable( token ) )
          funcname = gLocal_variable -> token ;
        else if ( ginternalfunc.find( token ) != ginternalfunc.end() )
          funcname = ginternalfunc[token] -> left -> token ;
        else if ( gSymbolmap.find( token ) != gSymbolmap.end() )
          funcname = gSymbolmap[token].root -> token ;
        
        
        // cout << "and it is an internal function " << funcname << endl ;
        if ( ( funcname == "#<procedure define>" || funcname == "#<procedure clean-environment>" ||
               funcname == "#<procedure exit>" ) && sum > 1 ) {
          if ( funcname == "#<procedure clean-environment>" )
            cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl ;
          else if ( funcname == "#<procedure define>" )
            cout << "ERROR (level of DEFINE)" << endl ;
          else if ( funcname == "#<procedure exit>" )
            cout << "ERROR (level of EXIT)" << endl ;
            
          throw 6 ;
        } // if
        else if ( funcname == "#<procedure define>" || funcname == "#<procedure cond>" ||
                  funcname == "#<procedure clean-environment>" || funcname == "#<procedure let>" ||
                  funcname == "#<procedure lambda>" ) {
          if ( funcname == "#<procedure define>" ) {
            result = Function_define( result, newAtom.root, sum ) ;
            result -> token = result -> token + " defined" ;
          } // if
          else if ( funcname == "#<procedure cond>" )
            result = Function_Cond( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure clean-environment>" ) {
            Function_CleanEnvironment() ;
            cout << "environment cleaned" << endl ;
          } // else if
          else if ( funcname == "#<procedure let>" ) {
            result = Function_Let( result, newAtom.root, sum ) ;
          } // else if
          else if ( funcname == "#<procedure lambda>" ) {
            result = Function_Lambda( result, newAtom.root, sum ) ;
          } // else if
        } // if
        else if ( funcname == "#<procedure if>" || funcname == "#<procedure or>" ||
                  funcname == "#<procedure and>" ) {
          if ( funcname == "#<procedure and>" )
            result = Function_And( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure or>" )
            result = Function_Or( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure if>" )
            result = Function_If( result, newAtom.root, sum ) ;
          
          // 檢查argument數量 後 如果有遇到S_exp 進入下一層eval_all
          // cout << "is if or and" << endl ;
        } // else if
        else {
          // 檢查argument數量 後 如果有遇到S_exp 進入下一層eval_all
          gGrammarFaultString = token ;
          // cout << "else func " << token << endl ;
          if ( funcname == "#<procedure cons>" )
            result = Function_cons( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure quote>" ) {
            if ( newAtom.type == "Quote" )
              result = newAtom.root -> right ;
            else
              result = newAtom.root -> right -> left ;
            if ( result -> token == "nil" ) {
              result -> token.clear() ;
              result -> left = new Tree ;
              result -> left -> token = "nil" ;
              result -> left -> left = NULL ;
              result -> left -> right = NULL ;
            } // if
              
          } // else if
          else if ( funcname == "#<procedure list>" )
            result = Function_list( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure car>" )
            result = Function_car( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure cdr>" )
            result = Function_cdr( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure atom?>" )
            result = Function_Atom_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure pair?>" )
            result = Function_Pair_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure list?>" )
            result = Function_List_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure null?>" )
            result = Function_Null_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure integer?>" )
            result = Function_Integer_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure real?>" )
            result = Function_Real_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure number?>" )
            result = Function_Number_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure string?>" )
            result = Function_String_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure boolean?>" )
            result = Function_Boolean_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure symbol?>" )
            result = Function_Symbol_or_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure +>" || funcname == "#<procedure ->" )
            result = Function_Plus_Minus_Multi( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure *>" || funcname == "#<procedure />" )
            result = Function_Plus_Minus_Multi( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure not>" )
            result = Function_Not( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure >>" || funcname == "#<procedure >=>" ||
                    funcname == "#<procedure <>" )
            result = Function_Than( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure <=>" || funcname == "#<procedure =>" )
            result = Function_Than( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure string-append>" )
            result = Function_String_append( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure string>?>" || funcname == "#<procedure string<?>"
                    || funcname == "#<procedure string=?>" )
            result = Function_String_Than( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure eqv?>" )
            result = Function_Eqv( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure equal?>" )
            result = Function_Equal( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure begin>" )
            result = Function_Begin( result, newAtom.root, sum ) ;
          else if ( funcname == "#<procedure exit>" )
            Function_Exit( result, newAtom.root, sum ) ;
          else {
            result = Function_Perform_Userfunc( funcname, newAtom.root, sum ) ;
          } // else
        } // else
      } // if
      else {
        if ( gSymbolmap.find( token ) != gSymbolmap.end() ) { // 是一個有bounded的symbol
          cout << "ERROR (attempt to apply non-function) : " ;
          if ( gSymbolmap[token].type == "S_exp" ) cout << "( " ;
          Print_S_exp( gSymbolmap[token].root, 0, firstLP, false ) ;
          throw 6 ;
        } // if
        else {
          gGrammarFaultKind = 5 ;
          gGrammarFaultString = token ;
          gGrammarFault = true ;
          throw 5 ;
        } // else
      } // else
      
    } // else if
    else {
      AtomorSymbol temp_struct ;
      temp_struct.type = "S_exp" ;
      temp_struct.root = newAtom.root -> left ;
      Tree * temp ;
      temp = Eval_All( temp_struct, result, sum + 1 ) ;
      if ( ! temp -> token.empty() ) {
        newAtom.root-> left -> token = temp -> token ;
        if ( newAtom.root-> left -> token.size() >= 13 ) {
          newAtom.root-> left -> token.erase( 0, 12 ) ;
          newAtom.root-> left -> token.erase( newAtom.root-> left -> token.size() -1, 1 ) ;
        } // if
      } // if
      else {
        cout << "ERROR (attempt to apply non-function) : ( " ;
        Print_S_exp( temp, 0, firstLP, false ) ;
        throw 6 ;
      } // else
      
      result = Eval_All( newAtom, result, sum + 1 ) ;
    } // else
  } // else
  
  return result ;
} // Eval_All()


int main() {
  char firstch, ch ;
  bool exit = false ;
  string token, token2, errormessage ;
  string atomstoken ;
  vector<string> alltoken, temp ;
  Tree * root = new Tree ;
  root -> left = NULL ;
  root -> right = NULL ;
  Tree * result = new Tree ;
  result -> left = NULL ;
  result -> right = NULL ;
  cin >> utest ;
  scanf( "%c", &firstch ) ;
  int test = 0 ;
  cout << "Welcome to OurScheme!\n" << endl ;
  bool firstLP = true ;
  AtomorSymbol newAtom ;
  Internalfunc_define( ginternalfunc ) ;
  
  while ( gFileeof == false && exit == false ) {
    cout << "> " ;
    try {
      
      ReadSExp( root, newAtom ) ; // newAtom.type == S_exp or Atom or Quote
      
      result = Eval_All( newAtom, result, 1 ) ;
      // cout << "first " << result -> token << endl ;;
      
      if ( result -> right != NULL ) {
        if ( result -> right -> left != NULL ) {
          cout << "( " ;
        } // if
        else {
          if ( ! result -> right -> token.empty() )
            cout << "( " ;
        } // else
      } // if
      
      // cout << "result" << endl ;
      Print_S_exp( result, 0, firstLP, false ) ;
      newAtom.root = NULL ;
      newAtom.type.clear() ;
      result = new Tree ;
      
      cout << endl ;
    } // try
    catch ( int err ) {
      if ( err == 1 ) { // "" ˊ沒有結尾的" gGrammarFaultKind = 1 ;
        if ( gGrammarFault == true  ) {
          errormessage = ErrorMessage() ;
          gGrammarFault = false ;
          cout << errormessage << endl << endl ;
        } // if
      } // if
      else if ( err == 2 ) { // gFileof eof gGrammarFaultKind = 4 ;
        gGrammarFaultKind = 4 ;
        gGrammarFault = true ;
        errormessage = ErrorMessage() ;
        cout << errormessage ;
        gFileeof = true ;
        gGrammarFault = false ;
        firstch = cin.peek() ;
        if ( firstch != '\n' ) SkipComment() ;
      } // else if
      else if ( err == 3 ) { // . 的錯誤 gGrammarFaultKind = 2,3 ;
        if ( gGrammarFault == true  ) {
          errormessage = ErrorMessage() ;
          gGrammarFault = false ;
          cout << errormessage << endl << endl ;
          Readatoken() ;
          firstch = cin.peek() ;
          if ( firstch != '\n' ) SkipComment() ;
        } // if
      } // else if
      else if ( err == 4 ) { // ( exit )
        exit = true ;
      } // else if
      else if ( err == 5 ) {
        errormessage = ErrorMessage() ;
        gGrammarFault = false ;
        cout << errormessage << endl << endl ;
      } // else if
      else if ( err == 6 ) {
        cout << endl ;
      } // else if
    } // catch
    
    while ( ! gLocalLamb.empty() )
      gLocalLamb.pop_back() ;
    gGrammarFault = false ;
    gRow = 1 ;
    gColumn = 1 ;
    root = new Tree ;
    root -> left = NULL ;
    root -> right = NULL ;
    firstLP = true ;
    ch = cin.peek() ;
    test = cin.peek() ;
    while ( ch != '\n' && ch == ' ' && test != -1 ) {
      ch = cin.get() ;
      gColumn ++ ;
      ch = cin.peek() ;
      test = cin.peek() ;
    } // while
    
    if ( ch == '\n' ) {
      ch = cin.get() ;
      gRow = 1 ;
      gColumn = 1 ;
    } // if
    else if ( ch == ';' ) {
      SkipComment() ;
      gRow = 1 ;
      gColumn = 1 ;
    } // else if
  } // while
  
  cout << "\nThanks for using OurScheme!" ;
  return 0;
} // main()
