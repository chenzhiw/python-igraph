#include <Python.h>
#include "igraph.h"
#include "common.h"
#include "error.h"
#include "convert.h"
#include "graphobject.h"
#include "vertexseqobject.h"
#include "vertexobject.h"
#include "edgeseqobject.h"
#include "edgeobject.h"

/**
 * \defgroup python_interface Python module implementation
 * \brief Functions implementing a Python interface to \a igraph
 * 
 * These functions provide a way to access \a igraph functions from Python.
 * It should be of interest of \a igraph developers only. Classes, functions
 * and methods exposed to Python are still to be documented. Until it is done,
 * just type the following to get help about \a igraph functions in Python
 * (assuming you have \c igraph.so somewhere in your Python library path):
 * 
 * \verbatim
import igraph
help(igraph)
help(igraph.Graph)
\endverbatim
 * 
 * Most of the functions provided here share the same calling conventions
 * (which are determined by the Python/C API). Since the role of the
 * arguments are the same across many functions, I won't explain them
 * everywhere, just give a quick overview of the common argument names here.
 * 
 * \param self the Python igraph.Graph object the method is working on
 * \param args pointer to the Python tuple containing the arguments
 * \param kwds pointer to the Python hash containing the keyword parameters
 * \param type the type object of a Python igraph.Graph object. Used usually
 * in constructors and class methods.
 * 
 * Any arguments not documented here should be mentioned at the documentation
 * of the appropriate method.
 * 
 * The functions which implement a Python method always return a pointer to
 * a \c PyObject. According to Python conventions, this is \c NULL if and
 * only if an exception was thrown by the method (or any of the functions
 * it has called). When I explain the return value of a function which
 * provides interface to an \a igraph function, I won't cover the case of
 * returning a \c NULL value, because this is the same for every such method.
 * The conclusion is that a method can return \c NULL even if I don't state
 * it explicitly.
 * 
 * Also please take into consideration that I'm documenting the C calls
 * with the abovementioned parameters here, and \em not the Python methods
 * which are presented to the user using the Python interface of \a igraph.
 * If you are looking for the documentation of the classes, methods and
 * functions exposed to Python, please use the \c help calls from Python
 * or use \c pydoc to generate a formatted version.
 *
 * \section weakrefs The usage of weak references in the Python interface
 * 
 * Many classes implemented in the Python interface (e.g. VertexSeq, Vertex...)
 * use weak references to keep track of the graph they are referencing to.
 * The use of weak references is twofold:
 * 
 * -# If we assign a VertexSeq or a Vertex of a given graph to a local
 *    variable and then destroy the graph, real references keep the graph
 *    alive and do not return the memory back to Python.
 * -# If we use real references, a Graph object will hold a reference
 *    to its VertexSeq (because we don't want to allocate a new VertexSeq
 *    object for the same graph every time it is requested), and the
 *    VertexSeq will also hold a reference to the Graph. This is a circular
 *    reference. Python does not reclaim the memory occupied by the Graph
 *    back when the Graph is destroyed, because the VertexSeq is holding a
 *    reference to it. Similarly, VertexSeq doesn't get freed because the
 *    Graph is holding a reference to it. These situations can only be
 *    resolved by the Python garbage collector which is invoked at regular
 *    intervals. Unfortunately, the garbage collector refuses to break
 *    circular references and free the objects participating in the circle
 *    when any of the objects has a \c __del__ method. In this case,
 *    \c igraph.Graph has one (which frees the underlying \c igraph_t
 *    graph), therefore our graphs never get freed when we use real
 *    references.
 */

/** \ingroup python_interface
 * \brief Method table for the igraph Python module
 */
static PyMethodDef igraphmodule_methods[] = 
{
   {NULL, NULL, 0, NULL}
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initigraph(void)
{
  PyObject* m;  ///< igraph module object
  
  igraphmodule_VertexSeqType.tp_traverse = (traverseproc)igraphmodule_VertexSeq_traverse;
  igraphmodule_VertexSeqType.tp_clear = (inquiry)igraphmodule_VertexSeq_clear;

  if (PyType_Ready(&igraphmodule_VertexSeqType) < 0) return;
  
  igraphmodule_VertexType.tp_traverse = (traverseproc)igraphmodule_Vertex_traverse;
  igraphmodule_VertexType.tp_clear = (inquiry)igraphmodule_Vertex_clear;

  if (PyType_Ready(&igraphmodule_VertexType) < 0) return;
  
  igraphmodule_EdgeSeqType.tp_traverse = (traverseproc)igraphmodule_EdgeSeq_traverse;
  igraphmodule_EdgeSeqType.tp_clear = (inquiry)igraphmodule_EdgeSeq_clear;

  if (PyType_Ready(&igraphmodule_EdgeSeqType) < 0) return;
  
  igraphmodule_EdgeType.tp_traverse = (traverseproc)igraphmodule_Edge_traverse;
  igraphmodule_EdgeType.tp_clear = (inquiry)igraphmodule_Edge_clear;

  if (PyType_Ready(&igraphmodule_EdgeType) < 0) return;
  
  igraphmodule_GraphType.tp_new = igraphmodule_Graph_new;
  igraphmodule_GraphType.tp_init = (initproc)igraphmodule_Graph_init;
  igraphmodule_GraphType.tp_methods = igraphmodule_Graph_methods;
  igraphmodule_GraphType.tp_getset = igraphmodule_Graph_getseters;
  
  if (PyType_Ready(&igraphmodule_GraphType) < 0) return;
  
  igraphmodule_InternalError =
    PyErr_NewException("igraph.InternalError", NULL, NULL);
  
  Py_INCREF(igraphmodule_InternalError);
  
  m = Py_InitModule3("igraph", igraphmodule_methods,
		     "Python interface for the igraph library");
  
  Py_INCREF(&igraphmodule_GraphType);
  // Maybe the next line is unnecessary?
  // Py_INCREF(&igraphmodule_VertexSeqType);
  
  PyModule_AddObject(m, "Graph", (PyObject*)&igraphmodule_GraphType);
  // Maybe the next line is unnecessary?
  // PyModule_AddObject(m, "VertexSeq", (PyObject*)&igraphmodule_VertexSeqType);
  
  PyModule_AddObject(m, "InternalError", igraphmodule_InternalError);
  PyModule_AddIntConstant(m, "OUT", IGRAPH_OUT);
  PyModule_AddIntConstant(m, "IN", IGRAPH_IN);
  PyModule_AddIntConstant(m, "ALL", IGRAPH_ALL);
  PyModule_AddIntConstant(m, "STAR_OUT", IGRAPH_STAR_OUT);
  PyModule_AddIntConstant(m, "STAR_IN", IGRAPH_STAR_IN);
  PyModule_AddIntConstant(m, "STAR_UNDIRECTED", IGRAPH_STAR_UNDIRECTED);
  PyModule_AddIntConstant(m, "TREE_OUT", IGRAPH_TREE_OUT);
  PyModule_AddIntConstant(m, "TREE_IN", IGRAPH_TREE_IN);
  PyModule_AddIntConstant(m, "TREE_UNDIRECTED", IGRAPH_TREE_UNDIRECTED);
  PyModule_AddIntConstant(m, "STRONG", IGRAPH_STRONG);
  PyModule_AddIntConstant(m, "WEAK", IGRAPH_WEAK);
  PyModule_AddIntConstant(m, "GET_ADJACENCY_UPPER", IGRAPH_GET_ADJACENCY_UPPER);
  PyModule_AddIntConstant(m, "GET_ADJACENCY_LOWER", IGRAPH_GET_ADJACENCY_LOWER);
  PyModule_AddIntConstant(m, "GET_ADJACENCY_BOTH", IGRAPH_GET_ADJACENCY_BOTH);
  
  /// initialize error handler
  igraph_set_error_handler(igraphmodule_igraph_error_hook);
}