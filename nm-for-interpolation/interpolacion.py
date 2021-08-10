import sympy
import math

def lagrange(X,Y):
    funcion = ''
    polinomio = ''
    contador = 0
    for i in range(len(Y)):
        for j in range(len(X)):
            if j == i:
                continue
            if contador==len(X)-2:
                funcion = funcion +'(x-'+str(X[j])+')/('+str(X[i])+'-'+str(X[j])+')'
            else:
                funcion = funcion +'(x-'+str(X[j])+')/('+str(X[i])+'-'+str(X[j])+')' + '*'
            contador +=1
        contador = 0
        if i != len(Y)-1:
            polinomio = polinomio + funcion + '*' + Y[i] + '+'
        else:
            polinomio = polinomio + funcion + '*' + Y[i]

        funcion = ''

    print('lagrange: ',polinomio)

    pol = sympy.sympify(polinomio)
    print('lagrange simplificado: ',sympy.simplify(pol))

def newton(X,Y):
    n = len(X)
    polinomio = ''
    multiplicacion = ''
    for i in range(n):
        for j in range(i):
            multiplicacion = multiplicacion + '*'+'(x-'+str(X[j])+')'
        if i != n-1:
            polinomio = polinomio + 'a_'+str(i)+multiplicacion + '+'
        else:
            polinomio = polinomio + 'a_'+str(i)+multiplicacion
        multiplicacion = ''

    return polinomio

def dif_divididas(X,Y):
    n = len(X) 
    a = []
    for i in range(n):
        a.append([])
    for i in range(n):
        a[0].append(Y[i])
    i = 0

    for j in range(1,n):
        for i in range(n-j):
            f = '('+str(a[j-1][i+1])+'-'+str(a[j-1][i])+')/('+str(X[i+j])+'-'+str(X[i])+')'
            a[j].insert(i,f)
    polinomio = newton(X,Y)
    print('newton:',polinomio)
    for b in range(n):
        pol = sympy.sympify(a[b][0])
        simplificado = str(sympy.simplify(pol))
        polinomio = polinomio.replace('a_'+str(b),simplificado)

    print('dif divididas:',polinomio)
 



def main():
    X = ['1','3/2','0','2']
    Y = ['3','13/4','3','5/3']
    #X = ['0','1','-1','2','-2']
    #Y = ['-5','-3','-15','39','-9']
    #X = input('Introduce los valores de x')
    #Y = input('Introduce los valores de y')
    #X = X.split(',')
    #Y = Y.split(',')
    print('X: ',X)
    print('Y: ',Y)

    lagrange(X,Y)
    newton(X,Y)
    dif_divididas(X,Y)


main()