/****************************************************************************
** Meta object code from reading C++ file 'downloadcontrol.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../downloadcontrol.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'downloadcontrol.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DownloadControl_t {
    QByteArrayData data[11];
    char stringdata0[115];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DownloadControl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DownloadControl_t qt_meta_stringdata_DownloadControl = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DownloadControl"
QT_MOC_LITERAL(1, 16, 20), // "FileDownloadFinished"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 8), // "saveFile"
QT_MOC_LITERAL(4, 47, 7), // "getPair"
QT_MOC_LITERAL(5, 55, 12), // "pair_2int64&"
QT_MOC_LITERAL(6, 68, 4), // "pair"
QT_MOC_LITERAL(7, 73, 1), // "i"
QT_MOC_LITERAL(8, 75, 15), // "SubPartFinished"
QT_MOC_LITERAL(9, 91, 8), // "NetSpeed"
QT_MOC_LITERAL(10, 100, 14) // "PrintThreadEnd"

    },
    "DownloadControl\0FileDownloadFinished\0"
    "\0saveFile\0getPair\0pair_2int64&\0pair\0"
    "i\0SubPartFinished\0NetSpeed\0PrintThreadEnd"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DownloadControl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    2,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   47,    2, 0x08 /* Private */,
       9,    0,   48,    2, 0x08 /* Private */,
      10,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5, QMetaType::Int,    6,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DownloadControl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DownloadControl *_t = static_cast<DownloadControl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->FileDownloadFinished((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->getPair((*reinterpret_cast< pair_2int64(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->SubPartFinished(); break;
        case 3: _t->NetSpeed(); break;
        case 4: _t->PrintThreadEnd(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DownloadControl::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DownloadControl::FileDownloadFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (DownloadControl::*_t)(pair_2int64 & , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DownloadControl::getPair)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject DownloadControl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DownloadControl.data,
      qt_meta_data_DownloadControl,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DownloadControl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DownloadControl::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DownloadControl.stringdata0))
        return static_cast<void*>(const_cast< DownloadControl*>(this));
    return QObject::qt_metacast(_clname);
}

int DownloadControl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DownloadControl::FileDownloadFinished(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DownloadControl::getPair(pair_2int64 & _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
