#include <Python.h>
#include "database.h"
#include <QDebug>

QT_USE_NAMESPACE_U1DB

static PyObject *Module_hello(PyObject *self, PyObject *args)
{
    return PyString_FromString("world");
}

static void u1dbqt_database_destructor(PyObject* capsule)
{
    Database* db = static_cast<Database*>(PyCapsule_GetPointer(capsule, NULL));
    delete db;
}

static void* u1dbqt_get_ptr(PyObject *self, char* name)
{
    PyObject* capsule = PyObject_GetAttrString(self, "qt_db");
    assert(capsule != 0);
    void* ptr = PyCapsule_GetPointer(capsule, name);
    assert(ptr != 0);
    return ptr;
}

static void u1dbqt_set_ptr (PyObject *self, char* name, void* ptr, PyCapsule_Destructor destr)
{
    // qDebug() << "set_ptr" << PyObject_Str(self) << name;
    PyObject* capsule = PyCapsule_New(ptr, name, destr);
    assert(capsule != 0);
    assert (PyObject_SetAttrString(self, "qt_db", capsule) != -1);
}

static PyObject *Database_init(PyObject *self, PyObject *args, PyObject *kwd)
{
    char *path = ":memory:";
    static char *kwlist[] = {"self", "path", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwd, "Os", kwlist,
                                     &self, &path))
        return NULL;

    Database* db = new Database();
    db->setPath(path);
    u1dbqt_set_ptr(self, "u1dbqt.db", db, u1dbqt_database_destructor);
    // qDebug() << "__init__" << db;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Database_put_doc(PyObject *self, PyObject *args, PyObject* kwd)
{
    char *contents = NULL;
    char *docId = NULL;
    static char *kwlist[] = {"self", "contents", "doc_id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwd, "Ozz", kwlist,
                                     &self, &contents, &docId))
        return NULL;

    Database* db = static_cast<Database*>(u1dbqt_get_ptr(self, "u1dbqt.db"));
    if (db->putDoc(QVariant())) {
        char* lastError = (char*)(db->lastError().data());
        if (strstr (lastError, "Invalid docID"))
            PyErr_SetString(PyErr_NewException("errors.InvalidDocId", NULL, NULL), lastError);
        else
            PyErr_SetString(PyErr_NewException("u1dbqt.QtDatabaseError", NULL, NULL), lastError);
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Database_get_doc(PyObject *self, PyObject *args, PyObject* kwd)
{
    char *docId = NULL;
    static char *kwlist[] = {"self", "doc_id", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwd, "Oz", kwlist,
                                     &self, &docId))
        return NULL;

    Database* db = static_cast<Database*>(u1dbqt_get_ptr(self, "u1dbqt.db"));
    QVariant doc(db->getDoc(QString(docId)));
    /* if (!doc.isValid()) {
        const char* lastError = db->lastError().data();
        PyErr_SetString(PyExc_ValueError, lastError);
        return NULL;
    } */
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Database_repr(PyObject *self, PyObject *args)
{
    return PyString_FromString("u1dbqt.Database");
}

static PyMethodDef ModuleMethods[] =
{
    {"hello", Module_hello, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyMethodDef DatabaseMethods[] =
{
    {"__init__", (PyCFunction)Database_init, METH_VARARGS | METH_KEYWORDS, ""},
    {"put_doc", (PyCFunction)Database_put_doc, METH_VARARGS | METH_KEYWORDS, ""},
    {"get_doc", (PyCFunction)Database_get_doc, METH_VARARGS | METH_KEYWORDS, ""},
    {"__repr__", Database_repr, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initu1dbqt(void)
{
    PyObject *module = Py_InitModule("u1dbqt", ModuleMethods);
    PyObject *moduleDict = PyModule_GetDict(module);

    PyObject *classDict = PyDict_New();
    PyObject *className = PyString_FromString("Database");
    PyObject *classInstance = PyClass_New(NULL, classDict, className);
    PyDict_SetItemString(moduleDict, "Database", classInstance);
    Py_DECREF(classDict);
    Py_DECREF(className);
    Py_DECREF(classInstance);

    PyMethodDef *def;
    for (def = DatabaseMethods; def->ml_name != NULL; def++) {
        PyObject *func = PyCFunction_New(def, NULL);
        PyObject *method = PyMethod_New(func, NULL, classInstance);
        PyDict_SetItemString(classDict, def->ml_name, method);
        Py_DECREF(func);
        Py_DECREF(method);
    }
}

