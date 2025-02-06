
LogicalAudio Suite for VCV Rack 2.
Created by AgostoSound.

The main concept in this stuite is to combine audio processing with propositional logic.

Modules:

• L-Panel:
Just a cool panel.

• L-Carrier:
This module allows the passage of an audio signal throught the six classic logic gates (and, or, xor, nand, nor, xnor) 
deppending on the control inputs A and B.


• L-Random:
Generate four random voltages, two bipolar (L-R) and two unipolar (LR-RR).
The voltages are generated within a range of +1V/-1V around a central voltage determined by the "LvL" knob (Spread). 
In the case of bipolar voltages, the "LvL" is applied to both the positive and negative sides, meaning that with an 
"LvL" set to 3, the voltages will be generated within ranges centered at +3V and -3V, in our example [-4V, -2V] and [2V, 4V].
At each tick (controlled by the "Freq" knob), the voltages are generated anywhere within the allowed ranges.
If the "Just One" switch is activated, the individual knobs are overridden, and both "Freq" and "LvL" are controlled by 
general knobs. Additionally, the values are unified so that they have the same voltage at any given moment; (L-R) are 
equalized on one side and (LR-RR) on the other, but they differ between each pair.


• L-Rantics:
Generate two random voltages within a range of +1V/-1V around a central voltage determined by the "LvL" knob (Spread). 
In the case of bipolar voltages, the "LvL" is applied to both the positive and negative sides, meaning that with an 
"LvL" set to 3, the voltages will be generated within ranges centered at +3V and -3V, in our example [-4V, -2V] and [2V, 4V].
You can select whether the signals are unipolar or bipolar, the switch affects both sides at the same time.
If both Gate inputs are connected, each time one receives a signal it will trigger the voltage change on the same side 
(Gate L controls output L).
If only one Gate input is connected, it controls the changes on both sides.
Vampi generates random ticks on each branch separately.
If the selector is on Gates and none is connected, the output is 0V on both sides.

Cambios pendientes:
(Back)
- OK - El switch de polaridad debe hacer que los voltages se generen de forma unipolar o bipolar (ambos L y R)
- OK - Si ambas Gate inputs están conectadas, cada vez que una reciba señal activará el cambio de voltage del mismo lado (Gate L controla salida L).
- OK - Si solo una Gate input está conectada, controla los cambios de ambos lados.
- OK - Si el selector está en Gates y no hay ninguna conectada, la salida debe ser 0V en ambos lados.
- OK - Vampi genera ticks de forma aleatoria en cada rama por separado.
- Si el input de CV está conectado, anula el LvL y usa el CV.


• L-Filter: (Pendiente)
Un filtro cuya frecuencia de corte y resonancia se controlan mediante operaciones lógicas entre dos entradas (A y B). 
Este módulo permite crear efectos de filtrado dinámicos que cambian en función de condiciones lógicas.
Funcionamiento:

Entradas:
Audio In: Entrada para la señal de audio que se va a filtrar.
A y B: Entradas lógicas que controlan el comportamiento del filtro.
CV In: Una entrada opcional de CV para modular manualmente la frecuencia de corte.

Salidas:
Audio Out: Salida de la señal filtrada.
Env Out: Una salida de envolvente que sigue los cambios en la frecuencia de corte (útil para modular otros módulos).

Controles:
Tipo de filtro: Un selector para elegir entre filtro paso bajo, paso alto, paso banda o rechazo de banda.
Operación lógica: Un selector para elegir la operación lógica (AND, OR, XOR, etc.) que controla la frecuencia de corte.
Resonancia: Un potenciómetro para ajustar la resonancia del filtro.
Rango de frecuencia: Un control para ajustar el rango base de la frecuencia de corte.

Cómo funciona:
La frecuencia de corte del filtro se modula en función de las entradas lógicas (A y B):
Si eliges la operación AND, la frecuencia de corte solo cambiará cuando ambas entradas estén activas.
Si eliges la operación OR, la frecuencia de corte cambiará cuando al menos una de las entradas esté activa.
Si eliges la operación XOR, la frecuencia de corte cambiará solo cuando una de las entradas esté activa, pero no ambas.
La resonancia y el tipo de filtro permiten ajustar el carácter del sonido, desde suave y cálido hasta agresivo y resonante.


L-Noise: (Pendiente)
Generador que produce diferentes tipos de ruido (blanco, rosa, marrón) y permite modular sus parámetros (amplitud, filtrado 
y panorámica) en función de operaciones lógicas entre dos entradas (A y B).

Entradas:
A y B: Entradas lógicas que controlan la modulación de los parámetros del ruido.
CV In: Una entrada opcional de CV para modular manualmente los parámetros.

Salidas:
Noise Out: Salida de la señal de ruido procesada.
Env Out: Una salida de envolvente que sigue los cambios en la amplitud del ruido (útil para modular otros módulos).

Controles:
Tipo de ruido: Un selector para elegir entre ruido blanco, rosa o marrón.
Operación lógica: Un selector para elegir la operación lógica (AND, OR, XOR, etc.) que controla la modulación.
Amplitud: Un potenciómetro para ajustar el nivel de salida del ruido.

Filtrado: 
Un control para ajustar la frecuencia de corte de un filtro paso bajo aplicado al ruido.
Panorámica: Un control para ajustar la posición estéreo del ruido.
