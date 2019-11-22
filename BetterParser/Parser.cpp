//
//  Parser.cpp
//  BetterParser
//
//  Created by Assad Yousuf on 11/20/19.
//  Copyright © 2019 Assad Yousuf. All rights reserved.
//

#include "Parser.h"


void Parser::parse_defualtcase(){
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}

void Parser::parse_case(){
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
}

void Parser::parse_caselist(){
    parse_case();
    Token danil=peek();
    if(danil.token_type==CASE){
        parse_caselist();
    }
}

void Parser::parse_forstmt(){
    expect(FOR);
    expect(LPAREN);
    parse_assignstmt();
    //parse_condition();
    expect(SEMICOLON);
    parse_assignstmt();
    expect(RPAREN);
    parse_body();
}

void Parser::parse_switchstmt(){
    expect(SWITCH);
    expect(ID);
    expect(LBRACE);
    parse_caselist();
    Token s=peek();
    if(s.token_type==DEFAULT){
        parse_defualtcase();
    }
    expect(RBRACE);
}

ConditionalOperatorType Parser::parse_relatop(){
    Token t=lexer.GetToken();;
    if(t.token_type==GREATER){
        return CONDITION_GREATER;
    }
    else if  ( t.token_type==LESS){
        return CONDITION_LESS;
    }
    else if (  t.token_type==NOTEQUAL) {
        return CONDITION_NOTEQUAL;
    }else {
        syntax_error();
        return CONDITION_LESS;
    }
}

void Parser::parse_condition(IfStatement *i){
   i->condition_operand1=parse_primary();
   i->condition_op= parse_relatop();
   i->condition_operand2= parse_primary();
}

IfStatement* Parser::parse_ifstmt(IfStatement *i){
    expect(IF);
    parse_condition(i);
   i->true_branch=parse_body();
    StatementNode *no_op=new StatementNode();
    setnop(no_op);
    append(no_op,i);
    i->false_branch=no_op;
    return i;
}

IfStatement* Parser::parse_whilestmt(){
    expect(WHILE);
    IfStatement *w=new IfStatement();
    parse_condition(w);
    w->true_branch=parse_body();
    return w;
    
}

PrintStatement* Parser::parse_printstmt(){
    PrintStatement* print=new PrintStatement();
    expect(PRINT);
    Token symbolToPrint=expect(ID);
    ValueNode* symbol=new ValueNode();
    //symbol->name=symbolToPrint.lexeme;
    //symbol->value=symbol_Table[symbolToPrint.lexeme];
    symbol=getLocation(symbolToPrint.lexeme);
    print->id=symbol;
    expect(SEMICOLON);
    return print;
}

 ArithmeticOperatorType Parser::parse_operation(){
    Token op=peek();
     if(op.token_type==PLUS){
          op=lexer.GetToken();
         return OPERATOR_PLUS;
     }
     else if(op.token_type==MINUS){
          op=lexer.GetToken();
         return OPERATOR_MINUS;
     }
     else if (op.token_type==MULT){
          op=lexer.GetToken();
         return OPERATOR_MULT;
     }
    else if(op.token_type==DIV){
         op=lexer.GetToken();
        return OPERATOR_DIV;
    } else {
        syntax_error();
        return OPERATOR_NONE;
    }
}

ValueNode* Parser::parse_primary(){
    ValueNode *primary=new ValueNode();
    Token IDorNUM=peek();
    /*if(IDorNUM.token_type==ID || IDorNUM.token_type==NUM){
        IDorNUM=lexer.GetToken();
        
    } else {
        syntax_error();
    }*/
    
    if(IDorNUM.token_type==ID){
        /*primary->name=IDorNUM.lexeme;
        primary->value=getLocation(IDorNUM.lexeme);*/
        primary=getLocation(IDorNUM.lexeme);
    }
    else if(IDorNUM.token_type==NUM){
        primary->name=IDorNUM.lexeme;
        primary->value=atoi(IDorNUM.lexeme.c_str());
    }
    else {
        syntax_error();
    }
    lexer.GetToken();
    return primary;
}

void Parser::parse_expr(AssignmentStatement* p){
    p->operand1=parse_primary();
    p->op=parse_operation();
    p->operand2=parse_primary();
}

AssignmentStatement* Parser::parse_assignstmt(){
    AssignmentStatement* assignstmt=new AssignmentStatement();
    Token LHS=expect(ID);
    
    
    ValueNode *left_handside=getLocation(LHS.lexeme);
    assignstmt->left_hand_side=left_handside;
    
    
    expect(EQUAL);
    Token s1=lexer.GetToken();
    Token s2=lexer.GetToken();
    lexer.UngetToken(s2);
    lexer.UngetToken(s1);
    
    if( (s1.token_type==ID || s1.token_type==NUM) && (s2.token_type==PLUS  || s2.token_type==MULT || s2.token_type==MINUS || s2.token_type==DIV) ){
        parse_expr(assignstmt);
        expect(SEMICOLON);
        return assignstmt;
    } else {
       assignstmt->operand1= parse_primary();
        assignstmt->operand2=nullptr;
        assignstmt->op=OPERATOR_NONE;
        expect(SEMICOLON);
        return assignstmt;
    }
}

StatementNode* Parser::parse_stmt(){
    StatementNode *st=new StatementNode();
    Token stmt=peek();
    if(stmt.token_type==ID){
        st->type=ASSIGN_STMT;
        st->assign_stmt=parse_assignstmt();
    }else if(stmt.token_type==PRINT){
        st->type=PRINT_STMT;
        st->print_stmt=parse_printstmt();
    }else if(stmt.token_type==WHILE){
        st->type=IF_STMT;
        IfStatement* w=parse_whilestmt();
        StatementNode *noo_pstmt=new StatementNode();
        setnop(noo_pstmt);
        w->false_branch=noo_pstmt;
        st->if_stmt=w;
        st->next=w->false_branch;
        appendWhile(st, w);
        
        
    }else if(stmt.token_type==IF){
        st->type=IF_STMT;
        IfStatement *i=new IfStatement();
        st->if_stmt=parse_ifstmt(i);
        st->next=i->false_branch;
    }else if(stmt.token_type==SWITCH){
        parse_switchstmt();
    }else if(stmt.token_type==FOR){
        parse_forstmt();
    }else {
        syntax_error();
    }
    return st;
}

StatementNode* Parser::parse_stmtlist(){
    StatementNode* st;
    StatementNode* st1;
    
    st=parse_stmt();
    Token w=peek();
    if(w.token_type == ID || w.token_type==PRINT || w.token_type== WHILE || w.token_type==IF || w.token_type== SWITCH || w.token_type == FOR){
        if(st->type==IF_STMT){
            st1= parse_stmtlist();
            st->next->next=st1;
        }
        else {
       st1= parse_stmtlist();
        st->next=st1;
        }
        
        return st;
    } else {
        return st;
    }
}

StatementNode* Parser::parse_body(){
    StatementNode *st1;
    expect(LBRACE);
    st1=parse_stmtlist();
    expect(RBRACE);
    return st1;
}

void Parser::parse_idlist(){
    Token Id=expect(ID);
    if(getLocation(Id.lexeme) == nullptr){
        ValueNode *newLocation=new ValueNode();
        newLocation->name=Id.lexeme;
        newLocation->value=0;
        mem.push_back(newLocation);
        //next_available++;
    }
    Token w=peek();
    if(w.token_type==COMMA){
        lexer.GetToken();
        parse_idlist();
    } else if(w.token_type==SEMICOLON){
        return;
    } else {
        syntax_error();
    }
}

void Parser::parse_varsection(){
    parse_idlist();
    expect(SEMICOLON);
}

StatementNode* Parser::parse_program(){
    parse_varsection();
    return parse_body();
}

struct StatementNode * parse_generate_intermediate_representation(){
    Parser *p=new Parser();
    return p->parse_program();
}


