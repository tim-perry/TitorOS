db 0 ;status

;address of first sector
;The starting sector fields are
;limited to 1023+1 cylinders, 255+1 heads, and 63 sector
; ending sector fields have the same limitations.
db 0;head
db 0;sector
db 0;cylinder

db 00 ;partition type

;address of last sector
db 0;head
db 0;sector
db 0;

;little-endian
dd 0;lba of first sector
dd 0; no of sectors