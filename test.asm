main:
    movd string             # push address of string on expr stack
    call puts               # display the string
    halt                    # stop machine

puts:
    pushb                   # push character from address at top of expr stack
    jz puts_exit            # if we reached end-of-string then exit
    put                     # display character
    dropb                   # drop the character
    movd 000001h            # lets goto the next character
    add                     # step to next char by adding 1 to the address at TOES
    jmp puts                # lets go print the character
puts_exit:
    ret                     # exit puts


string:                     # the string to display
    db "Hello Word"
    db 0
