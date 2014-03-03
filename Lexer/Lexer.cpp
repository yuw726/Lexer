#include <stdafx.h>
#include <stdio.h>
#include <io.h>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;

class Token
{
	public:
		string type;
		string lexeme;	
		char attribute;
		vector<long> value;
		short parameters;
		vector<string> param_type;
		short visibility;
		string class_property;
		string func_property;
};

struct tmore
{
	bool operator()(Token t1, Token t2) const
	{
		return (t1.visibility >= t2.visibility);
	}
};

Token current_token;
Token current_delim;
bool finish_parcing;
short visibility = 0;
set<Token, tmore> idtokentab;
set<Token, tmore> functokentab;
Token idtoken, functoken, exptoken;
string class_belonging = "";
string func_belonging = "";

FILE* f1;
string s;

string keyword[] = {"for", "while", "do", "foreach", "break",
					"continue", "return", "switch", "case", "default",
					"if", "else", "int", "char", "bool", 
					"decimal", "long", "struct", "object", "string", 
					"class", "interface", "delegate", "public", "private", 
					"protected", "internal", "new", "static", "virtual", 
					"override", "extern", "null", "sizeof", "namespace",
					"using", "get", "set", "float", "double",
					"void", "as", "is", "sbyte", "short",
					"byte", "uint", "ushort", "ulong", "signed",
					"unsigned", "readonly", "const", "volatile", "abstract",
					"sealed", "try", "catch", "finally", "throw",
					"ref", "out"};
unsigned char delimiter[] = {';', '(', ')', ',', '{', '}',
							 '"', '[', ']', ':', '.'};
unsigned char operators[] = {'+', '-', '=', '/', '*', '>', '<', '~', '!', '|', 
					  		 '&', '%'};

// -----------------------------------------------------------------------
// Lexer part

extern bool TryKeyword(const string &lexeme);
extern void TryDelimiter(const string lexeme);
extern bool TryDigit(const string &lexeme, unsigned long delimpos);
extern bool TryIdentifier(const string &lexeme, unsigned long delimpos, unsigned char next);
extern bool TryComplex(unsigned char delim, unsigned char next);

void GetToken(const string &lexeme, unsigned long delimpos, unsigned char next)
{
	if (!TryKeyword(lexeme))
	{
		TryIdentifier(lexeme, delimpos, next);
	}
}

bool TryKeyword(const string &lexeme)
{
	for (int i = 0; i != 62; i++)
	{
		if (lexeme == keyword[i])
		{
			cout << "Keyword: " << keyword[i] << "\n";
			current_token.type = "Keyword";
			current_token.lexeme = lexeme;
			return true;
		}
	}
	return false;
}

void TryDelimiter(unsigned char symbol)
{
	for (int i = 0; i != 11; i++)
	{ 
		if (symbol == ' ' || symbol == '\n' || symbol == '\t' || symbol == '\r')
		{
			//cout << "Space: " << symbol << "\n";
			current_delim.lexeme = symbol;
			current_delim.type = "Space";
		}
		else
		if (symbol == delimiter[i])
		{
			cout << "Delimiter: " << symbol << "\n";
			current_delim.type = "Delimiter";
			current_delim.lexeme = symbol;
		}
		else
		if (symbol == operators[i])
		{
			cout << "Operator: " << symbol << "\n";
			current_delim.type = "Operator";
			current_delim.lexeme = symbol;
		}
	}
}

bool TryDigit(const string &lexeme, unsigned long delimpos)
{
	int i = 0;
	while ( i < delimpos)
	{
		if (!isdigit(lexeme[i]))
		{
			cout << "Parcing error!: " << lexeme << "\n";
			current_token.type = "error";
			return false;
		}
		++i;
	}
	cout << "Number: " << lexeme << "\n";
	current_token.type = "Number";
	current_token.lexeme = lexeme;
	return true;
}

bool TryIdentifier(const string &lexeme, unsigned long delimpos, unsigned char next)
{
	if (isalpha(lexeme[0])|| (lexeme[0] == '_'))
	{
		for (int i = 1; i < delimpos; i++)
		{
			if (isalnum(lexeme[i]) || (lexeme[i] == '_')){}
			else return false;			
		}
		if (next == '(')
		{
			cout << "Function: " << lexeme << '\n';
			current_token.type = "Function";
			current_token.lexeme = lexeme;
			//functoken.lexeme = current_token.lexeme;
		}
		else
		if (next == '[')
		{
			cout << "Identifier: " << lexeme << '\n';
			current_token.type = "Identifier";
			current_token.attribute = 'a';
			current_token.lexeme = lexeme;
			//idtoken.attribute = 'a';
			//idtoken.lexeme = current_token.lexeme;
		}
		else
		if (next == '.')
		{
			cout << "Identifier: " << lexeme << '\n';
			current_token.type = "Identifier";
			current_token.attribute = 'n';
			current_token.lexeme = lexeme;
			//idtoken.attribute = 'n';
			//idtoken.lexeme = current_token.lexeme;
		}
		else
		{
			cout << "Identifier: " << lexeme << '\n';
			current_token.type = "Identifier";
			current_token.lexeme = lexeme;
			//idtoken.lexeme = current_token.lexeme;
		}
		return true;
	}
	else
	{
		TryDigit(lexeme, delimpos);
	}
	return false;
}

bool TryComplex(unsigned char delim, unsigned char next)
{
	if (next == '=')
	{
		if (delim == '=' || delim == '<' || delim == '>' || delim == '!')
		{
			cout << "Comparison operator: " << delim << next << '\n';
			current_delim.type = "Comparison operator";
			return true;
		}
		else
		if (delim == '+' || delim == '-')
		{
			cout << "Arithmetical operator: " << delim << next << '\n';
			current_delim.type = "Arithmetical operator";
			return true;
		}
	}
	else
	if (delim == '<' && next == '>')
	{
		cout << "Comparison operator: " << delim << next << '\n';
		current_delim.type = "Comparison operator";
		return true;
	}
	else
	if ((delim == '&' && next == '&') || (delim == '|' && next == '|'))
	{
		cout << "Logic operator: " << delim << next << '\n';
		current_delim.type = "Logic operator";
		return true;
	}
	else
	if ((delim == '<' && next == '<') || (delim == '>' && next == '>'))
	{
		cout << "Shift operator" << delim << next << '\n';
		current_delim.type = "Shift operator";
		return true;
	}
	else
	if ((delim == '+' && next == '+') || (delim == '-' && next == '-'))
	{
		cout << "Arithmetical operator" << delim << next << '\n';
		current_delim.type = "Arithmetical operator";
		return true;
	}
	return false;
}

void TryComment(unsigned long &delimpos, unsigned long &begpos, const string &delim)
{
	if (s[delimpos] == '/')
	{
		if (s[delimpos+1] == '/')
		{
			unsigned long pos = delimpos;
			delimpos = s.find_first_of("\n", delimpos + 1);
			cout << "Comment: " << s.substr(pos, delimpos - pos) << '\n';
			current_token.type = "Comment";
			begpos = delimpos + 1;			
			delimpos = s.find_first_of(delim, delimpos+1);
		}
		else
		if (s[delimpos+1] == '*')
		{
			unsigned long pos = delimpos;
			delimpos = s.find_first_of("*", delimpos + 2);
			while (s[delimpos+1] != '/')
			{
				if (delimpos == string::npos) 
				{
					cout << "Parcing error!: */ missed!\n";
					current_token.type = "error";
					exit(2);
				}	
				delimpos = s.find_first_of("*", delimpos + 1);
			}
			cout << "Comment: " << s.substr(pos, delimpos - pos + 2) << '\n';
			current_token.type = "Comment";
			begpos = delimpos + 2;
			delimpos = s.find_first_of(delim, delimpos + 2);
		}
	}
}

void TryString(unsigned long &delimpos, unsigned long &begpos, const string &delim)
{
	if (s[delimpos] == '"')
	{
		unsigned long pos = delimpos + 1;
		while (s[delimpos = s.find_first_of(delim, delimpos + 1)] != '"')
		{
			if (delimpos == string::npos) 
			{
				cout << "Parcing error!: \" missed!\n";
				current_token.type = "error";
				exit(2);
			}
		}
		cout << "String: " << s.substr(pos, delimpos - pos) << '\n';
		current_token.type = "String";
		begpos = delimpos + 1;
		delimpos = s.find_first_of(delim, delimpos + 1);
	}
}

void Lex()
{		
	const string delim = " ;(),.{}[]\n\t\r+-=/*><~!|&%:\"";
	static unsigned long delimpos = s.find_first_of(delim, 0);
	static unsigned long begpos = 0;
	
	if( delimpos != string::npos )
	{		
		TryComment(delimpos, begpos, delim);   // Определяем коментарии
		
		TryString(delimpos, begpos, delim);	  // Определяем константные строки

		// Определяем составные операторы
		if (TryComplex(s[delimpos], s[delimpos + 1]))
		{
			delimpos = s.find_first_of(delim, delimpos + 1);
			begpos = delimpos + 1;
			delimpos = s.find_first_of(delim, begpos);
		}

		// Всё остальное
		if (delimpos - begpos)
		{
			char next = s[delimpos];
			while (next == ' ') next = s[delimpos + 1];
			GetToken(s.substr(begpos, delimpos - begpos), delimpos - begpos, next); // String - delimiter!
		}
		//else current_token.type = ""; // Временная мера
		
		// Определяем разделители		
		TryDelimiter(s[delimpos]);
		//while (current_delim.type == "Space")
		//{						
		//	++delimpos;
		//	Lex();
		//	cout << "qwer" << s[delimpos + 1];
		//	if (s[delimpos + 1] == ' ' || s[delimpos + 1] == '\n' || s[delimpos + 1] == '\t' || s[delimpos + 1] == '\r')
		//	++delimpos;
		//	//TryDelimiter(s[delimpos]);
		//}
		begpos = delimpos + 1;
		delimpos = s.find_first_of(delim, begpos);				
	}
	else
	{
		finish_parcing = true;
	}
}

// -----------------------------------------------------------------------
// Parser part

short set_get = 0;
short cycle = 0;
short try_catch = 0;
short nscstat = 0;

extern bool IsExpression();
extern bool IsStatement();
extern bool IsSetGet();
extern void AddToIdTab();
extern void AddToFuncTab();
extern Token Find(Token current);
unsigned int type_sum = 0;
unsigned int type_quant = 0;
double type_res = 0.0;

bool IsFactor()
{
	cout << "fact ent\n";
	while (current_delim.type == "Space") Lex();
	if (current_delim.lexeme == "(")
	{
		cout << "(exp) ent\n";
		Lex();
		if (IsExpression() && current_delim.lexeme == ")")
		{
			cout << "(exp) ext\n";
			cout << "fact ext\n";
			Lex();
			return true;
		}
		else
		{
			cout << "Parcing error (expression)\n";cout << "fact ext\n";
			return false;
		}		
	}
	else
	if (current_token.type == "Number" || current_token.type == "Identifier")
	{		
		if (current_token.type == "Identifier")
		{			
			exptoken = Find(current_token);
			if (exptoken.type == "char") type_sum += 1;
			else
			if (exptoken.type == "short") type_sum += 2;
			else
			if (exptoken.type == "int" || exptoken.type == "bool" || exptoken.type == "long" ||
				exptoken.type == "float") type_sum += 4;
			else
			if (exptoken.type == "double") type_sum += 8;
			else type_sum += 10;
			++type_quant;
		}
		if (current_token.attribute == 'a')
		{
			if (current_delim.lexeme == "[")
			{
				Lex();
				while (current_delim.type == "Space") Lex();
				if (current_token.type == "Number" || current_token.type == "Identifier")
				{
					if (current_delim.lexeme == "]") 
					{
						Lex();
					}
					else return false;
				}
				else return false;
			}
		}
		else
		if (current_token.attribute == 'n')
		{			
			string remember_type = current_token.type;
			if (current_delim.lexeme == ".")
			{				
				Lex();
				if (current_token.type == "Identifier" || current_token.type == "Function")
				{
					cout << "It's all right, I know him.\n";
					++visibility;
					if (Find(current_token).type == remember_type)
					{
						--visibility;
						if (IsFactor()) return true;
						else return false;
					}
					else
					{
						--visibility;
						cout << "Unknown identifier " << remember_type << "." << current_token.lexeme << "\n";
						getchar();
						if (IsFactor()) return true;
						else return false;
					}
				}
				else return false;
			}
		}
      	else
		if (current_token.type == "Number" && current_delim.lexeme == ".")
      	{
          Lex();
          if (current_token.type == "Number"){type_sum += 8;++type_quant;}
          else return false;
      	}
		if (current_token.type == "Number") {type_sum += 4;++type_quant;}
		cout << "fact ext (num)\n";
		return true;
	}
	else{cout << "fact ext false\n"; return false;}
}

bool IsHPOperator() // Оператор с высоким приоритетом
{
	cout << "hp ent\n";
	while (current_delim.type == "Space") Lex();
	if (current_delim.lexeme == "&" || current_delim.lexeme == "|" || current_delim.lexeme == "=")
	{
		Lex();
		if (IsFactor())
		{
			if (IsHPOperator())
			{
				cout << "hp ext\n";
				return true;
			}
		}
	}
	else
	if (current_delim.type == "Shift operator")
	{
		Lex();
		if (IsFactor())
		{
			if (IsHPOperator())
			{
				cout << "hp ext\n";
				return true;	
			}
		}
	}
	else
	if (current_delim.lexeme == "*" || current_delim.lexeme == "/")
	{
		Lex();
		if (IsFactor())
		{
			if (IsHPOperator())
			{
				cout << "hp ext\n";
				return true;	
			}
		}
	}
	else
	{
		cout << "hp ext\n";
		return true;
	}
}

bool IsTerm()
{	
	cout << "term ent\n";
	if (IsFactor() && IsHPOperator()){cout << "term ext\n"; return true;}
	else {cout << "term ext\n";return false;}
}

bool IsLPOperator() // Оператор с низким приоритетом
{
	cout << "lp ent\n";
	while (current_delim.type == "Space") Lex();
	if (current_delim.lexeme == "+" || current_delim.lexeme == "-")	
	{
		Lex();		
		if (IsTerm())
		{
			if (IsLPOperator())
			{
				cout << "lp ext\n";
				return true;	
			}
		}
	}
	else
	if (current_delim.type == "Comparison operator" || current_delim.lexeme == "<" || current_delim.lexeme == ">")	
	{
		Lex();		
		if (IsTerm())
		{
			if (IsLPOperator())
			{
				cout << "lp ext\n";
				return true;	
			}
		}
	}
	else
	if (current_delim.type == "Logic operator")	
	{
		Lex();		
		if (IsTerm())
		{
			if (IsLPOperator())
			{
				cout << "lp ext\n";
				return true;	
			}
		}
	}
	else
	{
		cout << "lp ext\n";
		return true;
	}
}

bool IsExpression()
{
	cout << "exp ent\n";
	if (IsTerm() && IsLPOperator()) {cout << "exp ext\n";return true;}
	else {cout << "exp ext\n";return false;}
}

bool IsStatListNR() // Промежуточное представление StatList для устранения левой рекурсии
{
	while (current_delim.type == "Space") Lex();
	if (current_delim.lexeme == ";")
	{
		cout << "semicolon in the statlist(NR)\n";
		Lex();
		if (IsStatement() && IsStatListNR()) return true;
	}
	else 
	if (nscstat > 0)
	{
		cout << "none semicolon in the statlist(NR)\n";
		--nscstat;
		//Lex();
		if (IsStatement() && IsStatListNR()) return true;
	}
	else return true;
}

bool IsStatList()
{
	cout << "statlist ent\n";
	if (IsStatement() && IsStatListNR()) { cout << "statlist ext true\n"; return true;}
	else{ cout << "statlist ext false\n"; return false;}
}

bool IsOptStat() // Набор StatList'ов или пустая строка
{
	cout << "Opt ent\n";
	Lex();
	while (current_delim.type == "Space" && current_token.lexeme != "return") Lex();
	//if (current_delim.lexeme == "}") {cout << "opt ext true\n";return true;}
	if (IsStatList()) {cout << "opt ext by statlist true\n"; return true;}
	else 
	if (current_delim.lexeme == "}") {cout << "opt ext by '}' true\n";return true;}
	else {cout << "opt ext false\n";return false;}
	return true;
}

bool IsBlock()
{
    cout << "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbblock ent\n";
	while (current_delim.type == "Space") Lex();      // BECAREFUL
	if (current_delim.lexeme == "{")
	{
      if (IsOptStat())
      {
			while (current_delim.type == "Space") Lex();
			if (current_delim.lexeme == "}") return true;
			else
			{
				cout << "Parcing error: 'block' statement '}' missing.\n";
				return false;
			}
			return true;
      }
	}
	else return false;
}

bool IsCaseList()
{
	if (IsExpression() || current_token.lexeme == "default")
	{
		//Lex();
		while (current_delim.type == "Space") Lex();
		if (current_delim.lexeme == ":")
       	{
          	cout << "case : ent\n";
       		//Lex();
          	++cycle;
			++visibility;
          	if (IsBlock())
          	{
          		Lex();
             	while (current_delim.type == "Space") Lex();
				if (IsCaseList())
             	{
             		cout << "switch case ext true\n";
					Lex();	
					--visibility;
					return true;
				}
				--visibility;
				return true;
          	}
          	else
          	if (IsOptStat()) 
			{
				if (IsCaseList())
				{
             		cout << "switch case ext true\n";
					Lex();	
					--visibility;
					return true;
				}
				--visibility;
				return true;
			}
          	else return false;
       }
       else return false;
    }
    //else return false;
	cout << "switch case ext false\n";
}

bool IsSetGet()
{
   cout << "setget ent\n";
   if (current_token.lexeme == "get" && current_delim.lexeme == "{")
   {
		++visibility;		
	   cout << "get entttttttttttttttttttttt\n";
				if (IsOptStat())
				{
					//Lex(); // Неизвестно, нужно ли
					cout << "set get checking out\n";
					while (current_delim.type == "Space") Lex();
					if (current_delim.lexeme == "}") 
					{
						--set_get;
						--visibility;
						cout << "set get out\n";
						Lex();
						return true;
					}
					else return false;               
				}
   }
   else
	if (current_token.lexeme == "set" && current_delim.lexeme == "{")
   {
				cout << "set ent\n";
				++visibility;
				if (IsOptStat()) 
				{
					while (current_delim.type == "Space") Lex();
					if (current_delim.lexeme == "}") 
					{						
						while (current_delim.type == "Space") Lex();
						if (current_token.lexeme == "get" && current_delim.lexeme == "{")
						{
							cout << "get ent\n";													
							if (IsOptStat())
							{
								//Lex(); // Неизвестно, нужно ли
                        		cout << "set get checking out\n"; 
								while (current_delim.type == "Space") Lex();
								if (current_delim.lexeme == "}")
								{
									--set_get;
									--visibility;
									cout << "set get out\n"; 
									Lex();
									return true;
								}
                        		else return false;
							}
						}
					}
					else
					{
						cout << "Parcing error: statement set '}' missing.\n";
						return false;
					}
				}
				else return false;
   }
   else return false;
}

bool IsDeclaration()
{			
	cout << "declaration ent\n";
	while (current_delim.type == "Space") Lex();
	if (current_token.type == "Identifier")
	{
      	cout << "Identifier found\n";
		idtoken.lexeme = current_token.lexeme;
		cout << current_token.type << current_token.lexeme << "\n";
      	while (current_delim.type == "Space") Lex(); // Warning!
		if (current_delim.lexeme == "[")
		{
			Lex();
			while (current_delim.type == "Space") Lex();
			if (current_token.type == "Number")
			{
				idtoken.parameters = atoi(current_token.lexeme.c_str());
				if (current_delim.lexeme == "]") 
				{
					idtoken.attribute = 'a';
					Lex();
					while (current_delim.type == "Space") Lex();
					if (current_delim.lexeme == "=")
					{
						Lex();
						while (current_delim.type == "Space") Lex();
						if (current_delim.lexeme == "{")
						{
							while (current_token.type == "Number")
							{
								idtoken.value.push_back(atol(current_token.lexeme.c_str()));
								Lex();
								while (current_delim.type == "Space") Lex();
								if (current_delim.lexeme != ",") break;
								//cout << "Here\n";										
							}
							if (current_delim.lexeme == "}") 
							{
								Lex();
								AddToIdTab();
								while (current_delim.type == "Space") Lex();
							}									
							if (current_delim.lexeme == ";")
							{
								Lex();
								return true;
							}										
							else return false;									
						}
					}
				}
				else return false;
			}
			else return false;
		}
		else
		if (current_delim.lexeme == "=")
		{
         	cout << "= found\n";			
			Lex();
			while (current_delim.type == "Space") Lex();
			if (current_token.type == "Number" || current_token.type == "Identifier")
			{
				if (current_token.type == "Number")
				idtoken.value.push_back(atol(current_token.lexeme.c_str()));
				else 
				{					
					set<Token, tmore>::iterator i = idtokentab.begin();
					for (i; i != idtokentab.end(); i++)
					{
						if ((*i).lexeme == current_token.lexeme && (*i).visibility == visibility)
						{
							idtoken.value = (*i).value;							
							break;
						}
					}
					if (i == idtokentab.end()) 
					{
						cout << "Unknown variable " << current_token.lexeme << "\n";
						getchar();
					}
				}
				AddToIdTab(); // Debug assertion failure!
				cout << "tttttttttttttttttttttttttttttttttttttt " << (*idtoken.value.begin()) << "\n";

				cout << "number here\n";
				if (current_delim.lexeme == ";") {Lex(); return true;}
				else
				if (current_delim.lexeme == ",") 
				{							
					Lex();
					while (current_delim.type == "Space") Lex();
					if (current_token.type == "Identifier")
					{
						if (IsDeclaration()) return true;
						else return false;	
					}
					else return false;							
				}
				else return false;
			}
		}
		else
		if (current_delim.lexeme == ";")
		{
			Lex();
			AddToIdTab();
			return true;	
		}
		else
		if (current_delim.lexeme == ",") 
		{							
			Lex();
			while (current_delim.type == "Space") Lex();
			if (current_token.type == "Identifier")
			{
				if (IsDeclaration()) return true;
				else return false;	
			}
			else return false;							
		}
      if (current_delim.lexeme == "{")
      {
          	cout << "set get in future\n";  
		 	++set_get;
			Lex();
            while (current_delim.type == "Space") Lex();
            if (IsSetGet())
            {
            	cout << "set get out of id\n";
				Lex();
				while (current_delim.type == "Space") Lex();
               if (current_delim.lexeme == "}")
               {
               	cout << "id decl get ext\n";
				   Lex();
                  return true;
               }
            }
            else return false;
      }
		else return false;
	}
	else return false;
	return true;
}

bool IsFuncCall()
{
	cout << "fchere\n";
	functoken.lexeme = current_token.lexeme;
	cout << current_token.type << current_token.lexeme << "\n";
	Lex();
	while (current_delim.type == "Space") Lex();	

	if (current_token.type == "Identifier" || current_token.type == "String")
	{
		cout << "ihere\n";
		++functoken.parameters;
		functoken.param_type.push_back(Find(current_token).type);
		while (current_delim.type == "Space") Lex();			
		if (current_delim.lexeme == "[")			
		{
			Lex();
			while (current_delim.type == "Space") Lex();
			if (current_token.type == "Number")
			{
				if (current_delim.lexeme == "]") 
				{
					Lex();
				}
				else return false;
			}
			else return false;
		}
		else			
		if (current_delim.lexeme == ".") 
		{
			Lex();
			if (current_token.type == "Identifier" || current_token.type == "String" || current_token.type == "Function")
			{
				if (IsFuncCall())
				{
					cout << "fc ext true\n";
					return true;						
				}
				else return false;
			}
			else return false;
		}
		else			
		if (current_delim.lexeme == ")")
		{
			cout << "fc ext true\n";
			Token* temp = &functoken;
			functoken = Find(*temp);
			Lex();
			return true;
		}
		else
		if (current_delim.lexeme == ",") 
		{			
			if (IsFuncCall())
			{
				cout << "fc ext true\n";
				Lex();
				return true;						
			}
			else return false;
		}
		else return false;
	}	
	else
	if (current_delim.lexeme == ")")
	{
		cout << "fc ext true\n";
		Token* temp = &functoken;
		functoken = Find(*temp);
		Lex();	
		return true;		
	}
}

bool IsFuncDecl()
{
	cout << "fhere\n";	
	//functoken.lexeme = current_token.lexeme;
	Lex(); 
	while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();	
	if (current_token.lexeme == "int" || current_token.lexeme == "char" || current_token.lexeme == "bool" ||
		current_token.lexeme == "sbyte" || current_token.lexeme == "short" || current_token.lexeme == "byte" ||
		current_token.lexeme == "uint" || current_token.lexeme == "ushort" || current_token.lexeme == "ulong" ||
		current_token.lexeme == "long" || current_token.lexeme == "float" || current_token.lexeme =="double" ||
      current_token.lexeme == "object" || current_token.lexeme == "EventArgs")
	{			
		cout << "inhere\n";
		Lex();
		++functoken.parameters;
		functoken.param_type.push_back(current_token.lexeme);
		idtoken.type = current_token.lexeme;
		while (current_delim.type == "Space") Lex();
		
		if (current_token.type == "Identifier")
		{
			cout << "ihere\n";
			idtoken.lexeme = current_token.lexeme;
			while (current_delim.type == "Space") Lex();
			if (current_delim.lexeme == "[")			
			{				
				Lex();
				while (current_delim.type == "Space") Lex();
				if (current_token.type == "Number")
				{
					idtoken.parameters = atoi(current_token.lexeme.c_str());
					if (current_delim.lexeme == "]") 
					{
						idtoken.attribute = 'a';
						AddToIdTab();
						Lex();							
					}
					else return false;
				}
				else return false;
			}
			else
			if (current_delim.lexeme == "=")
			{
				cout << "=here\n";
				Lex(); 
				while (current_delim.type == "Space") Lex();
				if (current_token.type == "Number")
				{
					cout << "here\n";
					if (current_delim.lexeme == ")") 
					{
						AddToIdTab();						
						Lex();
						return true;
					}
					else
					if (current_delim.lexeme == ",")
					{							
						if (IsFuncDecl()) return true;
						else return false;							
					}
					else return false;
				}
			}
			else
			if (current_delim.lexeme == ")") 
			{
				AddToIdTab();				
				Lex();
				cout << "ihere ) ext true\n";
				return true;
			}
			//else
			if (current_delim.lexeme == ",")
			{
				
				if (IsFuncDecl()) return true;						
				else return false;
			}
			else return false;
		}
		else return false;
		return true;
	}
	else
	if (current_delim.lexeme == ")") 
	{		
		Lex();
		return true;
	}
}

bool IsDecl()
{
	cout << "decl ent\n";
	cout << "1current_token.type " << current_token.type << "\ncurrent_delim.lexeme " << current_delim.lexeme << '\n';

	if (current_token.lexeme == "int" || current_token.lexeme == "char" || current_token.lexeme == "bool" ||
		current_token.lexeme == "sbyte" || current_token.lexeme == "short" || current_token.lexeme == "byte" ||
		current_token.lexeme == "uint" || current_token.lexeme == "ushort" || current_token.lexeme == "ulong" ||
		current_token.lexeme == "long" || current_token.lexeme == "float" || current_token.lexeme =="double")
	{
		functoken.type = current_token.lexeme;
		idtoken.type = current_token.lexeme;
		cout << "2current_token.type " << current_token.type << "\ncurrent_delim.lexeme " << current_delim.lexeme << '\n';

		Lex();
		while (current_delim.type == "Space") Lex();
		cout << "3current_token.type " << current_token.type << "\ncurrent_delim.lexeme " << current_delim.lexeme << '\n';
		if (current_token.type == "Function")
		{
			functoken.lexeme = current_token.lexeme;
			++visibility;
			func_belonging = current_token.lexeme;
			if (IsFuncDecl())
			{
				if (IsBlock())
				{
					++set_get;
					--visibility;
					func_belonging = "";
					AddToFuncTab();
					return true;
				}
				else
				if (current_delim.lexeme == ";")
				{
					--visibility;
					func_belonging = "";
					AddToFuncTab();
					return true;
				}
				else return false;
			}
		}
		else
		if (current_token.type == "Identifier")
		{
			cout << "id ent\n";
			idtoken.lexeme = current_token.lexeme;
			idtoken.func_property = func_belonging;
			if (IsDeclaration()) return true;
			else return false;
		}
		else return false;
	}
	else
	if (current_token.lexeme == "void")
	{
		/*if (current_delim.lexeme == "*")
		{
			Lex();
			while (current_delim.type == "Space") Lex();
		}*/
      	cout << "void obtained\n";
		functoken.type = "void";
		Lex();
		while (current_delim.type == "Space") Lex();
		if (current_token.type == "Function")
		{
         	cout << "void function obtained\n";
			functoken.lexeme = current_token.lexeme;
			++visibility;
			func_belonging = current_token.lexeme;
			if (IsFuncDecl())
			{
				if (IsBlock())
				{
					++set_get;
					--visibility;
					func_belonging = "";
					AddToFuncTab();
					cout << "block in void ext true\n";
					return true;
				}
				else
				if (current_delim.lexeme == ";")
				{
					--visibility;
					func_belonging = "";
					AddToFuncTab();
					return true;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else
	if (current_token.lexeme == "class" || current_token.lexeme == "structure" || current_token.lexeme == "namespace")
	{
		cout << "class ent\n";
		idtoken.type = current_token.lexeme;
		Lex();
		while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
		int flag = 0;
		if (current_delim.lexeme == ":")
		{
			Lex();
			class_belonging = idtoken.lexeme = current_token.lexeme;
			flag = 1;
			while (current_delim.type == "Space") Lex();
		}
		if (current_token.type == "Identifier" && current_delim.lexeme == "{")
		{
			cout << "class abs ent\n";
			if (!flag) {class_belonging = idtoken.lexeme = current_token.lexeme;}			
			AddToIdTab();
			++visibility;
			Lex();
			while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
			if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
				current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
				current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
				current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
				current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")
			{
				Lex();
				if (IsDecl())
				{
				while (IsDecl())
				{
					cout << "class declaration while iteration (I think)\n";
					Lex();
					while (current_token.type != "Keyword" && current_delim.type == "Space") {cout << "s";Lex();}
					if (current_delim.lexeme == "}")
					{
						Lex();
						--visibility;
						class_belonging = "";
						cout << "class declaration probably ext (hurray!)\n";
						return true;
					}
					while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
					if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
						current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
						current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
						current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
						current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")
					{
						Lex();
						while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
					}
				}
					if (current_delim.lexeme == "}")
					{
						Lex();
						cout << "class decl prob ext222222222\n";
						--visibility;
						class_belonging = "";
						if (current_delim.lexeme == ";") return true;
						else return false;
					}
				}
			}
			else
			{
				if (IsDecl())
				{
				while (IsDecl())
				{
					Lex();
					if (current_delim.lexeme == "}")
					{
						Lex();
						--visibility;
						class_belonging = "";
						if (current_delim.lexeme == ";") return true;
						else return false;
					}
					while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
               		if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
						current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
						current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
						current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
						current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")
					{
						Lex();
                  		while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
               		}
				}
					if (current_delim.lexeme == "}")
					{
						Lex();
						--visibility;
						class_belonging = "";
						if (current_delim.lexeme == ";") return true;
						else return false;
					}
				}
			}
			while (current_delim.type == "Space") Lex();
			if (current_delim.lexeme == "}")
			{
				Lex();
				--visibility;
				class_belonging = "";
				if (current_delim.lexeme == ";") return true;
				else return false;
			}
			else return false;
		}
	}
	else
	if (current_token.type == "Function")
	{
		/*Lex();*/
		functoken.lexeme = current_token.lexeme;
		functoken.type = "Constructor";
		cout << "function intended\n";
		while (current_delim.type == "Space") Lex();
		if (current_delim.lexeme == "(")
		{
			while (current_delim.type == "Space") Lex();
			++visibility;
			if (IsFuncDecl()) 
			{
				if (IsBlock())
				{
					--visibility;
					AddToFuncTab();
					return true;
				}
				else
				if (current_delim.lexeme == ";")
				{
					--visibility;
					AddToFuncTab();
					return true;
				}
				else return false;
			}
			else return false;
		}
		cout << "function intendion failed\n";
		//Lex();
		//return false;
	}
	else return false;
	cout << "decl failed\n";
	//return false;
	return true;
}

bool IsStatement()
{
	cout << "sat ent \n";
   	while (current_delim.type == "Space" && current_token.type != "Keyword") {cout << "s";Lex();}
	cout << "current_token.type " << current_token.type << "\ncurrent_delim.lexeme " << current_delim.lexeme << '\n';

	if (current_token.type == "Identifier")
	{
		idtoken = Find(current_token);
		if (idtoken.type == "class" || idtoken.type == "structure" || idtoken.type == "namespace")
		{
			Lex();
			if (current_token.type == "Identifier") 
			{
				idtoken.type = idtoken.lexeme;
				idtoken.lexeme = current_token.lexeme;
				idtoken.attribute = 'n';
				AddToIdTab();
			}
		}
		while (current_delim.type == "Space") Lex();
		if (current_token.attribute == 'a')
		{
			if (current_delim.lexeme == "[")
			{
				Lex();
				while (current_delim.type == "Space") Lex();
				if (current_token.type == "Number" || current_token.type == "Identifier")
				{
					if (current_delim.lexeme == "]") 
					{
						Lex();
					}
					else return false;
				}
				else return false;
			}
		}
		else
		if (current_token.attribute == 'n')
		{
			string remember_type = current_token.type;
			if (current_delim.lexeme == ".")
			{
				Lex();
				if (current_token.type == "Identifier" || current_token.type == "Function")
				{
					++visibility;
					if (Find(current_token).type == remember_type)
					{
						--visibility;
						if (IsStatement()) return true;
						else return false;
					}
					else
					{
						--visibility;
						cout << "Unknown identifier " << remember_type << "." << current_token.lexeme << "\n";
						cout << "What's up, duck?\n";
						getchar();
						if (IsStatement()) return true;
						else return false;
					}
				}
				else return false;
			}
		}
		if (current_delim.lexeme == "=")
		{
			Lex();
			cout << "Lex " << current_token.type << '\n';
			cout << "Del " << current_delim.lexeme << '\n';
			type_quant = 0;
			type_sum = 0;
			if (idtoken.type == "char") type_res = 1.0;
			else
			if (idtoken.type == "short") type_res = 2.0;
			else
			if (idtoken.type == "int" || idtoken.type == "bool" || idtoken.type == "long" ||
				idtoken.type == "float") type_res = 4.0;
			else
			if (idtoken.type == "double") type_res = 8.0;
			if (IsExpression() && current_delim.lexeme == ";") 
			{
				cout << "sat ext \n";
				if (type_res < ((double)type_sum/((double)type_quant))) cout << "Warning: type conversion has failed\n";
				else cout << "Everything's okey\n";
				return true;
			}
		}		
		else
		if (current_delim.lexeme == ".")
		{
			cout << "I don't know, what is it?" << current_token.lexeme <<"\n";
			Lex();			
			if (current_token.type == "Identifier" || current_token.type == "Function")
			{
				if (IsStatement()) return true;
				else return false;
			}
		}
		else return false;
	}
	else
	if (current_token.type == "Function" && current_delim.lexeme == "(")
	{
		if (IsFuncCall()) {cout << "sat ext fc true\n"; return true;}
		else return false;
	}
	else
	if (current_token.type == "Keyword")	
	{
		if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
			current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
			current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
			current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
			current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")		
		{
			Lex();
			if (IsDecl()){return true;}
			else return false;			
		}
		else
		if (current_token.lexeme == "class" || current_token.lexeme == "structure" || current_token.lexeme == "namespace")
		{
			cout << "class ent\n";
			idtoken.type = current_token.lexeme;
			Lex();
			while (current_token.type != "Keyword" && current_delim.type == "Space") {cout << "s";Lex();}
			int flag = 0;
			if (current_delim.lexeme == ":") 
			{
				Lex();
				class_belonging = idtoken.lexeme = current_token.lexeme;
				flag = 1;
				if (current_token.type != "Identifier") {return false; }
			}
			if (current_token.type == "Identifier" && current_delim.lexeme == "{")
			{
				cout << "class abs ent\n";
				if (!flag) {class_belonging = idtoken.lexeme = current_token.lexeme;}	
				AddToIdTab();
				++visibility;
				Lex();
				while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
				if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
					current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
					current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
					current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
					current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")		
				{
					Lex();
					while (IsDecl())
					{
						Lex();
						if (current_delim.lexeme == "}")
						{
							Lex();
							--visibility;
							class_belonging = "";
							cout << "class decl prob ext\n";
							return true;
						}
						while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
                  		if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
							current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
							current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
							current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
							current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")
						{
							Lex();
                  			while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
               			}
					}
				}
				else
				{
               		while (IsDecl())
					{
                  		Lex();
						if (current_delim.lexeme == "}")
						{
							Lex();
							--visibility;
							class_belonging = "";
							cout << "class decl prob ext333333333\n";							
							return true;
						}
						while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
                  		if (current_token.lexeme == "public" || current_token.lexeme == "private" || current_token.lexeme == "protected" ||
							current_token.lexeme == "internal" || current_token.lexeme == "const" || current_token.lexeme == "static" ||
							current_token.lexeme == "virtual" || current_token.lexeme == "override" || current_token.lexeme == "extern" ||
							current_token.lexeme == "signed" || current_token.lexeme == "unsigned" || current_token.lexeme =="readonly" ||
							current_token.lexeme == "volatile" || current_token.lexeme == "abstract" || current_token.lexeme =="sealed")
						{
							Lex();
                  			while (current_token.type != "Keyword" && current_delim.type == "Space") Lex();
               			}
					}
				}
				while (current_delim.type == "Space") Lex();
				if (current_delim.lexeme == "}")
				{
					Lex(); 
					--visibility;
					class_belonging = "";
					cout << "class decl prob ext\n";
					return true;
				}
				else return false;
			}
		}		
		else
		if (current_token.lexeme == "continue" && cycle != 0)
		{
			if (current_delim.lexeme == ";")
         	{
            	Lex();
				return true;
         	}
		}
      	else		
		if (current_token.lexeme == "break" && cycle != 0)
		{
			if (current_delim.lexeme == ";")
         	{
				--cycle;
				cout << "break out\n";
            	Lex();
				while (current_delim.type == "Space") Lex();
				cout << "curtoklex " << current_token.lexeme << "\n";
         		cout << "break out ext\n";
				return true;
         	}
		}
		else      
		if (current_token.lexeme == "return" && set_get != 0)
		{
	         cout << "return found\n";
	      	 Lex();
	         while (current_delim.type == "Space") Lex();
	         if (IsExpression())
	         {
	            while (current_delim.type == "Space") Lex();
				if (current_delim.lexeme == ";")
	         	{
					--set_get;
	            	Lex();
	         		cout << "return out\n";
					return true;
	         	}
	         }
	         else return false;
		}
      	else
		if (current_token.lexeme == "throw" && try_catch != 0) 
		{						
			cout << "erbgierbei\n";
			Lex();
			return true;
		}
		else
		if (current_token.lexeme == "using") 
		{						
			while (current_delim.type == "Space") {cout << "s";Lex();}
			if (current_token.type == "Identifier") 
			{
				while (current_delim.lexeme == ".")
				{								
					if (current_token.type != "Identifier") break;	
					Lex();
				}
				if (current_delim.lexeme == ";")
				{
					Lex();
					return true;
				}
			}
		}
		else
		if (current_token.lexeme == "int" || current_token.lexeme == "char" || current_token.lexeme == "bool" ||
		current_token.lexeme == "sbyte" || current_token.lexeme == "short" || current_token.lexeme == "byte" ||
		current_token.lexeme == "uint" || current_token.lexeme == "ushort" || current_token.lexeme == "ulong" ||
		current_token.lexeme == "long" || current_token.lexeme == "float" || current_token.lexeme =="double" ||
		current_token.lexeme == "void" || current_token.lexeme == "class" || current_token.lexeme =="structure" ||
		current_token.lexeme == "namespace")
		{
			if (IsDecl()) {return true;}
		}
		else
		{
			while (current_delim.type == "Space") {cout << "s";Lex();}
			cout << "in key else ent\n"; 
			if (current_token.lexeme == "try" && current_delim.lexeme == "{")
			{
				cout << "try ent\n";
				++try_catch;
				++visibility;
				while (current_delim.type == "Space") Lex();
				if (IsOptStat()) 
				{										
					if (current_delim.lexeme == "}") 
					{						
						--visibility;
						Lex();
						while (current_delim.type == "Space") Lex();
						if (current_token.lexeme == "catch" && current_delim.lexeme == "{")
						{
							cout << "catch ent\n";
							++visibility;
							if (IsOptStat())
							{
								cout << "opt in catch cont\n";
								//Lex(); // Неизвестно, нужно ли
								while (current_delim.type == "Space") Lex();
								if (current_delim.lexeme == "}") 
								{
									--try_catch;
									++nscstat;
									--visibility;
									Lex();
									return true;
								}
							}
						}
					}
					else
					{
						cout << "Parcing error: statement try '}' missing.\n";
						return false;
					}
				}
				else return false;
			}
			else
			if (current_token.lexeme == "if" && current_delim.lexeme == "(")
			{
				cout << "if ent\n";
				++visibility;
				Lex();
				while (current_delim.type == "Space") Lex();
				if (IsExpression() && current_delim.lexeme == ")")
				{
					cout << "if () ext\n";
					Lex();
					while (current_delim.type == "Space") Lex();
					if (current_delim.lexeme == "{" && IsOptStat())
					{
						//Lex(); // Неизвестно, нужно ли
						while (current_delim.type == "Space") Lex();
						if (current_delim.lexeme == "}") 
						{
							++nscstat;
							--visibility;
							Lex();
							cout << "if ext \n";
							return true;
						}
						else
						{
							cout << "Parcing error: 'if' statement '}' missing.\n";
							return false;
						}
					}
					else
					if (IsStatement()) return true;
					else
					{
						cout << "Parcing error: if IsOptStat\n";
						return false;
					}
				}
				else
				{
					cout << "Parcing error: 'if' statement ')' missing.\n";
					return false;
				}
			}
			else
			if (current_token.lexeme == "while" && current_delim.lexeme == "(")
			{
				++cycle;
				++visibility;
				Lex();
				while (current_delim.type == "Space") Lex();
				if (IsExpression() && current_delim.lexeme == ")")
				{
					Lex();
					while (current_delim.type == "Space") Lex();
					if (current_delim.lexeme == "{" && IsOptStat())
					{
						//Lex(); // Неизвестно, нужно ли
						while (current_delim.type == "Space") Lex();
						if (current_delim.lexeme == "}")
						{
							--cycle;
							++nscstat;
							--visibility;
							Lex();
							cout << "sat ext \n";
							return true;
						}
						else
						{
							cout << "Parcing error: 'while' statement '}' missing.\n";
							return false;
						}
					}
					else
					if (IsStatement()) return true;
					else
					{
						cout << "Parcing error: while IsOptStat\n";
						return false;
					}
				}
				else
				{
					cout << "Parcing error: statement ')' missing.\n";
					return false;
				}
			}
			else
			if (current_token.lexeme == "for" && current_delim.lexeme == "(")
			{
				--cycle;
				++visibility;
				Lex();
				while (current_delim.type == "Space") Lex();
				if (IsExpression() && current_delim.lexeme == ";")
				{
					Lex();
					while (current_delim.type == "Space") Lex();
					if (IsExpression() && current_delim.lexeme == ";")
					{
						Lex();
						while (current_delim.type == "Space") Lex();
						if (IsExpression() && current_delim.lexeme == ")")
						{
							Lex();
							while (current_delim.type == "Space") Lex();
							if (current_delim.lexeme == "{" && IsOptStat())
							{
								//Lex(); // Неизвестно, нужно ли
								while (current_delim.type == "Space") Lex();
								if (current_delim.lexeme == "}") 
								{
									--cycle;
									++nscstat;
									--visibility;
									Lex();
									cout << "sat for ext \n";
									return true;
								}
								else
								{
									cout << "Parcing error: 'for' statement '}' missing.\n";
									return false;
								}
							}
							else
							if (IsStatement()) return true;
							else
							{
								cout << "Parcing error: for IsOptStat\n";
								return false;
							}
						}
					}
				}
			}
         else
         if (current_token.lexeme == "switch" && current_delim.lexeme == "(")
         {
            cout << "switch ent\n";
            if (IsExpression())
            {
               	cout << "switch exp ent\n";
               	//Lex();
                  while (current_delim.type == "Space") Lex();
                  if (current_delim.lexeme == "{")
                  {
                  	 cout << "case vdf ent\n";					 
                     Lex();
                  	 while (current_token.lexeme != "case" && current_delim.type == "Space") Lex();
                     while (current_token.lexeme == "case" || current_token.lexeme == "default")
                     {
                     	cout << "case ent\n";
                        Lex();
                  		while (current_delim.type == "Space") Lex();
                        if (IsExpression() || current_token.lexeme == "default")
            			{
                        	//Lex();
                  			while (current_delim.type == "Space") Lex();
                  			if (current_delim.lexeme == ":")
                           	{
                              	cout << "case : ent\n";
                           		//Lex();
                              	++cycle;
								++visibility;
                              	if (IsBlock())
                              	{
                              		Lex();
                                 	while (current_delim.type == "Space") Lex();
									if (IsCaseList())
             						{
					             		cout << "switch case ext\n";
										--visibility;
										Lex();					
										return true;
									}
									--visibility;
									return true;
                              	}
                              	else
                              	if (IsOptStat()) 
								{
									if (IsCaseList())
             						{
					             		cout << "switch case ext\n";
										Lex();	
										--visibility;
										return true;
									}
									--visibility;
									return true;
								}
                              	else return false;
                           }
                           else return false;
                        }
                        else return false;
                     }
                  }
                  else return false;
            }
            else return false;
         }
		}
	}
	else
	{
		while (current_delim.type == "Space") Lex();
		if (current_delim.lexeme == "{" && IsOptStat())
		{
			//Lex(); // Неизвестно, нужно ли
			++visibility;
			while (current_delim.type == "Space") Lex();
			if (current_delim.lexeme == "}") 
			{
				Lex();
				++nscstat;
				--visibility;
				cout << "sat ext \n";
				return true;
			}
			else
			{
				cout << "Parcing error: 'anything else' statement '}' missing.\n";
				return false;
			}
		}
		else {cout << "sat ext \n";return false;}
	}
	//else return false;
}

void Continue_Parcing()
{
	if (finish_parcing != true)
	{
		cout << "Lex " << current_token.type << '\n';
		cout << "Del " << current_delim.lexeme << '\n';
		if (current_token.type == "Keyword")
		{}
		else
		if (current_token.type == "Comment" || current_delim.type == "Space")
		{
			cout << "Next\n";
			Lex();
			Continue_Parcing();
			return;
		}
		if (IsStatement())
		{
			cout << "True!\n";
		}
		else
		{
			cout << "False!\n";
			//while (current_delim.lexeme == ";") {Lex();}
			return;
		}
		Lex();
		Continue_Parcing();
	}
}

void Start_Parcing()
{
	Lex();
	Continue_Parcing();
}

// -----------------------------------------------------------------------
// Semantic part

void AddToIdTab()
{
	Token* lexeme = new Token;
	*lexeme = idtoken;	
	(*lexeme).visibility = visibility;
	(*lexeme).class_property = class_belonging;
	(*lexeme).func_property = func_belonging;
	
	set<Token, tmore>::iterator i = idtokentab.begin();
	for (i; i != idtokentab.end(); i++)
	{
			if ((*i).lexeme == idtoken.lexeme && (*i).visibility == visibility && (*lexeme).func_property == idtoken.func_property)
			{
				cout << idtoken.lexeme << " already exists!\n";	
				break;
			}
	}
	if (i != idtokentab.end()) idtokentab.erase(i);
	
	idtokentab.insert(*lexeme);
	cout << "Identifier " << (*lexeme).lexeme << " added\n";
}

void AddToFuncTab()
{
	Token* lexeme = new Token;
	*lexeme = functoken;
	(*lexeme).visibility = visibility;
	(*lexeme).class_property = class_belonging;

	set<Token, tmore>::iterator i = functokentab.begin();
	for (i; i != functokentab.end(); i++)
	{
			if ((*i).lexeme == functoken.lexeme && (*i).parameters == functoken.parameters 
				&& (*i).param_type == functoken.param_type)
			{
				cout << functoken.lexeme << " already exists!\n";
				break;
			}
	}
	if (i != functokentab.end()) functokentab.erase(i);
	
	functokentab.insert(*lexeme);
	cout << "Function " << (*lexeme).lexeme << " added\n";
}

Token Find(Token current)
{
	if (current.type == "Identifier")
	{
		for (set<Token, tmore>::iterator i = idtokentab.begin(); i != idtokentab.end(); i++)
		{
			if ((*i).lexeme == current.lexeme && (*i).visibility <= visibility)
			{
				cout << "Found! " << current.lexeme << "\n";
				return (*i);
			}
		}
		cout << "Unknown identifier " << current.lexeme << "\n";
		getchar();
	}
	else
	{
		for (set<Token, tmore>::iterator i = functokentab.begin(); i != functokentab.end(); i++)
		{
			if ((*i).lexeme == current.lexeme && (*i).parameters == current.parameters 
				&& (*i).param_type == current.param_type)
			{
				cout << "Found! " << current.lexeme << "\n";
				return (*i);
			}
		}
		cout << "Unknown function " << current.lexeme << "\n";
		getchar();
	}
	return current;
}

// -----------------------------------------------------------------------
// Main procedure

int main(int argc, char* argv[])
{
	cout << "Exit this programm immediately! It can harm your health!\n";
	cout << "I'm not kidding! Exit it NOW!\n";
	cout << "OK, I have warned you.\n\n";
	if (f1 = fopen("3.cs", "rt"))
	{
		unsigned char *a;
		unsigned long size = 0;
		size = filelength(fileno(f1));
		s.resize(size);
		fread(&s[0], 1, size, f1);

		cout << s << "\n------------------------------------------------------------------\n";

		Start_Parcing();

		size = 0;
		s.resize(size);
		fclose(f1);
	}
	else
	{
		cout << "File does not exist.\n";
	}
	cout << "\n\n------------------------------------------------------------------\n";
	cout << "Programm has finished its job.\nAre you ready to exit? Then press ENTER, please.\n";
	getchar();
	cout << "Of course, I am ready. What a silly question.\n";
	return 0;
}
