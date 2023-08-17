#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <cstring>
#include <string>
#include "compiler.h"

using namespace std;

#define DEBUG 1     // 1 => Turn ON debugging, 0 => Turn OFF debugging

int mem[1000];
int next_available = 0;

std::vector<int> inputs;
int next_input = 0;

void debug(const char* format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        va_end (args);
    }
}

void execute_program(struct InstructionNode * program)
{
    struct InstructionNode * pc = program;
    int op1, op2, result;

    while(pc != NULL)
    {
        if(1 == 0) cout <<"COMMMAND TYPE: " << pc->type << endl;
        switch(pc->type)
        {
            case NOOP:
                if(1 == 0) cout <<"NOOP" << endl;
                pc = pc->next;
                break;
            case IN:
                if(1 == 0) cout <<"IN" << endl;

                mem[pc->input_inst.var_index] = inputs[next_input];
                if(1 == 0) cout <<"INPUT TO MEMORY " << pc->input_inst.var_index << " VALUE " << inputs[next_input] << endl;
                next_input++;
                pc = pc->next;
                break;
            case OUT:
                if(1 == 0) cout <<"OUT" << endl;
                printf("%d ", mem[pc->output_inst.var_index]);
                if(1 == 0) cout <<"OUTPUT FROM MEMORY " << pc->output_inst.var_index << " VALUE >> " <<  mem[pc->output_inst.var_index] << endl;
                pc = pc->next;
                break;
            case ASSIGN:
                if(1 == 0) cout <<"ASSIGN" << endl;
                switch(pc->assign_inst.op)
                {
                    case OPERATOR_PLUS:
                        op1 = mem[pc->assign_inst.operand1_index];
                        op2 = mem[pc->assign_inst.operand2_index];
                        if(1 == 0) cout <<"ADDITION: " << op1 << " + " << op2 << endl;
                        result = op1 + op2;
                        break;
                    case OPERATOR_MINUS:
                        op1 = mem[pc->assign_inst.operand1_index];
                        op2 = mem[pc->assign_inst.operand2_index];
                        result = op1 - op2;
                        break;
                    case OPERATOR_MULT:
                        op1 = mem[pc->assign_inst.operand1_index];
                        op2 = mem[pc->assign_inst.operand2_index];
                        result = op1 * op2;
                        break;
                    case OPERATOR_DIV:
                        op1 = mem[pc->assign_inst.operand1_index];
                        op2 = mem[pc->assign_inst.operand2_index];
                        result = op1 / op2;
                        break;
                    case OPERATOR_NONE:
                        op1 = mem[pc->assign_inst.operand1_index]; // i = 37     i = j
                        if(1 == 0) cout <<"ASSIGNMENT: " << pc->assign_inst.operand1_index << ": " << op1 << endl;
                        result = op1;
                        break;
                }
                mem[pc->assign_inst.left_hand_side_index] = result;
                if(1 == 0) cout <<"STORE TO LEFT SIDE " << pc->assign_inst.left_hand_side_index << " VALUE " << result << endl;
                if(1 == 0) cout <<"Advancing to the next of " << pc << endl;
                pc = pc->next;
                if(1 == 0) cout <<"NEXT INSTRUCTION: " << pc << endl;
                break;
            case CJMP:
                if(1 == 0) cout <<"CJMP" << endl;
                //if (pc->cjmp_inst.target == NULL)
                //{
                //    debug("Error: pc->cjmp_inst->target is null.\n");
                //    exit(1);
                //}
                op1 = mem[pc->cjmp_inst.operand1_index];
                op2 = mem[pc->cjmp_inst.operand2_index];
                if(1 == 0) cout <<"CONDITIONAL JUMP: " << op1 << " " << pc->cjmp_inst.condition_op << " " << op2 << endl;
                switch(pc->cjmp_inst.condition_op)
                {
                    case CONDITION_GREATER:
                        if(1 == 0) cout <<"GREATER THAN; advancing to body because " << op1 << " > " << op2 << endl;

                        if(op1 > op2) {
                            if(1 == 0) cout <<"NEXT" << endl;
                            pc = pc->next;
                            if(1 == 0) cout <<"NEXT: " << pc << endl;
                        }
                        else {
                            if(1 == 0) cout <<"OUT" << endl;
                            pc = pc->cjmp_inst.target;
                        }
                        break;
                    case CONDITION_LESS:
                        if(op1 < op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                    case CONDITION_NOTEQUAL:
                        if(op1 != op2)
                            pc = pc->next;
                        else
                            pc = pc->cjmp_inst.target;
                        break;
                }
                break;
            case JMP:
                if(1 == 0) cout <<"JMP" << endl;

                if (pc->jmp_inst.target == NULL)
                {
                    debug("Error: pc->jmp_inst->target is null.\n");
                    exit(1);
                }
                pc = pc->jmp_inst.target;
                break;
            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                exit(1);
                break;
        }
    }
}

int main()
{
    struct InstructionNode * program;
    program = parse_generate_intermediate_representation();
    execute_program(program);
    return 0;
}
