# Proyecto Breakout

Estructura base del proyecto en C++ con hilos (Pthreads).

# Estilo Visual del Juego Breakout

## Descripción General del Estilo Visual

El juego utiliza gráficos ASCII para representar todos los elementos. La interfaz se compone de caracteres de texto simples que forman los componentes del juego.

### Elementos Visuales:
- **`=====`** - Paleta del jugador
- **`O`** - Pelota
- **`#`** - Bloques
- **`|`** - Paredes laterales  
- **`-`** - Pared superior
- **Espacios en blanco** - Área de juego vacía

## Justificación de Decisiones Visuales

### 1. **Uso de ASCII simple**
- Los caracteres elegidos son fácilmente reconocibles
- Funcionan en cualquier terminal sin requerir librerías gráficas
- El rendimiento es óptimo incluso en hardware limitado

### 2. **Diseño funcional**
- La paleta `=====` es suficientemente ancha para interactuar con la pelota
- Los bloques `#` son visibles y se distinguen claramente
- Los bordes `|` y `-` definen claramente los límites del juego