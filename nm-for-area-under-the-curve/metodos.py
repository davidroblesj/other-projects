import math
import sympy

max = 0
def biseccion(a,b,f):
    x = sympy.Symbol('x')
    f = sympy.lambdify(x, f)
    p = (a + b)/2
    i = 0
    for i in range(max):
        if f(p)==0:
            break
        if f(p)*f(a)<0:
            b = p
        else:
            a = p
        p = (a + b)/2
    return [p,f(p),i]
   

def falsa_posicion(a,b,f):
    x = sympy.Symbol('x')
    f = sympy.lambdify(x, f)
    p = (a*f(b) - b*f(a))/(f(b)-f(a))
    i = 0
    for i in range(max):
        if f(p)==0:
            break
        if f(p)*f(a)<0:
            b = p
        else:
            a = p
        p = (a*f(b) - b*f(a))/(f(b) - f(a))
    return [p,f(p),i]

def newton(x,f):
    x_0 = x
    x = sympy.Symbol('x')
    f_prime = f.diff(x)
    f = sympy.lambdify(x, f)
    f_prime = sympy.lambdify(x, f_prime)
    i = 0
    p = x_0 - (f(x_0))/(f_prime(x_0))

    for i in range(max):
        if f(p)==0 or f_prime(p)==0:
            break
        p = p -(f(p))/(f_prime(p))       
    return [p,f(p),i]
def secante(a,b,f):
    x = sympy.Symbol('x')
    f = sympy.lambdify(x, f)
    x_n = b - ((b - a)*f(b))/(f(b)-f(a))
    i = 0
    for i in range(max):
        if f(x_n)==0:
            break
        a = b
        b = x_n
        if f(b)-f(a)!=0:
            x_n = b - ((b - a)*f(b))/(f(b)-f(a))
        else:
            break
    return [x_n,f(x_n),i]


def main():
    funcion = str(input('Introduce la funcion: Ejemplo: x**3-3*x+1.\n'))
    a = int(input('Introduce a: '))
    b = int(input('Introduce b: '))
    x_1 = int(input('Introduce x_1: '))
    global max 
    max = int(input('Iteraciones maximas: '))
    func = sympy.sympify(funcion)
    x = sympy.Symbol('x')
    f = sympy.lambdify(x, func)
    if f(a)*f(b)<0:
        p = biseccion(a,b,func)
        print('biseccion:'+'p:',p[0],'f(p)',p[1],'iteraciones:',p[2])
        p = falsa_posicion(a,b,func)
        print('falsa_posicion:'+'p:',p[0],'f(p)',p[1],'iteraciones:',p[2])
        p = newton(x_1,func)
        print('newton:'+'p:',p[0],'f(p)',p[1],'iteraciones:',p[2])
        p = secante(a,b,func)
        print('secante:'+'p:',p[0],'f(p)',p[1],'iteraciones:',p[2])
    else:
        raise Exception('Introduce un intervalo válido')


main()
