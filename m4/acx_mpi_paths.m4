# Code from LAM-MPI mailing lists archive.
AC_DEFUN([ACX_MPI_PATHS], [

AC_ARG_WITH(mpi,
            [  --with-mpi=MPIPATH        Build against MPI library (enabled)], ,
            with_mpi="yes")

#MPI_CFLAGS=
#MPI_CXXFLAGS=
#MPI_LIBS=
MPI_ENABLED=0

if test "${with_mpi}" != "no"
then

    MPI_STATUS=

    # ok, so now we actually have to find the MPI implementation.
    # yippie
    if test "${with_mpi}" = "yes"
    then
       MPIPATH="${PATH}"
    else
       MPIPATH="${with_mpi}/bin"
       MPI_CFLAGS="-I${with_mpi}/include ${MPI_CFLAGS}"
       MPI_CXXFLAGS="-I${with_mpi}/include ${MPI_CXXFLAGS}"
       MPI_LIBS="-L${with_mpi}/lib ${MPI_LIBS}"
    fi

    AC_PATH_PROGS(MPICC, mpicc hcc mpcc mpcc_r mpxlc, none, ${MPIPATH})
    AC_PATH_PROGS(MPICXX, mpic++ mpiCC mpCC hcp, none, ${MPIPATH})

    AC_MSG_CHECKING([for MPI wrapper compilers])
    if test ! "${MPICC}" = "none" -a ! "${MPICXX}" = "none"
    then
       AC_MSG_RESULT([found])
       # ok, so we have a wrapper compiler...
       AC_MSG_CHECKING([for MPI compiler style])
       if test ! "`${MPICC} -showme 2>&1 | grep lmpi`" = ""
       then
          # when used with showme, LAM doesn't actually do anything
          # so the files referenced don't have to exist...
          AC_MSG_RESULT([LAM])

          mpi_c_compile="`${MPICC} -showme -c USELESS.c`"
          mpi_cxx_compile="`${MPICXX} -showme -c USELESS.cc`"
          mpi_link="`${MPICC} -showme USELESS.o -o USELESS`"

          # remove compiler name
          mpi_c_compile="`echo ${mpi_c_compile} | cut -d' ' -f2-`"
          mpi_cxx_compile="`echo ${mpi_cxx_compile} | cut -d' ' -f2-`"
          mpi_link="`echo ${mpi_link} | cut -d' ' -f2-`"

          # remove the silly -c UESLESS.c / USELESS.o -o USELESS
          mpi_c_compile="`echo ${mpi_c_compile} | sed -e \"s/-c USELESS.c//g\"`"
          mpi_cxx_compile="`echo ${mpi_cxx_compile} | sed -e \"s/-c USELESS.cc//g\"`"
          mpi_link="`echo ${mpi_link} | sed -e \"s/USELESS.o -o USELESS//g\"`"

          # remove -I from link - the @<:@ and @:>@ become [ and ] when m4sh is done
          mpi_link="`echo ${mpi_link} | sed -e \"s/\-I@<:@^ @:>@*//g\"`"

          MPI_CFLAGS="${mpi_c_compile}"
          MPI_CXXFLAGS="${mpi_cxx_compile}"
          MPI_LIBS="${mpi_link}"
          MPI_STATUS="using wrapper flags"

       elif test ! "`${MPICC} -show 2>&1 | grep lmpi`" = ""
       then
          AC_MSG_RESULT([MPICH])

          mpi_c_compile="`${MPICC} -compile_info`"
          mpi_cxx_compile="`${MPICXX} -compile_info`"
          mpi_link="`${MPICC} -link_info`"

          # remove compiler name
          mpi_c_compile="`echo ${mpi_c_compile} | cut -d' ' -f2-`"
          mpi_cxx_compile="`echo ${mpi_cxx_compile} | cut -d' ' -f2-`"
          mpi_link="`echo ${mpi_link} | cut -d' ' -f2-`"

          # remove the silly -c
          mpi_c_compile="`echo ${mpi_c_compile} | sed -e \"s/ -c / /g\"`"
          mpi_cxx_compile="`echo ${mpi_cxx_compile} | sed -e \"s/ -c / /g\"`"

          MPI_CFLAGS="${mpi_c_compile}"
          MPI_CXXFLAGS="${mpi_cxx_compile}"
          MPI_LIBS="${mpi_link}"
          MPI_STATUS="using wrapper flags"

       else
          AC_MSG_RESULT([unknown])
          AC_MSG_WARN([Unknown wrapper compiler style - trying dumb libraries])
       fi
    else
       AC_MSG_RESULT([none found])
    fi

    if test "${MPI_STATUS}" = ""
    then
       # well, we don't have anything yet.  Let's see if life
       # "just works"
       AC_CHECK_FUNC(MPI_Init, [MPI_STATUS="in standard libs"])
    fi
    if test "${MPI_STATUS}" = ""
    then
       # try -lmpi
       AC_CHECK_LIB(mpi, MPI_Init, [MPI_LIBS="${MPI_LIBS} -lmpi" ; MPI_STATUS="-lmpi"])
    fi
    if test "${MPI_STATUS}" = ""
    then
       # try -lmpich
       AC_CHECK_LIB(mpich, MPI_Init, [MPI_LIBS="${MPI_LIBS} -lmpich" ; MPI_STATUS="-lmpich"])
    fi

    AC_MSG_CHECKING([for mpi.h])
    orig_CFLAGS="${CFLAGS}"
    orig_LIBS="${LIBS}"
    CFLAGS="${MPI_CFLAGS} ${CFLAGS}"
    LIBS="${MPI_LIBS} ${LIBS}"
    AC_TRY_COMPILE([#include <mpi.h>],[],
                   [AC_MSG_RESULT(yes)],
                   [AC_MSG_RESULT(no) ])
    CFLAGS="${orig_CFLAGS}"
    LIBS="${orig_LIBS}"

    if test "${MPI_STATUS}" = ""
    then
       AC_MSG_ERROR([Could not find working MPI implementation - aborting])
    fi

    AC_MSG_CHECKING([for MPI libraries])
    AC_MSG_RESULT([$MPI_STATUS])

    MPI_ENABLED=1
else
    AC_MSG_NOTICE([Compiling without MPI])
    MPI_ENABLED=0
fi

AC_SUBST(MPI_CFLAGS)
AC_SUBST(MPI_CXXFLAGS)
AC_SUBST(MPI_LIBS)
AC_SUBST(MPI_ENABLED)
])
