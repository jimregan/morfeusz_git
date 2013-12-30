# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.4
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_morfeusz', [dirname(__file__)])
        except ImportError:
            import _morfeusz
            return _morfeusz
        if fp is not None:
            try:
                _mod = imp.load_module('_morfeusz', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _morfeusz = swig_import_helper()
    del swig_import_helper
else:
    import _morfeusz
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class SwigPyIterator(_object):
    """Proxy of C++ swig::SwigPyIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, SwigPyIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, SwigPyIterator, name)
    def __init__(self, *args, **kwargs): raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _morfeusz.delete_SwigPyIterator
    __del__ = lambda self : None;
    def value(self):
        """
        value(self) -> PyObject
        Parameters:
           self: swig::SwigPyIterator const * value




        """
        return _morfeusz.SwigPyIterator_value(self)

    def incr(self, n = 1):
        """
        incr(self, n = 1) -> SwigPyIterator
        Parameters:
           n: size_t value

           n: size_t value

           n: size_t value

           n: size_t value

        incr(self) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator * value


        """
        return _morfeusz.SwigPyIterator_incr(self, n)

    def decr(self, n = 1):
        """
        decr(self, n = 1) -> SwigPyIterator
        Parameters:
           n: size_t value

           n: size_t value

           n: size_t value

           n: size_t value

        decr(self) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator * value


        """
        return _morfeusz.SwigPyIterator_decr(self, n)

    def distance(self, *args):
        """
        distance(self, x) -> ptrdiff_t
        Parameters:
           self: swig::SwigPyIterator const * value
           x: swig::SwigPyIterator const & value

           x: swig::SwigPyIterator const & value

        """
        return _morfeusz.SwigPyIterator_distance(self, *args)

    def equal(self, *args):
        """
        equal(self, x) -> bool
        Parameters:
           self: swig::SwigPyIterator const * value
           x: swig::SwigPyIterator const & value

           x: swig::SwigPyIterator const & value

        """
        return _morfeusz.SwigPyIterator_equal(self, *args)

    def copy(self):
        """
        copy(self) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator const * value




        """
        return _morfeusz.SwigPyIterator_copy(self)

    def next(self):
        """
        next(self) -> PyObject
        Parameters:
           self: swig::SwigPyIterator * value




        """
        return _morfeusz.SwigPyIterator_next(self)

    def __next__(self):
        """
        __next__(self) -> PyObject
        Parameters:
           self: swig::SwigPyIterator * value




        """
        return _morfeusz.SwigPyIterator___next__(self)

    def previous(self):
        """
        previous(self) -> PyObject
        Parameters:
           self: swig::SwigPyIterator * value




        """
        return _morfeusz.SwigPyIterator_previous(self)

    def advance(self, *args):
        """
        advance(self, n) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator * value
           n: ptrdiff_t value

           n: ptrdiff_t value

        """
        return _morfeusz.SwigPyIterator_advance(self, *args)

    def __eq__(self, *args):
        """
        __eq__(self, x) -> bool
        Parameters:
           self: swig::SwigPyIterator const * value
           x: swig::SwigPyIterator const & value

           x: swig::SwigPyIterator const & value

        """
        return _morfeusz.SwigPyIterator___eq__(self, *args)

    def __ne__(self, *args):
        """
        __ne__(self, x) -> bool
        Parameters:
           self: swig::SwigPyIterator const * value
           x: swig::SwigPyIterator const & value

           x: swig::SwigPyIterator const & value

        """
        return _morfeusz.SwigPyIterator___ne__(self, *args)

    def __iadd__(self, *args):
        """
        __iadd__(self, n) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator * value
           n: ptrdiff_t value

           n: ptrdiff_t value

        """
        return _morfeusz.SwigPyIterator___iadd__(self, *args)

    def __isub__(self, *args):
        """
        __isub__(self, n) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator * value
           n: ptrdiff_t value

           n: ptrdiff_t value

        """
        return _morfeusz.SwigPyIterator___isub__(self, *args)

    def __add__(self, *args):
        """
        __add__(self, n) -> SwigPyIterator
        Parameters:
           self: swig::SwigPyIterator const * value
           n: ptrdiff_t value

           n: ptrdiff_t value

        """
        return _morfeusz.SwigPyIterator___add__(self, *args)

    def __sub__(self, *args):
        """
        __sub__(self, n) -> SwigPyIterator
        Parameters:
           n: ptrdiff_t value

           n: ptrdiff_t value

        __sub__(self, x) -> ptrdiff_t
        Parameters:
           self: swig::SwigPyIterator const * value
           x: swig::SwigPyIterator const & value

           x: swig::SwigPyIterator const & value

        """
        return _morfeusz.SwigPyIterator___sub__(self, *args)

    def __iter__(self): return self
SwigPyIterator_swigregister = _morfeusz.SwigPyIterator_swigregister
SwigPyIterator_swigregister(SwigPyIterator)

class MorfeuszException(_object):
    """Proxy of C++ MorfeuszException class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MorfeuszException, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MorfeuszException, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self, what) -> MorfeuszException
        Parameters:
           what: std::string const & value

           what: std::string const & value

        """
        this = _morfeusz.new_MorfeuszException(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _morfeusz.delete_MorfeuszException
    __del__ = lambda self : None;
    def what(self):
        """
        what(self) -> char
        Parameters:
           self: MorfeuszException const * value




        """
        return _morfeusz.MorfeuszException_what(self)

MorfeuszException_swigregister = _morfeusz.MorfeuszException_swigregister
MorfeuszException_swigregister(MorfeuszException)

class Morfeusz(_object):
    """Proxy of C++ Morfeusz class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Morfeusz, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Morfeusz, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        """
        __init__(self) -> Morfeusz
        __init__(self, filename) -> Morfeusz
        Parameters:
           filename: std::string const & value

           filename: std::string const & value

        """
        this = _morfeusz.new_Morfeusz(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _morfeusz.delete_Morfeusz
    __del__ = lambda self : None;
    def analyze(self, *args):
        """
        analyze(self, text) -> ResultsIterator
        Parameters:
           text: std::string const & value

           text: std::string const & value

        analyze(self, text, result)
        Parameters:
           self: Morfeusz const * value
           text: std::string const & value
           result: std::vector< MorphInterpretation,std::allocator< MorphInterpretation > > & value

           text: std::string const & value
           result: std::vector< MorphInterpretation,std::allocator< MorphInterpretation > > & value

        """
        return _morfeusz.Morfeusz_analyze(self, *args)

    def setEncoding(self, *args):
        """
        setEncoding(self, encoding)
        Parameters:
           self: Morfeusz * value
           encoding: enum MorfeuszCharset value

           encoding: enum MorfeuszCharset value

        """
        return _morfeusz.Morfeusz_setEncoding(self, *args)

Morfeusz_swigregister = _morfeusz.Morfeusz_swigregister
Morfeusz_swigregister(Morfeusz)

class ResultsIterator(_object):
    """Proxy of C++ ResultsIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ResultsIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ResultsIterator, name)
    def __init__(self, *args, **kwargs): raise AttributeError("No constructor defined")
    __repr__ = _swig_repr
    def getNext(self):
        """
        getNext(self) -> MorphInterpretation
        Parameters:
           self: ResultsIterator * value




        """
        return _morfeusz.ResultsIterator_getNext(self)

    def hasNext(self):
        """
        hasNext(self) -> bool
        Parameters:
           self: ResultsIterator * value




        """
        return _morfeusz.ResultsIterator_hasNext(self)

    __swig_destroy__ = _morfeusz.delete_ResultsIterator
    __del__ = lambda self : None;
ResultsIterator_swigregister = _morfeusz.ResultsIterator_swigregister
ResultsIterator_swigregister(ResultsIterator)

class MorphInterpretation(_object):
    """Proxy of C++ MorphInterpretation class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MorphInterpretation, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MorphInterpretation, name)
    def __init__(self, *args, **kwargs): raise AttributeError("No constructor defined")
    __repr__ = _swig_repr
    def getStartNode(self):
        """
        getStartNode(self) -> int
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getStartNode(self)

    def getEndNode(self):
        """
        getEndNode(self) -> int
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getEndNode(self)

    def getOrth(self):
        """
        getOrth(self) -> string
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getOrth(self)

    def getLemma(self):
        """
        getLemma(self) -> string
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getLemma(self)

    def getTagnum(self):
        """
        getTagnum(self) -> int
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getTagnum(self)

    def getNamenum(self):
        """
        getNamenum(self) -> int
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getNamenum(self)

    def getTag(self):
        """
        getTag(self) -> string
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getTag(self)

    def getName(self):
        """
        getName(self) -> string
        Parameters:
           self: MorphInterpretation const * value




        """
        return _morfeusz.MorphInterpretation_getName(self)

    __swig_destroy__ = _morfeusz.delete_MorphInterpretation
    __del__ = lambda self : None;
MorphInterpretation_swigregister = _morfeusz.MorphInterpretation_swigregister
MorphInterpretation_swigregister(MorphInterpretation)

UTF8 = _morfeusz.UTF8
ISO8859_2 = _morfeusz.ISO8859_2
CP1250 = _morfeusz.CP1250
CP852 = _morfeusz.CP852
class InterpsVector(_object):
    """Proxy of C++ std::vector<(MorphInterpretation)> class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, InterpsVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, InterpsVector, name)
    __repr__ = _swig_repr
    def iterator(self):
        """
        iterator(self) -> SwigPyIterator
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_iterator(self)

    def __iter__(self): return self.iterator()
    def __nonzero__(self):
        """
        __nonzero__(self) -> bool
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector___nonzero__(self)

    def __bool__(self):
        """
        __bool__(self) -> bool
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector___bool__(self)

    def __len__(self):
        """
        __len__(self) -> size_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector___len__(self)

    def pop(self):
        """
        pop(self) -> value_type
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_pop(self)

    def __getslice__(self, *args):
        """
        __getslice__(self, i, j) -> InterpsVector
        Parameters:
           self: std::vector< MorphInterpretation > * value
           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value

           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value

        """
        return _morfeusz.InterpsVector___getslice__(self, *args)

    def __setslice__(self, *args):
        """
        __setslice__(self, i, j, v = std::vector< MorphInterpretation,std::allocator< MorphInterpretation > >())
        Parameters:
           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value
           v: std::vector< MorphInterpretation,std::allocator< MorphInterpretation > > const & value

           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value
           v: std::vector< MorphInterpretation,std::allocator< MorphInterpretation > > const & value

        __setslice__(self, i, j)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value

           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value

        """
        return _morfeusz.InterpsVector___setslice__(self, *args)

    def __delslice__(self, *args):
        """
        __delslice__(self, i, j)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value

           i: std::vector< MorphInterpretation >::difference_type value
           j: std::vector< MorphInterpretation >::difference_type value

        """
        return _morfeusz.InterpsVector___delslice__(self, *args)

    def __delitem__(self, *args):
        """
        __delitem__(self, i)
        Parameters:
           i: std::vector< MorphInterpretation >::difference_type value

           i: std::vector< MorphInterpretation >::difference_type value

        __delitem__(self, slice)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           slice: PySliceObject * value

           slice: PySliceObject * value

        """
        return _morfeusz.InterpsVector___delitem__(self, *args)

    def __getitem__(self, *args):
        """
        __getitem__(self, slice) -> InterpsVector
        Parameters:
           slice: PySliceObject * value

           slice: PySliceObject * value

        __getitem__(self, i) -> value_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value
           i: std::vector< MorphInterpretation >::difference_type value

           i: std::vector< MorphInterpretation >::difference_type value

        """
        return _morfeusz.InterpsVector___getitem__(self, *args)

    def __setitem__(self, *args):
        """
        __setitem__(self, slice, v)
        Parameters:
           slice: PySliceObject * value
           v: std::vector< MorphInterpretation,std::allocator< MorphInterpretation > > const & value

           slice: PySliceObject * value
           v: std::vector< MorphInterpretation,std::allocator< MorphInterpretation > > const & value

        __setitem__(self, slice)
        Parameters:
           slice: PySliceObject * value

           slice: PySliceObject * value

        __setitem__(self, i, x)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           i: std::vector< MorphInterpretation >::difference_type value
           x: std::vector< MorphInterpretation >::value_type const & value

           i: std::vector< MorphInterpretation >::difference_type value
           x: std::vector< MorphInterpretation >::value_type const & value

        """
        return _morfeusz.InterpsVector___setitem__(self, *args)

    def append(self, *args):
        """
        append(self, x)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           x: std::vector< MorphInterpretation >::value_type const & value

           x: std::vector< MorphInterpretation >::value_type const & value

        """
        return _morfeusz.InterpsVector_append(self, *args)

    def empty(self):
        """
        empty(self) -> bool
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector_empty(self)

    def size(self):
        """
        size(self) -> size_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector_size(self)

    def clear(self):
        """
        clear(self)
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_clear(self)

    def swap(self, *args):
        """
        swap(self, v)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           v: std::vector< MorphInterpretation > & value

           v: std::vector< MorphInterpretation > & value

        """
        return _morfeusz.InterpsVector_swap(self, *args)

    def get_allocator(self):
        """
        get_allocator(self) -> allocator_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector_get_allocator(self)

    def begin(self):
        """
        begin(self) -> iterator
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_begin(self)

    def end(self):
        """
        end(self) -> iterator
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_end(self)

    def rbegin(self):
        """
        rbegin(self) -> reverse_iterator
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_rbegin(self)

    def rend(self):
        """
        rend(self) -> reverse_iterator
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_rend(self)

    def pop_back(self):
        """
        pop_back(self)
        Parameters:
           self: std::vector< MorphInterpretation > * value




        """
        return _morfeusz.InterpsVector_pop_back(self)

    def erase(self, *args):
        """
        erase(self, pos) -> iterator
        Parameters:
           pos: std::vector< MorphInterpretation >::iterator value

           pos: std::vector< MorphInterpretation >::iterator value

        erase(self, first, last) -> iterator
        Parameters:
           self: std::vector< MorphInterpretation > * value
           first: std::vector< MorphInterpretation >::iterator value
           last: std::vector< MorphInterpretation >::iterator value

           first: std::vector< MorphInterpretation >::iterator value
           last: std::vector< MorphInterpretation >::iterator value

        """
        return _morfeusz.InterpsVector_erase(self, *args)

    def __init__(self, *args): 
        """
        __init__(self) -> InterpsVector
        __init__(self, arg0) -> InterpsVector
        Parameters:
           : std::vector< MorphInterpretation > const & value

           : std::vector< MorphInterpretation > const & value

        __init__(self, size, value) -> InterpsVector
        Parameters:
           size: std::vector< MorphInterpretation >::size_type value
           value: std::vector< MorphInterpretation >::value_type const & value

           size: std::vector< MorphInterpretation >::size_type value
           value: std::vector< MorphInterpretation >::value_type const & value

        """
        this = _morfeusz.new_InterpsVector(*args)
        try: self.this.append(this)
        except: self.this = this
    def push_back(self, *args):
        """
        push_back(self, x)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           x: std::vector< MorphInterpretation >::value_type const & value

           x: std::vector< MorphInterpretation >::value_type const & value

        """
        return _morfeusz.InterpsVector_push_back(self, *args)

    def front(self):
        """
        front(self) -> value_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector_front(self)

    def back(self):
        """
        back(self) -> value_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector_back(self)

    def assign(self, *args):
        """
        assign(self, n, x)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           n: std::vector< MorphInterpretation >::size_type value
           x: std::vector< MorphInterpretation >::value_type const & value

           n: std::vector< MorphInterpretation >::size_type value
           x: std::vector< MorphInterpretation >::value_type const & value

        """
        return _morfeusz.InterpsVector_assign(self, *args)

    def insert(self, *args):
        """
        insert(self, pos, x) -> iterator
        Parameters:
           pos: std::vector< MorphInterpretation >::iterator value
           x: std::vector< MorphInterpretation >::value_type const & value

           pos: std::vector< MorphInterpretation >::iterator value
           x: std::vector< MorphInterpretation >::value_type const & value

        insert(self, pos, n, x)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           pos: std::vector< MorphInterpretation >::iterator value
           n: std::vector< MorphInterpretation >::size_type value
           x: std::vector< MorphInterpretation >::value_type const & value

           pos: std::vector< MorphInterpretation >::iterator value
           n: std::vector< MorphInterpretation >::size_type value
           x: std::vector< MorphInterpretation >::value_type const & value

        """
        return _morfeusz.InterpsVector_insert(self, *args)

    def reserve(self, *args):
        """
        reserve(self, n)
        Parameters:
           self: std::vector< MorphInterpretation > * value
           n: std::vector< MorphInterpretation >::size_type value

           n: std::vector< MorphInterpretation >::size_type value

        """
        return _morfeusz.InterpsVector_reserve(self, *args)

    def capacity(self):
        """
        capacity(self) -> size_type
        Parameters:
           self: std::vector< MorphInterpretation > const * value




        """
        return _morfeusz.InterpsVector_capacity(self)

    __swig_destroy__ = _morfeusz.delete_InterpsVector
    __del__ = lambda self : None;
InterpsVector_swigregister = _morfeusz.InterpsVector_swigregister
InterpsVector_swigregister(InterpsVector)
cvar = _morfeusz.cvar
DEFAULT_MORFEUSZ_CHARSET = cvar.DEFAULT_MORFEUSZ_CHARSET

def analyze(self, text):
  res = InterpsVector()
  _morfeusz.Morfeusz_analyze(self, text, res)
  return list(res)

Morfeusz.analyze = analyze

def getOrth(self):
  return _morfeusz.MorphInterpretation_getOrth(self).decode('utf8')

def getLemma(self):
  return _morfeusz.MorphInterpretation_getLemma(self).decode('utf8')

def getTag(self):
  return _morfeusz.MorphInterpretation_getTag(self).decode('utf8')

def getName(self):
  return _morfeusz.MorphInterpretation_getName(self).decode('utf8')

MorphInterpretation.getOrth = getOrth
MorphInterpretation.getLemma = getLemma
MorphInterpretation.getTag = getTag
MorphInterpretation.getName = getName

# This file is compatible with both classic and new-style classes.


