;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									ENCABEZADO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

; Nombre:			Zingarelli Facundo
; Nro. alumno:		66092/1
; Parpadeo de LED con rutina NO-BLOQUEANTE

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;					   VECTORES DE INTERRUPCIÓN Y ORGANIZACIÓN
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

rjmp programa			; Se organizan los vectores de interrupción y
	.ORG 0X16			; estructura del programa
rjmp interrupt_T1A

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;						INICIALIZACIÓN Y CONFIGURACIÓN
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

programa:

ldi R20,$FF
out $04,R20 ; dirección de puerto B como salidas

ldi R20,$00
out $05,R20 ; inicializa Puerto B en 0

ldi R20,$40
sts $80,R20 ; configura TCCR1A en CTC - prescaler 1024 reloj interno
ldi R20,$0D ; TCCR1B
sts $81,R20
ldi R20,$00
ldi R20,$00
sts $84,R20 ; Inicializa contador en 0

ldi R20,$3D	
sts $89,R20 ; Se carga el valor a conmparar OCR1AH
ldi R20,$08
sts $88,R20 ; Se carga el valor a conmparar OCR1AL
ldi R20,$02 ; Habrá una interrupción cada 500 ms a 16Mhz

sts $6F,R20		; Habilita interrupt. OCIE1A
ldi r17,(1<<5)	; Máscara para conmutar el LED
sei				; Habilita interrupciones generales

ldi r16,(1<<1)	
out SMCR, r16		; Se configura el sleep en modo profundo

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									MAIN
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

loop:
	 sleep					; Entra en modo sleep profundo (Admite interrup. externas)
jmp loop

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;							RUTINA DE INTERRUPCIÓN
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

interrupt_T1A:
	in r16,PinB		; Sensa el estado del LED
	eor r16,r17		
	out PortB,r16	; Invierte el estado(conmuta)
reti

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;								FIN DEL PROGRAMA
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;