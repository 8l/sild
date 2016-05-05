#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "eval.h"
#include "print.h"
#include "cell.h"
#include "env.h"

/* ----------------- */
/* builtin functions */
/* ----------------- */

static void arity_check(char *caller_name, int args, C *c) {
    C *cur = c;
    int passed_in = 0;
    while (cur->type != NIL) {
        passed_in++;
        cur = cur->next;
    }
    if (passed_in != args) {
        fprintf(stderr,
        "\nArityError: %s expected %d, got %d: ",
        caller_name, args, passed_in
        );
        print_to_err(c);
        exit(1);
    }
}

C *quote(C *operand, C **env) {
    arity_check("quote", 1, operand);
    return operand;
}

C *car(C *operand, C **env) {
    arity_check("car", 1, operand);

    operand = eval(operand, env);
    if (operand->type != LIST) {
        exit(1);
    }
    C* outcell = operand->val.list;
    free_cell(operand->val.list->next);
    outcell->next = &nil;
    free(operand);
    return outcell;
}

C *cdr(C *operand, C **env) {
    arity_check("cdr", 1, operand);

    operand = eval(operand, env);
    if (operand->type != LIST || operand->val.list->type == NIL) {
        exit(1);
    }
    C *garbage = operand->val.list;
    operand->val.list = operand->val.list->next;
    free_one_cell(garbage);
    return operand;
}

C *cons(C *operand, C **env) {
    arity_check("cons", 2, operand);

    operand = eval(operand, env);
    C *operand2 = eval(operand->next, env);
    if (operand2->type != LIST) {
        exit(1);
    }
    operand->next = operand2->val.list;
    operand2->val.list = operand;
    return operand2;
}

C *atom(C *operand, C **env) {
    arity_check("atom", 1, operand);
    operand = eval(operand, env);

    C *out;
    if (operand->type == LIST && operand->val.list->type != NIL) {
        out = empty_list();
    } else {
        out = truth();
    }
    free_cell(operand);
    return out;
}

C *eq(C *operand, C **env) {
    arity_check("eq", 2, operand);
    operand = eval(operand, env);
    C *operand2 = eval(operand->next, env);

    C *out;
    if (
            (
             operand->type == BUILTIN && operand2->type == BUILTIN
             &&
             (operand->val.func.addr == operand2->val.func.addr)
            )
            ||
            (
             operand->type == LABEL && operand2->type == LABEL
             &&
             scmp(operand->val.label, operand2->val.label)
            )
            ||
            (
             operand->type == LIST && operand2->type == LIST
             &&
             (operand->val.list == &nil && operand2->val.list == &nil)
            )
       )
    {
        out = truth();
    } else {
        out = empty_list();
    }
    free_cell(operand);
    return out;
}


C *cond(C *operand, C **env) {
    if (operand->type == NIL) {
        fprintf(stderr, "\nArityError: cond expected at least 1 argument, got none.");
        exit(1);
    } else if (operand->next->type == NIL) {
        return eval(operand, env);
    }
    // assigning 3 operands to their own vars.
    C *op1 = operand;
    C *op2 = operand->next;
    C *op3 = operand->next->next;

    // isolating the first two arguments
    op1->next = &nil;
    op2->next = &nil;
    op1 = eval(op1, env);
    if (!(op1->type == LIST && op1->val.list->type == NIL)) {
        // free the boolean expression statement
        free_cell(op1);
        // free anything else that was passed in
        free_cell(op3);
        // return the evalled second arg.
        return eval(op2, env);
    } else {
        // won't need these anymore!
        free_cell(op1);
        free_cell(op2);

        if (op3->type != NIL) {
            return cond(op3, env);
        } else {
            // if the op3 type is NIL, you've reached the end of the cond form
            // without encountering any true predicates, and should return the
            // empty list.
            return makecell(LIST, (V){.list = &nil}, &nil);
        }
    }
}

C *display(C *operand, C **env) {
    arity_check("display", 1, operand);
    C *evalled = eval(operand, env);
    print(evalled);
    return get_void();
}

C *define(C *operand, C **env) {
    arity_check("define", 2, operand);
    set(env, operand, eval(operand->next, env));
    return truth();
}

