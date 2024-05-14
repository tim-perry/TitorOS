#include <stdint.h>
#include <stdarg.h>
#include <libc/stdio.h>
#include <libc/syscalls.h>
#include <libc/stdlib.h>



void printf(char* string, ...) {
    char* part = string;
    char* fstring = malloc(400); //arbitrary
    uint32_t value;
    
    va_list ptr;
    va_start(ptr, string);
    
    for(uint32_t i = 0; string[i] != 0; i++) {
        
        if (string[i] == '%') {
            string[i] = 0;
            write(stdout, part, 0);
            string[i] = '%';
            i++;
            //print formatted string from args depending on char after %
            switch (string[i]) {
                case '%':
                fstring[0] = '%';
                fstring[1] = 0;
                write(stdout, fstring, 0);
                break;

                case 'u':
                value = va_arg(ptr, uint32_t);
                uint32_t divisor = 1000000000;
                for(uint8_t j = 0; j < 10; j++) {
                    fstring[j] = value / divisor + 0x30;
                    value -= (value / divisor) * divisor;
                    divisor /= 10;
                }
                fstring[10] = 0;
                part = fstring;
                for(uint8_t j = 0; j < 9; j++) {
                    if (fstring[j] == '0') part++;
                    else break;
                }
                write(stdout, part, 0);
                break;
                
                case 'x':
                case 'X':
                value = va_arg(ptr, uint32_t);
                for(uint8_t j = 0; j < 8; j++) {
                    fstring[j] = ((value >> (28 - 4*j)) & 0xF) + 0x30;
                    if (fstring[j] > 0x39) fstring[j] += 7 + (uint8_t)string[i] - (uint8_t)'X';
                }
                write(stdout, "0x", 0);
                fstring[8] = 0;
                part = fstring;
                for(uint8_t j = 0; j < 3; j++) {
                    if (fstring[2*j] == '0' && fstring[2*j+1] == '0') part += 2;
                    else break;
                }
                write(stdout, part, 0);
                break;
                
                case 'c':
                fstring[0] = (char)va_arg(ptr, uint32_t);
                fstring[1] = 0;
                write(stdout, fstring, 0);
                break;

                case 's':
                write(stdout, va_arg(ptr, char*), 0);
                break;

                default:
                write(stdout, "%", 0);
                fstring[0] = string[i];
                fstring[1] = 0;
                write(stdout, fstring, 0);
                break;
            }
            part = string + i + 1;
        }
    }
    
    write(stdout, part, 0);
    free(fstring);
    va_end(ptr);
    return;
}