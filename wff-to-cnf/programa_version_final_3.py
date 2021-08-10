def distr(f1, f2):

    if f1[nodo_raiz(f1)] == '&':
        return ('('+distr(segment(f1, nodo_raiz(f1))[0], f2)+'&'+distr(segment(f1, nodo_raiz(f1))[1], f2)+')')
    if f2[nodo_raiz(f2)] == '&':
        return ('('+distr(f1, segment(f2, nodo_raiz(f2))[0])+'&'+distr(f1, segment(f2, nodo_raiz(f2))[1])+')')
    else:
        return ('('+f1+'|'+f2+')')


def cnf(fbf):
    if len(fbf) < 5:
        return fbf
    elif fbf[nodo_raiz(fbf)] == '&':
        return ('('+cnf(segment(fbf, nodo_raiz(fbf))[0])+'&'+cnf(segment(fbf, nodo_raiz(fbf))[1])+')')
    elif fbf[nodo_raiz(fbf)] == '|':
        return (distr(cnf(segment(fbf, nodo_raiz(fbf))[0]), cnf(segment(fbf, nodo_raiz(fbf))[1])))


def nodo_raiz(formula):
    contador = 0
    for i in range(0, len(formula)):
        if formula[i] == '(':
            contador = contador + 1
        if formula[i] == ')':
            contador = contador - 1
        if contador == 1 and (formula[i] == '|' or formula[i] == '&' or formula[i] == '>' or formula[i] == '~'):
            break
    return i


def impl_free(fbf):
    if len(fbf) < 5:
        fwi = fbf
    elif fbf[nodo_raiz(fbf)] == '>':
        fwi = ('((~' + impl_free(segment(fbf, nodo_raiz(fbf))
                                 [0]) + ')|' + impl_free(segment(fbf, nodo_raiz(fbf))[1]) + ')')
    elif fbf[nodo_raiz(fbf)] == '&':
        fwi = ('(' + impl_free(segment(fbf, nodo_raiz(fbf))
                               [0]) + '&' + impl_free(segment(fbf, nodo_raiz(fbf))[1]) + ')')
    elif fbf[nodo_raiz(fbf)] == '|':
        fwi = ('(' + impl_free(segment(fbf, nodo_raiz(fbf))
                               [0]) + '|' + impl_free(segment(fbf, nodo_raiz(fbf))[1]) + ')')
    elif fbf[nodo_raiz(fbf)] == '~':
        fwi = ('(~' + impl_free(segment(fbf, nodo_raiz(fbf))[1])+')')
    else:
        fwi = fbf
    return fwi


def segment(fbf, indice):
    if len(fbf) < 5:
        return fbf
    else:
        nfbf = [fbf[1:indice], fbf[indice+1:-1]]
        return nfbf


def nnf(fbf):
    if len(fbf) < 5:
        return fbf
    elif fbf[nodo_raiz(fbf)] == '~' and fbf[nodo_raiz(fbf) + 2] == '~' and fbf[nodo_raiz(fbf) + 3] == '(':
        return nnf(segment(fbf, nodo_raiz(fbf))[1][2:len(fbf)])
    elif fbf[nodo_raiz(fbf)] == '~' and fbf[nodo_raiz(fbf)+2] == '~':
        return nnf(segment(fbf, nodo_raiz(fbf))[1][2])
    elif fbf[nodo_raiz(fbf)] == '&':
        return ('('+nnf(segment(fbf, nodo_raiz(fbf))[0])+'&'+nnf(segment(fbf, nodo_raiz(fbf))[1])+')')
    elif fbf[nodo_raiz(fbf)] == '|':
        return ('('+nnf(segment(fbf, nodo_raiz(fbf))[0])+'|'+nnf(segment(fbf, nodo_raiz(fbf))[1])+')')
    elif fbf[nodo_raiz(fbf)] == '~' and fbf[nodo_raiz(fbf) + 1] == '(':
        fnf = segment(fbf, nodo_raiz(fbf))[1]
        if fnf[nodo_raiz(fnf)] == '&':
            return ('('+nnf('(~'+segment(fnf, nodo_raiz(fnf))[0]+')')+'|'+nnf('(~'+segment(fnf, nodo_raiz(fnf))[1]+')')+')')
        elif fnf[nodo_raiz(fnf)] == '|':
            return ('('+nnf('(~'+segment(fnf, nodo_raiz(fnf))[0]+')')+'&'+nnf('(~'+segment(fnf, nodo_raiz(fnf))[1]+')')+')')


fbf = input("Introduzca una formula bien formada: \nEjemplo: (~(p&(q>r)))\n")

cont = 0

for i in range(len(fbf)):
    if fbf[i] == '(':
        cont = cont + 1
    if fbf[i] == ')':
        cont = cont - 1

if cont == 0:

    print("Formula Original:", fbf)

    fbf = cnf(nnf(impl_free(fbf)))

    print("Formula en su Forma Normal Conjunta:", fbf)

else:
    print("Su formula no esta bien formada")
