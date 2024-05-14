#include <drivers/screen.h>
#include <drivers/utils.h>

void keypress() {

    switch (inbyte(0x60)) {
        case 0x01: keybuffer(0x1B); break;
        case 0x0E: keybuffer(0x08); break;

        case 0x10: keybuffer('q'); break;
        case 0x11: keybuffer('w'); break;
        case 0x12: keybuffer('e'); break;
        case 0x13: keybuffer('r'); break;
        case 0x14: keybuffer('t'); break;
        case 0x15: keybuffer('y'); break;
        case 0x16: keybuffer('u'); break;
        case 0x17: keybuffer('i'); break;
        case 0x18: keybuffer('o'); break;
        case 0x19: keybuffer('p'); break;
        case 0x1A: keybuffer('['); break;
        case 0x1B: keybuffer(']'); break;
    
        case 0x1C: keybuffer(0x0A); break;
        
        case 0x1E: keybuffer('a'); break;
        case 0x1F: keybuffer('s'); break;
        case 0x20: keybuffer('d'); break;
        case 0x21: keybuffer('f'); break;
        case 0x22: keybuffer('g'); break;
        case 0x23: keybuffer('h'); break;
        case 0x24: keybuffer('j'); break;
        case 0x25: keybuffer('k'); break;
        case 0x26: keybuffer('l'); break;
        case 0x27: keybuffer(';'); break;
        case 0x28: keybuffer('\''); break;
        case 0x29: keybuffer('`'); break;

        case 0x2B: keybuffer('\\'); break;
    
        case 0x2C: keybuffer('z'); break;
        case 0x2D: keybuffer('x'); break;
        case 0x2E: keybuffer('c'); break;
        case 0x2F: keybuffer('v'); break;
        case 0x30: keybuffer('b'); break;
        case 0x31: keybuffer('n'); break;
        case 0x32: keybuffer('m'); break;
        case 0x33: keybuffer(','); break;
        case 0x34: keybuffer('.'); break;
        case 0x35: keybuffer('/'); break;

        case 0x39: keybuffer(' '); break;
    }
}