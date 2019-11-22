//
//  Parser.hpp
//  BetterParser
//
//  Created by Assad Yousuf on 11/20/19.
//  Copyright © 2019 Assad Yousuf. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <string>
#include "lexer.h"
#include <iostream>
#include <map>
#include "compiler.h"
std::vector<ValueNode*> mem;
int next_available=0;

using namespace std;
class Parser{
private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
    
   
    void parse_varsection();
    void parse_idlist();
    StatementNode*  parse_body();
    StatementNode* parse_stmtlist();
    StatementNode* parse_stmt();
    AssignmentStatement* parse_assignstmt();
    void parse_expr(AssignmentStatement* p);
    ValueNode* parse_primary();
    ArithmeticOperatorType parse_operation();
    PrintStatement* parse_printstmt();
    IfStatement* parse_whilestmt();
    IfStatement* parse_ifstmt(IfStatement *i);
    void parse_condition(IfStatement *i);
    ConditionalOperatorType  parse_relatop();
    void parse_switchstmt();
    void parse_forstmt();
    void parse_caselist();
    void parse_case();
    void parse_defualtcase();
public:
    struct StatementNode* parse_generate_intermediate_representation();
    StatementNode* parse_program();
    
};


void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    lexer.GetToken().Print();
    exit(1);
}

Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}

ValueNode* getLocation(std::string string){
    for(int i=0; i<mem.size(); i++){
        if(string.compare(mem[i]->name) == 0){
            return mem[i];
        }
    }
    return nullptr;
    
}

void setnop( StatementNode *no_op){
    no_op->type=NOOP_STMT;
    no_op->assign_stmt=nullptr;
    no_op->goto_stmt=nullptr;
    no_op->if_stmt=nullptr;
    no_op->next=nullptr;
    no_op->print_stmt=nullptr;
}

void append(StatementNode  *no_op, IfStatement *i){
    StatementNode *temp=i->true_branch;
    while(i->true_branch->next!=nullptr){
        i->true_branch=i->true_branch->next;
    }
    i->true_branch->next=no_op;
    i->true_branch=temp;
    
}

void appendWhile(StatementNode  *ifstmt, IfStatement *ifnode){
    StatementNode *temp=ifnode->true_branch;
    while(ifnode->true_branch->next!=nullptr){
        ifnode->true_branch=ifnode->true_branch->next;
    }
    StatementNode *gostate=new StatementNode();
    gostate->type=GOTO_STMT;
    GotoStatement *go=new GotoStatement();
    gostate->goto_stmt=go;
    go->target=ifstmt;
    
    /*maitnence stuff..
    gostate->assign_stmt=nullptr;
    gostate->if_stmt=nullptr;
    gostate->print_stmt=nullptr;
    gostate->next=nullptr;
    */
    
    
    //appending gostmt to end of ifnode
    ifnode->true_branch->next=gostate;
    ifnode->true_branch=temp;
}
#endif /* Parser_hpp */
