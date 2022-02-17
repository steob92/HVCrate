from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext


setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [Extension('pyHVCrate',
                             sources=['./src/pyHVCrate.pyx'],
                             include_dirs=['./include/', "/usr/include/"],
                             extra_compile_args=['-std=c++11', "-DUNIX", "-DLINUX", "-pthread","-g", "-ldl","-lm","-lcaenhvwrapper", "-w", "-fPIC", "-static"],
                             libraries=["caenhvwrapper"],
                             language='c++')]
)
