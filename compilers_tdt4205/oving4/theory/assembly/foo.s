.data
.INTEGER: .string "%d\n"
.STRING0: .string "Sum is "
.STRING1: .string "You need at least one argument.\n"

.globl main

.text


foo:
    /* YOUR CODE HERE! */
    /* create stack frame */
    pushl   %ebp
    movl    %esp, %ebp /* copy stack pointer to base pointer */
    pushl   $0 /* int sum, first local on stack, access with -4(%ebp) */
    pushl   $0 /* int i = 1; the counter; second local on stack, access with -8(%ebp) */
    pushl   8(%ebp) /* argument 1; third local on stack, access with -12(%ebp) */

forloop:
    incl    -8(%ebp)
    movl    -12(%ebp),%ebx
    cmp     -8(%ebp),%ebx /* do i == N */
    je      loopend            /* if equal, return */
    movl    -8(%ebp),%eax /* copy i to eax for division */
    cdq		/* sign extend */
    movl    $3, %ebx   /* divide and modulo, mod is put in EDX */
    divl    %ebx
    cmp     $0, %edx
    jne     modfive
    /* add i to sum */
    movl    -8(%ebp), %ebx
    addl    %ebx, -4(%ebp)
    jmp     forloop
modfive:
    movl    -8(%ebp),%eax /* copy i to eax for division */
    cdq		/* sign extend */
    movl    $5, %ebx
    divl    %ebx   /* divide and modulo, mod is put in EDX */
    cmp     $0, %edx
    jne     forloop
    /* add i to sum */
    movl    -8(%ebp), %ebx
    addl    %ebx, -4(%ebp)
    jmp forloop
loopend:
    movl    -4(%ebp), %edx /* save sum in edx */
    /* print the return value we stored in edx before returning */
    pushl   %edx
    pushl   $.INTEGER
    call    printf
    addl    $8,%esp /* remove args */
    leave
    ret

main:
    /* Create a stack frame for main */
    pushl   %ebp
    movl    %esp,%ebp
    
    /* Check the number of arguments */
    movl    8(%ebp),%ebx
    cmp     $1,%ebx
    jg      args_ok
    
    /* Number of arguments was 0 (1 including program name, which is in argv[0]). Exit with error. */
    pushl   $.STRING1
    call printf
    addl    $4,%esp
    pushl   $1
    call exit

args_ok:

    /* Put the argv pointer into ebx */
    movl    12(%ebp),%ebx
    
    /* The second element of argv is our command line argument; increment ebx to point to that element */
    addl    $4,%ebx

    /* atoi(argv[1]) */
    pushl   (%ebx)
    call    atoi

    /* Rewind the stack to the state it was before pushing arguments onto it */
    addl    $4,%esp

    /* Push the return value from strtol onto the stack */
    pushl   %eax

    /* Call foo(), with one argument (top of stack) */
    call    foo

    /* Tear down the stack frame */
    leave

    /* exit(0) to indicate success */
    pushl   $0
    call    exit

