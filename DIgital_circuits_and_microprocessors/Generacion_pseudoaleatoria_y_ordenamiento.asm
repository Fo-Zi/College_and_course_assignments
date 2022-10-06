;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									ENCABEZADO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

; Nombre:		Zingarelli Facundo
; Nro. alumno:	66092/1
; Ordenamiento de vectores y generaci�n de n�meros aleatorios

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
; DEFINICION DE CONSTANTES Y VARIABLES DE LA RUTINA DE GENERACI�N DE N�MEROS ALEATORIOS
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

.equ a=3					; El algoritmo consiste en: X(i+1) = (a.X(i)+b) % m
.equ b=1					; Se definen entonces como constantes dichos par�metros
.equ m=31

; Variables utilizadas para la rutina de divisi�n:

.def signo=r14				; Registro de signo
.def resto=r15				; Resto		
.def dividendo=r16			; Dividendo			 
.def divisor=r17			; Divisor
.def contador=r18			; Contador
.def anterior=r19			; Se guarda el resultado de la division anterior

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;		 DEFINICION DE CONSTANTES Y VARIABLES DE LA RUTINA DE ORDENAMIENTO
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

.equ vector=0x0100			; Direcci�n del primer elemento del vector
.equ nro_elem=256			; N�mero de elementos del vector

.def contador1=r20			; Contador loop interno
.def contador2=r21			; Contador loop externo
.def aux_x=r22				; Registros aux para "mover" el
.def aux_y=r23				; contenido de una variable apuntada, a otra

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;									PROGRAMA
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

sbi DDRB,5					; PortB como salida (LED)

loop_10:

ldi r16,10					; r1 har� de contador para las conmutaciones del LED
add r2,r16					; se realiz� de esta manera por que todos los registros
							; de direccionamiento inmediato se utilizan en las rutinas.


loop_main:					; Loop de generaci�n y ordenamiento

ldi xl,LOW(vector)			; Se inicializa el puntero "x" para la primera iteraci�n 
ldi xh,HIGH(vector)			; de la rutina de n�meros aleatorios

ldi contador1,7				; Se inicializa contador1 para la rutina de nros aleat
rcall aleatorios			; Se generan los nros aleatorios
rcall ordenamiento			; Se ordenan los n�meros generados

dec r2
breq led					; Saltar� a conmutar el LED cada 10 secuencias de
							; generamiento/ordenamiento 

jmp loop_main				; Loop infinito


led:
	ldi r17,(1<<5)			; Se carga el valor de la m�scara para el led
	in r16,PinB				; Se guarda en r16 el estado del LED
	eor r16,r17			
	out PortB, r16			; Se invierte el estado (Enciende/apaga)
jmp loop_10

	
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;					  RUTINA DE GENERACION DE N�MEROS ALEATORIOS
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;

aleatorios:

ldi  r24,a								; Se carga en un registro la constante a para poder hacer
muls anterior,r24						; la multiplicaci�n 
mov	 anterior,R0						; se copia el resultado en la variable"anterior"
ldi  r24,b								; Se realiza lo mismo con b
add  anterior,r24						; Se copia lo almacenado en el dividendo para la prox
mov  dividendo,anterior					; iteraci�n
ldi divisor,m							; Se carga el divisor

division:	
	mov	 signo,dividendo				; Se copia el dividendo en"signo" para evaluar
	eor	 signo , divisor				; XOR con divisor
	sbrc divisor,7						; Si el MSB del divisor est� seteado
	neg	 divisor						; cambia el signo del divisor
	sbrc dividendo,7					; Si el MSB del dividendo est� seteado
	neg	 dividendo						; cambia el signo del dividendo, sino saltea la instr
	sub	 resto,resto					; Limpia el resto y el carry
	ldi	 contador,9						; Contador interno

loop_1:	

rol	dividendo							; Desplaza a izquierda el dividendo
dec	contador							; Decrementa contador
brne	loop_2							; Si termin� salta 
sbrc	signo,7							; Si el MSB of del registro de signo est� seteado 
										; saltea la inversi�n de signo

neg	dividendo						    ; A esta altura de la divisi�n "dividendo" almacena
										; el resultado de la divisi�n

st x+,resto								; Se guarda el resto en memoria
mov anterior,resto						; Se carga en"anterior"para la sig iteracion
dec contador1							; decrementa contador de cant de elementos
brne aleatorios							; Evalua si el contador llega a cero

ret										; Si lleg� a cero termin� de generar los 256
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
ldi yl,LOW(vector)						; (En el puntero x se almacenar� el menor y 
ldi yh,HIGH(vector)						;  el puntero y recorrer� el vector)


ldi contador1,6							; Se inicializan contadores
ldi contador2,$00


menor:									; Este loop se encarga de hallar el menor
	inc yl								; de los elementos restantes del vector
	ld r16,x
	ld r17,y							; Se cargan en r16 y r17, los elem apuntados
	cp r17,r16						
		
	brlt ymenor							; Si el apuntado por y es menor salta a"ymenor"
	
	dec contador1						; Decrementa contador y eval�a
	breq switch							; Si lleg� a cero salta a"switch"
jmp menor

ymenor:									
	movw x,y							; Se copia el valor apuntado en x
	dec contador1						; Decrementa contador y eval�a
	breq switch							; Si no lleg� a cero, sig iteracion,
jmp menor								; de lo contrario salta a "switch"


switch:
	ldi yl,LOW(vector)					; Se apunta y al primer elemento y
	ldi yh,HIGH(vector)					; se le suma la contador2 (loop externo)
	add yl,contador2
	
	ld  aux_y,y							; Se almacenan los n�meros apuntados en var aux
	ld  aux_x,x
	st  x,aux_y							; Se intercambian los contenidos
	st  y,aux_x
	
	inc yl								; Se apunta al prox elem y se copia la direcc
	movw x,y							; en x tambi�n
	
	inc contador2						; Se incr contador de loop externo
	ldi contador1,6						; Se vuelve a inicializar contador interno
	sub contador1,contador2				; Se le resta dicho valor
		
	cpi contador2,6						; se eval�a si termin� de ordenar todos los
brne menor								; vectores. Si es as� sale de la rutina, de lo
										; contrario vuelve a "menor"
ret

;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;
;								 FIN DEL PROGRAMA
;<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<;




