;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									ENCABEZADO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

; Nombre:		Zingarelli Facundo
; Nro. alumno:	66092/1
; Parpadeo de LED con rutina BLOQUEANTE

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;						DEFINICIÓN DE CONSTANTES Y VARIABLES
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

.equ	contINT = 1000	; Contador interno
.def	contEXT= r18	; Contador externo

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;					INICIALIZACIÓN Y CONFIGURACIÓN DE PUERTO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

ldi r16,HIGH(RAMEND)	; Inicializa la pila
out SPH,r16
ldi r16,LOW(RAMEND)
out SPL,r16				

sbi DDRB,5				; PortB salida (LED)
cbi PORTB,5				; Se inicializa en bajo
ldi r17,(1<<5)			; Máscara para encender el LED

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;								RUTINA BLOQUEANTE
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

Loop1:	
	ldi contEXT,200			; Se setea el cont externo para la próxima iteración
	in  r16,PinB			; Se sensa el estado del LED
	eor r16,r17				
	out portB,r16			; Se invierte el estado (conmuta)
	

	Loop2:						
		ldi	xl,LOW(contINT)		; Se inicializa el contador interno
		ldi	xh,HIGH(contINT)	

		Loop3:	
			sbiw	x,1			; Resta 1 al contador de 2 bytes
		brne	Loop3			

		dec	contEXT				; Decrementa contador externo
	
	brne Loop2					; Evalúa

jmp Loop1

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									FIN DEL PROGRAMA
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;