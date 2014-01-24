'''
Created on 24 sty 2014

@author: mlenart
'''

import preprocessor

if __name__ == '__main__':
    text = '''
dupa
#define asd XXX
#define X(x) a x b
#define Y(x) X(x) c
#define B(x) X(x)
#define Z(x) Y(X(x)) d
#define AB(asd) dupa asd dupa
Y(Z(a) b X(c) Y(d))
#ifdef extra
asdfasa
#ifdef extra
asdfasdfasdfa
#endif
#ifdef superextra
aaaa asd
#endif
#endif

#ifdef superextra
asdfasdfada
#endif

AB(x)
'''
    for line in preprocessor.preprocess(text.split('\n'), ['extra', 'superextra']):
        print line