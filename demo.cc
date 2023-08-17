#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include <ostream>
#include <map>
#include "compiler.h"
#include "lexer.h"
using namespace std;

LexicalAnalyzer lexer;

int varID = 0;
map<string, int> varMap;

Token consume(TokenType expected_type); // consume the current token and move to the next token
Token peek();
void processVarSection();
void processIdList();
struct InstructionNode* processBody();
struct InstructionNode* processStmtList();
struct InstructionNode* processStmt();
struct InstructionNode* processInputStmt();
struct InstructionNode* processOutputStmt();
struct InstructionNode* processAssignStmt();
struct InstructionNode* processIfStmt();
struct InstructionNode* processWhileStmt();
struct InstructionNode* processForStmt();
struct InstructionNode* processSwitchStmt();
void processInputs();
void parseNumList();
struct InstructionNode* processProgram();

Token consume(TokenType expected_type) { // consume the current token and move to the next token
    Token t = lexer.GetToken();
    return t;
}

Token peek() { // peek at the next token
    Token t = lexer.GetToken();
    //if(1 == 0) cout <<t.lexeme << " " << t.line_no << endl;
    lexer.UngetToken(1);
    return t;
}

void processVarSection() { 
    // Token t = require(ID);
    if(1 == 0) cout <<"vars" << endl;
    processIdList();
    consume(SEMICOLON);
    if(1 == 0) cout <<"varSection end" << endl;
}

void processIdList() {
    Token t = consume(ID);
    varMap[t.lexeme] = varID; // add to map
    if(1 == 0) cout <<t.lexeme << " " << varID << endl;
    varID++;
    t = peek();
    //if(1 == 0) cout <<t.token_type << " " << COMMA << endl;
    if (t.token_type == COMMA) {
        consume(COMMA);
        processIdList();
    }
}

struct InstructionNode* processBody() {
    if(1 == 0) cout <<"body" << endl;
    struct InstructionNode* result = nullptr;
    if(1 == 0) cout <<"BETTER BE LBRACE: " << peek().token_type << endl;
    consume(LBRACE);
    result = processStmtList();
    consume(RBRACE);
    if(1 == 0) cout <<"body end" << endl;
    return result;
}

struct InstructionNode* processStmtList() {
    if(1 == 0) cout <<"stmtList" << endl;
    struct InstructionNode* stmt = processStmt();
    struct InstructionNode* afterStmtChain = stmt;
    while (afterStmtChain->next != nullptr) {
        afterStmtChain = afterStmtChain->next;
    }


    Token t = peek();
    if (t.token_type != RBRACE) {
        if(1 == 0) cout <<"NO CLOSING BRACKET" << endl;
        afterStmtChain->next = processStmtList();
    }
    return stmt;
}

struct InstructionNode* processStmt() {
    if(1 == 0) cout <<"stmt" << endl;
    struct InstructionNode* result = nullptr;
    Token t = peek();
    if(1 == 0) cout <<t.lexeme << " " << t.line_no << " " << t.token_type << endl;
    switch (t.token_type) {
        case INPUT:
            result = processInputStmt();
            break;
        case OUTPUT:
            result = processOutputStmt();
            break;
        case ID:
            result = processAssignStmt();
            break;
        case IF:
            result = processIfStmt();
            break;
        case WHILE:
            result = processWhileStmt();
            break;
        case FOR:
            result = processForStmt();
            break;
        case SWITCH:
            result = processSwitchStmt();
            break;
        default:
            if(1 == 0) cout <<"error" << endl;
            exit(0);
    }
    return result;
}

InstructionNode* processInputStmt() {
    InstructionNode* inputInst = new InstructionNode();
    inputInst->type = IN;
    consume(INPUT);
    Token t = consume(ID);
    if(1 == 0) cout <<"INPUT" << " " << t.lexeme << endl;
    inputInst->input_inst.var_index = varMap[t.lexeme];
    inputInst->next = nullptr;
    consume(SEMICOLON);
    return inputInst;
}

InstructionNode* processOutputStmt() {
    InstructionNode* outputInst = new InstructionNode();
    outputInst->type = OUT;
    consume(OUTPUT);
    Token t = consume(ID);
    if(1 == 0) cout <<"OUTPUT" << " " << t.lexeme << endl;
    outputInst->output_inst.var_index = varMap[t.lexeme];
    outputInst->next = nullptr;
    consume(SEMICOLON);
    return outputInst;
}

InstructionNode* processAssignStmt() {
    InstructionNode* assignInst = new InstructionNode();
    assignInst->type = ASSIGN;

    Token t = consume(ID);
    assignInst->assign_inst.left_hand_side_index = varMap[t.lexeme];
    consume(EQUAL); // consume the equal sign

    Token t1 = lexer.GetToken(); // i = t1 (t1 mightbe variable or constant number)
    if (t1.token_type == ID) {
        assignInst->assign_inst.operand1_index = varMap[t1.lexeme];
    } else if (t1.token_type == NUM) {
        // create a new variable
        mem[varID] = atoi(t1.lexeme.c_str());
        assignInst->assign_inst.operand1_index = varID;
        varID++;
    }

    Token t2 = peek();
    
    if (t2.token_type == SEMICOLON) {
        assignInst->assign_inst.op = OPERATOR_NONE;
        if(1 == 0) cout <<"ASSIGNSimple" << " " << t.lexeme << " " << t1.lexeme << endl;
    } else {
        consume(t2.token_type);
        switch (t2.token_type) {
            case PLUS:
                assignInst->assign_inst.op = OPERATOR_PLUS;
                if(1 == 0) cout <<"plus" << endl;
                break;
            case MINUS:
                assignInst->assign_inst.op = OPERATOR_MINUS;
                if(1 == 0) cout <<"minus" << endl;
                break;
            case MULT:
                assignInst->assign_inst.op = OPERATOR_MULT;
                if(1 == 0) cout <<"mult" << endl;
                break;
            case DIV:
                assignInst->assign_inst.op = OPERATOR_DIV;
                if(1 == 0) cout <<"div" << endl;
                break;
            default:
                if(1 == 0) cout <<"unrecognized operator" << endl;
                if(1 == 0) cout <<t2.token_type << endl;
                exit(0);
        }

        Token t3 = lexer.GetToken();
        if (t3.token_type == NUM) { 
            // create a new variable
            mem[varID] = atoi(t3.lexeme.c_str());
            assignInst->assign_inst.operand2_index = varID;
            varID++;

        } else if (t3.token_type == ID) {
            assignInst->assign_inst.operand2_index = varMap[t3.lexeme];
        }
        if(1 == 0) cout <<"ASSIGNEvent" << " " << t.lexeme << " " << t1.lexeme << " " << t2.lexeme << " " << t3.lexeme << endl;
    }
    consume(SEMICOLON);
    assignInst->next = nullptr;
    return assignInst;
}


void fillConditional(InstructionNode* ifInst) {

    Token t1 = peek();
    consume(t1.token_type);
    if (t1.token_type == ID) {
        ifInst->cjmp_inst.operand1_index = varMap[t1.lexeme];
    } else if (t1.token_type == NUM) {
        // create a new variable
        mem[varID] = atoi(t1.lexeme.c_str());
        ifInst->cjmp_inst.operand1_index = varID;
        varID++;
    }

    Token t2 = peek();
    consume(t2.token_type);
    if(t2.token_type == GREATER) {
        ifInst->cjmp_inst.condition_op = CONDITION_GREATER;
    } else if (t2.token_type == LESS) {
        ifInst->cjmp_inst.condition_op = CONDITION_LESS;
    } else if (t2.token_type == NOTEQUAL) {
        ifInst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    }

    Token t3 = peek();
    consume(t3.token_type);
    if (t3.token_type == NUM) {
        // create a new variable
        mem[varID] = atoi(t3.lexeme.c_str());
        ifInst->cjmp_inst.operand2_index = varID;
        varID++;
    } else if (t3.token_type == ID) {
        ifInst->cjmp_inst.operand2_index = varMap[t3.lexeme];
    }

}

InstructionNode* processIfStmt() {
    if(1 == 0) cout <<"if" << endl;
    InstructionNode* ifInst = new InstructionNode();
    consume(IF);
    ifInst->type = CJMP;

    fillConditional(ifInst);
    if(1 == 0) cout <<"CONDITIONAL FILLED: " << peek().token_type << endl;

    InstructionNode * ifBody = processBody();
    ifInst->next = ifBody;

    InstructionNode * ifBodyEnd = new InstructionNode();
    ifBodyEnd->type = NOOP;

    InstructionNode * afterBody = ifBody;
    while (afterBody->next != nullptr) {
        afterBody = afterBody->next;
    }
    afterBody->next = ifBodyEnd;

    ifInst->cjmp_inst.target = ifBodyEnd;

    return ifInst;
}

InstructionNode* processWhileStmt() {
    if(1 == 0) cout <<"while" << endl;

    InstructionNode* whileInst = new InstructionNode();
    consume(WHILE);
    whileInst->type = CJMP;

    fillConditional(whileInst);
    if(1 == 0) cout <<"CONDITIONAL FILLED: " << peek().token_type << endl;

    InstructionNode * ifBody = processBody();
    whileInst->next = ifBody;

    InstructionNode * afterBody = ifBody;
    while (afterBody->next != nullptr) {
        afterBody = afterBody->next;
    }

    InstructionNode * jumper = new InstructionNode();
    jumper->type = JMP;
    jumper->jmp_inst.target = whileInst;

    afterBody->next = jumper;

    InstructionNode * whileBodyEnd = new InstructionNode();
    whileBodyEnd->type = NOOP;

    jumper->next = whileBodyEnd;
    whileInst->cjmp_inst.target = whileBodyEnd;


    return whileInst;
}

// FOR LPAREN assign stmt condition SEMICOLON assign stmt RPAREN body
// FOR ( b = a + 1 ; b < c ; b = b + 2 ) {    
InstructionNode* processForStmt() {
    if(1 == 0) cout <<"for" << endl;

    consume(FOR);
    consume(LPAREN);

    // b = 7
    // for ( a = 5 ; a < b ; a = a + 1 ) {
    // becomes:
    // b = 7
    // a = 5
    // while a < b {
    //     output a
    //     a = a + 1
    // }


    InstructionNode* forInitialize = processAssignStmt();

    InstructionNode* whileInst = new InstructionNode();
    whileInst->type = CJMP;
    forInitialize->next = whileInst;

    fillConditional(whileInst);
    if(1 == 0) cout <<"CONDITIONAL FILLED: " << peek().token_type << endl;
    consume(SEMICOLON);

    InstructionNode * forProgress = processAssignStmt(); // "i++" as it were
    consume(RPAREN);

    InstructionNode * ifBody = processBody();
    whileInst->next = ifBody;

    InstructionNode * afterBody = ifBody;
    while (afterBody->next != nullptr) {
        afterBody = afterBody->next;
    }

    afterBody->next = forProgress;
    afterBody = forProgress;

    InstructionNode * jumper = new InstructionNode();
    jumper->type = JMP;
    jumper->jmp_inst.target = whileInst;

    afterBody->next = jumper;

    InstructionNode * whileBodyEnd = new InstructionNode();
    whileBodyEnd->type = NOOP;

    jumper->next = whileBodyEnd;
    whileInst->cjmp_inst.target = whileBodyEnd;


    return forInitialize;
    
}

/*
SWITCH a {
        CASE 3 : { output b ; }
        CASE 5 : { output b ; output c ; }
        CASE 7 : { output b ; output c ; output d ; }
        CASE 9 : { output b ; output c ; output d ; output e ; }
        DEFAULT : { output e ; output d ; }
    }
*/
InstructionNode* processSwitchStmt() {
    if(1 == 0) cout <<"switch" << endl;

    InstructionNode * switchStart = new InstructionNode();
    switchStart->type = NOOP;
    consume(SWITCH);
    InstructionNode* switchChain = switchStart; // begin building on the chain

    Token compareVar = consume(ID);

    consume(LBRACE);
    // start with a no-op to build upon
    // for each case, create an if statement with a body as defined, ending in a no-op for false and after-body
    // chain these together with the default at the end
    InstructionNode * switchOut = new InstructionNode();
    switchOut->type = NOOP;

    Token t = peek();
    while(t.token_type == CASE) {
        if(1 == 0) cout <<"CASE TIME: " << t.token_type << " " << t.lexeme << endl;
        consume(CASE);
        Token t2 = peek();
        if(1 == 0) cout <<"CASE: " << t2.lexeme << endl;
        consume(NUM);
        consume(COLON);

        // case 3: runs if case IS 3, but we can ONLY check for non-equality
        // if->cjmp_inst.target is where we go if something is FALSE. non-equality being FALSE 
        //  means that the case is true, so the cjmp_inst.target points to the body (becuase it's NOT not different)
        InstructionNode* caseIf = new InstructionNode();
        caseIf->type = CJMP;
        if(1 == 0) cout <<"Comparing equality of " << compareVar.lexeme << " AND " << t2.lexeme << endl;
        caseIf->cjmp_inst.operand1_index = varMap[compareVar.lexeme];
        // NEED TO INSERT INTO MEMORY, NOT CONVERT INTO TO INDEX

        if (t2.token_type == ID) {
            caseIf->cjmp_inst.operand2_index = varMap[t2.lexeme];
        } else if (t2.token_type == NUM) {
            // create a new variable
            mem[varID] = atoi(t2.lexeme.c_str());
            caseIf->cjmp_inst.operand2_index  = varID;
            varID++;
        }

        caseIf->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        // ifEnd is where we go if it's TRUE that it's not equal to this case
        InstructionNode* ifEnd = new InstructionNode();
        ifEnd->type = NOOP;
        caseIf->next = ifEnd; // if "var != case" is true, go to the end of the if

        // process the case body (the stuff we do if it's NOT not equal)
        InstructionNode* caseBody = processBody();
        InstructionNode* afterBody = caseBody;
        while(afterBody->next != nullptr) {
            afterBody = afterBody->next;
        }

        // after doing this, there won't be any more cases to consider, so get out of the switch
        afterBody->next = switchOut;

        // if it's false that it's not equal, run the body
        caseIf->cjmp_inst.target = caseBody;

        // append the if statement to the switch chain
        switchChain->next = caseIf;
        // advance the switch chain to the end of this comparison
        switchChain = ifEnd;

        t = peek();
    }

    if(1 == 0) cout <<"LOOKING FOR A DEFAULT: " << peek().token_type << endl;
    consume(DEFAULT);
    consume(COLON);
    InstructionNode* defaultBody = processBody();
    switchChain->next = defaultBody;
    InstructionNode* afterDefault = defaultBody;
    while(afterDefault->next != nullptr) {
        afterDefault = afterDefault->next;
    }
    consume(RBRACE);
    afterDefault->next = switchOut;
    switchChain = switchOut; // move to the final instrution

    return switchStart; // return the start of the switch
}

void processInputs() {
    if(1 == 0) cout <<"inputs" << endl;
    parseNumList();
    if(1 == 0) cout <<"inputs end" << endl;
}

void parseNumList() {
    Token t = consume(NUM);
    inputs.push_back(stoi(t.lexeme));
    if(1 == 0) cout <<inputs[inputs.size() - 1] << endl;
    Token t2 = peek();
    if (t2.token_type == NUM) {
        parseNumList();
    }
}

InstructionNode* processProgram() {
    processVarSection();
    InstructionNode* body = processBody();
    processInputs();
    return body;
}
struct InstructionNode * parse_generate_intermediate_representation()
{ 
    //if(1 == 0) cout <<"Hello World" << endl;
    //exit(0);

    InstructionNode* process = processProgram();
    InstructionNode* runner = process;
    /*
    if(1 == 0) cout <<"-------------------" << endl;
    while (runner != nullptr) {
        if(1 == 0) cout <<runner->type << endl;
        switch (runner->type) {
            case IN:
                if(1 == 0) cout <<"IN" << " " << runner->input_inst.var_index << endl;
                break;
            case OUT:
                if(1 == 0) cout <<"OUT" << " " << runner->output_inst.var_index << endl;
                break;
            case ASSIGN:
                if(1 == 0) cout <<"ASSIGN" << " " << runner->assign_inst.left_hand_side_index << " " 
                        << runner->assign_inst.operand1_index << " " << runner->assign_inst.op 
                        << " " << runner->assign_inst.operand2_index << endl;
                break;
            case CJMP:
                if(1 == 0) cout <<"CJMP" << " " << runner->cjmp_inst.operand1_index << " "
                     << runner->cjmp_inst.condition_op << " " << runner->cjmp_inst.operand2_index << " "
                    << runner->cjmp_inst.target << endl;
                break;
            case NOOP:
                if(1 == 0) cout <<"NOOP" << endl;
                break;
            default:
                if(1 == 0) cout <<"unrecognized instruction" << endl;
                exit(0);
        }

        runner = runner->next;
    }
    */
    for(int i = 0; i < inputs.size(); i++) {
        if(1 == 0) cout <<"IN: " << i << ": " << inputs[i] << endl;
    }
    if(1 == 0) cout <<"done" << endl;
    return process; 
    /*
    // Sample program for demonstration purpose only
    // Replace the following with a parser that reads the program from stdin &
    // creates appropriate data structures to be executed by execute_program()
    // This is the imaginary input for the following construction:
    
    // a, b, c, d;
    // {
    //     input a;
    //     input b;
    //     c = 10;
    // 
    //     IF c <> a
    //     {
    //         output b;
    //     }
    // 
    //     IF c > 1
    //     {
    //         a = b + 900;
    //         input d;
    //         IF a > 10
    //         {
    //             output d;
    //         }
    //     }
    // 
    //     d = 0;
    //     WHILE d < 4
    //     {
    //         c = a + d;
    //         IF d > 1
    //         {
    //             output d;
    //         }
    //         d = d + 1;
    //     }
    // }
    // 1 2 3 4 5 6

    // Assigning location for variable "a"
    int address_a = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for variable "b"
    int address_b = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for variable "c"
    int address_c = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for variable "d"
    int address_d = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for constant 10
    int address_ten = next_available;
    mem[next_available] = 10;
    next_available++;

    // Assigning location for constant 1
    int address_one = next_available;
    mem[next_available] = 1;
    next_available++;

    // Assigning location for constant 900
    int address_ninehundred = next_available;
    mem[next_available] = 900;
    next_available++;

    // Assigning location for constant 3
    int address_three = next_available;
    mem[next_available] = 3;
    next_available++;

    // Assigning location for constant 0
    int address_zero = next_available;
    mem[next_available] = 0;
    next_available++;

    // Assigning location for constant 4
    int address_four = next_available;
    mem[next_available] = 4;
    next_available++;

    struct InstructionNode * i1 = new InstructionNode;
    struct InstructionNode * i2 = new InstructionNode;
    struct InstructionNode * i3 = new InstructionNode;
    struct InstructionNode * i4 = new InstructionNode;
    struct InstructionNode * i5 = new InstructionNode;
    struct InstructionNode * i6 = new InstructionNode;
    struct InstructionNode * i7 = new InstructionNode;
    struct InstructionNode * i8 = new InstructionNode;
    struct InstructionNode * i9 = new InstructionNode;
    struct InstructionNode * i10 = new InstructionNode;
    struct InstructionNode * i11 = new InstructionNode;
    struct InstructionNode * i12 = new InstructionNode;
    struct InstructionNode * i13 = new InstructionNode;
    struct InstructionNode * i14 = new InstructionNode;
    struct InstructionNode * i15 = new InstructionNode;
    struct InstructionNode * i16 = new InstructionNode;
    struct InstructionNode * i17 = new InstructionNode;
    struct InstructionNode * i18 = new InstructionNode;
    struct InstructionNode * i19 = new InstructionNode;
    struct InstructionNode * i20 = new InstructionNode;
    struct InstructionNode * i21 = new InstructionNode;
    struct InstructionNode * i22 = new InstructionNode;

    i1->type = IN;                                      // input a
    i1->input_inst.var_index = address_a;
    i1->next = i2;

    i2->type = IN;                                      // input b
    i2->input_inst.var_index = address_b;
    i2->next = i3;

    i3->type = ASSIGN;                                  // c = 10
    i3->assign_inst.left_hand_side_index = address_c;
    i3->assign_inst.type = OPERATOR_NONE;
    i3->assign_inst.operand1_index = address_ten;
    i3->next = i4;

    i4->type = CJMP;                                    // if c <> a
    i4->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    i4->cjmp_inst.operand1_index = address_c;
    i4->cjmp_inst.operand2_index = address_a;
    i4->cjmp_inst.target = i6;                          // if not (c <> a) skip forward to NOOP
    i4->next = i5;

    i5->type = OUT;                                     // output b
    i5->output_inst.var_index = address_b;
    i5->next = i6;

    i6->type = NOOP;                                    // NOOP after IF
    i6->next = i7;

    i7->type = CJMP;                                    // if c > 1
    i7->cjmp_inst.condition_op = CONDITION_GREATER;
    i7->cjmp_inst.operand1_index = address_c;
    i7->cjmp_inst.operand2_index = address_one;
    i7->cjmp_inst.target = i13;                         // if not (c > 1) skip forward to NOOP (way down)
    i7->next = i8;

    i8->type = ASSIGN;                                  // a = b + 900
    i8->assign_inst.left_hand_side_index = address_a;
    i8->assign_inst.type = OPERATOR_PLUS;
    i8->assign_inst.operand1_index = address_b;
    i8->assign_inst.operand2_index = address_ninehundred;
    i8->next = i9;

    i9->type = IN;                                      // input d
    i9->input_inst.var_index = address_d;
    i9->next = i10;

    i10->type = CJMP;                                   // if a > 10
    i10->cjmp_inst.condition_op = CONDITION_GREATER;
    i10->cjmp_inst.operand1_index = address_a;
    i10->cjmp_inst.operand2_index = address_ten;
    i10->cjmp_inst.target = i12;                        // if not (a > 10) skipp forward to NOOP
    i10->next = i11;

    i11->type = OUT;                                    // output d
    i11->output_inst.var_index = address_d;
    i11->next = i12;

    i12->type = NOOP;                                   // NOOP after inner IF
    i12->next = i13;

    i13->type = NOOP;                                   // NOOP after outer IF
    i13->next = i14;

    i14->type = ASSIGN;                                 // d = 0
    i14->assign_inst.left_hand_side_index = address_d;
    i14->assign_inst.type = OPERATOR_NONE;
    i14->assign_inst.operand1_index = address_zero;
    i14->next = i15;

    i15->type = CJMP;                                   // if d < 4
    i15->cjmp_inst.condition_op = CONDITION_LESS;
    i15->cjmp_inst.operand1_index = address_d;
    i15->cjmp_inst.operand2_index = address_four;
    i15->cjmp_inst.target = i22;                        // if not (d < 4) skip whole WHILE body
    i15->next = i16;

    i16->type = ASSIGN;                                 // c = a + d
    i16->assign_inst.left_hand_side_index = address_c;
    i16->assign_inst.type = OPERATOR_PLUS;
    i16->assign_inst.operand1_index = address_a;
    i16->assign_inst.operand2_index = address_d;
    i16->next = i17;

    i17->type = CJMP;                                   // if d > 1
    i17->cjmp_inst.condition_op = CONDITION_GREATER;
    i17->cjmp_inst.operand1_index = address_d;
    i17->cjmp_inst.operand2_index = address_one;
    i17->cjmp_inst.target = i19;                        // if not (d > 1) skip body of IF
    i17->next = i18;

    i18->type = OUT;                                    // output d
    i18->output_inst.var_index = address_d;
    i18->next = i19;

    i19->type = NOOP;                                   // NOOP after body of IF
    i19->next = i20;

    i20->type = ASSIGN;                                 // d = d + 1
    i20->assign_inst.left_hand_side_index = address_d;
    i20->assign_inst.type = OPERATOR_PLUS;
    i20->assign_inst.operand1_index = address_d;
    i20->assign_inst.operand2_index = address_one;
    i20->next = i21;

    i21->type = JMP;
    i21->jmp_inst.target = i15;
    i21->next = i22;

    i22->type = NOOP;                                   // NOOP after body of WHILE
    i22->next = NULL;

    // Inputs
    inputs.push_back(1);
    inputs.push_back(2);
    inputs.push_back(3);
    inputs.push_back(4);
    inputs.push_back(5);
    inputs.push_back(6);

    return i1; */
}
