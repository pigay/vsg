#include <Python.h>
#include <pygobject.h>

/* include any extra headers needed here */
#include <vsg/vsg.h>
#include "vsgconstants.h"
#include "vsgconstantsd.h"
#include "vsgconstantsf.h"

void pyvsg_register_classes (PyObject *d);
extern PyMethodDef pyvsg_functions[];

DL_EXPORT(void)
     initvsg (void)
{
  PyObject *m, *d;

  /* perform any initialisation required by the library here */

  m = Py_InitModule ("vsg", pyvsg_functions);
  d = PyModule_GetDict (m);

  init_pygobject ();

  vsg_init ();

  pyvsg_register_classes (d);

  /* add anything else to the module dictionary (such as constants) */
  vsg_constants_register (m);
  vsg_constantsd_register (m);
  vsg_constantsf_register (m);
}
