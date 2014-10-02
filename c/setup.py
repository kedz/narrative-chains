from distutils.core import setup, Extension
 
module1 = Extension('nchain', sources = ['src/python/nchain.py.c',
                                         'src/corenlp/document.c',
                                         'src/corenlp/xmlparser.c',
                                         'src/cooc/nchains.c'],
                    library_dirs = ['obj/',   '/usr/local/lib'],
                    include_dirs = ['src/', '/usr/local/include/glib-2.0', 
                                    '/usr/local/lib/glib-2.0/include',
                                    '/usr/include/libxml2'],
                    extra_compile_args = ['-std=c99'],
                    libraries = ['glib-2.0', 'z', 'gio-2.0', 'gobject-2.0', 
                                  'xml2'])
#'lz', 'lgio-2.0', 'lgobject-2.0', -lglib-2.0 -lxml2 -lpython2.7   -lm])
 
setup (name = 'PackageName',
        version = '1.0',
        description = 'This is a demo package',
        ext_modules = [module1])
