

# include <iostream>
# include <iomanip>
# include <vector>
# include <string>
# include <stdio.h>
# include <stdlib.h>
# include <sstream>

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
  
  if ( ch == '\n' )
    ch = cin.get() ;
} // SkipComment()

bool IsNotwhitespace( char & ch ) { // check
  int check ;
  ch = cin.peek() ;
  check = cin.peek() ;
  if ( check == -1 ) {
    gFileeof = true ;
    return false ;
  } // if
  // cout << ch << endl ;
  while ( ch == ' ' || ch == '\n' || ch == ';' ) { // 讀入直到不是空白
    ch = cin.get() ;
    gColumn ++ ; // 加一字元
    // cout << "white!" ;
    if ( ch == ';' ) {
      // cout << "white!" ;
      // ch = cin.get() ;
      // gColumn ++ ; // 加一字元
      SkipComment();
      gRow ++ ;
      gColumn = 1 ;
    } // if
    
    if ( ch == '\n' ) {
      // cout << "test111" ;
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
      return false ;
    } // if
    
  } // while
  
  ch = cin.get() ;
  gColumn ++ ; // 加一字元
  // cout << "firstchar " << ch << " //" << endl ;
  // cout << gColumn ;
  return true ;
} // IsNotwhitespace()

bool IsNumber( char ch ) { // 判斷一個字是不是數字
  if ( ( ch >= 48 && ch <= 57 ) ) return true ;
  else if ( ch == '+' || ch == '-' ) return true ;
  else return false ;
} // IsNumber()

bool IsInteger( string token ) {
  int i = 0 ;
  if ( token[0] == '+' || token[0] == '-' )
    i = 1 ;
  else
    i = 0 ;
  for ( i = i ; i < token.size() ; i ++ ) {
    if ( !IsNumber( token[i] ) ) {
      return false ;
    } // if
  } // for
  
  if ( token.size() == 0 )
    return false ;
  return true ;
} // IsInteger()

string GetaToken( char ch ) {
  string token ;
  if ( ch == '(' ) {
    token = token + ch ;
  } // if
  else if ( ch == ')' ) {
    token = token + ch ;
  } // else if
  else if ( ch == '\"' ) {
    // cout << "test" << endl ;
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
  
  // cout << "token" << token << endl ;
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
  int i = 0 ;
  bool check = false ;
  if ( token[0] == '-' || token[0] == '+' ) {
    i = 1 ;
  } // if
  else i = 0 ;
  for ( i = i ; i < token.size() ; i ++ ) {
    if ( token[i] >= 48 && token[i] <= 57 )
      check = true ;
    if ( ( token[i] >= 48 && token[i] <= 57 ) || token[i] == '.' ) {
      token = token ;
    } // if
    else {
      return false ;
    } // else
  } // for
  
  if ( check == false || !HavedDot( token ) )
    return false ;
  return true ;
} // Isfloat()

bool IsString( string token ) {
  int size = token.size() ;
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
  } // if
  
  while ( temp != ' ' && temp != '(' && temp != ')' && temp != '\n' && temp != ';' ) {
    temp = cin.get() ;
    gColumn ++ ; // 加一字元
    token = token + temp ;
    temp = cin.peek() ;
    check = cin.peek() ;
    if ( check == -1 ) {
      gFileeof = true ;
    } // if
    
  } // while
  
  
  if ( temp == '\n' ) {
    // cout << token << "test1" ;
    // temp = cin.get() ;
    // gColumn = 1 ;
    // gRow++ ;
  } // if
  
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
    } // if
    
  } // while
  
  if ( temp == '\n' ) {
    temp = cin.get() ;
    gGrammarFault = true ;
    gGrammarFaultKind = 1 ;
    // cout << "fault " << temp ;
  } // if
  else if ( temp == '\"' ) {
    cin.get() ;
    gColumn ++ ; // 加一字元
    token = token + temp ;
    temp = cin.peek() ;
    check = cin.peek() ;
    if ( check != -1 && temp == '\n' ) {
      cin.get() ;
    } // if
  } // else if
  
  
  return token ;
} // ReadString()

string Peekatoken() {
  string token ;
  char firstch ;
  
  if ( gNewtoken.empty() ) {
    if ( IsNotwhitespace( firstch ) ) {
      // cout << "firstch " << firstch << " " ;
      token = GetaToken( firstch ) ;
      gNewtoken = token ;
    } // if
    else { // 如果沒有token了
      
    } // else
  } // if
  else {
    token = gNewtoken ;
  } // else
  // cout << gColumn ;
  return token ;
} // Peekatoken()

string Readatoken() {
  string token ;
  token = gNewtoken ;
  gNewtoken.clear() ;
  return token ;
} // Readatoken()

/*
bool Split( vector<string> originstring, vector<string> & leftstring, vector<string> & rightstring ) {
  string temp ;
  bool check = false ;
  int LP = 0, RP = 0 ;
  for( int i = 1 ; i < originstring.size() -1 ; i ++ ) {
    if ( check == false ) { // 還沒遇到.
      if ( originstring[i] == "." && LP == RP ) { // .前面 LP=RP
        check = true ;
      } // if
      else {
        temp = originstring[i] ;
        leftstring.push_back( temp ) ;
      } // else
    } // if
    else {
      temp = originstring[i] ;
      rightstring.push_back( temp ) ;
    } // else
    
    if ( originstring[i] == "(" )
      LP ++ ;
    if ( originstring[i] == ")" )
      RP ++ ;
  } // for
  return true ;
} // Split()

Tree * Buildtree( vector<string> tokens, Tree * subroot ) {
  vector<string> lefttoken, righttoken ;
  Split( tokens, lefttoken, righttoken ) ;
  Tree * lefttree = new Tree ;
  Tree * righttree = new Tree ;
  if ( lefttoken.size() != 0 )
    subroot -> left = lefttree ;
  if ( righttoken.size() != 0 )
    subroot -> right = righttree ;
  
  
  if ( lefttoken.size() == 1 ) {
    lefttree -> token = lefttoken[0] ;
    // return subroot ;
  } // if
  else {
    lefttree = Buildtree( lefttoken, lefttree ) ;
  } // else
  
  if ( righttoken.size() == 1 ) {
    righttree -> token = righttoken[0] ;
    // return subroot ;
  } // if
  else {
    righttree = Buildtree( righttoken, righttree ) ;
  } // else
  
  return subroot ;
} // Buildtree()
*/
void Printtree( Tree * root ) {
  Tree * temp = root ;
  if ( temp -> left != NULL )
    Printtree( temp -> left ) ;
  if ( ! temp -> token.empty() ) {
    cout << "tree " << temp -> token ;
  } // if
  
  if ( temp -> right != NULL )
    Printtree( temp -> right ) ;
} // Printtree()


bool Else_S_exp( vector<string> & alltoken, string & token, Tree * root ) ;

bool Atom( vector<string> alltoken, string & token ) {
  string temp ;
  // cout << temp << "   1";
  if ( token.size() == 0 ) {
    return false ;
  } // if
  
  if ( token == "." ) {
    // cout << "check" ;
    gColumn -- ;
    gGrammarFault = true ;
    gGrammarFaultKind = 2 ;
    gGrammarFaultString = token ;
    return false ;
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
      token = "nil" ; // 可能會錯 ()
      // Readatoken() ;
      alltoken.push_back( token ) ;
      return true ;
    } // if
    else {
      // cout << token << " dsdsd" ;
      return false ;
    } // else
  } // else if
  else if ( token == ")" ) {
    gColumn -- ;
    gGrammarFault = true ;
    gGrammarFaultKind = 2 ;
    gGrammarFaultString = token ;
    return false ;
  } // else if
  else if ( token == "'" ) { // SYMBOL
    return false ;
  } // else if
  else {
    return true ;
  } // else
  
  return false ;
} // Atom()

int gtest = 0 ;
bool S_exp( vector<string> & alltoken, string & token, Tree * root ) {
  Tree * temp ;
  Tree * lefttree = new Tree ;
  lefttree -> left = NULL ;
  lefttree -> right = NULL ;
  Tree * righttree = new Tree ;
  righttree -> left = NULL ;
  righttree -> right = NULL ;
  
  token = Peekatoken() ;
  
  alltoken.push_back( token ) ;
  
  // cout << "test111 " << token << alltoken.size() ;
  if ( Atom( alltoken, token ) ) {
    Readatoken() ;
    lefttree -> token = token ;
    root -> left = lefttree ; // 往左建樹 給值
    // cout << " " << token << endl ; //   do something
    return true ;
  } // if
  else if ( token == "(" ) {
    // cout << " qqq " ;
    root -> left = lefttree ;
    temp = root -> left ;
    Else_S_exp( alltoken, token, temp ) ; // 另外的S_exp
    root -> right = righttree ;
    temp = root -> right ; // 向右
    token = Peekatoken() ;
    while ( token != ")" && token != "." && S_exp( alltoken, token, temp ) ) {
      // cout << "recur" <<  gtest << " "  ;
      gtest ++ ;
      token = Peekatoken() ;
      if ( token != ")" && token != "." ) {
        temp -> right = new Tree ;
        temp = temp -> right ;
        temp -> right = NULL ;
        temp -> left = NULL ;
      } // if
      
    } // while
    
    // token = Peekatoken() ;
    if ( token == "." ) {
      token = Readatoken() ;
      token = Peekatoken() ;
      if ( token != ")" && Else_S_exp( alltoken, token, temp ) ) { // else S_exp
        token = Peekatoken() ;
        if ( token == ")" ) {
          token = Readatoken() ;
          alltoken.push_back( token ) ;
          // cout << token << " 555\n" ;
          return true ;
        } // if
        else {
          token = Readatoken() ; // 之後再確認
          gGrammarFault = true ;
          gGrammarFaultKind = 2 ;
          gGrammarFaultString = token ;
          return false ;
        } // else
      } // if
      else {
        token = Readatoken() ;
        gGrammarFault = true ;
        gGrammarFaultKind = 2 ;
        gGrammarFaultString = token ;
        return false ;
      } // else
      // cout << token << " 23445\n" ;
      
    } // if
    else if ( token == ")" ) {
      token = Readatoken() ; // 沒有接nil 如果有錯注意
      gtest -- ;
      return true ;
      // 往右建樹
    } // else if
    else {
      gGrammarFault = true ;
      gGrammarFaultKind = 3 ;
      gGrammarFaultString = ")" ;
      return false ;
    } // else
  } // else if
  else if ( token == "\'" ) {
    // cout << "quote " ;
    S_exp( alltoken, token, root ) ;
  } // else if
  else {
    // cout << token ;
    return false ;
  } // else
  
  return false ;
} // S_exp()

bool Else_S_exp( vector<string> & alltoken, string & token, Tree * root ) {
  Tree * temp = root ;
  Tree * lefttree = new Tree ;
  lefttree -> left = NULL ;
  lefttree -> right = NULL ;
  Tree * righttree = new Tree ;
  righttree -> left = NULL ;
  righttree -> right = NULL ;
  
  token = Peekatoken() ;
  // token = Readatoken() ;
  alltoken.push_back( token ) ;
  // cout << "test111" ;
  if ( Atom( alltoken, token ) ) {
    Readatoken() ;
    // root -> left = lefttree ; // 往左建樹 給值
    root -> token = token ;
    // cout << " " << token << endl ; //   do something
    return true ;
  } // if
  else if ( token == "(" ) {
    cout << "test " ;
    root -> left = lefttree ;
    temp = root -> left ;
    Else_S_exp( alltoken, token, temp ) ; // 另外的S_exp
    root -> right = righttree ;
    temp = root -> right ; // 向右
    token = Peekatoken() ;
    while ( token != "." && S_exp( alltoken, token, temp ) ) {
      // cout << "recur" <<  gtest << " "  ;
      gtest ++ ;
      temp -> right = new Tree ;
      temp = temp -> right ;
      temp -> right = NULL ;
      temp -> left = NULL ;
      token = Peekatoken() ;
    } // while
    
    // token = Peekatoken() ;
    if ( token == "." ) {
      token = Readatoken() ;
      token = Peekatoken() ;
      if ( token != ")" && Else_S_exp( alltoken, token, temp ) ) { // else S_exp
        token = Peekatoken() ;
        if ( token == ")" ) {
          token = Readatoken() ;
          alltoken.push_back( token ) ;
          // cout << token << " 555\n" ;
          return true ;
        } // if
        else {
          token = Readatoken() ;
          gGrammarFault = true ;
          gGrammarFaultKind = 2 ;
          gGrammarFaultString = token ;
          return false ;
        } // else
      } // if
      else {
        token = Readatoken() ;
        gGrammarFault = true ;
        gGrammarFaultKind = 2 ;
        gGrammarFaultString = token ;
        return false ;
      } // else
    } // if
    else if ( token == ")" ) {
      token = Readatoken() ;
      temp -> token = "nil" ;
      // cout << token << " 444\n" ;
      // token = Readatoken() ;
      gtest -- ;
      return true ;
      // 往右建樹
    } // else if
    else if ( token == "exit" ) {
      // error
    } // else if
    else {
      return false ;
    } // else
  } // else if
  else if ( token == "\'" ) {
    // cout << "quote " ;
    S_exp( alltoken, token, temp ) ;
  } // else if
  else {
    // cout << "return false" << endl ;
    return false ;
  } // else
  
  return true ;
} // Else_S_exp()

void Print_S_exp( Tree * root, int num ) {
  Tree * temp = root ;
  if ( temp -> left != NULL ) {
    cout << "(" ;
    Print_S_exp( temp -> left, num ) ;
  } // if
  
  if ( ! temp -> token.empty() ) {
    for ( int j = 0 ; j < num ; j ++ )
      cout << "  " ;
    cout << "tree " << temp -> token ;
  } // if
  
  if ( temp -> right != NULL ) {
    Print_S_exp( temp -> right, num ) ;
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
      /*
      gColumn ++ ;
      aa.str( "" ) ;
      aa << gColumn ;
      column = aa.str() ;
      */
      token = "ERROR (no closing quote) : END-OF-LINE encountered at Line " ;
      token = token + row + " Column " + column ;
    } // if
    else if ( gGrammarFaultKind == 2 ) {
      token = "ERROR (unexpected token) : atom or '(' expected when token at Line " ;
      token = token + row + " Column " + column + " is >>" + gGrammarFaultString + "<<" ;
    } // else if
    else if ( gGrammarFaultKind == 3 ) {
      token = "ERROR( unexpected token) : ')' expected when token at Line " ;
      token = token + row + " Column " + column + " is >>" + gGrammarFaultString + "<<" ;
    } // else if
    else if ( gGrammarFaultKind == 4 ) {
      token = "ERROR (no more input) : END-OF-FILE encountered" ;
    } // else if
    
    gGrammarFault = false ;
  } // if
  
  return token ;
} // ErrorMessage()

int main() {
  char firstch ;
  bool exit = false ;
  string token, token2, errormessage ;
  vector<string> alltoken, temp ;
  Tree * root = new Tree ;
  cin >> utest ;
  scanf( "%c", &firstch ) ;
  int i = 0 ;
  cout << "Welcome to OurScheme!\n" << endl ;
  
  
  while ( gFileeof == false && exit == false ) {
    cout << "> " ;
    token = Peekatoken() ;
    
    alltoken.clear() ;
    // cout << "to " << token << " ot" << gColumn ;
    if ( token == "(" ) {
      // cout << "test" << token << endl ;
      alltoken.push_back( "(" ) ;
      if ( S_exp( alltoken, token, root ) ) {
        alltoken.push_back( ")" ) ;
        // cout << "size " << alltoken.size() << endl ;
        // for ( int m = 0 ; m < alltoken.size() ; m ++ )
        // cout << alltoken[m] ;
        if ( alltoken.size() == 3 )
          if ( alltoken[0] == "(" && alltoken[1] == "exit" && alltoken[2] == ")" )
            exit = true ;
      } // if
      else {
        // cout << " dd" << token ;
        gRow -- ;
        gColumn -- ;
        errormessage = ErrorMessage() ;
        cout << errormessage << endl << endl ;
      } // else
      
      // cout << " end" << token ;
    } // else if
    else  if ( Atom( temp, token ) ) {
      token2 = Readatoken() ;
      // cout << "tett" ;
      // token = Readatoken() ;
      if ( Isfloat( token ) ) {
        cout << fixed << setprecision( 3 ) << atof( token.c_str() );
      } // if
      else if ( gGrammarFault == true ) {
        errormessage = ErrorMessage() ;
        cout << errormessage ;
        gGrammarFault = false ;
        firstch = cin.peek() ;
        if ( firstch != '\n' )
          SkipComment() ;
        
      } // else if
      else
        cout << token ;
      cout << endl << endl ;
    } // if
    else if ( token == "'" ) {
      token = Readatoken() ;
    } // else if
    
    else {
      if ( token == ")" || token == "." ) {
        token = Readatoken() ;
      } // if
      // cout << "fault" << gColumn << endl ;
      if ( gGrammarFault == true  ) {
        errormessage = ErrorMessage() ;
        gGrammarFault = false ;
        cout << errormessage << endl << endl ;
        firstch = cin.peek() ;
        if ( firstch != '\n' )
          SkipComment() ;
        
      } // if
      
    } // else
    
    Printtree( root ) ;
    
    if ( gFileeof == true && exit == false ) {
      gGrammarFaultKind = 4 ;
      gGrammarFault = true ;
      errormessage = ErrorMessage() ;
      gGrammarFault = false ;
      cout << errormessage ;
    } // if
    
    gGrammarFault = false ;
    gRow = 1 ;
    gColumn = 1 ;
  } // while
  
  
  
  cout << "\nThanks for using OurScheme!" ;
  return 0;
} // main()
