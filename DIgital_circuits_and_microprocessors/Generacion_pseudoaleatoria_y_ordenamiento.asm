;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									ENCABEZADO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

; Nombre:		Zingarelli Facundo
; Nro. alumno:	66092/1
; Ordenamiento de vectores y generación de números aleatorios

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
; DEFINICION DE CONSTANTES Y VARIABLES DE LA RUTINA DE GENERACIÓN DE NÚMEROS ALEATORIOS
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

.equ a=3					; El algoritmo consiste en: X(i+1) = (a.X(i)+b) % m
.equ b=1					; Se definen entonces como constantes dichos parámetros
.equ m=31

; Variables utilizadas para la rutina de división:

.def signo=r14				; Registro de signo
.def resto=r15				; Resto		
.def dividendo=r16			; Dividendo			 
.def divisor=r17			; Divisor
.def contador=r18			; Contador
.def anterior=r19			; Se guarda el resultado de la division anterior

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;		 DEFINICION DE CONSTANTES Y VARIABLES DE LA RUTINA DE ORDENAMIENTO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

.equ vector=0x0100			; Dirección del primer elemento del vector
.equ nro_elem=256			; Número de elementos del vector

.def contador1=r20			; Contador loop interno
.def contador2=r21			; Contador loop externo
.def aux_x=r22				; Registros aux para "mover" el
.def aux_y=r23				; contenido de una variable apuntada, a otra

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									PROGRAMA
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

sbi DDRB,5					; PortB como salida (LED)

loop_10:

ldi r16,10					; r1 hará de contador para las conmutaciones del LED
add r2,r16					; se realizó de esta manera por que todos los registros
							; de direccionamiento inmediato se utilizan en las rutinas.


loop_main:					; Loop de generación y ordenamiento

ldi xl,LOW(vector)			; Se inicializa el puntero "x" para la primera iteración 
ldi xh,HIGH(vector)			; de la rutina de números aleatorios

ldi contador1,7				; Se inicializa contador1 para la rutina de nros aleat
rcall aleatorios			; Se generan los nros aleatorios
rcall ordenamiento			; Se ordenan los números generados

dec r2
breq led					; Saltará a conmutar el LED cada 10 secuencias de
							; generamiento/ordenamiento 

jmp loop_main				; Loop infinito


led:
	ldi r17,(1<<5)			; Se carga el valor de la máscara para el led
	in r16,PinB				; Se guarda en r16 el estado del LED
	eor r16,r17			
	out PortB, r16			; Se invierte el estado (Enciende/apaga)
jmp loop_10

	
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;					  RUTINA DE GENERACION DE NÚMEROS ALEATORIOS
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

aleatorios:

ldi  r24,a								; Se carga en un registro la constante a para poder hacer
muls anterior,r24						; la multiplicación 
mov	 anterior,R0						; se copia el resultado en la variable"anterior"
ldi  r24,b								; Se realiza lo mismo con b
add  anterior,r24						; Se copia lo almacenado en el dividendo para la prox
mov  dividendo,anterior					; iteración
ldi divisor,m							; Se carga el divisor

division:	
	mov	 signo,dividendo				; Se copia el dividendo en"signo" para evaluar
	eor	 signo , divisor				; XOR con divisor
	sbrc divisor,7						; Si el MSB del divisor está seteado
	neg	 divisor						; cambia el signo del divisor
	sbrc dividendo,7					; Si el MSB del dividendo está seteado
	neg	 dividendo						; cambia el signo del dividendo, sino saltea la instr
	sub	 resto,resto					; Limpia el resto y el carry
	ldi	 contador,9						; Contador interno

loop_1:	

rol	dividendo							; Desplaza a izquierda el dividendo
dec	contador							; Decrementa contador
brne	loop_2							; Si terminó salta 
sbrc	signo,7							; Si el MSB of del registro de signo está seteado 
										; saltea la inversión de signo

neg	dividendo						    ; A esta altura de la división "dividendo" almacena
										; el resultado de la división

st x+,resto								; Se guarda el resto en memoria
mov anterior,resto						; Se carga en"anterior"para la sig iteracion
dec contador1							; decrementa contador de cant de elementos
brne aleatorios							; Evalua si el contador llega a cero

ret										; Si llegó a cero terminó de generar los 256
										; elementos y sale de la rutina.

loop_2:	
	rol	resto							; Desplaza a izq el resto para poder evaluar
	sub	 resto,divisor					; el signo
brcc carry_set							; Si el resultado es neg salta a carry_set
	add	 resto,divisor					; Setea el resto a su estado previo
	clc									; limpia el carry para que sea desplazado en el resultado
rjmp	loop_1							


carry_set:	
	sec									; Setea el carry para ser desplazado en el resultado
rjmp	loop_1

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;								RUTINA DE ORDENAMIENTO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

ordenamiento:

ldi xl,LOW(vector)						; Se inicializan punteros en el primer elem 
ldi xh,HIGH(vector)						; del vector
ldi yl,LOW(vector)						; (En el puntero x se almacenará el menor y 
ldi yh,HIGH(vector)						;  el puntero y recorrerá el vector)


ldi contador1,6							; Se inicializan contadores
ldi contador2,$00


menor:									; Este loop se encarga de hallar el menor
	inc yl								; de los elementos restantes del vector
	ld r16,x
	ld r17,y							; Se cargan en r16 y r17, los elem apuntados
	cp r17,r16						
		
	brlt ymenor							; Si el apuntado por y es menor salta a"ymenor"
	
	dec contador1						; Decrementa contador y evalúa
	breq switch							; Si llegó a cero salta a"switch"
jmp menor

ymenor:									
	movw x,y							; Se copia el valor apuntado en x
	dec contador1						; Decrementa contador y evalúa
	breq switch							; Si no llegó a cero, sig iteracion,
jmp menor								; de lo contrario salta a "switch"


switch:
	ldi yl,LOW(vector)					; Se apunta y al primer elemento y
	ldi yh,HIGH(vector)					; se le suma la contador2 (loop externo)
	add yl,contador2
	
	ld  aux_y,y							; Se almacenan los números apuntados en var aux
	ld  aux_x,x
	st  x,aux_y							; Se intercambian los contenidos
	st  y,aux_x
	
	inc yl								; Se apunta al prox elem y se copia la direcc
	movw x,y							; en x también
	
	inc contador2						; Se incr contador de loop externo
	ldi contador1,6						; Se vuelve a inicializar contador interno
	sub contador1,contador2				; Se le resta dicho valor
		
	cpi contador2,6						; se evalúa si terminó de ordenar todos los
brne menor								; vectores. Si es así sale de la rutina, de lo
										; contrario vuelve a "menor"
ret

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;								 FIN DEL PROGRAMA
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;




